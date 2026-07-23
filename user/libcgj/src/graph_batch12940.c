/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12940: exclusive IM soft batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12940(void);
 *     - Returns the compile-time graph batch number for this TU (12940).
 *   uint32_t __gj_batch_id_12940  (alias)
 *   __libcgj_batch12940_marker = "libcgj-batch12940"
 *
 * Exclusive continuum CREATE-ONLY (12931-12940: IM soft stubs —
 * ok slots all→0; im_soft_ready→1; batch_id→12940). Unique
 * gj_batch_id_12940 surface only; no multi-def. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12940_marker[] = "libcgj-batch12940";

#define B12940_BATCH_ID  12940u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12940_id(void)
{
	return B12940_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12940 - report this TU's graph batch number.
 *
 * Always returns 12940. Useful for link-time presence checks and
 * runtime graph revision probes without string compares on the marker.
 * No parent wires.
 */
uint32_t
gj_batch_id_12940(void)
{
	(void)NULL;
	return b12940_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12940(void)
    __attribute__((alias("gj_batch_id_12940")));
