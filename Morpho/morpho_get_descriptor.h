/*
 * morpho.h
 *
 *  Created on: 20.05.2022
 *      Author: radomir.turca@codelec.cz
 */

#ifndef INC_MORPHO_GET_DESCRIPTOR_H_
#define INC_MORPHO_GET_DESCRIPTOR_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void MORPHO_GetDescriptor_Request(uint8_t* data, size_t* dataSize);
int MORPHO_GetDescriptor_Response(const uint8_t* data, size_t dataSize, uint8_t* ilvStatus, const char** product, const char** sensor, const char** software);

#ifdef __cplusplus
}
#endif

#endif /* INC_MORPHO_GET_DESCRIPTOR_H_ */
