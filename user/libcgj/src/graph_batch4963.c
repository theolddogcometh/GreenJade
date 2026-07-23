/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4963: ELFDATA2LSB data encoding constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_elf_data2lsb_u(void);
 *     - Returns ELFDATA2LSB (1): little-endian 2's complement encoding.
 *   uint32_t __gj_elf_data2lsb_u  (alias)
 *   __libcgj_batch4963_marker = "libcgj-batch4963"
 *
 * Exclusive continuum CREATE-ONLY (4961-4970: elf_magic0_u, elf_class64_u,
 * elf_data2lsb_u, elf_et_exec_u, elf_et_dyn_u, elf_em_x86_64_u,
 * elf_is_magic_u, elf_phentsize64_u, elf_shentsize64_u, batch_id_4970).
 * Distinct from gj_elf_data (batch1143) / gj_elf_data_from_ei —
 * unique gj_elf_data2lsb_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4963_marker[] = "libcgj-batch4963";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4963_data2lsb(void)
{
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_elf_data2lsb_u - ELFDATA2LSB constant (1).
 *
 * Always returns 1, the EI_DATA value for little-endian objects.
 * Self-contained; no parent wires.
 */
uint32_t
gj_elf_data2lsb_u(void)
{
	(void)NULL;
	return b4963_data2lsb();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_elf_data2lsb_u(void)
    __attribute__((alias("gj_elf_data2lsb_u")));
