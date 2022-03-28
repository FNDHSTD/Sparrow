#include <Sparrow/BaseType.h>
#include <Sparrow/Graphic.h>
#include <Sparrow/Memory.h>
#include <Sparrow/Printk.h>
#include <Uefi.h>
#include <X64/GDT.h>
#include <X64/IDT.h>

VOID InitKernel(BOOT_PARAMETER *BP) {
    InitGraphic(BP->GP);
    InitPrintk(BP->FontFile->Address, BP->GP->HorizontalResolution, BP->GP->VerticalResolution);
    InitIDT();
    InitMemory(BP->MM);
    Printk("Hello,Sparrow!\n");

    // GDTR
    GDTR gdtr;
    Printk("gdtr:\n");
    asm volatile("sgdt %0"
                 : "=m"(gdtr)
                 :
                 :);
    Printk("size=%#x, addr=%#x\n", gdtr.size, gdtr.addr);

    // GDT
    // U64 *gdt = (U64 *)gdtr.addr;
    // Printk("gdt:\n");
    // for (I32 i = 0; i < (gdtr.size + 1) / 8; i++)
    // {
    //     Printk("%#018llx\n", gdt[i]);
    // }

    // cr0
    // U8 cr0[8] = {0};
    // Printk("cr0:\n");
    // asm volatile("mov %%cr0, %%rax\n\t"
    //              "mov %%rax, %0"
    //              : "=m"(cr0)
    //              :
    //              : "ax");
    // for (I32 i = 0; i < 8; i++)
    // {
    //     Printk("%02x ", cr0[7 - i]);
    // }
    // Printk("\n");

    // cr1
    // U8 cr1[8] = {0};
    // Printk("cr1:\n");
    // asm volatile("mov %%cr1, %%rax\n\t"
    //              "mov %%rax, %0"
    //              : "=m"(cr1)
    //              :
    //              : "ax");
    // for (I32 i = 0; i < 8; i++)
    // {
    //     Printk("%02x ", cr1[7 - i]);
    // }
    // Printk("\n");

    // cr2
    // U8 cr2[8] = {0};
    // Printk("cr2:\n");
    // asm volatile("mov %%cr2, %%rax\n\t"
    //              "mov %%rax, %0"
    //              : "=m"(cr2)
    //              :
    //              : "ax");
    // for (I32 i = 0; i < 8; i++)
    // {
    //     Printk("%02x ", cr2[7 - i]);
    // }
    // Printk("\n");

    // cr3
    // U8 cr3[8] = {0};
    // Printk("cr3:\n");
    // asm volatile("mov %%cr3, %%rax\n\t"
    //              "mov %%rax, %0"
    //              : "=m"(cr3)
    //              :
    //              : "ax");
    // for (I32 i = 0; i < 8; i++)
    // {
    //     Printk("%02x ", cr3[7 - i]);
    // }
    // Printk("\n");

    // cr4
    // U8 cr4[8] = {0};
    // Printk("cr4:\n");
    // asm volatile("mov %%cr4, %%rax\n\t"
    //              "mov %%rax, %0"
    //              : "=m"(cr4)
    //              :
    //              : "ax");
    // for (I32 i = 0; i < 8; i++)
    // {
    //     Printk("%02x ", cr4[7 - i]);
    // }
    // Printk("\n");

    // cpuid
    // U8 cpuid[4] = {0};
    // Printk("cpuid:\n");
    // asm volatile("mov $0x1, %%eax\n\t"
    //              "cpuid\n\t"
    //              "mov %%edx, %0\n\t"
    //              : "=m"(cpuid)
    //              :
    //              : "ax", "bx", "cx", "dx");
    // for (I32 i = 0; i < 4; i++)
    // {
    //     Printk("%02x ", cpuid[3 - i]);
    // }
    // Printk("\n");

    // EFER
    // U8 eferh[4] = {0};
    // U8 eferl[4] = {0};
    // Printk("efer:\n");
    // asm volatile("mov $0xC0000080, %%ecx\n\t"
    //              "rdmsr\n\t"
    //              "mov %%edx, %0\n\t"
    //              "mov %%eax, %1\n\t"
    //              : "=m"(eferh), "=m"(eferl)
    //              :
    //              : "dx", "ax", "cx");
    // for (I32 i = 0; i < 4; i++)
    // {
    //     Printk("%02x ", eferh[3 - i]);
    // }
    // for (I32 i = 0; i < 4; i++)
    // {
    //     Printk("%02x ", eferl[3 - i]);
    // }
    // Printk("\n");
    // Printk("FrameBufferBase = %#llx\n", BP->GP->FrameBufferBase);

    asm volatile("nop\n\t");

    while (1)
        ;
}
