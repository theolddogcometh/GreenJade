/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8430: hash/fnv-ish wave batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8430(void);
 *     - Returns the compile-time graph batch number for this TU (8430).
 *   uint32_t __gj_batch_id_8430  (alias)
 *   __libcgj_batch8430_marker = "libcgj-batch8430"
 *
 * Hash/fnv-ish integer stubs wave closer (8421-8430). Distinct from
 * gj_batch_id / gj_batch_id_N of other TUs — unique _8430 surface;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8430_marker[] = "libcgj-batch8430";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8430_id(void)
{
	return 8430u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8430 - report this TU's graph batch number.
 *
 * Always returns 8430.
 */
uint32_t
gj_batch_id_8430(void)
{
	(void)NULL;
	return b8430_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8430(void)
    __attribute__((alias("gj_batch_id_8430")));
