#ifndef __PRINTK_H__
#define __PRINTK_H__

#include <stdarg.h>

#include <Sparrow/BaseType.h>
#include <Sparrow/Graphic.h>

#define MINUS  0b1      // -
#define PLUS   0b10     // +
#define NUMBER 0b100    // #
#define ZERO   0b1000   // 0
#define SPACE  0b10000  // 空格
#define SMALL  0b100000 // 大小写

STATUS_CODE InitPrintk(IN PHYSICAL_ADDRESS FontAddr, IN U32 ScreenWidth, IN U32 ScreenHight);

STATUS_CODE numToStr(IN I64 n, IN U32 base, OUT C8 str[65], OUT I32 *size);

STATUS_CODE uNumToStr(IN U64 n, IN U32 base, OUT C8 str[65], OUT I32 *size);

STATUS_CODE drawChar(IN C8 c);

STATUS_CODE putChar(IN C8 c);

STATUS_CODE printStr(IN C8 *s);

STATUS_CODE printChar(IN C8 c, IN U8 flag, IN I32 minWidth);

STATUS_CODE printDec(IN I64 d, IN U8 flag, IN I32 minWidth);

STATUS_CODE printHex(IN U64 d, IN U8 flag, IN I32 minWidth);

STATUS_CODE printOct();

STATUS_CODE printFloat();

STATUS_CODE Printk(IN C8 *format, ...);

STATUS_CODE rollUp();

#endif
