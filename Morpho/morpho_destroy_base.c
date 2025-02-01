#include "morpho_destroy_base.h"
#include "morpho_definitions.h"
#include "morpho_protocol.h"

#include "Ilv_definitions.h"
#include "Ilv_crc.h"
#include "Ilv_errors.h"

#include <stddef.h>
#include <string.h>
#include <stdio.h>

void MORPHO_DestroyBase_Request(uint8_t* packet, size_t* packetSize)
{
    uint8_t data[4];
    size_t dataSize = 0;

    //DATA 4b
	//ILV - Identifier 1b/Length 2b/Value
    data[dataSize++] = ILV_DESTROY_DB;
    data[dataSize++] = sizeof(uint8_t);
    data[dataSize++] = 0;
    data[dataSize++] = 0;

    MORPHO_MakeSOP(PACKED_ID_TYPE_DATA, 1, 1, MORPHO_GetProtocol().RequestCounter, packet, packetSize);
    MORPHO_AddDataToPacket(packet, packetSize, data, dataSize);
    MORPHO_AddEOP(packet, packetSize);
}

/*
DestroyBase Response

	NACK: 02 E4 00			- STX + ID + RC

	ACK:  02 E2 00			- STX + ID + RC
	response:
		02 E1 00 			- STX + ID + RC
		07 3B 00   			- ILV_GET_DESCRIPTOR + 0x003B total length 59B
		00  				- status ILV_OK/ILVERR_BADPARAMETER/ILVERR_ERROR

		5C D5 1B 03			- CrcL + CrcH + DLE + ETX

	confirm:
	ACK:  02 62 00			- STX + ID + RC
*/

int MORPHO_DestroyBase_Response(const uint8_t* value, size_t valueSize, uint8_t* ilvStatus)
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
        return MORPHO_WARN_VAL_ILV_ERROR;

	return MORPHO_OK;
}
