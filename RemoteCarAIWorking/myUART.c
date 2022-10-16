#include "myUART.h"
#include "cmsis_os2.h"

uint8_t rx_data;
uint8_t uart_data;
extern osMessageQueueId_t motorQ;
extern osMessageQueueId_t rxBuffer;

void tBuffer() { //tBrain
	for(;;) {
		osMessageQueueGet(rxBuffer, &uart_data, NULL, osWaitForever);
		osMessageQueuePut(motorQ, &uart_data, NULL, 0);
	}
}

void UART2_IRQHandler(void) {
	NVIC_ClearPendingIRQ(UART2_IRQn);
	if (UART2->S1 & UART_S1_RDRF_MASK) {
		rx_data = UART2->D;		
		osMessageQueuePut(rxBuffer, &rx_data , NULL, 0);  
	} 
  PORTE->ISFR = 0xffffffff;//clear ISFR
}
void initUART(uint32_t baud_rate){
	uint32_t divisor, bus_clock;
	
	PORTE-> PCR[PTE23] &= ~PORT_PCR_MUX_MASK;
	PORTE-> PCR[PTE23] |= PORT_PCR_MUX(4);
	
	UART2->C2 &= ~((UART_C2_TE_MASK) | (UART_C2_RE_MASK));
	
	bus_clock = DEFAULT_SYSTEM_CLOCK/2;
	divisor = bus_clock / (baud_rate*16);
	UART2->BDH = UART_BDH_SBR(divisor >> 8);
	UART2->BDL = UART_BDL_SBR(divisor);
	
	UART2->C1 = 0;
	UART2->S2 = 0;
	UART2->C3 = 0;
	
	UART2->C2 |= (UART_C2_RE_MASK);
	NVIC_SetPriority(UART2_IRQn, 128);
	NVIC_ClearPendingIRQ(UART2_IRQn);
	NVIC_EnableIRQ(UART2_IRQn);
	UART2->C2 |= UART_C2_RIE_MASK;
}

