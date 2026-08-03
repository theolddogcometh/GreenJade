/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft Linux DMA + MMIO bodies for host r8169.ko-class ksym resolve.
 * Pure C11 freestanding. Dual MIT OR Apache-2.0. No GPL / no Linux .ko source.
 *
 * Soft only:
 *   - linux_dma_soft_init: ready lamp + linux_ksym_register of bodies
 *   - dma_alloc_attrs / free: pmm pages, identity VA=PA when pa < 4 GiB
 *   - dma_map_* / sync: identity cookies; coherent soft (no cache flush)
 *   - ioremap / readl-class: identity cast + volatile MMIO access
 *   - kmalloc_trace / kfree: kheap or pmm soft
 *
 * Soft ≠ product: no VT-d grant, no full struct page/device ABI.
 * Cap: LINUX_DMA_SOFT_PAGE_MAX coherent pages.
 *
 * Greppable markers (keep stable):
 *   linux_dma_soft: soft init PASS n=
 *   linux_dma_soft: soft ksym register PASS|SKIP
 */
#include <gj/config.h>
#include <gj/kheap.h>
#include <gj/klog.h>
#include <gj/linux_dma_soft.h>
#include <gj/pmm.h>
#include <gj/string.h>
#include <gj/types.h>
#include <gj/vmm.h>

/*
 * F2 linux_ksym may be linked later. Weak unresolved → NULL; init skips export.
 * Coordinator may also point ksym at these symbols by C name without this call.
 */
int linux_ksym_register(const char *szName, void *pFn) __attribute__((weak));

/* ---- Soft state --------------------------------------------------------- */

static int g_fReady;
static u32 g_cInitCalls;
static u32 g_cPagesUsed;
static u32 g_cAllocOk;
static u32 g_cAllocFail;
static u32 g_cFree;
static u32 g_cMap;
static u32 g_cUnmap;
static u32 g_cSyncCpu;
static u32 g_cSyncDev;
static u32 g_cIoremap;
static u32 g_cIounmap;
static u32 g_cKmallocOk;
static u32 g_cKfree;

struct ldmas_dma_slot {
    u8         u8Live;
    u8         u8Pad[3];
    u32        cPages;
    gj_paddr_t paBase;
    void      *pCpu;
    u64        u64Dma;
    unsigned long cbSize;
};

struct ldmas_iomap {
    u8    u8Live;
    u8    u8Pad[7];
    u64   u64Phys;
    unsigned long cbSize;
    void *pVa;
};

struct ldmas_kmalloc {
    u8    u8Live;
    u8    u8Kind; /* 0 = kheap, 1 = pmm */
    u8    u8Pad[2];
    u32   cPages;
    void *pCpu;
    gj_paddr_t paBase;
    unsigned long cbSize;
};

static struct ldmas_dma_slot  g_aDma[LINUX_DMA_SOFT_SLOT_MAX];
static struct ldmas_iomap     g_aIo[LINUX_DMA_SOFT_IOMAP_MAX];
static struct ldmas_kmalloc   g_aKm[LINUX_DMA_SOFT_KMALLOC_MAX];

/* ---- Helpers ------------------------------------------------------------ */

static void
ldmas_ksym_one(const char *szName, void *pFn, u32 *pu32Ok, u32 *pu32Skip)
{
    int nSt;

    if (szName == NULL || pFn == NULL) {
        if (pu32Skip != NULL && *pu32Skip < 0xffffffffu) {
            (*pu32Skip)++;
        }
        return;
    }
    if (linux_ksym_register == NULL) {
        if (pu32Skip != NULL && *pu32Skip < 0xffffffffu) {
            (*pu32Skip)++;
        }
        return;
    }
    nSt = linux_ksym_register(szName, pFn);
    if (nSt == 0) {
        if (pu32Ok != NULL && *pu32Ok < 0xffffffffu) {
            (*pu32Ok)++;
        }
    } else {
        if (pu32Skip != NULL && *pu32Skip < 0xffffffffu) {
            (*pu32Skip)++;
        }
    }
}

static void
ldmas_inc(u32 *pu32)
{
    if (pu32 != NULL && *pu32 < 0xffffffffu) {
        (*pu32)++;
    }
}

/**
 * CPU VA for a physical frame: identity when pa < 4 GiB, else HHDM soft.
 */
static void *
ldmas_cpu_va(gj_paddr_t pa)
{
    if (pa == 0) {
        return NULL;
    }
    if ((u64)pa < LINUX_DMA_SOFT_ID_MAX) {
        return (void *)(uintptr_t)pa;
    }
    return (void *)hhdm_to_virt(pa);
}

static u32
ldmas_pages_for(unsigned long cbSize)
{
    unsigned long cb;

    if (cbSize == 0ul) {
        return 0u;
    }
    cb = cbSize + (unsigned long)GJ_PAGE_SIZE - 1ul;
    cb /= (unsigned long)GJ_PAGE_SIZE;
    if (cb > 0xfffffffful) {
        return 0xffffffffu;
    }
    return (u32)cb;
}

static void
ldmas_zero_pages(gj_paddr_t paBase, u32 cPages)
{
    u32 i;
    void *pVa;

    for (i = 0u; i < cPages; i++) {
        pVa = ldmas_cpu_va(paBase + (gj_paddr_t)i * (gj_paddr_t)GJ_PAGE_SIZE);
        if (pVa != NULL) {
            memset(pVa, 0, GJ_PAGE_SIZE);
        }
    }
}

/**
 * Prefer low (identity) frames: hold high briefly and retry freelist.
 */
static gj_paddr_t
ldmas_alloc_pages_low(u32 cPages)
{
    gj_paddr_t aHold[16];
    u32 cHold = 0u;
    gj_paddr_t pa = 0;
    u32 i;

    if (cPages == 0u) {
        return 0;
    }

    for (i = 0u; i < 16u; i++) {
        if (cPages == 1u) {
            pa = pmm_alloc();
        } else {
            pa = pmm_alloc_pages(cPages);
        }
        if (pa == 0) {
            break;
        }
        if ((u64)pa < LINUX_DMA_SOFT_ID_MAX) {
            /* Free held high pages. */
            while (cHold > 0u) {
                cHold--;
                if (cPages == 1u) {
                    pmm_free(aHold[cHold]);
                } else {
                    pmm_free_pages(aHold[cHold], cPages);
                }
            }
            return pa;
        }
        if (cHold < 16u) {
            aHold[cHold++] = pa;
        } else {
            if (cPages == 1u) {
                pmm_free(pa);
            } else {
                pmm_free_pages(pa, cPages);
            }
            break;
        }
    }

    /* Return a held high page if any (CPU via HHDM); else 0. */
    if (cHold > 0u) {
        pa = aHold[0];
        for (i = 1u; i < cHold; i++) {
            if (cPages == 1u) {
                pmm_free(aHold[i]);
            } else {
                pmm_free_pages(aHold[i], cPages);
            }
        }
        return pa;
    }
    return 0;
}

static struct ldmas_dma_slot *
ldmas_dma_slot_alloc(void)
{
    u32 i;

    for (i = 0u; i < LINUX_DMA_SOFT_SLOT_MAX; i++) {
        if (g_aDma[i].u8Live == 0u) {
            memset(&g_aDma[i], 0, sizeof(g_aDma[i]));
            g_aDma[i].u8Live = 1u;
            return &g_aDma[i];
        }
    }
    return NULL;
}

static struct ldmas_dma_slot *
ldmas_dma_slot_find(void *pCpu, u64 u64Dma)
{
    u32 i;

    for (i = 0u; i < LINUX_DMA_SOFT_SLOT_MAX; i++) {
        if (g_aDma[i].u8Live == 0u) {
            continue;
        }
        if (pCpu != NULL && g_aDma[i].pCpu == pCpu) {
            return &g_aDma[i];
        }
        if (u64Dma != 0ull && g_aDma[i].u64Dma == u64Dma) {
            return &g_aDma[i];
        }
    }
    return NULL;
}

static void
ldmas_dma_slot_free(struct ldmas_dma_slot *pSlot)
{
    if (pSlot == NULL || pSlot->u8Live == 0u) {
        return;
    }
    if (pSlot->cPages > 0u && pSlot->paBase != 0) {
        if (pSlot->cPages == 1u) {
            pmm_free(pSlot->paBase);
        } else {
            pmm_free_pages(pSlot->paBase, pSlot->cPages);
        }
        if (g_cPagesUsed >= pSlot->cPages) {
            g_cPagesUsed -= pSlot->cPages;
        } else {
            g_cPagesUsed = 0u;
        }
    }
    memset(pSlot, 0, sizeof(*pSlot));
}

/* ---- DMA alloc / free --------------------------------------------------- */

void *
dma_alloc_attrs(void *dev, unsigned long size, u64 *pDma, unsigned gfp,
                unsigned long attrs)
{
    u32 cPages;
    gj_paddr_t pa;
    void *pCpu;
    struct ldmas_dma_slot *pSlot;

    (void)dev;
    (void)gfp;
    (void)attrs;

    if (pDma != NULL) {
        *pDma = 0ull;
    }
    if (size == 0ul || pDma == NULL) {
        ldmas_inc(&g_cAllocFail);
        return NULL;
    }

    cPages = ldmas_pages_for(size);
    if (cPages == 0u || cPages > LINUX_DMA_SOFT_PAGE_MAX) {
        ldmas_inc(&g_cAllocFail);
        return NULL;
    }
    if (g_cPagesUsed + cPages > LINUX_DMA_SOFT_PAGE_MAX) {
        ldmas_inc(&g_cAllocFail);
        return NULL;
    }

    pSlot = ldmas_dma_slot_alloc();
    if (pSlot == NULL) {
        ldmas_inc(&g_cAllocFail);
        return NULL;
    }

    pa = ldmas_alloc_pages_low(cPages);
    if (pa == 0) {
        pSlot->u8Live = 0u;
        ldmas_inc(&g_cAllocFail);
        return NULL;
    }

    pCpu = ldmas_cpu_va(pa);
    if (pCpu == NULL) {
        if (cPages == 1u) {
            pmm_free(pa);
        } else {
            pmm_free_pages(pa, cPages);
        }
        pSlot->u8Live = 0u;
        ldmas_inc(&g_cAllocFail);
        return NULL;
    }

    ldmas_zero_pages(pa, cPages);

    pSlot->cPages = cPages;
    pSlot->paBase = pa;
    pSlot->pCpu = pCpu;
    pSlot->u64Dma = (u64)pa;
    pSlot->cbSize = size;

    g_cPagesUsed += cPages;
    *pDma = (u64)pa;
    ldmas_inc(&g_cAllocOk);
    return pCpu;
}

void
dma_free_attrs(void *dev, unsigned long size, void *cpu, u64 dma,
               unsigned long attrs)
{
    struct ldmas_dma_slot *pSlot;

    (void)dev;
    (void)size;
    (void)attrs;

    if (cpu == NULL && dma == 0ull) {
        return;
    }
    pSlot = ldmas_dma_slot_find(cpu, dma);
    if (pSlot == NULL) {
        return;
    }
    ldmas_dma_slot_free(pSlot);
    ldmas_inc(&g_cFree);
}

void *
dma_alloc_coherent(void *dev, unsigned long size, u64 *pDma, unsigned gfp)
{
    return dma_alloc_attrs(dev, size, pDma, gfp, 0ul);
}

void
dma_free_coherent(void *dev, unsigned long size, void *cpu, u64 dma)
{
    dma_free_attrs(dev, size, cpu, dma, 0ul);
}

/* ---- Map / unmap / sync ------------------------------------------------- */

u64
dma_map_page_attrs(void *dev, void *page, unsigned long offset,
                   unsigned long size, int dir, unsigned long attrs)
{
    u64 u64Base;

    (void)dev;
    (void)size;
    (void)dir;
    (void)attrs;

    if (page == NULL) {
        return 0ull;
    }
    /*
     * Soft: page cookie treated as identity VA or soft page base.
     * Host r8169.ko may pass real struct page *; soft best-effort only.
     */
    u64Base = (u64)(uintptr_t)page;
    ldmas_inc(&g_cMap);
    return u64Base + (u64)offset;
}

void
dma_unmap_page_attrs(void *dev, u64 dma, unsigned long size, int dir,
                     unsigned long attrs)
{
    (void)dev;
    (void)dma;
    (void)size;
    (void)dir;
    (void)attrs;
    ldmas_inc(&g_cUnmap);
}

u64
dma_map_single(void *dev, void *cpu, unsigned long size, int dir)
{
    (void)dev;
    (void)size;
    (void)dir;

    if (cpu == NULL) {
        return 0ull;
    }
    ldmas_inc(&g_cMap);
    /* Soft identity: bus address == CPU pointer value for low identity. */
    return (u64)(uintptr_t)cpu;
}

void
dma_unmap_single(void *dev, u64 dma, unsigned long size, int dir)
{
    (void)dev;
    (void)dma;
    (void)size;
    (void)dir;
    ldmas_inc(&g_cUnmap);
}

void
__dma_sync_single_for_cpu(void *dev, u64 addr, unsigned long size, int dir)
{
    (void)dev;
    (void)addr;
    (void)size;
    (void)dir;
    /* Soft coherent: compiler barrier only. */
    __asm__ __volatile__("" ::: "memory");
    ldmas_inc(&g_cSyncCpu);
}

void
__dma_sync_single_for_device(void *dev, u64 addr, unsigned long size, int dir)
{
    (void)dev;
    (void)addr;
    (void)size;
    (void)dir;
    __asm__ __volatile__("" ::: "memory");
    ldmas_inc(&g_cSyncDev);
}

int
dma_set_mask(void *dev, u64 mask)
{
    (void)dev;
    (void)mask;
    return 0;
}

int
dma_set_coherent_mask(void *dev, u64 mask)
{
    (void)dev;
    (void)mask;
    return 0;
}

/* ---- MMIO --------------------------------------------------------------- */

static void *
ldmas_ioremap_common(u64 phys, unsigned long size)
{
    void *pVa;
    u32 i;
    struct ldmas_iomap *pSlot;

    if (size == 0ul) {
        return NULL;
    }

    /* Soft identity VA = PA (kernel identity / UC BAR maps for low+device). */
    pVa = (void *)(uintptr_t)phys;

    pSlot = NULL;
    for (i = 0u; i < LINUX_DMA_SOFT_IOMAP_MAX; i++) {
        if (g_aIo[i].u8Live == 0u) {
            pSlot = &g_aIo[i];
            break;
        }
    }
    if (pSlot != NULL) {
        memset(pSlot, 0, sizeof(*pSlot));
        pSlot->u8Live = 1u;
        pSlot->u64Phys = phys;
        pSlot->cbSize = size;
        pSlot->pVa = pVa;
    }

    ldmas_inc(&g_cIoremap);
    return pVa;
}

void *
ioremap(u64 phys, unsigned long size)
{
    return ldmas_ioremap_common(phys, size);
}

void *
ioremap_wc(u64 phys, unsigned long size)
{
    return ldmas_ioremap_common(phys, size);
}

void *
ioremap_noprofile(u64 phys, unsigned long size)
{
    return ldmas_ioremap_common(phys, size);
}

void
iounmap(void *addr)
{
    u32 i;

    if (addr == NULL) {
        return;
    }
    for (i = 0u; i < LINUX_DMA_SOFT_IOMAP_MAX; i++) {
        if (g_aIo[i].u8Live != 0u && g_aIo[i].pVa == addr) {
            memset(&g_aIo[i], 0, sizeof(g_aIo[i]));
            break;
        }
    }
    ldmas_inc(&g_cIounmap);
}

u32
readl(const volatile void *addr)
{
    if (addr == NULL) {
        return 0u;
    }
    return *(const volatile u32 *)addr;
}

void
writel(u32 val, volatile void *addr)
{
    if (addr == NULL) {
        return;
    }
    *(volatile u32 *)addr = val;
}

u16
readw(const volatile void *addr)
{
    if (addr == NULL) {
        return 0u;
    }
    return *(const volatile u16 *)addr;
}

void
writew(u16 val, volatile void *addr)
{
    if (addr == NULL) {
        return;
    }
    *(volatile u16 *)addr = val;
}

u8
readb(const volatile void *addr)
{
    if (addr == NULL) {
        return 0u;
    }
    return *(const volatile u8 *)addr;
}

void
writeb(u8 val, volatile void *addr)
{
    if (addr == NULL) {
        return;
    }
    *(volatile u8 *)addr = val;
}

/* ---- kmalloc_trace / kfree ---------------------------------------------- */

void *
kmalloc_trace(void *s, unsigned long size, unsigned gfp)
{
    void *p;
    u32 cPages;
    gj_paddr_t pa;
    u32 i;
    struct ldmas_kmalloc *pSlot;

    (void)s;
    (void)gfp;

    if (size == 0ul) {
        return NULL;
    }

    /* Prefer kheap for sub-page payloads. */
    if (size < (unsigned long)GJ_PAGE_SIZE - 64ul) {
        p = kheap_alloc((size_t)size);
        if (p != NULL) {
            /* Track as kheap for honest free path. */
            for (i = 0u; i < LINUX_DMA_SOFT_KMALLOC_MAX; i++) {
                if (g_aKm[i].u8Live == 0u) {
                    memset(&g_aKm[i], 0, sizeof(g_aKm[i]));
                    g_aKm[i].u8Live = 1u;
                    g_aKm[i].u8Kind = 0u;
                    g_aKm[i].pCpu = p;
                    g_aKm[i].cbSize = size;
                    break;
                }
            }
            ldmas_inc(&g_cKmallocOk);
            return p;
        }
    }

    cPages = ldmas_pages_for(size);
    if (cPages == 0u || cPages > LINUX_DMA_SOFT_PAGE_MAX) {
        return NULL;
    }

    pSlot = NULL;
    for (i = 0u; i < LINUX_DMA_SOFT_KMALLOC_MAX; i++) {
        if (g_aKm[i].u8Live == 0u) {
            pSlot = &g_aKm[i];
            break;
        }
    }
    if (pSlot == NULL) {
        return NULL;
    }

    pa = ldmas_alloc_pages_low(cPages);
    if (pa == 0) {
        return NULL;
    }
    p = ldmas_cpu_va(pa);
    if (p == NULL) {
        if (cPages == 1u) {
            pmm_free(pa);
        } else {
            pmm_free_pages(pa, cPages);
        }
        return NULL;
    }
    ldmas_zero_pages(pa, cPages);

    memset(pSlot, 0, sizeof(*pSlot));
    pSlot->u8Live = 1u;
    pSlot->u8Kind = 1u;
    pSlot->cPages = cPages;
    pSlot->pCpu = p;
    pSlot->paBase = pa;
    pSlot->cbSize = size;
    ldmas_inc(&g_cKmallocOk);
    return p;
}

void
kfree(const void *p)
{
    u32 i;
    struct ldmas_kmalloc *pSlot;

    if (p == NULL) {
        return;
    }

    for (i = 0u; i < LINUX_DMA_SOFT_KMALLOC_MAX; i++) {
        if (g_aKm[i].u8Live == 0u || g_aKm[i].pCpu != (void *)p) {
            continue;
        }
        pSlot = &g_aKm[i];
        if (pSlot->u8Kind == 0u) {
            kheap_free((void *)p);
        } else if (pSlot->cPages > 0u && pSlot->paBase != 0) {
            if (pSlot->cPages == 1u) {
                pmm_free(pSlot->paBase);
            } else {
                pmm_free_pages(pSlot->paBase, pSlot->cPages);
            }
        }
        memset(pSlot, 0, sizeof(*pSlot));
        ldmas_inc(&g_cKfree);
        return;
    }

    /*
     * Soft: untracked pointer — do not probe kheap (header walk may fault
     * on non-heap VA). Silent soft free count only.
     */
    ldmas_inc(&g_cKfree);
}

/* ---- Init / diagnostics ------------------------------------------------- */

void
linux_dma_soft_init(void)
{
    u32 u32KsymOk;
    u32 u32KsymSkip;
    u32 u32N;

    g_cInitCalls++;
    if (g_fReady) {
        /* Grep: linux_dma_soft: soft init PASS n= */
        kprintf("linux_dma_soft: soft init PASS n=%u (idempotent call=%u)\n",
                (unsigned)g_cInitCalls, (unsigned)g_cInitCalls);
        return;
    }

    g_cPagesUsed = 0u;
    g_cAllocOk = 0u;
    g_cAllocFail = 0u;
    g_cFree = 0u;
    g_cMap = 0u;
    g_cUnmap = 0u;
    g_cSyncCpu = 0u;
    g_cSyncDev = 0u;
    g_cIoremap = 0u;
    g_cIounmap = 0u;
    g_cKmallocOk = 0u;
    g_cKfree = 0u;
    memset(g_aDma, 0, sizeof(g_aDma));
    memset(g_aIo, 0, sizeof(g_aIo));
    memset(g_aKm, 0, sizeof(g_aKm));
    g_fReady = 1;

    /*
     * Export soft bodies for F2 ksym / module resolve (replace stubs).
     * Weak linux_ksym_register: no-op SKIP when F2 not linked yet.
     */
    u32KsymOk = 0u;
    u32KsymSkip = 0u;

    ldmas_ksym_one("dma_alloc_attrs", (void *)dma_alloc_attrs, &u32KsymOk,
                   &u32KsymSkip);
    ldmas_ksym_one("dma_alloc_coherent", (void *)dma_alloc_coherent, &u32KsymOk,
                   &u32KsymSkip);
    ldmas_ksym_one("dma_free_attrs", (void *)dma_free_attrs, &u32KsymOk,
                   &u32KsymSkip);
    ldmas_ksym_one("dma_free_coherent", (void *)dma_free_coherent, &u32KsymOk,
                   &u32KsymSkip);
    ldmas_ksym_one("dma_map_page_attrs", (void *)dma_map_page_attrs, &u32KsymOk,
                   &u32KsymSkip);
    ldmas_ksym_one("dma_unmap_page_attrs", (void *)dma_unmap_page_attrs,
                   &u32KsymOk, &u32KsymSkip);
    ldmas_ksym_one("dma_map_single", (void *)dma_map_single, &u32KsymOk,
                   &u32KsymSkip);
    ldmas_ksym_one("dma_unmap_single", (void *)dma_unmap_single, &u32KsymOk,
                   &u32KsymSkip);
    ldmas_ksym_one("__dma_sync_single_for_cpu",
                   (void *)__dma_sync_single_for_cpu, &u32KsymOk, &u32KsymSkip);
    ldmas_ksym_one("__dma_sync_single_for_device",
                   (void *)__dma_sync_single_for_device, &u32KsymOk,
                   &u32KsymSkip);
    ldmas_ksym_one("dma_set_mask", (void *)dma_set_mask, &u32KsymOk,
                   &u32KsymSkip);
    ldmas_ksym_one("dma_set_coherent_mask", (void *)dma_set_coherent_mask,
                   &u32KsymOk, &u32KsymSkip);

    ldmas_ksym_one("ioremap", (void *)ioremap, &u32KsymOk, &u32KsymSkip);
    ldmas_ksym_one("ioremap_wc", (void *)ioremap_wc, &u32KsymOk, &u32KsymSkip);
    ldmas_ksym_one("ioremap_noprofile", (void *)ioremap_noprofile, &u32KsymOk,
                   &u32KsymSkip);
    ldmas_ksym_one("iounmap", (void *)iounmap, &u32KsymOk, &u32KsymSkip);

    ldmas_ksym_one("readl", (void *)readl, &u32KsymOk, &u32KsymSkip);
    ldmas_ksym_one("writel", (void *)writel, &u32KsymOk, &u32KsymSkip);
    ldmas_ksym_one("readw", (void *)readw, &u32KsymOk, &u32KsymSkip);
    ldmas_ksym_one("writew", (void *)writew, &u32KsymOk, &u32KsymSkip);
    ldmas_ksym_one("readb", (void *)readb, &u32KsymOk, &u32KsymSkip);
    ldmas_ksym_one("writeb", (void *)writeb, &u32KsymOk, &u32KsymSkip);

    ldmas_ksym_one("kmalloc_trace", (void *)kmalloc_trace, &u32KsymOk,
                   &u32KsymSkip);
    ldmas_ksym_one("kfree", (void *)kfree, &u32KsymOk, &u32KsymSkip);

    u32N = u32KsymOk;

    /* Grep: linux_dma_soft: soft init PASS n= */
    kprintf("linux_dma_soft: soft init PASS n=%u page_max=%u slot_max=%u "
            "id_max=4GiB soft_ne_product=1\n",
            (unsigned)u32N, (unsigned)LINUX_DMA_SOFT_PAGE_MAX,
            (unsigned)LINUX_DMA_SOFT_SLOT_MAX);

    if (u32KsymOk > 0u) {
        /* Grep: linux_dma_soft: soft ksym register PASS */
        kprintf("linux_dma_soft: soft ksym register PASS n=%u skip=%u\n",
                (unsigned)u32KsymOk, (unsigned)u32KsymSkip);
    } else {
        /* Grep: linux_dma_soft: soft ksym register SKIP */
        kprintf("linux_dma_soft: soft ksym register SKIP (no linux_ksym or "
                "all failed) skip=%u\n",
                (unsigned)u32KsymSkip);
    }
}

int
linux_dma_soft_ready(void)
{
    return g_fReady;
}

u32
linux_dma_soft_pages_used(void)
{
    return g_cPagesUsed;
}

u32
linux_dma_soft_alloc_ok(void)
{
    return g_cAllocOk;
}
