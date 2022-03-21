#ifndef __GDT_H__
#define __GDT_H__

#include <BaseType.h>

#define SEG_S(x) ((x) << 4)          //描述符类型，系统段为 0， 代码 or 数据段为0
#define SEG_DPL(x) (((x)&0b11) << 5) //特权级
#define SEG_P(x) ((x) << 7)          //存在位
#define SEG_AVL(x) ((x) << 12)       //没什么卵用
#define SEG_L(x) ((x) << 13)         // IA-32e模式
#define SEG_DB(x) ((x) << 14)        // 64位模式应设为0
#define SEG_G(x) ((x) << 15)         //粒度。设为 1 则粒度为4K

#define SEG_DATA_RD 0x00        // Read-Only
#define SEG_DATA_RDA 0x01       // Read-Only, accessed
#define SEG_DATA_RDWR 0x02      // Read/Write
#define SEG_DATA_RDWRA 0x03     // Read/Write, accessed
#define SEG_DATA_RDEXPD 0x04    // Read-Only, expand-down
#define SEG_DATA_RDEXPDA 0x05   // Read-Only, expand-down, accessed
#define SEG_DATA_RDWREXPD 0x06  // Read/Write, expand-down
#define SEG_DATA_RDWREXPDA 0x07 // Read/Write, expand-down, accessed
#define SEG_CODE_EX 0x08        // Execute-Only
#define SEG_CODE_EXA 0x09       // Execute-Only, accessed
#define SEG_CODE_EXRD 0x0A      // Execute/Read
#define SEG_CODE_EXRDA 0x0B     // Execute/Read, accessed
#define SEG_CODE_EXC 0x0C       // Execute-Only, conforming
#define SEG_CODE_EXCA 0x0D      // Execute-Only, conforming, accessed
#define SEG_CODE_EXRDC 0x0E     // Execute/Read, conforming
#define SEG_CODE_EXRDCA 0x0F    // Execute/Read, conforming, accessed

#define GDT_CODE_PL0 SEG_S(1) | SEG_DPL(0) | SEG_P(1) | SEG_AVL(0) | SEG_L(1) | SEG_DB(0) | SEG_G(1) | SEG_CODE_EXRD

#define GDT_DATA_PL0 SEG_S(1) | SEG_DPL(0) | SEG_P(1) | SEG_AVL(0) | SEG_L(1) | SEG_DB(0) | SEG_G(1) | SEG_DATA_RDWR

#define GDT_CODE_PL3 SEG_S(1) | SEG_DPL(3) | SEG_P(1) | SEG_AVL(0) | SEG_L(1) | SEG_DB(0) | SEG_G(1) | SEG_CODE_EXRD

#define GDT_DATA_PL3 SEG_S(1) | SEG_DPL(3) | SEG_P(1) | SEG_AVL(0) | SEG_L(1) | SEG_DB(0) | SEG_G(1) | SEG_DATA_RDWR

#pragma pack(1)
typedef struct
{
    U16 size;
    PHYSICAL_ADDRESS addr;
} GDTR;
#pragma pack()

U64 createDescriptor(U32 Base, U32 Limit, U16 Flag);

VOID InitGDT();

#endif