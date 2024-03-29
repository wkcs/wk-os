/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#ifndef __ASM_TYPES_H__
#define __ASM_TYPES_H__

#include <asm_types.h>

typedef __wk_s8_t wk_s8_t;
typedef __wk_u8_t wk_u8_t;

typedef __wk_s16_t wk_s16_t;
typedef __wk_u16_t wk_u16_t;

typedef __wk_s32_t wk_s32_t;
typedef __wk_u32_t wk_u32_t;

typedef __wk_s64_t wk_s64_t;
typedef __wk_u64_t wk_u64_t;

/**********************************/
typedef __wk_s8_t __s8;
typedef __wk_u8_t __u8;

typedef __wk_s16_t __s16;
typedef __wk_u16_t __u16;

typedef __wk_s32_t __s32;
typedef __wk_u32_t __u32;

typedef __wk_s64_t __s64;
typedef __wk_u64_t __u64;

/***********************************/
typedef __wk_s8_t s8;
typedef __wk_u8_t u8;

typedef __wk_s16_t s16;
typedef __wk_u16_t u16;

typedef __wk_s32_t s32;
typedef __wk_u32_t u32;

typedef __wk_s64_t s64;
typedef __wk_u64_t u64;

/***********************************/
typedef __wk_s8_t int8_t;
typedef __wk_u8_t uint8_t;

typedef __wk_s16_t int16_t;
typedef __wk_u16_t uint16_t;

typedef __wk_s32_t int32_t;
typedef __wk_u32_t uint32_t;

typedef __wk_s64_t int64_t;
typedef __wk_u64_t uint64_t;
 

typedef __wk_size_t size_t;

typedef __wk_u8_t bool;

typedef __dma_addr_t dma_addr_t;
typedef __dma64_addr_t dma64_addr_t;

typedef __addr_t addr_t;

typedef struct {
    int counter;
} atomic_t;

#define USHRT_MAX	((u16)(~0U))
#define SHRT_MAX	((s16)(USHRT_MAX>>1))
#define SHRT_MIN	((s16)(-SHRT_MAX - 1))
#define INT_MAX		((int)(~0U>>1))
#define INT_MIN		(-INT_MAX - 1)
#define UINT_MAX	(~0U)
#define LONG_MAX	((long)(~0UL>>1))
#define LONG_MIN	(-LONG_MAX - 1)
#define ULONG_MAX	(~0UL)
#define LLONG_MAX	((long long)(~0ULL>>1))
#define LLONG_MIN	(-LLONG_MAX - 1)
#define ULLONG_MAX	(~0ULL)
#define SIZE_MAX	(~(size_t)0)

#define U8_MAX		((u8)~0U)
#define S8_MAX		((s8)(U8_MAX>>1))
#define S8_MIN		((s8)(-S8_MAX - 1))
#define U16_MAX		((u16)~0U)
#define S16_MAX		((s16)(U16_MAX>>1))
#define S16_MIN		((s16)(-S16_MAX - 1))
#define U32_MAX		((u32)~0U)
#define S32_MAX		((s32)(U32_MAX>>1))
#define S32_MIN		((s32)(-S32_MAX - 1))
#define U64_MAX		((u64)~0ULL)
#define S64_MAX		((s64)(U64_MAX>>1))
#define S64_MIN		((s64)(-S64_MAX - 1))

#define WK_U8_MAX		((wk_u8_t)~0U)
#define WK_S8_MAX		((wk_s8_t)(WK_U8_MAX>>1))
#define WK_S8_MIN		((wk_s8_t)(-WK_S8_MAX - 1))
#define WK_U16_MAX		((wk_u16_t)~0U)
#define WK_S16_MAX		((wk_s16_t)(WK_U16_MAX>>1))
#define WK_S16_MIN		((wk_s16_t)(-WK_S16_MAX - 1))
#define WK_U32_MAX		((wk_u32_t)~0U)
#define WK_S32_MAX		((wk_s32_t)(WK_U32_MAX>>1))
#define WK_S32_MIN		((wk_s32_t)(-WK_S32_MAX - 1))
#define WK_U64_MAX		((wk_u64_t)~0ULL)
#define WK_S64_MAX		((wk_s64_t)(WK_U64_MAX>>1))
#define WK_S64_MIN		((wk_s64_t)(-WK_S64_MAX - 1))

#undef NULL
#define NULL ((void *)0)

enum {
	false	= 0,
	true	= 1
};

#endif