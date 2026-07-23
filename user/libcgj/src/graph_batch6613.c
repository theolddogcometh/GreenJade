/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6613: mmap/mprotect PROT_EXEC flag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mmap_prot_exec_u_6613(uint32_t prot);
 *     - Return 1 when PROT_EXEC (0x4) is set in prot; else 0.
 *   uint32_t __gj_mmap_prot_exec_u_6613  (alias)
 *   __libcgj_batch6613_marker = "libcgj-batch6613"
 *
 * Exclusive continuum CREATE-ONLY (6611-6620: mmap/mprotect flag stubs —
 * prot_read, prot_write, prot_exec, prot_none_ok, prot_pack, prot_mask_ok,
 * map_shared, map_private, map_type_ok, flags_ready). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6613_marker[] = "libcgj-batch6613";

/* Soft PROT_EXEC (Linux/glibc-shaped). */
#define B6613_PROT_EXEC  ((uint32_t)0x4u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6613_prot_exec(uint32_t u32Prot)
{
	if ((u32Prot & B6613_PROT_EXEC) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mmap_prot_exec_u_6613 - soft PROT_EXEC presence test.
 *
 * prot: raw mmap/mprotect protection bits
 *
 * Returns 1 when bit 2 (PROT_EXEC) is set, else 0. Pure mask.
 * No parent wires.
 */
uint32_t
gj_mmap_prot_exec_u_6613(uint32_t prot)
{
	(void)NULL;
	return b6613_prot_exec(prot);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mmap_prot_exec_u_6613(uint32_t prot)
    __attribute__((alias("gj_mmap_prot_exec_u_6613")));
