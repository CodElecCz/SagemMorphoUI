/*
 * morpho.h
 *
 *  Created on: 20.05.2022
 *      Author: radomir.turca@codelec.cz
 */

#ifndef INC_MORPHO_CONFIGURE_UART_H_
#define INC_MORPHO_CONFIGURE_UART_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void MORPHO_ConfigureUart_Request(uint8_t* packet, size_t* packetSize, uint32_t bps);
int MORPHO_ConfigureUart_Response(const uint8_t* data, size_t dataSize, uint8_t* ilvStatus);

#ifdef __cplusplus
}
#endif

#endif /* INC_MORPHO_CONFIGURE_UART_H_ */
