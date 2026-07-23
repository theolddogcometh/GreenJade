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
 * Soft surface (unwired):
 *   gj_so_batch2_export_a / _b — distinct data markers (0x422 / 0x423)
 *   gj_so_batch2_init          — restores both markers
 *   gj_so_batch2_id            — XOR of markers (-Werror clean)
 *   gj_so_batch2_chain_next    — one step of SysV hash chain walk
 *   gj_so_batch2_chain_find    — soft linear chain probe by name hash slot
 */
#include <stddef.h>
#include <stdint.h>

/* Matches GJ_SO_BATCH2_EXPORT_*_VALUE in include/gj_so.h (soft section). */
#define GJ_SO_BATCH2_EXPORT_A_VALUE  ((uint64_t)0x422)
#define GJ_SO_BATCH2_EXPORT_B_VALUE  ((uint64_t)0x423)

volatile uint64_t gj_so_batch2_export_a = GJ_SO_BATCH2_EXPORT_A_VALUE;
volatile uint64_t gj_so_batch2_export_b = GJ_SO_BATCH2_EXPORT_B_VALUE;

void
gj_so_batch2_init(void)
{
	gj_so_batch2_export_a = GJ_SO_BATCH2_EXPORT_A_VALUE;
	gj_so_batch2_export_b = GJ_SO_BATCH2_EXPORT_B_VALUE;
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
