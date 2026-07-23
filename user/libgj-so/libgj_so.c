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
 * Product exports (keep minimal; soft batches live under src/ unwired):
 *   gj_so_export     — defined u64; GLOB_DAT / JUMP_SLOT targets resolve here
 *   gj_so_init       — optional ctor-style reset of the export
 *   gj_so_get_export — function return of current export (JUMP_SLOT target)
 *   gj_so_sysv_hash  — classic ELF SysV name hash (DT_HASH algorithm)
 */
#include <stddef.h>
#include <stdint.h>

/* Defined export — GLOB_DAT / JUMP_SLOT resolve to this VA (+load bias). */
volatile uint64_t gj_so_export = 0x42;

void
gj_so_init(void)
{
	gj_so_export = 0x42;
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
