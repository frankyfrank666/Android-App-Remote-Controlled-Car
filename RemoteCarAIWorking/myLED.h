#include "MKL25Z4.h"                    // Device header
#include "RTE_Components.h"
#include "cmsis_os2.h"
#define MASK(x) (1 << (x))

// LED SPEEDS
#define RED_FAST_FLASH_DELAY 250 // milliseconds
#define RED_FLASH_DELAY 500 //milliseconds
#define GREEN_FLASH_DELAY 200 //milliseconds

// Rear LED Pin Config
#define RED_LED_BAR 20 // PortE

//GREEN_LED_BAR 1 - 10 count from no input side
#define GREEN_1 5 //PTD
#define GREEN_2 0 //PTD
#define GREEN_3 2 //PTD
#define GREEN_4 3 //PTD
#define GREEN_5 1 //PTD
#define GREEN_6 7 //PTD
#define GREEN_7 6 //PTD

#define GREEN_8 12  //PTC
#define GREEN_9 13  //PTC
#define GREEN_10 16 //PTC

void greenRunning(void);
void greenAllOn(void);
void redFlash(void);
void redFastFlash(void);
void initLED(void);
void tRedLed(void);
void tGreenLed(void);

