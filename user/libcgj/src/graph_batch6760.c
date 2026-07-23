/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6760: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_6760(void);
 *     - Returns the compile-time graph batch number for this TU (6760).
 *   uint32_t __gj_batch_id_6760  (alias)
 *   __libcgj_batch6760_marker = "libcgj-batch6760"
 *
 * Exclusive continuum CREATE-ONLY (6751-6760: landlock rule type stubs —
 * path_beneath, net_port, type_ok, path_id, net_id, pack, needs_path_attr,
 * needs_net_attr, errorish, batch_id_6760). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6760_marker[] = "libcgj-batch6760";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6760_id(void)
{
	return 6760u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_6760 - report this TU's graph batch number.
 *
 * Always returns 6760. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_6760(void)
{
	(void)NULL;
	return b6760_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_6760(void)
    __attribute__((alias("gj_batch_id_6760")));
