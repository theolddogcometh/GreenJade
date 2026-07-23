/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7870: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7870(void);
 *     - Returns the compile-time graph batch number for this TU (7870).
 *   uint32_t __gj_batch_id_7870  (alias)
 *   __libcgj_batch7870_marker = "libcgj-batch7870"
 *
 * Exclusive continuum CREATE-ONLY (7861-7870: nl_langinfo item stubs —
 * codeset_id, d_t_fmt_id, radixchar_id, is_codeset, is_d_t_fmt,
 * is_radixchar, item_ok, item_pack, item_errorish, batch_id_7870).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7870_marker[] = "libcgj-batch7870";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7870_id(void)
{
	return 7870u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7870 - report this TU's graph batch number.
 *
 * Always returns 7870. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_7870(void)
{
	(void)NULL;
	return b7870_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_7870(void)
    __attribute__((alias("gj_batch_id_7870")));
