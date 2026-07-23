/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4969: Elf64_Shdr entry size constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_elf_shentsize64_u(void);
 *     - Returns the fixed Elf64 section-header entry size (64 bytes).
 *   uint32_t __gj_elf_shentsize64_u  (alias)
 *   __libcgj_batch4969_marker = "libcgj-batch4969"
 *
 * Exclusive continuum CREATE-ONLY (4961-4970: elf_magic0_u, elf_class64_u,
 * elf_data2lsb_u, elf_et_exec_u, elf_et_dyn_u, elf_em_x86_64_u,
 * elf_is_magic_u, elf_phentsize64_u, elf_shentsize64_u, batch_id_4970).
 * Distinct from gj_elf_phentsize64_u (batch4968) — unique
 * gj_elf_shentsize64_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4969_marker[] = "libcgj-batch4969";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4969_shentsize64(void)
{
	return 64u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_elf_shentsize64_u - Elf64_Shdr size / e_shentsize (64).
 *
 * Always returns 64, the standard size of one Elf64 section header.
 * Self-contained; no parent wires.
 */
uint32_t
gj_elf_shentsize64_u(void)
{
	(void)NULL;
	return b4969_shentsize64();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_elf_shentsize64_u(void)
    __attribute__((alias("gj_elf_shentsize64_u")));
