#include <Sparrow/Memory.h>
#include <Sparrow/PMM.h>
#include <Sparrow/Printk.h>

U32 NumberOfNodes = 0;
U32 MaxNodes = 0;

FREE_MEMORY_NODE *gFreeMemoryList = NULL;

C8 *GetMemType(MEMORY_TYPE t) {
    switch (t) {
        case FREE_MEMORY:
            return "FREE_MEMORY";
            break;
        case OS_CODE:
            return "OS_CODE";
            break;
        case OS_DATA:
            return "OS_DATA";
            break;
        case AP_CODE:
            return "AP_CODE";
            break;
        case AP_DATA:
            return "AP_DATA";
            break;
        case UEFI_MEMORY:
            return "UEFI_MEMORY";
            break;
        case MMIO_MEMORY:
            return "MMIO_MEMORY";
            break;
        case RESERVED_MEMORY:
            return "RESERVED_MEMORY";
            break;
        default:
            break;
    }
    return "";
}

STATUS_CODE InitMemory(IN MEMORY_MAP *MM) {
    /*
     *1.转换物理内存类型
     */
    // Descriptor 的数量
    U32 DescriptorCount = MM->MemoryMapSize / MM->DescriptorSize;
    // Descriptor 指针
    EFI_MEMORY_DESCRIPTOR *Descriptor = MM->MemoryMap;

    U64 PageCount = 0;
    for (I32 i = 0; i < DescriptorCount; i++) {
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
        PageCount += Descriptor[i].NumberOfPages;
    }
    Printk("PageCount = %lld\n", PageCount);
    Printk("DescriptorCount = %d\n", DescriptorCount);

    // // [DEBUG]查看转换后的数据
    // Printk("|Type|       Start        |        End         | PageSize\n");
    // for (I32 i = 0; i < DescriptorCount; i++) {
    //     // for (I32 i = 0; i < 15; i++) {
    //     Printk("| %15s | %#18llx | %#18llx | %d\n", GetMemType(MM->MemoryMap[i].Type), MM->MemoryMap[i].PhysicalStart, MM->MemoryMap[i].PhysicalStart + (MM->MemoryMap[i].NumberOfPages << 12) - 1, MM->MemoryMap[i].NumberOfPages);
    // }

    /*
     * 2.初始化物理内存管理器
     */
    InitPMM(MM);
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
    U8 *p2 = (U8 *)(addr + size - remainder);
    for (U32 i = 0; i < remainder; i++) {
        p2[i] = 0x00;
    }
}
