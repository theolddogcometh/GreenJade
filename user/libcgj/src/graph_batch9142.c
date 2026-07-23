/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9142: soft zlib level-ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_zlib_level_ok_u_9142(void);
 *     - Returns 1 (zlib compression-level soft-id continuum ok).
 *       Pure-data product tag; does not select or apply levels.
 *   uint32_t __gj_zlib_level_ok_u_9142  (alias)
 *   __libcgj_batch9142_marker = "libcgj-batch9142"
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

const char __libcgj_batch9142_marker[] = "libcgj-batch9142";

#define B9142_ZLIB_LEVEL_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9142_zlib_level_ok(void)
{
	return B9142_ZLIB_LEVEL_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_zlib_level_ok_u_9142 - zlib level soft-id continuum ok flag.
 *
 * Always returns 1. Soft pure-data product tag; does not set compression
 * levels or call libc. No parent wires.
 */
uint32_t
gj_zlib_level_ok_u_9142(void)
{
	(void)NULL;
	return b9142_zlib_level_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_zlib_level_ok_u_9142(void)
    __attribute__((alias("gj_zlib_level_ok_u_9142")));
