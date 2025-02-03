/*
 * morpho.h
 *
 *  Created on: 20.05.2022
 *      Author: radomir.turca@codelec.cz
 */

#ifndef INC_MORPHO_ADD_BASE_RECORD_H_
#define INC_MORPHO_ADD_BASE_RECORD_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int MORPHO_AddBaseRecord_Request(uint8_t* packet, size_t* packetSize,
                                 const uint8_t tmplate[], size_t tmplateSize, uint8_t tmplateId,
                                 const char* userId,
                                 const char* userData[], size_t userDataSize, uint8_t userDataFieldSize,
                                 uint8_t no_check);

int MORPHO_AddBaseRecord_Response(const uint8_t* data, size_t dataSize, uint8_t* ilvStatus, uint8_t* baseStatus, uint32_t* userIndex);

#ifdef __cplusplus
}
#endif

#endif /* INC_MORPHO_ADD_BASE_RECORD_H_ */
