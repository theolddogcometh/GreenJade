/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7070: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7070(void);
 *     - Returns the compile-time graph batch number for this TU (7070).
 *   uint32_t __gj_batch_id_7070  (alias)
 *   __libcgj_batch7070_marker = "libcgj-batch7070"
 *
 * Exclusive continuum CREATE-ONLY (7061-7070: capset/capget version stubs —
 * is_v1, is_v2, is_v3, version_ok, v1_id, v2_id, v3_id, u32s,
 * version_errorish, batch_id_7070). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7070_marker[] = "libcgj-batch7070";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7070_id(void)
{
	return 7070u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7070 - report this TU's graph batch number.
 *
 * Always returns 7070. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_7070(void)
{
	(void)NULL;
	return b7070_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_7070(void)
    __attribute__((alias("gj_batch_id_7070")));
