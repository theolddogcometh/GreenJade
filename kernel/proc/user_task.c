/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Map ring-3 trampoline + stack; enter via sysret.
 * Personality window hosts freestanding protonrt door server (G-PERS).
 *
 * VA layout (high enough to clear kernel identity BSS):
 *   ring3 code  @ GJ_USER_CODE_VA   (0x0100_0000)
 *   ring3 stack @ GJ_USER_STACK_TOP (0x0110_0000, grows down)
 *   pers  code  @ GJ_PERS_CODE_VA   (0x0120_0000)
 *   pers  stack @ GJ_PERS_STACK_TOP (0x0130_0000, grows down)
 *
 * Soft product (grep: user: ring3 map soft | user: personality map soft):
 *   Post-map PTE soft (P|U, code RX / stack RW|NX) + payload soft match
 *   Layout soft (user band, page align, code/stack non-overlap, dual windows)
 *   Fail closed on soft bad; greppable soft PASS|FAIL; soft stats counters
 *   Idempotent re-map: already-live soft recheck without double-install
 *
 * Pure C freestanding; dual MIT OR Apache-2.0.
 */
#include <gj/config.h>
#include <gj/cpu.h>
#include <gj/cpu_sys.h>
#include <gj/klog.h>
#include <gj/memobj.h>
#include <gj/pmm.h>
#include <gj/process.h>
#include <gj/string.h>
#include <gj/user_access.h>
#include <gj/user_task.h>
#include <gj/vmm.h>

/* PTE soft bits (mirror vmm / user_copy; local so this unit stays self-contained). */
#define UT_PTE_P   (1ull << 0)
#define UT_PTE_W   (1ull << 1)
#define UT_PTE_U   (1ull << 2)
#define UT_PTE_NX  (1ull << 63)
#define UT_PTE_ADDR_MASK 0x000ffffffffff000ull

static u64 g_u64PersEntry;
static u64 g_u64PersStack;

extern char gj_user_ring3_blob[];
extern char gj_user_ring3_blob_end[];
extern char gj_protonrt_user_blob[];
extern char gj_protonrt_user_blob_end[];

static int g_fUserMapped;
static int g_fPersMapped;

/* ---- Soft map / enter counters (grep: user: soft stats) ---------------- */
static u32 g_cRing3MapOk;
static u32 g_cRing3MapFail;
static u32 g_cRing3Soft;
static u32 g_cRing3SoftBad;
static u32 g_cPersMapOk;
static u32 g_cPersMapFail;
static u32 g_cPersSoft;
static u32 g_cPersSoftBad;
static u32 g_cEnterOk;
static u32 g_cEnterSkip;
static u32 g_cCodePages;
static u32 g_cStackPages;
/* Soft-tracked install sizes for recheck / partial unmap. */
static size_t g_cbRing3Blob;
static size_t g_cbPersBlob;
static u32    g_cRing3CodePages;
static u32    g_cPersCodePages;

/**
 * Install user RX (or RW) page: zero frame under kernel CR3, copy payload,
 * map with final prot. Avoids intermediate RW→RX races / wrong PTE flags.
 */
static gj_status_t
user_install_page(gj_vaddr_t va, const void *pSrc, size_t cbSrc, u32 u32Prot)
{
    gj_paddr_t pa;
    u64 u64Saved;
    gj_status_t st;

    pa = pmm_alloc();
    if (pa == 0) {
        return GJ_ERR_NOMEM;
    }
    u64Saved = cpu_read_cr3();
    cpu_load_cr3(vmm_kernel_cr3());
    memset((void *)(gj_vaddr_t)pa, 0, GJ_PAGE_SIZE);
    if (pSrc != NULL && cbSrc > 0) {
        if (cbSrc > GJ_PAGE_SIZE) {
            cbSrc = GJ_PAGE_SIZE;
        }
        memcpy((void *)(gj_vaddr_t)pa, pSrc, cbSrc);
    }
    cpu_load_cr3(u64Saved);

    st = vmm_map_page(va, pa, u32Prot | GJ_VMM_PROT_USER);
    if (st != GJ_OK) {
        pmm_free(pa);
        return st;
    }
    return GJ_OK;
}

/**
 * Soft-unmap + free a contiguous install window (partial fail cleanup).
 * Best-effort: ignores NOENT; never panics.
 */
static void
user_unmap_window(gj_vaddr_t vaBase, u32 cPages)
{
    u32 iPage;

    for (iPage = 0; iPage < cPages; iPage++) {
        gj_vaddr_t va = vaBase + (gj_vaddr_t)iPage * (gj_vaddr_t)GJ_PAGE_SIZE;
        gj_paddr_t pa = vmm_virt_to_phys(va);

        (void)vmm_unmap_page(va);
        if (pa != 0) {
            pmm_free(pa & (gj_paddr_t)UT_PTE_ADDR_MASK);
        }
    }
}

/**
 * Multi-page RX blob install (soft max cMaxPages).
 * On any page fail: reverse-unmaps prior pages of this install.
 */
static gj_status_t
user_install_blob_rx(gj_vaddr_t vaBase, const void *pSrc, size_t cbSrc,
                     u32 cMaxPages, u32 *pOutPages)
{
    u32 cPages;
    u32 iPage;
    gj_status_t st;

    if (pOutPages != NULL) {
        *pOutPages = 0;
    }
    if (pSrc == NULL || cbSrc == 0 || cMaxPages == 0) {
        return GJ_ERR_INVAL;
    }
    cPages = (u32)((cbSrc + (size_t)GJ_PAGE_SIZE - 1u) / (size_t)GJ_PAGE_SIZE);
    if (cPages == 0 || cPages > cMaxPages) {
        return GJ_ERR_INVAL;
    }

    for (iPage = 0; iPage < cPages; iPage++) {
        size_t cbOff = (size_t)iPage * (size_t)GJ_PAGE_SIZE;
        size_t cbChunk = cbSrc - cbOff;
        const u8 *pChunk = (const u8 *)pSrc + cbOff;

        if (cbChunk > (size_t)GJ_PAGE_SIZE) {
            cbChunk = (size_t)GJ_PAGE_SIZE;
        }
        st = user_install_page(vaBase + (gj_vaddr_t)iPage * (gj_vaddr_t)GJ_PAGE_SIZE,
                               pChunk, cbChunk,
                               GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC);
        if (st != GJ_OK) {
            user_unmap_window(vaBase, iPage);
            return st;
        }
    }
    g_cCodePages += cPages;
    if (pOutPages != NULL) {
        *pOutPages = cPages;
    }
    return GJ_OK;
}

/**
 * Map stack pages growing down from u64StackTop (RW user).
 * Soft: reverse-unmaps partial stack on first failure.
 * Returns 0 on success, -1 on first map failure.
 */
static int
user_map_stack(u64 u64StackTop, u32 cPages)
{
    u32 iPage;
    gj_status_t st;

    if (cPages == 0 || (u64StackTop & ((u64)GJ_PAGE_SIZE - 1ull)) != 0) {
        kprintf("user: map stack soft FAIL align top=0x%lx pages=%u\n",
                (unsigned long)u64StackTop, cPages);
        return -1;
    }

    for (iPage = 0; iPage < cPages; iPage++) {
        u64 u64PageVa = u64StackTop - ((u64)iPage + 1ull) * GJ_PAGE_SIZE;

        st = user_install_page((gj_vaddr_t)u64PageVa, NULL, 0,
                               GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE);
        if (st != GJ_OK) {
            u32 iUndo;

            kprintf("user: map stack page %u va=0x%lx failed %d\n", iPage,
                    (unsigned long)u64PageVa, (int)st);
            /* Soft reverse: unmap pages already installed in this call. */
            for (iUndo = 0; iUndo < iPage; iUndo++) {
                u64 u64UndoVa =
                    u64StackTop - ((u64)iUndo + 1ull) * GJ_PAGE_SIZE;
                gj_paddr_t pa = vmm_virt_to_phys((gj_vaddr_t)u64UndoVa);

                (void)vmm_unmap_page((gj_vaddr_t)u64UndoVa);
                if (pa != 0) {
                    pmm_free(pa & (gj_paddr_t)UT_PTE_ADDR_MASK);
                }
            }
            return -1;
        }
    }
    g_cStackPages += cPages;
    return 0;
}

/**
 * Soft layout: code + stack inside user band, page-aligned, non-overlapping.
 * Also refuses collision with the sibling product window (ring3 vs personality).
 */
static int
user_layout_soft_ok(u64 u64CodeVa, size_t cbCode, u64 u64StackTop, u32 cStackPages,
                    u64 u64SiblingCode, size_t cbSibling, u64 u64SiblingStackTop,
                    u32 cSiblingStack, int fSiblingLive)
{
    u64 u64CodeEnd;
    u64 u64StackLo;
    u64 u64StackHi;
    size_t cbCodeRound;

    if ((u64CodeVa & ((u64)GJ_PAGE_SIZE - 1ull)) != 0) {
        return 0;
    }
    if ((u64StackTop & ((u64)GJ_PAGE_SIZE - 1ull)) != 0) {
        return 0;
    }
    if (cbCode == 0 || cStackPages == 0) {
        return 0;
    }

    cbCodeRound =
        (cbCode + (size_t)GJ_PAGE_SIZE - 1u) & ~((size_t)GJ_PAGE_SIZE - 1u);
    u64CodeEnd = u64CodeVa + (u64)cbCodeRound;
    u64StackHi = u64StackTop;
    u64StackLo = u64StackTop - (u64)cStackPages * (u64)GJ_PAGE_SIZE;

    if (u64StackLo >= u64StackHi) {
        return 0; /* underflow */
    }
    /* Entire code + stack must sit in the canonical user window. */
    if (!user_range_ok(u64CodeVa, (u64)cbCodeRound)) {
        return 0;
    }
    if (!user_range_ok(u64StackLo, u64StackHi - u64StackLo)) {
        return 0;
    }
    /* Code must not overlap own stack. */
    if (!(u64CodeEnd <= u64StackLo || u64CodeVa >= u64StackHi)) {
        return 0;
    }

    if (fSiblingLive != 0 && cbSibling > 0 && cSiblingStack > 0) {
        size_t cbSibRound =
            (cbSibling + (size_t)GJ_PAGE_SIZE - 1u) &
            ~((size_t)GJ_PAGE_SIZE - 1u);
        u64 u64SibCodeEnd = u64SiblingCode + (u64)cbSibRound;
        u64 u64SibStackHi = u64SiblingStackTop;
        u64 u64SibStackLo =
            u64SiblingStackTop - (u64)cSiblingStack * (u64)GJ_PAGE_SIZE;

        /* Soft: dual windows must not share code or stack pages. */
        if (!(u64CodeEnd <= u64SiblingCode || u64CodeVa >= u64SibCodeEnd)) {
            return 0;
        }
        if (!(u64StackHi <= u64SibStackLo || u64StackLo >= u64SibStackHi)) {
            return 0;
        }
        if (!(u64CodeEnd <= u64SibStackLo || u64CodeVa >= u64SibStackHi)) {
            return 0;
        }
        if (!(u64StackHi <= u64SiblingCode || u64StackLo >= u64SibCodeEnd)) {
            return 0;
        }
    }
    return 1;
}

/**
 * Soft RX leaf: present + USER + not writable + executable (!NX).
 */
static int
user_soft_pte_rx(gj_vaddr_t va)
{
    u64 u64Pte = vmm_read_pte(va);

    if ((u64Pte & UT_PTE_P) == 0) {
        return 0;
    }
    if ((u64Pte & UT_PTE_U) == 0) {
        return 0;
    }
    if ((u64Pte & UT_PTE_W) != 0) {
        return 0; /* W^X soft for trampoline / door code */
    }
    if ((u64Pte & UT_PTE_NX) != 0) {
        return 0;
    }
    return 1;
}

/**
 * Soft RW leaf: present + USER + writable + NX preferred (non-exec stack).
 */
static int
user_soft_pte_rw(gj_vaddr_t va)
{
    u64 u64Pte = vmm_read_pte(va);

    if ((u64Pte & UT_PTE_P) == 0) {
        return 0;
    }
    if ((u64Pte & UT_PTE_U) == 0) {
        return 0;
    }
    if ((u64Pte & UT_PTE_W) == 0) {
        return 0;
    }
    if ((u64Pte & UT_PTE_NX) == 0) {
        return 0; /* stack must not be executable */
    }
    return 1;
}

/**
 * Soft payload match: compare mapped frames to blob under kernel CR3 identity.
 * cb may span multiple pages. Returns 1 on match, 0 on mismatch / missing PA.
 */
static int
user_soft_payload_match(gj_vaddr_t vaBase, const void *pSrc, size_t cbSrc)
{
    size_t cbOff;
    u64 u64Saved;
    const u8 *pWant;

    if (pSrc == NULL || cbSrc == 0) {
        return 0;
    }
    pWant = (const u8 *)pSrc;
    u64Saved = cpu_read_cr3();
    cpu_load_cr3(vmm_kernel_cr3());

    for (cbOff = 0; cbOff < cbSrc; ) {
        gj_vaddr_t vaPage =
            (vaBase + (gj_vaddr_t)cbOff) & ~(gj_vaddr_t)(GJ_PAGE_SIZE - 1);
        size_t cbPageOff = (size_t)((vaBase + (gj_vaddr_t)cbOff) - vaPage);
        size_t cbChunk = (size_t)GJ_PAGE_SIZE - cbPageOff;
        gj_paddr_t pa;
        const u8 *pHave;
        size_t iByte;

        if (cbChunk > cbSrc - cbOff) {
            cbChunk = cbSrc - cbOff;
        }
        /*
         * Walk under the *saved* (process) CR3 so we read the product PTE,
         * then compare bytes via kernel identity of that PA.
         */
        cpu_load_cr3(u64Saved);
        pa = vmm_virt_to_phys(vaPage);
        cpu_load_cr3(vmm_kernel_cr3());
        if (pa == 0) {
            cpu_load_cr3(u64Saved);
            return 0;
        }
        pHave = (const u8 *)(gj_vaddr_t)(pa & (gj_paddr_t)UT_PTE_ADDR_MASK);
        pHave += cbPageOff;
        for (iByte = 0; iByte < cbChunk; iByte++) {
            if (pHave[iByte] != pWant[cbOff + iByte]) {
                cpu_load_cr3(u64Saved);
                return 0;
            }
        }
        cbOff += cbChunk;
    }

    cpu_load_cr3(u64Saved);
    return 1;
}

/**
 * Soft post-map observe for a code+stack window.
 * fWantNativePersona: 0 = expect LINUX (1), 1 = expect NATIVE (0); -1 = skip.
 * szTag is "ring3" or "personality" for greppable lines.
 * Returns GJ_OK on soft PASS; GJ_ERR_* on soft FAIL (caller fail-closes).
 */
static gj_status_t
user_map_soft_observe(u64 u64CodeVa, size_t cbCode, const void *pBlob,
                      u64 u64StackTop, u32 cStackPages, struct gj_process *pProc,
                      int nWantPersona, /* 0 native, 1 linux, -1 skip */
                      const char *szTag, u32 *pSoftOk, u32 *pSoftBad)
{
    u32 iPage;
    u32 cCodePages;
    u64 u64StackLo;
    int fMapped;
    int fAccess;

    if (szTag == NULL) {
        szTag = "user";
    }
    if (cbCode == 0 || pBlob == NULL || cStackPages == 0) {
        if (pSoftBad != NULL) {
            (*pSoftBad)++;
        }
        kprintf("user: %s map soft FAIL inval cb=%lu pages=%u\n", szTag,
                (unsigned long)cbCode, cStackPages);
        return GJ_ERR_INVAL;
    }

    cCodePages =
        (u32)((cbCode + (size_t)GJ_PAGE_SIZE - 1u) / (size_t)GJ_PAGE_SIZE);
    u64StackLo = u64StackTop - (u64)cStackPages * (u64)GJ_PAGE_SIZE;

    /* Soft: every code leaf RX + USER. */
    for (iPage = 0; iPage < cCodePages; iPage++) {
        gj_vaddr_t va =
            (gj_vaddr_t)u64CodeVa + (gj_vaddr_t)iPage * (gj_vaddr_t)GJ_PAGE_SIZE;

        if (!user_soft_pte_rx(va)) {
            if (pSoftBad != NULL) {
                (*pSoftBad)++;
            }
            kprintf("user: %s map soft FAIL code pte va=0x%lx pte=0x%lx\n",
                    szTag, (unsigned long)va,
                    (unsigned long)vmm_read_pte(va));
            return GJ_ERR_FAULT;
        }
    }

    /* Soft: every stack leaf RW|NX + USER. */
    for (iPage = 0; iPage < cStackPages; iPage++) {
        gj_vaddr_t va =
            (gj_vaddr_t)(u64StackTop - ((u64)iPage + 1ull) * GJ_PAGE_SIZE);

        if (!user_soft_pte_rw(va)) {
            if (pSoftBad != NULL) {
                (*pSoftBad)++;
            }
            kprintf("user: %s map soft FAIL stack pte va=0x%lx pte=0x%lx\n",
                    szTag, (unsigned long)va,
                    (unsigned long)vmm_read_pte(va));
            return GJ_ERR_FAULT;
        }
    }

    /* Soft: range helpers agree with raw PTE walk. */
    fMapped = user_range_mapped(u64CodeVa, (u64)cbCode);
    fAccess = user_range_mapped_access(u64StackLo,
                                       (u64)cStackPages * (u64)GJ_PAGE_SIZE,
                                       GJ_USER_ACCESS_READ | GJ_USER_ACCESS_WRITE);
    if (fMapped == 0 || fAccess == 0) {
        if (pSoftBad != NULL) {
            (*pSoftBad)++;
        }
        kprintf("user: %s map soft FAIL range code=%d stack_w=%d\n", szTag,
                fMapped, fAccess);
        return GJ_ERR_FAULT;
    }

    /* Soft: payload bytes still match the embedded blob. */
    if (!user_soft_payload_match((gj_vaddr_t)u64CodeVa, pBlob, cbCode)) {
        if (pSoftBad != NULL) {
            (*pSoftBad)++;
        }
        kprintf("user: %s map soft FAIL payload mismatch cb=%lu\n", szTag,
                (unsigned long)cbCode);
        return GJ_ERR_FAULT;
    }

    /* Soft: process personality / JIT contract when a PCB is provided. */
    if (pProc != NULL && nWantPersona >= 0) {
        if ((int)pProc->u32Personality != nWantPersona) {
            if (pSoftBad != NULL) {
                (*pSoftBad)++;
            }
            kprintf("user: %s map soft FAIL persona=%u want=%d\n", szTag,
                    pProc->u32Personality, nWantPersona);
            return GJ_ERR_PERM;
        }
        /* Ring3 trampoline must not advertise CapJit; personality is native. */
        if (nWantPersona == 1 && pProc->u32Jit != 0) {
            if (pSoftBad != NULL) {
                (*pSoftBad)++;
            }
            kprintf("user: %s map soft FAIL jit=%u want=0\n", szTag,
                    pProc->u32Jit);
            return GJ_ERR_PERM;
        }
    }

    if (pSoftOk != NULL) {
        (*pSoftOk)++;
    }
    /* Grep: user: ring3 map soft PASS / user: personality map soft PASS */
    kprintf("user: %s map soft PASS code@0x%lx stacktop@0x%lx code_pages=%u "
            "stack_pages=%u blob=%lu pte_rx=1 pte_rw=1 payload=1\n",
            szTag, (unsigned long)u64CodeVa, (unsigned long)u64StackTop,
            cCodePages, cStackPages, (unsigned long)cbCode);
    return GJ_OK;
}

/**
 * Soft teardown of a just-installed window after soft observe FAIL.
 */
static void
user_map_soft_fail_teardown(gj_vaddr_t vaCode, u32 cCodePages, u64 u64StackTop,
                            u32 cStackPages)
{
    u32 iPage;

    user_unmap_window(vaCode, cCodePages);
    for (iPage = 0; iPage < cStackPages; iPage++) {
        u64 u64PageVa = u64StackTop - ((u64)iPage + 1ull) * GJ_PAGE_SIZE;
        gj_paddr_t pa = vmm_virt_to_phys((gj_vaddr_t)u64PageVa);

        (void)vmm_unmap_page((gj_vaddr_t)u64PageVa);
        if (pa != 0) {
            pmm_free(pa & (gj_paddr_t)UT_PTE_ADDR_MASK);
        }
    }
}

int
user_task_map_ring3(struct gj_process *pProc)
{
    size_t cbBlob;
    gj_status_t st;
    u32 cCodePages = 0;

    cbBlob = (size_t)(gj_user_ring3_blob_end - gj_user_ring3_blob);
    if (cbBlob == 0 ||
        cbBlob > (size_t)GJ_USER_CODE_PAGES * (size_t)GJ_PAGE_SIZE) {
        g_cRing3MapFail++;
        kprintf("user: bad ring3 blob size %lu\n", (unsigned long)cbBlob);
        return -1;
    }

    /* Soft idempotent: already live + soft recheck still green → success. */
    if (g_fUserMapped != 0) {
        if (user_task_ring3_map_soft() == 0) {
            kprintf("user: ring3 map soft already live recheck PASS\n");
            return 0;
        }
        /* Soft bad on live map: tear down before reinstall (avoid PA leak). */
        user_map_soft_fail_teardown((gj_vaddr_t)GJ_USER_CODE_VA,
                                    g_cRing3CodePages != 0 ? g_cRing3CodePages
                                                           : 1u,
                                    GJ_USER_STACK_TOP, GJ_USER_STACK_PAGES);
        g_fUserMapped = 0;
        g_cbRing3Blob = 0;
        g_cRing3CodePages = 0;
    }

    if (!user_layout_soft_ok(GJ_USER_CODE_VA, cbBlob, GJ_USER_STACK_TOP,
                             GJ_USER_STACK_PAGES, GJ_PERS_CODE_VA, g_cbPersBlob,
                             GJ_PERS_STACK_TOP, GJ_PERS_STACK_PAGES,
                             g_fPersMapped)) {
        g_cRing3MapFail++;
        g_cRing3SoftBad++;
        kprintf("user: ring3 map soft FAIL layout\n");
        return -1;
    }

    if (pProc != NULL) {
        if (process_as_ensure(pProc) != GJ_OK) {
            g_cRing3MapFail++;
            kprintf("user: as_ensure failed\n");
            return -1;
        }
        process_as_activate(pProc);
    }

    st = user_install_blob_rx((gj_vaddr_t)GJ_USER_CODE_VA, gj_user_ring3_blob,
                              cbBlob, GJ_USER_CODE_PAGES, &cCodePages);
    if (st != GJ_OK) {
        g_cRing3MapFail++;
        kprintf("user: map code RX failed %d\n", (int)st);
        return -1;
    }

    if (user_map_stack(GJ_USER_STACK_TOP, GJ_USER_STACK_PAGES) != 0) {
        user_unmap_window((gj_vaddr_t)GJ_USER_CODE_VA, cCodePages);
        g_cRing3MapFail++;
        return -1;
    }

    if (pProc != NULL) {
        pProc->u32Personality = 1; /* Linux-shaped trampoline for ABI smokes */
        gj_process_set_jit(pProc, 0);
    }

    st = user_map_soft_observe(GJ_USER_CODE_VA, cbBlob, gj_user_ring3_blob,
                               GJ_USER_STACK_TOP, GJ_USER_STACK_PAGES, pProc,
                               /* linux */ 1, "ring3", &g_cRing3Soft,
                               &g_cRing3SoftBad);
    if (st != GJ_OK) {
        /* Fail closed: drop partial product window. */
        user_map_soft_fail_teardown((gj_vaddr_t)GJ_USER_CODE_VA, cCodePages,
                                    GJ_USER_STACK_TOP, GJ_USER_STACK_PAGES);
        g_cRing3MapFail++;
        kprintf("user: ring3 map soft FAIL st=%d\n", (int)st);
        return -1;
    }

    g_fUserMapped = 1;
    g_cbRing3Blob = cbBlob;
    g_cRing3CodePages = cCodePages;
    g_cRing3MapOk++;
    kprintf("user: ring3 code@0x%lx stacktop@0x%lx blob=%lu pages=%u PASS\n",
            (unsigned long)GJ_USER_CODE_VA,
            (unsigned long)GJ_USER_STACK_TOP, (unsigned long)cbBlob, cCodePages);
    return 0;
}

void
user_task_enter_ring3(void)
{
    if (!g_fUserMapped && !g_fPersMapped) {
        g_cEnterSkip++;
        kprintf("user: not mapped\n");
        kprintf("user: ring3 enter soft SKIP not_mapped\n");
        return;
    }
    if (!cpu_syscall_ready()) {
        g_cEnterSkip++;
        kprintf("user: SYSCALL not ready\n");
        kprintf("user: ring3 enter soft SKIP syscall\n");
        return;
    }
    /* Soft recheck of the trampoline window before irrevocable enter. */
    if (g_fUserMapped != 0 && user_task_ring3_map_soft() != 0) {
        g_cEnterSkip++;
        kprintf("user: ring3 enter soft SKIP recheck\n");
        return;
    }
    g_cEnterOk++;
    kprintf("user: entering ring3…\n");
    kprintf("user: ring3 enter soft PASS code@0x%lx stack@0x%lx\n",
            (unsigned long)GJ_USER_CODE_VA, (unsigned long)GJ_USER_STACK_TOP);
    cpu_enter_user(GJ_USER_CODE_VA, GJ_USER_STACK_TOP);
}

int
user_personality_map(struct gj_process *pProc)
{
    size_t cbBlob;
    gj_status_t st;
    u32 cCodePages = 0;

    cbBlob = (size_t)(gj_protonrt_user_blob_end - gj_protonrt_user_blob);
    if (cbBlob == 0 ||
        cbBlob > (size_t)GJ_PERS_CODE_PAGES * (size_t)GJ_PAGE_SIZE) {
        g_cPersMapFail++;
        kprintf("user: bad protonrt blob size %lu\n", (unsigned long)cbBlob);
        return -1;
    }

    /* Soft idempotent: live personality soft recheck. */
    if (g_fPersMapped != 0) {
        if (user_personality_map_soft() == 0) {
            kprintf("user: personality map soft already live recheck PASS\n");
            return 0;
        }
        /* Soft bad on live map: tear down before reinstall (avoid PA leak). */
        user_map_soft_fail_teardown((gj_vaddr_t)GJ_PERS_CODE_VA,
                                    g_cPersCodePages != 0 ? g_cPersCodePages
                                                          : 1u,
                                    GJ_PERS_STACK_TOP, GJ_PERS_STACK_PAGES);
        g_fPersMapped = 0;
        g_u64PersEntry = 0;
        g_u64PersStack = 0;
        g_cbPersBlob = 0;
        g_cPersCodePages = 0;
    }

    if (!user_layout_soft_ok(GJ_PERS_CODE_VA, cbBlob, GJ_PERS_STACK_TOP,
                             GJ_PERS_STACK_PAGES, GJ_USER_CODE_VA, g_cbRing3Blob,
                             GJ_USER_STACK_TOP, GJ_USER_STACK_PAGES,
                             g_fUserMapped)) {
        g_cPersMapFail++;
        g_cPersSoftBad++;
        kprintf("user: personality map soft FAIL layout\n");
        return -1;
    }

    if (pProc != NULL) {
        if (process_as_ensure(pProc) != GJ_OK) {
            g_cPersMapFail++;
            kprintf("user: personality as_ensure failed\n");
            return -1;
        }
        process_as_activate(pProc);
        /* G-PERS: native syscalls for door IPC */
        pProc->u32Personality = 0;
    }

    st = user_install_blob_rx((gj_vaddr_t)GJ_PERS_CODE_VA, gj_protonrt_user_blob,
                              cbBlob, GJ_PERS_CODE_PAGES, &cCodePages);
    if (st != GJ_OK) {
        g_cPersMapFail++;
        kprintf("user: personality map code failed %d\n", (int)st);
        return -1;
    }

    if (user_map_stack(GJ_PERS_STACK_TOP, GJ_PERS_STACK_PAGES) != 0) {
        user_unmap_window((gj_vaddr_t)GJ_PERS_CODE_VA, cCodePages);
        g_cPersMapFail++;
        return -1;
    }

    st = user_map_soft_observe(GJ_PERS_CODE_VA, cbBlob, gj_protonrt_user_blob,
                               GJ_PERS_STACK_TOP, GJ_PERS_STACK_PAGES, pProc,
                               /* native */ 0, "personality", &g_cPersSoft,
                               &g_cPersSoftBad);
    if (st != GJ_OK) {
        user_map_soft_fail_teardown((gj_vaddr_t)GJ_PERS_CODE_VA, cCodePages,
                                    GJ_PERS_STACK_TOP, GJ_PERS_STACK_PAGES);
        g_cPersMapFail++;
        kprintf("user: personality map soft FAIL st=%d\n", (int)st);
        return -1;
    }

    g_fPersMapped = 1;
    g_u64PersEntry = GJ_PERS_CODE_VA;
    g_u64PersStack = GJ_PERS_STACK_TOP;
    g_cbPersBlob = cbBlob;
    g_cPersCodePages = cCodePages;
    g_cPersMapOk++;
    kprintf("user: personality server mapped @0x%lx (%lu bytes) pages=%u "
            "native PASS\n",
            (unsigned long)GJ_PERS_CODE_VA, (unsigned long)cbBlob, cCodePages);
    return 0;
}

u64
user_personality_entry(void)
{
    return g_u64PersEntry;
}

u64
user_personality_stack(void)
{
    return g_u64PersStack;
}

int
user_task_ring3_mapped(void)
{
    return g_fUserMapped != 0 ? 1 : 0;
}

int
user_personality_mapped(void)
{
    return g_fPersMapped != 0 ? 1 : 0;
}

gj_status_t
user_task_stats(struct gj_user_task_stats *pOut)
{
    if (pOut == NULL) {
        return GJ_ERR_INVAL;
    }
    pOut->u32Ring3Ok = g_cRing3MapOk;
    pOut->u32Ring3Fail = g_cRing3MapFail;
    pOut->u32Ring3Soft = g_cRing3Soft;
    pOut->u32Ring3SoftBad = g_cRing3SoftBad;
    pOut->u32PersOk = g_cPersMapOk;
    pOut->u32PersFail = g_cPersMapFail;
    pOut->u32PersSoft = g_cPersSoft;
    pOut->u32PersSoftBad = g_cPersSoftBad;
    pOut->u32EnterOk = g_cEnterOk;
    pOut->u32EnterSkip = g_cEnterSkip;
    pOut->u32CodePages = g_cCodePages;
    pOut->u32StackPages = g_cStackPages;
    pOut->u32Ring3Mapped = g_fUserMapped != 0 ? 1u : 0u;
    pOut->u32PersMapped = g_fPersMapped != 0 ? 1u : 0u;
    return GJ_OK;
}

u32
user_task_stats_soft(void)
{
    /* Grep: user: soft stats */
    kprintf("user: soft stats ring3_ok=%u ring3_fail=%u ring3_soft=%u "
            "ring3_soft_bad=%u pers_ok=%u pers_fail=%u pers_soft=%u "
            "pers_soft_bad=%u enter_ok=%u enter_skip=%u code_pages=%u "
            "stack_pages=%u ring3_live=%u pers_live=%u\n",
            g_cRing3MapOk, g_cRing3MapFail, g_cRing3Soft, g_cRing3SoftBad,
            g_cPersMapOk, g_cPersMapFail, g_cPersSoft, g_cPersSoftBad,
            g_cEnterOk, g_cEnterSkip, g_cCodePages, g_cStackPages,
            g_fUserMapped != 0 ? 1u : 0u, g_fPersMapped != 0 ? 1u : 0u);
    return g_cRing3Soft;
}

int
user_task_ring3_map_soft(void)
{
    gj_status_t st;

    if (g_fUserMapped == 0 || g_cbRing3Blob == 0) {
        kprintf("user: ring3 map soft recheck FAIL not_mapped\n");
        return -1;
    }
    st = user_map_soft_observe(GJ_USER_CODE_VA, g_cbRing3Blob, gj_user_ring3_blob,
                               GJ_USER_STACK_TOP, GJ_USER_STACK_PAGES, NULL,
                               -1, "ring3", &g_cRing3Soft, &g_cRing3SoftBad);
    if (st != GJ_OK) {
        kprintf("user: ring3 map soft recheck FAIL st=%d\n", (int)st);
        return -1;
    }
    kprintf("user: ring3 map soft recheck PASS\n");
    return 0;
}

int
user_personality_map_soft(void)
{
    gj_status_t st;

    if (g_fPersMapped == 0 || g_cbPersBlob == 0) {
        kprintf("user: personality map soft recheck FAIL not_mapped\n");
        return -1;
    }
    if (g_u64PersEntry != GJ_PERS_CODE_VA ||
        g_u64PersStack != GJ_PERS_STACK_TOP) {
        g_cPersSoftBad++;
        kprintf("user: personality map soft recheck FAIL entry=0x%lx "
                "stack=0x%lx\n",
                (unsigned long)g_u64PersEntry, (unsigned long)g_u64PersStack);
        return -1;
    }
    st = user_map_soft_observe(GJ_PERS_CODE_VA, g_cbPersBlob, gj_protonrt_user_blob,
                               GJ_PERS_STACK_TOP, GJ_PERS_STACK_PAGES, NULL, -1,
                               "personality", &g_cPersSoft, &g_cPersSoftBad);
    if (st != GJ_OK) {
        kprintf("user: personality map soft recheck FAIL st=%d\n", (int)st);
        return -1;
    }
    kprintf("user: personality map soft recheck PASS\n");
    return 0;
}
