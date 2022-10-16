#include "MKL25Z4.h"                    // Device header
#include "cmsis_os2.h"
#include "myMotor.h"
#define MASK(x) (1<<(x))
#define DIST_STOP 500
#define AVERAGE_WINDOW 3

void initSonicLED(void);
void redOn(void);
void redOff(void);

volatile int echoRise = 1;
volatile int distCount = 0;
volatile int distSum = 0;
volatile int distNow = 0;
volatile int touch = 0;
volatile int overflow = 1;

//volatile int canStop;
extern osSemaphoreId_t canStartSonic;
extern osSemaphoreId_t canSonicStop;
volatile osSemaphoreId_t nextMeasure;

void TPM1_IRQHandler(void) {
	
	NVIC_ClearPendingIRQ(TPM1_IRQn);
	TPM1_STATUS |= TPM_STATUS_CH1F_MASK;
	
	if (TPM1->SC & TPM_SC_TOF_MASK || TPM1_C1V >= 6500 ) {
		overflow = 1;
		distSum = UINT16_MAX - 7500 * AVERAGE_WINDOW -100;
	} else {overflow = 0;}
	if (echoRise) { 
		TPM1_CNT = 0; 
		echoRise = 0;
		TPM1_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
		TPM1_C1SC |= TPM_CnSC_ELSB_MASK;
	} else {
				distSum  += TPM1_C1V;
				distCount+=1;
				if(distCount == AVERAGE_WINDOW){
					distNow = distSum/AVERAGE_WINDOW;
					distSum = 0;
					distCount = 0;
					
					if (distNow <= DIST_STOP) { //about 1 tick :1 mm
						redOn(); 
						//canStop = 1;
						osSemaphoreRelease(canSonicStop);
					} else {
						redOff();
					}
				}
				echoRise = 1;
				//osSemaphoreRelease(nextMeasure);
	}
	NVIC_DisableIRQ(TPM1_IRQn);
}

void initSonic() {
		initSonicLED();
		SIM_SCGC6 |= SIM_SCGC6_TPM1_MASK;

		SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
		SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1); //MCGFLLCLK clock or MCGPLLCLK/2

		SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;
	
		//Trigger
		PORTB_PCR0 &= ~PORT_PCR_MUX_MASK;
		PORTB_PCR0 |= PORT_PCR_MUX(3);
		//Echo
		PORTB_PCR1 &= ~PORT_PCR_MUX_MASK;
		PORTB_PCR1 |= PORT_PCR_MUX(3);
		
		TPM1_SC &= ~(TPM_SC_CMOD_MASK | TPM_SC_PS_MASK | TPM_SC_CPWMS_MASK);
		TPM1_SC |= TPM_SC_PS(7);
		TPM1_MOD = 200000;
		TPM1_C1V = 5000;
		TPM1_C1SC |= TPM_CnSC_CHIE_MASK;
	
		nextMeasure = osSemaphoreNew(1,0,NULL);
}

void tUltrasonic() {
		osSemaphoreAcquire(canStartSonic,osWaitForever);
		
		while(1) {
			
				TPM1_SC &= ~TPM_SC_CMOD_MASK;
				TPM1_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK)); 
				TPM1_C0SC |= (TPM_CnSC_ELSB_MASK | TPM_CnSC_MSA_MASK);

				TPM1_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
				TPM1_C1SC |= TPM_CnSC_ELSA_MASK;
				//TPM1_C1V = 500;

				NVIC_EnableIRQ(TPM1_IRQn);
				NVIC_SetPriority(TPM1_IRQn,0);
				NVIC_ClearPendingIRQ(TPM1_IRQn);
				TPM1_SC |= TPM_SC_CMOD(1);
				
				//TPM1_C1V = 500;
				osDelay(40);
				//osSemaphoreAcquire(nextMeasure,osWaitForever);
		}
}

void redOn(){PTB -> PCOR |= MASK(18);}
void redOff(){PTB -> PSOR |= MASK(18);}
void initSonicLED(void) {
	SIM->SCGC5 |= (SIM_SCGC5_PORTB_MASK);
	PORTB->PCR[18] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[18] |= PORT_PCR_MUX(1);
	PTB->PDDR |= MASK(18);
	redOff();
}
