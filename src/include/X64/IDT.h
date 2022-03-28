#ifndef __IDT_H__
#define __IDT_H__

#include <Sparrow/BaseType.h>

#pragma pack(1)
typedef struct
{
    U16              Limit;
    PHYSICAL_ADDRESS Base;
} IDTR;

typedef struct
{
    U16 Offset1;
    U16 SegmentSelector;
    U8  IST_Reserved;
    U8  Type_0_DPL_P;
    U16 Offset2;
    U32 Offset3;
    U32 Reserved;
} IDT_GATE_DESCRIPTOR;

#pragma pack()

VOID InitIDT();

VOID setIDTGateDescriptor(U8 Signal, U16 Selector, U64 Offset, U8 Type, U8 DPL);

VOID test();

#endif
