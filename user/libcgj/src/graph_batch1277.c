/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1277: default RNG reseed interval constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rng_reseed_interval(void);
 *     — Return the default CSPRNG reseed interval in bytes of
 *       output (65536).
 *   uint32_t __gj_rng_reseed_interval  (alias)
 *   __libcgj_batch1277_marker = "libcgj-batch1277"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1277_marker[] = "libcgj-batch1277";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rng_reseed_interval — default reseed period (bytes of RNG output).
 *
 * Always returns 65536.
 */
uint32_t
gj_rng_reseed_interval(void)
{
	return 65536u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rng_reseed_interval(void)
    __attribute__((alias("gj_rng_reseed_interval")));
