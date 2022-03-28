#include <Sparrow/Memory.h>
#include <Sparrow/Printk.h>

BGRR_PIXEL *gPrintkFontAddr;
U32         gPrintkCharHight;
U32         gPrintkCharWidth;
U32         gPrintkCharPerLine;
U32         gPrintkLineCount;
U32         gPrintkRow;
U32         gPrintkColumn;

extern BGRR_PIXEL *gFrameBufferBase;

STATUS_CODE InitPrintk(IN EFI_PHYSICAL_ADDRESS FontAddr, IN U32 ScreenWidth, IN U32 ScreenHight) {
    gPrintkFontAddr    = (BGRR_PIXEL *)FontAddr;
    gPrintkCharHight   = 30;
    gPrintkCharWidth   = 15;
    gPrintkCharPerLine = ScreenWidth / gPrintkCharWidth;
    gPrintkLineCount   = ScreenHight / gPrintkCharHight;
    gPrintkRow         = 0;
    gPrintkColumn      = 0;
    return 0;
}

STATUS_CODE drawChar(IN C8 c) {
    U32 SourceX      = (c - 32) * gPrintkCharWidth;
    U32 SourceY      = 0;
    U32 DestinationX = gPrintkColumn * gPrintkCharWidth;
    U32 DestinationY = gPrintkRow * gPrintkCharHight;
    for (U32 i = 0; i < gPrintkCharWidth; i++) {
        for (U32 j = 0; j < gPrintkCharHight; j++) {
            BGRR_PIXEL color = gPrintkFontAddr[(SourceX + i) + (SourceY + 1425 * j)];
            PutPixel(DestinationX + i, DestinationY + j, color);
        }
    }
    return 0;
}

STATUS_CODE putChar(IN C8 c) {
    if (gPrintkRow == gPrintkLineCount) {
        rollUp();
    }

    // 识别控制字符
    if (c == '\n') {
        gPrintkColumn = 0;
        gPrintkRow++;
        return 0;
    }
    if (c == '\t') {
        U32 remainder = gPrintkColumn % 4;
        if (remainder == 0) {
            putChar(' ');
            putChar(' ');
            putChar(' ');
            putChar(' ');
        } else {
            for (U32 i = 0; i < 4 - remainder; i++) {
                putChar(' ');
            }
        }
        return 0;
    }

    // 输出显示字符
    if (c < 32 || c > 126) {
        return 1;
    }
    drawChar(c);
    gPrintkColumn++;
    if (gPrintkColumn == gPrintkCharPerLine) {
        gPrintkColumn = 0;
        gPrintkRow++;
    }
    return 0;
}

STATUS_CODE printChar(IN C8 c, IN U8 flag, IN I32 minWidth) {
    if (minWidth <= 0) {
        putChar(c);
        return 0;
    }

    C8 paddingChar = ' ';
    if (flag & ZERO) {
        paddingChar = '0';
    }

    if (flag & MINUS) {
        putChar(c);
        for (U8 i = 0; i < minWidth - 1; i++) {
            putChar(' ');
        }
    } else if (flag & PLUS) {
        for (U8 i = 0; i < minWidth - 1; i++) {
            putChar(paddingChar);
        }
        putChar(c);
    } else {
        putChar(c);
    }
    return 0;
}

STATUS_CODE printStr(IN C8 *s) {
    while (*s) {
        putChar(*s);
        s++;
    }
    return 0;
}

STATUS_CODE numToStr(IN I64 n, IN U32 base, OUT C8 str[65], OUT I32 *size) {
    I32 i = 0;
    I32 j = 0;
    C8  tmp[64];
    if (n < 0) {
        n        = -n;
        str[j++] = '-';
        *size    = 1;
    } else {
        *size = 0;
    }
    if (base > 36) {
        return 1;
    }
    C8 clist[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
    while (n / base > 0) {
        tmp[i++] = clist[n % base];
        n /= base;
    }
    tmp[i] = clist[n % base];
    *size += i + 1;
    while (i >= 0) {
        str[j++] = tmp[i--];
    }
    return 0;
}

STATUS_CODE uNumToStr(IN U64 n, IN U32 base, OUT C8 str[65], OUT I32 *size) {
    I32 i = 0;
    I32 j = 0;
    C8  tmp[64];

    if (base > 36) {
        return 1;
    }
    C8 clist[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
    while (n / base > 0) {
        tmp[i++] = clist[n % base];
        n /= base;
    }
    tmp[i] = clist[n % base];
    *size  = i + 1;
    while (i >= 0) {
        str[j++] = tmp[i--];
    }
    return 0;
}

STATUS_CODE printDec(IN I64 d, IN U8 flag, IN I32 minWidth) {
    C8  tmp[64];
    C8 *str = tmp;
    I32 size;
    numToStr(d, 10, str, &size);

    // 默认填充空格
    C8 paddingChar = ' ';

    // 左对齐
    if (flag & MINUS) {
        for (I32 i = 0; i < size; i++) {
            putChar(str[i]);
        }
        for (I32 i = 0; i < minWidth - size; i++) {
            putChar(paddingChar);
        }
    }
    // 默认右对齐
    else {
        if (flag & ZERO) {
            paddingChar = '0';
            if (d < 0) {
                putChar(*(str++));
                size--;
                minWidth--;
            }
        }

        for (I32 i = 0; i < minWidth - size; i++) {
            putChar(paddingChar);
        }
        for (I32 i = 0; i < size; i++) {
            putChar(str[i]);
        }
    }
    return 0;
}

STATUS_CODE printHex(IN U64 d, IN U8 flag, IN I32 minWidth) {
    C8  str[65];
    I32 size = 0;
    uNumToStr(d, 16, str, &size);

    // 默认填充空格
    C8 paddingChar = ' ';

    // 左对齐
    if (flag & MINUS) {
        // #
        if (flag & NUMBER) {
            putChar('0');
            putChar('x');
            minWidth -= 2;
        }
        for (I32 i = 0; i < size; i++) {
            putChar(str[i]);
        }
        for (I32 i = 0; i < minWidth - size; i++) {
            putChar(paddingChar);
        }
    }
    // 默认右对齐
    else {
        // 0填充
        if (flag & ZERO) {
            paddingChar = '0';
            // #
            if (flag & NUMBER) {
                putChar('0');
                putChar('x');
                minWidth -= 2;
            }
            for (I32 i = 0; i < minWidth - size; i++) {
                putChar(paddingChar);
            }
            for (I32 i = 0; i < size; i++) {
                putChar(str[i]);
            }
        }
        // 空格填充
        else {
            // #
            if (flag & NUMBER) {
                minWidth -= 2;
            }
            for (I32 i = 0; i < minWidth - size; i++) {
                putChar(paddingChar);
            }
            if (flag & NUMBER) {
                putChar('0');
                putChar('x');
            }
            for (I32 i = 0; i < size; i++) {
                putChar(str[i]);
            }
        }
    }
    return 0;
}

STATUS_CODE Printk(IN C8 *format, ...) {
    va_list args;
    va_start(args, format);
    C8 *s = format;

    while (*s != '\0') {
        // 不用格式化直接输出
        if (*s != '%') {
            putChar(*s);
            s++;
            continue;
        }

        // 要格式化再输出
        s++;
        U8   flag     = 0;
        I32  padding  = 0; // 填充的位数
        BOOL firstNum = 1; // 第一次读到数字
        BOOL stop     = 0; // 停止下方while
        U8   intSize  = 32;
        while (!stop && *s != '\0') {
            switch (*s) {
                // 输出: %
                case '%':
                    putChar('%');
                    stop = 1;
                    break;
                // 输出结果左对齐，右边填空格（和输出最小宽度搭配使用）
                case '-':
                    flag |= MINUS;
                    break;
                // 输出结果右对齐，左边填空格（和输出最小宽度搭配使用），同时显示符号
                case '+':
                    flag |= PLUS;
                    break;
                // 对 c、s、d、u 类无影响，对 o 类输出时加前缀 0，x 类输出时加前缀 0x
                case '#':
                    flag |= NUMBER;
                    break;
                // 输出符号，值为正时冠以空格，为负时冠以负号
                case ' ':
                    flag |= SPACE;
                    break;
                // 放置在十进制整数的输出最小宽度前，用于当实际位数少于最小宽度时在前面补 0
                case '0':
                    if (firstNum) {
                        flag |= ZERO;
                        firstNum = 0;
                        break;
                    }
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    firstNum = 0;
                    padding  = padding * 10 + *s - '0';
                    break;
                case 'l':
                    if (*(++s) == 'l') {
                        intSize = 64;
                    }
                    break;
                case 'c':
                    printChar(va_arg(args, I32), flag, padding);
                    stop = 1;
                    break;
                case 'd':
                    if (intSize == 64) {
                        printDec(va_arg(args, I64), flag, padding);
                    } else {
                        printDec(va_arg(args, I32), flag, padding);
                    }
                    stop = 1;
                    break;
                case 'X':
                case 'x':
                    if (intSize == 64) {
                        printHex(va_arg(args, U64), flag, padding);
                    } else {
                        printHex(va_arg(args, U32), flag, padding);
                    }
                    stop = 1;
                    break;
                default:
                    Printk("Illegal parameter.");
                    break;
            }
            s++;
        }
    }
    va_end(args);
    return 0;
}

STATUS_CODE rollUp() {
    gPrintkRow--;
    for (U32 i = 1; i < gPrintkLineCount; i++) {
        PHYSICAL_ADDRESS source      = (PHYSICAL_ADDRESS)gFrameBufferBase + i * gPrintkCharHight * gPrintkCharPerLine * gPrintkCharWidth * sizeof(BGRR_PIXEL);
        PHYSICAL_ADDRESS destination = (PHYSICAL_ADDRESS)gFrameBufferBase + (i - 1) * gPrintkCharHight * gPrintkCharPerLine * gPrintkCharWidth * sizeof(BGRR_PIXEL);
        MemCopy(source, destination, sizeof(BGRR_PIXEL) * gPrintkCharWidth * gPrintkCharPerLine * gPrintkCharHight);
        SetZero(source, sizeof(BGRR_PIXEL) * gPrintkCharWidth * gPrintkCharPerLine * gPrintkCharHight);
    }
}
