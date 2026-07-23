/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3390: exclusive wave batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3390(void);
 *     - Returns the compile-time graph batch number for this TU (3390).
 *   uint32_t __gj_batch_id_3390  (alias)
 *   __libcgj_batch3390_marker = "libcgj-batch3390"
 *
 * Net exclusive wave (3381-3390). Distinct from gj_batch_id_3300
 * (batch3300) and other batch_id_* symbols — unique surface; no
 * multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3390_marker[] = "libcgj-batch3390";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3390_id(void)
{
	return 3390u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3390 - report this TU's graph batch number.
 *
 * Always returns 3390. Does not call libc. No parent wires.
 */
uint32_t
gj_batch_id_3390(void)
{
	(void)NULL;
	return b3390_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3390(void)
    __attribute__((alias("gj_batch_id_3390")));
