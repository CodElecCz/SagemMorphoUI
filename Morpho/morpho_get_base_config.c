#include "morpho_get_base_config.h"
#include "morpho_definitions.h"
#include "morpho_protocol.h"

#include "Ilv_definitions.h"
#include "Ilv_crc.h"
#include "Ilv_errors.h"

#include <stddef.h>
#include <string.h>
#include <stdio.h>

extern uint8_t 	RequestCounter;

void MORPHO_GetBaseConfig_Request(uint8_t* packet, size_t* packetSize)
{
    uint8_t data[4];
    size_t dataSize = 0;

    //DATA 4b
	//ILV - Identifier 1b/Length 2b/Value
    data[dataSize++] = ILV_GET_BASE_CONFIG;
    data[dataSize++] = sizeof(uint8_t);
    data[dataSize++] = 0;
    data[dataSize++] = 0;

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
		07 3B 00   			- ILV_GET_DESCRIPTOR + 0x003B total length 59B
		00  				- status ILV_OK/ILVERR_BADPARAMETER/ILVERR_ERROR
		02					- Number fingers/record
		F4 01 00 00			- Max record number (500)
		04 00 00 00 		- Current Record Number
		F0 01 00 00			- Free Record Number
		02 00 00 00			- Number of Fields		
		12 0C 00 ...		- ILV: ID_TIMESTAMP,  Length value 0x000C
        0F 08 00 0f 00 ...	- ILV: ID_PUBLIC_FIELD, Length value 0x0008, Field size 2 bytes, Field name 6 bytes
        46 69 72 73 74 00   - First
        4c 61 73 74 00 00   - Last
        38 01 00 00         - ILV: Biometric Algorithm Param 00 - regular, 01 - norm

		5C D5 1B 03			- CrcL + CrcH + DLE + ETX

	confirm:
	ACK:  02 62 00			- STX + ID + RC
*/

int MORPHO_GetBaseConfig_Response(const uint8_t* value, size_t valueSize, uint8_t* ilvStatus, SMorpho_GetBaseConfig *params)
{
	if(valueSize==0)
		return MORPHO_WARN_VAL_NO_DATA;

	//all data collected
	uint8_t status = value[0];
	if(ilvStatus) *ilvStatus = status;

	if(status == ILV_OK)
	{
        params->fingerNb = value[1];
        params->recNbMax = value[2] + ((uint32_t)value[3] << 8) + ((uint32_t)value[4] << 16) + ((uint32_t)value[5] << 24);
        params->recNb = value[6] + ((uint32_t)value[7] << 8) + ((uint32_t)value[8] << 16) + ((uint32_t)value[9] << 24);
        params->recNbFree = value[10] + ((uint32_t)value[11] << 8) + ((uint32_t)value[12] << 16) + ((uint32_t)value[13] << 24);
        params->fieldNb = value[14] + ((uint32_t)value[15] << 8) + ((uint32_t)value[16] << 16) + ((uint32_t)value[17] << 24);

        int index = 18;
        int fieldIndex = 0;
        do
        {
            uint8_t desc = value[index];
            uint16_t size = value[index + 1] + ((uint16_t)value[index + 2] << 8);
            index += 3;

            switch(desc)
            {
            case ID_TIMESTAMP:
                break;
            case ID_PUBLIC_FIELD:
                params->fieldDescription[fieldIndex].size = value[index] + ((uint16_t)value[index + 1] << 8);
                params->fieldDescription[fieldIndex].name = (const char*)&value[index + 2];
                fieldIndex++;
                break;
            default:
                break;
            }

            index += size;

        }while(index < valueSize);
	}
	else
        return MORPHO_WARN_VAL_ILV_ERROR;

	return MORPHO_OK;
}
