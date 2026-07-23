/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8420: wchar-class wave batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8420(void);
 *     - Returns the compile-time graph batch number for this TU (8420).
 *   uint32_t __gj_batch_id_8420  (alias)
 *   __libcgj_batch8420_marker = "libcgj-batch8420"
 *
 * Wchar class stubs wave closer (8411-8420). Distinct from
 * gj_batch_id / gj_batch_id_N of other TUs — unique _8420 surface;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8420_marker[] = "libcgj-batch8420";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8420_id(void)
{
	return 8420u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8420 - report this TU's graph batch number.
 *
 * Always returns 8420.
 */
uint32_t
gj_batch_id_8420(void)
{
	(void)NULL;
	return b8420_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8420(void)
    __attribute__((alias("gj_batch_id_8420")));
