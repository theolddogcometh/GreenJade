/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Product SO surface (libgj-gnu): GNU string-extension batch 1.
 *
 * Freestanding pure C. Dual-licensed MIT OR Apache-2.0.
 * Intended for the parent Makefile to link into libgj-gnu.so.1 (GNU-hash
 * product SO). Not linked today — libgj_gnu.c alone is the shipped SO.
 * Does NOT touch libcgj.
 *
 * Ownership policy: only MISSING GNU-extension symbols are defined in this
 * tree. Candidate symbols that already live in user/libcgj must not be
 * redefined here (strverscmp / memrchr / rawmemchr / strchrnul are PRESENT
 * in libcgj). This TU therefore exports batch markers + soft deepen helpers
 * only until a missing symbol is assigned to this tree.
 *
 * Marker / soft symbols:
 *   gj_gnu_batch1_export      — data (0x471); optional future resolve target
 *   gj_gnu_batch1_soft_stamp  — soft companion stamp ('B1s1')
 *   gj_gnu_batch1_init        — restores markers carefully
 *   gj_gnu_batch1_id          — returns current marker
 *   gj_gnu_batch1_soft_get    — soft read of marker
 *   gj_gnu_batch1_soft_probe  — soft check + careful restore
 *
 * Soft surface (Wave 76 exclusive deepen):
 *   gj_gnu_batch1_soft_wave      — Wave 76 stamp (40)
 *   gj_gnu_batch1_soft_inventory — greppable cold inventory line
 *
 * greppable: GJ_GNU_BATCH1_SOFT_MARKER
 * greppable: GJ_GNU_BATCH1_SOFT_INIT
 * greppable: GJ_GNU_BATCH1_SOFT_GET
 * greppable: GJ_GNU_BATCH1_SOFT_PROBE
 * greppable: GJ_GNU_BATCH1_SOFT_STAMP
 * greppable: libgj-gnu: soft batch1 wave=70
 */
#include <stddef.h>
#include <stdint.h>

#define GJ_GNU_BATCH1_CANON       ((uint64_t)0x471)
#define GJ_GNU_BATCH1_STAMP_CANON ((uint64_t)0x42317331ull) /* 'B1s1' */
#define GJ_GNU_BATCH1_SOFT_WAVE 70u

/* greppable: libgj-gnu: soft batch1 wave=70 */
static const char g_szGnuBatch1SoftInventory[] =
	"libgj-gnu: soft batch1 wave=70 areas=6 "
	"export,stamp,init,id,get,probe "
	"marker=0x471 stamp=B1s1 freestanding=1 multi_server=0 "
	"confine=0 bar3=0";

/* greppable: GJ_GNU_BATCH1_SOFT_MARKER */
volatile uint64_t gj_gnu_batch1_export = GJ_GNU_BATCH1_CANON;

/* greppable: GJ_GNU_BATCH1_SOFT_STAMP */
volatile uint64_t gj_gnu_batch1_soft_stamp = GJ_GNU_BATCH1_STAMP_CANON;

void
gj_gnu_batch1_init(void)
{
    /* greppable: GJ_GNU_BATCH1_SOFT_INIT */
    gj_gnu_batch1_export = GJ_GNU_BATCH1_CANON;
    gj_gnu_batch1_soft_stamp = GJ_GNU_BATCH1_STAMP_CANON;
}

/* Keep TU useful under -Werror -Wunused when only the marker is present. */
uint64_t
gj_gnu_batch1_id(void)
{
    return gj_gnu_batch1_export;
}

/*
 * Soft get of the batch marker. Read-only; does not mutate.
 */
uint64_t
gj_gnu_batch1_soft_get(void)
{
    /* greppable: GJ_GNU_BATCH1_SOFT_GET */
    return gj_gnu_batch1_export;
}

/*
 * Soft probe: 1 if marker is canonical; on soft miss carefully restores
 * marker + stamp and returns 0. Never hard-fails.
 */
int
gj_gnu_batch1_soft_probe(void)
{
    /* greppable: GJ_GNU_BATCH1_SOFT_PROBE */
    if (gj_gnu_batch1_export != GJ_GNU_BATCH1_CANON) {
        gj_gnu_batch1_export = GJ_GNU_BATCH1_CANON;
        gj_gnu_batch1_soft_stamp = GJ_GNU_BATCH1_STAMP_CANON;
        return 0;
    }
    if (gj_gnu_batch1_soft_stamp != GJ_GNU_BATCH1_STAMP_CANON) {
        gj_gnu_batch1_soft_stamp = GJ_GNU_BATCH1_STAMP_CANON;
        return 0;
    }
    return 1;
}

/* Cold soft inventory: Wave 76 stamp. Grep: libgj-gnu: soft batch1 wave= */
unsigned
gj_gnu_batch1_soft_wave(void)
{
	return (unsigned)GJ_GNU_BATCH1_SOFT_WAVE;
}

/* Cold soft inventory line. Grep: libgj-gnu: soft batch1 */
const char *
gj_gnu_batch1_soft_inventory(void)
{
	return g_szGnuBatch1SoftInventory;
}
