#include <X64/GDT.h>
#include <Printk.h>

#define GDT_ENTRY 5

U64 GDT[GDT_ENTRY];

U64 createDescriptor(U32 Base, U32 Limit, U16 Flag)
{
    U64 Descriptor = 0;
    Descriptor |= Limit & 0x000F0000;
    Descriptor |= (Flag << 8) & 0x00F0FF00;
    Descriptor |= (Base >> 16) & 0x000000FF;
    Descriptor |= Base & 0xFF000000;
    Descriptor <<= 32;
    Descriptor |= Base << 16;
    Descriptor |= Limit & 0x0000FFFF;
    return Descriptor;
}

VOID InitGDT()
{
    GDT[0] = 0;
    GDT[1] = createDescriptor(0, 0x000FFFFF, (GDT_CODE_PL0));
    GDT[2] = createDescriptor(0, 0x000FFFFF, (GDT_DATA_PL0));
    GDT[3] = createDescriptor(0, 0x000FFFFF, (GDT_CODE_PL3));
    GDT[4] = createDescriptor(0, 0x000FFFFF, (GDT_DATA_PL3));
    GDTR gdtr = {
        5 * sizeof(U64) - 1,
        (PHYSICAL_ADDRESS)GDT,
    };
    asm volatile("lgdt %0\n\t"
                 ""
                 :
                 : "m"(gdtr)
                 :);
}