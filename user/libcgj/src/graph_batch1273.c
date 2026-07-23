/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1273: AES-128 key size constant.
 *
 * Surface (unique symbols):
 *   size_t gj_key_size_aes128(void);
 *     — Always returns 16.
 *   size_t __gj_key_size_aes128  (alias)
 *   __libcgj_batch1273_marker = "libcgj-batch1273"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1273_marker[] = "libcgj-batch1273";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_key_size_aes128 — AES-128 key size constant.
 *
 * Always returns 16.
 */
size_t
gj_key_size_aes128(void)
{
	return 16u;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_key_size_aes128(void)
    __attribute__((alias("gj_key_size_aes128")));
