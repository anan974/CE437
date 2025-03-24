
#include "CAN_Handler.h"
CAN_TxHeaderTypeDef TxHeader;
uint32_t Mailbox;
uint8_t TxBuffer[] = { 4, 5, 6, 7, 0, 0, 0, 8 };

CAN_RxHeaderTypeDef RxHeader;
CAN_FilterTypeDef canfilterconfig;
uint8_t RxData[8];

void CAN_ComInit_Std(CAN_TxHeaderTypeDef *Tx_Header, CAN_HandleTypeDef *hcan, uint32_t id, uint8_t dlc)
{
	Tx_Header->IDE = CAN_ID_STD;
	Tx_Header->DLC = dlc;
	Tx_Header->StdId = id;
	Tx_Header->RTR = CAN_RTR_DATA;
	HAL_CAN_Start(hcan);
}

void CAN_FilterInit_SingleFF0(CAN_HandleTypeDef *hcan, CAN_FilterTypeDef *canfilterconfig, uint32_t addr)
{
	canfilterconfig->FilterActivation = CAN_FILTER_ENABLE;
	canfilterconfig->FilterBank = 0;
	canfilterconfig->FilterFIFOAssignment = CAN_FILTER_FIFO0;
	canfilterconfig->FilterIdHigh = addr << 5;
	canfilterconfig->FilterIdLow = 0x0000;
	canfilterconfig->FilterMaskIdHigh = addr << 5;
	canfilterconfig->FilterMaskIdLow = 0x0000;
	canfilterconfig->FilterMode = CAN_FILTERMODE_IDMASK;
	canfilterconfig->FilterScale = CAN_FILTERSCALE_32BIT;

	HAL_CAN_ConfigFilter(hcan, canfilterconfig);
	HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
}

uint32_t CAN_Transmit(CAN_HandleTypeDef *hcan, CAN_TxHeaderTypeDef *Tx_Header, uint8_t* buffer)
{
	HAL_CAN_AddTxMessage(hcan, Tx_Header, buffer, &Mailbox);
	return Mailbox;
}
