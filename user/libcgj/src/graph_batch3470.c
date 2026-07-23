/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3470: exclusive wave batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3470(void);
 *     - Returns the compile-time graph batch number for this TU (3470).
 *   uint32_t __gj_batch_id_3470  (alias)
 *   __libcgj_batch3470_marker = "libcgj-batch3470"
 *
 * CREATE-ONLY exclusive continuum wave (3461-3470). Distinct from other
 * batch_id_* symbols — unique surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3470_marker[] = "libcgj-batch3470";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3470_id(void)
{
	return 3470u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3470 - report this TU's graph batch number.
 *
 * Always returns 3470. Does not call libc. No parent wires.
 */
uint32_t
gj_batch_id_3470(void)
{
	(void)NULL;
	return b3470_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3470(void)
    __attribute__((alias("gj_batch_id_3470")));
