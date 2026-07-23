/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3066: degrees → radians, Q16.16 (exclusive _u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_deg_to_rad_q16_u(uint32_t deg_q16);
 *     - rad_q16 = deg_q16 * π / 180 in unsigned Q16.16.
 *       Uses π ≈ 205887/65536; (deg * 205887) / (65536 * 180).
 *   uint32_t __gj_deg_to_rad_q16_u  (alias)
 *   __libcgj_batch3066_marker = "libcgj-batch3066"
 *
 * Exclusive angle-conversion _u surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3066_marker[] = "libcgj-batch3066";

/*
 * π in unsigned Q16.16: floor(π * 65536 + 0.5) = 205887
 * (3.141592653589793 * 65536 ≈ 205887.416)
 *
 * rad_real = deg_real * π / 180
 * rad_q16  = deg_q16 * π / 180
 *          = deg_q16 * 205887 / (65536 * 180)
 */
#define B3066_PI_Q16   ((uint32_t)205887u)
#define B3066_Q16_ONE  ((uint64_t)65536ull)

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deg_to_rad_q16_u — convert degrees to radians in unsigned Q16.16.
 *
 * rad = deg * π / 180 with both angles in Q16.16. Computed as
 * (deg_q16 * PI_Q16) / (2^16 * 180). No parent wires.
 */
uint32_t
gj_deg_to_rad_q16_u(uint32_t u32DegQ16)
{
	uint64_t u64Num;

	(void)NULL;
	u64Num = (uint64_t)u32DegQ16 * (uint64_t)B3066_PI_Q16;
	return (uint32_t)(u64Num / (B3066_Q16_ONE * 180ull));
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deg_to_rad_q16_u(uint32_t u32DegQ16)
    __attribute__((alias("gj_deg_to_rad_q16_u")));
