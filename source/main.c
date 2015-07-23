#include "stm32f0xx.h"                  // Device header

/**
 * Set System Clock Speed to 48Mhz
 */
void setToMaxSpeed(void);

/**
 * Configures  the DAC in Pin PA4
 * @param initial_output the DAC output value right after initialization is completed
 */
void dac_init(float initial_output);

/**
 * Set the output value of the DAC
 * @param dac_val DAC output in volts
 */
void dac_output(float dac_val);

int main(void){
	setToMaxSpeed();
	dac_init(0.0);
	while(1){
		for(float volts=0.0; volts<=3.0; volts+=0.1 ){
			dac_output(volts);
			for(int i=0;i<0x7FFFFF;i++);//wait some time
		}
	}
}

void setToMaxSpeed(void){
	int internalClockCounter;
	RCC_PLLCmd(DISABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY));
	RCC_HSEConfig(RCC_HSE_OFF);
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2,RCC_PLLMul_12);
	RCC_PREDIV1Config(RCC_PREDIV1_Div1);
	RCC_PLLCmd(ENABLE);
	while(!RCC_GetFlagStatus(RCC_FLAG_PLLRDY));
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	for(internalClockCounter=0;internalClockCounter<1024;internalClockCounter++){
		if(RCC_GetSYSCLKSource()==RCC_SYSCLKSource_PLLCLK){
			SystemCoreClockUpdate();
			break;
		}
	}
}

void dac_init(float initial_output){
	//Configure PA4 as analog pin
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef myGPIO;
	GPIO_StructInit(&myGPIO);
	myGPIO.GPIO_Pin=GPIO_Pin_4;
	myGPIO.GPIO_Mode=GPIO_Mode_AN;
	GPIO_Init(GPIOA,&myGPIO);
	
	//Configure DAC
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC,ENABLE);
	
	DAC_InitTypeDef myDAC;
	DAC_StructInit(&myDAC);
	myDAC.DAC_Trigger=DAC_Trigger_None;
	myDAC.DAC_WaveGeneration=DAC_WaveGeneration_None;
	myDAC.DAC_OutputBuffer=DAC_OutputBuffer_Enable;
	DAC_Init(DAC_Channel_1,&myDAC);
	
	DAC_SetChannel1Data(DAC_Align_12b_R,((uint16_t)(initial_output*(4095/3.27))));
	
	DAC_Cmd(DAC_Channel_1,ENABLE);
}

void dac_output(float dac_val){
	DAC_SetChannel1Data(DAC_Align_12b_R,((uint16_t)(dac_val*(4095/3.27))));
}
