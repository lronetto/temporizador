

#include "stm32f10x_conf.h"
//#include "var.h"

typedef union{
	float val;
	struct{
		char b[4];
    	}byte;
}floatbyte_T;

typedef struct{
	uint8_t status;
	uint8_t id_disp;
	uint8_t id_sys;
	uint8_t tempo;
	floatbyte_T setpoint,hist;
}Disp_T;


typedef struct {
	Disp_T disp;
}EE_t;

void EE_Write(EE_t EEP);
void EE_Read(EE_t *EEP);
