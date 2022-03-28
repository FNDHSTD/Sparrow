#include <Sparrow/Memory.h>
#include <Sparrow/PMM.h>
#include <Sparrow/Printk.h>

U32 NumberOfNodes = 0;
U32 MaxNodes      = 0;

FREE_MEMORY_NODE *gFreeMemoryList = NULL;

STATUS_CODE InitMemory(IN MEMORY_MAP *MM) {
    // Descriptor 的数量
    U32 DescriptorCount = MM->MemoryMapSize / MM->DescriptorSize;

    // [DEBUG]查看原始数据
    Printk("|Type|       Start        |        End         | PageSize\n");
    for (I32 i = 0; i < DescriptorCount; i++) {
        Printk("| %02d | %#18llx | %#18llx | %d\n", MM->MemoryMap[i].Type, MM->MemoryMap[i].PhysicalStart, MM->MemoryMap[i].PhysicalStart + (MM->MemoryMap[i].NumberOfPages << 12) - 1, MM->MemoryMap[i].NumberOfPages);
    }
    Printk("DescriptorCount = %d\n", DescriptorCount);

    /**
     * @brief 1.初始化物理内存管理器
     * 
     */
    // Descriptor 指针
    EFI_MEMORY_DESCRIPTOR *Descriptor = MM->MemoryMap;
    // 最大物理地址
    EFI_PHYSICAL_ADDRESS MaxAddress = 0;
    // 转换内存的类型，顺便记录最大的物理地址
    for (I32 i = 0; i < DescriptorCount; i++) {
        EFI_PHYSICAL_ADDRESS EndAddress = Descriptor[i].PhysicalStart + (Descriptor[i].NumberOfPages << 12) - 1;
        if (EndAddress > MaxAddress) {
            MaxAddress = EndAddress;
        }
        switch (Descriptor[i].Type) {
            case EfiBootServicesCode:
            case EfiBootServicesData:
            case EfiConventionalMemory:
                Descriptor[i].Type = FREE_MEMORY;
                break;
            case EfiLoaderCode:
                Descriptor[i].Type = OS_CODE;
                break;
            case EfiLoaderData:
                Descriptor[i].Type = OS_DATA;
                break;
            case EfiRuntimeServicesCode:
            case EfiRuntimeServicesData:
            case EfiACPIReclaimMemory:
            case EfiACPIMemoryNVS:
                Descriptor[i].Type = UEFI_MEMORY;
                break;
            case EfiMemoryMappedIO:
            case EfiMemoryMappedIOPortSpace:
                Descriptor[i].Type = MMIO_MEMORY;
                break;
            case EfiUnusableMemory:
            case EfiPalCode:
            case EfiPersistentMemory:
            case EfiMaxMemoryType:
            case EfiReservedMemoryType:
                Descriptor[i].Type = RESERVED_MEMORY;
                break;
            default:
                break;
        }
    }
    // 打印最大的物理地址
    Printk("MaxAddress = %#llx\n", MaxAddress);

    // 检查物理地址是否包含整数个4K的页框
    if ((MaxAddress + 1) & 0xFFF) {
        // 减掉多余的物理地址
        MaxAddress -= (MaxAddress + 1) & 0xFFF;
    }
    // BitMap 需要的内存空间（bytes）
    U32 BitMapSize = ((MaxAddress + 1) >> 12) / 8;
    // BitMap 需要的内存页，向上取整
    U32 BitMapPSize = (BitMapSize >> 12) + ((BitMapSize & 0xFFF) ? 1 : 0);
    // BitMap 的地址
    PHYSICAL_ADDRESS BitMapAddr;
    // 找个地方存 BitMap
    for (I32 i = 0; i < DescriptorCount; i++) {
        if (Descriptor[i].NumberOfPages >= BitMapPSize) {
            BitMapAddr = Descriptor[i].PhysicalStart;
            if (Descriptor[i].NumberOfPages > BitMapPSize) {
                Descriptor[i].PhysicalStart += BitMapPSize << 12;
                Descriptor[i].NumberOfPages -= BitMapPSize;
            } else {
                Descriptor[i].Type = OS_DATA;
            }
            // TODO: 这里其实还可以优化一下，不用写这么多 0
            SetZero(BitMapAddr, BitMapSize);
        }
    }
    // 初始化物理内存管理器
    InitPMM(MaxAddress, BitMapAddr);
    // TODO：四级分页
}

STATUS_CODE MemCopy(IN PHYSICAL_ADDRESS source, IN PHYSICAL_ADDRESS destination, IN U64 size) {
    if (size == 0 || source == destination) {
        return 0;
    }

    U64 *p1 = (U64 *)source;
    U64 *p2 = (U64 *)destination;

    U32 remainder = size % 8;

    U8 *p3 = (U8 *)(source + size - remainder);
    U8 *p4 = (U8 *)(destination + size - remainder);

    // 尾覆盖类型
    if (destination > source && destination <= (source + size - 1)) {
        for (I64 i = size / 8 - 1; i >= 0; i--) {
            p2[i] = p1[i];
        }
        for (I32 i = remainder - 1; i >= 0; i--) {
            p4[i] = p3[i];
        }
        return 0;
    }

    // 非尾覆盖类型
    for (I64 i = 0; i < size / 8; i++) {
        p2[i] = p1[i];
    }
    for (I32 i = 0; i < remainder; i++) {
        p4[i] = p3[i];
    }
    return 0;
}

STATUS_CODE SetZero(IN PHYSICAL_ADDRESS addr, IN U64 size) {
    if (size == 0) {
        return 0;
    }
    U64 *p1 = (U64 *)addr;
    for (U64 i = 0; i < size / 8; i++) {
        p1[i] = 0x0000000000000000;
    }
    U32 remainder = size % 8;
    U8 *p2        = (U8 *)(addr + size - remainder);
    for (U32 i = 0; i < remainder; i++) {
        p2[i] = 0x00;
    }
}
