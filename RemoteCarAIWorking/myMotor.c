#include "myMotor.h"
extern volatile state_type curr_state;
extern osMutexId_t MotorBusy;

extern osSemaphoreId_t canStartAI;
extern osSemaphoreId_t canSonicStop;
extern osSemaphoreId_t canStartSonic;
extern int start_challenge;
extern int end_challenge;
/*
PTC 1 - CH 0  Right Fwd
PTC 2 - CH 1	Right Rev
PTE 29- CH 2	Left Rev
PTE 30- CH 3	Left Fwd
*/
static int SPEED_CONTROL [11] = {
		STOP,
	
		FWD_F,REV_F,
		FWD_S,REV_S,

		FWD_L,FWD_R,
		REV_L,REV_R,

		STP_L,STP_R
};

void changeSpeed(uint16_t CMD){
		osMutexAcquire(MotorBusy,osWaitForever);

		TPM0->MOD = 7500;		
		uint16_t LSpd =  0x0F & (SPEED_CONTROL[CMD] >> 4);
		uint16_t RSpd = 0x0F & (SPEED_CONTROL[CMD]);

		if(LSpd == 6){
		 TPM0_C2V = 0;
		 TPM0_C3V = 0;
		} else if(LSpd > 6){
				TPM0_C2V = 0;
				TPM0_C3V = MAX_DUTY_CYCLE / 6 * (LSpd - 6);
		} else {
				TPM0_C3V = 0;
				TPM0_C2V = MAX_DUTY_CYCLE / 6 * (6 - LSpd);
		}
		if(RSpd == 6){
				TPM0_C0V = 0;
				TPM0_C1V = 0;
		}else if(RSpd > 6){
				TPM0_C1V = 0;
				TPM0_C0V = MAX_DUTY_CYCLE / 6 * (RSpd - 6);
		}else{
				TPM0_C0V = 0;
				TPM0_C1V = MAX_DUTY_CYCLE / 6 * (6 - RSpd);
		}
		
		if(LSpd == 6 && RSpd == 6) {
			curr_state = STATIONARY;
		} else if( (LSpd > 6 && RSpd > 6) ) {
			curr_state = FORWARD;
		} else if( (LSpd < 6 && RSpd < 6) ) {
			curr_state = REVERSE;
		} else {
			curr_state = TURNING;
		}
				
		osMutexRelease(MotorBusy);
}

/** Stop the motors**/
void stopMotors(){
	osMutexRelease(MotorBusy);
	osMutexAcquire(MotorBusy,osWaitForever);

	TPM0->MOD = 0;
	TPM0_C0V = 0; // stop left fw
	TPM0_C1V = 0; // stop left bk
	TPM0_C2V = 0; // stop left bk
	TPM0_C3V = 0; // stop left bk
	curr_state = STATIONARY;
	
	osMutexRelease(MotorBusy);
}



void changeSpeedTank(uint16_t CMD){
		osMutexAcquire(MotorBusy, osWaitForever);
	
		TPM0->MOD = 7500;
		
		uint16_t LSpd =  0x0F & (CMD >> 4);
		uint16_t RSpd = 0x0F & (CMD);
		//3 fwd 2 rev
		if(LSpd == 7){
		 TPM0_C2V = 0;
		 TPM0_C3V = 0;
		} else if(LSpd > 7){
				TPM0_C2V = 0;
				TPM0_C3V = MAX_DUTY_CYCLE / 6 + (MAX_DUTY_CYCLE * 5/6) /7*(LSpd-7);
		} else {
				TPM0_C3V = 0;
				TPM0_C2V = MAX_DUTY_CYCLE / 6 + (MAX_DUTY_CYCLE * 5/6) /7*(7-LSpd);
		}
		//0 fwd, 1 rev
		if(RSpd == 7){
				TPM0_C0V = 0;
				TPM0_C1V = 0;
		}else if(RSpd > 7){
				TPM0_C1V = 0;
				TPM0_C0V = MAX_DUTY_CYCLE / 6 + (MAX_DUTY_CYCLE * 5/6) /7*(RSpd-7);
		}else{
				TPM0_C0V = 0;
				TPM0_C1V = MAX_DUTY_CYCLE / 6 + (MAX_DUTY_CYCLE * 5/6) /7*(7-RSpd);
		}
		
		if(LSpd == 7 && RSpd == 7) {
			curr_state = STATIONARY;
		} else if( (LSpd > 7 && RSpd > 7) ) {
			curr_state = FORWARD;
		} else if( (LSpd < 7 && RSpd < 7) ) {
			curr_state = REVERSE;
		} else {
			curr_state = TURNING;
		}
		
		osMutexRelease(MotorBusy);
}

/////////////////////////////////////////////////////////////////////////////

#define LL 11
#define RR 12
#define UU 13
#define LT_TIME 220
#define RT_TIME 340
#define FWD_TIME 480

#define turnL_Speed 0x5A
#define turnR_Speed 0xA5
#define fwd_Speed 0xBB
#define BreakTime 700
void TurnRightAndStop(void);

void presetMotion(uint16_t CMD){
	if(CMD == 11){
		changeSpeedTank(turnL_Speed); //Left turn
		osDelay(LT_TIME);
	} else if(CMD == 12){
		changeSpeedTank(turnR_Speed); //Right turn
		//TurnRightAndStop();
		osDelay(RT_TIME);
	} else if(CMD == 13){
		changeSpeedTank(fwd_Speed); //U-turn
		osDelay(FWD_TIME);
	}
	stopMotors();
}
#define Steps 4
void TurnRightAndStop(){
	
	presetMotion(RR);
	stopMotors();
	osDelay(BreakTime);

}
void TurnLeftAndStop(){
	
	presetMotion(LL);
	stopMotors();
	osDelay(BreakTime);

}
void goStrightAndStop(){
				changeSpeedTank(fwd_Speed);
				osDelay(FWD_TIME);
				stopMotors();
				osDelay(BreakTime);
}

void tSelfDrive(){	
	
		osSemaphoreAcquire(canStartAI,osWaitForever);
	
				start_challenge = 1;
				end_challenge = 0;

				changeSpeedTank(fwd_Speed);
		osSemaphoreRelease(canStartSonic);
	
		osSemaphoreAcquire(canSonicStop,osWaitForever);
		osSemaphoreAcquire(canSonicStop,osWaitForever);

				stopMotors();
				osDelay(BreakTime);
				
				TurnLeftAndStop();				
				changeSpeedTank(fwd_Speed);
				goStrightAndStop();
				
				TurnRightAndStop();
				goStrightAndStop();

				TurnRightAndStop();
				goStrightAndStop();

				TurnRightAndStop();
				goStrightAndStop();
				
				TurnLeftAndStop();
				
				changeSpeedTank(fwd_Speed);
				
		osSemaphoreAcquire(canSonicStop,osWaitForever);
		osSemaphoreAcquire(canSonicStop,osWaitForever);
		osSemaphoreAcquire(canSonicStop,osWaitForever);
		osSemaphoreAcquire(canSonicStop,osWaitForever);

				stopMotors();
				
				start_challenge = 0;
				end_challenge = 1;

}
