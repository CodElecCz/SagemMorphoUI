#include "morpho_create_base.h"
#include "morpho_definitions.h"
#include "morpho_protocol.h"

#include "Ilv_definitions.h"
#include "Ilv_crc.h"
#include "Ilv_errors.h"

#include <stddef.h>
#include <string.h>
#include <stdio.h>

void MORPHO_CreateBase_Request(uint8_t* packet, size_t* packetSize, uint16_t maxNbOfRec, SMorpho_CreateBase_UserData userData)
{
    uint8_t data[256];
    size_t dataSize = 0;

    uint8_t maxNbOfFp = 2;

	//DATA
	//ILV - Identifier 1b/Length 2b/Value
    data[dataSize++] = ILV_CREATE_DB;
    data[dataSize++] = 0;
    data[dataSize++] = 0;
    data[dataSize++] = 0;	//Database identifier
    data[dataSize++] = 0;	//RFU

    data[dataSize++] = 0xff & maxNbOfRec;
    data[dataSize++] = 0xff & (maxNbOfRec >> 8);

    data[dataSize++] = maxNbOfFp;

    for(uint8_t i = 0; i < userData.fieldCount; i++)
	{
		//ILV - Identifier 1b/Length 2b/Value
        data[dataSize++] = ID_PUBLIC_FIELD;
        data[dataSize++] = 0x08;
        data[dataSize++] = 0;

        //Field Size: Define the maximum size (in bytes) of a record. It cannot exceed 128 bytes.
        data[dataSize++] = userData.fieldSize;
        data[dataSize++] = 0;

        //Field Name: String specifying the field name. The size of this string must be equal to 6.
        uint8_t fieldNameSize = (strlen(userData.fieldName[i]) > 6)? 6 : (uint8_t)strlen(userData.fieldName[i]);
        memcpy(&data[dataSize], userData.fieldName[i], fieldNameSize);
        dataSize += fieldNameSize;

        for(uint8_t j = fieldNameSize; j < 6; j++)
            data[dataSize++] = 0;
	}

    //ILV - Identifier 1b/Length 2b/Value
    data[dataSize++] = ID_BIO_ALGO_PARAM;			//Biometric Algorithm Parameter
    data[dataSize++] = 1;
    data[dataSize++] = 0;
    data[dataSize++] = 0;

	//recalculate size
    data[1] = 0xff & (dataSize - 3);
    data[2] = 0xff & ((dataSize - 3) >> 8);

    MORPHO_MakeSOP(PACKED_ID_TYPE_DATA, 1, 1, MORPHO_GetProtocol().RequestCounter, packet, packetSize);
    MORPHO_AddDataToPacket(packet, packetSize, data, dataSize);
    MORPHO_AddEOP(packet, packetSize);
}

/*
GetBaseConfig Response

	NACK: 02 E4 00			- STX + ID + RC

	ACK:  02 E2 00			- STX + ID + RC
	response:
		02 E1 00 			- STX + ID + RC
		35 06 00   			- ILV_ADD_RECORD + 0x0006 total length 6B
		00  				- status ILV_OK/ILVERR_BADPARAMETER/ILVERR_ERROR

		5C D5 1B 03			- CrcL + CrcH + DLE + ETX

	confirm:
	ACK:  02 62 00			- STX + ID + RC
*/

int MORPHO_CreateBase_Response(const uint8_t* value, size_t valueSize, uint8_t* ilvStatus)
{
	if(valueSize==0)
		return MORPHO_WARN_VAL_NO_DATA;

	//all data collected
    uint8_t err = value[0];
    if(ilvStatus) *ilvStatus = err;

    if(err == ILV_OK)
	{

	}
	else
        return MORPHO_WARN_VAL_ILV_ERROR;

	return MORPHO_OK;
}
