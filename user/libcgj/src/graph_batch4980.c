/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4980: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4980(void);
 *     - Returns the compile-time graph batch number for this TU (4980).
 *   uint32_t __gj_batch_id_4980  (alias)
 *   __libcgj_batch4980_marker = "libcgj-batch4980"
 *
 * Exclusive continuum CREATE-ONLY PE unique wave (4971-4980:
 * pe_dos_magic_u, pe_nt_sig_u, pe_machine_amd64_u, pe_machine_i386_u,
 * pe_is_dos_u, pe_opt_hdr64_magic_u, pe_opt_hdr32_magic_u,
 * pe_subdir_count_u, pe_align_u, batch_id_4980). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols —
 * avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4980_marker[] = "libcgj-batch4980";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4980_id(void)
{
	return 4980u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4980 - report this TU's graph batch number.
 *
 * Always returns 4980. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4980(void)
{
	(void)NULL;
	return b4980_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4980(void)
    __attribute__((alias("gj_batch_id_4980")));
