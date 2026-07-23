/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8481: ELF identity EI_MAG0 constant stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_elf_mag0_id_8481(void);
 *     - Returns the ELF file identity magic byte 0 value (0x7F / EI_MAG0).
 *   uint32_t __gj_elf_mag0_id_8481  (alias)
 *   __libcgj_batch8481_marker = "libcgj-batch8481"
 *
 * Exclusive continuum CREATE-ONLY (8481-8490: elf_mag0_id, elf_class64_id,
 * elf_data2lsb_id, elf_version_id, elf_osabi_sysv_id, elf_type_dyn_id,
 * elf_type_exec_id, elf_mach_x86_64_id, elf_ident_ok_u, batch_id_8490).
 * Distinct from gj_elf_magic0_u (batch4961) / gj_elf_is_magic (batch1141) —
 * unique gj_elf_mag0_id_8481 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8481_marker[] = "libcgj-batch8481";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8481_mag0(void)
{
	return 0x7fu;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_elf_mag0_id_8481 - ELF EI_MAG0 constant (0x7F).
 *
 * Always returns 0x7F, the first byte of the ELF identification magic.
 * Self-contained; no parent wires.
 */
uint32_t
gj_elf_mag0_id_8481(void)
{
	(void)NULL;
	return b8481_mag0();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_elf_mag0_id_8481(void)
    __attribute__((alias("gj_elf_mag0_id_8481")));
