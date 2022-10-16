#include "MKL25Z4.h"
#include <math.h>
#include "cmsis_os2.h"

#define MAX_DUTY_CYCLE 0x1D4C //7500 (50Hz)

typedef enum STATES {
	STATIONARY,
	FORWARD,
	REVERSE,
	TURNING
} state_type;

void changeSpeed(uint16_t);
void stopMotors(void);
void presetMotion(uint16_t);
void changeSpeedTank(uint16_t);
void tSelfDrive(void);
// left 4 bits = L speed
// righ 4 bits = R speed

/*
MAX_DUTY_CYCLE = 12 or 0xC)
stop = 6
-MAX_DUTY_CYCLE = 0
*/

#define STOP  0x66
#define FWD_F 0xCC
#define REV_F 0x00

#define FWD_S 0x88
#define REV_S 0x33

#define FWD_L 0x68
#define FWD_R 0x86

#define REV_L 0x73
#define REV_R 0x37

#define STP_L 0x2a
#define STP_R	0xa2

/*
Arduino section code
   ("STOP" ) != -1) 0));}
  //MOVE
   ("FWD_F") != -1) 1));}
   ("REV_F") != -1) 2));}

   ("FWD_S") != -1) 3));}
   ("REV_S") != -1) 4));}
  
  //Turning
   ("FWD_L") != -1) 5));}
   ("FWD_R") != -1) 6));}
   ("REV_L") != -1) 7));}
   ("REV_R") != -1) 8));}
  
   ("STP_L") != -1) 9));}
   ("STP_R") != -1) 10));}
	//////////////////////////////////////////////////////////////////////
   ("R_90") != -1) 11));}
   ("L_90") != -1) 12));}
   ("U_T") != -1) 13));}

  //Mode
   ("MAN_M") != -1) 14));}
   ("AI_M") != -1) 15));}
*/



