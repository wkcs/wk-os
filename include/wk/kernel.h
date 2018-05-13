/*
 * Copyright (C) 2018 胡启航<Hu Qihang>
 *
 * Author: wkcs
 * 
 * Email: hqh2030@gmail.com, huqihan@live.com
 */

#ifndef __WK_KERNEL_H__
#define __WK_KERNEL_H__

#include <asm/types.h>

/*#define USHRT_MAX	((u16)(~0U))
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
#define WK_S64_MIN		((wk_s64_t)(-WK_S64_MAX - 1)) */

#undef NULL
#define NULL ((void *)0)

enum {
	false	= 0,
	true	= 1
};

#define wk_offsetof(type, member) (size_t)&(((type *)0)->member)

/**
 * wk_container_of - cast a member of a structure out to the containing structure
 * @ptr:	the pointer to the member.
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 *
 */
#define wk_container_of(ptr, type, member) ({				\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);   \
	(type *)( (size_t)__mptr - wk_offsetof(type, member) );})


#endif