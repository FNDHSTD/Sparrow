#ifndef __GRAPHIC_H__
#define __GRAPHIC_H__

#include "BaseType.h"
#include "Memory.h"
#include "Uefi.h"

#pragma pack(1)
typedef struct
{
    U8 Blue;
    U8 Green;
    U8 Red;
    U8 Reserved;
} BGRR_PIXEL;
#pragma pack()

STATUS_CODE InitGraphic(IN GRAPHIC_PARAMETER *GP);

STATUS_CODE PutPixel(IN U32 x, IN U32 y, IN BGRR_PIXEL color);

STATUS_CODE Clear();

#endif
