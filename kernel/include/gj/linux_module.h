/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft freestanding loader for Linux x86_64 .ko ELF relocatable objects (ET_REL).
 * Pure C11 freestanding. Dual MIT OR Apache-2.0. No GPL / no Linux source copy.
 *
 * Scope
 * -----
 * Seed product path for loading an ELF64 ET_REL image already present in kernel
 * memory (not xz-decompress). SHF_ALLOC sections are copied into PMM pages
 * (HHDM VA), relocations applied, SHN_UNDEF resolved via linux_ksym_lookup.
 * Init/exit symbols are the classic module entry points init_module /
 * cleanup_module.
 *
 * Honesty
 * -------
 * Soft surface only: max 8 modules, limited reloc set, no full Linux module
 * API (modversions, vermagic, livepatch, sysfs). Not G-AC-1 product acceptance
 * for shipping GPL .ko as product AC — staging / ABI-module-path development seed.
 *
 * greppable: linux_module: soft load source=embed|media|mem|finit name=
 * greppable: linux_module: soft load PASS|FAIL source= name= missing=
 * greppable: linux_module: soft init PASS|FAIL
 * greppable: linux_module: soft media path OPEN|SKIP name= reason=
 *
 * Soft class-path lamps (main.c, soft≠product; not freestanding thrash):
 *   main: soft linux_module path PASS|FAIL|SKIP          — r8169 embed
 *   main: soft linux_module xhci path PASS|FAIL|SKIP     — xhci_pci (8086:a12f)
 *   main: soft usb multi-mod order …                     — HC+MSC order stub
 *   main: soft linux_module usb_storage path PASS|FAIL|SKIP — MSC leaf
 *   SKIP builtin = host had no .ko embed (xhci often builtin; see collect
 *   meta/XHCI-STATUS.txt + meta/USB-STATUS.txt). xhci PRESENT only when
 *   xhci_pci_mod_blob linked; usb_storage when usb_storage_mod_blob linked.
 *
 * Source tags (D4/D5 honesty; Soft≠product):
 *   embed  — linked r8169 / optional xhci_pci / optional usb_storage blobs
 *   media  — GJ-PERSIST/linux-drivers/modules/ (OPEN: no ext4 at boot)
 *   mem    — generic load_mem (syscall bounce default)
 *   finit  — finit_module / init_module cold path
 *
 * Implementation: kernel/mm/linux_module.c
 * Optional peer: gj/linux_ksym.h (linux_ksym_lookup) — weak stub if absent.
 */
#pragma once

#include <gj/types.h>

/** Soft table capacity (slots). */
#define GJ_LINUX_MODULE_MAX 8u

/**
 * One-time bring-up: clear soft module table and diagnostics.
 * Safe to call more than once (idempotent soft reset of empty table only if
 * no live modules; always clears last-unresolved name).
 */
void linux_module_init(void);

/**
 * Load ELF64 ET_REL from a kernel memory buffer (not xz).
 * Source tag for lamps defaults to "mem" (syscall / generic).
 *
 * @param pElf   image bytes
 * @param cb     image size
 * @param szName module name for the soft table (NULL → "anon")
 * @return 0 on success, negative GJ_ERR_* on failure
 *
 * On unresolved SHN_UNDEF (non-weak), records the symbol name for
 * linux_module_last_unresolved() and returns GJ_ERR_NOENT. Does not call init.
 */
i64 linux_module_load_mem(const void *pElf, size_t cb, const char *szName);

/**
 * Same as linux_module_load_mem with an explicit source tag for greppable lamps.
 *
 * @param szSource  "embed" | "media" | "mem" | "finit" | other short tag
 *                  (NULL / empty → "mem")
 */
i64 linux_module_load_mem_src(const void *pElf, size_t cb, const char *szName,
                              const char *szSource);

/**
 * Call init_module for a previously loaded module (if present).
 * @return 0 if no init or init returned 0; negative on missing module / init fail
 */
i64 linux_module_init_call(const char *szName);

/**
 * Call cleanup_module for a previously loaded module (if present).
 * @return 0 if no exit or exit ran; negative if module not found
 */
i64 linux_module_exit_call(const char *szName);

/** Non-zero if @szName is present in the soft loaded table. */
int linux_module_loaded(const char *szName);

/**
 * Soft diagnostic: relocated load image VA range for a named module.
 * Soft≠product; used by netdev ops-range checks (Option B research).
 * @param szName  module name (e.g. "r8169")
 * @param ppBase  out: HHDM VA of load base (may be NULL out-arg)
 * @param pcb     out: load byte length (may be NULL out-arg)
 * @return 0 if found with non-zero base/size, -1 otherwise
 */
int linux_module_load_va_range(const char *szName, void **ppBase, u64 *pcb);

/** Number of live soft module slots. */
u32 linux_module_count(void);

/**
 * Last missing SHN_UNDEF symbol name from a failed load (diagnostic).
 * Empty string if none. Pointer is stable until the next failed resolve.
 */
const char *linux_module_last_unresolved(void);
