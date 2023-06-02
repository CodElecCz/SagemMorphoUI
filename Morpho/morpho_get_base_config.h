/*
 * morpho.h
 *
 *  Created on: 20.05.2022
 *      Author: radomir.turca@codelec.cz
 */

#ifndef INC_MORPHO_GET_BASE_CONFIG_H_
#define INC_MORPHO_GET_BASE_CONFIG_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SMorpho_GetBaseConfig_fieldDescription_Size    16

typedef struct
{
    uint16_t size;
    const char* name;
} SMorpho_PublicField;

typedef struct
{
    uint8_t fingerNb;
    uint32_t recNbMax;
    uint32_t recNb;
    uint32_t recNbFree;
    uint32_t fieldNb;
    SMorpho_PublicField fieldDescription[SMorpho_GetBaseConfig_fieldDescription_Size];
} SMorpho_GetBaseConfig;

void MORPHO_GetBaseConfig_Request(uint8_t* packet, size_t* packetSize);
int MORPHO_GetBaseConfig_Response(const uint8_t* data, size_t dataSize, uint8_t* ilvStatus, SMorpho_GetBaseConfig* params);

#ifdef __cplusplus
}
#endif

#endif /* INC_MORPHO_GET_BASE_CONFIG_H_ */
