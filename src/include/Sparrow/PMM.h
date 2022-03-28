#ifndef __PMM_H__
#define __PMM_H__

#include <Sparrow/BaseType.h>

typedef struct
{
    // 位图所在物理地址
    U64 *BitMap;
    // 节点数
    U64 NumberOfNodes;

    // 最大物理地址
    PHYSICAL_ADDRESS MaxAddr;
    // 最大页地址
    PHYSICAL_ADDRESS MaxPageAddr;
} PMM;

// 声明在 PMM.c 中
extern PMM gPMM;

typedef struct
{

} M_PAGE;

typedef struct M_BLOCK {
    U64             PAGE_ID;
    struct M_BLOCK *Next;
} M_BLOCK;

STATUS_CODE InitPMM(IN PHYSICAL_ADDRESS MaxAddress, IN PHYSICAL_ADDRESS BitMapAddr);

STATUS_CODE FreePages(IN PHYSICAL_ADDRESS PAddr, IN U64 NumberOfPages);

STATUS_CODE AlloPages(IN OUT PHYSICAL_ADDRESS *PAddr, IN U64 NumberOfPages);
#endif
