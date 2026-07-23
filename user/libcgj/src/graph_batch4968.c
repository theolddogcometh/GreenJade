/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4968: Elf64_Phdr entry size constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_elf_phentsize64_u(void);
 *     - Returns the fixed Elf64 program-header entry size (56 bytes).
 *   uint32_t __gj_elf_phentsize64_u  (alias)
 *   __libcgj_batch4968_marker = "libcgj-batch4968"
 *
 * Exclusive continuum CREATE-ONLY (4961-4970: elf_magic0_u, elf_class64_u,
 * elf_data2lsb_u, elf_et_exec_u, elf_et_dyn_u, elf_em_x86_64_u,
 * elf_is_magic_u, elf_phentsize64_u, elf_shentsize64_u, batch_id_4970).
 * Unique gj_elf_phentsize64_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4968_marker[] = "libcgj-batch4968";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4968_phentsize64(void)
{
	return 56u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_elf_phentsize64_u - Elf64_Phdr size / e_phentsize (56).
 *
 * Always returns 56, the standard size of one Elf64 program header.
 * Self-contained; no parent wires.
 */
uint32_t
gj_elf_phentsize64_u(void)
{
	(void)NULL;
	return b4968_phentsize64();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_elf_phentsize64_u(void)
    __attribute__((alias("gj_elf_phentsize64_u")));
