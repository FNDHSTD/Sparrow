#include <Sparrow/BaseType.h>
#include <Sparrow/PMM.h>
#include <Uefi.h>

PMM gPMM;

STATUS_CODE InitPMM(IN PHYSICAL_ADDRESS MaxAddress, IN PHYSICAL_ADDRESS BitMapAddr) {
    // 检查最大地址合法性，低12位应全为1：0x·············FFF
    if (MaxAddress & 0xFFF != 0xFFF) {
        return 1;
    }
    gPMM.MaxAddr     = MaxAddress;
    gPMM.BitMap      = (U64 *)BitMapAddr;
    gPMM.MaxPageAddr = MaxAddress & 0x000;
}

STATUS_CODE FreePages(IN PHYSICAL_ADDRESS PAddr, IN U64 NumberOfPages) {
    // 检查地址是否合法：要求低12位全为0（4K对齐）
    if (PAddr & 0xFFF) {
        return 1;
    }
    // 第一个页框的序号
    U64 PageIndex = PAddr >> 12;
    // 第一个页框在节点内的偏移0~63
    U8 Offset = PAddr % 64;
    // 第一个页框所在的BitMap节点
    U64 *Node = &gPMM.BitMap[PageIndex / 64 + (Offset ? 1 : 0)];
    U64  Mask = 0xFFFFFFFFFFFFFFFF;
    if (NumberOfPages <= 64 - Offset) {
        // 只要处理一个节点
        Mask <<= Offset;
        Mask >>= Offset;
        Mask >>= 64 - Offset - NumberOfPages;
        Mask <<= 64 - Offset - NumberOfPages;
        if (*Node & Mask) {
            // 要释放的页框中至少有一个页框已经被释放了
            return 2;
        }
        *Node |= Mask;
        return 0;
    }
    // 要处理多个节点···
    // 先处理第一个
    Mask <<= Offset;
    Mask >>= Offset;
    if (*Node & Mask) {
        // 要释放的页框中至少有一个页框已经被释放了
        return 2;
    }
    *Node |= Mask;
    Node++;

    // 处理中间节点···

    // 剩余要处理的页框数
    NumberOfPages -= 64 - Offset;
    // 循环次数
    U32 Quotitent = NumberOfPages / 64;
    for (I32 i = 0; i < Quotitent; i++) {
        if (*Node & 0xFFFFFFFFFFFFFFFF != 0) {
            // 要释放的页框中至少有一个页框已经被释放了
            return 2;
        }
        *Node = 0xFFFFFFFFFFFFFFFF;
        Node++;
    }

    // 处理剩余的零碎节点，少于64个···

    // 剩余的页框数
    U32 Remainder = NumberOfPages % 64;

    Mask = 0xFFFFFFFFFFFFFFFF;
    Mask >>= 64 - Remainder;
    Mask <<= 64 - Remainder;
    if (*Node & 0xFFFFFFFFFFFFFFFF != 0) {
        // 要释放的页框中至少有一个页框已经被释放了
        return 2;
    }
    *Node |= Mask;
    return 0;
}

STATUS_CODE AlloPages(IN OUT PHYSICAL_ADDRESS *PAddr, IN U64 NumberOfPages) {
    // 申请的页框数大于64个（256KB）
    if (NumberOfPages > 64) {
        }
}
