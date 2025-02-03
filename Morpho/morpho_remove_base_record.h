/*
 * morpho.h
 *
 *  Created on: 20.05.2022
 *      Author: radomir.turca@codelec.cz
 */

#ifndef INC_MORPHO_REMOVE_BASE_RECORD_H_
#define INC_MORPHO_REMOVE_BASE_RECORD_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void MORPHO_RemoveBaseRecord_Request(uint8_t* data, size_t* dataSize, uint32_t userIndex);
void MORPHO_RemoveBaseRecordId_Request(uint8_t* packet, size_t* packetSize, const char userId[]);
int MORPHO_RemoveBaseRecord_Response(const uint8_t* data, size_t dataSize, uint8_t* ilvStatus);

#ifdef __cplusplus
}
#endif

#endif /* INC_MORPHO_REMOVE_BASE_RECORD_H_ */
