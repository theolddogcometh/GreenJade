/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8490: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8490(void);
 *     - Returns the compile-time graph batch number for this TU (8490).
 *   uint32_t __gj_batch_id_8490  (alias)
 *   __libcgj_batch8490_marker = "libcgj-batch8490"
 *
 * Exclusive continuum CREATE-ONLY (8481-8490: elf_mag0_id, elf_class64_id,
 * elf_data2lsb_id, elf_version_id, elf_osabi_sysv_id, elf_type_dyn_id,
 * elf_type_exec_id, elf_mach_x86_64_id, elf_ident_ok_u, batch_id_8490).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8490_marker[] = "libcgj-batch8490";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8490_id(void)
{
	return 8490u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8490 - report this TU's graph batch number.
 *
 * Always returns 8490. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_8490(void)
{
	(void)NULL;
	return b8490_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8490(void)
    __attribute__((alias("gj_batch_id_8490")));
