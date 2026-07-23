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
 * Soft surface (unwired; Wave 19 exclusive deepen):
 *   gj_so_batch1_export      — data (0x421); optional future resolve target
 *   gj_so_batch1_soft_stamp  — soft companion stamp ('B1s1')
 *   gj_so_batch1_init        — restores the marker + stamp
 *   gj_so_batch1_id          — returns current marker (-Werror clean)
 *   gj_so_batch1_sysv_hash   — classic ELF SysV hash (DT_HASH name hash)
 *   gj_so_batch1_streq       — freestanding string equality for chain match
 *   gj_so_batch1_bucket      — hash % nbucket index helper
 *   gj_so_batch1_soft_get    — soft read of marker
 *   gj_so_batch1_soft_probe  — soft check + careful restore
 *   gj_so_batch1_soft_wave   — Wave 19 stamp (17)
 *   gj_so_batch1_soft_inventory — greppable cold inventory line
 *
 * greppable: GJ_SO_BATCH1_SOFT_MARKER
 * greppable: GJ_SO_BATCH1_SOFT_INIT
 * greppable: GJ_SO_BATCH1_SOFT_GET
 * greppable: GJ_SO_BATCH1_SOFT_PROBE
 * greppable: GJ_SO_BATCH1_SOFT_STAMP
 * greppable: libgj-so: soft batch1 wave=19
 */
#include <stddef.h>
#include <stdint.h>

/* Matches GJ_SO_BATCH1_EXPORT_VALUE in include/gj_so.h (soft section). */
#define GJ_SO_BATCH1_EXPORT_VALUE  ((uint64_t)0x421)
#define GJ_SO_BATCH1_STAMP_CANON   ((uint64_t)0x42317331ull) /* 'B1s1' */
#define GJ_SO_BATCH1_SOFT_WAVE     19u

/* greppable: libgj-so: soft batch1 wave=19 */
static const char g_szSoBatch1SoftInventory[] =
	"libgj-so: soft batch1 wave=19 areas=9 "
	"export,stamp,init,id,hash,streq,bucket,get,probe "
	"marker=0x421 stamp=B1s1 freestanding=1 multi_server=0 "
	"confine=0 bar3=0";

/* greppable: GJ_SO_BATCH1_SOFT_MARKER */
volatile uint64_t gj_so_batch1_export = GJ_SO_BATCH1_EXPORT_VALUE;

/* greppable: GJ_SO_BATCH1_SOFT_STAMP */
volatile uint64_t gj_so_batch1_soft_stamp = GJ_SO_BATCH1_STAMP_CANON;

void
gj_so_batch1_init(void)
{
	/* greppable: GJ_SO_BATCH1_SOFT_INIT */
	gj_so_batch1_export = GJ_SO_BATCH1_EXPORT_VALUE;
	gj_so_batch1_soft_stamp = GJ_SO_BATCH1_STAMP_CANON;
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

/*
 * Soft get of the batch marker. Read-only; does not mutate.
 */
uint64_t
gj_so_batch1_soft_get(void)
{
	/* greppable: GJ_SO_BATCH1_SOFT_GET */
	return gj_so_batch1_export;
}

/*
 * Soft probe: 1 if marker is canonical; on soft miss carefully restores
 * marker + stamp and returns 0. Never hard-fails.
 */
int
gj_so_batch1_soft_probe(void)
{
	/* greppable: GJ_SO_BATCH1_SOFT_PROBE */
	if (gj_so_batch1_export != GJ_SO_BATCH1_EXPORT_VALUE) {
		gj_so_batch1_export = GJ_SO_BATCH1_EXPORT_VALUE;
		gj_so_batch1_soft_stamp = GJ_SO_BATCH1_STAMP_CANON;
		return 0;
	}
	if (gj_so_batch1_soft_stamp != GJ_SO_BATCH1_STAMP_CANON) {
		gj_so_batch1_soft_stamp = GJ_SO_BATCH1_STAMP_CANON;
		return 0;
	}
	return 1;
}

/* Cold soft inventory: Wave 19 stamp. Grep: libgj-so: soft batch1 wave= */
unsigned
gj_so_batch1_soft_wave(void)
{
	return (unsigned)GJ_SO_BATCH1_SOFT_WAVE;
}

/* Cold soft inventory line. Grep: libgj-so: soft batch1 */
const char *
gj_so_batch1_soft_inventory(void)
{
	return g_szSoBatch1SoftInventory;
}
