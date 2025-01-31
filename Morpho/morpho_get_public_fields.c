#include "morpho_get_public_fields.h"
#include "morpho_definitions.h"
#include "morpho_protocol.h"

#include "Ilv_definitions.h"
#include "Ilv_crc.h"
#include "Ilv_errors.h"

#include <stddef.h>
#include <string.h>
#include <stdio.h>

extern uint8_t 	RequestCounter;

void MORPHO_GetPublicFields_Request(uint8_t* packet, size_t* packetSize, uint32_t fieldIndex)
{
    uint8_t data[15];
    size_t dataSize = 0;

	//DATA 12b
	//ILV - Identifier 1b/Length 2b/Value
    data[dataSize++] = ILV_GET_PUBLIC_FIELDS;
    data[dataSize++] = 5;
    data[dataSize++] = 0;
    data[dataSize++] = 0;	//Database identifier
    data[dataSize++] = fieldIndex & 0xff;
    data[dataSize++] = (fieldIndex >> 8) & 0xff;
    data[dataSize++] = (fieldIndex >> 16) & 0xff;
    data[dataSize++] = (fieldIndex >> 24) & 0xff;

    MORPHO_MakeSOP(PACKED_ID_TYPE_DATA, 1, 1, RequestCounter, packet, packetSize);
    MORPHO_AddDataToPacket(packet, packetSize, data, dataSize);
    MORPHO_AddEOP(packet, packetSize);
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

int MORPHO_GetPublicFields_Response(const uint8_t* value, size_t valueSize, uint8_t* ilvStatus, uint32_t* userNumber)
{
	if(valueSize==0)
		return MORPHO_WARN_VAL_NO_DATA;

	//all data collected
	uint8_t status = value[0];
	if(ilvStatus) *ilvStatus = status;

	if(status == ILV_OK)
	{
		if(userNumber)
			*userNumber = value[1] + (value[2] << 8) + (value[3] << 16) + (value[4] << 24);
	}
	else
        return MORPHO_WARN_VAL_ILV_ERROR;

	return MORPHO_OK;
}
