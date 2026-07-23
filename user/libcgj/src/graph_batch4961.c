/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4961: ELF identity magic byte 0 (EI_MAG0).
 *
 * Surface (unique symbols):
 *   uint32_t gj_elf_magic0_u(void);
 *     - Returns the ELF file identity magic byte 0 value (0x7F / EI_MAG0).
 *   uint32_t __gj_elf_magic0_u  (alias)
 *   __libcgj_batch4961_marker = "libcgj-batch4961"
 *
 * Exclusive continuum CREATE-ONLY (4961-4970: elf_magic0_u, elf_class64_u,
 * elf_data2lsb_u, elf_et_exec_u, elf_et_dyn_u, elf_em_x86_64_u,
 * elf_is_magic_u, elf_phentsize64_u, elf_shentsize64_u, batch_id_4970).
 * Distinct from gj_elf_is_magic (batch1141) / gj_elf_class (batch1142) —
 * unique gj_elf_magic0_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4961_marker[] = "libcgj-batch4961";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4961_magic0(void)
{
	return 0x7fu;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_elf_magic0_u - ELF EI_MAG0 constant (0x7F).
 *
 * Always returns 0x7F, the first byte of the ELF identification magic.
 * Self-contained; no parent wires.
 */
uint32_t
gj_elf_magic0_u(void)
{
	(void)NULL;
	return b4961_magic0();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_elf_magic0_u(void)
    __attribute__((alias("gj_elf_magic0_u")));
