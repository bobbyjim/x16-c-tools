#ifndef ADSR_H
#define ADSR_H

#define  ADSR_ON  					1
#define  ADSR_OFF  					0

#define  ADSR_INSTALLER             0x0460
#define  ADSR_ACTIVATE_VOICE        0x0463
#define  ADSR_VOICE7_TEST           0x0465
#define  ADSR_HANDLER_OFF           0x0468
#define  ADSR_HANDLER_ON            0x046a
#define  ADSR_SET_FREQUENCY         0x046c

#define  ADSR_SET_STATE_IDLE(v)		ADSR_STATE[v] = 0
#define  ADSR_SET_STATE_ATTACK(v)	ADSR_STATE[v] = 2
#define  ADSR_SET_STATE_DECAY(v)	ADSR_STATE[v] = 4
#define  ADSR_SET_STATE_SUSTAIN(v)	ADSR_STATE[v] = 6
#define  ADSR_SET_STATE_RELEASE(v)	ADSR_STATE[v] = 8
#define  ADSR_SET_VOLUME(v,a)		ADSR_VOLUME[v] = a

#define  ADSR_STATE                    ((unsigned char*) (0x400))
#define  ADSR_VOLUME                   ((unsigned char*) (0x408))
#define  ADSR_VOLUME_FRACTIONAL        ((unsigned char*) (0x410))
#define  ADSR_ATTACK                   ((unsigned char*) (0x418))
#define  ADSR_ATTACK_FRACTIONAL        ((unsigned char*) (0x420))
#define  ADSR_DECAY                    ((unsigned char*) (0x428))
#define  ADSR_DECAY_FRACTIONAL         ((unsigned char*) (0x430))
#define  ADSR_SUSTAIN_LEVEL            ((unsigned char*) (0x438))
#define  ADSR_SUSTAIN_TIMER            ((unsigned char*) (0x440))
#define  ADSR_SUSTAIN_TIMER_FRACTIONAL ((unsigned char*) (0x448))
#define  ADSR_RELEASE                  ((unsigned char*) (0x450))
#define  ADSR_RELEASE_FRACTIONAL       ((unsigned char*) (0x458))

void adsr_setFrequency(unsigned char voice, unsigned int freq);
void adsr_setAttack(unsigned char voice, unsigned char level);
void adsr_setDecay(unsigned char voice, unsigned char level, unsigned char sustain_level);
void adsr_setSustain(unsigned char voice, unsigned char level);
void adsr_setRelease(unsigned char voice, unsigned char level);

void adsr_activateVoice(unsigned char voice, unsigned char volume);
void adsr_releaseVoice(unsigned char voice);
void adsr_setHandler(unsigned char on);
void adsr_install();

#endif
