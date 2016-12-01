#include <stddef.h>
#include "stm32l1xx.h"

volatile uint16_t ADC1ConvertedValue[4] = {0,0,0,0};//Stores converted vals
volatile uint16_t hodnota1, hodnota2, hodnota3, hodnota4;
void adc_init(void)			// inicalizujem ADC a GPIO
  {
	ADC_InitTypeDef       ADC_InitStruct;
	ADC_CommonInitTypeDef ADC_CommonInitStruct;
	DMA_InitTypeDef       DMA_InitStruct;
	GPIO_InitTypeDef      GPIO_InitStruct;

	/* Enable ADC1, DMA1 and GPIO clocks ****************************************/
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);//ADC1 is connected to the APB2 peripheral bus

	/* DMA1 Stream0 channel0 configuration **************************************/
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;//ADC1's data register
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)&ADC1ConvertedValue[0];
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStruct.DMA_BufferSize = 4;
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//Reads 16 bit values
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//Stores 16 bit values
	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;
	DMA_Init(DMA1_Channel1, &DMA_InitStruct);
	DMA_Cmd(DMA1_Channel1, ENABLE); //Enable the DMA1 - Channel1
	/* Configure GPIO pins ******************************************************/
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4;// PA0, PA1, PA2
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;//The pins are configured in analog mode
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL ;//We don't need any pull up or pull down
	GPIO_Init(GPIOA, &GPIO_InitStruct);//Initialize GPIOC pins with the configuration
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;//PB1
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;//The pins are configured in analog mode
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL ;//We don't need any pull up or pull down
	GPIO_Init(GPIOB, &GPIO_InitStruct);//Initialize GPIOA pins with the configuration
	RCC_HSICmd(ENABLE);

	/* ADC Common Init **********************************************************/
	ADC_CommonInit(&ADC_CommonInitStruct);

	ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;//Input voltage is converted into a 12bit int (max 4095)
	ADC_InitStruct.ADC_ScanConvMode = ENABLE;//The scan is configured in multiple channels
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;//Continuous conversion: input signal is sampled more than once
	ADC_InitStruct.ADC_NbrOfConversion = 4;
	ADC_InitStruct.ADC_ExternalTrigConv = 0;
	ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;//Data converted will be shifted to right
	ADC_Init(ADC1, &ADC_InitStruct);//Initialize ADC with the configuration

	/* Select the channels to be read from **************************************/
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_16Cycles);//PC0
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_16Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 3, ADC_SampleTime_16Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 4, ADC_SampleTime_16Cycles);


	/* Enable DMA request after last transfer (Single-ADC mode) */
	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);

	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);

	/* Start ADC1 Software Conversion */
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_ADONS) == RESET){}

	ADC_SoftwareStartConv(ADC1);
  }


int main(void)
{
  adc_init();
  DMA_Cmd(DMA1_Channel1, ENABLE);
  //Start ADC1 Software Conversion
  ADC_SoftwareStartConv(ADC1);

  //wait for DMA complete
  while(1){
  	hodnota1 = ADC1ConvertedValue[0];
  	hodnota2 = ADC1ConvertedValue[1];
  	hodnota3 = ADC1ConvertedValue[2];
  	hodnota4 = ADC1ConvertedValue[3];
  	}
  return 0;
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/*
 * Minimal __assert_func used by the assert() macro
 * */
void __assert_func(const char *file, int line, const char *func, const char *failedexpr)
{
  while(1)
  {}
}

/*
 * Minimal __assert() uses __assert__func()
 * */
void __assert(const char *file, int line, const char *failedexpr)
{
   __assert_func (file, line, NULL, failedexpr);
}
