#ifndef __PMM_H__
#define __PMM_H__

#include <Sparrow/BaseType.h>

#define MAX_CONTIGUOUS_FRAME 13

typedef enum {
    FRAME,
    BLOCK_HEADER,
} M_Type;

typedef struct M_PAGE {
    // Type定义在M_Type中
    U8 Type;
    // 释放与否
    BOOL Freed;
    // 当前块大小2^M个页框，只有当Type == BLOCK_HEADER时有效
    U16 M;
    // 下一个块，只有当(Type == BLOCK_HEADER && Freed == True)时有效
    struct M_PAGE *Next;
    // 上一个块，只有当(Type == BLOCK_HEADER && Freed == True)时有效
    struct M_PAGE *Pre;
} M_PAGE;

typedef struct {
    // 块链表
    M_PAGE *BlockList[MAX_CONTIGUOUS_FRAME];
    // 页框域中当前的空闲页框数量
    U64 FreePageNumber;
    // 页框域中管理的页框数量
    U64 MaxPageNumber;
} M_ZONE;

typedef struct {
    // M_PAGE数组
    M_PAGE *PageMap;
    // 最大物理地址
    PHYSICAL_ADDRESS MaxPhAddr;
    // 最大页地址
    PHYSICAL_ADDRESS MaxPageAddr;
    // 页框的数量
    U64 FrameNumber;
    // 通用页框域
    M_ZONE NormalZone;
} PMM;

// 声明在 PMM.c 中
extern PMM gPMM;

STATUS_CODE InitPMM(IN MEMORY_MAP *MM);

STATUS_CODE FreeBlock(IN PHYSICAL_ADDRESS PhyAddr);

#endif
