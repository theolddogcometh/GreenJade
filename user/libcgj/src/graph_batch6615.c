/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6615: mmap/mprotect prot pack soft stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mmap_prot_pack_u_6615(uint32_t want_read, uint32_t want_write,
 *                                     uint32_t want_exec);
 *     - Soft pack: non-zero want_read → PROT_READ, want_write → PROT_WRITE,
 *       want_exec → PROT_EXEC; OR the selected bits.
 *   uint32_t __gj_mmap_prot_pack_u_6615  (alias)
 *   __libcgj_batch6615_marker = "libcgj-batch6615"
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

const char __libcgj_batch6615_marker[] = "libcgj-batch6615";

#define B6615_PROT_READ   ((uint32_t)0x1u)
#define B6615_PROT_WRITE  ((uint32_t)0x2u)
#define B6615_PROT_EXEC   ((uint32_t)0x4u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6615_prot_pack(uint32_t u32Read, uint32_t u32Write, uint32_t u32Exec)
{
	uint32_t u32Out = 0u;

	if (u32Read != 0u) {
		u32Out |= B6615_PROT_READ;
	}
	if (u32Write != 0u) {
		u32Out |= B6615_PROT_WRITE;
	}
	if (u32Exec != 0u) {
		u32Out |= B6615_PROT_EXEC;
	}
	return u32Out;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mmap_prot_pack_u_6615 - soft pack of r/w/x request bits into prot.
 *
 * want_read:  non-zero to include PROT_READ
 * want_write: non-zero to include PROT_WRITE
 * want_exec:  non-zero to include PROT_EXEC
 *
 * Returns packed prot mask (0..7 low bits). Pure integer. No parent wires.
 */
uint32_t
gj_mmap_prot_pack_u_6615(uint32_t want_read, uint32_t want_write,
    uint32_t want_exec)
{
	(void)NULL;
	return b6615_prot_pack(want_read, want_write, want_exec);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mmap_prot_pack_u_6615(uint32_t want_read, uint32_t want_write,
    uint32_t want_exec)
    __attribute__((alias("gj_mmap_prot_pack_u_6615")));
