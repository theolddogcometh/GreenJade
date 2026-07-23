/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6614: mmap/mprotect PROT_NONE soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mmap_prot_none_ok_6614(uint32_t prot);
 *     - Return 1 when prot is PROT_NONE (0); else 0.
 *   uint32_t __gj_mmap_prot_none_ok_6614  (alias)
 *   __libcgj_batch6614_marker = "libcgj-batch6614"
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

const char __libcgj_batch6614_marker[] = "libcgj-batch6614";

/* Soft PROT_NONE (Linux/glibc-shaped). */
#define B6614_PROT_NONE  ((uint32_t)0x0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6614_prot_none_ok(uint32_t u32Prot)
{
	if (u32Prot == B6614_PROT_NONE) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mmap_prot_none_ok_6614 - soft PROT_NONE equality test.
 *
 * prot: raw mmap/mprotect protection bits
 *
 * Returns 1 when prot is exactly 0 (PROT_NONE), else 0.
 * No parent wires.
 */
uint32_t
gj_mmap_prot_none_ok_6614(uint32_t prot)
{
	(void)NULL;
	return b6614_prot_none_ok(prot);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mmap_prot_none_ok_6614(uint32_t prot)
    __attribute__((alias("gj_mmap_prot_none_ok_6614")));
