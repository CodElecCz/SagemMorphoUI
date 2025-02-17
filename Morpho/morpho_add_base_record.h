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

typedef struct
{
    const uint8_t* data[20];
    uint8_t dataSize;

    //Field Size: Define the maximum size (in bytes) of a record. It cannot exceed 128 bytes.
    uint8_t fieldSize;
} SMorpho_AddBaseRecord_UserData;

typedef struct
{
    uint8_t baseStatus;
    uint32_t userIndex;
} SMorpho_AddBaseRecord;

int MORPHO_AddBaseRecord_Request(uint8_t* packet, size_t* packetSize,
                                 const uint8_t tmplate[], size_t tmplateSize, uint8_t tmplateId,
                                 const char* userId,
                                 SMorpho_AddBaseRecord_UserData userData,
                                 uint8_t no_check);

int MORPHO_AddBaseRecord_Response(const uint8_t* data, size_t dataSize, uint8_t* ilvStatus, SMorpho_AddBaseRecord* rec);

#ifdef __cplusplus
}
#endif

#endif /* INC_MORPHO_ADD_BASE_RECORD_H_ */
