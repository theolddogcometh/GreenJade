/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3580: batch identity for exclusive wave
 * 3571-3580.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3580(void);
 *     - Returns the compile-time graph batch number for this TU (3580).
 *   uint32_t __gj_batch_id_3580  (alias)
 *   __libcgj_batch3580_marker = "libcgj-batch3580"
 *
 * CREATE-ONLY exclusive continuum wave (3571-3580): mem_eq_n_u2,
 * mem_ne_n_u, mem_lt_n_u, mem_prefix_eq_u, mem_suffix_eq_u,
 * mem_fill_u32_u, mem_fill_u64_u, mem_scan_u32_u, mem_scan_u64_u,
 * batch_id_3580. Unique gj_batch_id_3580 surface only; no multi-def.
 * Does NOT redefine gj_batch_id / prior batch_id_* symbols.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3580_marker[] = "libcgj-batch3580";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3580_id(void)
{
	return 3580u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3580 - report this TU's graph batch number.
 *
 * Always returns 3580. Link-time presence tags the 3571-3580 wave.
 * No parent wires.
 */
uint32_t
gj_batch_id_3580(void)
{
	(void)NULL;
	return b3580_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3580(void)
    __attribute__((alias("gj_batch_id_3580")));
