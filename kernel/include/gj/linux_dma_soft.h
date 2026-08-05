/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft Linux DMA + MMIO bodies for host-collected modules (e.g. r8169.ko).
 * Clean-room freestanding C11. Dual MIT OR Apache-2.0. No GPL / no Linux source.
 *
 * Purpose
 * -------
 * Real-ish soft replacements for dma_* / ioremap / pci_iomap / pcim_* /
 * kmalloc / kzalloc / kmalloc_trace / kfree exports registered into
 * linux_ksym after starter stubs (linux_ksym_register replace path). Soft:
 * pmm-backed coherent pages + identity VA=PA for the first 4 GiB; MMIO is
 * identity cast + volatile access; pci_iomap uses soft BAR helpers with a
 * gated hostish Strategy A fallback (linux_pci_hostish_off.h).
 *
 * Soft ≠ product
 * --------------
 * No VT-d window grant, no full struct device / struct page ABI, no claim of
 * binary Linux DMA API completeness. Soft firmware is embed-table only
 * (rtl_nic/rtl8168*.fw via scripts/embed-linux-fw.sh). Cap total soft DMA
 * pages (see LINUX_DMA_SOFT_PAGE_MAX). Soft modules / ksym resolve are the
 * consumers.
 *
 * Greppable markers (keep stable):
 *   linux_dma_soft: soft init PASS n=
 *   linux_dma_soft: soft ksym register PASS|SKIP
 *   linux_dma_soft: soft firmware HIT name=
 *   linux_dma_soft: soft firmware MISS name=
 *   linux_dma_soft: soft memcpy_fromio …
 *
 * See docs/LINUX_MODULE_PATH.md · docs/LAPTOP_LINUX_DRIVER_HOST.md.
 */
#pragma once

#include <gj/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Soft coherent DMA page budget (4 KiB frames). */
#define LINUX_DMA_SOFT_PAGE_MAX  256u

/** Soft tracking slots for coherent allocs. */
#define LINUX_DMA_SOFT_SLOT_MAX  64u

/** Soft ioremap tracking slots. */
#define LINUX_DMA_SOFT_IOMAP_MAX 32u

/** Soft kmalloc_trace tracking slots (pmm path). */
#define LINUX_DMA_SOFT_KMALLOC_MAX 128u

/** Identity soft ceiling: PA < 4 GiB → CPU VA = (void *)(uintptr_t)pa. */
#define LINUX_DMA_SOFT_ID_MAX  (0x100000000ull)

/**
 * Soft init: idempotent ready lamp + linux_ksym_register of soft bodies.
 * Weak-call safe when F2 linux_ksym is not linked.
 * Grep: linux_dma_soft: soft init PASS n=
 */
void linux_dma_soft_init(void);

/** Non-zero after successful soft init. */
int  linux_dma_soft_ready(void);

/* ---- Soft bodies with Linux C names (x86_64 SysV; void * soft edges) ---- */

void *dma_alloc_attrs(void *dev, unsigned long size, u64 *pDma, unsigned gfp,
                      unsigned long attrs);
void  dma_free_attrs(void *dev, unsigned long size, void *cpu, u64 dma,
                     unsigned long attrs);

/** Alias soft: dma_alloc_attrs(..., attrs=0). */
void *dma_alloc_coherent(void *dev, unsigned long size, u64 *pDma,
                         unsigned gfp);
void  dma_free_coherent(void *dev, unsigned long size, void *cpu, u64 dma);

u64   dma_map_page_attrs(void *dev, void *page, unsigned long offset,
                         unsigned long size, int dir, unsigned long attrs);
void  dma_unmap_page_attrs(void *dev, u64 dma, unsigned long size, int dir,
                           unsigned long attrs);

/** Soft identity map of a CPU pointer (alias path for map_single). */
u64   dma_map_single(void *dev, void *cpu, unsigned long size, int dir);
void  dma_unmap_single(void *dev, u64 dma, unsigned long size, int dir);

void  __dma_sync_single_for_cpu(void *dev, u64 addr, unsigned long size,
                                int dir);
void  __dma_sync_single_for_device(void *dev, u64 addr, unsigned long size,
                                   int dir);

int   dma_set_mask(void *dev, u64 mask);
int   dma_set_coherent_mask(void *dev, u64 mask);
/** Soft: set both masks (dma_set_mask then coherent). */
int   dma_set_mask_and_coherent(void *dev, u64 mask);

void *ioremap(u64 phys, unsigned long size);
void *ioremap_wc(u64 phys, unsigned long size);
void *ioremap_noprofile(u64 phys, unsigned long size); /* alias soft */
void  iounmap(void *addr);

/**
 * Soft pci_iomap: BAR phys/len via soft pci_resource_* (or gated hostish
 * resource[] offsets), then ioremap. Soft≠product MMIO map.
 */
void *pci_iomap(void *dev, int nBar, unsigned long ulMaxLen);
void  pci_iounmap(void *dev, void *pAddr);

/** Soft managed PCI enable / iomap / MWI (re-register over ksym stubs). */
int   pcim_enable_device(void *dev);
void *pcim_iomap_region(void *dev, int nBar, const char *szName);
int   pcim_set_mwi(void *dev);

u32   readl(const volatile void *addr);
void  writel(u32 val, volatile void *addr);
u16   readw(const volatile void *addr);
void  writew(u16 val, volatile void *addr);
u8    readb(const volatile void *addr);
void  writeb(u8 val, volatile void *addr);

/**
 * Soft memcpy_fromio: byte-copy from volatile MMIO/IO VA into CPU buffer.
 * Empty ksym stub returned 0 without writing *to — post-probe EEPROM/MAC
 * reads need a real copy. Soft≠product (no ordered MMIO barriers).
 */
void  memcpy_fromio(void *pTo, const volatile void *pFrom, unsigned long cb);

/**
 * Soft kmalloc: kheap for small; pmm (+ identity VA) for larger.
 * s is ignored (soft kmem_cache cookie). kmalloc/kzalloc are thin wrappers.
 */
void *kmalloc_trace(void *s, unsigned long size, unsigned gfp);
void *kmalloc(unsigned long size, unsigned gfp);
void *kzalloc(unsigned long size, unsigned gfp);
void  kfree(const void *p);

/*
 * Soft firmware (Linux C names; void ** = const struct firmware ** edge).
 * HIT: name matches soft embed table (rtl_nic/rtl8168*.fw) → *ppFw = soft
 *      {size,data,priv=NULL} over rodata .incbin; return 0.
 * MISS: *ppFw = NULL; return -ENOENT.
 * release_firmware frees soft slot only (blob stays in .rodata).
 * Soft ≠ product. Grep: linux_dma_soft: soft firmware HIT|MISS name=
 */
int  firmware_request_nowarn(void **ppFw, const char *szName, void *pDev);
int  request_firmware(void **ppFw, const char *szName, void *pDev);
int  request_firmware_direct(void **ppFw, const char *szName, void *pDev);
void release_firmware(const void *pFw);

/** Soft diagnostics. */
u32   linux_dma_soft_pages_used(void);
u32   linux_dma_soft_alloc_ok(void);

#ifdef __cplusplus
}
#endif
