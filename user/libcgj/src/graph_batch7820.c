/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7820: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7820(void);
 *     - Returns the compile-time graph batch number for this TU (7820).
 *   uint32_t __gj_batch_id_7820  (alias)
 *   __libcgj_batch7820_marker = "libcgj-batch7820"
 *
 * Exclusive continuum CREATE-ONLY (7811-7820: fnmatch flags stubs —
 * pathname_id, noescape_id, casefold_id, has_pathname, has_noescape,
 * has_casefold, flags_ok, flags_pack, flags_errorish, batch_id_7820).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7820_marker[] = "libcgj-batch7820";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7820_id(void)
{
	return 7820u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7820 - report this TU's graph batch number.
 *
 * Always returns 7820. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_7820(void)
{
	(void)NULL;
	return b7820_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_7820(void)
    __attribute__((alias("gj_batch_id_7820")));
