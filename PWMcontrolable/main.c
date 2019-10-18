
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
#include "inc/hw_nvic.h"
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
#include "uc1701.h"

void KeyIntHandler();
#define GPIO_LOCK_KEY 0x4C4F434B
#define GPIO_O_CR 0x00000524 // GPIO Commit
#define GPIO_O_LOCK 0x00000520 // GPIO Lock

#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif
//this code is the main function
int FRE=10000;
float D=0.5;//Ƶ�ʣ�ռ�ձ�
int main(void)
{
	SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);//����ϵͳʱ��
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);//ʹ��Fģ��
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);//ʹ��Fģ��
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);

		GPIOPinConfigure(GPIO_PF2_T1CCP0);
	    GPIOPinTypeTimer(GPIO_PORTF_BASE, GPIO_PIN_2);

	    TimerConfigure(TIMER1_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PWM);
	    TimerLoadSet(TIMER1_BASE, TIMER_A, 50000);//����PWMƵ��50kHz
		TimerMatchSet(TIMER1_BASE, TIMER_A,(50000)*D);//ռ�ձ�50%
	    TimerEnable(TIMER1_BASE, TIMER_A);

	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;

	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,GPIO_PIN_0); //����PF2��PF3Ϊ���ģʽ
	GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_0,1);//��PF0д0����Ϩ������

	GPIOPinTypeGPIOInput(GPIO_PORTA_BASE,GPIO_PIN_3|GPIO_PIN_2); //��PF4����Ϊ����ģʽ
	GPIOIntRegister(GPIO_PORTA_BASE, KeyIntHandler);//ΪFģ��ע��һ���жϷ�����������ʹ��Fģ���ж�
	GPIOIntTypeSet(GPIO_PORTA_BASE,GPIO_PIN_3|GPIO_PIN_2,GPIO_FALLING_EDGE); //����PF4Ϊ�½��ش���
	GPIOIntEnable(GPIO_PORTA_BASE,GPIO_PIN_3|GPIO_PIN_2); //ʹ��PF4�ж�
	IntEnable(INT_GPIOF_TM4C123);//INT_GPIOF_TM4C123�����궨����inc/hw_ints.h�У�ֵΪ46��IntEnable������interrupt.c��
//	HWREG(NVIC_EN0) |= 0x40000000;        //����һ�仰�����õ�ͬ��ʹ��IRQ30����PORTF�ж�

	while(1)
	{
		/*UC1701Clear();
				UC1701DisplayN(1, 1, D);*/
	}
}

//this code is the handler function for key
	void KeyIntHandler()
	{
		//GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_0,(0x01^GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)));
		//temp = GPIOIntStatus(GPIO_PORTA_BASE, 0);// & GPIO_RIS_GPIO_M; //GPIORIS

		if(GPIOIntStatus(GPIO_PORTA_BASE, 0)== 4)
		{
			GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_0,1);
			if(D<1)D=D+0.1;

		}
		else if(GPIOIntStatus(GPIO_PORTA_BASE, 0)== 8)
		{
			GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_0,0);
			if(D>0)D=D-0.1;
		}
		TimerMatchSet(TIMER1_BASE, TIMER_A,(50000)*D);
		GPIOIntClear(GPIO_PORTA_BASE,GPIO_INT_PIN_2);
		GPIOIntClear(GPIO_PORTA_BASE,GPIO_INT_PIN_3);

	}
