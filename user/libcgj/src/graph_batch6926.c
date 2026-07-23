/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6926: copy_file_range zero-length ok lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cfr_len_zero_ok_6926(void);
 *     - Returns 1 (soft: zero-length copy_file_range is a valid
 *       short-circuit / no-op that yields zero bytes transferred).
 *   uint32_t __gj_cfr_len_zero_ok_6926  (alias)
 *   __libcgj_batch6926_marker = "libcgj-batch6926"
 *
 * Exclusive continuum CREATE-ONLY (6921-6930: copy_file_range stubs).
 * Unique gj_cfr_len_zero_ok_6926 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6926_marker[] = "libcgj-batch6926";

/* Soft: len == 0 is a valid no-op short-circuit. */
#define B6926_LEN_ZERO_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6926_len_zero_ok(void)
{
	return B6926_LEN_ZERO_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cfr_len_zero_ok_6926 - soft zero-length acceptance lamp.
 *
 * Always returns 1. Pure-data catalog stub for product copy_file_range
 * length policy; does not perform I/O. No parent wires.
 */
uint32_t
gj_cfr_len_zero_ok_6926(void)
{
	(void)NULL;
	return b6926_len_zero_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cfr_len_zero_ok_6926(void)
    __attribute__((alias("gj_cfr_len_zero_ok_6926")));
