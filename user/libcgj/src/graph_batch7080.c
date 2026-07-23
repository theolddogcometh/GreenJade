/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7080: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7080(void);
 *     - Returns the compile-time graph batch number for this TU (7080).
 *   uint32_t __gj_batch_id_7080  (alias)
 *   __libcgj_batch7080_marker = "libcgj-batch7080"
 *
 * Exclusive continuum CREATE-ONLY (7071-7080: ptrace request stubs —
 * traceme, peektext, peekdata, poketext, pokedata, cont, attach,
 * detach, req_ok, batch_id_7080). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7080_marker[] = "libcgj-batch7080";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7080_id(void)
{
	return 7080u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7080 - report this TU's graph batch number.
 *
 * Always returns 7080. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_7080(void)
{
	(void)NULL;
	return b7080_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_7080(void)
    __attribute__((alias("gj_batch_id_7080")));
