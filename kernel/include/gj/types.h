/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Core scalar types for GreenJade kernel + freestanding user libraries.
 *
 * Scope
 * -----
 * Header-only typedef aliases over C11 freestanding fixed-width integers and
 * pointer-sized addresses. No runtime code, no architecture ifdefs, no libc.
 *
 * Design intent
 * -------------
 * Short names (u8/u32/…) keep hot paths and struct layouts readable without
 * sacrificing fixed width. Address and status aliases force a single spelling
 * for physical/virtual addresses and return codes across PMM, VMM, caps, IPC.
 *
 * Include rules (freestanding)
 * ----------------------------
 * Only <stdint.h>, <stddef.h>, <stdbool.h> — no hosted libc. Safe from early
 * boot, interrupt context, and user libgj builds that share the same surface.
 *
 * Naming crosswalk (STYLE.md Systems Hungarian)
 * ---------------------------------------------
 *   u8/u16/u32/u64, i8/…/i64  → variable prefixes match type width
 *   gj_vaddr_t                → va…   (virtual address)
 *   gj_paddr_t                → pa…   (physical address)
 *   gj_status_t               → st…   (0 = OK, negative GJ_ERR_*)
 *   struct gj_cap_ref         → ref… / cap… (not a bare integer; see gj/cap.h)
 *
 * Non-goals
 * ---------
 * Capability handles are NOT typedef'd here as a single integer. Scheme A uses
 * struct gj_cap_ref { u64 slot; u32 gen } so stale handles fail closed — see
 * docs/CAP_ADDRESSING.md and docs/SECURITY_CORE_DESIGN.md (S1–S2).
 */
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* ---- Fixed-width integers ----------------------------------------------- */
/*
 * Prefer these over bare int/long in kernel structs, wire formats, and
 * registers. Width is ABI-stable across x86_64 and aarch64 freestanding builds.
 *
 * Signed variants (i*) are for quantities that may be negative (offsets,
 * Linux-style errno-shaped values at some ABI edges). Prefer u* for sizes,
 * counts-as-unsigned, bitfields, and generation counters.
 */
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

/* ---- Address and status aliases ----------------------------------------- */

/**
 * Virtual address as used by the CPU MMU (kernel or user space).
 *
 * Backed by uintptr_t so pointer arithmetic and casting stay portable without
 * assuming a fixed 64-bit VA on every future port. On product arches (x86_64,
 * aarch64) this is 64-bit; do not pack flags into unused high bits without an
 * explicit documented scheme.
 *
 * Hungarian: vaEntry, vaStack, vaMapBase.
 */
typedef uintptr_t gj_vaddr_t;

/**
 * Physical address (frame base, MMIO PA, firmware memory map entries).
 *
 * Always 64-bit so multi-TiB machines and high MMIO do not force a later ABI
 * break (see GJ_PMM_MAX_PHYS_TIB / HHDM in gj/config.h). Zero is a valid PA
 * only where the caller documents it (e.g. "0 means allocation failure" for
 * some PMM helpers — check that API; do not assume globally).
 *
 * Hungarian: paBase, paPage, paKernelStart.
 */
typedef uint64_t  gj_paddr_t;

/*
 * Capability user handle is struct gj_cap_ref (slot u64 + gen u32) — see gj/cap.h.
 * Deliberately not a typedef here: packing into one u64 invited silent stale use.
 */

/**
 * Kernel-native status: GJ_OK (0) or negative GJ_ERR_* from gj/error.h.
 *
 * Convention (STYLE.md §Error handling):
 *   - Success is exactly 0 (GJ_OK).
 *   - Failure is a negative enum value from error.h; never a positive errno.
 *   - Callers test `if (st != GJ_OK)` / `if (st < 0)`; do not treat nonzero
 *     positive as success.
 *
 * Some personality edges (doors, Linux syscall shims) return Linux-style
 * negative errno (i64) instead; those APIs document the ABI explicitly and
 * are not gj_status_t. Do not mix the two without an explicit conversion.
 *
 * Hungarian: stResult, stMap, stCap.
 */
typedef int       gj_status_t;
