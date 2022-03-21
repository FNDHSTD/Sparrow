#include <Memory.h>

STATUS_CODE InitMemory(IN MEMORY_MAP *MM)
{
    // TODO：四级分页
    /*
        Step 1: 把 EfiBootServicesCode 和 EfiBootServicesData 类型的内存先释放掉, 并且转换类型
    */
    // 先处理第 0 个
    U32 DescriptorCount = MM->MemoryMapSize / MM->DescriptorSize;
    // 查看原始数据
    // Printk("|Type|       Start        |        End         | PageSize\n");
    // for (I32 i = 0; i < 20; i++)
    // {
    //     Printk("| %02d | %#18llx | %#18llx | %d\n", MM->MemoryMap[i].Type, MM->MemoryMap[i].PhysicalStart, MM->MemoryMap[i].PhysicalStart + (MM->MemoryMap[i].NumberOfPages << 12) - 1, MM->MemoryMap[i].NumberOfPages);
    // }
    // Printk("DescriptorCount = %d\n", DescriptorCount);

    EFI_MEMORY_DESCRIPTOR *Descriptor;
    Descriptor = MM->MemoryMap;
    switch (Descriptor->Type)
    {
    case EfiBootServicesCode:
    case EfiBootServicesData:
    case EfiConventionalMemory:
        Descriptor->Type = FREE_MEMORY;
        break;
    case EfiLoaderCode:
        Descriptor->Type = OS_CODE;
        break;
    case EfiLoaderData:
        Descriptor->Type = OS_DATA;
        break;
    case EfiRuntimeServicesCode:
    case EfiRuntimeServicesData:
    case EfiACPIReclaimMemory:
    case EfiACPIMemoryNVS:
        Descriptor->Type = UEFI_MEMORY;
        break;
    case EfiMemoryMappedIO:
    case EfiMemoryMappedIOPortSpace:
        Descriptor->Type = MMIO_MEMORY;
        break;
    case EfiUnusableMemory:
    case EfiPalCode:
    case EfiPersistentMemory:
    case EfiMaxMemoryType:
    case EfiReservedMemoryType:
        Descriptor->Type = RESERVED_MEMORY;
        break;
    default:
        break;
    }
    // 处理后续
    U32 PageCount = 1;
    for (I32 i = 1; i < DescriptorCount; i++)
    {
        PageCount += Descriptor[i].NumberOfPages;
        switch (Descriptor[i].Type)
        {
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
        // 归并
        if (Descriptor[i].Type == Descriptor[i - 1].Type && (Descriptor[i].PhysicalStart - 1) == (Descriptor[i - 1].PhysicalStart + (Descriptor[i - 1].NumberOfPages << 12)))
        {
            Descriptor[i].PhysicalStart = Descriptor[i - 1].PhysicalStart;
            Descriptor[i].NumberOfPages += Descriptor[i - 1].NumberOfPages;
            Descriptor[i - 1].PhysicalStart = 0;
            Descriptor[i - 1].NumberOfPages = 0;
        }
    }
    Printk("PageCount = %d\n", PageCount);

    /*
        Step 2: 重新构建内存描述结构
    */
    MEMORY_DESCRIPTOR *NewDescriptor;
    U64 NewDescriptorSize = sizeof(MEMORY_DESCRIPTOR) * DescriptorCount;
    Printk("NewDescriptorSize = %d Bytes\n", NewDescriptorSize);
    // 找一个地方存着
    for (I32 i = 0; i < DescriptorCount; i++)
    {
        if (Descriptor[i].Type != FREE_MEMORY)
        {
            continue;
        }
        if ((Descriptor[i].NumberOfPages << 12) == NewDescriptorSize)
        {
            NewDescriptor = (MEMORY_DESCRIPTOR *)Descriptor[i].PhysicalStart;
            Descriptor[i].Type = OS_DATA;
            SetZero((PHYSICAL_ADDRESS)NewDescriptor, NewDescriptorSize);
            break;
        }
        else if ((Descriptor[i].NumberOfPages << 12) > NewDescriptorSize)
        {
            NewDescriptor = (MEMORY_DESCRIPTOR *)Descriptor[i].PhysicalStart;
            U32 NewDescriptorPageCount = NewDescriptorSize / 4096;
            if (NewDescriptorSize % 4096)
                NewDescriptorPageCount++;
            Descriptor[i].PhysicalStart += NewDescriptorPageCount << 12;
            Descriptor[i].NumberOfPages -= NewDescriptorPageCount;
            SetZero((PHYSICAL_ADDRESS)NewDescriptor, NewDescriptorPageCount << 12);
            break;
        }
    }
    // 开始构建
    PageCount = 0;
    I32 j = 0;
    for (I32 i = 0; i < DescriptorCount; i++)
    {
        PageCount += Descriptor[i].NumberOfPages;
        if (Descriptor[i].NumberOfPages == 0)
        {
            // 不应该存在大小为 0 的描述符
            Printk("Fuck, there are some problem!\n");
            continue;
        }
        // 如果 NewDescriptor[j] 没有初始化
        if (NewDescriptor[j].PageSize == 0)
        {
            NewDescriptor[j].Type = Descriptor[i].Type;
            NewDescriptor[j].PageSize = Descriptor[i].NumberOfPages;
            NewDescriptor[j].Start = Descriptor[i].PhysicalStart;
        }
        else
        // 如果 NewDescriptor[j] 初始化了
        {
            // 如果 NewDescriptor[j] 和 Descriptor[i] 类型相同而且连续, 就直接增加 NewDescriptor[j] 的长度
            if (NewDescriptor[j].Type == Descriptor[i].Type && (NewDescriptor[j].Start + (NewDescriptor[j].PageSize << 12)) == Descriptor[i].PhysicalStart)
            {
                NewDescriptor[j].PageSize += Descriptor[i].NumberOfPages;
            }
            else
            // 否则用下一个 NewDescriptor, 下一个 NewDescriptor 一定是未初始化的
            {
                j++;
                NewDescriptor[j].Type = Descriptor[i].Type;
                NewDescriptor[j].PageSize = Descriptor[i].NumberOfPages;
                NewDescriptor[j].Start = Descriptor[i].PhysicalStart;
            }
        }
    }
    Printk("|Type|       Start        |        End         | PageSize\n");
    U32 NewPageCount = 0;
    for (I32 i = 0; i <= j; i++)
    {
        NewPageCount += NewDescriptor[i].PageSize;
        Printk("| %02d | %#18llx | %#18llx | %d\n", NewDescriptor[i].Type, NewDescriptor[i].Start, NewDescriptor[i].Start + (NewDescriptor[i].PageSize << 12) - 1, NewDescriptor[i].PageSize);
    }

    Printk("PageCount = %d\n", PageCount);
    Printk("NewPageCount = %d\n", NewPageCount);
    Printk("NewDescriptor = %#x\n", NewDescriptor);
}

STATUS_CODE MemCopy(IN PHYSICAL_ADDRESS source, IN PHYSICAL_ADDRESS destination, IN U64 size)
{
    if (size == 0 || source == destination)
    {
        return 0;
    }

    U64 *p1 = (U64 *)source;
    U64 *p2 = (U64 *)destination;
    U32 remainder = size % 8;
    U8 *p3 = (U8 *)(source + size - remainder);
    U8 *p4 = (U8 *)(destination + size - remainder);

    // 尾覆盖类型
    if (destination > source && destination <= (source + size - 1))
    {
        for (I64 i = size / 8 - 1; i >= 0; i--)
        {
            p2[i] = p1[i];
        }
        for (I32 i = remainder - 1; i >= 0; i--)
        {
            p4[i] = p3[i];
        }
        return 0;
    }

    // 非尾覆盖类型
    for (I64 i = 0; i < size / 8; i++)
    {
        p2[i] = p1[i];
    }
    for (I32 i = 0; i < remainder; i++)
    {
        p4[i] = p3[i];
    }
    return 0;
}

STATUS_CODE SetZero(IN PHYSICAL_ADDRESS addr, IN U64 size)
{
    if (size == 0)
    {
        return 0;
    }
    U64 *p1 = (U64 *)addr;
    for (U64 i = 0; i < size / 8; i++)
    {
        p1[i] = 0x0000000000000000;
    }
    U32 remainder = size % 8;
    U8 *p2 = (U8 *)(addr + size - remainder);
    for (U32 i = 0; i < remainder; i++)
    {
        p2[i] = 0x00;
    }
}
