/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9150: compress soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9150(void);
 *     - Returns the compile-time graph batch number for this TU (9150).
 *   uint32_t __gj_batch_id_9150  (alias)
 *   __libcgj_batch9150_marker = "libcgj-batch9150"
 *
 * Exclusive continuum CREATE-ONLY (9141-9150: compress soft id stubs —
 * zlib_window_id_9141, zlib_level_ok_u_9142, gzip_magic_id_9143,
 * deflate_ready_u_9144, inflate_ready_u_9145, lz4_ready_u_9146,
 * zstd_ready_u_9147, brotli_ready_u_9148, compress_ready_u_9149,
 * batch_id_9150).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128. No compression implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9150_marker[] = "libcgj-batch9150";

#define B9150_BATCH_ID  9150u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9150_id(void)
{
	return B9150_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9150 - report this TU's graph batch number.
 *
 * Always returns 9150.
 */
uint32_t
gj_batch_id_9150(void)
{
	(void)NULL;
	return b9150_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9150(void)
    __attribute__((alias("gj_batch_id_9150")));
