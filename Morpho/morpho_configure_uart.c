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

void MORPHO_ConfigureUart_Request(uint8_t* packed, size_t* dataSize, uint32_t bps)
{
	size_t packedSize = 0;

	//STX 1b
	packed[packedSize++] = STX;

	//ID 1b - PACKET IDENTIFIER
	packed[packedSize++] = PACKED_ID_TYPE_DATA|PACKED_ID_FLAG_FIRST|PACKED_ID_FLAG_LAST; //0x61;

	//RC 1b
	packed[packedSize++] = RequestCounter++;

	//DATA 14b
	//ILV - Identifier 1b/Length 2b/Value
	packed[packedSize++] = ILV_CONFIG_UART;
	packed[packedSize++] = 0x0D;
	packed[packedSize++] = 0;

	//DATA 10b
	//ILV - Identifier 1b/Length 2b/Value
	packed[packedSize++] = ID_COM1;
	packed[packedSize++] = 0x0A;
	packed[packedSize++] = 0;
	packed[packedSize++] = bps & 0xff;			//baudrate
	packed[packedSize++] = (bps >> 8) & 0xff;
	packed[packedSize++] = (bps >> 16) & 0xff;
	packed[packedSize++] = (bps >> 24) & 0xff;
	packed[packedSize++] = 8; //byte size
	packed[packedSize++] = 1; //stop bit
	packed[packedSize++] = 0; //parity
	packed[packedSize++] = 2; //flow
	packed[packedSize++] = 0; //send
	packed[packedSize++] = 0; //format

	//CRC 2b - of data
	uint8_t CrcH = 0;
	uint8_t CrcL = 0;
	IlvCrc16(&packed[3], 16, &CrcH, &CrcL);
	packed[packedSize++] = CrcL;
	packed[packedSize++] = CrcH;

	//DLE 1b
	packed[packedSize++] = DLE;

	//ETX 1b
	packed[packedSize++] = ETX;

	if(dataSize) *dataSize = packedSize;
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
