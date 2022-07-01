#include "morpho_protocol.h"
#include "Ilv_definitions.h"
#include "Ilv_crc.h"
#include "Ilv_errors.h"

#include "morpho_definitions.h"
#include "morpho_get_descriptor.h"
#include "morpho_get_base_config.h"
#include "morpho_get_data.h"
#include "morpho_add_base_record.h"
#include "morpho_configure_uart.h"

#include <stddef.h>
#include <string.h>
#include <stdio.h>

uint8_t 			RequestCounter = 0;
static uint8_t 		ResponseCounter = 0;

void MORPHO_ResetCounter(void)
{
    RequestCounter = 0;
    ResponseCounter = 0;
}

void MORPHO_ResponseAck(const uint8_t* packet, size_t* packetSize)
{
    MORPHO_MakeSOP(PACKED_ID_TYPE_ACK, 1, 1, ResponseCounter, packet, packetSize);
    //RequestCounter += 1;
}

void MORPHO_ResponseNack(const uint8_t* packet, size_t* packetSize)
{
    MORPHO_MakeSOP(PACKED_ID_TYPE_NACK, 1, 1, ResponseCounter, packet, packetSize);
    //RequestCounter += 1;
}

void MORPHO_MakeSOP(uint8_t Type, uint8_t First, uint8_t Last, uint8_t RC, uint8_t* PacketToSend, size_t* PacketCurrentSize)
{
    uint8_t Byte;

    //  ajout STX
    PacketToSend[0] = STX;

    //  ajout ID
    Byte = Type;
    if (First)
        Byte |= PACKED_ID_FLAG_FIRST;
    if (Last)
        Byte |= PACKED_ID_FLAG_LAST;

    PacketToSend[1] = Byte;

    //  stuffing sur RC et ajout de RC
    switch (RC)
    {
        case DLE:
            PacketToSend[2] = DLE;
            PacketToSend[3] = DLE;
            *PacketCurrentSize = 4;
            break;
        case XON:
            PacketToSend[2] = DLE;
            PacketToSend[3] = XON+1;
            *PacketCurrentSize = 4;
            break;
        case XOFF:
            PacketToSend[2] = DLE;
            PacketToSend[3] = XOFF+1;
            *PacketCurrentSize = 4;
            break;
        default:
            PacketToSend[2] = RC;
            *PacketCurrentSize = 3;
    }
}

void MORPHO_AddDataToPacket(uint8_t* PacketToSend, size_t* PacketCurrentSize, uint8_t* Data, size_t SizeOfSendData)
{
    uint32_t i;
    uint8_t CRC[2] = {0x00,0x00};

    // Calcul du CRC
    IlvCrc16(Data, SizeOfSendData, CRC+1, CRC);

    // Ajout des Data en faisant le stuffing
    for(i = 0; i < SizeOfSendData; i++)
    {
        switch (Data[i])
        {
            case DLE:
                PacketToSend[(*PacketCurrentSize)++] = DLE;
                PacketToSend[(*PacketCurrentSize)++] = DLE;
                break;
            case XON:
                PacketToSend[(*PacketCurrentSize)++] = DLE;
                PacketToSend[(*PacketCurrentSize)++] = XON+1;
                break;
            case XOFF:
                PacketToSend[(*PacketCurrentSize)++] = DLE;
                PacketToSend[(*PacketCurrentSize)++] = XOFF+1;
                break;
            default:
                PacketToSend[(*PacketCurrentSize)++] = Data[i];
                break;
        }
    }

    // Ajout du CRC en faisant le Stuffing
    for(i = 0; i < 2; i++)
    {
        switch (CRC[i])
        {
            case DLE:
                PacketToSend[(*PacketCurrentSize)++] = DLE;
                PacketToSend[(*PacketCurrentSize)++] = DLE;
                break;
            case XON:
                PacketToSend[(*PacketCurrentSize)++] = DLE;
                PacketToSend[(*PacketCurrentSize)++] = XON+1;
                break;
            case XOFF:
                PacketToSend[(*PacketCurrentSize)++] = DLE;
                PacketToSend[(*PacketCurrentSize)++] = XOFF+1;
                break;
            default:
                PacketToSend[(*PacketCurrentSize)++] = CRC[i];
                break;
        }
    }
}

void MORPHO_AddEOP(uint8_t* PacketToSend, size_t* PacketCurrentSize)
{
    PacketToSend[(*PacketCurrentSize)++] = DLE;
    PacketToSend[(*PacketCurrentSize)++] = ETX;
}

static int MORPHO_UnStuffing(uint8_t* Data)
{
    switch (*Data)
    {
        case DLE:
            break;
        case XON+1:
            *Data = XON;	// <XON>
            break;
        case XOFF+1:
            *Data = XOFF;	// <XOFF>
            break;
        default:
            break;
    }

    return MORPHO_OK;
}

static int MORPHO_UnStuffingData(uint8_t* Data, size_t* DataSize)
{
    size_t dataSize = *DataSize;
    uint32_t i;
    uint32_t act = 0;

    for(i = 0; i < dataSize; i++, act++)
    {
        if(Data[i] == DLE)
        {
            i++;
            uint8_t d = Data[i];
            MORPHO_UnStuffing(&d);
            Data[act] = d;
        }
        else
        {
            Data[act] = Data[i];
        }
    }

    *DataSize = act;

    return MORPHO_OK;
}

int MORPHO_ReciveSOP(const uint8_t* packet, size_t packetSize, uint8_t* RC, size_t* sopSize)
{
    if(packetSize < 3)
		return MORPHO_ERR_RESPONSE_LENGTH;

    if(packet[0] != STX)
		return MORPHO_ERR_RESPONSE_STX;

    if(packet[1] & PACKED_ID_TYPE_NACK)
        return MORPHO_ERR_RESPONSE_NACK;
    else if(packet[1] & PACKED_ID_TYPE_ACK)
        RequestCounter += 1;

    if(packet[2] != DLE)
    {
        *RC = packet[2];
        *sopSize = 3;
    }
    else //stuffed
    {
        if(packetSize < 4)
            return MORPHO_ERR_RESPONSE_LENGTH;

        uint8_t rc = packet[3];
        MORPHO_UnStuffing(&rc);
        *RC = rc;
        *sopSize = 4;
    }

	return MORPHO_OK;
}

int MORPHO_ReceiveData(uint8_t* packet, size_t packetSize, uint8_t** value, size_t* valueSize)
{
    size_t sopSize = 0;
    uint8_t rc = 0;
    int err = MORPHO_ReciveSOP(packet, packetSize, &rc, &sopSize);
    if(err < MORPHO_OK)
        return err;

    ResponseCounter = rc;

    if(packetSize < (sopSize + 4))
        return MORPHO_ERR_RESPONSE_LENGTH;

    if(packet[packetSize - 2] != DLE)
        return MORPHO_ERR_DATA_DLE;

    if(packet[packetSize - 1] != ETX)
        return MORPHO_ERR_DATA_ETX;

    uint8_t* stuffed = &packet[sopSize];
    size_t stuffedSize = packetSize - sopSize - 2;

    MORPHO_UnStuffingData(stuffed, &stuffedSize);

    //ILV 1b + 2b + data
    uint16_t length = stuffed[1] + (stuffed[2] << 8);
    if(length != stuffedSize - 3 - 2) //3 ILV + 2 CRC
        return MORPHO_ERR_VAL_LENGTH;

    *value = &stuffed[3];
    *valueSize = stuffedSize - 3 - 2;

	return MORPHO_OK;
}
