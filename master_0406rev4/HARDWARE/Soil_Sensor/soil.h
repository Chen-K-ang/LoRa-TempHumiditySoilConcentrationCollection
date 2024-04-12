#ifndef _SOIL_H
#define _SOIL_H


#include "sys.h"

typedef struct
{
	float temp;
	float humidity;
	float ph_val;
	float N_val;
	float P_val;
	float K_val;
}SOIL_TypeDef;

extern SOIL_TypeDef SOIL_Sensor;

#define SOIL_SensorADDR 0X01

#define HUMIDITY_REG 0X0000
#define TEMPERATURE_REG 0X0001
#define CONDUCTIVITY_REG 0X0002
#define PH_REG 0X0003
#define N_REG 0X0004
#define P_REG 0X0005
#define K_REG 0X0006

void Soil_SendCommand(u8 SensorAddr,u8 Cmd,u16 StartAddr,u16 Len);
void Soil_ValAnalysis(SOIL_TypeDef *soil);
void SOIL_GetOneData(SOIL_TypeDef *soil);

#endif
