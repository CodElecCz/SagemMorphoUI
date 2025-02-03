#include "morpho_add_base_record.h"
#include "morpho_definitions.h"
#include "morpho_protocol.h"

#include "Ilv_definitions.h"
#include "Ilv_crc.h"
#include "Ilv_errors.h"

#include <stddef.h>
#include <string.h>
#include <stdio.h>

int MORPHO_AddBaseRecord_Request(uint8_t* packet, size_t* packetSize,
							     const uint8_t tmplate[], size_t tmplateSize, uint8_t tmplateId,
								 const char* userId,
                                 const char* userData[], size_t userDataSize, uint8_t userDataFieldSize,
								 uint8_t no_check)
{
    uint8_t data[512]; //512
    size_t dataSize = 0;

    if(sizeof(data) < (4 + 3 + tmplateSize + 4 + strlen(userId) + 4))
    	return MORPHO_ERR_DATA_LENGTH; //TODO: check userDataSize

	//DATA
	//ILV - Identifier 1b/Length 2b/Value
    data[dataSize++] = ILV_ADD_RECORD;
    data[dataSize++] = 0;
    data[dataSize++] = 0;
    data[dataSize++] = 0;	//Database identifier

	//ILV - Identifier 1b/Length 2b/Value
    data[dataSize++] = tmplateId;
    data[dataSize++] = tmplateSize;
    data[dataSize++] = 0;
    memcpy(&data[dataSize], tmplate, tmplateSize);
    dataSize += tmplateSize;

	//ILV - Identifier 1b/Length 2b/Value
    data[dataSize++] = ID_USER_ID;
    data[dataSize++] = strlen(userId) + 1;	//The maximum size (L) of the User ID is 24 bytes
    data[dataSize++] = 0;
    memcpy(&data[dataSize], userId, strlen(userId));
    dataSize += strlen(userId);
    data[dataSize++] = 0;

	for(int i = 0; i < userDataSize; i++)
	{
		//ILV - Identifier 1b/Length 2b/Value
        data[dataSize++] = ID_PUC_DATA;
        data[dataSize++] = userDataFieldSize;
        data[dataSize++] = 0;
        memcpy(&data[dataSize], userData[i], userDataFieldSize);
        dataSize += userDataFieldSize;
	}

	//ILV - Identifier 1b/Length 2b/Value
    data[dataSize++] = ID_NO_CHECK_ON_TEMPLATE;			//No check on template
    data[dataSize++] = 1;
    data[dataSize++] = 0;
    data[dataSize++] = no_check;

	//recalculate size
    data[1] = 0xff & (dataSize - 3);
    data[2] = 0xff & ((dataSize - 3) >> 8);

    MORPHO_MakeSOP(PACKED_ID_TYPE_DATA, 1, 1, MORPHO_GetProtocol().RequestCounter, packet, packetSize);
    MORPHO_AddDataToPacket(packet, packetSize, data, dataSize);
    MORPHO_AddEOP(packet, packetSize);

    return MORPHO_OK;
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
    uint8_t err = value[0];
    if(ilvStatus) *ilvStatus = err;

    if(err == ILV_OK)
	{
        if(valueSize>1)
        {
            uint8_t status = value[1];
            if(baseStatus) *baseStatus = status;

            if(status != ILVSTS_OK)
                return MORPHO_WARN_VAL_ILV_STATUS;
        }

        if(valueSize>5)
            *userIndex = value[2] + (value[3] << 8) + (value[4] << 16) + (value[5] << 24);
	}
	else
        return MORPHO_WARN_VAL_ILV_ERROR;

	return MORPHO_OK;
}
