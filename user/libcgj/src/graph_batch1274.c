/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1274: AES-256 key size constant.
 *
 * Surface (unique symbols):
 *   size_t gj_key_size_aes256(void);
 *     — Always returns 32.
 *   size_t __gj_key_size_aes256  (alias)
 *   __libcgj_batch1274_marker = "libcgj-batch1274"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1274_marker[] = "libcgj-batch1274";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_key_size_aes256 — AES-256 key size constant.
 *
 * Always returns 32.
 */
size_t
gj_key_size_aes256(void)
{
	return 32u;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_key_size_aes256(void)
    __attribute__((alias("gj_key_size_aes256")));
