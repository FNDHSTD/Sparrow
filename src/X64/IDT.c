#include <Sparrow/Printk.h>
#include <X64/IDT.h>

// IDT 入口数量，最大256
#define IDT_ENTRY 20

IDT_GATE_DESCRIPTOR IDT[IDT_ENTRY];

VOID setIDTGateDescriptor(U8 Signal, U16 Selector, PHYSICAL_ADDRESS Offset, U8 Type, U8 DPL) {
    IDT[Signal].IST_Reserved    = 0;
    IDT[Signal].Reserved        = 0;
    IDT[Signal].SegmentSelector = Selector;
    IDT[Signal].Type_0_DPL_P    = Type;

    IDT[Signal].Type_0_DPL_P |= DPL << 5;
    IDT[Signal].Type_0_DPL_P |= 1 << 7;

    IDT[Signal].Offset1 = Offset;
    IDT[Signal].Offset2 = Offset >> 16;
    IDT[Signal].Offset3 = Offset >> 32;
}

VOID InitIDT() {

    IDTR idtr = {
        IDT_ENTRY * sizeof(IDT_GATE_DESCRIPTOR) - 1,
        (PHYSICAL_ADDRESS)IDT,
    };
    setIDTGateDescriptor(0, 8, (PHYSICAL_ADDRESS)test, 0xE, 0);
    setIDTGateDescriptor(1, 8, (PHYSICAL_ADDRESS)test, 0xE, 0);
    setIDTGateDescriptor(2, 8, (PHYSICAL_ADDRESS)test, 0xE, 0);
    setIDTGateDescriptor(3, 8, (PHYSICAL_ADDRESS)test, 0xE, 0);
    setIDTGateDescriptor(4, 8, (PHYSICAL_ADDRESS)test, 0xE, 0);
    setIDTGateDescriptor(5, 8, (PHYSICAL_ADDRESS)test, 0xE, 0);
    setIDTGateDescriptor(6, 8, (PHYSICAL_ADDRESS)test, 0xE, 0);
    setIDTGateDescriptor(7, 8, (PHYSICAL_ADDRESS)test, 0xE, 0);
    setIDTGateDescriptor(8, 8, (PHYSICAL_ADDRESS)test, 0xE, 0);
    setIDTGateDescriptor(9, 8, (PHYSICAL_ADDRESS)test, 0xE, 0);
    setIDTGateDescriptor(10, 8, (PHYSICAL_ADDRESS)test, 0xE, 0);
    setIDTGateDescriptor(11, 8, (PHYSICAL_ADDRESS)test, 0xE, 0);
    setIDTGateDescriptor(12, 8, (PHYSICAL_ADDRESS)test, 0xE, 0);
    setIDTGateDescriptor(13, 8, (PHYSICAL_ADDRESS)test, 0xE, 0);
    setIDTGateDescriptor(14, 8, (PHYSICAL_ADDRESS)test, 0xE, 0);
    setIDTGateDescriptor(15, 8, (PHYSICAL_ADDRESS)test, 0xE, 0);
    setIDTGateDescriptor(16, 8, (PHYSICAL_ADDRESS)test, 0xE, 0);
    setIDTGateDescriptor(17, 8, (PHYSICAL_ADDRESS)test, 0xE, 0);
    setIDTGateDescriptor(18, 8, (PHYSICAL_ADDRESS)test, 0xE, 0);
    setIDTGateDescriptor(19, 8, (PHYSICAL_ADDRESS)test, 0xE, 0);
    // asm volatile(
    //     "sidt %0\n\t"
    //     : "=m"(idtr)
    //     :
    //     :);
    asm volatile(
        "lidt %0\n\t"
        "sti\n\t"
        :
        : "m"(idtr)
        :);
    Printk("idtr.Base = %#x, idtr.Limit = %#llx\n", idtr.Base, idtr.Limit);
}

VOID test() {
    Printk("This is in interrupt!\n");
    while (1)
        ;
}
