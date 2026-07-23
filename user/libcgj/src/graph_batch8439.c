/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8439: CRC ok / match integer stub (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_crc_ok_u_8439(uint32_t crc, uint32_t expect);
 *     - Return 1 if crc equals expect; else 0. Soft integrity probe
 *       for CRC residual vs expected check value.
 *   uint32_t __gj_crc_ok_u_8439  (alias)
 *   __libcgj_batch8439_marker = "libcgj-batch8439"
 *
 * Exclusive continuum CREATE-ONLY (8431-8440: crc/poly integer stubs).
 * Unique gj_crc_ok_u_8439 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8439_marker[] = "libcgj-batch8439";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8439_crc_ok(uint32_t u32Crc, uint32_t u32Expect)
{
	if (u32Crc == u32Expect) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc_ok_u_8439 - CRC residual match probe.
 *
 * crc: computed residual (or final digest)
 * expect: expected check value
 *
 * Returns 1 on match, 0 otherwise. Soft integer helper. No parent wires.
 */
uint32_t
gj_crc_ok_u_8439(uint32_t u32Crc, uint32_t u32Expect)
{
	(void)NULL;
	return b8439_crc_ok(u32Crc, u32Expect);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_crc_ok_u_8439(uint32_t u32Crc, uint32_t u32Expect)
    __attribute__((alias("gj_crc_ok_u_8439")));
