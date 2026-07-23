/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * 4K map/unmap/protect on top of boot identity map (split 2MiB as needed).
 * Per-process CR3: G-AS-* (DESIGN_SPEC_COMPLETE §2).
 * HHDM (P-MEM-5) unlocks PMM hierarchical free for PA ≥ 4 GiB (768G soak).
 *
 * Greppable serial markers (kernel/mm/vmm.c):
 *   vmm: HHDM base=
 *   vmm: as_create cr3=
 *   vmm: as_destroy leaf=
 *   vmm: COW break
 *   vmm: map_device_uc
 */
#pragma once

#include <gj/error.h>
#include <gj/types.h>

#define GJ_VMM_PROT_READ  (1u << 0)
#define GJ_VMM_PROT_WRITE (1u << 1)
#define GJ_VMM_PROT_EXEC  (1u << 2)
#define GJ_VMM_PROT_USER  (1u << 3)

void vmm_init(void);

/**
 * Map physical [0, paMax) at GJ_HHDM_BASE with 2 MiB pages (P-MEM-5).
 * Must run on kernel CR3 after vmm_init. Enables pmm_release_high so the
 * hierarchical freelist can list PA ≥ 4 GiB (large-RAM / 768G path).
 * Prints greppable `vmm: HHDM base=`.
 */
gj_status_t vmm_hhdm_init(u64 paMax);

/** Phys → kernel VA (HHDM if ready, else identity for low mem). */
gj_vaddr_t hhdm_to_virt(gj_paddr_t pa);
gj_paddr_t hhdm_to_phys(gj_vaddr_t va);
int        hhdm_ready(void);

/**
 * Create a new address space: allocate PML4, share kernel half of template,
 * empty user half (G-AS-1, G-AS-2). Returns CR3 physical address or 0.
 */
u64 vmm_as_create(void);

/** Destroy a private AS (frees user page tables + PML4; not the boot AS). */
gj_status_t vmm_as_destroy(u64 u64Cr3);

/**
 * Clone private non-identity user 4K pages from src CR3 into dst CR3.
 * Skips kernel-shared table subtrees and identity (PA==VA) leftovers.
 * Caps copies at u32Max (0 → default 256). Writes count to *pCopied if set.
 */
gj_status_t vmm_as_clone_user_pages(u64 u64SrcCr3, u64 u64DstCr3, u32 u32Max,
                                    u32 *pCopied);

/**
 * Break a COW leaf under the *active* CR3 (write-fault path).
 * Returns GJ_OK if broken; GJ_ERR_NOENT if not a COW page.
 */
gj_status_t vmm_cow_break_page(gj_vaddr_t va);

/** CR3 of the boot/kernel template address space. */
u64 vmm_kernel_cr3(void);

/**
 * Map/unmap/protect operate on the *active* CR3 (current CPU).
 * Callers must switch AS first when targeting a non-current process.
 */
gj_status_t vmm_map_page(gj_vaddr_t va, gj_paddr_t pa, u32 u32Prot);

gj_status_t vmm_unmap_page(gj_vaddr_t va);

/** Change prot on existing mapping. */
gj_status_t vmm_protect_page(gj_vaddr_t va, u32 u32Prot);

/**
 * Invalidate VA on all online CPUs (local invlpg + IPI shootdown).
 * Call after PTE clear/change when multi-CPU may have cached the mapping.
 */
void vmm_tlb_flush_page(gj_vaddr_t va);

/** Translate VA -> PA under active CR3; 0 if not present. */
gj_paddr_t vmm_virt_to_phys(gj_vaddr_t va);

/** Allocate anon pages and map them; returns VA or 0. */
gj_vaddr_t vmm_mmap_anon(u64 u64Hint, size_t cbLen, u32 u32Prot, int fFixed);

gj_status_t vmm_munmap(gj_vaddr_t va, size_t cbLen);

/**
 * Per-process anon cursor (VA next). When pCursor is NULL, uses global.
 * Used so each AS has independent mmap placement.
 */
void vmm_set_anon_cursor(u64 *pCursor);
u64 *vmm_get_anon_cursor(void);

/** Raw PTE value for VA under active CR3 (0 if walk fails). */
u64 vmm_read_pte(gj_vaddr_t va);

/**
 * Force identity R/W (kernel, non-user) mappings for [va, va+cb).
 * Repairs both the kernel template CR3 and the active CR3 when they differ.
 * Use before kernel BSS/image stores when a private AS may have left leaves RO.
 */
gj_status_t vmm_ensure_identity_rw(gj_vaddr_t va, size_t cb);

/**
 * Map device MMIO [pa, pa+cb) at identity VA=pa (kernel CR3 / template).
 * Needed when firmware places PCI BARs above boot identity/HHDM RAM range.
 * Rejects ranges that collide with kernel image/BSS.
 */
gj_status_t vmm_map_device(gj_paddr_t pa, u64 cb);

/**
 * Map device MMIO into the dedicated high UC window (GJ_DEVICE_MMIO_BASE+PA).
 * Safe for T1 soft CAP when BAR is in low physical space. Writes *pVaOut.
 */
gj_status_t vmm_map_device_uc(gj_paddr_t pa, u64 cb, gj_vaddr_t *pVaOut);
