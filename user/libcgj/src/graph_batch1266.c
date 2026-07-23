/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1266: SHA-1 digest size constant.
 *
 * Surface (unique symbols):
 *   size_t gj_digest_size_sha1(void);
 *     — Always returns 20.
 *   size_t __gj_digest_size_sha1  (alias)
 *   __libcgj_batch1266_marker = "libcgj-batch1266"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1266_marker[] = "libcgj-batch1266";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_digest_size_sha1 — SHA-1 digest size constant.
 *
 * Always returns 20.
 */
size_t
gj_digest_size_sha1(void)
{
	return 20u;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_digest_size_sha1(void)
    __attribute__((alias("gj_digest_size_sha1")));
