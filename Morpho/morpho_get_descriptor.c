#include "morpho_get_descriptor.h"
#include "morpho_definitions.h"
#include "morpho_protocol.h"

#include "Ilv_definitions.h"

#include <stddef.h>
#include <string.h>
#include <stdio.h>

void MORPHO_GetDescriptorText_Request(uint8_t* packet, size_t* packetSize)
{    
    uint8_t data[4];
    size_t dataSize = 0;

    //DATA 4b
	//ILV - Identifier 1b/Length 2b/Value
    data[dataSize++] = ILV_GET_DESCRIPTOR;
    data[dataSize++] = sizeof(uint8_t);
    data[dataSize++] = 0;
    data[dataSize++] = ID_FORMAT_TEXT;

    MORPHO_MakeSOP(PACKED_ID_TYPE_DATA, 1, 1, MORPHO_GetProtocol().RequestCounter, packet, packetSize);
    MORPHO_AddDataToPacket(packet, packetSize, data, dataSize);
    MORPHO_AddEOP(packet, packetSize);
}

void MORPHO_GetDescriptorBinVer_Request(uint8_t* packet, size_t* packetSize)
{
    uint8_t data[4];
    size_t dataSize = 0;

    //DATA 4b
	//ILV - Identifier 1b/Length 2b/Value
    data[dataSize++] = ILV_GET_DESCRIPTOR;
    data[dataSize++] = sizeof(uint8_t);
    data[dataSize++] = 0;
    data[dataSize++] = ID_FORMAT_BIN;

    MORPHO_MakeSOP(PACKED_ID_TYPE_DATA, 1, 1, MORPHO_GetProtocol().RequestCounter, packet, packetSize);
    MORPHO_AddDataToPacket(packet, packetSize, data, dataSize);
    MORPHO_AddEOP(packet, packetSize);
}

void MORPHO_GetDescriptorBinMaxUser_Request(uint8_t* packet, size_t* packetSize)
{
    uint8_t data[4];
    size_t dataSize = 0;

    //DATA 4b
	//ILV - Identifier 1b/Length 2b/Value
    data[dataSize++] = ILV_GET_DESCRIPTOR;
    data[dataSize++] = sizeof(uint8_t);
    data[dataSize++] = 0;
    data[dataSize++] = ID_FORMAT_BIN_MAX_USER;

    MORPHO_MakeSOP(PACKED_ID_TYPE_DATA, 1, 1, MORPHO_GetProtocol().RequestCounter, packet, packetSize);
    MORPHO_AddDataToPacket(packet, packetSize, data, dataSize);
    MORPHO_AddEOP(packet, packetSize);
}

/*
GetDescriptor Response

	NACK: 02 E4 00			- STX + ID + RC

	ACK:  02 E2 00			- STX + ID + RC
	response:
		02 E1 00 			- STX + ID + RC
		05 EC 00   			- ILV_GET_DESCRIPTOR + 0x00EC total length 236b
		00  				- status ILV_OK/ILVERR_BADPARAMETER/ILVERR_ERROR
		29                  - ID_DESC_PRODUCT
		A1 00 ..			- str 161b
		2B					- ID_DESC_SENSOR
		2A 00 ..			- str 42b
		2A					- ID_DESC_SOFTWARE
		17 00 ..			- str 23b

		total length: 1 + (3+161) + (3+42) + (3+23) = 236b

		95 93 1B 03			- CrcL + CrcH + DLE + ETX

	confirm:
	ACK:  02 62 00			- STX + ID + RC

	ID_DESC_PRODUCT / ID_DESC_SENSOR / ID_DESC_SOFTWARE

	Product Name:..CBM\r\n
	Mobi5 Serial Number:.1009C742778\r\n
	MSO OEM Serial Number:.1014N004016\r\n
	MSO OEM Product ID:.251606824\r\n
	Total flash size :..1048576 o.1024 Ko\r\n

	ProductId:.00000000-0\r\n
	SerialNumber:.0000

	MSO Version: 09.00.c-C
*/

int MORPHO_GetDescriptorText_Response(const uint8_t* value, size_t valueSize, uint8_t* ilvStatus, const char** product, const char** sensor, const char** software)
{
	if(valueSize==0)
		return MORPHO_WARN_VAL_NO_DATA;

	//all data collected
	uint8_t status = value[0];
	if(ilvStatus) *ilvStatus = status;

	if(status == ILV_OK)
	{
        int index = 1;

		do
		{
            uint8_t desc = value[index];
            uint16_t size = value[index + 1] + (value[index + 2] << 8);
            index += 3;

			switch(desc)
			{
			case ID_DESC_PRODUCT:
				*product = (const char*)&value[index];
				break;
			case ID_DESC_SENSOR:
				*sensor = (const char*)&value[index];
				break;
			case ID_DESC_SOFTWARE:
				*software = (const char*)&value[index];
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

int MORPHO_GetDescriptor_Response(const uint8_t* value, size_t valueSize, uint8_t* ilvStatus, uint8_t* type)
{
	if(valueSize==0)
		return MORPHO_WARN_VAL_NO_DATA;

	//all data collected
	uint8_t status = value[0];
	if(ilvStatus) *ilvStatus = status;

	if(status == ILV_OK)
	{
		switch(value[1])
		{
		case ID_FORMAT_BIN:
			if(type) *type = 1;
			break;
		case ID_FORMAT_BIN_MAX_USER:
			if(type) *type = 2;
			break;
		default:
			if(type) *type = 0;
			break;
		}
	}
	else
		return MORPHO_WARN_VAL_ILV_ERROR;

	return MORPHO_OK;
}

int MORPHO_GetDescriptorBinVer_Response(const uint8_t* value, size_t valueSize, uint8_t* ilvStatus, const char** version, uint16_t* versionSize)
{
	if(valueSize==0)
		return MORPHO_WARN_VAL_NO_DATA;

	//all data collected
	uint8_t status = value[0];
	if(ilvStatus) *ilvStatus = status;

	if(status == ILV_OK)
	{
		switch(value[1])
		{
		case ID_FORMAT_BIN:
			if(valueSize > 4)
			{
				if(versionSize) *versionSize = value[2] + (value[3] << 8);
				if(version) *version = (const char*)&value[4];
			}
			break;
		}
	}
	else
        return MORPHO_WARN_VAL_ILV_ERROR;

	return MORPHO_OK;
}

int MORPHO_GetDescriptorBinMaxUser_Response(const uint8_t* value, size_t valueSize, uint8_t* ilvStatus, uint16_t* max_user)
{
	if(valueSize==0)
		return MORPHO_WARN_VAL_NO_DATA;

	//all data collected
	uint8_t status = value[0];
	if(ilvStatus) *ilvStatus = status;

	if(status == ILV_OK)
	{
		switch(value[1])
		{
		case ID_FORMAT_BIN_MAX_USER:
			if(valueSize > 5)
			{
				if(max_user) *max_user = value[4] + (value[5] << 8);
			}
			break;
		}
	}
	else
        return MORPHO_WARN_VAL_ILV_ERROR;

	return MORPHO_OK;
}
