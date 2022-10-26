#include "morpho_asynchronous_message.h"
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
*/

#define COMMAND_CMD_SIZE    9
static const char* COMMAND_CMD[COMMAND_CMD_SIZE + 1] =
{
    "MORPHO_MOVE_NO_FINGER",
    "MORPHO_MOVE_FINGER_UP",
    "MORPHO_MOVE_FINGER_DOWN",
    "MORPHO_MOVE_FINGER_LEFT",
    "MORPHO_MOVE_FINGER_RIGHT",
    "MORPHO_PRESS_FINGER_HARDER",
    "MORPHO_LATENT",
    "MORPHO_REMOVE_FINGER",
    "MORPHO_FINGER_OK",
    "MORPHO_UNKNOWN"
};

int MORPHO_AsynMeassage_Response(const uint8_t* value, size_t valueSize, uint8_t* ilvStatus, const char** msg)
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
            case ID_MESSAGE_COMMAND_CMD:
                {
                    uint8_t command = value[index];
                    if(command < COMMAND_CMD_SIZE)
                    {
                        *msg = COMMAND_CMD[command];
                    }
                    else
                        *msg = COMMAND_CMD[COMMAND_CMD_SIZE];
                }
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
