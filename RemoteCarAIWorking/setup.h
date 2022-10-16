#include "MKL25Z4.h"                    // Device header
#define MASK(x) (1 << (x))

// PWM Driver Ports port Motor
#define LEFT_FORWARD 30 //PTE
#define LEFT_REVERSE 29	//PTE
#define RIGHT_FORWARD 1 //PTC
#define RIGHT_REVERSE 2 //PTC

void initClockGate(void);
void initMotorPWM(void);
