/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8220: ctype-class wave batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8220(void);
 *     - Returns the compile-time graph batch number for this TU (8220).
 *   uint32_t __gj_batch_id_8220  (alias)
 *   __libcgj_batch8220_marker = "libcgj-batch8220"
 *
 * Ctype class stubs wave closer (8211-8220). Distinct from
 * gj_batch_id / gj_batch_id_N of other TUs — unique _8220 surface;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8220_marker[] = "libcgj-batch8220";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8220_id(void)
{
	return 8220u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8220 - report this TU's graph batch number.
 *
 * Always returns 8220.
 */
uint32_t
gj_batch_id_8220(void)
{
	(void)NULL;
	return b8220_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8220(void)
    __attribute__((alias("gj_batch_id_8220")));
