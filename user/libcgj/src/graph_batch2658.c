/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2658: gj_fletcher32_final_u - Fletcher-32 final.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fletcher32_final_u(uint64_t state);
 *     - Finalize a running Fletcher-32 packed u64 state (batch2657 layout).
 *       If a pending odd byte is set, treat it as a LE word with high
 *       byte zero. Returns (sum2 << 16) | sum1 with both sums in 0..65534.
 *   uint32_t __gj_fletcher32_final_u  (alias)
 *   __libcgj_batch2658_marker = "libcgj-batch2658"
 *
 * Companion to gj_fletcher32_update_u (batch2657). Distinct from one-shot
 * gj_fletcher32 (batch183). Streaming checksum wave (2651-2660).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2658_marker[] = "libcgj-batch2658";

#define B2658_SUM2_SHIFT  16
#define B2658_PEND_SHIFT  32
#define B2658_FLAG_SHIFT  40
#define B2658_HAS_PEND    (1ull << B2658_FLAG_SHIFT)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2658_mod65535(uint32_t u32X)
{
	u32X = (u32X & 0xffffu) + (u32X >> 16);
	u32X = (u32X & 0xffffu) + (u32X >> 16);
	if (u32X >= 65535u) {
		u32X -= 65535u;
	}
	return u32X;
}

static uint32_t
b2658_final(uint64_t u64State)
{
	uint32_t u32Sum1;
	uint32_t u32Sum2;
	unsigned char u8Pend;
	int fHasPend;

	u32Sum1 = (uint32_t)(u64State & 0xffffu);
	u32Sum2 = (uint32_t)((u64State >> B2658_SUM2_SHIFT) & 0xffffu);
	fHasPend = ((u64State & B2658_HAS_PEND) != 0ull) ? 1 : 0;
	u8Pend = (unsigned char)((u64State >> B2658_PEND_SHIFT) & 0xffu);

	if (fHasPend != 0) {
		/* Trailing odd byte as word with high half zero. */
		u32Sum1 += (uint32_t)u8Pend;
		u32Sum2 += u32Sum1;
	}

	u32Sum1 = b2658_mod65535(u32Sum1);
	u32Sum2 = b2658_mod65535(u32Sum2);
	return (u32Sum2 << 16) | u32Sum1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fletcher32_final_u - finalize a streaming Fletcher-32.
 *
 * state: packed u64 from gj_fletcher32_update_u (or 0 if empty → 0)
 *
 * Returns the 32-bit Fletcher-32 value (sum2 << 16) | sum1.
 */
uint32_t
gj_fletcher32_final_u(uint64_t u64State)
{
	(void)NULL;
	return b2658_final(u64State);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fletcher32_final_u(uint64_t u64State)
    __attribute__((alias("gj_fletcher32_final_u")));
