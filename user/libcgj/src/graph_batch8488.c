/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8488: EM_X86_64 machine type constant stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_elf_mach_x86_64_id_8488(void);
 *     - Returns EM_X86_64 (62): AMD x86-64 architecture e_machine.
 *   uint32_t __gj_elf_mach_x86_64_id_8488  (alias)
 *   __libcgj_batch8488_marker = "libcgj-batch8488"
 *
 * Exclusive continuum CREATE-ONLY (8481-8490: elf_mag0_id, elf_class64_id,
 * elf_data2lsb_id, elf_version_id, elf_osabi_sysv_id, elf_type_dyn_id,
 * elf_type_exec_id, elf_mach_x86_64_id, elf_ident_ok_u, batch_id_8490).
 * Distinct from gj_elf_em_x86_64_u (batch4966) — unique
 * gj_elf_mach_x86_64_id_8488 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8488_marker[] = "libcgj-batch8488";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8488_mach_x86_64(void)
{
	return 62u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_elf_mach_x86_64_id_8488 - EM_X86_64 constant (62).
 *
 * Always returns 62, the e_machine value for AMD x86-64.
 * Self-contained; no parent wires.
 */
uint32_t
gj_elf_mach_x86_64_id_8488(void)
{
	(void)NULL;
	return b8488_mach_x86_64();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_elf_mach_x86_64_id_8488(void)
    __attribute__((alias("gj_elf_mach_x86_64_id_8488")));
