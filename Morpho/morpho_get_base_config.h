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

void MORPHO_GetBaseConfig_Request(uint8_t* packet, size_t* packetSize);
int MORPHO_GetBaseConfig_Response(const uint8_t* data, size_t dataSize, uint8_t* ilvStatus);

#ifdef __cplusplus
}
#endif

#endif /* INC_MORPHO_GET_BASE_CONFIG_H_ */
