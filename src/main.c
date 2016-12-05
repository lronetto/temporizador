#include "stm32f10x.h"
#include "conf.h"
#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BOT_P		!(GPIOB->IDR & GPIO_Pin_5)
#define	BOT_MAIS	!(GPIOB->IDR & GPIO_Pin_6)
#define	BOT_MENOS	!(GPIOB->IDR & GPIO_Pin_7)

#define BUZZER		GPIO_Pin_15
#define RELE		GPIO_Pin_12

#define DISP_GPIO	GPIOA
#define DISP_1		GPIO_Pin_10
#define DISP_2		GPIO_Pin_9
#define DISP_3		GPIO_Pin_8
#define DISP_4		GPIO_Pin_11

#define DISP_A		GPIO_Pin_6
#define DISP_B		GPIO_Pin_7
#define DISP_C		GPIO_Pin_1
#define DISP_D		GPIO_Pin_3
#define DISP_E		GPIO_Pin_2
#define DISP_F		GPIO_Pin_5
#define DISP_G		GPIO_Pin_4
#define DISP_DP		GPIO_Pin_0
#define DISP_P		10
#define MASK_DISPS	(DISP_1 | DISP_2 | DISP_3 | DISP_4 )

#define MASK_DISPD	(DISP_A | DISP_B | DISP_C | DISP_D | DISP_E | DISP_F | DISP_G | DISP_DP)


const uint32_t	disps[]={DISP_1,DISP_2,DISP_3,DISP_4};

const uint32_t dispd[11]={
						DISP_A | DISP_B | DISP_C | DISP_D | DISP_E | DISP_F,			//0
						DISP_B | DISP_C,												//1
						DISP_A | DISP_B | DISP_D | DISP_E | DISP_G,						//2
						DISP_A | DISP_B | DISP_C | DISP_D | DISP_G,						//3
						DISP_B | DISP_C | DISP_F | DISP_G,								//4
						DISP_A | DISP_C | DISP_D | DISP_F | DISP_G,						//5
						DISP_A | DISP_C | DISP_D | DISP_E | DISP_F | DISP_G,			//6
						DISP_A | DISP_B | DISP_C,										//7
						DISP_A | DISP_B | DISP_C | DISP_D | DISP_E | DISP_F | DISP_G,	//8
						DISP_A | DISP_B | DISP_C,										//9
						DISP_A | DISP_B | DISP_D | DISP_E | DISP_F | DISP_G};			//P
uint8_t vet_disp[4];
uint8_t vet_dispi=0;

uint16_t secondsp=120;
uint16_t seconds=0,flag_p=0,flag_i=0,flag_cnt=0,flag_a=0;

void gpio_init();
void pisca_led();
void atu();

uint32_t start_pisca;
int main(){
	uint32_t start,tm_aux,tm_p;
	gpio_init();
	Delay_Init();
	usart3_init();
	__enable_irq();
	start_pisca=TIM2->CNT;
	start=TIM2->CNT;
	seconds=secondsp;
	while(1){
		pisca_led();
		atu();
		if((TIM2->CNT-start)>1000){
			start=TIM2->CNT;
		}
		if(BOT_P | flag_p){
			tm_aux=TIM2->CNT;
			while(BOT_P);
			if((TIM2->CNT-tm_aux)>=2000){
				flag_p=1;
				tm_p=TIM2->CNT;
				}
			if((TIM2->CNT-tm_p)>=10000) flag_p=0;
			if(BOT_MAIS){
				Delay_ms(50);
				if(seconds <1000) seconds++;
				else seconds=0;
				while(BOT_MAIS);
				}
			if(BOT_MENOS){
				Delay_ms(50);
				if(seconds > 0 ) seconds--;
				else seconds=999;
				while(BOT_MENOS);
				}
			secondsp=seconds;
			}
		else{
			if(BOT_MAIS){
				Delay_ms(100);
				if(BOT_MAIS){
					flag_i=1;
					GPIOA->ODR|=RELE;
					}
				}
			}
		}
	}
void atu(){
	uint8_t d1=0,d2=0,d3=0,d4=0;
	uint16_t aux=seconds;
	d2=aux/100;
	aux%=100;
	d3=aux/10;
	d4=aux%10;
	if(flag_p) vet_disp[0]=DISP_P;
	else vet_disp[0]=0;
	vet_disp[1]=d2;
	vet_disp[2]=d3;
	vet_disp[3]=d4;
	printf("d1=%d d2=%d d3=%d d4=%d \n\r",d1,d2,d3,d4);
}
void TIM4_IRQHandler(void){
	if (TIM4->SR & TIM_SR_UIF){
		DISP_GPIO->ODR&=~(MASK_DISPS | MASK_DISPD);
		DISP_GPIO->ODR|=disps[vet_dispi];
		if(flag_i)
			DISP_GPIO->ODR|=dispd[vet_disp[vet_dispi]] | DISP_DP;
		else
			DISP_GPIO->ODR|=dispd[vet_disp[vet_dispi]];

		vet_dispi++;
		if(vet_dispi==4) vet_dispi=0;
		TIM4->SR &= ~TIM_SR_UIF;
		if(flag_i){
			flag_cnt++;
			if(flag_cnt==9){
				flag_cnt=0;
				if(seconds>0)  seconds--;
				if(seconds==0){
					flag_a=1;
					flag_i=0;
					seconds=secondsp;
					GPIOA->ODR&=~RELE;
				}
			}
		}
		}
	}
void pisca_led(){
	if((TIM2->CNT-start_pisca)>500){
		GPIOC->ODR&=~GPIO_Pin_13;
		Delay_ms(50);
		GPIOC->ODR|=GPIO_Pin_13;
		start_pisca=TIM2->CNT;
		}
}
void gpio_init(){
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  MASK_DISPS | MASK_DISPD | RELE | BUZZER;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DISP_GPIO, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  BOT_P | BOT_MAIS | BOT_MENOS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

}
