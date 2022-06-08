#include "morpho_get_base_config.h"
#include "morpho_definitions.h"
#include "morpho.h"

#include "Ilv_definitions.h"
#include "Ilv_crc.h"
#include "Ilv_errors.h"

#include <stddef.h>
#include <string.h>
#include <stdio.h>

extern uint8_t 	RequestCounter;

void MORPHO_GetBaseConfig_Request(uint8_t* packed, size_t* dataSize)
{
	size_t packedSize = 0;

	//STX 1b
	packed[packedSize++] = STX;

	//ID 1b - PACKET IDENTIFIER
	packed[packedSize++] = PACKED_ID_TYPE_DATA|PACKED_ID_FLAG_FIRST|PACKED_ID_FLAG_LAST; //0x61;

	//RC 1b
	packed[packedSize++] = RequestCounter++;

	//DATA 1024b
	//ILV - Identifier 1b/Length 2b/Value
	packed[packedSize++] = ILV_GET_BASE_CONFIG;
	packed[packedSize++] = sizeof(uint8_t);
	packed[packedSize++] = 0;
	packed[packedSize++] = 0;

	//CRC 2b - of data
	uint8_t CrcH = 0;
	uint8_t CrcL = 0;
	IlvCrc16(&packed[3], 4, &CrcH, &CrcL);
	packed[packedSize++] = CrcL;
	packed[packedSize++] = CrcH;

	//DLE 1b
	packed[packedSize++] = DLE;

	//ETX 1b
	packed[packedSize++] = ETX;

	if(dataSize) *dataSize = packedSize;
}

/*
GetBaseConfig Response

	NACK: 02 E4 00			- STX + ID + RC

	ACK:  02 E2 00			- STX + ID + RC
	response:
		02 E1 00 			- STX + ID + RC
		07 3B 00   			- ILV_GET_DESCRIPTOR + 0x003B total length 59B
		00  				- status ILV_OK/ILVERR_BADPARAMETER/ILVERR_ERROR
		02					- Number fingers/record
		F4 01 00 00			- Max record number (500)
		04 00 00 00 		- Current Record Number
		F0 01 00 00			- Free Record Number
		02 00 00 00			- Number of Fields
		1B
		12 0C 00 ...		- ILV: ID_TIMESTAMP,  Length value 0x000C
		0F 08 00 ...		- ILV: ID_PUBLIC_FIELD, Length value 0x0008, Field size 2 bytes, Field name 6 bytes
		...

		5C D5 1B 03			- CrcL + CrcH + DLE + ETX

	confirm:
	ACK:  02 62 00			- STX + ID + RC
*/

int MORPHO_GetBaseConfig_Response(const uint8_t* value, size_t valueSize, uint8_t* ilvStatus)
{
	if(valueSize==0)
		return MORPHO_WARN_VAL_NO_DATA;

	//all data collected
	uint8_t status = value[0];
	if(ilvStatus) *ilvStatus = status;

	if(status == ILV_OK)
	{
		;
	}
	else
		return MORPHO_WARN_VAL_ILV_STATUS;

	return MORPHO_OK;
}
