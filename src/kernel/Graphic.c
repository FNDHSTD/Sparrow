#include <Sparrow/Graphic.h>
#include <Sparrow/Memory.h>

BGRR_PIXEL *gFrameBufferBase;
U32         gFrameBufferSize;
U32         gScreenWidth;
U32         gScreenHeight;

STATUS_CODE InitGraphic(IN GRAPHIC_PARAMETER *GP) {
    gFrameBufferBase = (BGRR_PIXEL *)GP->FrameBufferBase;
    gFrameBufferSize = GP->FrameBufferSize;
    gScreenWidth     = GP->HorizontalResolution;
    gScreenHeight    = GP->VerticalResolution;
    return 0;
}

STATUS_CODE PutPixel(IN U32 x, IN U32 y, IN BGRR_PIXEL color) {
    if (x < 0 || x >= gScreenWidth) {
        if (y < 0 || y >= gScreenHeight) {
            return 1;
        }
    }
    gFrameBufferBase[x + y * gScreenWidth] = color;
    return 0;
}

STATUS_CODE GetPixel(IN U32 x, IN U32 y, OUT BGRR_PIXEL *color) {
    if (x < 0 || x >= gScreenWidth) {
        if (y < 0 || y >= gScreenHeight) {
            return 1;
        }
    }
    *color = gFrameBufferBase[x + y * gScreenWidth];
    return 0;
}

STATUS_CODE Clear() {
    SetZero((PHYSICAL_ADDRESS)gFrameBufferBase, gFrameBufferSize);
}
