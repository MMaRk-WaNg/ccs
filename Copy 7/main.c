#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_timer.h"
#include "inc/hw_ints.h"
#include "inc/hw_gpio.h"
#include "inc/hw_i2c.h"
#include "inc/hw_sysctl.h"
#include "uc1701.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/i2c.h"
#include "driverlib/udma.h"
#include "driverlib/fpu.h"
#include "driverlib/rom.h"

uint32_t Temp1;
uint32_t Temp2;
uint32_t temp1;//last Temp1
int step = 0;
int t;
int tt;
float round = 10; //Ò»È¦³¤mm

#ifdef DEBUG
void __error__(char *pcFilename, unsigned long ulLine) {}
#endif

void Timer2IntHandler(void) {
	TimerIntClear(TIMER1_BASE, TIMER_ICR_CAECINT);
	TimerIntClear(TIMER2_BASE, TIMER_ICR_CAECINT);
	temp1 = Temp1;
	Temp1 = TimerValueGet(TIMER1_BASE, TIMER_A);
	Temp2 = TimerValueGet(TIMER2_BASE, TIMER_A);

}

int main(void) {
	SysCtlClockSet(
			SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN
					| SYSCTL_XTAL_16MHZ);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinConfigure(GPIO_PF2_T1CCP0);
	GPIOPinTypeTimer(GPIO_PORTF_BASE, GPIO_PIN_2);
	TimerConfigure(TIMER1_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_TIME_UP);
	TimerIntRegister(TIMER1_BASE, TIMER_A, Timer2IntHandler);
	TimerIntEnable(TIMER1_BASE, TIMER_CAPA_EVENT);
	TimerEnable(TIMER1_BASE, TIMER_A);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinConfigure(GPIO_PF4_T2CCP0);
	GPIOPinTypeTimer(GPIO_PORTF_BASE, GPIO_PIN_4);
	TimerConfigure(TIMER2_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_TIME_UP);
	TimerIntRegister(TIMER2_BASE, TIMER_A, Timer2IntHandler);
	TimerIntEnable(TIMER2_BASE, TIMER_CAPA_EVENT);
	TimerEnable(TIMER2_BASE, TIMER_A);
	while (1) {
		UC1701Init(60000);
		UC1701Clear();
		int t1, t2, tt1;
		t1 = Temp1;
		t2 = Temp2;
		tt1 = temp1;
		UC1701DisplayN(1, 1, t1);
		UC1701DisplayN(3, 1, t2);
		UC1701DisplayN(2, 1, tt1);
		if (t1 > t2&&(t1-t2)<1500)
		{
			UC1701CharDispaly(0, 1, "-");
			/*UC1701DisplayN(1, 1, 0.02*(Temp1-Temp2));*/
		}
		else{
			UC1701CharDispaly(0, 1, "-");
			/*UC1701DisplayN(1, 1, 0.02*(Temp2-Temp1));*/
		}
		t = (t1 - tt1);
		tt = 10 * 50000000 / (256 * t);
		if (t > 0 && tt < 1000) {
			UC1701DisplayN(0, 3, tt);
			/*UC1701CharDispaly(1, 5, "us");*/
			SysCtlDelay(5000000);
		}
	}
}
