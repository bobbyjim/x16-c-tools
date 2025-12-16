#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <conio.h>
#include <cx16.h>

#include "ADSR.h"
#include "timer.h"

#define MIDDLE_C 702

// Test waveforms
#define PSG_WAVE_PULSE      63
#define PSG_WAVE_SAWTOOTH   (1 << 6)
#define PSG_WAVE_TRIANGLE   (2 << 6)
#define PSG_WAVE_NOISE      (3 << 6)

// Test state
unsigned char g_test_count = 0;
unsigned char g_test_passed = 0;
unsigned char g_test_failed = 0;

// Helper: Set up a voice with frequency and waveform
void setup_voice(unsigned char voice, unsigned int freq, unsigned char waveform)
{
   VERA.control    = 0;
   VERA.address    = 0xf9c0 + (voice * 4);
   VERA.address_hi = VERA_INC_1 + 1;
   VERA.data0      = freq & 0xff;
   VERA.data0      = freq >> 8;
   VERA.data0      = 0xC0;  // L/R, vol will be controlled by ADSR
   VERA.data0      = waveform;
}

// Helper: Print test header
void test_start(const char* name, const char* waveform)
{
   g_test_count++;
   printf("\r\n[TEST %u] %s (%s)...\r\n", g_test_count, name, waveform);
}

// Helper: Check if value is in expected range
unsigned char check_range(unsigned char value, unsigned char min, unsigned char max, const char* label)
{
   if (value >= min && value <= max) {
      printf("  %s: %u [%u-%u] OK\r\n", label, value, min, max);
      return 1;
   } else {
      printf("  %s: %u [expected %u-%u] FAIL\r\n", label, value, min, max);
      return 0;
   }
}

// Helper: Check exact value
unsigned char check_value(unsigned char value, unsigned char expected, const char* label)
{
   if (value == expected) {
      printf("  %s: %u OK\r\n", label, value);
      return 1;
   } else {
      printf("  %s: %u [expected %u] FAIL\r\n", label, value, expected);
      return 0;
   }
}

// Helper: Report test result
void test_result(unsigned char passed)
{
   if (passed) {
      printf("  RESULT: PASS\r\n");
      g_test_passed++;
   } else {
      printf("  RESULT: FAIL\r\n");
      g_test_failed++;
   }
}

// TEST 1: Fast Attack - volume should ramp up quickly
void test_fast_attack()
{
   unsigned char voice = 0;
   unsigned char passed = 1;
   
   test_start("Fast Attack", "Pulse");
   
   setup_voice(voice, MIDDLE_C, PSG_WAVE_PULSE);
   
   // Configure: fast attack (4), no decay, no sustain timer, slow release
   ADSR_ATTACK[voice] = 4;
   ADSR_ATTACK_FRACTIONAL[voice] = 0;
   ADSR_DECAY[voice] = 0;
   ADSR_DECAY_FRACTIONAL[voice] = 0;
   ADSR_SUSTAIN_LEVEL[voice] = 63;
   ADSR_SUSTAIN_TIMER[voice] = 0;
   ADSR_SUSTAIN_TIMER_FRACTIONAL[voice] = 0;
   ADSR_RELEASE[voice] = 0;
   ADSR_RELEASE_FRACTIONAL[voice] = 50;
   
   // Activate voice
   adsr_activateVoice(voice, 63);
   passed &= check_value(ADSR_STATE[voice], 2, "Initial state (Attack)");
   
   // Wait a bit and check volume is increasing
   pause_jiffies(10);
   passed &= check_range(ADSR_VOLUME[voice], 20, 63, "Volume after 10 jiffies");
   
   // Wait more, should reach max and enter decay
   pause_jiffies(20);
   passed &= check_value(ADSR_VOLUME[voice], 63, "Volume at peak");
   passed &= check_value(ADSR_STATE[voice], 4, "State after peak (Decay)");
   
   // Should quickly hit sustain since decay=0
   pause_jiffies(5);
   passed &= check_value(ADSR_STATE[voice], 6, "State after decay (Sustain)");
   
   // Release voice
   adsr_releaseVoice(voice);
   passed &= check_value(ADSR_STATE[voice], 8, "State after release");
   
   // Wait and check volume decreasing
   pause_jiffies(20);
   passed &= check_range(ADSR_VOLUME[voice], 1, 55, "Volume during release");
   
   // Wait for voice to go silent
   pause_jiffies(50);
   passed &= check_value(ADSR_VOLUME[voice], 0, "Final volume");
   passed &= check_value(ADSR_STATE[voice], 0, "Final state (Idle)");
   
   test_result(passed);
}

// TEST 2: Slow Decay - volume should gradually decrease to sustain level
void test_slow_decay()
{
   unsigned char voice = 1;
   unsigned char passed = 1;
   
   test_start("Slow Decay", "Sawtooth");
   
   setup_voice(voice, MIDDLE_C, PSG_WAVE_SAWTOOTH);
   
   // Configure: fast attack, slow decay to level 40
   ADSR_ATTACK[voice] = 4;
   ADSR_ATTACK_FRACTIONAL[voice] = 0;
   ADSR_DECAY[voice] = 0;
   ADSR_DECAY_FRACTIONAL[voice] = 50;
   ADSR_SUSTAIN_LEVEL[voice] = 40;
   ADSR_SUSTAIN_TIMER[voice] = 0;
   ADSR_SUSTAIN_TIMER_FRACTIONAL[voice] = 0;
   ADSR_RELEASE[voice] = 0;
   ADSR_RELEASE_FRACTIONAL[voice] = 50;
   
   adsr_activateVoice(voice, 63);
   
   // Should quickly hit peak
   pause_jiffies(20);
   passed &= check_value(ADSR_VOLUME[voice], 63, "Volume at peak");
   passed &= check_value(ADSR_STATE[voice], 4, "State (Decay)");
   
   // Check decay in progress
   pause_jiffies(20);
   passed &= check_range(ADSR_VOLUME[voice], 45, 62, "Volume during decay");
   passed &= check_value(ADSR_STATE[voice], 4, "Still in Decay");
   
   // Wait longer, should reach sustain level
   pause_jiffies(60);
   passed &= check_value(ADSR_VOLUME[voice], 40, "Volume at sustain level");
   passed &= check_value(ADSR_STATE[voice], 6, "State (Sustain)");
   
   // Release
   adsr_releaseVoice(voice);
   pause_jiffies(5);
   passed &= check_value(ADSR_STATE[voice], 8, "State (Release)");
   
   // Wait for voice to go silent
   pause_jiffies(50);
   passed &= check_value(ADSR_VOLUME[voice], 0, "Final volume");
   passed &= check_value(ADSR_STATE[voice], 0, "Final state (Idle)");
   
   test_result(passed);
}

// TEST 3: Sustain Hold - should stay at sustain level until timer expires
void test_sustain_hold()
{
   unsigned char voice = 2;
   unsigned char passed = 1;
   
   test_start("Sustain Hold", "Triangle");
   
   setup_voice(voice, MIDDLE_C, PSG_WAVE_TRIANGLE);
   
   // Configure: fast attack, no decay, sustain at 50 for ~30 jiffies, then release
   ADSR_ATTACK[voice] = 4;
   ADSR_ATTACK_FRACTIONAL[voice] = 0;
   ADSR_DECAY[voice] = 0;
   ADSR_DECAY_FRACTIONAL[voice] = 0;
   ADSR_SUSTAIN_LEVEL[voice] = 50;
   ADSR_SUSTAIN_TIMER[voice] = 30;
   ADSR_SUSTAIN_TIMER_FRACTIONAL[voice] = 0;
   ADSR_RELEASE[voice] = 0;
   ADSR_RELEASE_FRACTIONAL[voice] = 50;
   
   adsr_activateVoice(voice, 63);
   
   // Should hit sustain quickly
   pause_jiffies(20);
   passed &= check_value(ADSR_STATE[voice], 6, "State (Sustain)");
   passed &= check_value(ADSR_VOLUME[voice], 50, "Volume at sustain");
   
   // Wait 15 jiffies, should still be sustaining
   pause_jiffies(15);
   passed &= check_value(ADSR_STATE[voice], 6, "Still Sustaining");
   passed &= check_value(ADSR_VOLUME[voice], 50, "Volume unchanged");
   
   // Wait another 20 jiffies, timer should expire and enter release
   pause_jiffies(20);
   passed &= check_value(ADSR_STATE[voice], 8, "State (Auto-Release)");
   
   // Check volume decreasing
   pause_jiffies(10);
   passed &= check_range(ADSR_VOLUME[voice], 1, 45, "Volume during release");
   
   // Wait for voice to go silent
   pause_jiffies(50);
   passed &= check_value(ADSR_VOLUME[voice], 0, "Final volume");
   passed &= check_value(ADSR_STATE[voice], 0, "Final state (Idle)");
   
   test_result(passed);
}

// TEST 4: Quick Release - volume should drop to 0 quickly
void test_quick_release()
{
   unsigned char voice = 3;
   unsigned char passed = 1;
   
   test_start("Quick Release", "Pulse");
   
   setup_voice(voice, MIDDLE_C, PSG_WAVE_PULSE);
   
   // Configure: fast attack, fast release
   ADSR_ATTACK[voice] = 4;
   ADSR_ATTACK_FRACTIONAL[voice] = 0;
   ADSR_DECAY[voice] = 0;
   ADSR_DECAY_FRACTIONAL[voice] = 0;
   ADSR_SUSTAIN_LEVEL[voice] = 63;
   ADSR_SUSTAIN_TIMER[voice] = 0;
   ADSR_SUSTAIN_TIMER_FRACTIONAL[voice] = 0;
   ADSR_RELEASE[voice] = 4;
   ADSR_RELEASE_FRACTIONAL[voice] = 0;
   
   adsr_activateVoice(voice, 63);
   
   // Reach sustain
   pause_jiffies(20);
   passed &= check_value(ADSR_STATE[voice], 6, "State (Sustain)");
   
   // Release
   adsr_releaseVoice(voice);
   passed &= check_value(ADSR_STATE[voice], 8, "State (Release)");
   
   // Wait a bit, volume should drop quickly
   pause_jiffies(10);
   passed &= check_range(ADSR_VOLUME[voice], 20, 55, "Volume after quick release");
   
   // Wait more, should be silent and idle
   pause_jiffies(20);
   passed &= check_value(ADSR_VOLUME[voice], 0, "Volume at 0");
   passed &= check_value(ADSR_STATE[voice], 0, "State (Idle)");
   
   test_result(passed);
}

// TEST 5: Full Cycle - complete ADSR cycle
void test_full_cycle()
{
   unsigned char voice = 4;
   unsigned char passed = 1;
   
   test_start("Full Cycle", "Sawtooth");
   
   setup_voice(voice, MIDDLE_C, PSG_WAVE_SAWTOOTH);
   
   // Configure: moderate attack, decay, sustain, release
   ADSR_ATTACK[voice] = 2;
   ADSR_ATTACK_FRACTIONAL[voice] = 0;
   ADSR_DECAY[voice] = 0;
   ADSR_DECAY_FRACTIONAL[voice] = 50;
   ADSR_SUSTAIN_LEVEL[voice] = 35;
   ADSR_SUSTAIN_TIMER[voice] = 15;
   ADSR_SUSTAIN_TIMER_FRACTIONAL[voice] = 0;
   ADSR_RELEASE[voice] = 0;
   ADSR_RELEASE_FRACTIONAL[voice] = 80;
   
   adsr_activateVoice(voice, 63);
   passed &= check_value(ADSR_STATE[voice], 2, "State (Attack)");
   
   // Attack phase
   pause_jiffies(15);
   passed &= check_value(ADSR_STATE[voice], 4, "State (Decay)");
   
   // Decay phase
   pause_jiffies(20);
   passed &= check_value(ADSR_STATE[voice], 6, "State (Sustain)");
   passed &= check_value(ADSR_VOLUME[voice], 35, "Volume at sustain");
   
   // Sustain phase
   pause_jiffies(10);
   passed &= check_value(ADSR_STATE[voice], 6, "Still Sustaining");
   
   // Wait for auto-release
   pause_jiffies(10);
   passed &= check_value(ADSR_STATE[voice], 8, "State (Auto-Release)");
   
   // Release phase
   pause_jiffies(20);
   passed &= check_range(ADSR_VOLUME[voice], 0, 20, "Volume during release");
   
   // Complete
   pause_jiffies(30);
   passed &= check_value(ADSR_VOLUME[voice], 0, "Final volume");
   passed &= check_value(ADSR_STATE[voice], 0, "Final state (Idle)");
   
   test_result(passed);
}

// TEST 6: Retrigger During Attack - new attack should restart
void test_retrigger_attack()
{
   unsigned char voice = 5;
   unsigned char passed = 1;
   unsigned char vol1, vol2;
   
   test_start("Retrigger During Attack", "Triangle");
   
   setup_voice(voice, MIDDLE_C, PSG_WAVE_TRIANGLE);
   
   // Configure: slow attack
   ADSR_ATTACK[voice] = 1;
   ADSR_ATTACK_FRACTIONAL[voice] = 0;
   ADSR_DECAY[voice] = 0;
   ADSR_DECAY_FRACTIONAL[voice] = 0;
   ADSR_SUSTAIN_LEVEL[voice] = 63;
   ADSR_SUSTAIN_TIMER[voice] = 0;
   ADSR_SUSTAIN_TIMER_FRACTIONAL[voice] = 0;
   ADSR_RELEASE[voice] = 0;
   ADSR_RELEASE_FRACTIONAL[voice] = 50;
   
   adsr_activateVoice(voice, 63);
   
   // Let it attack partway
   pause_jiffies(15);
   passed &= check_value(ADSR_STATE[voice], 2, "State (Attack)");
   vol1 = ADSR_VOLUME[voice];
   passed &= check_range(vol1, 10, 45, "Partial attack volume");
   
   // Retrigger
   adsr_activateVoice(voice, 63);
   passed &= check_value(ADSR_STATE[voice], 2, "State still Attack");
   vol2 = ADSR_VOLUME[voice];
   
   // Volume should have increased or stayed similar (not dropped)
   if (vol2 >= vol1 - 5) {
      printf("  Retrigger preserved volume: %u->%u OK\r\n", vol1, vol2);
   } else {
      printf("  Retrigger dropped volume: %u->%u FAIL\r\n", vol1, vol2);
      passed = 0;
   }
   
   // Should continue attacking
   pause_jiffies(25);
   passed &= check_value(ADSR_VOLUME[voice], 63, "Reached peak after retrigger");
      // Clean up - release and wait for silence
   adsr_releaseVoice(voice);
   pause_jiffies(50);
   passed &= check_value(ADSR_VOLUME[voice], 0, "Final volume");
   passed &= check_value(ADSR_STATE[voice], 0, "Final state (Idle)");
      test_result(passed);
}

// TEST 7: Release During Decay - manual release should work
void test_release_during_decay()
{
   unsigned char voice = 6;
   unsigned char passed = 1;
   
   test_start("Release During Decay", "Noise");
   
   setup_voice(voice, MIDDLE_C, PSG_WAVE_NOISE);
   
   // Configure: fast attack, slow decay
   ADSR_ATTACK[voice] = 8;
   ADSR_ATTACK_FRACTIONAL[voice] = 0;
   ADSR_DECAY[voice] = 0;
   ADSR_DECAY_FRACTIONAL[voice] = 80;
   ADSR_SUSTAIN_LEVEL[voice] = 20;
   ADSR_SUSTAIN_TIMER[voice] = 0;
   ADSR_SUSTAIN_TIMER_FRACTIONAL[voice] = 0;
   ADSR_RELEASE[voice] = 1;
   ADSR_RELEASE_FRACTIONAL[voice] = 0;
   
   adsr_activateVoice(voice, 63);
   
   // Wait for decay phase
   pause_jiffies(12);
   passed &= check_value(ADSR_STATE[voice], 4, "State (Decay)");
   passed &= check_range(ADSR_VOLUME[voice], 30, 62, "Volume during decay");
   
   // Manually release during decay
   adsr_releaseVoice(voice);
   passed &= check_value(ADSR_STATE[voice], 8, "State (Release)");
   
   // Should continue to 0
   pause_jiffies(25);
   passed &= check_range(ADSR_VOLUME[voice], 0, 10, "Volume after release");
      // Wait for complete silence
   pause_jiffies(30);
   passed &= check_value(ADSR_VOLUME[voice], 0, "Final volume");
   passed &= check_value(ADSR_STATE[voice], 0, "Final state (Idle)");
      test_result(passed);
}

// TEST 8: Zero Attack - should jump to full volume immediately
void test_zero_attack()
{
   unsigned char voice = 7;
   unsigned char passed = 1;
   
   test_start("Zero Attack (Instant)", "Pulse");
   
   setup_voice(voice, MIDDLE_C, PSG_WAVE_PULSE);
   
   // Configure: zero attack
   ADSR_ATTACK[voice] = 0;
   ADSR_ATTACK_FRACTIONAL[voice] = 0;
   ADSR_DECAY[voice] = 0;
   ADSR_DECAY_FRACTIONAL[voice] = 50;
   ADSR_SUSTAIN_LEVEL[voice] = 50;
   ADSR_SUSTAIN_TIMER[voice] = 0;
   ADSR_SUSTAIN_TIMER_FRACTIONAL[voice] = 0;
   ADSR_RELEASE[voice] = 0;
   ADSR_RELEASE_FRACTIONAL[voice] = 50;
   
   adsr_activateVoice(voice, 63);
   
   // Should immediately jump to decay
   pause_jiffies(3);
   passed &= check_value(ADSR_STATE[voice], 4, "State (Decay) - skipped attack");
   passed &= check_value(ADSR_VOLUME[voice], 63, "Volume at peak immediately");
   
   // Should decay
   pause_jiffies(15);
   passed &= check_range(ADSR_VOLUME[voice], 50, 62, "Volume during decay");
   
   // Clean up - release and wait for silence
   adsr_releaseVoice(voice);
   pause_jiffies(50);
   passed &= check_value(ADSR_VOLUME[voice], 0, "Final volume");
   passed &= check_value(ADSR_STATE[voice], 0, "Final state (Idle)");
   
   test_result(passed);
}

void main()
{
   printf("\r\n=== ADSR Test Suite ===\r\n");
   printf("Loading ADSR library...\r\n");
   
   adsr_install();
   adsr_setHandler(ADSR_ON);
   
   printf("Starting tests...\r\n");
   
   // Run all tests
   test_fast_attack();
   test_slow_decay();
   test_sustain_hold();
   test_quick_release();
   test_full_cycle();
   test_retrigger_attack();
   test_release_during_decay();
   test_zero_attack();
   
   // Summary
   printf("\r\n=== SUMMARY ===\r\n");
   printf("Total Tests: %u\r\n", g_test_count);
   printf("Passed: %u\r\n", g_test_passed);
   printf("Failed: %u\r\n", g_test_failed);
   
   if (g_test_failed == 0) {
      printf("\r\nALL TESTS PASSED!\r\n");
   } else {
      printf("\r\nSOME TESTS FAILED!\r\n");
   }
   
   printf("\r\nPress any key to exit...\r\n");
   cgetc();
}
