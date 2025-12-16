# Commander X16 ADSR Envelope Library

A high-performance ADSR (Attack, Decay, Sustain, Release) envelope generator for the Commander X16's PSG audio chip. Runs in IRQ handler for smooth, automatic volume control across the first 8 PSG voices.

## Features

- Full ADSR envelope control for voices 0-7
- IRQ-driven for smooth envelopes independent of main program timing
- 16-bit fractional arithmetic for precise, smooth volume transitions
- Independent envelope settings per voice
- Low memory footprint (546 bytes at $0400)
- Clean C API for easy integration

## Memory Map

| Address Range | Contents |
|--------------|----------|
| $0400-$045F | Envelope state data (8 voices × 12 bytes) |
| $0460-$046C | Jump table for API entry points |
| $046D-$04FF | Implementation code and variables |

**Important**: This library installs at $0400 and takes over the IRQ vector. It chains to the default IRQ handler after processing envelopes.

## Installation

### From C:

```c
#include "ADSR.h"

void main() {
    adsr_install();          // Load and install the library
    adsr_setHandler(ADSR_ON); // Enable envelope processing
    
    // Your code here...
}
```

### Files Required:

- `adsr1.prg` - The compiled assembly library (must be on disk 8)
- `ADSR.h` - C header file
- `ADSR.c` - C wrapper functions
- `timer.h` / `timer.c` - Timing utilities

## API Reference

### Initialization

#### `void adsr_install()`
Loads the ADSR library from disk and installs it at $0400. Also backs up and replaces the IRQ vector.

**Call once at program startup.**

#### `void adsr_setHandler(unsigned char on)`
Enables or disables envelope processing in the IRQ handler.

**Parameters:**
- `on` - `ADSR_ON` (1) to enable, `ADSR_OFF` (0) to disable

**Note:** Disabling stops envelope updates but doesn't silence voices or reset state.

---

### Envelope Configuration

Configure envelope parameters for each voice. All rates use a 16-bit fractional format internally, but the API accepts simple 8-bit values for convenience.

#### `void adsr_setAttack(unsigned char voice, unsigned char level)`
Sets the attack rate (how fast volume ramps up from 0→63).

**Parameters:**
- `voice` - Voice number (0-7)
- `level` - Attack rate (0=instant, higher=faster)

**Typical values:** 0-8

#### `void adsr_setDecay(unsigned char voice, unsigned char level, unsigned char sustain_level)`
Sets the decay rate and sustain level (volume decays from peak down to sustain level).

**Parameters:**
- `voice` - Voice number (0-7)  
- `level` - Decay rate (0=instant, higher=faster)
- `sustain_level` - Target volume (0-63)

**Typical values:** 
- `level`: 20-100
- `sustain_level`: 30-50

#### `void adsr_setSustain(unsigned char voice, unsigned char level)`
Sets the sustain timer (how long to hold at sustain level before auto-releasing).

**Parameters:**
- `voice` - Voice number (0-7)
- `level` - Timer duration in jiffies (0=infinite, wait for manual release)

**Typical values:** 0 (infinite), or 20-60 for timed notes

#### `void adsr_setRelease(unsigned char voice, unsigned char level)`
Sets the release rate (how fast volume fades to 0 after note-off).

**Parameters:**
- `voice` - Voice number (0-7)
- `level` - Release rate (0=instant, higher=faster)

**Typical values:** 30-100

---

### Playback Control

#### `void adsr_activateVoice(unsigned char voice, unsigned char volume)`
Triggers the attack phase for a voice. Sets initial volume and begins the ADSR cycle.

**Parameters:**
- `voice` - Voice number (0-7)
- `volume` - Target peak volume (0-63, typically use 63)

**Note:** Set frequency via VERA before calling this.

#### `void adsr_releaseVoice(unsigned char voice)`
Manually triggers the release phase (note-off). Volume will decay to 0 according to release rate.

**Parameters:**
- `voice` - Voice number (0-7)

---

## ADSR State Machine

Each voice progresses through these states:

```
0: IDLE    - Silent, no processing
2: ATTACK  - Volume ramping up to peak (63)
4: DECAY   - Volume decaying to sustain level
6: SUSTAIN - Holding at sustain level
8: RELEASE - Fading to silence
   → IDLE
```

States are even numbers (2, 4, 6, 8) to index into a jump table efficiently.

## Direct Memory Access

For advanced usage, you can read/write envelope data directly:

```c
ADSR_STATE[voice]                    // Current state (0,2,4,6,8)
ADSR_VOLUME[voice]                   // Current volume (0-63)
ADSR_VOLUME_FRACTIONAL[voice]        // Volume fractional part
ADSR_ATTACK[voice]                   // Attack rate (high byte)
ADSR_ATTACK_FRACTIONAL[voice]        // Attack rate (low byte)
ADSR_DECAY[voice]                    // Decay rate (high byte)
ADSR_DECAY_FRACTIONAL[voice]         // Decay rate (low byte)
ADSR_SUSTAIN_LEVEL[voice]            // Sustain level (0-63)
ADSR_SUSTAIN_TIMER[voice]            // Sustain timer (high byte)
ADSR_SUSTAIN_TIMER_FRACTIONAL[voice] // Sustain timer (low byte)
ADSR_RELEASE[voice]                  // Release rate (high byte)
ADSR_RELEASE_FRACTIONAL[voice]       // Release rate (low byte)
```

## Usage Examples

### Basic Note Playback

```c
// Configure voice 0 for a piano-like sound
adsr_setAttack(0, 2);        // Fast attack
adsr_setDecay(0, 30, 40);    // Decay to level 40
adsr_setSustain(0, 0);       // Hold indefinitely
adsr_setRelease(0, 50);      // Medium release

// Set frequency via VERA
VERA.control    = 0;
VERA.address    = 0xf9c0;    // Voice 0 base
VERA.address_hi = VERA_INC_1 + 1;
VERA.data0      = freq & 0xff;
VERA.data0      = freq >> 8;
VERA.data0      = 0xC0;      // L/R channels (volume controlled by ADSR)
VERA.data0      = 0x3F;      // Pulse wave

// Play note
adsr_activateVoice(0, 63);

// Later... stop note
adsr_releaseVoice(0);
```

### Music Playback

```c
unsigned char prev_note = 255;

for (;;) {
    unsigned char note = song_data[index++];
    
    if (note != prev_note) {
        if (note == 0) {
            // Rest - release voice
            adsr_releaseVoice(0);
        } else {
            // New note - set frequency and trigger
            VERA.control    = 0;
            VERA.address    = 0xf9c0;
            VERA.address_hi = VERA_INC_1 + 1;
            VERA.data0      = frequency_table[note] & 0xff;
            VERA.data0      = frequency_table[note] >> 8;
            adsr_activateVoice(0, 63);
        }
        prev_note = note;
    }
    
    pause_jiffies(30); // Note duration
}
```

### Sound Effects

```c
// Explosion (noise with fast attack/release)
adsr_setAttack(2, 8);
adsr_setDecay(2, 0, 63);
adsr_setSustain(2, 0);
adsr_setRelease(2, 100);

VERA.control    = 0;
VERA.address    = 0xf9c8;    // Voice 2
VERA.address_hi = VERA_INC_1 + 1;
VERA.data0      = 0;
VERA.data0      = 0;
VERA.data0      = 0xC0;
VERA.data0      = 0xC0;      // Noise waveform

adsr_activateVoice(2, 63);
```

## Frequency Reference

Middle C (C4) = 702 Hz  
VERA frequency = (sample_rate × note_freq) / 25MHz  
For 48kHz: freq_value = note_freq_hz × 1920 / 1000

## Technical Notes

### IRQ Timing
The library processes all 8 voices every IRQ (60Hz NTSC / 50Hz PAL). Each voice takes ~50-80 cycles depending on state.

### Carry Flag Fix
Version 2024-12-15 fixed critical carry flag bugs in the state machine. Always use `clc` before `adc` and `sec` before `sbc`.

### Volume Ranges
- Attack stops at volume ≥ 62 (rounds to 63)
- Release stops at volume < 4 (rounds to 0)
- Sustain level is 6-bit (0-63)

### Testing
Run `TEST-ADSR` program for comprehensive envelope verification. Tests cover:
- Fast/slow attack
- Decay to sustain
- Sustain hold and timer
- Quick/slow release  
- State transitions
- Retriggering

## Troubleshooting

**No sound:**
- Verify `adsr_install()` was called
- Check `adsr_setHandler(ADSR_ON)` was called
- Ensure frequency is set via VERA before `adsr_activateVoice()`
- Check VERA waveform register (byte 3) is not 0

**Clicks/pops:**
- Increase release rate for smoother fadeout
- Check for proper note-off (call `adsr_releaseVoice()` before new note)

**Wrong tempo:**
- IRQ runs at 60Hz, so `pause_jiffies(30)` = 0.5 seconds
- Adjust note duration to taste

**Volume too quiet:**
- ADSR controls the L/R volume bits (top 2 bits + 6-bit volume)
- Make sure VERA register byte 2 has bits 6-7 set (L/R channels)

## License

See LICENSE file in project root.

## Version History

- **2024-12-15**: Fixed carry flag handling in state machine (critical bug fix)
- **2024-12-15**: Added comprehensive test suite
- **2024-12-15**: Improved C API and documentation
