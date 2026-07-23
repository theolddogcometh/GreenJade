/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8160: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8160(void);
 *     - Returns the compile-time graph batch number for this TU (8160).
 *   uint32_t __gj_batch_id_8160  (alias)
 *   __libcgj_batch8160_marker = "libcgj-batch8160"
 *
 * Exclusive continuum CREATE-ONLY (8151-8160: strtok sep stubs —
 * sep_comma, sep_colon, sep_space, sep_tab, sep_nl, is_sep, is_ws,
 * is_csv, sep_ok, batch_id_8160). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8160_marker[] = "libcgj-batch8160";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8160_id(void)
{
	return 8160u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8160 - report this TU's graph batch number.
 *
 * Always returns 8160. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_8160(void)
{
	(void)NULL;
	return b8160_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8160(void)
    __attribute__((alias("gj_batch_id_8160")));
