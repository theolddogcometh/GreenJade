/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Virtual memory manager — 4K map/unmap/protect on top of boot identity,
 * per-process CR3 (G-AS-*), HHDM (P-MEM-5), COW fork leaves, device UC.
 *
 * Pure C11 freestanding. Dual license: MIT OR Apache-2.0.
 *
 * Scope
 * -----
 * This header is the kernel-facing VMM surface (x86_64 product path in
 * kernel/mm/vmm.c). It installs and walks PTEs only; frames are owned by
 * PMM / memobj (G-MO-*). User product maps go through memobj so G-MAP-2
 * (USER bit + user VA band) stays single-pathed.
 *
 * Design anchors
 * --------------
 *   docs/DESIGN_SPEC_COMPLETE.md  G-AS-1..5, G-MAP-*, G-MO-4 debug-only
 *   docs/X86_64_INTEL_PLATFORM.md P-MEM-1/4/5 (4-level, 2MiB HHDM, HHDM)
 *   docs/IMPLEMENTATION.md        HHDM base, identity bring-up
 *   docs/SECURITY_CORE_DESIGN.md  fail-closed; no user-mapped kernel data
 *
 * Geometry (see gj/config.h)
 * --------------------------
 *   GJ_PAGE_SIZE          4 KiB leaves (map/unmap unit)
 *   GJ_HHDM_BASE          phys 0 → higher-half direct map
 *   GJ_DEVICE_MMIO_BASE   UC window: VA = BASE + PA (first 512 GiB PA)
 *   Boot identity         low PA==VA until HHDM / private AS
 *
 * Layering
 * --------
 *   pmm / pmm_core   — physical frames
 *   vmm (this)       — page tables, AS create/clone/destroy, COW, HHDM
 *   memobj           — object + region views; calls vmm_map_page
 *   fault            — pager cookie path; may call COW / map later
 *   smep             — clear U on kernel maps; enable CR4.SMEP/SMAP
 *   user_access      — range + present/U/(W|COW) before copy
 *
 * Soft product surface
 * --------------------
 *   VMM_HHDM           — 2 MiB HHDM of [0, paMax); unlocks pmm_release_high
 *   VMM_AS_CREATE      — private PML4; kernel half shared (G-AS-1/2)
 *   VMM_AS_DESTROY     — free user tables + private leaves; not boot AS
 *   VMM_COW            — share RO+COW; break on write fault
 *   VMM_MAP_DEVICE_UC  — high UC BAR window for T1 soft CAP
 *   VMM_ENSURE_ID_RW   — repair RO identity leaves before BSS stores
 *
 * Active-CR3 rule (normative for map/unmap/protect/translate)
 * -----------------------------------------------------------
 * Map/unmap/protect/read_pte/virt_to_phys operate on the *active* CR3 of
 * the current CPU. Callers targeting another process must activate that
 * AS first (process_as_activate) or use create/clone helpers that take
 * explicit CR3 arguments.
 *
 * Greppable serial markers (kernel/mm/vmm.c)
 * ------------------------------------------
 *   vmm: HHDM base=
 *   vmm: as_create cr3= … live= total= PASS
 *   vmm: as_destroy leaf= … priv= cow_drop= tables= … PASS
 *   vmm: COW break … free_old|PASS  (also live=/frees=)
 *   vmm: as_clone_user … cow= rocopy= cow_live=
 *   vmm: map_device_uc … pages= soft PASS
 *   vmm: ensure_identity_rw … fixed= dual= soft PASS
 *
 * greppable: VMM_HHDM VMM_AS_CREATE VMM_AS_DESTROY VMM_COW G-AS
 * greppable: VMM_MAP_DEVICE_UC VMM_ENSURE_ID_RW P-MEM-5 G-MAP
 */
#pragma once

#include <gj/error.h>
#include <gj/types.h>

/* ---- Protection bits (leaf PTE intent) ---------------------------------- */
/*
 * Software-facing mask passed to map/protect/mmap. Implementation folds these
 * into arch PTE bits (R/W, NX, U/S). Product user maps must include
 * GJ_VMM_PROT_USER (memobj forces this — G-MAP-2). W|X without CapJit is a
 * higher-layer policy (P-MEM-6 / personality); VMM may still install bits.
 */
#define GJ_VMM_PROT_READ  (1u << 0)
#define GJ_VMM_PROT_WRITE (1u << 1)
#define GJ_VMM_PROT_EXEC  (1u << 2)
#define GJ_VMM_PROT_USER  (1u << 3)

/**
 * Early VMM bring-up after boot identity map exists.
 * Idempotent soft: may establish template walk helpers; does not create HHDM.
 * Call before vmm_hhdm_init / product as_create.
 */
void vmm_init(void);

/**
 * Map physical [0, paMax) at GJ_HHDM_BASE with 2 MiB pages (P-MEM-5).
 *
 * Preconditions:
 *   - vmm_init done; runs on kernel/template CR3
 *   - paMax should cover max RAM (pmm_max_phys or 4 GiB floor)
 *
 * Postconditions:
 *   - hhdm_ready() non-zero on success
 *   - Enables pmm_release_high so hierarchical freelist can list PA ≥ 4 GiB
 *     (large-RAM / 768G / 1 TiB design path)
 *
 * Prints greppable `vmm: HHDM base=`.
 * Returns GJ_OK or GJ_ERR_* (NOMEM / INVAL) on failure.
 */
gj_status_t vmm_hhdm_init(u64 paMax);

/**
 * Phys → kernel VA: HHDM if ready, else identity for low mem.
 * Not for device MMIO outside RAM (use vmm_map_device / map_device_uc).
 */
gj_vaddr_t hhdm_to_virt(gj_paddr_t pa);

/**
 * Inverse of hhdm_to_virt for HHDM VAs; identity for low VA when HHDM off.
 * Caller must not feed arbitrary user VAs (use vmm_virt_to_phys under AS).
 */
gj_paddr_t hhdm_to_phys(gj_vaddr_t va);

/** Non-zero after successful vmm_hhdm_init. Soft query for PMM high release. */
int hhdm_ready(void);

/**
 * Create a new address space: allocate PML4, share kernel half of template,
 * empty user half (G-AS-1, G-AS-2).
 *
 * Returns CR3 physical address, or 0 on alloc failure.
 * Does not switch the CPU; caller stores into process.u64Cr3 and activates.
 * Logs greppable `vmm: as_create cr3=` with live/total counters.
 */
u64 vmm_as_create(void);

/**
 * Destroy a private AS (frees user page tables + private/COW-drop leaves).
 *
 * Never pass the boot/kernel template CR3 (vmm_kernel_cr3).
 * Always logs greppable `vmm: as_destroy leaf=` (leaf may be 0).
 * cow_drop= / priv= count frame reclaim; tables= freed intermediate PTs.
 * Returns GJ_OK, or GJ_ERR_INVAL for kernel/boot CR3 / null.
 */
gj_status_t vmm_as_destroy(u64 u64Cr3);

/**
 * Clone private non-identity user 4K pages from src CR3 into dst CR3.
 *
 * Skips kernel-shared table subtrees and identity (PA==VA) leftovers.
 * Prefer true COW (RO + software COW bit) when writable; RO copy otherwise.
 * Caps copies at u32Max (0 → default 256). Writes count to *pCopied if set.
 *
 * Logs cow= vs rocopy= share counts and cow_live=.
 * Returns GJ_OK or GJ_ERR_* (NOMEM when frames/tables exhaust).
 */
gj_status_t vmm_as_clone_user_pages(u64 u64SrcCr3, u64 u64DstCr3, u32 u32Max,
                                    u32 *pCopied);

/**
 * Break a COW leaf under the *active* CR3 (write-fault path).
 *
 * Allocates a private frame, copies content, installs writable non-COW PTE,
 * drops COW refcount (may free_old shared frame).
 * Returns GJ_OK if broken; GJ_ERR_NOENT if not a COW page; NOMEM on alloc.
 * Logs greppable `vmm: COW break` with free_old / rem / live.
 */
gj_status_t vmm_cow_break_page(gj_vaddr_t va);

/** CR3 of the boot/kernel template address space (never as_destroy this). */
u64 vmm_kernel_cr3(void);

/**
 * Map one 4K page under *active* CR3: VA → PA with u32Prot.
 * May split boot 2 MiB leaves as needed. TLB maintenance is caller/impl.
 * Returns GJ_OK or GJ_ERR_*.
 */
gj_status_t vmm_map_page(gj_vaddr_t va, gj_paddr_t pa, u32 u32Prot);

/**
 * Unmap one 4K VA under *active* CR3 (clear present; may free intermediate
 * empties soft). Does not free the physical frame — owner (memobj/PMM) does.
 */
gj_status_t vmm_unmap_page(gj_vaddr_t va);

/**
 * Change prot on an existing present mapping under *active* CR3.
 * Does not allocate frames. Fail closed if leaf missing (GJ_ERR_NOENT).
 */
gj_status_t vmm_protect_page(gj_vaddr_t va, u32 u32Prot);

/**
 * Invalidate VA on all online CPUs (local invlpg + IPI shootdown, G-AS-4).
 * Call after PTE clear/change when multi-CPU may have cached the mapping.
 * UP: local invalidate only.
 */
void vmm_tlb_flush_page(gj_vaddr_t va);

/**
 * Translate VA → PA under *active* CR3; 0 if not present or walk fails.
 * Large-page walks return the containing frame PA + offset soft as page PA.
 */
gj_paddr_t vmm_virt_to_phys(gj_vaddr_t va);

/**
 * Allocate anon frames (PMM) and map them under *active* CR3.
 * u64Hint / fFixed control placement; returns VA base or 0 on failure.
 * Prefer memobj_map_anon for product user maps (G-MO-1, USER, region table).
 */
gj_vaddr_t vmm_mmap_anon(u64 u64Hint, size_t cbLen, u32 u32Prot, int fFixed);

/**
 * Unmap [va, va+cbLen) under *active* CR3 (page-granular).
 * Frame reclaim policy matches map path (anon helper free vs object-owned).
 */
gj_status_t vmm_munmap(gj_vaddr_t va, size_t cbLen);

/**
 * Per-process anon cursor (VA next for non-fixed mmap).
 * When pCursor is NULL, VMM uses a global fallback (bring-up / kernel AS).
 * process_as_* wires process.u64AnonNext so each AS has independent placement.
 */
void vmm_set_anon_cursor(u64 *pCursor);

/** Current anon cursor pointer (may be global or process-owned). */
u64 *vmm_get_anon_cursor(void);

/**
 * Raw PTE value for VA under *active* CR3 (0 if walk fails / not present).
 * Soft COW / present probes (fault coalesce, user_range_mapped_access) use this.
 */
u64 vmm_read_pte(gj_vaddr_t va);

/**
 * Force identity R/W (kernel, non-user) mappings for [va, va+cb).
 *
 * Repairs both the kernel template CR3 and the active CR3 when they differ
 * (private AS may have left identity leaves RO via COW share).
 * Use before kernel BSS/image stores that must not #PF on RO leaves.
 * Soft-path greppable: `vmm: ensure_identity_rw … soft PASS` (always).
 */
gj_status_t vmm_ensure_identity_rw(gj_vaddr_t va, size_t cb);

/**
 * Map device MMIO [pa, pa+cb) at identity VA=pa on kernel CR3 / template.
 *
 * Needed when firmware places PCI BARs above boot identity/HHDM RAM range
 * but drivers still use PA as VA. Rejects ranges that collide with kernel
 * image/BSS. Prefer vmm_map_device_uc when identity would collide with RAM.
 */
gj_status_t vmm_map_device(gj_paddr_t pa, u64 cb);

/**
 * Map device MMIO into the dedicated high UC window (GJ_DEVICE_MMIO_BASE+PA).
 *
 * Safe for T1 soft CAP when BAR is in low physical space. Writes *pVaOut
 * (required non-NULL). Soft-path greppable: `vmm: map_device_uc … soft PASS`
 * (or soft reject). Span limited by GJ_DEVICE_MMIO_SPAN (config).
 */
gj_status_t vmm_map_device_uc(gj_paddr_t pa, u64 cb, gj_vaddr_t *pVaOut);
