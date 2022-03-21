#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "BaseType.h"
#include "Uefi.h"
#include "Printk.h"


typedef enum
{
    OS_CODE,
    OS_DATA,
    AP_CODE,
    AP_DATA,
    UEFI_MEMORY,
    MMIO_MEMORY,
    FREE_MEMORY,
    RESERVED_MEMORY,
} MEMORY_TYPE;

typedef struct MEMORY_DESCRIPTOR_NODE
{
    PHYSICAL_ADDRESS Start;
    U64 PageSize;
    MEMORY_TYPE Type;
    struct MEMORY_DESCRIPTOR_NODE *Next;
} MEMORY_DESCRIPTOR_NODE;

typedef struct
{
    MEMORY_TYPE Type;
    PHYSICAL_ADDRESS Start;
    U64 PageSize;
} MEMORY_DESCRIPTOR;

STATUS_CODE InitMemory(IN MEMORY_MAP *MM);

STATUS_CODE MemCopy(IN PHYSICAL_ADDRESS source, IN PHYSICAL_ADDRESS destination, IN U64 size);

STATUS_CODE SetZero(IN PHYSICAL_ADDRESS addr, IN U64 size);

#endif