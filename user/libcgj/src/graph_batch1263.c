/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1263: HMAC-SHA-256 block size constant.
 *
 * Surface (unique symbols):
 *   size_t gj_hmac_block_size_sha256(void);
 *     — Always returns 64.
 *   size_t __gj_hmac_block_size_sha256  (alias)
 *   __libcgj_batch1263_marker = "libcgj-batch1263"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1263_marker[] = "libcgj-batch1263";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hmac_block_size_sha256 — HMAC-SHA-256 block size constant.
 *
 * Always returns 64.
 */
size_t
gj_hmac_block_size_sha256(void)
{
	return 64u;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_hmac_block_size_sha256(void)
    __attribute__((alias("gj_hmac_block_size_sha256")));
