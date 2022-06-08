/*
 * morpho.h
 *
 *  Created on: 20.05.2022
 *      Author: radomir.turca@codelec.cz
 */

#ifndef INC_MORPHO_GET_DATA_H_
#define INC_MORPHO_GET_DATA_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void MORPHO_GetData_Request(uint8_t* data, size_t* dataSize, uint32_t fieldIndex, uint32_t userIndex);
int MORPHO_GetData_Response(const uint8_t* data, size_t dataSize, uint8_t* ilvStatus, const char** usedData);

#ifdef __cplusplus
}
#endif

#endif /* INC_MORPHO_GET_DATA_H_ */
