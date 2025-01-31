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

void MORPHO_GetDescriptorText_Request(uint8_t* data, size_t* dataSize);
void MORPHO_GetDescriptorBinVer_Request(uint8_t* packet, size_t* packetSize);
void MORPHO_GetDescriptorBinMaxUser_Request(uint8_t* packet, size_t* packetSize);

int MORPHO_GetDescriptor_Response(const uint8_t* data, size_t dataSize, uint8_t* ilvStatus, uint8_t* type);
int MORPHO_GetDescriptorText_Response(const uint8_t* data, size_t dataSize, uint8_t* ilvStatus, const char** product, const char** sensor, const char** software);
int MORPHO_GetDescriptorBinVer_Response(const uint8_t* value, size_t valueSize, uint8_t* ilvStatus, const char** version, uint16_t* versionSize);
int MORPHO_GetDescriptorBinMaxUser_Response(const uint8_t* value, size_t valueSize, uint8_t* ilvStatus, uint16_t* max_user);

#ifdef __cplusplus
}
#endif

#endif /* INC_MORPHO_GET_DESCRIPTOR_H_ */
