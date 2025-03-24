
#ifndef INC_CAN_HANDLER_H_
#define INC_CAN_HANDLER_H_

#include "can.h"
#include "main.h"

#define ACTUATOR_ADDR	0x0A2
#define SENSOR_ADDR		0X012

extern CAN_TxHeaderTypeDef TxHeader;
extern uint8_t TxBuffer[8];
extern uint32_t Mailbox;

extern CAN_RxHeaderTypeDef RxHeader;
extern CAN_FilterTypeDef canfilterconfig;
extern uint8_t RxData[8];

void CAN_ComInit_Std(CAN_TxHeaderTypeDef *Tx_Header, CAN_HandleTypeDef *hcan, uint32_t id, uint8_t dlc);
void CAN_FilterInit_SingleFF0(CAN_HandleTypeDef *hcan, CAN_FilterTypeDef *canfilterconfig, uint32_t addr);
uint32_t CAN_Transmit(CAN_HandleTypeDef *hcan, CAN_TxHeaderTypeDef *Tx_Header, uint8_t* buffer);

#endif /* INC_CAN_HANDLER_H_ */
