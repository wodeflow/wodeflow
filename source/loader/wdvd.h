#ifndef _WDVD_H_
#define _WDVD_H_

#include "config_flags.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Prototypes */
s32 WDVD_Init(void);
s32 WDVD_Close(void);
s32 WDVD_GetHandle(void);
s32 WDVD_Reset(void);
s32 WDVD_ReadDiskId(void *);
s32 WDVD_Seek(u64);
s32 WDVD_Offset(u64);
s32 WDVD_StopLaser(void);
s32 WDVD_StopMotor(void);
s32 WDVD_OpenPartition(u64 offset, void* Ticket, void* Certificate, unsigned int Cert_Len, void* Out);
s32 WDVD_OpenPartitionSimple(u64 offset);
s32 WDVD_ClosePartition(void);
s32 WDVD_UnencryptedRead(void *, u32, u64);
s32 WDVD_Read(void *, u32, u64);
s32 WDVD_WaitForDisc(void);
s32 WDVD_GetCoverStatus(u32 *);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

