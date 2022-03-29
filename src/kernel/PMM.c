#include <Sparrow/BaseType.h>
#include <Sparrow/Memory.h>
#include <Sparrow/PMM.h>
#include <Sparrow/Printk.h>
#include <Uefi.h>
#include <libc/math.h>

PMM gPMM;

STATUS_CODE InitPMM(IN MEMORY_MAP *MM) {
    // Descriptor 指针
    EFI_MEMORY_DESCRIPTOR *Descriptor = MM->MemoryMap;
    // Descriptor的数量
    U64 DescriptorCount = MM->MemoryMapSize / MM->DescriptorSize;
    // 最大物理地址
    gPMM.MaxPhAddr = 0;
    // 记录最大的物理地址
    for (I32 i = 0; i < DescriptorCount; i++) {
        EFI_PHYSICAL_ADDRESS EndAddress = Descriptor[i].PhysicalStart + (Descriptor[i].NumberOfPages << 12) - 1;
        if (EndAddress > gPMM.MaxPhAddr) {
            gPMM.MaxPhAddr = EndAddress;
        }
    }
    Printk("gPMM.MaxPhAddr = %#llx\n", gPMM.MaxPhAddr);
    // 减掉多余的物理地址
    gPMM.MaxPhAddr -= (gPMM.MaxPhAddr + 1) & 0xFFF;
    // 页框的数量
    gPMM.FrameNumber = (gPMM.MaxPhAddr >> 12) + 1;
    // PageMap需要的内存空间（bytes）
    U64 PageMapSize = sizeof(M_PAGE) * gPMM.FrameNumber;
    Printk("sizeof(M_PAGE) = %lld\n", sizeof(M_PAGE));
    Printk("PageMapSize = %lld bytes\n", PageMapSize);
    // PageMap需要的内存页，向上取整
    U64 PageMapPSize = (PageMapSize >> 12) + ((PageMapSize & 0xFFF) ? 1 : 0);
    Printk("PageMapPSize = %lld\n", PageMapPSize);
    // 找个地方存 PageMap
    for (I32 i = 0; i < DescriptorCount; i++) {
        if (Descriptor[i].NumberOfPages >= PageMapPSize && Descriptor[i].Type == FREE_MEMORY) {
            gPMM.PageMap = (M_PAGE *)(Descriptor[i].PhysicalStart);
            Printk("gPMM.PageMap = %#llx\n", gPMM.PageMap);
            if (Descriptor[i].NumberOfPages > PageMapPSize) {
                Descriptor[i].PhysicalStart += PageMapPSize << 12;
                Descriptor[i].NumberOfPages -= PageMapPSize;
            } else {
                Descriptor[i].Type = OS_DATA;
            }
            break;
        }
    }
    // TODO: 这里其实还可以优化一下，不用写这么多 0
    SetZero((PHYSICAL_ADDRESS)gPMM.PageMap, PageMapSize);
    // 初始化最大页地址
    gPMM.MaxPageAddr = gPMM.MaxPhAddr - 0xFFF;
    Printk("gPMM.MaxPageAddr = %#llx\n", gPMM.MaxPageAddr);

    // 初始化通用页框域, 把所以页框都给通用页框域
    gPMM.NormalZone.MaxPageNumber = gPMM.FrameNumber;
    Printk("gPMM.NormalZone.MaxPageNumber = %#llx\n", gPMM.NormalZone.MaxPageNumber);
    gPMM.NormalZone.FreePageNumber = 0;
    Printk("gPMM.NormalZone.PageNumber = %lld\n", gPMM.NormalZone.FreePageNumber);

    for (I32 i = 0; i < MAX_CONTIGUOUS_FRAME; i++) {
        gPMM.NormalZone.BlockList[i] = NULL;
    }
    for (U64 i = 0; i < gPMM.NormalZone.MaxPageNumber; i++) {
        gPMM.PageMap[i].M = 0;
        gPMM.PageMap[i].Type = BLOCK_HEADER;
    }
    for (U64 i = 0; i < gPMM.NormalZone.MaxPageNumber; i++) {
        FreeBlock(i << 12);
    }
    return 0;
}

STATUS_CODE FreeBlock(IN PHYSICAL_ADDRESS PhyAddr) {
    if ((PhyAddr & 0xFFF) != 0) {
        // 不是页地址
        return 1;
    }
    if (PhyAddr > gPMM.MaxPageAddr) {
        // 页地址越界
        return 2;
    }

    U64 PageIndex = PhyAddr >> 12;

    M_PAGE *ThisBlock = &(gPMM.PageMap[PageIndex]);
    if (ThisBlock->Type != BLOCK_HEADER) {
        // 不是块地址
        return 3;
    }
    if (ThisBlock->Freed) {
        // 此块已经被释放
        return 4;
    }
    /**
     * 1. 查看此块的伙伴是否被占用
     * 
     * PageIndex如果是奇数那对应的伙伴编号一定是PageIndex-1
     * PageIndex如果是偶数分为一下两种情况：
     *      · PageIndex/BlockSize是奇数，其伙伴编号为PageIndex-BlockSize
     *      · PageIndex/BlockSize是偶数，其伙伴编号为PageIndex+BlockSize
     * 
     * 2. 如果伙伴被占用就直接释放此块，反之将伙伴从链表摘下并与伙伴合并，然后再次调用FreeBlock()释放合并之后的块
     * 
     * 注：与伙伴合并时，编号小的为主块
     * 
     */
    // 块大小
    U32 BlockSize = pow(2, ThisBlock->M);
    // 伙伴编号
    U64 PartnerIndex;
    if (PageIndex % 2 == 1) {
        PartnerIndex = PageIndex - 1;
    } else {
        if ((PageIndex / BlockSize) % 2 == 1) {
            PartnerIndex = PageIndex - BlockSize;
        } else {
            PartnerIndex = PageIndex + BlockSize;
        }
    }
    // 伙伴块
    M_PAGE *Partner = &(gPMM.PageMap[PartnerIndex]);
    ThisBlock->Freed = True;
    if (Partner->Freed) {
        // 伙伴可用合
        if (gPMM.NormalZone.BlockList[Partner->M] == Partner) {
            // 伙伴是链表头
            if (Partner->Next == Partner) {
                gPMM.NormalZone.BlockList[Partner->M] = NULL;
            } else {
                gPMM.NormalZone.BlockList[Partner->M] = Partner->Next;
            }
        }
        // 将伙伴摘下
        Partner->Next->Pre = Partner->Pre;
        Partner->Pre->Next = Partner->Next;

        if (PartnerIndex < PageIndex) {
            // 伙伴块是主块
            ThisBlock->Type = FRAME;
            Partner->M++;
            Partner->Freed = False;
            FreeBlock(PartnerIndex << 12);
        } else {
            // 此块是主块
            Partner->Type = FRAME;
            ThisBlock->M++;
            ThisBlock->Freed = False;
            FreeBlock(PhyAddr);
        }
    } else {
        // 伙伴被占用，直接释放此块
        // ThisBlock->Next = gPMM.NormalZone.BlockList[ThisBlock->M];
        // gPMM.NormalZone.BlockList[ThisBlock->M] = ThisBlock;
        if (gPMM.NormalZone.BlockList[ThisBlock->M] == NULL) {
            ThisBlock->Next = ThisBlock;
            ThisBlock->Pre = ThisBlock;
            gPMM.NormalZone.BlockList[ThisBlock->M] = ThisBlock;
        } else {
            gPMM.NormalZone.BlockList[ThisBlock->M]->Next->Pre = ThisBlock;
            ThisBlock->Next = gPMM.NormalZone.BlockList[ThisBlock->M]->Next;
            gPMM.NormalZone.BlockList[ThisBlock->M]->Next = ThisBlock;
            ThisBlock->Pre = gPMM.NormalZone.BlockList[ThisBlock->M];
        }
    }
    return 0;
}

STATUS_CODE AlloPages(IN OUT PHYSICAL_ADDRESS *PhyAddr, IN U64 NumberOfPages) {
}
