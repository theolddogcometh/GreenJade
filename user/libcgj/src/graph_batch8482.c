/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8482: ELFCLASS64 identity class constant stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_elf_class64_id_8482(void);
 *     - Returns ELFCLASS64 (2): 64-bit ELF object file class.
 *   uint32_t __gj_elf_class64_id_8482  (alias)
 *   __libcgj_batch8482_marker = "libcgj-batch8482"
 *
 * Exclusive continuum CREATE-ONLY (8481-8490: elf_mag0_id, elf_class64_id,
 * elf_data2lsb_id, elf_version_id, elf_osabi_sysv_id, elf_type_dyn_id,
 * elf_type_exec_id, elf_mach_x86_64_id, elf_ident_ok_u, batch_id_8490).
 * Distinct from gj_elf_class64_u (batch4962) / gj_elf_class (batch1142) —
 * unique gj_elf_class64_id_8482 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8482_marker[] = "libcgj-batch8482";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8482_class64(void)
{
	return 2u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_elf_class64_id_8482 - ELFCLASS64 constant (2).
 *
 * Always returns 2, the EI_CLASS value for 64-bit ELF objects.
 * Self-contained; no parent wires.
 */
uint32_t
gj_elf_class64_id_8482(void)
{
	(void)NULL;
	return b8482_class64();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_elf_class64_id_8482(void)
    __attribute__((alias("gj_elf_class64_id_8482")));
