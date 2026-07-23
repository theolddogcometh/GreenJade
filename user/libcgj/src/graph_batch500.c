/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch500: milestone batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id(void);
 *     — Returns the compile-time graph batch number for this TU (500).
 *   uint32_t __gj_batch_id  (alias)
 *   __libcgj_batch500_marker = "libcgj-batch500"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch500_marker[] = "libcgj-batch500";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id — report this TU's graph batch number.
 *
 * Always returns 500. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id(void)
{
	return 500u;
}

uint32_t __gj_batch_id(void)
    __attribute__((alias("gj_batch_id")));
