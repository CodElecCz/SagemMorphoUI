#include "morpho.h"
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

uint8_t 				RequestCounter = 0;
static uint16_t 		ResponseCounter = 0;


void MORPHO_ResponseAck(uint8_t* packed, size_t* dataSize)
{
	size_t packedSize = 0;

	//STX 1b
	packed[packedSize++] = STX;

	//ID 1b - PACKET IDENTIFIER
	packed[packedSize++] = PACKED_ID_TYPE_ACK|PACKED_ID_FLAG_FIRST|PACKED_ID_FLAG_LAST; //0x62;

    //RC 1b
	packed[packedSize++] = ResponseCounter & 0xff;

    if(dataSize) *dataSize = packedSize;
}

int MORPHO_CheckResponse(const uint8_t* data, size_t dataSize, const uint8_t** response, size_t* responseSize)
{
	if(dataSize < 3)
		return MORPHO_ERR_RESPONSE_LENGTH;

	if(data[0] != STX)
		return MORPHO_ERR_RESPONSE_STX;

	if(data[2] != (RequestCounter - 1))
		return MORPHO_ERR_RESPONSE_RC;

	if(data[1]&PACKED_ID_TYPE_NACK)
		return MORPHO_ERR_RESPONSE_NACK;

	if(data[1]&PACKED_ID_TYPE_ACK)
	{
		if(dataSize >= (3 + 10))
		{
			*response = &data[3];
			*responseSize = dataSize - 3;
		}
	}

	return MORPHO_OK;
}

int MORPHO_CheckResponseData(const uint8_t* data, size_t dataSize, const uint8_t** value, size_t* valueSize)
{
	if(dataSize < 10)
		return MORPHO_ERR_DATA_LENGTH;

	if(data[0] != STX)
		return MORPHO_ERR_DATA_STX;

	if((data[1]&PACKED_ID_TYPE_DATA) == 0)
		return MORPHO_ERR_DATA_FLAG;

	ResponseCounter = data[2];
	uint32_t lengthIndex = 4;

	//TODO: issue, sometimes 2bytes instead of one
#if 0
	if(data[3] != Request)
	{
		if(data[4] != Request)
			return MORPHO_ERR_REQUEST_ID;

		ResponseCounter = data[2] + (data[3] << 8);
		lengthIndex++;
	}
#endif
	uint16_t length = data[lengthIndex] + (data[lengthIndex + 1] << 8);

	if(length + 10 > dataSize)
		return MORPHO_ERR_VAL_LENGTH;

#if 0
	uint8_t CrcL = data[6 + length];
	uint8_t CrcH = data[6 + length + 1];

	if(data[6 + length + 2] != DLE)
		return MORPHO_ERR_DATA_DLE;

	if(data[6 + length + 3] != ETX)
		return MORPHO_ERR_DATA_ETX;
#else //index from end - data length issue
	uint8_t CrcL = data[dataSize - 4];
	uint8_t CrcH = data[dataSize - 3];

	if(data[dataSize - 2] != DLE)
		return MORPHO_ERR_DATA_DLE;

	if(data[dataSize - 1] != ETX)
		return MORPHO_ERR_DATA_ETX;
#endif
	*value = &data[lengthIndex + 2];
	*valueSize = dataSize - 10;

	return MORPHO_OK;
}
