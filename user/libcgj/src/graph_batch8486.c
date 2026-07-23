/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8486: ET_DYN ELF object type constant stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_elf_type_dyn_id_8486(void);
 *     - Returns ET_DYN (3): shared object / PIE dynamic object type.
 *   uint32_t __gj_elf_type_dyn_id_8486  (alias)
 *   __libcgj_batch8486_marker = "libcgj-batch8486"
 *
 * Exclusive continuum CREATE-ONLY (8481-8490: elf_mag0_id, elf_class64_id,
 * elf_data2lsb_id, elf_version_id, elf_osabi_sysv_id, elf_type_dyn_id,
 * elf_type_exec_id, elf_mach_x86_64_id, elf_ident_ok_u, batch_id_8490).
 * Distinct from gj_elf_et_dyn_u (batch4965) — unique
 * gj_elf_type_dyn_id_8486 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8486_marker[] = "libcgj-batch8486";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8486_type_dyn(void)
{
	return 3u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_elf_type_dyn_id_8486 - ET_DYN constant (3).
 *
 * Always returns 3, the e_type value for shared/PIE ELF objects.
 * Self-contained; no parent wires.
 */
uint32_t
gj_elf_type_dyn_id_8486(void)
{
	(void)NULL;
	return b8486_type_dyn();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_elf_type_dyn_id_8486(void)
    __attribute__((alias("gj_elf_type_dyn_id_8486")));
