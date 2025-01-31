/*
 * morpho.h
 *
 *  Created on: 07.08.2023
 *      Author: radomir.turca@codelec.cz
 */

#ifndef INC_MORPHO_GET_PULBIC_FIELDS_H_
#define INC_MORPHO_GET_PULBIC_FIELDS_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void MORPHO_GetPublicFields_Request(uint8_t* data, size_t* dataSize, uint32_t fieldIndex);
int MORPHO_GetPublicFields_Response(const uint8_t* data, size_t dataSize, uint8_t* ilvStatus, uint32_t* userNumber);

#ifdef __cplusplus
}
#endif

#endif /* INC_MORPHO_GET_PULBIC_FIELDS_H_ */
