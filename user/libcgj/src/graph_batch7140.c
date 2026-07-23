/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7140: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7140(void);
 *     - Returns the compile-time graph batch number for this TU (7140).
 *   uint32_t __gj_batch_id_7140  (alias)
 *   __libcgj_batch7140_marker = "libcgj-batch7140"
 *
 * Exclusive continuum CREATE-ONLY (7131-7140: mq_open flag stubs —
 * creat_id, excl_id, nonblock_id, has_creat, has_excl, has_nonblock,
 * flags_ok, flags_pack, flags_errorish, batch_id_7140).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7140_marker[] = "libcgj-batch7140";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7140_id(void)
{
	return 7140u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7140 - report this TU's graph batch number.
 *
 * Always returns 7140. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_7140(void)
{
	(void)NULL;
	return b7140_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_7140(void)
    __attribute__((alias("gj_batch_id_7140")));
