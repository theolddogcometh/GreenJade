/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * libgj-gnu.so.1 — product DT_NEEDED SO with DT_GNU_HASH.
 *
 * Clean-room freestanding C. Counterpart to libgj-so.so.1 (SysV DT_HASH).
 * Used by ld-gj multi-SO resolve smokes and install rootfs/ESP staging.
 *
 * Build: make libgj-gnu  →  build/user/libgj-gnu.so.1
 *   -shared -nostdlib -fPIC -ffreestanding --hash-style=gnu
 *   -soname libgj-gnu.so.1
 *
 * Pure C11. Dual-licensed MIT OR Apache-2.0.
 *
 * Stable product contracts (must not change smoke values):
 *   gj_gnu_export — data (0x43); GLOB_DAT / JUMP_SLOT resolve target
 *   gj_gnu_init   — function; re-stores the export value
 *
 * Soft deepen surface (extra dynsyms enrich .gnu.hash bloom/chains;
 * never hard-fail; greppable markers below):
 *   greppable: GJ_GNU_SOFT_EXPORT_CANON
 *   greppable: GJ_GNU_SOFT_INIT_RESTORE
 *   greppable: GJ_GNU_SOFT_STAMP
 *   greppable: GJ_GNU_SOFT_GET
 *   greppable: GJ_GNU_SOFT_ID
 *   greppable: GJ_GNU_SOFT_PROBE
 *   greppable: GJ_GNU_SOFT_TOUCH
 */
#include <stdint.h>

/* Canonical product export value — smoke / resolve contract. */
#define GJ_GNU_EXPORT_CANON     ((uint64_t)0x43)

/* Soft stamp: ASCII 'GNU1' — companion dynsym, not a smoke contract. */
#define GJ_GNU_SOFT_STAMP_CANON ((uint64_t)0x474E5531ull)

/* Soft capability bits (presence of soft deepen surface). */
#define GJ_GNU_SOFT_CAP_GET     ((uint32_t)0x1u)
#define GJ_GNU_SOFT_CAP_ID      ((uint32_t)0x2u)
#define GJ_GNU_SOFT_CAP_PROBE   ((uint32_t)0x4u)
#define GJ_GNU_SOFT_CAP_TOUCH   ((uint32_t)0x8u)
#define GJ_GNU_SOFT_CAP_MASK    ((uint32_t)0xfu)

/* greppable: GJ_GNU_SOFT_EXPORT_CANON */
volatile uint64_t gj_gnu_export = GJ_GNU_EXPORT_CANON;

/* greppable: GJ_GNU_SOFT_STAMP */
volatile uint64_t gj_gnu_soft_stamp = GJ_GNU_SOFT_STAMP_CANON;

/* Soft presence / capability word (data export; GLOB_DAT soft target). */
volatile uint32_t gj_gnu_soft_caps = GJ_GNU_SOFT_CAP_MASK;

void
gj_gnu_init(void)
{
    /* greppable: GJ_GNU_SOFT_INIT_RESTORE */
    gj_gnu_export = GJ_GNU_EXPORT_CANON;
    gj_gnu_soft_stamp = GJ_GNU_SOFT_STAMP_CANON;
    gj_gnu_soft_caps = GJ_GNU_SOFT_CAP_MASK;
}

/*
 * Soft JUMP_SLOT companion: returns current product export value.
 * Careful: read-only; does not mutate state.
 */
uint64_t
gj_gnu_soft_get(void)
{
    /* greppable: GJ_GNU_SOFT_GET */
    return gj_gnu_export;
}

/*
 * Soft SO identity for multi-SO probes (stamp, not the 0x43 contract).
 */
uint64_t
gj_gnu_soft_id(void)
{
    /* greppable: GJ_GNU_SOFT_ID */
    return gj_gnu_soft_stamp;
}

/*
 * Soft probe of the product export contract.
 * Returns 1 when canonical; on soft miss carefully restores and returns 0.
 * Never traps / never hard-fails the product path.
 */
int
gj_gnu_soft_probe(void)
{
    /* greppable: GJ_GNU_SOFT_PROBE */
    if (gj_gnu_export != GJ_GNU_EXPORT_CANON) {
        gj_gnu_export = GJ_GNU_EXPORT_CANON;
        return 0;
    }
    return 1;
}

/*
 * Soft touch: re-assert export + stamp + caps in one freestanding call.
 * Useful as a JUMP_SLOT resolve target that exercises store side-effects.
 * Returns the canonical export value after touch.
 */
uint64_t
gj_gnu_soft_touch(void)
{
    /* greppable: GJ_GNU_SOFT_TOUCH */
    gj_gnu_export = GJ_GNU_EXPORT_CANON;
    gj_gnu_soft_stamp = GJ_GNU_SOFT_STAMP_CANON;
    gj_gnu_soft_caps = GJ_GNU_SOFT_CAP_MASK;
    return gj_gnu_export;
}
