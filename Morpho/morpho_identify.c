#include "morpho_identify.h"
#include "morpho_definitions.h"
#include "morpho_protocol.h"

#include "Ilv_definitions.h"
#include "Ilv_crc.h"
#include "Ilv_errors.h"

#include <stddef.h>
#include <string.h>
#include <stdio.h>

extern uint8_t 	RequestCounter;

void MORPHO_Identify_Request(uint8_t* packet, size_t* packetSize, uint16_t timeout, uint16_t threshold, uint16_t keepAlive)
{
    uint8_t data[24];
    size_t dataSize = 0;

	//DATA
	//ILV - Identifier 1b/Length 2b/Value
    data[dataSize++] = ILV_IDENTIFY;
    data[dataSize++] = 0;
    data[dataSize++] = 0;
    data[dataSize++] = 0;	//Database identifier

    data[dataSize++] = 0xff & timeout;
    data[dataSize++] = 0xff & (timeout >> 8);

    data[dataSize++] = 0xff & threshold;
    data[dataSize++] = 0xff & (threshold >> 8);

    data[dataSize++] = 0;	//RFU

    if(keepAlive > 0)
    {
        //0x01 Finger position messages
        //0x02 Live low-resolution images, and acquired fingerprint image (normal resolution)
        //0x04 Enrolment step messages (ENROLL command only)
        //0x08 Image of the acquired fingerprint only.
        //0x40 Quality of the acquired fingerprint.
        //0x80 Live quality of the detected fingerprint.
    	data[dataSize++] = ID_ASYNCHRONOUS_EVENT;
        data[dataSize++] = 0x4;
        data[dataSize++] = 0;
        data[dataSize++] = 0x01;
		data[dataSize++] = 0;
		data[dataSize++] = 0;
		data[dataSize++] = 0;

		data[dataSize++] = ID_ALIVE_MESSAGE_TIME;
        data[dataSize++] = 0x4;
        data[dataSize++] = 0;
        data[dataSize++] = 0xff & keepAlive;
        data[dataSize++] = 0xff & (keepAlive >> 8);
		data[dataSize++] = 0;
		data[dataSize++] = 0;
    }

	//recalculate size
    data[1] = 0xff & (dataSize - 3);
    data[2] = 0xff & ((dataSize - 3) >> 8);

    MORPHO_MakeSOP(PACKED_ID_TYPE_DATA, 1, 1, RequestCounter, packet, packetSize);
    MORPHO_AddDataToPacket(packet, packetSize, data, dataSize);
    MORPHO_AddEOP(packet, packetSize);
}

/*
Identify Response

	NACK: 02 E4 00			- STX + ID + RC

	ACK:  02 E2 00			- STX + ID + RC
	response:
		02 E1 00 			- STX + ID + RC
        22 26 00   			- ILV_ADD_RECORD + 0x0026 total length 26B
		00  				- status ILV_OK/ILVERR_BADPARAMETER/ILVERR_ERROR
        01                  - matching result ILVSTS_HIT
        00 00 00 00         - User Index
        4A 0A 00 ..         - ILV: User Id
        14 05 00 ..         - ILV: Additional user data field 1
        14 08 00 ..         - ILV: Additional user data field 2

		5C D5 1B 03			- CrcL + CrcH + DLE + ETX

	confirm:
	ACK:  02 62 00			- STX + ID + RC
*/

int MORPHO_Identify_Response(const uint8_t* value, size_t valueSize, uint8_t* ilvStatus, uint8_t *baseStatus, uint32_t* userIndex, const char **userId)
{
	if(valueSize==0)
		return MORPHO_WARN_VAL_NO_DATA;

	//all data collected
    uint8_t err = value[0];
    if(ilvStatus) *ilvStatus = err;

    if(err == ILV_OK)
	{
        if(valueSize > 1)
        {
            uint8_t status = value[1];
            if(baseStatus)
                *baseStatus = status;

            if(status != ILVSTS_HIT)
                return MORPHO_WARN_VAL_ILV_STATUS;

            if(valueSize > 5)
            {
                if(userIndex)
                    *userIndex = value[2] + (value[3] << 8) + (value[4] << 16) + (value[5] << 24);
            }

            if(valueSize > 9 && value[6] == ID_USER_ID)
            {
            	uint8_t length = value[7] + (value[8] << 8);
				if(userId)
					*userId = (const char*)&value[9];
            }
        }
	}
	else
        return MORPHO_WARN_VAL_ILV_ERROR;

	return MORPHO_OK;
}
