/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1269: Poly1305 tag size constant.
 *
 * Surface (unique symbols):
 *   size_t gj_tag_size_poly1305(void);
 *     — Always returns 16.
 *   size_t __gj_tag_size_poly1305  (alias)
 *   __libcgj_batch1269_marker = "libcgj-batch1269"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1269_marker[] = "libcgj-batch1269";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tag_size_poly1305 — Poly1305 tag size constant.
 *
 * Always returns 16.
 */
size_t
gj_tag_size_poly1305(void)
{
	return 16u;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_tag_size_poly1305(void)
    __attribute__((alias("gj_tag_size_poly1305")));
