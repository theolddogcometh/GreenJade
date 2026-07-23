/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3360: milestone 3360 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3360(void);
 *     - Returns the compile-time graph batch number for this TU (3360).
 *   uint32_t __gj_batch_id_3360  (alias)
 *   __libcgj_batch3360_marker = "libcgj-batch3360"
 *
 * Milestone for the ELF/PE/COFF exclusive continuum wave
 * (batches 3351-3359: elf_shnum_u, elf_phnum_u, elf_shentsize_u,
 * elf_phentsize_u, elf_entry_u, pe_machine_u, pe_sections_u,
 * pe_timestamp_u, coff_nscns_u). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3360_marker[] = "libcgj-batch3360";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3360_id(void)
{
	return 3360u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3360 - report this TU's graph batch number.
 *
 * Always returns 3360. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_3360(void)
{
	(void)NULL;
	return b3360_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3360(void)
    __attribute__((alias("gj_batch_id_3360")));
