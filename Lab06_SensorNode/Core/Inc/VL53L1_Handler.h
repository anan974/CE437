
#include "main.h"
#include "usart.h"
#include "i2c.h"
#include "VL53L1X.h"
#include "VL53L1X_api.h"

#ifndef INC_VL53L1_HANDLER_H_
#define INC_VL53L1_HANDLER_H_

uint8_t VL53L1_BootDualSensors(VL53L1X* sensor1, VL53L1X* sensor2);
void VL53L1_Error_Hanlder(int status);
uint16_t VL53L1_GetDistance(VL53L1X* sensor);
void VL53L1_TestSensors(char *buffer);
#endif /* INC_VL53L1_HANDLER_H_ */
