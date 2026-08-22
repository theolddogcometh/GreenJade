/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft freestanding loader for Linux x86_64 .ko ELF relocatable objects (ET_REL).
 * Pure C11 freestanding. Dual MIT OR Apache-2.0. No GPL / no Linux source copy.
 *
 * Scope (Soft!=product; G-AC-1)
 * ----------------------------
 * Eng residual path only: load an ELF64 ET_REL image already present in kernel
 * memory (not xz-decompress). SHF_ALLOC sections are copied into PMM pages
 * (HHDM VA), relocations applied, SHN_UNDEF resolved via linux_ksym_lookup.
 * Init/exit symbols are the classic module entry points init_module /
 * cleanup_module. Soft load residual targets r8169 / usb_storage eng only -
 * never product wire ownership. Product drivers = userspace UDX+ABI (hot+cold
 * Linux-shaped hosts). Freestanding class drivers = SKIP (not product).
 *
 * Honesty
 * -------
 * Soft surface only: max 8 modules, limited reloc set, no full Linux module
 * API (modversions, vermagic, livepatch, sysfs). Not G-AC-1 product acceptance
 * for shipping GPL .ko as product AC - staging / ABI-module-path eng seed.
 * Soft residual != product AC. Soft load PASS != wire ownership.
 *
 * G-AC-1 / freestanding belt (default; Soft!=product)
 * --------------------------------------------------
 * GJ_SOFT_MODULE_RUN_INIT defaults 0 -> freestanding_no_exec for ALL staged .ko
 * residual (all_ko_skip_exec=1). GJ_LMOD_NEVER_EXEC_KO=1 under that default:
 * never call init_module / never execute .ko text as product. Load PASS =
 * stage only (stage_only=1; soft load/stage eng residual only). init_call
 * SKIP exec for every .ko residual - reinforced belt: picker always returns
 * freestanding_no_exec (or more specific residual reason) under RUN_INIT=0 /
 * ALL_KO_SKIP / NEVER_EXEC / STAGE_ONLY; hard compile-time belt closes any
 * picker regression so pfnInit is never called. Soft residual != product AC.
 * Soft!=product. Product drivers remain userspace UDX+ABI; freestanding
 * class SKIP.
 *
 * greppable: linux_module: soft load source=embed|media|mem|finit name=
 * greppable: linux_module: soft load PASS|FAIL source= name= missing=
 *   (PASS: init_sym= exit_sym= init_policy=SKIP_EXEC never_exec_ko=1
 *    all_ko_skip_exec=1 stage_only=1 - soft load/stage eng only;
 *    eng residual r8169/usb_storage; wire=0; never product wire ownership;
 *    Soft!=product; != G-AC-1 product AC)
 * greppable: linux_module: soft FAIL KSYM name= missing= kind=FAIL_KSYM
 * greppable: linux_module: soft FAIL KSYM summary name= n= first=
 * greppable: linux_module: soft init PASS|FAIL name= INIT= kind=INIT0_ENG|INIT_FAIL
 * greppable: linux_module: soft SKIP exec name= reason= kind=SKIP_EXEC
 *   (all_ko=1 freestanding_no_exec=1 never_exec_ko=1; Soft!=product)
 * greppable: linux_module: soft multi-mod SKIP exec name= reason=
 * greppable: linux_module: soft residual Dual DoD D name= triad=
 * greppable: residual_reasons=freestanding_no_exec|deps_soft_virtual_only|pfn_outside_module
 * greppable: linux_module: soft residual freestanding_no_exec
 *   (all_ko_skip_exec=1 stage_only=1 RUN_INIT=0 never_exec_ko=1 Soft!=product)
 * greppable: linux_module: soft freestanding SKIP residual name=
 * greppable: linux_module: soft usb_storage SKIP residual name= picked=
 * greppable: linux_module: soft usb_storage SKIP honesty map residual_reasons=
 * greppable: linux_module: soft usb_storage SKIP residual already=1 first_reason=
 * greppable: linux_module: soft usb_storage SKIP exit residual name=
 * greppable: linux_module: soft usb_storage SKIP exit honesty map residual_reasons=
 * greppable: linux_module: soft lamp class name= kind= reason=
 * greppable: linux_module: soft SKIP exit name= reason=
 * greppable: linux_module: soft media path OPEN|SKIP name= reason=
 * greppable: linux_module: soft load order rank= name= deps=
 * greppable: linux_module: soft load order ready|WAIT name= need=
 * greppable: linux_module: soft deps_ready name= via= init_policy=
 * greppable: linux_module: soft dep virtual <canon> soft=1 product=0
 * greppable: linux_module: soft export ksym n= name=
 *
 * Lean residual (G-AC-1 / Soft!=product; once at linux_module_init; denser):
 *   linux_module: soft residual freestanding_no_exec RUN_INIT=0 never_exec_ko=1
 *     all_ko_skip_exec=1 stage_only=1 eng=r8169,usb_storage wire=0
 *     freestanding_class=SKIP product_drivers=userspace_UDX_ABI
 *   RUN_INIT=0 / freestanding_no_exec honesty denser: SKIP exec ALL staged .ko
 *   residual (never execute init in kernel as product). Soft load/stage eng
 *   residual only (r8169/usb_storage) - never product wire ownership. Product
 *   drivers = userspace UDX+ABI; freestanding class SKIP. Soft!=product.
 *   No version stamp; storm=0. Dual MIT/Apache.
 *
 * Lamp honesty triad (FAIL KSYM != SKIP exec != INIT=0 eng) - Dual DoD D residual:
 *   FAIL KSYM  kind=FAIL_KSYM - load-time SHN_UNDEF / need:DEP; no init ran.
 *                Never emit SKIP exec for this.
 *   SKIP exec  kind=SKIP_EXEC - load may PASS; init body intentionally not run.
 *                Reasons (most specific first):
 *                empty_load | already_inited|already_skipped | deps_missing |
 *                deps_soft_virtual_only | no_init_symbol |
 *                pfn_outside_module | freestanding_no_exec
 *                Gap D residual catalog (sparse residual_reasons=):
 *                freestanding_no_exec | deps_soft_virtual_only |
 *                pfn_outside_module (greppable under freestanding default).
 *                policy=freestanding_no_exec under GJ_SOFT_MODULE_RUN_INIT=0
 *                (G-AC-1; never accidental in-kernel .ko init as product).
 *                Sparse residual lamps only - no multi-line residual storms.
 *                Rate-limit re-entry: already=1 single line (no storms).
 *                All .ko first SKIP: soft SKIP exec + freestanding residual +
 *                lamp class (single lines). USB leaf: + multi-mod + Dual DoD.
 *                usb_storage Gap D (product=0; Soft!=product): + residual +
 *                honesty map residual_reasons= (r0/r1/r2; two lines max).
 *                Do NOT put INIT=0 on SKIP lamps (INIT=0 token = eng only).
 *   INIT=0 eng kind=INIT0_ENG - init_module returned 0 (eng opt-in only).
 *                Soft eng only; never under RUN_INIT=0; never USB multi-mod leaf.
 *   Soft!=product on every lamp; staged .ko != G-AC-1 product AC.
 *   Parent may skip .ko init_module; loader lamps stay honest either way.
 *
 * Soft class-path lamps (main.c; Soft!=product; freestanding class SKIP):
 *   main: soft linux_module path PASS|FAIL|SKIP          - r8169 embed eng residual
 *   main: soft linux_module xhci path PASS|FAIL|SKIP     - xhci_pci (8086:a12f)
 *   main: soft usb multi-mod order ...                     - HC+MSC order stub
 *   main: soft linux_module usb_storage path PASS|FAIL|SKIP - MSC leaf eng residual
 *   Soft r8169 / usb_storage load = eng residual only; never product wire
 *   ownership. Product NIC/USB = userspace UDX+ABI (rtl8168_udx / xhci_udx).
 *   SKIP builtin = host had no .ko embed (xhci often builtin; see collect
 *   meta/XHCI-STATUS.txt + meta/USB-STATUS.txt). xhci PRESENT only when
 *   xhci_pci_mod_blob linked; usb_storage when usb_storage_mod_blob linked.
 *
 * Soft dep virtual (honest soft!=real .ko / != product usbcore):
 *   When host ships usbcore/scsi_mod as BUILTIN, soft seeds (linux_usb_soft)
 *   register virtual slots "usbcore-soft" / "scsi_mod-soft". deps_ready maps
 *   soft dep "usbcore" -> satisfied by real loaded("usbcore") OR soft virtual.
 *   linux_module_loaded("usbcore") stays 0 - does not lie about real load.
 *   Grep: linux_module: soft dep virtual usbcore soft=1 product=0
 *   Grep: linux_module: soft deps_ready name=usb_storage via=soft-virtual
 *         init_policy=SKIP_EXEC reason=deps_soft_virtual_only
 *   When deps are soft-virtual only, init_call emits SKIP exec
 *   reason=deps_soft_virtual_only (not INIT PASS / not stick).
 *
 * Source tags (D4/D5 honesty; Soft!=product):
 *   embed  - linked r8169 / optional xhci_pci / optional usb_storage blobs
 *   media  - GJ-PERSIST/linux-drivers/modules/ (OPEN: no ext4 at boot)
 *   mem    - generic load_mem (syscall bounce default)
 *   finit  - finit_module / init_module cold path
 *
 * Implementation: kernel/mm/linux_module.c
 * Optional peer: gj/linux_ksym.h (linux_ksym_lookup) - weak stub if absent.
 */
#pragma once

#include <gj/types.h>

/** Soft table capacity (slots). */
#define GJ_LINUX_MODULE_MAX 8u

/** Soft cap on unique unresolved names reported per failed load. */
#define GJ_LINUX_MODULE_UNRES_MAX 48u

/*
 * Freestanding never-exec belt (G-AC-1; Soft!=product).
 * Default RUN_INIT=0 -> freestanding_no_exec denser: SKIP exec ALL staged
 * .ko residual (all_ko_skip_exec=1). Never execute staged .ko init_module
 * in kernel as product. Soft load/stage eng residual only (stage_only=1;
 * r8169 / usb_storage) - never product wire ownership. Product drivers =
 * userspace UDX+ABI; freestanding class SKIP. Soft!=product. Opt-in eng
 * must deliberately -DGJ_SOFT_MODULE_RUN_INIT=1 and lift the matching
 * _Static_assert set in linux_module.c (RUN_INIT / NEVER_EXEC / ALL_KO /
 * STAGE_ONLY). Reinforced: any one of ALL_KO_SKIP / NEVER_EXEC / STAGE_ONLY /
 * RUN_INIT=0 forces SKIP exec all .ko residual. No version stamp.
 */
#ifndef GJ_SOFT_MODULE_RUN_INIT
#define GJ_SOFT_MODULE_RUN_INIT 0
#endif
#if GJ_SOFT_MODULE_RUN_INIT == 0
#define GJ_LMOD_NEVER_EXEC_KO 1
/** Greppable: all staged .ko SKIP exec under freestanding residual. */
#define GJ_LMOD_ALL_KO_SKIP_EXEC 1
/** Greppable: load PASS stages only (soft load/stage eng residual). */
#define GJ_LMOD_STAGE_ONLY 1
#else
#define GJ_LMOD_NEVER_EXEC_KO 0
#define GJ_LMOD_ALL_KO_SKIP_EXEC 0
#define GJ_LMOD_STAGE_ONLY 0
#endif
/** Greppable freestanding policy token (soft residual; != product AC). */
#define GJ_LMOD_POLICY_FREESTANDING "freestanding_no_exec"
/** Greppable law stamp: SKIP exec all .ko residual (default freestanding). */
#define GJ_LMOD_SKIP_EXEC_ALL_KO_RESIDUAL 1

/**
 * One-time bring-up: clear soft module table and diagnostics.
 * Safe to call more than once (idempotent soft reset of empty table only if
 * no live modules; always clears last-unresolved name).
 * Emits lean residual once (storm=0; no version stamp; denser honesty):
 *   linux_module: soft residual freestanding_no_exec RUN_INIT=0 never_exec_ko=1
 *     all_ko_skip_exec=1 stage_only=1 eng=r8169,usb_storage wire=0
 *     freestanding_class=SKIP product_drivers=userspace_UDX_ABI
 * Soft!=product; G-AC-1; SKIP exec all .ko residual; soft load/stage eng only;
 * never product wire ownership via soft load residual.
 */
void linux_module_init(void);

/**
 * Load ELF64 ET_REL from a kernel memory buffer (not xz).
 * Source tag for lamps defaults to "mem" (syscall / generic).
 *
 * @param pElf   image bytes
 * @param cb     image size
 * @param szName module name for the soft table (NULL -> "anon")
 * @return 0 on success, negative GJ_ERR_* on failure
 *
 * On unresolved SHN_UNDEF (non-weak), records the symbol name(s) for
 * linux_module_last_unresolved() / unresolved_count(), emits greppable
 * FAIL KSYM lines, and returns GJ_ERR_NOENT. Does not call init.
 *
 * Load PASS is stage-only honesty (G-AC-1 / Soft!=product):
 *   soft load PASS ... init_sym= exit_sym= init_policy=SKIP_EXEC
 *   never_exec_ko=1 all_ko_skip_exec=1 stage_only=1 RUN_INIT=0
 *   policy=freestanding_no_exec
 *   eng=1 wire=0 when name is r8169|usb_storage (soft load residual eng only)
 * init_sym=1 means the init_module symbol was bound - NOT that the body ran.
 * Under default freestanding belt, init_call will SKIP exec all .ko residual
 * (never .ko as product). Soft load/stage eng only - never product wire
 * ownership; product drivers = userspace UDX+ABI; freestanding class SKIP.
 */
i64 linux_module_load_mem(const void *pElf, size_t cb, const char *szName);

/**
 * Same as linux_module_load_mem with an explicit source tag for greppable lamps.
 *
 * @param szSource  "embed" | "media" | "mem" | "finit" | other short tag
 *                  (NULL / empty -> "mem")
 */
i64 linux_module_load_mem_src(const void *pElf, size_t cb, const char *szName,
                              const char *szSource);

/**
 * Call init_module for a previously loaded module (if present).
 *
 * Honesty triad (Dual DoD D residual): FAIL KSYM is load-time only.
 * This path may emit sparse residual lamps (single lines; no multi-line storms):
 *   soft SKIP exec name= reason= kind=SKIP_EXEC
 *     - init body not run. Reason priority:
 *       empty_load | already_skipped | deps_missing |
 *       deps_soft_virtual_only | no_init_symbol |
 *       pfn_outside_module | freestanding_no_exec
 *       (USB multi-mod leaf and/or GJ_SOFT_MODULE_RUN_INIT=0 default).
 *       INIT=0 is never placed on SKIP lamps.
 *       policy=freestanding_no_exec tags freestanding never-exec stance.
 *   soft freestanding SKIP residual name= (all .ko under RUN_INIT=0)
 *   soft multi-mod SKIP exec / residual Dual DoD D (USB leaf only)
 *   soft usb_storage SKIP residual + honesty map residual_reasons= (MSC leaf)
 *   soft lamp class name= kind=SKIP_EXEC|INIT0_ENG|INIT_FAIL reason=
 *   soft init PASS name= INIT=0 kind=INIT0_ENG - eng only (RUN_INIT=1); never USB leaf
 *   soft init FAIL name= INIT= kind=INIT_FAIL
 * Soft!=product; SKIP != product PASS; staged .ko != G-AC-1.
 * Re-entry SKIP lamps rate-limited (already=1 single line; no stamp storms).
 * Default GJ_SOFT_MODULE_RUN_INIT=0 - SKIP exec ALL .ko residual (G-AC-1;
 * all_ko_skip_exec=1 never_exec_ko=1). Soft load/stage eng only (stage_only=1).
 * Lean residual once (linux_module_init): freestanding_no_exec never_exec_ko=1
 *   all_ko_skip_exec=1 stage_only=1 eng=r8169,usb_storage wire=0
 *   freestanding_class=SKIP product_drivers=userspace_UDX_ABI
 *   (denser RUN_INIT=0 honesty). Soft residual != product AC.
 * Soft load residual eng only - never product wire ownership.
 * Parent may skip this call entirely - lamps only fire when invoked.
 *
 * Greppable: linux_module: soft SKIP exec name= reason= kind=SKIP_EXEC
 * Greppable: residual_reasons=freestanding_no_exec|deps_soft_virtual_only|pfn_outside_module
 * Greppable: linux_module: soft residual freestanding_no_exec
 * Greppable: linux_module: soft freestanding SKIP residual name=
 * Greppable: linux_module: soft usb_storage SKIP residual name= picked=
 * Greppable: linux_module: soft usb_storage SKIP honesty map residual_reasons=
 * Greppable: linux_module: soft multi-mod SKIP exec name= reason=
 * Greppable: linux_module: soft residual Dual DoD D name= triad=
 * Greppable: linux_module: soft lamp class name= kind= reason=
 * Greppable: linux_module: soft init PASS|FAIL name= INIT=
 * @return 0 on SKIP exec / no init / init returned 0; negative on missing
 *         module / init fail
 */
i64 linux_module_init_call(const char *szName);

/**
 * Call cleanup_module for a previously loaded module (if present).
 * Only runs cleanup_module when init_module body actually ran (u8InitRan).
 * After intentional SKIP exec, emits soft SKIP exit (does not touch .ko text).
 * greppable: linux_module: soft SKIP exit name= reason=
 * greppable: linux_module: soft usb_storage SKIP exit residual name=
 *   - Gap D residual exit honesty for MSC leaf (product=0; Soft!=product).
 *     first_reason= policy= residual_reasons= greppable; cleanup not run.
 * greppable: linux_module: soft usb_storage SKIP exit honesty map residual_reasons=
 *   - exit catalog expand r0/r1/r2 (once; product=0; sparse).
 * @return 0 if no exit / exit ran / SKIP exit; negative if module not found
 */
i64 linux_module_exit_call(const char *szName);

/** Non-zero if @szName is present in the soft loaded table. */
int linux_module_loaded(const char *szName);

/**
 * Soft diagnostic: relocated load image VA range for a named module.
 * Soft!=product; used by netdev ops-range checks (Option B research).
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
 * First entry when multiple unresolved were collected.
 */
const char *linux_module_last_unresolved(void);

/**
 * Number of unique strong SHN_UNDEF misses from the last failed load
 * (capped at GJ_LINUX_MODULE_UNRES_MAX). 0 if last load passed / cleared.
 */
u32 linux_module_unresolved_count(void);

/**
 * Soft load-order rank for known class modules (lower loads earlier).
 * Unknown names return a high rank (1000). Soft!=product; not full dep graph.
 *
 * Known ranks (examples):
 *   usb_common=10, usbcore=20, scsi_mod=25, xhci_hcd=30, xhci_pci=40,
 *   usb_storage=50, libphy=10, mdio_devres=15, r8169=50
 */
int linux_module_load_order_rank(const char *szName);

/**
 * Soft comma-separated dependency list for a known module name.
 * Empty string if unknown / no soft deps recorded.
 * Pointer is a static string (stable). Soft!=product.
 */
const char *linux_module_soft_deps(const char *szName);

/**
 * Soft dependency readiness for @szName.
 * Checks soft deps against linux_module_loaded() OR soft dep virtual markers
 * (e.g. "usbcore-soft" satisfying dep "usbcore" when linux_usb_soft seed is
 * present). Missing real+virtual -> WAIT + FAIL KSYM need:DEP (kind=FAIL_KSYM).
 * Ready via soft-virtual -> greppable foreshadow:
 *   soft deps_ready name= via=soft-virtual init_policy=SKIP_EXEC
 *   reason=deps_soft_virtual_only
 * (init_call will SKIP exec; never INIT PASS / product usbcore).
 *
 * @return 0 if ready (or no soft deps known); -1 if a soft dep is missing
 *         (still does not block load - caller may use as pre-check).
 * Soft!=product: virtual PASS is eng-lamp only, not real usbcore.ko.
 */
int linux_module_deps_ready(const char *szName);

/**
 * Register a soft-virtual "loaded" marker for a canonical dep name.
 * Creates honest virtual slot name "<canon>-soft" (e.g. usbcore -> usbcore-soft).
 * Does NOT set linux_module_loaded(canon) - real table stays honest.
 * Grep: linux_module: soft dep virtual <canon> soft=1 product=0
 *
 * @param szCanon  canonical soft dep name ("usbcore", "scsi_mod", ...)
 * @return 0 on success / already present; -1 on full / bad name
 */
int linux_module_soft_dep_virtual_register(const char *szCanon);

/**
 * Non-zero if a soft-virtual marker exists for @szCanon (not real .ko load).
 * Soft!=product.
 */
int linux_module_soft_dep_virtual_present(const char *szCanon);

/**
 * Emit greppable soft load-order plan for a class.
 * @param szClass  "usb" | "net" | other (default: both usb+net)
 * greppable: linux_module: soft load order rank= name= deps=
 */
void linux_module_soft_order_log(const char *szClass);
