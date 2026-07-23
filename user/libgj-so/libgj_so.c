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
 * Soft deepen surface (Wave 14 exclusive; extra dynsyms enrich DT_HASH
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
 *   greppable: libgj-so: soft deepen wave=14
 * Soft batches live under src/ (unwired). Diagnostics only — not bar3.
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

/* Wave 14 soft inventory stamp (source greppable; not a smoke contract). */
#define GJ_SO_SOFT_WAVE         14u
#define GJ_SO_SOFT_AREAS        9u
/* areas: export init get hash stamp caps probe touch path */

/* greppable: GJ_SO_SOFT_EXPORT_CANON */
volatile uint64_t gj_so_export = GJ_SO_EXPORT_CANON;

/* greppable: GJ_SO_SOFT_STAMP */
volatile uint64_t gj_so_soft_stamp = GJ_SO_SOFT_STAMP_CANON;

/* Soft presence / capability word (data export; GLOB_DAT soft target). */
/* greppable: GJ_SO_SOFT_CAPS */
volatile uint32_t gj_so_soft_caps = GJ_SO_SOFT_CAP_MASK;

/*
 * Cold soft inventory blob (rodata). Never consulted by hot product path.
 * Grep: libgj-so: soft
 */
static const char g_szSoSoftInventory[] =
    "libgj-so: soft inventory export=0x42 hash=sysv "
    "stamp=SYSV caps=0x1f areas=9 wave=14 "
    "symbols=gj_so_export,gj_so_init,gj_so_get_export,gj_so_sysv_hash,"
    "gj_so_soft_stamp,gj_so_soft_caps,gj_so_soft_get,gj_so_soft_id,"
    "gj_so_soft_probe,gj_so_soft_touch "
    "policy=freestanding,sysv_hash,no_libc,soft_null "
    "deepen=wave14 hot_path=clean";

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
	return gj_so_export;
}

/*
 * Soft SO identity for multi-SO probes (stamp, not the 0x42 contract).
 */
uint64_t
gj_so_soft_id(void)
{
	/* greppable: GJ_SO_SOFT_ID */
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
	gj_so_export = GJ_SO_EXPORT_CANON;
	gj_so_soft_stamp = GJ_SO_SOFT_STAMP_CANON;
	gj_so_soft_caps = GJ_SO_SOFT_CAP_MASK;
	return gj_so_export;
}

/*
 * Cold soft inventory accessor — not used by any product resolve path.
 * Returns the greppable "libgj-so: soft …" product line (NUL-terminated).
 * Grep: libgj-so: soft inventory
 * greppable: libgj-so: soft deepen wave=14
 */
const char *
gj_so_soft_inventory(void)
{
	return g_szSoSoftInventory;
}

/* Cold soft inventory: Wave 14 stamp. Grep: libgj-so: soft wave= */
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
