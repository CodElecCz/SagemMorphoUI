/*
 * morpho.h
 *
 *  Created on: 20.05.2022
 *      Author: radomir.turca@codelec.cz
 */

#ifndef INC_MORPHO_DEFINITIONS_H_
#define INC_MORPHO_DEFINITIONS_H_

#include "Ilv_definitions.h"

#ifdef __cplusplus
extern "C" {
#endif

enum MorphoRequest
{
	MorphoRequest_None = 0,
	MorphoRequest_GetDescriptor = ILV_GET_DESCRIPTOR,
	MorphoRequest_ConfigureUart = ILV_CONFIG_UART,
	MorphoRequest_GetBaseConfig = ILV_GET_BASE_CONFIG,
	MorphoRequest_GetData       = ILV_GET_DATA,
	MorphoRequest_AddBaseRecord = ILV_ADD_RECORD,
    MorphoRequest_EraseBase     = ILV_ERASE_BASE,
};

#define DLE						0x1B
#define STX						0x02
#define ETX						0x03
#define XON						0x11
#define XOFF					0x13

/*
 * ID
 * 0x61 Single Data Packet (the whole original Data is stored in the Packet)
 * 0x41 First Data Packet : contains the first bytes of a segmented Data
 * 0x01 Intermediate Data Packet : contains a part of a segmented Data
 * 0x21 Last Data Packet : contains the last bytes of a segmented Data
 *
 */
#define PACKED_ID_TYPE_DATA		0x01
#define PACKED_ID_TYPE_ACK		0x02
#define PACKED_ID_TYPE_NACK		0x04
#define PACKED_ID_TYPE_RFU		0x08
#define PACKED_ID_MBZ			0x10	/* reserved - Must be ZERO to avoid ID to be equal to DLE */
#define PACKED_ID_FLAG_LAST		0x20
#define PACKED_ID_FLAG_FIRST	0x40
#define PACKED_ID_FLAG_DIR		0x80

#define PACKED_ID_MASK_TYPE		(PACKED_ID_TYPE_DATA|PACKED_ID_TYPE_ACK|PACKED_ID_TYPE_NACK|PACKED_ID_TYPE_RFU|PACKED_ID_MBZ)

/*
 * MORPHO
 */
#define MORPHO_WARN_VAL_NO_DATA		3
#define MORPHO_WARN_VAL_ILV_STATUS	2
#define MORPHO_WARN_VAL_ILV_ERROR	1
#define MORPHO_OK					0
#define MORPHO_ERR_RESPONSE_LENGTH	-1
#define MORPHO_ERR_RESPONSE_STX		-2
#define MORPHO_ERR_RESPONSE_RC		-3
#define MORPHO_ERR_RESPONSE_NACK	-4
#define MORPHO_ERR_RESPONSE_DATA	-5
#define MORPHO_ERR_DATA_LENGTH		-6
#define MORPHO_ERR_DATA_STX			-7
#define MORPHO_ERR_DATA_FLAG		-8
#define MORPHO_ERR_REQUEST_ID		-9
#define MORPHO_ERR_DATA_CRC			-10
#define MORPHO_ERR_DATA_DLE			-11
#define MORPHO_ERR_DATA_ETX			-12
#define MORPHO_ERR_VAL_LENGTH		-13

#ifdef __cplusplus
}
#endif

#endif /* INC_MORPHO_DEFINITIONS_H_ */
