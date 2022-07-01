/*
 * morpho.h
 *
 *  Created on: 20.05.2022
 *      Author: radomir.turca@codelec.cz
 */

#ifndef INC_MORPHO_CANCEL_H_
#define INC_MORPHO_CANCEL_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void MORPHO_Cancel_Request(uint8_t* packet, size_t* packetSize);

#ifdef __cplusplus
}
#endif

#endif /* INC_MORPHO_CANCEL_H_ */
