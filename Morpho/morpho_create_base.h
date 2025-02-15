/*
 * morpho.h
 *
 *  Created on: 20.05.2022
 *      Author: radomir.turca@codelec.cz
 */

#ifndef INC_MORPHO_CREATE_BASE_H_
#define INC_MORPHO_CREATE_BASE_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    //Field Name: String specifying the field name. The size of this string must be equal to 6.
    const char* fieldName[20];
    uint8_t fieldCount;

    //Field Size: Define the maximum size (in bytes) of a record. It cannot exceed 128 bytes.
    uint8_t fieldSize;
} SMorpho_CreateBase_UserData;

void MORPHO_CreateBase_Request(uint8_t* packet, size_t* packetSize, uint16_t maxNbOfRec, SMorpho_CreateBase_UserData userData);
int MORPHO_CreateBase_Response(const uint8_t* data, size_t dataSize, uint8_t* ilvStatus);

#ifdef __cplusplus
}
#endif

#endif /* INC_MORPHO_CREATE_BASE_H_ */
