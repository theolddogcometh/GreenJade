/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9143: soft gzip magic catalog id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gzip_magic_id_9143(void);
 *     - Returns 0x1f8b (gzip member ID1/ID2 magic, RFC 1952). Soft
 *       catalog constant only; no gzip parse, emit, or I/O.
 *   uint32_t __gj_gzip_magic_id_9143  (alias)
 *   __libcgj_batch9143_marker = "libcgj-batch9143"
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

const char __libcgj_batch9143_marker[] = "libcgj-batch9143";

/* Soft gzip member ID1/ID2 magic catalog id (RFC 1952). */
#define B9143_GZIP_MAGIC_ID  0x1f8bu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9143_gzip_magic_id(void)
{
	return B9143_GZIP_MAGIC_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gzip_magic_id_9143 - soft gzip magic catalog id.
 *
 * Always returns 0x1f8b. Soft continuum constant; does not parse gzip
 * members or call libc. No parent wires.
 */
uint32_t
gj_gzip_magic_id_9143(void)
{
	(void)NULL;
	return b9143_gzip_magic_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gzip_magic_id_9143(void)
    __attribute__((alias("gj_gzip_magic_id_9143")));
