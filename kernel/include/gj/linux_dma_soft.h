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
 * pmm-backed coherent pages + identity VA=PA for the first 4 GiB; MMIO is
 * identity cast + volatile access; pci_iomap uses soft BAR helpers with a
 * gated hostish Strategy A fallback (linux_pci_hostish_off.h).
 *
 * Soft != product
 * --------------
 * No VT-d window grant, no full struct device / struct page ABI, no claim of
 * binary Linux DMA API completeness. Soft firmware is embed-table only
 * (rtl_nic/rtl8168*.fw via scripts/embed-linux-fw.sh). Cap total soft DMA
 * pages (see LINUX_DMA_SOFT_PAGE_MAX). Soft modules / ksym resolve are the
 * consumers. Shape mirrors userspace UDX dma_* (alloc/map/mapping_error)
 * for later hot/cold ABI hostability - eng residual only.
 *
 * Residual lean (UDX / userspace driver DMA path eng):
 * Soft dma_alloc_coherent / dma_free_coherent, dma_map_single /
 * dma_unmap_single, dma_mapping_error, dma_sync_single_for_cpu /
 * dma_sync_single_for_device, dma_set_mask bodies track the UDX
 * Linux-shaped DMA surface (udx_dma_*) so DDI/UDX DMA caps can host later
 * without freestanding ring thrash. Soft identity cookie != product DMA
 * window cap / VT-d mint (product OPEN). Soft!=product; G-AC-1.
 * Wire-safe residual when freestanding SKIP: while freestanding owns
 * wire (gate0 always; gate1 while rtl8168_ready), soft ioremap/pci_iomap and
 * map of live 10ec:8168 BAR phys SKIP/NOOP - no second VA, never MMIO
 * as DMA cookie. ksym register SKIP still keeps wire-safe residual lamps.
 * layout_ver>=3: force32 geometric + window_ok + VT-d identity notes peer
 * udx_dma_window_ok / udx_dma_is_low / udx_dma_in_vtd_identity; residual
 * api honesty once-lamp; product iommu_grant OPEN honesty.
 * Grep: linux_dma_soft: soft residual lean PASS
 * Grep: linux_dma_soft: soft residual lean UDX eng
 * Grep: linux_dma_soft: soft residual api honesty
 * Grep: linux_dma_soft: soft freestanding SKIP wire_safe
 *
 * Hybrid SOFT zero-touch (gate0 residual companion to linux_pci_soft):
 * Soft ioremap / pci_iomap must NOOP live 10ec:8168 BAR while freestanding
 * owns wire (no second VA / dual-drive). Soft!=product; G-AC-1.
 * pci_iomap NOOP honesty: reason=hw_touch|vid_dual|bar_phys (once lamp).
 *
 * Dual-map reclaim residual (R0 companion; Soft!=product; G-AC-1):
 * Soft second VA on freestanding 8168 BAR orphans FS rings -> reclaim
 * conflict / STATUS R0. Soft never dual-maps; if a leave-behind iomap
 * slot ever covers live BAR phys under hybrid refuse, scrub it (no
 * freestanding unmap). pci_iomap_range / pcim_iomap_regions|table /
 * pci_ioremap_bar route the same NOOP. Soft!=product - eng residual only.
 *
 * H4 residual (assurance hazard H4 - soft BAR thrash -> freestanding R0):
 * Soft iomap/ioremap of live 10ec:8168 BAR while freestanding owns wire
 * thrashs rings -> RX dead (STATUS R0). Permanent rule: no second VA /
 * no dual-map under hybrid refuse; scrub leave-behind; map_* never
 * treats BAR MMIO as DMA cookie. Soft!=product; G-AC-1. Not product AC.
 * Grep: linux_dma_soft: soft H4 residual
 *
 * Map residual (this pass):
 *   - dma_mapping_error / dma_max_mapping_size / dmam_alloc_attrs soft bodies
 *     replace empty ksym stubs (map-ish + managed alloc).
 *   - dma_map_* refuses live 8168 BAR phys as a buffer cookie (never MMIO
 *     as DMA); size > MAP_MAX refuse. Soft!=product.
 *   - dma_sync_single_for_{cpu,device} soft aliases of __dma_sync_* (ksym).
 *   - Hard-capped logs only (once / miss-cap); no stamp storms.
 *
 * Greppable markers (keep stable):
 *   linux_dma_soft: soft init PASS n=
 *   linux_dma_soft: soft ksym register PASS|SKIP
 *   linux_dma_soft: soft firmware HIT name=
 *   linux_dma_soft: soft firmware MISS name=
 *   linux_dma_soft: soft memcpy_fromio ...
 *   linux_dma_soft: soft pci_iomap NOOP hybrid
 *   linux_dma_soft: soft ioremap NOOP hybrid
 *   linux_dma_soft: soft hybrid zero-touch PASS
 *   linux_dma_soft: soft dual-map reclaim scrub
 *   linux_dma_soft: soft dual-map reclaim residual
 *   linux_dma_soft: soft H4 residual
 *   linux_dma_soft: soft residual lean PASS
 *   linux_dma_soft: soft residual lean UDX eng
 *   linux_dma_soft: soft residual api honesty
 *   linux_dma_soft: soft freestanding SKIP wire_safe
 *   linux_dma_soft: soft force32 residual
 *   linux_dma_soft: soft window residual
 *   linux_dma_soft: soft map residual ...
 *   linux_dma_soft: soft dma_mapping_error ...
 *   linux_dma_soft: soft dma_max_mapping_size ...
 *   linux_dma_soft: soft dmam_alloc_attrs ...
 *   linux_dma_soft: soft dma_sync ...
 *
 * See docs/LINUX_MODULE_PATH.md, docs/LAPTOP_LINUX_DRIVER_HOST.md,
 * docs/R8169_MMIO_HANDOFF.md, docs/UDX_LINUX_PORTER.md.
 */
#pragma once

#include <gj/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Soft coherent DMA page budget (4 KiB frames). */
#define LINUX_DMA_SOFT_PAGE_MAX  256u

/** Soft tracking slots for coherent allocs. */
#define LINUX_DMA_SOFT_SLOT_MAX  64u

/** Soft ioremap tracking slots. */
#define LINUX_DMA_SOFT_IOMAP_MAX 32u

/** Soft kmalloc_trace tracking slots (pmm path). */
#define LINUX_DMA_SOFT_KMALLOC_MAX 128u

/** Soft streaming map bookkeeping slots (map_single / map_page residual). */
#define LINUX_DMA_SOFT_MAP_SLOT_MAX 64u

/** Identity soft ceiling: PA < 4 GiB -> CPU VA = (void *)(uintptr_t)pa. */
#define LINUX_DMA_SOFT_ID_MAX  (0x100000000ull)

/**
 * Soft low-zone ceiling (alias of ID_MAX). Peers UDX_DMA_LOW_MAX /
 * GJ_DMA_LOW_MAX for force32 masters residual. Soft!=product.
 */
#define LINUX_DMA_SOFT_LOW_MAX  LINUX_DMA_SOFT_ID_MAX

/**
 * Bring-up VT-d identity cover end (1 GiB). Peers UDX_DMA_VTD_IDENTITY_LIMIT /
 * GJ_DMA_VTD_IDENTITY_LIMIT. Soft geometric only - no live VT-d program.
 * Soft residual prefers coherent PA in this zone for force32 UDX hosts.
 * Soft!=product; G-AC-1.
 */
#define LINUX_DMA_SOFT_VTD_IDENTITY_LIMIT  (1024ull * 1024ull * 1024ull)

/*
 * Soft residual lean layout version (eng only; Soft!=host ABI; not stamp).
 * Bump when UDX-shaped soft DMA surface grows.
 * Grep: linux_dma_soft: soft residual lean PASS layout_ver=
 * ver1: map residual + hybrid SKIP wire-safe + UDX shape honesty.
 * ver2: UDX eng deepen - free/unmap in shape catalog; product DMA window
 *       cap OPEN honesty; soft residual lean UDX eng once-lamp; full
 *       udx_dma crosswalk (alloc_coherent free_coherent map_single
 *       unmap_single mapping_error sync set_mask).
 * ver3: force32 geometric + window_ok + VT-d identity prefer for coherent
 *       alloc; residual api honesty once-lamp; iommu_grant OPEN honesty;
 *       max_mapping_size / set_mask_and_coherent in UDX path catalog;
 *       soft residual lean UDX eng deepen (peers udx: dma soft residual lean).
 */
#define LINUX_DMA_SOFT_LAYOUT_VER  3u

/**
 * Soft single-map size ceiling (identity window honesty).
 * Empty ksym stub returned 0 (breaks .ko map sizing); soft reports ID ceiling.
 * Soft!=product IOMMU limit. Peers UDX_DMA_MAP_MAX.
 */
#define LINUX_DMA_SOFT_MAP_MAX  LINUX_DMA_SOFT_ID_MAX

/** Hard-cap for soft firmware MISS / HIT log spam. */
#define LINUX_DMA_SOFT_FW_LOG_MAX  16u

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

/**
 * Soft managed coherent alloc (dmam_*). No full devres auto-free table -
 * same pmm path as dma_alloc_attrs. Soft!=product managed lifetime.
 * Replaces empty ksym dmam_alloc_attrs stub.
 * Grep: linux_dma_soft: soft dmam_alloc_attrs
 */
void *dmam_alloc_attrs(void *dev, unsigned long size, u64 *pDma, unsigned gfp,
                       unsigned long attrs);
/** Soft managed free: alias dma_free_attrs (no devres table). */
void  dmam_free_coherent(void *dev, unsigned long size, void *cpu, u64 dma);

u64   dma_map_page_attrs(void *dev, void *page, unsigned long offset,
                         unsigned long size, int dir, unsigned long attrs);
void  dma_unmap_page_attrs(void *dev, u64 dma, unsigned long size, int dir,
                           unsigned long attrs);

/**
 * Soft identity map of a CPU pointer (alias path for map_single).
 * Residual: refuses live 10ec:8168 BAR phys as buffer cookie (never MMIO
 * as DMA); size > LINUX_DMA_SOFT_MAP_MAX refuse. Cookie 0 -> mapping_error.
 * Soft!=product.
 */
u64   dma_map_single(void *dev, void *cpu, unsigned long size, int dir);
void  dma_unmap_single(void *dev, u64 dma, unsigned long size, int dir);

/**
 * Soft dma_mapping_error: non-zero means map failed (cookie == 0).
 * Mirrors udx_dma_mapping_error; useful for userspace DMA story later.
 * Replaces empty / missing ksym. Soft!=product.
 * Grep: linux_dma_soft: soft dma_mapping_error
 */
int   dma_mapping_error(void *dev, u64 dma);

/**
 * Soft dma_max_mapping_size: identity soft ceiling (LINUX_DMA_SOFT_MAP_MAX).
 * Empty ksym stub returned 0 (hostile to .ko sizing). Soft!=product.
 * Grep: linux_dma_soft: soft dma_max_mapping_size
 */
unsigned long dma_max_mapping_size(void *dev);

void  __dma_sync_single_for_cpu(void *dev, u64 addr, unsigned long size,
                                int dir);
void  __dma_sync_single_for_device(void *dev, u64 addr, unsigned long size,
                                   int dir);
/**
 * Soft residual: non-__ ksym names (alias __dma_sync_*). Empty stubs
 * returned 0 without barrier. Soft!=product.
 * Grep: linux_dma_soft: soft dma_sync
 */
void  dma_sync_single_for_cpu(void *dev, u64 addr, unsigned long size, int dir);
void  dma_sync_single_for_device(void *dev, u64 addr, unsigned long size,
                                 int dir);

int   dma_set_mask(void *dev, u64 mask);
int   dma_set_coherent_mask(void *dev, u64 mask);
/** Soft: set both masks (dma_set_mask then coherent). */
int   dma_set_mask_and_coherent(void *dev, u64 mask);

/**
 * Soft ioremap: identity VA=PA when allowed. Hybrid residual: returns NULL
 * when phys is the live 10ec:8168 BAR (devmgr inventory) and freestanding
 * owns wire (gate0 always; gate1 while rtl8168_ready). Soft!=product.
 * Grep: linux_dma_soft: soft ioremap NOOP hybrid
 */
void *ioremap(u64 phys, unsigned long size);
void *ioremap_wc(u64 phys, unsigned long size);
void *ioremap_noprofile(u64 phys, unsigned long size); /* alias soft */
void *ioremap_uc(u64 phys, unsigned long size);        /* alias soft H4 */
void *ioremap_cache(u64 phys, unsigned long size);     /* alias soft H4 */
void  iounmap(void *addr);

/**
 * Soft pci_iomap: BAR phys/len via soft pci_resource_* (or gated hostish
 * resource[] offsets), then ioremap. Soft!=product MMIO map.
 * Hybrid: returns NULL for 10ec:8168 while freestanding owns wire.
 * NOOP honesty reasons (once lamp reason=...):
 *   hw_touch  - linux_pci_soft_hw_touch_ok==0 (primary zero-touch)
 *   vid_dual  - hybrid refused + soft/hostish VID 10ec:8168
 *   bar_phys  - live 8168 MEM BAR start after resource lookup
 * Gate0 always / gate1 while ready. No second VA on freestanding BAR
 * (dual-map -> reclaim conflict / R0). Soft!=product.
 * Grep: linux_dma_soft: soft pci_iomap NOOP hybrid
 */
void *pci_iomap(void *dev, int nBar, unsigned long ulMaxLen);
void  pci_iounmap(void *dev, void *pAddr);

/**
 * Soft pci_iomap_range: same hybrid dual-map refuse as pci_iomap, then
 * identity map at BAR start+offset. Soft!=product; G-AC-1.
 */
void *pci_iomap_range(void *dev, int nBar, unsigned long ulOff,
                      unsigned long ulMaxLen);

/**
 * Soft pci_ioremap_bar: full-BAR ioremap via soft BAR start/len.
 * H4 residual: same hybrid dual-map refuse as pci_iomap (no second VA on
 * freestanding 8168 BAR). Soft!=product; G-AC-1.
 * Grep: linux_dma_soft: soft pci_iomap NOOP hybrid (shared refuse path)
 */
void *pci_ioremap_bar(void *dev, int nBar);

/** Soft managed PCI enable / iomap / MWI (re-register over ksym stubs). */
int   pcim_enable_device(void *dev);
void *pcim_iomap_region(void *dev, int nBar, const char *szName);
/**
 * Soft pcim_iomap_regions: hybrid 8168 -> NULL (no dual-map table).
 * Else soft-try pci_iomap per set bit in mask into a static table.
 * Soft!=product (no full devres).
 */
void *pcim_iomap_regions(void *dev, int nMask, const char *szName);
/**
 * Soft pcim_iomap_table: last soft pcim_iomap_regions table, or NULL.
 * Never exposes freestanding 8168 BAR dual-map. Soft!=product.
 */
void *pcim_iomap_table(void *dev);
int   pcim_set_mwi(void *dev);

u32   readl(const volatile void *addr);
void  writel(u32 val, volatile void *addr);
u16   readw(const volatile void *addr);
void  writew(u16 val, volatile void *addr);
u8    readb(const volatile void *addr);
void  writeb(u8 val, volatile void *addr);

/**
 * Soft memcpy_fromio: byte-copy from volatile MMIO/IO VA into CPU buffer.
 * Empty ksym stub returned 0 without writing *to - post-probe EEPROM/MAC
 * reads need a real copy. Soft!=product (no ordered MMIO barriers).
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
 * HIT: name matches soft embed table (rtl_nic/rtl8168*.fw) -> *ppFw = soft
 *      {size,data,priv=NULL} over rodata .incbin; return 0.
 * MISS: *ppFw = NULL; return -ENOENT.
 * release_firmware frees soft slot only (blob stays in .rodata).
 * Soft != product. Grep: linux_dma_soft: soft firmware HIT|MISS name=
 */
int  firmware_request_nowarn(void **ppFw, const char *szName, void *pDev);
int  request_firmware(void **ppFw, const char *szName, void *pDev);
int  request_firmware_direct(void **ppFw, const char *szName, void *pDev);
void release_firmware(const void *pFw);

/** Soft diagnostics (map residual / hybrid NOOP / alloc budget). */
u32   linux_dma_soft_pages_used(void);
u32   linux_dma_soft_alloc_ok(void);
u32   linux_dma_soft_map_ok(void);
u32   linux_dma_soft_hybrid_noop(void);

/**
 * Soft dual-map reclaim residual: drop any soft iomap slots whose phys is
 * a live freestanding 10ec:8168 MEM BAR while hybrid map is refused.
 * Also clears pcim table leave-behind identity VAs + map-slot BAR cookies.
 * Soft bookkeeping only - never unmaps freestanding g_pMmio.
 * Returns count of slots scrubbed. Soft!=product; G-AC-1.
 * Grep: linux_dma_soft: soft dual-map reclaim scrub
 */
u32   linux_dma_soft_drop_freestanding_bar_maps(void);

/**
 * Non-zero if soft currently holds a live 8168 BAR iomap slot under hybrid
 * refuse (dual-map risk vs freestanding reclaim). Soft!=product.
 */
int   linux_dma_soft_freestanding_bar_dual_mapped(void);

/**
 * H4 residual: scrub leave-behind, then non-zero if freestanding 8168 BAR
 * has no soft second VA (dual_mapped==0 under hybrid refuse, or map allowed
 * post-quiesce). Soft!=product; G-AC-1. Not product AC / not DoD close.
 * Grep: linux_dma_soft: soft H4 residual
 */
int   linux_dma_soft_h4_no_dual_map_ok(void);

/**
 * Soft residual inventory dump (once; hard-capped). Observation only.
 * Grep: linux_dma_soft: soft map residual
 */
void  linux_dma_soft_map_residual_inventory(void);

/**
 * Once-lamp soft residual lean honesty (UDX/userspace DMA path eng).
 * Lamps wire-safe freestanding SKIP + UDX shape
 * (dma_alloc_coherent dma_free_coherent dma_map_single dma_unmap_single
 * dma_mapping_error dma_sync_single_for_cpu dma_sync_single_for_device
 * dma_set_mask) + product window OPEN + force32/window geometric residual
 * + residual api honesty. Soft!=product; G-AC-1; hard-capped.
 * Returns 1 if lamp fired.
 * Grep: linux_dma_soft: soft residual lean PASS
 * Grep: linux_dma_soft: soft residual lean UDX eng
 * Grep: linux_dma_soft: soft residual api honesty
 * Grep: linux_dma_soft: soft freestanding SKIP wire_safe
 */
int   linux_dma_soft_residual_lean_lamp_once(void);

/**
 * Non-zero if soft is wire-safe vs freestanding 8168 BAR ownership:
 * no soft second VA / no MMIO-as-DMA cookie under hybrid refuse.
 * Soft!=product; eng residual only (not product AC / not Dual DoD close).
 */
int   linux_dma_soft_wire_safe(void);

/**
 * Non-zero when soft UDX-shaped DMA surface is present (ready after init):
 * dma_alloc_coherent / dma_free_coherent, dma_map_single / dma_unmap_single,
 * dma_mapping_error, dma_sync_single_for_cpu / dma_sync_single_for_device,
 * dma_set_mask, geometric window_ok / is_low / in_vtd_identity.
 * Supports DDI/UDX DMA caps later - eng residual only; Soft!=product.
 * layout_ver>=3: force32 geometric + residual api honesty + iommu_grant OPEN.
 */
int   linux_dma_soft_udx_shape_ok(void);

/** Soft residual lean layout version (LINUX_DMA_SOFT_LAYOUT_VER). */
u32   linux_dma_soft_layout_ver(void);

/* ---- UDX geometric residual (Soft!=product; peers udx_dma_*) ------------ */

/**
 * Non-zero if soft DMA cookie is in preferred low zone (cookie < LOW_MAX).
 * Peers udx_dma_is_low. Soft geometric only - no live VT-d.
 * Soft!=product; G-AC-1.
 */
int   linux_dma_soft_is_low(u64 u64Dma);

/**
 * Non-zero if [dma, dma+cb) sits in bring-up VT-d identity [0, 1 GiB).
 * Peers udx_dma_in_vtd_identity. Soft geometric only - no live IOMMU tables.
 * Soft!=product; G-AC-1.
 */
int   linux_dma_soft_in_vtd_identity(u64 u64Dma, unsigned long cb);

/**
 * Soft geometric ok for a DMA window grant note (UDX / DDI residual).
 * fForce32 non-zero: range must sit in VT-d identity [0, 1 GiB).
 * Non-force32: non-zero non-overflowing range only.
 * Peers udx_dma_window_ok / dma_buf_window_ok. Soft!=product: does not mint
 * DMA window caps or call iommu_window_grant. Not product AC.
 * Grep: linux_dma_soft: soft window residual
 * Grep: linux_dma_soft: soft force32 residual
 */
int   linux_dma_soft_window_ok(u64 u64Dma, unsigned long cb, int fForce32);

/**
 * Soft residual: non-zero when UDX eng residual surface is complete after
 * init (shape ok + layout_ver >= 3 geometric + residual lean ready).
 * Soft!=product; eng residual only - not Dual DoD close / not product AC.
 */
int   linux_dma_soft_udx_eng_ok(void);

/**
 * Soft window residual tallies (observation only).
 * window_ok / window_fail / force32 notes.
 */
u32   linux_dma_soft_window_ok_count(void);
u32   linux_dma_soft_window_fail_count(void);
u32   linux_dma_soft_force32_note_count(void);

#ifdef __cplusplus
}
#endif
