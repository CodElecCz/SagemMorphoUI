#ifndef __Ilv_Errors_h
#define __Ilv_Errors_h

#include <stddef.h>
#include <stdint.h>

/*******************************************************************************
-- Errors implementation
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

const char* IlvConvertError(uint8_t error);
const char* IlvConvertStatus(uint8_t status);

#ifdef __cplusplus
}
#endif

#endif /*__Ilv_Errors_h*/
