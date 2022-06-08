#ifndef __Ilv_Crc_h
#define __Ilv_Crc_h

#include <stddef.h>
#include <stdint.h>

/*******************************************************************************
-- ComputeCrc16 implementation
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

void IlvCrc16 (uint8_t* Buffer, size_t Len, uint8_t* CrcH, uint8_t* CrcL);

#ifdef __cplusplus
}
#endif

#endif /*__Ilv_Crc_h*/
