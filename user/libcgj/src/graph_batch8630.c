/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8630: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8630(void);
 *     - Returns the compile-time graph batch number for this TU (8630).
 *   uint32_t __gj_batch_id_8630  (alias)
 *   __libcgj_batch8630_marker = "libcgj-batch8630"
 *
 * Exclusive continuum CREATE-ONLY (8621-8630: dynlink reloc id stubs —
 * rel_none_id, rel_64_id, rel_pc32_id, rel_gotpc_id, rel_plt_id,
 * rel_copy_id, rel_glob_dat_id, rel_jump_slot_id, rel_relative_id,
 * batch_id_8630). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. Soft pure-data catalog
 * only; no reloc application. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8630_marker[] = "libcgj-batch8630";

#define B8630_BATCH_ID  8630u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8630_id(void)
{
	return B8630_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8630 - report this TU's graph batch number.
 *
 * Always returns 8630. Soft pure-data product tag that the dynlink
 * reloc id stubs exclusive wave is present. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_batch_id_8630(void)
{
	(void)NULL;
	return b8630_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8630(void)
    __attribute__((alias("gj_batch_id_8630")));
