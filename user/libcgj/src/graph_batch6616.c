/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6616: mmap/mprotect prot mask soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mmap_prot_mask_ok_6616(uint32_t prot);
 *     - Return 1 when prot has only known soft bits PROT_READ|WRITE|EXEC
 *       (low 3 bits); 0 if any higher bit is set.
 *   uint32_t __gj_mmap_prot_mask_ok_6616  (alias)
 *   __libcgj_batch6616_marker = "libcgj-batch6616"
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

const char __libcgj_batch6616_marker[] = "libcgj-batch6616";

/* Soft known PROT bits for this stub continuum (R|W|X). */
#define B6616_PROT_KNOWN  ((uint32_t)0x7u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6616_prot_mask_ok(uint32_t u32Prot)
{
	if ((u32Prot & ~B6616_PROT_KNOWN) != 0u) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mmap_prot_mask_ok_6616 - soft known-prot-bits validity.
 *
 * prot: raw mmap/mprotect protection bits
 *
 * Returns 1 when only bits 0..2 may be set (PROT_READ|WRITE|EXEC,
 * including PROT_NONE), else 0. Pure integer. No parent wires.
 */
uint32_t
gj_mmap_prot_mask_ok_6616(uint32_t prot)
{
	(void)NULL;
	return b6616_prot_mask_ok(prot);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mmap_prot_mask_ok_6616(uint32_t prot)
    __attribute__((alias("gj_mmap_prot_mask_ok_6616")));
