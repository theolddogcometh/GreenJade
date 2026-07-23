/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7160: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7160(void);
 *     - Returns the compile-time graph batch number for this TU (7160).
 *   uint32_t __gj_batch_id_7160  (alias)
 *   __libcgj_batch7160_marker = "libcgj-batch7160"
 *
 * Exclusive continuum CREATE-ONLY (7151-7160: sem_open flag stubs —
 * creat_id, excl_id, known_mask_id, has_creat, has_excl, needs_mode,
 * flags_ok, flags_pack, flags_errorish, batch_id_7160).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7160_marker[] = "libcgj-batch7160";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7160_id(void)
{
	return 7160u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7160 - report this TU's graph batch number.
 *
 * Always returns 7160. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_7160(void)
{
	(void)NULL;
	return b7160_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_7160(void)
    __attribute__((alias("gj_batch_id_7160")));
