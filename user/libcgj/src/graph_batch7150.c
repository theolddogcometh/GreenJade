/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7150: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7150(void);
 *     - Returns the compile-time graph batch number for this TU (7150).
 *   uint32_t __gj_batch_id_7150  (alias)
 *   __libcgj_batch7150_marker = "libcgj-batch7150"
 *
 * Exclusive continuum CREATE-ONLY (7141-7150: shm_open flag stubs —
 * o_rdwr_id, o_creat_id, o_excl_id, has_rdwr, has_creat, has_excl,
 * flags_ok, flags_pack, flags_errorish, batch_id_7150).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7150_marker[] = "libcgj-batch7150";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7150_id(void)
{
	return 7150u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7150 - report this TU's graph batch number.
 *
 * Always returns 7150. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_7150(void)
{
	(void)NULL;
	return b7150_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_7150(void)
    __attribute__((alias("gj_batch_id_7150")));
