/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1276: default salt size constant.
 *
 * Surface (unique symbols):
 *   size_t gj_salt_size_default(void);
 *     — Always returns 16.
 *   size_t __gj_salt_size_default  (alias)
 *   __libcgj_batch1276_marker = "libcgj-batch1276"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1276_marker[] = "libcgj-batch1276";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_salt_size_default — default salt size constant.
 *
 * Always returns 16.
 */
size_t
gj_salt_size_default(void)
{
	return 16u;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_salt_size_default(void)
    __attribute__((alias("gj_salt_size_default")));
