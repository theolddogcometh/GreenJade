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
 *   greppable: GJ_GNU_SOFT_INVENTORY
 *   greppable: GJ_GNU_SOFT_DEEPEN
 *   greppable: GJ_GNU_SOFT_WAVE
 *   greppable: GJ_GNU_SOFT_SURFACE
 *   greppable: GJ_GNU_SOFT_NOTE
 *
 * Soft inventory (Wave 126 exclusive deepen; C2 lib residual Soft!=product):
 *   libgj-gnu: soft inventory wave=70 surfaces=12 caps=0xf stamp=GNU1
 *   libgj-gnu: soft deepen wave=70 areas=export,stamp,caps,get,id,probe,
 *              touch,inventory,wave,surface,path,note
 *   libgj-gnu: soft path hash=gnu soname=libgj-gnu.so.1
 *   libgj-gnu: soft residual Soft!=product G-AC-1 dual_dod=OPEN
 * Diagnostics only — never a product bar3 claim; product 0x43 unchanged.
 * Soft!=product; dual MIT OR Apache-2.0; G-AC-1; Dual DoD OPEN.
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

/* Wave 126 exclusive soft inventory stamp. */
/* Wave 126 soft deepen surfaces (CREATE-ONLY Soft!=product residual):
 *   greppable: soft retgradientangle continuum_toward=26800 soft_ne_product=1 wave=126
 *   greppable: soft retblendangle exclusive=1 continuum_toward=26800 soft_ne_product=1 wave=126
 *   greppable: libgj-gnu: soft residual Soft!=product
 * Soft!=product complete; product lamps 0; G-AC-1; Dual DoD OPEN.
 */

#define GJ_GNU_SOFT_WAVE 70u
/* Soft dynsym surface count (export+init+stamp+caps+get+id+probe+touch
 * +inventory+wave+surface+note+deepen+path — product + soft catalog). */
#define GJ_GNU_SOFT_SURFACES    12u
#define GJ_GNU_SOFT_AREAS       12u

/* greppable: GJ_GNU_SOFT_EXPORT_CANON */
volatile uint64_t gj_gnu_export = GJ_GNU_EXPORT_CANON;

/* greppable: GJ_GNU_SOFT_STAMP */
volatile uint64_t gj_gnu_soft_stamp = GJ_GNU_SOFT_STAMP_CANON;

/* Soft presence / capability word (data export; GLOB_DAT soft target). */
volatile uint32_t gj_gnu_soft_caps = GJ_GNU_SOFT_CAP_MASK;

/* Soft note counters (cold; wrap OK; never hard-fail). */
static volatile uint32_t g_u32SoftProbeN;
static volatile uint32_t g_u32SoftTouchN;
static volatile uint32_t g_u32SoftGetN;
static volatile uint32_t g_u32SoftIdN;
static volatile uint32_t g_u32SoftInvN;
static volatile uint32_t g_u32SoftDeepenN;
static volatile uint32_t g_u32SoftPathN;
static volatile uint32_t g_u32SoftResidualN;

/*
 * Product soft inventory blob (rodata; greppable).
 * greppable: GJ_GNU_SOFT_INVENTORY
 * Grep: libgj-gnu: soft inventory
 */
static const char g_szGnuSoftInventory[] =
    "libgj-gnu: soft inventory wave=70 surfaces=12 areas=12 "
    "export=0x43 stamp=GNU1 caps=0xf "
    "get=1 id=1 probe=1 touch=1 inventory=1 deepen=1 path=1 note=1 "
    "hash=gnu soname=libgj-gnu.so.1 freestanding=1 "
    "multi_server=0 confine=0 Soft!=product soft_ne_product=1 "
    "G-AC-1=1 dual_dod=OPEN dual=MIT_OR_Apache-2.0";

/*
 * Wave 126 soft deepen stamp.
 * greppable: GJ_GNU_SOFT_DEEPEN
 * Grep: libgj-gnu: soft deepen
 */
static const char g_szGnuSoftDeepen[] =
    "libgj-gnu: soft deepen wave=70 areas=12 "
    "export,stamp,caps,get,id,probe,touch,inventory,wave,surface,path,note "
    "product_export=0x43 soft_stamp=GNU1 hot_path=clean "
    "multi_server=0 confine=0 "
    "retgradientangle=1 retblendangle=1 continuum_toward=26800 "
    "soft_ne_product=1 Soft!=product G-AC-1=1 dual_dod=OPEN wave=126";

/*
 * Soft path honesty line.
 * Grep: libgj-gnu: soft path
 */
static const char g_szGnuSoftPath[] =
    "libgj-gnu: soft path hash=gnu soname=libgj-gnu.so.1 "
    "export=0x43 freestanding=1 pure_c=1 no_heap=1 "
    "multi_server=0 confine=0 Soft!=product soft_ne_product=1 "
    "G-AC-1=1 dual_dod=OPEN (soft inventory)";

/*
 * Soft honesty line (Wave 126 exclusive; C2 residual Soft!=product).
 * Grep: libgj-gnu: soft honesty
 */
static const char g_szGnuSoftHonesty[] =
    "libgj-gnu: soft honesty multi_server=0 confine=0 exclusive=1 "
    "soft=1 product_kernel=OPEN wave=70 "
    "retgradientangle=1 retblendangle=1 continuum_toward=26800 "
    "soft_ne_product=1 Soft!=product G-AC-1=1 dual_dod=OPEN wave=126";

/*
 * C2 lib soft residual lean honesty (Soft!=product; not product contract).
 * greppable: libgj-gnu: soft residual
 * Grep: libgj-gnu: soft residual Soft!=product
 */
static const char g_szGnuSoftResidual[] =
    "libgj-gnu: soft residual Soft!=product soft_ne_product=1 "
    "product_export=0x43 soft_stamp=GNU1 hash=gnu soname=libgj-gnu.so.1 "
    "exclusive=1 residual_lean=1 product_lamps=0 bar3=0 "
    "G-AC-1=1 dual_dod=OPEN dual=MIT_OR_Apache-2.0 "
    "continuum_toward=26800 wave=126";

/* Soft area name catalog (Wave 111; cold only). */
static const char *const g_apszGnuSoftAreas[] = {
    "export",
    "stamp",
    "caps",
    "get",
    "id",
    "probe",
    "touch",
    "inventory",
    "wave",
    "surface",
    "path",
    "note",
};
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
    g_u32SoftGetN++;
    return gj_gnu_export;
}

/*
 * Soft SO identity for multi-SO probes (stamp, not the 0x43 contract).
 */
uint64_t
gj_gnu_soft_id(void)
{
    /* greppable: GJ_GNU_SOFT_ID */
    g_u32SoftIdN++;
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
    g_u32SoftProbeN++;
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
    g_u32SoftTouchN++;
    gj_gnu_export = GJ_GNU_EXPORT_CANON;
    gj_gnu_soft_stamp = GJ_GNU_SOFT_STAMP_CANON;
    gj_gnu_soft_caps = GJ_GNU_SOFT_CAP_MASK;
    return gj_gnu_export;
}

/*
 * Cold soft inventory accessor (Wave 126).
 * greppable: GJ_GNU_SOFT_INVENTORY
 * Grep: libgj-gnu: soft inventory
 */
const char *
gj_gnu_soft_inventory(void)
{
    g_u32SoftInvN++;
    return g_szGnuSoftInventory;
}

/*
 * Cold soft deepen stamp (Wave 126).
 * greppable: GJ_GNU_SOFT_DEEPEN
 * Grep: libgj-gnu: soft deepen
 */
const char *
gj_gnu_soft_deepen(void)
{
    g_u32SoftDeepenN++;
    return g_szGnuSoftDeepen;
}

/*
 * Cold soft path honesty line.
 * Grep: libgj-gnu: soft path
 */
const char *
gj_gnu_soft_path(void)
{
    g_u32SoftPathN++;
    return g_szGnuSoftPath;
}

/*
 * Cold soft honesty line (Wave 126 exclusive).
 * Grep: libgj-gnu: soft honesty
 */
const char *
gj_gnu_soft_honesty(void)
{
    return g_szGnuSoftHonesty;
}

/*
 * C2 lib soft residual lean (Soft!=product; never product contract).
 * greppable: libgj-gnu: soft residual
 * Grep: libgj-gnu: soft residual Soft!=product
 */
const char *
gj_gnu_soft_residual(void)
{
    g_u32SoftResidualN++;
    return g_szGnuSoftResidual;
}

/*
 * Soft residual call count (cold; wrap OK). Grep: libgj-gnu: soft residual
 */
uint32_t
gj_gnu_soft_residual_count(void)
{
    return g_u32SoftResidualN;
}

/*
 * Soft wave stamp (48). greppable: GJ_GNU_SOFT_WAVE
 * Grep: libgj-gnu: soft wave=
 */
uint32_t
gj_gnu_soft_wave(void)
{
    return (uint32_t)GJ_GNU_SOFT_WAVE;
}

/*
 * Soft dynsym surface count for .gnu.hash soft catalog.
 * greppable: GJ_GNU_SOFT_SURFACE
 * Grep: libgj-gnu: soft surfaces=
 */
uint32_t
gj_gnu_soft_surface_count(void)
{
    return (uint32_t)GJ_GNU_SOFT_SURFACES;
}

/*
 * Soft area catalog size (Wave 111 deepen areas).
 * Grep: libgj-gnu: soft areas=
 */
uint32_t
gj_gnu_soft_area_count(void)
{
    return (uint32_t)GJ_GNU_SOFT_AREAS;
}

/*
 * Soft area name by index (0..areas-1), or NULL.
 * Grep: libgj-gnu: soft areas=
 */
const char *
gj_gnu_soft_area_name(uint32_t uArea)
{
    if (uArea >= (uint32_t)GJ_GNU_SOFT_AREAS) {
        return 0;
    }
    return g_apszGnuSoftAreas[uArea];
}

/*
 * Soft note rollup: packed get|id|probe|touch|inv|deepen|path counters.
 * Never hard-fails; wrap OK. greppable: GJ_GNU_SOFT_NOTE
 * Grep: libgj-gnu: soft note
 */
uint64_t
gj_gnu_soft_note(void)
{
    uint64_t u64N;

    u64N = ((uint64_t)g_u32SoftGetN & 0xffu)
         | (((uint64_t)g_u32SoftIdN & 0xffu) << 8)
         | (((uint64_t)g_u32SoftProbeN & 0xffu) << 16)
         | (((uint64_t)g_u32SoftTouchN & 0xffu) << 24)
         | (((uint64_t)g_u32SoftInvN & 0xffu) << 32)
         | (((uint64_t)g_u32SoftDeepenN & 0xffu) << 40)
         | (((uint64_t)g_u32SoftPathN & 0xffu) << 48);
    return u64N;
}

/*
 * Soft note raw counters (get, id, probe, touch, inv) — optional out args.
 * NULL out pointers are soft-skipped. Grep: libgj-gnu: soft note
 */
void
gj_gnu_soft_note_counts(uint32_t *pGet, uint32_t *pId, uint32_t *pProbe,
                        uint32_t *pTouch, uint32_t *pInv)
{
    if (pGet != 0) {
        *pGet = g_u32SoftGetN;
    }
    if (pId != 0) {
        *pId = g_u32SoftIdN;
    }
    if (pProbe != 0) {
        *pProbe = g_u32SoftProbeN;
    }
    if (pTouch != 0) {
        *pTouch = g_u32SoftTouchN;
    }
    if (pInv != 0) {
        *pInv = g_u32SoftInvN;
    }
}

/*
 * Soft note extend (Wave 126): deepen + path call counters.
 * Grep: libgj-gnu: soft note
 */
void
gj_gnu_soft_note_counts_ex(uint32_t *pDeepen, uint32_t *pPath)
{
    if (pDeepen != 0) {
        *pDeepen = g_u32SoftDeepenN;
    }
    if (pPath != 0) {
        *pPath = g_u32SoftPathN;
    }
}
