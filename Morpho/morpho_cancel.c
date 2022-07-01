#include "morpho_cancel.h"
#include "morpho_definitions.h"
#include "morpho_protocol.h"

#include "Ilv_definitions.h"
#include "Ilv_crc.h"
#include "Ilv_errors.h"

#include <stddef.h>
#include <string.h>
#include <stdio.h>

extern uint8_t 	RequestCounter;

void MORPHO_Cancel_Request(uint8_t* packet, size_t* packetSize)
{
    uint8_t data[3];
    size_t dataSize = 0;

	//DATA
	//ILV - Identifier 1b/Length 2b/Value
    data[dataSize++] = ILV_CANCEL;
    data[dataSize++] = 0;
    data[dataSize++] = 0;

    MORPHO_MakeSOP(PACKED_ID_TYPE_DATA, 1, 1, RequestCounter, packet, packetSize);
    MORPHO_AddDataToPacket(packet, packetSize, data, dataSize);
    MORPHO_AddEOP(packet, packetSize);
}

/*
Cancel Response

	NACK: 02 E4 00			- STX + ID + RC

	ACK:  02 E2 00			- STX + ID + RC

*/


