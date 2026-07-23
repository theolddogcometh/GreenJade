/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1275: AES-GCM IV/nonce size constant.
 *
 * Surface (unique symbols):
 *   size_t gj_iv_size_aes_gcm(void);
 *     — Always returns 12.
 *   size_t __gj_iv_size_aes_gcm  (alias)
 *   __libcgj_batch1275_marker = "libcgj-batch1275"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1275_marker[] = "libcgj-batch1275";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_iv_size_aes_gcm — AES-GCM IV/nonce size constant.
 *
 * Always returns 12.
 */
size_t
gj_iv_size_aes_gcm(void)
{
	return 12u;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_iv_size_aes_gcm(void)
    __attribute__((alias("gj_iv_size_aes_gcm")));
