#include "MKL25Z4.h"                    // Device header
#include "music.h"
#include <math.h> 
#include "cmsis_os2.h"

#define TO_MOD(x) 375000/(x)
#define MASK(x) (1 << (x))
extern int start_challenge;
extern int end_challenge;

int nokiaMelody[] = {
	NOTE_E5, 8, NOTE_D5, 8, NOTE_FS4, 4, NOTE_GS4, 4, 
  NOTE_CS5, 8, NOTE_B4, 8, NOTE_D4, 4, NOTE_E4, 4, 
  NOTE_B4, 8, NOTE_A4, 8, NOTE_CS4, 4, NOTE_E4, 4,
  NOTE_A4, 2, 
};

int happyBdayMelody[] = {
	NOTE_C4,4, NOTE_C4,8, 
  NOTE_D4,-4, NOTE_C4,-4, NOTE_F4,-4,
  NOTE_E4,-2, NOTE_C4,4, NOTE_C4,8, 
  NOTE_D4,-4, NOTE_C4,-4, NOTE_G4,-4,
  NOTE_F4,-2, NOTE_C4,4, NOTE_C4,8,

  NOTE_C5,-4, NOTE_A4,-4, NOTE_F4,-4, 
  NOTE_E4,-4, NOTE_D4,-4, NOTE_AS4,4, NOTE_AS4,8,
  NOTE_A4,-4, NOTE_F4,-4, NOTE_G4,-4,
  NOTE_F4,-2,
};

void initMusic(void)
{
	SIM->SCGC5 |= (SIM_SCGC5_PORTC_MASK);
	// PTB3 ALT 3 for TPM2 CH1
	PORTB->PCR[PTB3] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[PTB3] |= PORT_PCR_MUX(3);
	PTB->PDDR |= MASK(3);
	
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	SIM->SCGC6 |= SIM_SCGC6_TPM2_MASK;
	SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
	 
	TPM2->MOD = 7500;
	
	TPM2->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	TPM2->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
	TPM2->SC &= ~(TPM_SC_CPWMS_MASK);
	
	TPM2_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_ELSB_MASK));
	TPM2_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
}

void musicShutUp(void){
	start_challenge = 0;
	end_challenge = 0;
}

void playMusic(void) {

  while(1) {
				while(start_challenge) {
							int tempo = 140;
							int notes = sizeof(happyBdayMelody) / sizeof(happyBdayMelody[0]);

							int wholenote = (60000 * 4) / tempo;
							int divider = 0;
							int noteDuration = 0;
							uint32_t period;

							for(int i = 0; i<notes; i+=2) {

									if(start_challenge == 0){break;}
									
									divider = happyBdayMelody[i + 1];
									if (divider > 0) {
									noteDuration = (wholenote) / divider;
									} else if (divider < 0) {
									noteDuration = (wholenote) / (int)fabs((float)divider);
									noteDuration *= 1.5;
									}
									period = TO_MOD(happyBdayMelody[i]);
									TPM2->MOD = period;
									TPM2_C1V = period / 8; //12.5% duty cycle
									osDelay( noteDuration * 2/3);
							}
				}

				while(end_challenge){
								int tempo = 94;
								int notes = sizeof(nokiaMelody) / sizeof(nokiaMelody[0]);

								int wholenote = (60000 * 4) / tempo;
								int divider = 0;
								int noteDuration = 0;
								uint32_t period;

								for(int i = 0; i<notes; i+=2) {
										if(end_challenge == 0){break;}
										
										divider = nokiaMelody[i + 1];
										if (divider > 0) {
										noteDuration = (wholenote) / divider;
										} else if (divider < 0) {
										noteDuration = (wholenote) / (int)fabs((float)divider);
										noteDuration *= 1.5;
										}
										
								period = TO_MOD(nokiaMelody[i]);
								TPM2->MOD = period;
								TPM2_C1V = period / 8; //12.5% duty cycle
								osDelay(noteDuration);
								}
					}
					TPM2_C1V = 0;
	}
}
