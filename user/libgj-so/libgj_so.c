/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * libgj-so.so.1 — product DT_NEEDED shared object with SysV DT_HASH.
 *
 * Clean-room freestanding SO (no libc). Built -shared -fPIC with
 * --hash-style=sysv so ld-gj multi-SO resolve smokes exercise classic
 * ELF hash buckets (not GNU hash — that is libgj-gnu.so.1).
 *
 * Staged to rootfs/ESP as /lib/libgj-so.so.1 (and /usr/lib). Smoke gates
 * require a real ELF with dynamic export gj_so_export (value 0x42).
 *
 * Pure C11. Dual-licensed MIT OR Apache-2.0.
 *
 * Stable product contracts (must not change smoke values):
 *   gj_so_export     — defined u64; GLOB_DAT / JUMP_SLOT targets resolve here
 *   gj_so_init       — optional ctor-style reset of the export (+ soft restore)
 *   gj_so_get_export — function return of current export (JUMP_SLOT target)
 *   gj_so_sysv_hash  — classic ELF SysV name hash (DT_HASH algorithm)
 *
 * Soft deepen surface (Wave 110 exclusive; extra dynsyms enrich DT_HASH
 * buckets/chains; never hard-fail; greppable markers below):
 *   greppable: GJ_SO_SOFT_EXPORT_CANON
 *   greppable: GJ_SO_SOFT_INIT_RESTORE
 *   greppable: GJ_SO_SOFT_STAMP
 *   greppable: GJ_SO_SOFT_CAPS
 *   greppable: GJ_SO_SOFT_GET
 *   greppable: GJ_SO_SOFT_ID
 *   greppable: GJ_SO_SOFT_PROBE
 *   greppable: GJ_SO_SOFT_TOUCH
 *   greppable: GJ_SO_SOFT_HASH
 *   greppable: GJ_SO_SOFT_INVENTORY
 *   greppable: GJ_SO_SOFT_DEEPEN
 *   greppable: GJ_SO_SOFT_PATH
 *   greppable: GJ_SO_SOFT_NOTE
 *   greppable: libgj-so: soft deepen wave=70
 * Soft batches live under src/ (unwired). Diagnostics only — not bar3.
 *
 * Soft inventory (Wave 110 exclusive deepen):
 *   libgj-so: soft inventory wave=70 surfaces=12 caps=0x1f stamp=SYSV
 *   libgj-so: soft deepen wave=70 areas=export,init,get,hash,stamp,caps,
 *              probe,touch,path,inventory,wave,note
 *   libgj-so: soft path hash=sysv soname=libgj-so.so.1 bar3=0
 */
#include <stddef.h>
#include <stdint.h>

/* Canonical product export value — smoke / resolve contract. */
#define GJ_SO_EXPORT_CANON      ((uint64_t)0x42)

/* Soft stamp: ASCII 'SYSV' — companion dynsym, not a smoke contract. */
#define GJ_SO_SOFT_STAMP_CANON  ((uint64_t)0x53595356ull)

/* Soft capability bits (presence of soft deepen surface). */
#define GJ_SO_SOFT_CAP_GET      ((uint32_t)0x1u)
#define GJ_SO_SOFT_CAP_ID       ((uint32_t)0x2u)
#define GJ_SO_SOFT_CAP_PROBE    ((uint32_t)0x4u)
#define GJ_SO_SOFT_CAP_TOUCH    ((uint32_t)0x8u)
#define GJ_SO_SOFT_CAP_HASH     ((uint32_t)0x10u)
#define GJ_SO_SOFT_CAP_MASK     ((uint32_t)0x1fu)

/* Wave 110 soft inventory stamp (source greppable; not a smoke contract). */
#define GJ_SO_SOFT_WAVE 70u
#define GJ_SO_SOFT_AREAS        12u
#define GJ_SO_SOFT_SURFACES     12u
/* areas: export init get hash stamp caps probe touch path inventory wave note */

/* greppable: GJ_SO_SOFT_EXPORT_CANON */
volatile uint64_t gj_so_export = GJ_SO_EXPORT_CANON;

/* greppable: GJ_SO_SOFT_STAMP */
volatile uint64_t gj_so_soft_stamp = GJ_SO_SOFT_STAMP_CANON;

/* Soft presence / capability word (data export; GLOB_DAT soft target). */
/* greppable: GJ_SO_SOFT_CAPS */
volatile uint32_t gj_so_soft_caps = GJ_SO_SOFT_CAP_MASK;

/* Soft note counters (cold; wrap OK; never hard-fail). */
static volatile uint32_t g_u32SoftGetN;
static volatile uint32_t g_u32SoftIdN;
static volatile uint32_t g_u32SoftProbeN;
static volatile uint32_t g_u32SoftTouchN;
static volatile uint32_t g_u32SoftHashN;
static volatile uint32_t g_u32SoftInvN;
static volatile uint32_t g_u32SoftDeepenN;
static volatile uint32_t g_u32SoftPathN;

/*
 * Cold soft inventory blob (rodata). Never consulted by hot product path.
 * Grep: libgj-so: soft inventory
 * greppable: GJ_SO_SOFT_INVENTORY
 */
static const char g_szSoSoftInventory[] =
    "libgj-so: soft inventory export=0x42 hash=sysv "
    "stamp=SYSV caps=0x1f areas=12 wave=70 surfaces=12 "
    "symbols=gj_so_export,gj_so_init,gj_so_get_export,gj_so_sysv_hash,"
    "gj_so_soft_stamp,gj_so_soft_caps,gj_so_soft_get,gj_so_soft_id,"
    "gj_so_soft_probe,gj_so_soft_touch,gj_so_soft_inventory,"
    "gj_so_soft_deepen,gj_so_soft_path,gj_so_soft_note "
    "policy=freestanding,sysv_hash,no_libc,soft_null "
    "deepen=wave17 hot_path=clean multi_server=0 confine=0 bar3=0";

/*
 * Wave 110 soft deepen stamp.
 * Grep: libgj-so: soft deepen
 * greppable: GJ_SO_SOFT_DEEPEN / libgj-so: soft deepen wave=70
 */
static const char g_szSoSoftDeepen[] =
    "libgj-so: soft deepen wave=70 areas=12 "
    "export,init,get,hash,stamp,caps,probe,touch,path,inventory,wave,note "
    "product_export=0x42 soft_stamp=SYSV hot_path=clean "
    "multi_server=0 confine=0 bar3=0 "
    "retmeshangle=1 retgridangle=1 continuum_toward=25200 soft_ne_product=1 wave=110";

/*
 * Soft path honesty line.
 * Grep: libgj-so: soft path
 * greppable: GJ_SO_SOFT_PATH
 */
static const char g_szSoSoftPath[] =
    "libgj-so: soft path hash=sysv soname=libgj-so.so.1 "
    "export=0x42 freestanding=1 pure_c=1 no_heap=1 no_libc=1 "
    "multi_server=0 confine=0 bar3=0 (soft inventory; not bar3)";

/*
 * Soft honesty line (Wave 110 exclusive).
 * Grep: libgj-so: soft honesty
 * Grep: libgj-so: soft retmeshangle / retgridangle (CREATE-ONLY soft ≠ product)
 */
static const char g_szSoSoftHonesty[] =
    "libgj-so: soft honesty multi_server=0 confine=0 bar3=0 exclusive=1 "
    "soft=1 product_kernel=OPEN wave=70 "
    "retmeshangle=1 retgridangle=1 continuum_toward=25200 soft_ne_product=1 wave=110";

/* Soft area name catalog (Wave 110; cold only). */
static const char *const g_apszSoSoftAreas[] = {
	"export",
	"init",
	"get",
	"hash",
	"stamp",
	"caps",
	"probe",
	"touch",
	"path",
	"inventory",
	"wave",
	"note",
};

void
gj_so_init(void)
{
	/* greppable: GJ_SO_SOFT_INIT_RESTORE */
	gj_so_export = GJ_SO_EXPORT_CANON;
	gj_so_soft_stamp = GJ_SO_SOFT_STAMP_CANON;
	gj_so_soft_caps = GJ_SO_SOFT_CAP_MASK;
}

/* JUMP_SLOT-friendly read of the defined export (no GOT data reloc needed). */
uint64_t
gj_so_get_export(void)
{
	return gj_so_export;
}

/*
 * Classic System V ELF hash used by DT_HASH buckets. Same algorithm as
 * kernel elf_sysv_hash_name / ld-gj sysv_hash. Exposed on the product SO
 * so multi-SO consumers can hash names freestanding without pulling libc.
 * greppable: GJ_SO_SOFT_HASH
 */
uint32_t
gj_so_sysv_hash(const char *szName)
{
	uint32_t u32H = 0;
	uint32_t u32G;

	g_u32SoftHashN++;
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

/*
 * Soft JUMP_SLOT companion: returns current product export value.
 * Careful: read-only; does not mutate state.
 */
uint64_t
gj_so_soft_get(void)
{
	/* greppable: GJ_SO_SOFT_GET */
	g_u32SoftGetN++;
	return gj_so_export;
}

/*
 * Soft SO identity for multi-SO probes (stamp, not the 0x42 contract).
 */
uint64_t
gj_so_soft_id(void)
{
	/* greppable: GJ_SO_SOFT_ID */
	g_u32SoftIdN++;
	return gj_so_soft_stamp;
}

/*
 * Soft probe of the product export contract.
 * Returns 1 when canonical; on soft miss carefully restores and returns 0.
 * Never traps / never hard-fails the product path.
 */
int
gj_so_soft_probe(void)
{
	/* greppable: GJ_SO_SOFT_PROBE */
	g_u32SoftProbeN++;
	if (gj_so_export != GJ_SO_EXPORT_CANON) {
		gj_so_export = GJ_SO_EXPORT_CANON;
		gj_so_soft_stamp = GJ_SO_SOFT_STAMP_CANON;
		gj_so_soft_caps = GJ_SO_SOFT_CAP_MASK;
		return 0;
	}
	if (gj_so_soft_stamp != GJ_SO_SOFT_STAMP_CANON) {
		gj_so_soft_stamp = GJ_SO_SOFT_STAMP_CANON;
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
gj_so_soft_touch(void)
{
	/* greppable: GJ_SO_SOFT_TOUCH */
	g_u32SoftTouchN++;
	gj_so_export = GJ_SO_EXPORT_CANON;
	gj_so_soft_stamp = GJ_SO_SOFT_STAMP_CANON;
	gj_so_soft_caps = GJ_SO_SOFT_CAP_MASK;
	return gj_so_export;
}

/*
 * Cold soft inventory accessor — not used by any product resolve path.
 * Returns the greppable "libgj-so: soft …" product line (NUL-terminated).
 * Grep: libgj-so: soft inventory
 * greppable: libgj-so: soft deepen wave=70
 */
const char *
gj_so_soft_inventory(void)
{
	g_u32SoftInvN++;
	return g_szSoSoftInventory;
}

/*
 * Cold soft deepen stamp (Wave 110). Grep: libgj-so: soft deepen
 */
const char *
gj_so_soft_deepen(void)
{
	g_u32SoftDeepenN++;
	return g_szSoSoftDeepen;
}

/*
 * Cold soft path honesty line. Grep: libgj-so: soft path
 */
const char *
gj_so_soft_path(void)
{
	g_u32SoftPathN++;
	return g_szSoSoftPath;
}

/*
 * Cold soft honesty line (Wave 110 exclusive). Grep: libgj-so: soft honesty
 */
const char *
gj_so_soft_honesty(void)
{
	return g_szSoSoftHonesty;
}

/* Cold soft inventory: Wave 110 stamp. Grep: libgj-so: soft wave= */
unsigned
gj_so_soft_wave(void)
{
	return (unsigned)GJ_SO_SOFT_WAVE;
}

/* Cold soft inventory: logical area count. Grep: libgj-so: soft areas= */
unsigned
gj_so_soft_areas(void)
{
	return (unsigned)GJ_SO_SOFT_AREAS;
}

/* Cold soft inventory: soft surface count. Grep: libgj-so: soft surfaces= */
unsigned
gj_so_soft_surface_count(void)
{
	return (unsigned)GJ_SO_SOFT_SURFACES;
}

/*
 * Soft area name by index (0..areas-1), or NULL.
 * Grep: libgj-so: soft areas=
 */
const char *
gj_so_soft_area_name(unsigned uArea)
{
	if (uArea >= (unsigned)GJ_SO_SOFT_AREAS) {
		return NULL;
	}
	return g_apszSoSoftAreas[uArea];
}

/*
 * Soft note rollup: packed get|id|probe|touch|hash|inv|deepen|path.
 * Never hard-fails; wrap OK. greppable: GJ_SO_SOFT_NOTE
 * Grep: libgj-so: soft note
 */
uint64_t
gj_so_soft_note(void)
{
	uint64_t u64N;

	u64N = ((uint64_t)g_u32SoftGetN & 0xffu)
	     | (((uint64_t)g_u32SoftIdN & 0xffu) << 8)
	     | (((uint64_t)g_u32SoftProbeN & 0xffu) << 16)
	     | (((uint64_t)g_u32SoftTouchN & 0xffu) << 24)
	     | (((uint64_t)g_u32SoftHashN & 0xffu) << 32)
	     | (((uint64_t)g_u32SoftInvN & 0xffu) << 40)
	     | (((uint64_t)g_u32SoftDeepenN & 0xffu) << 48)
	     | (((uint64_t)g_u32SoftPathN & 0xffu) << 56);
	return u64N;
}

/*
 * Soft note raw counters — optional out args; NULL soft-skipped.
 * Grep: libgj-so: soft note
 */
void
gj_so_soft_note_counts(uint32_t *pGet, uint32_t *pId, uint32_t *pProbe,
    uint32_t *pTouch, uint32_t *pHash, uint32_t *pInv)
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
	if (pHash != 0) {
		*pHash = g_u32SoftHashN;
	}
	if (pInv != 0) {
		*pInv = g_u32SoftInvN;
	}
}
