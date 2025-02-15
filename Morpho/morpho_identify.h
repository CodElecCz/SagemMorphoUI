/*
 * morpho.h
 *
 *  Created on: 20.05.2022
 *      Author: radomir.turca@codelec.cz
 */

#ifndef INC_MORPHO_IDENTIFY_H_
#define INC_MORPHO_IDENTIFY_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    uint8_t baseStatus;
    uint32_t userIndex;
    uint16_t userIdSize;
    const char* userId;
} SMorpho_Identify;

void MORPHO_Identify_Request(uint8_t* packet, size_t* packetSize, uint16_t timeout, uint16_t threshold, uint16_t keepAlive);
int MORPHO_Identify_Response(const uint8_t* data, size_t dataSize, uint8_t* ilvStatus, SMorpho_Identify* identify);

#ifdef __cplusplus
}
#endif

#endif /* INC_MORPHO_IDENTIFY_H_ */
