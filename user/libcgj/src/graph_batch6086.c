/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6086: SD path join buffer need (two components).
 *
 * Surface (unique symbols):
 *   uint32_t gj_sd_path_join_need_6086(uint32_t a_len, uint32_t b_len);
 *     - Returns a_len + b_len + 1 (separator) when no 32-bit wrap, else 0.
 *   uint32_t __gj_sd_path_join_need_6086  (alias)
 *   __libcgj_batch6086_marker = "libcgj-batch6086"
 *
 * Exclusive continuum CREATE-ONLY (6081-6090: SD card / microSD path
 * stubs). Unique gj_sd_path_join_need_6086 surface only; no multi-def.
 * Soft pure-integer join sizing; does not write a path. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6086_marker[] = "libcgj-batch6086";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6086_join_need(uint32_t uA, uint32_t uB)
{
	uint32_t uSum;

	/* need = a + b + 1 ('/'); reject any 32-bit wrap. */
	if (uA > (0xFFFFFFFFu - uB)) {
		return 0u;
	}
	uSum = uA + uB;
	if (uSum > (0xFFFFFFFFu - 1u)) {
		return 0u;
	}
	return uSum + 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sd_path_join_need_6086 - soft two-component SD path join size.
 *
 * a_len / b_len: byte lengths of path components (excluding separator).
 * Returns a_len + b_len + 1 on success, or 0 on uint32 overflow.
 * Does not allocate or copy. No parent wires.
 */
uint32_t
gj_sd_path_join_need_6086(uint32_t uALen, uint32_t uBLen)
{
	(void)NULL;
	return b6086_join_need(uALen, uBLen);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sd_path_join_need_6086(uint32_t uALen, uint32_t uBLen)
    __attribute__((alias("gj_sd_path_join_need_6086")));
