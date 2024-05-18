#include "CH552.H"
#include "System.h"
#include "CH552_GPIO.h"

int main()
{
	CfgFsys();	//CH559 clock selection configuration
	mDelaymS(1);
	//CH559GPIOModeSelt(1, 1, 4); //pin1.4 as output (mode 1)
	gpio_set_mode(GPIO_MODE_PP, GPIO_PORT_1, GPIO_PIN_4);

	while (TRUE)
	{
		//SCS = 1;
		gpio_set_pin(GPIO_PORT_1, GPIO_PIN_4);
		mDelaymS(125);
		//SCS = 0;
		gpio_clear_pin(GPIO_PORT_1, GPIO_PIN_4);
		mDelaymS(125);
	}
}

