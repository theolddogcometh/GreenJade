/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7340: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7340(void);
 *     - Returns the compile-time graph batch number for this TU (7340).
 *   uint32_t __gj_batch_id_7340  (alias)
 *   __libcgj_batch7340_marker = "libcgj-batch7340"
 *
 * Exclusive continuum CREATE-ONLY (7331-7340: flock op stubs —
 * lock_sh_id, lock_ex_id, lock_nb_id, lock_un_id, has_sh, has_ex,
 * has_nb, ops_ok, ops_pack, batch_id_7340).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7340_marker[] = "libcgj-batch7340";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7340_id(void)
{
	return 7340u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7340 - report this TU's graph batch number.
 *
 * Always returns 7340. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_7340(void)
{
	(void)NULL;
	return b7340_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_7340(void)
    __attribute__((alias("gj_batch_id_7340")));
