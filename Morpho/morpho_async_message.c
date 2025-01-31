#include "morpho_async_message.h"
#include "morpho_definitions.h"
#include "morpho_protocol.h"

#include "Ilv_definitions.h"
#include "Ilv_crc.h"
#include "Ilv_errors.h"

#include <stddef.h>
#include <string.h>
#include <stdio.h>

/*
AsynMessage Response

	response:		
        71 08 00   			- ILV_ADD_RECORD + 0x0008 total length 8B
        00  				- status ILV_OK/ILVERR_ERROR


		5C D5 1B 03			- CrcL + CrcH + DLE + ETX

	confirm:
	ACK:  02 62 00			- STX + ID + RC

	Identify
	MORPHO Data [0x02 0xE1 0xA1 0x71 0x08 0x00 0x00 0x01 0x04 0x00 0x00 0x00 0x00 0x00 0x50 0x97 0x1B 0x03 ]
	MORPHO Data [0x02 0xE1 0xC7 0x71 0x08 0x00 0x00 0x01 0x04 0x00 0x09 0x00 0x00 0x00 0x27 0x64 0x1B 0x03 ] MORPHO_UNKNOWN
	MORPHO Data [0x02 0xE1 0xC8 0x71 0x08 0x00 0x00 0x01 0x04 0x00 0x08 0x00 0x00 0x00 0x93 0x12 0x1B 0x03 ] MORPHO_FINGER_OK

	TemplateAdd 10k
	MORPHO Data [0x02 0xE1 0x5F 0x71 0x04 0x00 0x00 0x06 0x00 0x00 0x01 0x88 0x1B 0x03 ]
*/

#define FINGER_CMD_SIZE    10
static const char* FINGER_CMD[FINGER_CMD_SIZE + 1] =
{
    "NO_FINGER",
    "MOVE_FINGER_UP",
    "MOVE_FINGER_DOWN",
    "MOVE_FINGER_LEFT",
    "MOVE_FINGER_RIGHT",
    "PRESS_HARDER",
    "LATENT",
    "REMOVE_FINGER",
    "FINGER_OK",
    "FINGER_DETECTED",
	"UNKNOWN"
};

int MORPHO_AsyncMeassage_Response(const uint8_t* value, size_t valueSize, uint8_t* ilvStatus, const char** msg)
{
	if(valueSize==0)
		return MORPHO_WARN_VAL_NO_DATA;

	//all data collected
    uint8_t err = value[0];
    if(ilvStatus) *ilvStatus = err;

    if(err == ILV_OK)
	{
        uint16_t index = 1;

        do
        {
            uint8_t message = value[index];
            uint16_t size = value[index + 1] + (value[index + 2] << 8);
            index += 3;

            switch(message)
            {
            case ID_MESSAGE_FINGER_CMD:
                {
                    uint8_t command = value[index];
                    if(command < FINGER_CMD_SIZE)
                    {
                        *msg = FINGER_CMD[command];
                    }
                    else
                        *msg = FINGER_CMD[FINGER_CMD_SIZE];
                }
                break;
            case ID_MESSAGE_BUSY_WARNING:
            	*msg = "BUSY_WARNING";
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
