/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch988: virtual page size constant query.
 *
 * Surface (unique symbols):
 *   uint32_t gj_page_size(void);
 *     — Return the assumed virtual memory page size in bytes (4096).
 *   uint32_t __gj_page_size  (alias)
 *   __libcgj_batch988_marker = "libcgj-batch988"
 *
 * Does NOT redefine gj_cacheline_size (batch987) or sysconf surfaces.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch988_marker[] = "libcgj-batch988";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_page_size — report assumed base page size.
 *
 * Always returns 4096 (standard 4 KiB page).
 */
uint32_t
gj_page_size(void)
{
	return 4096u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_page_size(void)
    __attribute__((alias("gj_page_size")));
