/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1265: SHA-512 digest size constant.
 *
 * Surface (unique symbols):
 *   size_t gj_digest_size_sha512(void);
 *     — Always returns 64.
 *   size_t __gj_digest_size_sha512  (alias)
 *   __libcgj_batch1265_marker = "libcgj-batch1265"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1265_marker[] = "libcgj-batch1265";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_digest_size_sha512 — SHA-512 digest size constant.
 *
 * Always returns 64.
 */
size_t
gj_digest_size_sha512(void)
{
	return 64u;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_digest_size_sha512(void)
    __attribute__((alias("gj_digest_size_sha512")));
