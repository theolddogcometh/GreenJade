/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4965: ET_DYN ELF object type constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_elf_et_dyn_u(void);
 *     - Returns ET_DYN (3): shared object / PIE dynamic object type.
 *   uint32_t __gj_elf_et_dyn_u  (alias)
 *   __libcgj_batch4965_marker = "libcgj-batch4965"
 *
 * Exclusive continuum CREATE-ONLY (4961-4970: elf_magic0_u, elf_class64_u,
 * elf_data2lsb_u, elf_et_exec_u, elf_et_dyn_u, elf_em_x86_64_u,
 * elf_is_magic_u, elf_phentsize64_u, elf_shentsize64_u, batch_id_4970).
 * Distinct from gj_elf_et_exec_u (batch4964) — unique gj_elf_et_dyn_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4965_marker[] = "libcgj-batch4965";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4965_et_dyn(void)
{
	return 3u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_elf_et_dyn_u - ET_DYN constant (3).
 *
 * Always returns 3, the e_type value for shared/PIE dynamic objects.
 * Self-contained; no parent wires.
 */
uint32_t
gj_elf_et_dyn_u(void)
{
	(void)NULL;
	return b4965_et_dyn();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_elf_et_dyn_u(void)
    __attribute__((alias("gj_elf_et_dyn_u")));
