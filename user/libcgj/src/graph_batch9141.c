/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9141: soft zlib windowBits catalog id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_zlib_window_id_9141(void);
 *     - Returns 15 (zlib default windowBits / MAX_WBITS, RFC 1950/1951).
 *       Soft catalog constant only; no inflate/deflate or I/O.
 *   uint32_t __gj_zlib_window_id_9141  (alias)
 *   __libcgj_batch9141_marker = "libcgj-batch9141"
 *
 * Exclusive continuum CREATE-ONLY (9141-9150: compress soft id stubs —
 * zlib_window_id_9141, zlib_level_ok_u_9142, gzip_magic_id_9143,
 * deflate_ready_u_9144, inflate_ready_u_9145, lz4_ready_u_9146,
 * zstd_ready_u_9147, brotli_ready_u_9148, compress_ready_u_9149,
 * batch_id_9150).
 * Unique surface only; no multi-def. No parent wires. No __int128.
 * No compression implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9141_marker[] = "libcgj-batch9141";

/* Soft zlib default windowBits catalog id (MAX_WBITS). */
#define B9141_ZLIB_WINDOW_ID  15u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9141_zlib_window_id(void)
{
	return B9141_ZLIB_WINDOW_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_zlib_window_id_9141 - soft zlib windowBits catalog id.
 *
 * Always returns 15. Soft continuum constant; does not configure zlib
 * streams or compress data. No parent wires.
 */
uint32_t
gj_zlib_window_id_9141(void)
{
	(void)NULL;
	return b9141_zlib_window_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_zlib_window_id_9141(void)
    __attribute__((alias("gj_zlib_window_id_9141")));
