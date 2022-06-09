#include "morpho_configure_uart.h"
#include "morpho_definitions.h"
#include "morpho.h"

#include "Ilv_definitions.h"
#include "Ilv_crc.h"
#include "Ilv_errors.h"

#include <stddef.h>
#include <string.h>
#include <stdio.h>

extern uint8_t 	RequestCounter;

void MORPHO_ConfigureUart_Request(uint8_t* packet, size_t* packetSize, uint32_t bps)
{
    uint8_t data[16];
    size_t dataSize = 0;

	//DATA 14b
	//ILV - Identifier 1b/Length 2b/Value
    data[dataSize++] = ILV_CONFIG_UART;
    data[dataSize++] = 0x0D;
    data[dataSize++] = 0;

	//DATA 10b
	//ILV - Identifier 1b/Length 2b/Value
    data[dataSize++] = ID_COM1;
    data[dataSize++] = 0x0A;
    data[dataSize++] = 0;
    data[dataSize++] = bps & 0xff;			//baudrate
    data[dataSize++] = (bps >> 8) & 0xff;
    data[dataSize++] = (bps >> 16) & 0xff;
    data[dataSize++] = (bps >> 24) & 0xff;
    data[dataSize++] = 8; //byte size
    data[dataSize++] = 1; //stop bit
    data[dataSize++] = 0; //parity
    data[dataSize++] = 2; //flow
    data[dataSize++] = 0; //send
    data[dataSize++] = 0; //format

    MORPHO_MakeSOP(PACKED_ID_TYPE_DATA, 1, 1, RequestCounter, packet, packetSize);
    MORPHO_AddDataToPacket(packet, packetSize, data, dataSize);
    MORPHO_AddEOP(packet, packetSize);
}

/*
ConfigureUart Response

	NACK: 02 E4 00			- STX + ID + RC

	ACK:  02 E2 00			- STX + ID + RC
	response:
		02 E1 00 			- STX + ID + RC
		EE 01 00   			- ILV_GET_DESCRIPTOR + 0x0005 total length 1b
		00 80 				- status ILV_OK/ILVERR_ERROR
		1B 12 1B 03			- CrcL + CrcH + DLE + ETX

	confirm:
	ACK:  02 62 00			- STX + ID + RC

*/

int MORPHO_ConfigureUart_Response(const uint8_t* value, size_t valueSize, uint8_t* ilvStatus)
{
	if(valueSize==0)
		return MORPHO_WARN_VAL_NO_DATA;

	//all data collected
	uint8_t status = value[0];
	if(ilvStatus) *ilvStatus = status;

	if(status == ILV_OK)
	{

	}
	else
		return MORPHO_WARN_VAL_ILV_STATUS;

	return MORPHO_OK;
}
