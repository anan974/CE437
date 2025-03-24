
#include "VL53L1_Handler.h"

uint8_t VL53L1_BootDualSensors(VL53L1X* sensor1, VL53L1X* sensor2)
{
	int status = 0;
	TOF_TurnOff(sensor1);
	TOF_TurnOff(sensor2);

	status |= TOF_BootSensor(sensor1);
	status |= TOF_BootSensor(sensor2);


	return status;
}

void VL53L1_Error_Hanlder(int status) {
	char buffer[10];
	sprintf(buffer, "%d", (int)status);
	Print(buffer, 200);
	while (1) {}
}

uint16_t VL53L1_GetDistance(VL53L1X* sensor) {
	MX_I2C1_Init();
	sensor->distance = TOF_GetDistance(sensor);
	return sensor->distance;
}
