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
#include <gj/vmm.h>

/* Keep user windows above kernel identity BSS (historically collided at 0x400000). */
#define GJ_USER_CODE_VA     0x0000000001000000ull
#define GJ_USER_STACK_TOP   0x0000000001100000ull
#define GJ_USER_STACK_PAGES 4u
/* Personality server lives in a separate window so ring3 smoke can coexist */
#define GJ_PERS_CODE_VA     0x0000000001200000ull
#define GJ_PERS_STACK_TOP   0x0000000001300000ull

static u64 g_u64PersEntry;
static u64 g_u64PersStack;

extern char gj_user_ring3_blob[];
extern char gj_user_ring3_blob_end[];
extern char gj_protonrt_user_blob[];
extern char gj_protonrt_user_blob_end[];

static int g_fUserMapped;
static int g_fPersMapped;

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
 * Map stack pages growing down from u64StackTop (RW user).
 * Returns 0 on success, -1 on first map failure.
 */
static int
user_map_stack(u64 u64StackTop, u32 cPages)
{
    u32 iPage;
    gj_status_t st;

    for (iPage = 0; iPage < cPages; iPage++) {
        u64 u64PageVa = u64StackTop - ((u64)iPage + 1ull) * GJ_PAGE_SIZE;

        st = user_install_page((gj_vaddr_t)u64PageVa, NULL, 0,
                               GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE);
        if (st != GJ_OK) {
            kprintf("user: map stack page %u va=0x%lx failed %d\n", iPage,
                    (unsigned long)u64PageVa, (int)st);
            return -1;
        }
    }
    return 0;
}

int
user_task_map_ring3(struct gj_process *pProc)
{
    size_t cbBlob;
    gj_status_t st;

    cbBlob = (size_t)(gj_user_ring3_blob_end - gj_user_ring3_blob);
    if (cbBlob == 0 || cbBlob > GJ_PAGE_SIZE) {
        kprintf("user: bad ring3 blob size %lu\n", (unsigned long)cbBlob);
        return -1;
    }

    if (pProc != NULL) {
        if (process_as_ensure(pProc) != GJ_OK) {
            kprintf("user: as_ensure failed\n");
            return -1;
        }
        process_as_activate(pProc);
    }

    st = user_install_page(GJ_USER_CODE_VA, gj_user_ring3_blob, cbBlob,
                           GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC);
    if (st != GJ_OK) {
        kprintf("user: map code RX failed %d\n", (int)st);
        return -1;
    }

    if (user_map_stack(GJ_USER_STACK_TOP, GJ_USER_STACK_PAGES) != 0) {
        return -1;
    }

    if (pProc != NULL) {
        pProc->u32Personality = 1; /* Linux-shaped trampoline for ABI smokes */
        gj_process_set_jit(pProc, 0);
    }
    g_fUserMapped = 1;
    kprintf("user: ring3 code@0x%lx stacktop@0x%lx blob=%lu PASS\n",
            (unsigned long)GJ_USER_CODE_VA,
            (unsigned long)GJ_USER_STACK_TOP,
            (unsigned long)cbBlob);
    return 0;
}

void
user_task_enter_ring3(void)
{
    if (!g_fUserMapped && !g_fPersMapped) {
        kprintf("user: not mapped\n");
        return;
    }
    if (!cpu_syscall_ready()) {
        kprintf("user: SYSCALL not ready\n");
        return;
    }
    kprintf("user: entering ring3…\n");
    cpu_enter_user(GJ_USER_CODE_VA, GJ_USER_STACK_TOP);
}

int
user_personality_map(struct gj_process *pProc)
{
    size_t cbBlob;
    gj_status_t st;

    cbBlob = (size_t)(gj_protonrt_user_blob_end - gj_protonrt_user_blob);
    if (cbBlob == 0 || cbBlob > GJ_PAGE_SIZE) {
        kprintf("user: bad protonrt blob size %lu\n", (unsigned long)cbBlob);
        return -1;
    }

    if (pProc != NULL) {
        if (process_as_ensure(pProc) != GJ_OK) {
            kprintf("user: personality as_ensure failed\n");
            return -1;
        }
        process_as_activate(pProc);
        /* G-PERS: native syscalls for door IPC */
        pProc->u32Personality = 0;
    }

    st = user_install_page(GJ_PERS_CODE_VA, gj_protonrt_user_blob, cbBlob,
                           GJ_VMM_PROT_READ | GJ_VMM_PROT_EXEC);
    if (st != GJ_OK) {
        kprintf("user: personality map code failed %d\n", (int)st);
        return -1;
    }

    if (user_map_stack(GJ_PERS_STACK_TOP, GJ_USER_STACK_PAGES) != 0) {
        return -1;
    }

    g_fPersMapped = 1;
    g_u64PersEntry = GJ_PERS_CODE_VA;
    g_u64PersStack = GJ_PERS_STACK_TOP;
    kprintf("user: personality server mapped @0x%lx (%lu bytes) native PASS\n",
            (unsigned long)GJ_PERS_CODE_VA, (unsigned long)cbBlob);
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
