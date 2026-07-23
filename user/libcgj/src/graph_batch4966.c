/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4966: EM_X86_64 machine type constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_elf_em_x86_64_u(void);
 *     - Returns EM_X86_64 (62): AMD x86-64 architecture e_machine.
 *   uint32_t __gj_elf_em_x86_64_u  (alias)
 *   __libcgj_batch4966_marker = "libcgj-batch4966"
 *
 * Exclusive continuum CREATE-ONLY (4961-4970: elf_magic0_u, elf_class64_u,
 * elf_data2lsb_u, elf_et_exec_u, elf_et_dyn_u, elf_em_x86_64_u,
 * elf_is_magic_u, elf_phentsize64_u, elf_shentsize64_u, batch_id_4970).
 * Unique gj_elf_em_x86_64_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4966_marker[] = "libcgj-batch4966";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4966_em_x86_64(void)
{
	return 62u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_elf_em_x86_64_u - EM_X86_64 constant (62).
 *
 * Always returns 62, the e_machine value for AMD x86-64.
 * Self-contained; no parent wires.
 */
uint32_t
gj_elf_em_x86_64_u(void)
{
	(void)NULL;
	return b4966_em_x86_64();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_elf_em_x86_64_u(void)
    __attribute__((alias("gj_elf_em_x86_64_u")));
