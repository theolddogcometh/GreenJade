/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4967: ELF identity magic probe (uint32 _u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_elf_is_magic_u(const uint8_t *p);
 *     - Return 1 if p begins with ELF magic 0x7F 'E' 'L' 'F'; else 0.
 *       NULL → 0. Distinct from gj_elf_is_magic (batch1141) which takes
 *       (const void *, size_t) and returns int.
 *   uint32_t __gj_elf_is_magic_u  (alias)
 *   __libcgj_batch4967_marker = "libcgj-batch4967"
 *
 * Exclusive continuum CREATE-ONLY (4961-4970: elf_magic0_u, elf_class64_u,
 * elf_data2lsb_u, elf_et_exec_u, elf_et_dyn_u, elf_em_x86_64_u,
 * elf_is_magic_u, elf_phentsize64_u, elf_shentsize64_u, batch_id_4970).
 * Distinct from gj_elf_is_magic (batch1141) — unique gj_elf_is_magic_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4967_marker[] = "libcgj-batch4967";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4967_is_magic(const uint8_t *p)
{
	if (p == NULL) {
		return 0u;
	}
	if (p[0] != 0x7fu || p[1] != (uint8_t)'E' ||
	    p[2] != (uint8_t)'L' || p[3] != (uint8_t)'F') {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_elf_is_magic_u - 1 if p[0..3] is 0x7F 'E' 'L' 'F', else 0.
 *
 * p: pointer to at least 4 identity bytes (NULL → 0)
 *
 * Caller must ensure p is NULL or points to a readable 4-byte window.
 * Self-contained; no parent wires.
 */
uint32_t
gj_elf_is_magic_u(const uint8_t *p)
{
	(void)NULL;
	return b4967_is_magic(p);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_elf_is_magic_u(const uint8_t *p)
    __attribute__((alias("gj_elf_is_magic_u")));
