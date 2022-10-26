/*
 * morpho.h
 *
 *  Created on: 20.05.2022
 *      Author: radomir.turca@codelec.cz
 */

#ifndef INC_MORPHO_ASYN_MESSAGE_H_
#define INC_MORPHO_ASYN_MESSAGE_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int MORPHO_AsynMeassage_Response(const uint8_t* data, size_t dataSize, uint8_t* ilvStatus, const char **message);

#ifdef __cplusplus
}
#endif

#endif /* INC_MORPHO_ASYN_MESSAGE_H_ */
