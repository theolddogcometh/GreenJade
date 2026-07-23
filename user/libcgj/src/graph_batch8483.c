/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8483: ELFDATA2LSB data encoding constant stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_elf_data2lsb_id_8483(void);
 *     - Returns ELFDATA2LSB (1): little-endian 2's complement encoding.
 *   uint32_t __gj_elf_data2lsb_id_8483  (alias)
 *   __libcgj_batch8483_marker = "libcgj-batch8483"
 *
 * Exclusive continuum CREATE-ONLY (8481-8490: elf_mag0_id, elf_class64_id,
 * elf_data2lsb_id, elf_version_id, elf_osabi_sysv_id, elf_type_dyn_id,
 * elf_type_exec_id, elf_mach_x86_64_id, elf_ident_ok_u, batch_id_8490).
 * Distinct from gj_elf_data2lsb_u (batch4963) / gj_elf_data (batch1143) —
 * unique gj_elf_data2lsb_id_8483 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8483_marker[] = "libcgj-batch8483";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8483_data2lsb(void)
{
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_elf_data2lsb_id_8483 - ELFDATA2LSB constant (1).
 *
 * Always returns 1, the EI_DATA value for little-endian objects.
 * Self-contained; no parent wires.
 */
uint32_t
gj_elf_data2lsb_id_8483(void)
{
	(void)NULL;
	return b8483_data2lsb();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_elf_data2lsb_id_8483(void)
    __attribute__((alias("gj_elf_data2lsb_id_8483")));
