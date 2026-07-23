/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1267: MD5 digest size constant.
 *
 * Surface (unique symbols):
 *   size_t gj_digest_size_md5(void);
 *     — Always returns 16.
 *   size_t __gj_digest_size_md5  (alias)
 *   __libcgj_batch1267_marker = "libcgj-batch1267"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1267_marker[] = "libcgj-batch1267";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_digest_size_md5 — MD5 digest size constant.
 *
 * Always returns 16.
 */
size_t
gj_digest_size_md5(void)
{
	return 16u;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_digest_size_md5(void)
    __attribute__((alias("gj_digest_size_md5")));
