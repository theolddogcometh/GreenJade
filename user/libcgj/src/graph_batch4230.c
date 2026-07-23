/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4230: continuum wave batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4230(void);
 *     - Returns the compile-time graph batch number for this TU (4230).
 *   uint32_t __gj_batch_id_4230  (alias)
 *   __libcgj_batch4230_marker = "libcgj-batch4230"
 *
 * CREATE-ONLY exclusive continuum wave (4221-4230): mem ops size_t
 * unique names. Milestone/identity TU for the wave. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols - avoid
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4230_marker[] = "libcgj-batch4230";

/* Exclusive continuum id for wave 4221-4230. */
#define B4230_ID  4230u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4230_id(void)
{
	return B4230_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4230 - report this TU's graph batch number.
 *
 * Always returns 4230. Link-time presence tags the mem-ops continuum
 * wave (4221-4230). Does not call libc.
 * No parent wires.
 */
uint32_t
gj_batch_id_4230(void)
{
	(void)NULL;
	return b4230_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4230(void)
    __attribute__((alias("gj_batch_id_4230")));
