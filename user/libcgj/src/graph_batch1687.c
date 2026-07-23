/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1687: Porter-Duff source-over alpha blend (u8).
 *
 * Surface (unique symbols):
 *   int gj_blend_over_u8(uint8_t *dst_rgba, const uint8_t *src_rgba);
 *     — Non-premultiplied RGBA8 src-over. dst and src are 4-byte
 *       {R,G,B,A}. Result written in-place to dst. NULL → -1; else 0.
 *   int __gj_blend_over_u8  (alias)
 *   __libcgj_batch1687_marker = "libcgj-batch1687"
 *
 * Color / pixel exclusive integer wave. Does NOT redefine prior batch
 * symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1687_marker[] = "libcgj-batch1687";

/* ---- freestanding helpers ---------------------------------------------- */

/* Divide by 255 with rounding: (x + 127) / 255 for wide products. */
static uint32_t
b1687_div255(uint32_t u32X)
{
	return (u32X + 127u) / 255u;
}

/*
 * Blend one non-premultiplied channel:
 *   out_c = (src_c * src_a + dst_c * dst_a * (255 - src_a) / 255) / out_a
 * when out_a != 0; else 0.
 */
static uint8_t
b1687_over_chan(uint8_t u8SrcC, uint8_t u8SrcA, uint8_t u8DstC, uint8_t u8DstA,
    uint8_t u8OutA)
{
	uint32_t u32SrcW;
	uint32_t u32DstW;
	uint32_t u32Num;
	uint32_t u32Out;

	if (u8OutA == 0u) {
		return 0u;
	}

	u32SrcW = (uint32_t)u8SrcC * (uint32_t)u8SrcA;
	u32DstW = (uint32_t)u8DstC * (uint32_t)u8DstA;
	u32DstW = b1687_div255(u32DstW * (255u - (uint32_t)u8SrcA));
	u32Num = u32SrcW + u32DstW;
	/* Round-to-nearest when normalizing by out alpha. */
	u32Out = (u32Num + ((uint32_t)u8OutA / 2u)) / (uint32_t)u8OutA;
	if (u32Out > 255u) {
		return 255u;
	}
	return (uint8_t)u32Out;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_blend_over_u8 — Porter-Duff source-over on non-premultiplied RGBA8.
 *
 * out_a = src_a + dst_a * (255 - src_a) / 255
 * out_c = (src_c * src_a + dst_c * dst_a * (255 - src_a) / 255) / out_a
 *
 * pDstRgba / pSrcRgba: 4 bytes each {R,G,B,A}. Result → pDstRgba.
 * Returns 0 on success, -1 if either pointer is NULL.
 */
int
gj_blend_over_u8(uint8_t *pDstRgba, const uint8_t *pSrcRgba)
{
	uint8_t u8SrcR, u8SrcG, u8SrcB, u8SrcA;
	uint8_t u8DstR, u8DstG, u8DstB, u8DstA;
	uint8_t u8OutA;
	uint32_t u32A;

	if (pDstRgba == NULL || pSrcRgba == NULL) {
		return -1;
	}

	u8SrcR = pSrcRgba[0];
	u8SrcG = pSrcRgba[1];
	u8SrcB = pSrcRgba[2];
	u8SrcA = pSrcRgba[3];
	u8DstR = pDstRgba[0];
	u8DstG = pDstRgba[1];
	u8DstB = pDstRgba[2];
	u8DstA = pDstRgba[3];

	u32A = (uint32_t)u8SrcA +
	    b1687_div255((uint32_t)u8DstA * (255u - (uint32_t)u8SrcA));
	if (u32A > 255u) {
		u32A = 255u;
	}
	u8OutA = (uint8_t)u32A;

	pDstRgba[0] = b1687_over_chan(u8SrcR, u8SrcA, u8DstR, u8DstA, u8OutA);
	pDstRgba[1] = b1687_over_chan(u8SrcG, u8SrcA, u8DstG, u8DstA, u8OutA);
	pDstRgba[2] = b1687_over_chan(u8SrcB, u8SrcA, u8DstB, u8DstA, u8OutA);
	pDstRgba[3] = u8OutA;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_blend_over_u8(uint8_t *pDstRgba, const uint8_t *pSrcRgba)
    __attribute__((alias("gj_blend_over_u8")));
