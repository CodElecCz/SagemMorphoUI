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

static SMorphoProtocol morhpoProtocol = {0, 0};

void MORPHO_ResetCounter(void)
{
    morhpoProtocol.RequestCounter = 0;
    morhpoProtocol.ResponseCounter = 0;
}

void MORPHO_SetCounter(uint8_t requestCounter)
{
    morhpoProtocol.RequestCounter = requestCounter;
}

uint8_t MORPHO_GetCounter(void)
{
    return morhpoProtocol.RequestCounter;
}

SMorphoProtocol MORPHO_GetProtocol(void)
{
    return morhpoProtocol;
}

void MORPHO_ResponseAck(uint8_t* packet, size_t* packetSize)
{
    MORPHO_MakeSOP(PACKED_ID_TYPE_ACK, 1, 1, morhpoProtocol.ResponseCounter, packet, packetSize);
    //RequestCounter += 1;
}

void MORPHO_ResponseNack(uint8_t* packet, size_t* packetSize)
{
    MORPHO_MakeSOP(PACKED_ID_TYPE_NACK, 1, 1, morhpoProtocol.ResponseCounter, packet, packetSize);
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

int MORPHO_ReceiveSOP(const uint8_t* packet, size_t packetSize, uint8_t* RC, uint8_t* flag, size_t* sopSize, uint8_t isData)
{
    if(packetSize < 3)
		return MORPHO_ERR_RESPONSE_LENGTH;

    if(packet[0] != STX)
		return MORPHO_ERR_RESPONSE_STX;

    if(!isData && packet[1]&PACKED_ID_TYPE_DATA)
        return MORPHO_ERR_RESPONSE_DATA;
    else if(packet[1] & PACKED_ID_TYPE_ACK || packet[1]&PACKED_ID_TYPE_NACK)
        morhpoProtocol.RequestCounter += 1;

    if(packet[2] != DLE)
    {
        if(RC != NULL)
            *RC = packet[2];
        *sopSize = 3;
    }
    else //stuffed
    {
        if(packetSize < 4)
            return MORPHO_ERR_RESPONSE_LENGTH;

        uint8_t rc = packet[3];
        MORPHO_UnStuffing(&rc);
        if(RC != NULL)
            *RC = rc;
        *sopSize = 4;
    }

    if(packet[1]&PACKED_ID_TYPE_NACK)
        return MORPHO_ERR_RESPONSE_NACK;

    if(flag)
    {
        *flag = (packet[1]&(PACKED_ID_FLAG_FIRST | PACKED_ID_FLAG_LAST));
    }

	return MORPHO_OK;
}

static const uint16_t CRC_CCITT_TABLE[256] =
{
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
    0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
    0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
    0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
    0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
    0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
    0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
    0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
    0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
    0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
    0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
    0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
    0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
    0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
    0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
    0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
    0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
    0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
    0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
    0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
    0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
    0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
    0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
};

static uint16_t Calculate_CRC_CCITT(const unsigned char* buffer, int size)
{
    uint16_t tmp;
    uint16_t crc = 0x0000;

    for (int i=0; i < size ; i++)
    {
        tmp = (crc >> 8) ^ buffer[i];
        crc = (uint16_t)(crc << 8) ^ CRC_CCITT_TABLE[tmp];
    }

    return crc;
}

int MORPHO_ReceiveData(uint8_t* packet, size_t packetSize, uint8_t* identifier, uint8_t** value, size_t* valueSize)
{
    size_t sopSize = 0;
    uint8_t rc = 0;
    uint8_t flag = 0;
    int err = MORPHO_ReceiveSOP(packet, packetSize, &rc, &flag, &sopSize, 1);
    if(err < MORPHO_OK)
        return err;

    morhpoProtocol.ResponseCounter = rc;

    if(packetSize < (sopSize + 4))
        return MORPHO_ERR_RESPONSE_LENGTH;

    if(packet[packetSize - 2] != DLE)
        return MORPHO_ERR_DATA_DLE;

    if(packet[packetSize - 1] != ETX)
        return MORPHO_ERR_DATA_ETX;

    uint8_t* stuffed = &packet[sopSize];
    size_t stuffedSize = packetSize - sopSize - 2;

    MORPHO_UnStuffingData(stuffed, &stuffedSize);

    uint16_t crc = stuffed[stuffedSize - 2] + (stuffed[stuffedSize - 1] << 8);
    uint16_t crc_calc = Calculate_CRC_CCITT(stuffed, stuffedSize-2);
    if(crc != crc_calc)
        return MORPHO_ERR_DATA_CRC;

    uint8_t type = flag >> 5;
    if(type == 2) //first
    {
        //ILV 1b + 2b + data
        uint16_t length = stuffed[1] + (stuffed[2] << 8); //length of all packets
        uint16_t lengthPacket = stuffedSize - 3 - 2;      //3 ILV + 2 CRC

        *identifier = stuffed[0];
        *value = &stuffed[3];
        *valueSize = lengthPacket;

        morhpoProtocol.Identifier = stuffed[0];
        morhpoProtocol.PacketIndex = 0;
        morhpoProtocol.PacketSizeAct = lengthPacket;
        morhpoProtocol.PacketSizeTotal = length;

        return MORPHO_WARN_DATA_CONTINUE;
    }
    else if(type == 0) //middle
    {
        uint16_t lengthPacket = stuffedSize - 2;      //2 CRC

        *identifier = morhpoProtocol.Identifier;
        *value = &stuffed[0];
        *valueSize = lengthPacket;

        morhpoProtocol.PacketIndex++;
        morhpoProtocol.PacketSizeAct += lengthPacket;

        return MORPHO_WARN_DATA_CONTINUE;
    }
    else if(type == 1) //last
    {
        uint16_t lengthPacket = stuffedSize - 2;      //2 CRC

        *identifier = morhpoProtocol.Identifier;
        *value = &stuffed[0];
        *valueSize = lengthPacket;

        morhpoProtocol.PacketIndex++;
        morhpoProtocol.PacketSizeAct += lengthPacket;

        return MORPHO_OK;
    }
    else //full frame type == 3
    {
        //ILV 1b + 2b + data
        uint16_t length = stuffed[1] + (stuffed[2] << 8); //length of all packets
        uint16_t lengthPacket = stuffedSize - 3 - 2;      //3 ILV + 2 CRC

        *identifier = stuffed[0];
        *value = &stuffed[3];
        *valueSize = lengthPacket;

        morhpoProtocol.PacketIndex = 0;
        morhpoProtocol.PacketSizeAct = lengthPacket;
        morhpoProtocol.PacketSizeTotal = length;

        if(length != lengthPacket)
        {
            *valueSize = length;
            return MORPHO_ERR_VAL_LENGTH;
        }

        return MORPHO_OK;
    }
}
