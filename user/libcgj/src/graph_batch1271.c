/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1271: AES-GCM AEAD tag/overhead size.
 *
 * Surface (unique symbols):
 *   size_t gj_aead_overhead_gcm(void);
 *     — Always returns 16.
 *   size_t __gj_aead_overhead_gcm  (alias)
 *   __libcgj_batch1271_marker = "libcgj-batch1271"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1271_marker[] = "libcgj-batch1271";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_aead_overhead_gcm — AES-GCM AEAD tag/overhead size.
 *
 * Always returns 16.
 */
size_t
gj_aead_overhead_gcm(void)
{
	return 16u;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_aead_overhead_gcm(void)
    __attribute__((alias("gj_aead_overhead_gcm")));
