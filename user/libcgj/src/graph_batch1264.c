/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1264: SHA-256 digest size constant.
 *
 * Surface (unique symbols):
 *   size_t gj_digest_size_sha256(void);
 *     — Always returns 32.
 *   size_t __gj_digest_size_sha256  (alias)
 *   __libcgj_batch1264_marker = "libcgj-batch1264"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1264_marker[] = "libcgj-batch1264";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_digest_size_sha256 — SHA-256 digest size constant.
 *
 * Always returns 32.
 */
size_t
gj_digest_size_sha256(void)
{
	return 32u;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_digest_size_sha256(void)
    __attribute__((alias("gj_digest_size_sha256")));
