#include "MKL25Z4.h"                    // Device header
#include "cmsis_os2.h"
#include "setup.h"
#include "myMotor.h"
#include "myLED.h"
#include "mySonic.h"
#include "myUART.h"
#include "music.h"

osMutexId_t MotorBusy;
volatile osSemaphoreId_t canStartAI;
volatile osSemaphoreId_t canStartSonic;
volatile osSemaphoreId_t canSonicStop;

volatile state_type curr_state = STATIONARY;
volatile int start_challenge = 0;
volatile int end_challenge = 0;

osMessageQueueId_t motorQ;
osMessageQueueId_t rxBuffer;

void Core_control() {
	uint8_t RecvCommand = 0x00;
	uint8_t TankControl = 0;
	
	for(;;) {
		osMessageQueueGet(motorQ, &RecvCommand, NULL, osWaitForever);
		if(RecvCommand == 0xff){
			//TankControl ^= 1;
		} else if (TankControl){
			changeSpeedTank(RecvCommand);
		}
		
		else if(RecvCommand < 11){
			changeSpeed(RecvCommand);
		} else if (RecvCommand < 14){
			presetMotion(RecvCommand);
		}	else if (RecvCommand < 16) {
			
			osSemaphoreRelease(canStartAI);
			
		}	else if (RecvCommand == 16) {
			start_challenge = 1;
			end_challenge = 0;
		} else if (RecvCommand == 17) {
			start_challenge = 0;
			end_challenge = 1;
		} else if (RecvCommand == 18) {
			start_challenge = 0;
			end_challenge = 0;
		} else {}
		 
	}
}

int main(void) {
  SystemCoreClockUpdate();
	initClockGate();
	initUART(BAUD_RATE);
	initMotorPWM();
	initLED();
	initMusic();	
	initSonic();
	
	osKernelInitialize();                //-------------------------------------------

	MotorBusy = osMutexNew(NULL);
	canStartAI = osSemaphoreNew(1,0,NULL);
	canStartSonic = osSemaphoreNew(1,0,NULL);
	canSonicStop = osSemaphoreNew(1,0,NULL);
	
	osThreadNew(tGreenLed, NULL, NULL);
	osThreadNew(tRedLed, NULL, NULL);
	osThreadNew(tBuffer, NULL, NULL);
	osThreadNew(Core_control, NULL, NULL);

	rxBuffer = osMessageQueueNew(1, sizeof(uint8_t), NULL);
	motorQ = osMessageQueueNew(1, sizeof(uint8_t), NULL);

	osThreadNew(playMusic, NULL, NULL);
	osThreadNew(tUltrasonic, NULL, NULL);
	osThreadNew(tSelfDrive, NULL, NULL);
	osKernelStart();                      // ---------------------------------------------	
	
  for (;;){}
}

