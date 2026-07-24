/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Product SO surface (libgj-so): SysV DT_HASH soft batch 2.
 *
 * Freestanding pure C. Dual-licensed MIT OR Apache-2.0.
 * Soft multi-export / chain-walk helpers for future multi-bucket resolve
 * stress once the parent Makefile wires this TU into libgj-so.so.1.
 * Not linked today — libgj_so.c alone is the shipped SO.
 *
 * Coherent soft package with continuum MILESTONE 14700 host gates:
 *   - user/libcgj/tests/NOTES_milestone_14700.txt
 *   - user/libcgj/tests/cgj_soft_milestone_14700.c
 *   - user/libgj-so/src/graph_so_batch1.c
 *   - user/libgj-so/src/graph_so_batch2.c (this TU)
 * Soft only; decls also in include/gj_so.h. Marker values match
 * GJ_SO_BATCH2_EXPORT_A_VALUE (0x422) / _B_VALUE (0x423) there.
 *
 * Soft surface (unwired; Wave 40 exclusive deepen):
 *   gj_so_batch2_export_a / _b — distinct data markers (0x422 / 0x423)
 *   gj_so_batch2_soft_stamp    — soft companion stamp ('B2s2')
 *   gj_so_batch2_init          — restores both markers + stamp
 *   gj_so_batch2_id            — XOR of markers (-Werror clean)
 *   gj_so_batch2_chain_next    — one step of SysV hash chain walk
 *   gj_so_batch2_chain_find    — soft linear chain probe by name hash slot
 *   gj_so_batch2_soft_get      — soft XOR id (read-only)
 *   gj_so_batch2_soft_probe    — soft check + careful restore
 *   gj_so_batch2_soft_wave     — Wave 40 stamp (40)
 *   gj_so_batch2_soft_inventory — greppable cold inventory line
 *
 * greppable: GJ_SO_BATCH2_SOFT_MARKER
 * greppable: GJ_SO_BATCH2_SOFT_INIT
 * greppable: GJ_SO_BATCH2_SOFT_GET
 * greppable: GJ_SO_BATCH2_SOFT_PROBE
 * greppable: GJ_SO_BATCH2_SOFT_STAMP
 * greppable: libgj-so: soft batch2 wave=40
 */
#include <stddef.h>
#include <stdint.h>

/* Matches GJ_SO_BATCH2_EXPORT_*_VALUE in include/gj_so.h (soft section). */
#define GJ_SO_BATCH2_EXPORT_A_VALUE  ((uint64_t)0x422)
#define GJ_SO_BATCH2_EXPORT_B_VALUE  ((uint64_t)0x423)
#define GJ_SO_BATCH2_STAMP_CANON     ((uint64_t)0x42327332ull) /* 'B2s2' */
#define GJ_SO_BATCH2_SOFT_WAVE       40u

/* greppable: libgj-so: soft batch2 wave=40 */
static const char g_szSoBatch2SoftInventory[] =
	"libgj-so: soft batch2 wave=40 areas=9 "
	"export_a,export_b,stamp,init,id,chain_next,chain_find,get,probe "
	"marker_a=0x422 marker_b=0x423 stamp=B2s2 freestanding=1 "
	"multi_server=0 confine=0 bar3=0";

/* greppable: GJ_SO_BATCH2_SOFT_MARKER */
volatile uint64_t gj_so_batch2_export_a = GJ_SO_BATCH2_EXPORT_A_VALUE;
volatile uint64_t gj_so_batch2_export_b = GJ_SO_BATCH2_EXPORT_B_VALUE;

/* greppable: GJ_SO_BATCH2_SOFT_STAMP */
volatile uint64_t gj_so_batch2_soft_stamp = GJ_SO_BATCH2_STAMP_CANON;

void
gj_so_batch2_init(void)
{
	/* greppable: GJ_SO_BATCH2_SOFT_INIT */
	gj_so_batch2_export_a = GJ_SO_BATCH2_EXPORT_A_VALUE;
	gj_so_batch2_export_b = GJ_SO_BATCH2_EXPORT_B_VALUE;
	gj_so_batch2_soft_stamp = GJ_SO_BATCH2_STAMP_CANON;
}

uint64_t
gj_so_batch2_id(void)
{
	return gj_so_batch2_export_a ^ gj_so_batch2_export_b;
}

/*
 * One SysV DT_HASH chain step: chain[idx] while idx in range, else 0.
 * pChain may be NULL (returns 0). Bounds-checked freestanding helper.
 * Index 0 is STN_UNDEF / empty-bucket terminator (classic SysV).
 */
uint32_t
gj_so_batch2_chain_next(const uint32_t *pChain, uint32_t u32Idx,
    uint32_t u32Nchain)
{
	if (pChain == NULL || u32Nchain == 0 || u32Idx >= u32Nchain) {
		return 0;
	}
	return pChain[u32Idx];
}

/*
 * Soft chain walk: start at u32Start, follow pChain until u32Guard hits
 * u32Nchain or a zero link. Invokes pfnMatch(u32Idx, pCtx) on each index;
 * returns the first index where pfnMatch is non-zero, or 0 if none.
 *
 * Models the kernel/ld-gj SysV bucket→chain walk without requiring a live
 * ELF image. Soft only — not a product resolve path. Uses batch1 streq
 * style freestanding match via caller-supplied pfnMatch.
 */
uint32_t
gj_so_batch2_chain_find(const uint32_t *pChain, uint32_t u32Nchain,
    uint32_t u32Start, int (*pfnMatch)(uint32_t u32Idx, void *pCtx),
    void *pCtx)
{
	uint32_t u32Idx;
	uint32_t u32Guard;

	if (pChain == NULL || u32Nchain == 0 || pfnMatch == NULL) {
		return 0;
	}
	u32Idx = u32Start;
	for (u32Guard = 0;
	    u32Idx != 0 && u32Idx < u32Nchain && u32Guard < u32Nchain;
	    u32Guard++) {
		if (pfnMatch(u32Idx, pCtx) != 0) {
			return u32Idx;
		}
		u32Idx = pChain[u32Idx];
	}
	return 0;
}

/*
 * Soft get of the batch XOR id. Read-only; does not mutate.
 */
uint64_t
gj_so_batch2_soft_get(void)
{
	/* greppable: GJ_SO_BATCH2_SOFT_GET */
	return gj_so_batch2_export_a ^ gj_so_batch2_export_b;
}

/*
 * Soft probe: 1 if both markers + stamp are canonical; on soft miss
 * carefully restores and returns 0. Never hard-fails.
 */
int
gj_so_batch2_soft_probe(void)
{
	/* greppable: GJ_SO_BATCH2_SOFT_PROBE */
	if (gj_so_batch2_export_a != GJ_SO_BATCH2_EXPORT_A_VALUE ||
	    gj_so_batch2_export_b != GJ_SO_BATCH2_EXPORT_B_VALUE) {
		gj_so_batch2_export_a = GJ_SO_BATCH2_EXPORT_A_VALUE;
		gj_so_batch2_export_b = GJ_SO_BATCH2_EXPORT_B_VALUE;
		gj_so_batch2_soft_stamp = GJ_SO_BATCH2_STAMP_CANON;
		return 0;
	}
	if (gj_so_batch2_soft_stamp != GJ_SO_BATCH2_STAMP_CANON) {
		gj_so_batch2_soft_stamp = GJ_SO_BATCH2_STAMP_CANON;
		return 0;
	}
	return 1;
}

/* Cold soft inventory: Wave 40 stamp. Grep: libgj-so: soft batch2 wave= */
unsigned
gj_so_batch2_soft_wave(void)
{
	return (unsigned)GJ_SO_BATCH2_SOFT_WAVE;
}

/* Cold soft inventory line. Grep: libgj-so: soft batch2 */
const char *
gj_so_batch2_soft_inventory(void)
{
	return g_szSoBatch2SoftInventory;
}
