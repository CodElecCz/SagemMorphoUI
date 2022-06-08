#include "morpho_get_data.h"
#include "morpho_definitions.h"
#include "morpho.h"

#include "Ilv_definitions.h"
#include "Ilv_crc.h"
#include "Ilv_errors.h"

#include <stddef.h>
#include <string.h>
#include <stdio.h>

extern uint8_t 	RequestCounter;

void MORPHO_GetData_Request(uint8_t* packed, size_t* dataSize, uint32_t fieldIndex, uint32_t userIndex)
{
	size_t packedSize = 0;

	//STX 1b
	packed[packedSize++] = STX;

	//ID 1b - PACKET IDENTIFIER
	packed[packedSize++] = PACKED_ID_TYPE_DATA|PACKED_ID_FLAG_FIRST|PACKED_ID_FLAG_LAST; //0x61;

	//RC 1b
	packed[packedSize++] = RequestCounter++;

	//DATA 12b
	//ILV - Identifier 1b/Length 2b/Value
	packed[packedSize++] = ILV_GET_DATA;
	packed[packedSize++] = 5 + 7;
	packed[packedSize++] = 0;
	packed[packedSize++] = 0;	//Database identifier
	packed[packedSize++] = fieldIndex & 0xff;
	packed[packedSize++] = (fieldIndex >> 8) & 0xff;
	packed[packedSize++] = (fieldIndex >> 16) & 0xff;
	packed[packedSize++] = (fieldIndex >> 24) & 0xff;

	//DATA 7b
	//ILV - Identifier 1b/Length 2b/Value
	packed[packedSize++] = ID_USER_INDEX;
	packed[packedSize++] = 4;
	packed[packedSize++] = 0;
	packed[packedSize++] = userIndex & 0xff;
	packed[packedSize++] = (userIndex >> 8) & 0xff;
	packed[packedSize++] = (userIndex >> 16) & 0xff;
	packed[packedSize++] = (userIndex >> 24) & 0xff;

	//CRC 2b - of data
	uint8_t CrcH = 0;
	uint8_t CrcL = 0;
	IlvCrc16(&packed[3], 15, &CrcH, &CrcL);
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
		3F 03 00   			- ILV_GET_DATA + 0x0003 total length 3B
		00  				- status ILV_OK/ILVERR_BADPARAMETER/ILVERR_ERROR
		31 00				- User Data "1"

		5C D5 1B 03			- CrcL + CrcH + DLE + ETX

	confirm:
	ACK:  02 62 00			- STX + ID + RC
*/

int MORPHO_GetData_Response(const uint8_t* value, size_t valueSize, uint8_t* ilvStatus, const char** userData)
{
	if(valueSize==0)
		return MORPHO_WARN_VAL_NO_DATA;

	//all data collected
	uint8_t status = value[0];
	if(ilvStatus) *ilvStatus = status;

	if(status == ILV_OK)
	{
		*userData = (const char*)&value[1];
	}
	else
		return MORPHO_WARN_VAL_ILV_STATUS;

	return MORPHO_OK;
}
