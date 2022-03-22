#ifndef __UEFI_H__
#define __UEFI_H__

typedef unsigned char      UINT8;
typedef UINT8              CHAR8;
typedef unsigned int       UINT32;
typedef unsigned long long UINT64;
typedef UINT64             UINTN;
typedef UINT64             EFI_PHYSICAL_ADDRESS;
typedef UINT64             EFI_VIRTUAL_ADDRESS;

#pragma pack(1)
typedef struct
{
    UINT8 Blue;
    UINT8 Green;
    UINT8 Red;
    UINT8 Reserved;
} EFI_GRAPHICS_OUTPUT_BLT_PIXEL;
#pragma pack()

typedef struct
{
    ///
    /// Type of the memory region.
    /// Type EFI_MEMORY_TYPE is defined in the
    /// AllocatePages() function description.
    ///
    UINT32 Type;
    ///
    /// Physical address of the first byte in the memory region. PhysicalStart must be
    /// aligned on a 4 KiB boundary, and must not be above 0xfffffffffffff000. Type
    /// EFI_PHYSICAL_ADDRESS is defined in the AllocatePages() function description
    ///
    EFI_PHYSICAL_ADDRESS PhysicalStart;
    ///
    /// Virtual address of the first byte in the memory region.
    /// VirtualStart must be aligned on a 4 KiB boundary,
    /// and must not be above 0xfffffffffffff000.
    ///
    EFI_VIRTUAL_ADDRESS VirtualStart;
    ///
    /// NumberOfPagesNumber of 4 KiB pages in the memory region.
    /// NumberOfPages must not be 0, and must not be any value
    /// that would represent a memory page with a start address,
    /// either physical or virtual, above 0xfffffffffffff000.
    ///
    UINT64 NumberOfPages;
    ///
    /// Attributes of the memory region that describe the bit mask of capabilities
    /// for that memory region, and not necessarily the current settings for that
    /// memory region.
    ///
    UINT64 Attribute;
    UINT64 Reserved;
} EFI_MEMORY_DESCRIPTOR;

typedef enum {
    ///
    /// 保留
    ///
    EfiReservedMemoryType,
    ///
    /// 分配给OS加载器的代码
    /// (Note that UEFI OS loaders are UEFI applications.)
    ///
    EfiLoaderCode,
    ///
    /// 分配给OS加载器的数据，应用程序分配内存的默认类型
    ///
    EfiLoaderData,
    ///
    /// The code portions of a loaded Boot Services Driver.
    ///
    EfiBootServicesCode,
    ///
    /// The data portions of a loaded Boot Serves Driver, and the default data
    /// allocation type used by a Boot Services Driver to allocate pool memory.
    ///
    EfiBootServicesData,
    ///
    /// The code portions of a loaded Runtime Services Driver.
    ///
    EfiRuntimeServicesCode,
    ///
    /// The data portions of a loaded Runtime Services Driver and the default
    /// data allocation type used by a Runtime Services Driver to allocate pool memory.
    ///
    EfiRuntimeServicesData,
    ///
    /// 可分配内存
    ///
    EfiConventionalMemory,
    ///
    /// 内存区域存在错误，不能使用
    ///
    EfiUnusableMemory,
    ///
    /// 用于存放ACPI表
    ///
    EfiACPIReclaimMemory,
    ///
    /// 保留给固件使用
    ///
    EfiACPIMemoryNVS,
    ///
    /// MMIO内存，可被运行时服务使用
    ///
    EfiMemoryMappedIO,
    ///
    /// 	MMIO端口，被CPU用于转换内存周期到IO周期
    ///
    EfiMemoryMappedIOPortSpace,
    ///
    /// 保留给固件使用
    ///
    EfiPalCode,
    ///
    /// 作为EfiConventionalMemory工作的内存区域
    ///
    EfiPersistentMemory,
    EfiMaxMemoryType
} EFI_MEMORY_TYPE;

typedef struct
{
    UINTN                  MemoryMapSize;
    EFI_MEMORY_DESCRIPTOR *MemoryMap;
    UINTN                  MapKey;
    UINTN                  DescriptorSize;
    UINT32                 DescriptorVersion;
} MEMORY_MAP;

typedef struct
{
    PHYSICAL_ADDRESS FrameBufferBase;
    UINT64           FrameBufferSize;
    UINT32           HorizontalResolution;
    UINT32           VerticalResolution;
} GRAPHIC_PARAMETER;

typedef struct
{
    PHYSICAL_ADDRESS Address;
    UINT64           Size;
    UINT64           Pages;
} File;

typedef struct
{
    MEMORY_MAP *       MM;
    GRAPHIC_PARAMETER *GP;
    File *             KernelFile;
    File *             FontFile;
} BOOT_PARAMETER;

#endif
