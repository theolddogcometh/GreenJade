/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1268: ChaCha20 nonce size constant.
 *
 * Surface (unique symbols):
 *   size_t gj_nonce_size_chacha20(void);
 *     — Always returns 12.
 *   size_t __gj_nonce_size_chacha20  (alias)
 *   __libcgj_batch1268_marker = "libcgj-batch1268"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1268_marker[] = "libcgj-batch1268";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nonce_size_chacha20 — ChaCha20 nonce size constant.
 *
 * Always returns 12.
 */
size_t
gj_nonce_size_chacha20(void)
{
	return 12u;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_nonce_size_chacha20(void)
    __attribute__((alias("gj_nonce_size_chacha20")));
