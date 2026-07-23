/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Product SO surface (libgj-so): SysV DT_HASH soft batch 1.
 *
 * Freestanding pure C. Dual-licensed MIT OR Apache-2.0.
 * Intended for the parent Makefile to link into libgj-so.so.1 (SysV-hash
 * product SO) when a missing symbol is assigned here. Not linked today —
 * libgj_so.c alone is the shipped SO. Does NOT touch libcgj.
 *
 * Coherent soft package with continuum MILESTONE 14700 host gates:
 *   - user/libcgj/tests/NOTES_milestone_14700.txt
 *   - user/libcgj/tests/cgj_soft_milestone_14700.c
 *   - user/libgj-so/src/graph_so_batch1.c (this TU)
 *   - user/libgj-so/src/graph_so_batch2.c
 * Soft only; decls also in include/gj_so.h. Marker value matches
 * GJ_SO_BATCH1_EXPORT_VALUE (0x421) there.
 *
 * Ownership policy: only MISSING SysV/product-SO symbols are defined in
 * this tree. Candidate symbols that already live in user/libcgj (or the
 * shipped product TU) must not be redefined here.
 *
 * Soft surface (unwired):
 *   gj_so_batch1_export    — data (0x421); optional future resolve target
 *   gj_so_batch1_init      — restores the marker
 *   gj_so_batch1_id        — returns current marker (-Werror clean)
 *   gj_so_batch1_sysv_hash — classic ELF SysV hash (DT_HASH name hash)
 *   gj_so_batch1_streq     — freestanding string equality for chain match
 *   gj_so_batch1_bucket    — hash % nbucket index helper
 */
#include <stddef.h>
#include <stdint.h>

/* Matches GJ_SO_BATCH1_EXPORT_VALUE in include/gj_so.h (soft section). */
#define GJ_SO_BATCH1_EXPORT_VALUE  ((uint64_t)0x421)

/* Product SO batch presence marker (GLOB_DAT / JUMP_SLOT smoke target). */
volatile uint64_t gj_so_batch1_export = GJ_SO_BATCH1_EXPORT_VALUE;

void
gj_so_batch1_init(void)
{
	gj_so_batch1_export = GJ_SO_BATCH1_EXPORT_VALUE;
}

/* Keep TU useful under -Werror -Wunused when only the marker is present. */
uint64_t
gj_so_batch1_id(void)
{
	return gj_so_batch1_export;
}

/*
 * Classic System V ELF hash (DT_HASH). Matches kernel elf_sysv_hash_name
 * and ld-gj sysv_hash — freestanding reimplementation for this SO tree.
 * Same algorithm as product gj_so_sysv_hash in libgj_so.c.
 */
uint32_t
gj_so_batch1_sysv_hash(const char *szName)
{
	uint32_t u32H = 0;
	uint32_t u32G;

	if (szName == NULL) {
		return 0;
	}
	while (*szName != '\0') {
		u32H = (u32H << 4) + (uint32_t)(unsigned char)*szName++;
		u32G = u32H & 0xf0000000u;
		if (u32G != 0) {
			u32H ^= u32G >> 24;
		}
		u32H &= ~u32G;
	}
	return u32H;
}

/* Freestanding equality used by soft chain-walk helpers (NULL-safe). */
int
gj_so_batch1_streq(const char *szA, const char *szB)
{
	if (szA == NULL || szB == NULL) {
		return szA == szB;
	}
	while (*szA != '\0' && *szA == *szB) {
		szA++;
		szB++;
	}
	return *szA == *szB;
}

/* Bucket index for SysV hash table: hash % nbucket (0 when empty table). */
uint32_t
gj_so_batch1_bucket(uint32_t u32Hash, uint32_t u32Nbucket)
{
	if (u32Nbucket == 0) {
		return 0;
	}
	return u32Hash % u32Nbucket;
}
