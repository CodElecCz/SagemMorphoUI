/*
 * morpho.h
 *
 *  Created on: 20.05.2022
 *      Author: radomir.turca@codelec.cz
 */

#ifndef INC_MORPHO_H_
#define INC_MORPHO_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int MORPHO_CheckResponse(const uint8_t* data, size_t dataSize, const uint8_t** response, size_t* responseSize);
int MORPHO_CheckResponseData(const uint8_t* data, size_t dataSize, const uint8_t** value, size_t* valueSize);

void MORPHO_ResponseAck(uint8_t* packed, size_t* dataSize);

#ifdef __cplusplus
}
#endif

#endif /* INC_MORPHO_H_ */
