#include "morpho_add_base_record.h"
#include "morpho_definitions.h"
#include "morpho.h"

#include "Ilv_definitions.h"
#include "Ilv_crc.h"
#include "Ilv_errors.h"

#include <stddef.h>
#include <string.h>
#include <stdio.h>

extern uint8_t 	RequestCounter;

void MORPHO_AddBaseRecord_Request(uint8_t* packed, size_t* dataSize,
							     const uint8_t tmplate[], size_t tmplateSize, uint8_t tmplateId,
								 const char* userId,
								 const char* userData[], size_t userDataSize,
								 uint8_t no_check)
{
	size_t packedSize = 0;

	//STX 1b
	packed[packedSize++] = STX;

	//ID 1b - PACKET IDENTIFIER
	packed[packedSize++] = PACKED_ID_TYPE_DATA|PACKED_ID_FLAG_FIRST|PACKED_ID_FLAG_LAST; //0x61;

	//RC 1b
	packed[packedSize++] = RequestCounter++;

	//DATA
	//ILV - Identifier 1b/Length 2b/Value
	packed[packedSize++] = ILV_ADD_RECORD;
	packed[packedSize++] = 1;
	packed[packedSize++] = 0;
	packed[packedSize++] = 0;	//Database identifier

	//ILV - Identifier 1b/Length 2b/Value
	packed[packedSize++] = tmplateId;
	packed[packedSize++] = tmplateSize;
	packed[packedSize++] = 0;
	memcpy(&packed[packedSize], tmplate, tmplateSize);
	packedSize += tmplateSize;

	//ILV - Identifier 1b/Length 2b/Value
	packed[packedSize++] = ID_USER_ID;
	packed[packedSize++] = strlen(userId) + 1;
	packed[packedSize++] = 0;
	memcpy(&packed[packedSize], userId, strlen(userId));
	packedSize += strlen(userId);
	packed[packedSize++] = 0;

	for(int i = 0; i < userDataSize; i++)
	{
		//ILV - Identifier 1b/Length 2b/Value
		packed[packedSize++] = ID_PUC_DATA;
		packed[packedSize++] = strlen(userData[i]) + 1;
		packed[packedSize++] = 0;
		memcpy(&packed[packedSize], userData[i], strlen(userData[i]));
		packedSize += strlen(userData[i]);
		packed[packedSize++] = 0;
	}

	//ILV - Identifier 1b/Length 2b/Value
	packed[packedSize++] = ID_NO_CHECK_ON_TEMPLATE;			//No check on template
	packed[packedSize++] = 1;
	packed[packedSize++] = 0;
	packed[packedSize++] = no_check;

	//recalculate size
	packed[4] = packedSize - 3 - 3;

	//CRC 2b - of data
	uint8_t CrcH = 0;
	uint8_t CrcL = 0;
	IlvCrc16(&packed[3], packedSize - 3, &CrcH, &CrcL);
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
		35 06 00   			- ILV_ADD_RECORD + 0x0006 total length 6B
		00  				- status ILV_OK/ILVERR_BADPARAMETER/ILVERR_ERROR
		31 					- Base Status
		00 00 00 00			- User Database Index

		5C D5 1B 03			- CrcL + CrcH + DLE + ETX

	confirm:
	ACK:  02 62 00			- STX + ID + RC
*/

int MORPHO_AddBaseRecord_Response(const uint8_t* value, size_t valueSize, uint8_t* ilvStatus, uint8_t *baseStatus, uint32_t* userIndex)
{
	if(valueSize==0)
		return MORPHO_WARN_VAL_NO_DATA;

	//all data collected
	uint8_t status = value[0];
	if(ilvStatus) *ilvStatus = status;

	if(status == ILV_OK)
	{
		*baseStatus = value[1];
		*userIndex = value[2] + (value[3] << 8) + (value[4] << 16) + (value[5] << 24);
	}
	else
		return MORPHO_WARN_VAL_ILV_STATUS;

	return MORPHO_OK;
}
