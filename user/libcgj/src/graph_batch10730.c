/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10730: mesa soft stubs batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10730(void);
 *     - Returns the compile-time graph batch number for this TU (10730).
 *   uint32_t __gj_batch_id_10730  (alias)
 *   __libcgj_batch10730_marker = "libcgj-batch10730"
 *
 * Exclusive continuum CREATE-ONLY (10721-10730: mesa soft stubs —
 * ok slots all→0; mesa_soft_ready→1; batch_id→10730). Unique
 * gj_batch_id_10730 surface only; no multi-def. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. Closes the 10721-10730 continuum (mesa/radv/amdvlk/anv/
 * lavapipe/zink/iris/v3d soft ok stubs + mesa soft ready). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10730_marker[] = "libcgj-batch10730";

#define B10730_BATCH_ID  10730u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10730_id(void)
{
	return B10730_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10730 - report this TU's graph batch number.
 *
 * Always returns 10730. Soft pure-data identity for the mesa soft
 * stubs continuum close. Does not call libc. No parent wires.
 */
uint32_t
gj_batch_id_10730(void)
{
	(void)NULL;
	return b10730_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10730(void)
    __attribute__((alias("gj_batch_id_10730")));
