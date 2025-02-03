#include "morpho_get_public_fields.h"
#include "morpho_definitions.h"
#include "morpho_protocol.h"

#include "Ilv_definitions.h"
#include "Ilv_crc.h"
#include "Ilv_errors.h"

#include <stddef.h>
#include <string.h>
#include <stdio.h>

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
        3E 44 00   			- ILV_GET_DATA + 0x0044 total length 3B
		00  				- status ILV_OK/ILVERR_BADPARAMETER/ILVERR_ERROR
        03 00 00 00         - nb of fields
        32                  - FIELD CONTENT ID 0x32
            12 00           - size 18
            01 00 00 00     - Field index 4b
            0a 00 00 00     - Data length 4b = 10
            <data>

		5C D5 1B 03			- CrcL + CrcH + DLE + ETX

	confirm:
	ACK:  02 62 00			- STX + ID + RC
*/

int MORPHO_GetPublicFields_Response(const uint8_t* value, size_t valueSize, uint8_t* ilvStatus, SMorpho_GetPublicFields* fields)
{
	if(valueSize==0)
		return MORPHO_WARN_VAL_NO_DATA;

	//all data collected
	uint8_t status = value[0];
	if(ilvStatus) *ilvStatus = status;

	if(status == ILV_OK)
	{
        if(fields != NULL)
        {
            fields->fieldNb = value[1] + (value[2] << 8) + (value[3] << 16) + (value[4] << 24);

            uint16_t fieldIndex = 0;
            for(uint16_t i = 5; i < valueSize; i++)
            {
                if(value[i] == ID_FIELD_CONTENT)
                {
                    uint16_t fieldSize = value[i+1] + (value[i+2] << 8);
                    uint32_t fieldIndex = value[i+3] + (value[i+4] << 8) + (value[i+5] << 16) + (value[i+6] << 24);
                    uint32_t dataSize = value[i+7] + (value[i+8] << 8) + (value[i+9] << 16) + (value[i+10] << 24);

                    fields->fieldDescription[fieldIndex] = &value[i+11];
                    if(fields->fieldSize == 0)
                        fields->fieldSize = dataSize;

                    i += 10 + dataSize;

                    if(SMorpho_GetPublicFields_Size > fieldIndex + 1)
                        fieldIndex++;
                    else
                        break;
                }
                else
                    break;
            }
        }
	}
	else
        return MORPHO_WARN_VAL_ILV_ERROR;

	return MORPHO_OK;
}
