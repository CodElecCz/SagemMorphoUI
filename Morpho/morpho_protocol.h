/*
 * morpho.h
 *
 *  Created on: 20.05.2022
 *      Author: radomir.turca@codelec.cz
 */

#ifndef INC_MORPHO_PROTOCOL_H_
#define INC_MORPHO_PROTOCOL_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void MORPHO_ResetCounter(void);
void MORPHO_SetCounter(uint8_t requestCounter);
uint8_t MORPHO_GetCounter(void);
void MORPHO_ResponseAck(uint8_t* packet, size_t* packetSize);
void MORPHO_ResponseNack(uint8_t* packet, size_t* packetSize);

//request
void MORPHO_MakeSOP(uint8_t Type, uint8_t First, uint8_t Last, uint8_t RC, uint8_t* PacketToSend, size_t* PacketCurrentSize);
void MORPHO_AddDataToPacket(uint8_t* PacketToSend, size_t* PacketCurrentSize, uint8_t* Data, size_t SizeOfSendData);
void MORPHO_AddEOP(uint8_t* PacketToSend, size_t* PacketCurrentSize);

//response
int MORPHO_ReciveSOP(const uint8_t* packet, size_t packetSize, uint8_t *RC, size_t* sopSize);
int MORPHO_ReceiveData(uint8_t *packet, size_t packetSize, uint8_t **value, size_t* valueSize);

#ifdef __cplusplus
}
#endif

#endif /* INC_MORPHO_PROTOCOL_H_ */
