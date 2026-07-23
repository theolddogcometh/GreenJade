/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8489: ELF identity magic probe (uint32 _u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_elf_ident_ok_u_8489(const uint8_t *p);
 *     - Return 1 if p begins with ELF magic 0x7F 'E' 'L' 'F'; else 0.
 *       NULL → 0. Distinct from gj_elf_is_magic (batch1141) which takes
 *       (const void *, size_t) and returns int, and from
 *       gj_elf_is_magic_u (batch4967).
 *   uint32_t __gj_elf_ident_ok_u_8489  (alias)
 *   __libcgj_batch8489_marker = "libcgj-batch8489"
 *
 * Exclusive continuum CREATE-ONLY (8481-8490: elf_mag0_id, elf_class64_id,
 * elf_data2lsb_id, elf_version_id, elf_osabi_sysv_id, elf_type_dyn_id,
 * elf_type_exec_id, elf_mach_x86_64_id, elf_ident_ok_u, batch_id_8490).
 * Unique gj_elf_ident_ok_u_8489 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8489_marker[] = "libcgj-batch8489";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8489_ident_ok(const uint8_t *p)
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
 * gj_elf_ident_ok_u_8489 - 1 if p[0..3] is 0x7F 'E' 'L' 'F', else 0.
 *
 * p: pointer to at least 4 identity bytes (NULL → 0)
 *
 * Caller must ensure p is NULL or points to a readable 4-byte window.
 * Self-contained; no parent wires.
 */
uint32_t
gj_elf_ident_ok_u_8489(const uint8_t *p)
{
	(void)NULL;
	return b8489_ident_ok(p);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_elf_ident_ok_u_8489(const uint8_t *p)
    __attribute__((alias("gj_elf_ident_ok_u_8489")));
