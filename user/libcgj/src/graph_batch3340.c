/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3340: exclusive wave batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3340(void);
 *     - Returns the compile-time graph batch number for this TU (3340).
 *   uint32_t __gj_batch_id_3340  (alias)
 *   __libcgj_batch3340_marker = "libcgj-batch3340"
 *
 * Exclusive continuum CREATE-ONLY (3331-3340). Unique gj_batch_id_3340
 * surface only; no multi-def. Distinct from gj_batch_id_3300
 * (batch3300) and earlier batch_id_* symbols. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3340_marker[] = "libcgj-batch3340";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3340_id(void)
{
	return 3340u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3340 - report this TU's graph batch number.
 *
 * Always returns 3340. Link-time presence tags the exclusive continuum
 * wave close. Does not call libc. No parent wires.
 */
uint32_t
gj_batch_id_3340(void)
{
	(void)NULL;
	return b3340_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3340(void)
    __attribute__((alias("gj_batch_id_3340")));
