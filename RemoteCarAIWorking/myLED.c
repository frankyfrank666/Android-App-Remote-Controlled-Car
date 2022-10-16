#include "myLED.h"
#include "myMotor.h"

extern volatile state_type curr_state;

static int GREEN_LED_ARRAY [10] = {GREEN_1,GREEN_2,GREEN_3,GREEN_4,GREEN_5,GREEN_6,GREEN_7,GREEN_8,GREEN_9,GREEN_10};


void tRedLed(){
	while(1){
		while(curr_state == STATIONARY){
			redFastFlash();
		}
		while(curr_state != STATIONARY){
			redFlash();
		}
	}
}
void tGreenLed(){
	while(1){
		while(curr_state == STATIONARY){
			greenAllOn();
		}
		while(curr_state != STATIONARY){
			greenRunning();
		}
	}
}


















// GREEN RUNNING
// when moving in any direction, green running
void greenRunning(){
	for(int i = 0; i < 10;i++){
		if(curr_state == STATIONARY){break;}
		
		if (i < 7) {
				PTD -> PSOR |= MASK(GREEN_LED_ARRAY[i]);
		} else {
				PTC -> PSOR |= MASK(GREEN_LED_ARRAY[i]);
		}
		osDelay(GREEN_FLASH_DELAY);
		PTD -> PCOR |= MASK(GREEN_1) | MASK(GREEN_2) | MASK(GREEN_3) | MASK(GREEN_4) | MASK(GREEN_5) | MASK(GREEN_6) | MASK(GREEN_7);
		PTC -> PCOR |= MASK(GREEN_8) | MASK(GREEN_9) | MASK(GREEN_10);
	}
}

// GREEN BOOTUP
// When stopped, green all lights up
void greenAllOn(){
		PTD -> PSOR |= MASK(GREEN_1) | MASK(GREEN_2) | MASK(GREEN_3) | MASK(GREEN_4) | MASK(GREEN_5) | MASK(GREEN_6) | MASK(GREEN_7);
		PTC -> PSOR |= MASK(GREEN_8) | MASK(GREEN_9) | MASK(GREEN_10);
}

// RED FLASHING
// when moving at any direction, 500ms ON, 500ms OFF,
void redFlash(){
	PTE -> PSOR |= MASK(RED_LED_BAR);
	osDelay(RED_FLASH_DELAY);
	PTE -> PCOR |= MASK(RED_LED_BAR);
	osDelay(RED_FLASH_DELAY);
}

// RED FAST FLASH
// when stopped
void redFastFlash(){
	PTE -> PCOR |= MASK(RED_LED_BAR);
	osDelay(RED_FAST_FLASH_DELAY);
	PTE -> PSOR |= MASK(RED_LED_BAR);
	osDelay(RED_FAST_FLASH_DELAY);
}



void initLED(void) {
	SIM->SCGC5 |= (SIM_SCGC5_PORTD_MASK) | (SIM_SCGC5_PORTC_MASK) | (SIM_SCGC5_PORTE_MASK) | (SIM_SCGC5_PORTB_MASK);

	//RED LED BAR
	PORTE->PCR[RED_LED_BAR] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[RED_LED_BAR] |= PORT_PCR_MUX(1);
	
	//Green LED BAR
  //PORT D {GREEN_1,GREEN_2,GREEN_3,GREEN_4,GREEN_5,GREEN_6, GREEN_7
	PORTD->PCR[GREEN_1] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[GREEN_1] |= PORT_PCR_MUX(1);
	PORTD->PCR[GREEN_2] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[GREEN_2] |= PORT_PCR_MUX(1);
	PORTD->PCR[GREEN_3] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[GREEN_3] |= PORT_PCR_MUX(1);
	PORTD->PCR[GREEN_4] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[GREEN_4] |= PORT_PCR_MUX(1);
	PORTD->PCR[GREEN_5] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[GREEN_5] |= PORT_PCR_MUX(1);
	PORTD->PCR[GREEN_6] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[GREEN_6] |= PORT_PCR_MUX(1);
	PORTD->PCR[GREEN_7] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[GREEN_7] |= PORT_PCR_MUX(1);
	
	//PORT C ,GREEN_8,GREEN_9,GREEN_10}
	PORTC->PCR[GREEN_8] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[GREEN_8] |= PORT_PCR_MUX(1);
	PORTC->PCR[GREEN_9] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[GREEN_9] |= PORT_PCR_MUX(1);
	PORTC->PCR[GREEN_10] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[GREEN_10] |= PORT_PCR_MUX(1);
	 
	// Set Data Direction Registers
	PTE->PDDR |= MASK(RED_LED_BAR);
	PTD->PDDR |= MASK(GREEN_1) | MASK(GREEN_2) | MASK(GREEN_3) | MASK(GREEN_4) | MASK(GREEN_5) | MASK(GREEN_6) | MASK(GREEN_7);
	PTC->PDDR |= MASK(GREEN_8) | MASK(GREEN_9) | MASK(GREEN_10);
}
