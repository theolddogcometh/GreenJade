/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3067: radians → degrees, Q16.16 (exclusive _u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_rad_to_deg_q16_u(uint32_t rad_q16);
 *     - deg_q16 = rad_q16 * 180 / π in unsigned Q16.16.
 *       Uses π ≈ 205887/65536; (rad * 180 * 65536) / 205887.
 *   uint32_t __gj_rad_to_deg_q16_u  (alias)
 *   __libcgj_batch3067_marker = "libcgj-batch3067"
 *
 * Exclusive angle-conversion _u surface; no multi-def. Inverse of
 * gj_deg_to_rad_q16_u (batch3066) but self-contained — no parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3067_marker[] = "libcgj-batch3067";

/*
 * π in unsigned Q16.16: floor(π * 65536 + 0.5) = 205887
 * (matched to batch3066 for round-trip consistency).
 *
 * deg_real = rad_real * 180 / π
 * deg_q16  = rad_q16 * 180 / π
 *          = rad_q16 * 180 * 65536 / 205887
 */
#define B3067_PI_Q16   ((uint32_t)205887u)
#define B3067_Q16_ONE  ((uint64_t)65536ull)

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rad_to_deg_q16_u — convert radians to degrees in unsigned Q16.16.
 *
 * deg = rad * 180 / π with both angles in Q16.16. Computed as
 * (rad_q16 * 180 * 2^16) / PI_Q16. No parent wires.
 */
uint32_t
gj_rad_to_deg_q16_u(uint32_t u32RadQ16)
{
	uint64_t u64Num;

	(void)NULL;
	u64Num = (uint64_t)u32RadQ16 * 180ull * B3067_Q16_ONE;
	return (uint32_t)(u64Num / (uint64_t)B3067_PI_Q16);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rad_to_deg_q16_u(uint32_t u32RadQ16)
    __attribute__((alias("gj_rad_to_deg_q16_u")));
