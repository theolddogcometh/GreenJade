/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Internal UDX helpers (not for driver .c).
 *
 * Soft inventory stamps (Wave 14 remaining exclusive deepen -- soft only):
 *   udx: dma soft ...   -- src/dma.c
 *   udx: host soft ...  -- src/host.c  (windows + BAR poke + Cap residual)
 *   udx: mmio soft ...  -- src/mmio.c
 *   udx: pci soft ...   -- src/pci.c
 * Prior exclusive units (not this wave's edit surface):
 *   udx: soft ... / udx: spsc soft ... / udx: notify soft ...
 *
 * ---------------------------------------------------------------------------
 * Lean residual deepen (C2 product path; Soft!=product; G-AC-1; Dual DoD OPEN)
 * ---------------------------------------------------------------------------
 * This header is the freestanding/internal residual spine shared by UDX .c
 * units. Soft residual honesty only — never product close, never .ko AC.
 *
 * Dual DoD residual (both OPEN; Soft!=product; lamps alone != close):
 *   DoD A  xhci_udx    (8086:a12f)  -- window + IRQ badge + DMA residual
 *   DoD B  rtl8168_udx (10ec:8168)  -- window + IRQ badge + iommu_grant
 * Product direction: Linux-shaped userspace UDX hosts over hot+cold ABI +
 * DDI*UDX (product=UDX+ABI). Freestanding kernel class drivers SKIP.
 *
 * Soft API honesty (internal helpers; Cap mint OPEN):
 *   host window register/lookup  != MMIO_FRAME cap mint (window residual)
 *   freestanding DMA/IOMEM pools != product heap / CNode DMA window mint
 *   udx_gj_notify_wait(_poll)    != IRQ Notification cap mint (badge soft)
 *   udx_gj_iommu_grant (op5)     != DMA window CNode mint (soft note only)
 *   udx_gj_syscall* / GJ_SYS_DDI != product multi-server confine close
 *   udx_gj_ddi_syscall*          != product DDI cap install (door residual)
 *   udx_work_flush / should_run  != product NAPI / hard IRQ EOI residual
 *   udx_gj_soft_residual_skip    != product fail closed (SKIP residual only)
 *
 * Cap residual rollup fed by sibling once-lamps (Soft!=product; G-AC-1):
 *   udx: soft residual lean / freestanding class SKIP / product=UDX+ABI
 *   udx: {mmio,dma,notify,host,pci,spsc} soft residual lean + api honesty
 * Freestanding pool constants below feed soft slab/pool inventory lines.
 *
 * ---------------------------------------------------------------------------
 * C2 residual spine deepen (this exclusive unit; Soft!=product; stamp-free)
 * ---------------------------------------------------------------------------
 * Spine residual surfaces (shared by core/dma/host/irq/mmio/pci/work):
 *   window residual  — register/unregister/lookup table only (Cap mint OPEN)
 *   fs_pool residual — static DMA slab + IOMEM slots (host-libc skips pools)
 *   notify residual  — NOTIFY_WAIT which=0 badge poll/wait (Notification OPEN)
 *   badge residual   — bit N → IRQ line N; badge word depth 64 (soft shape)
 *   iommu residual   — PLATFORM_INFO op5 soft note (VT-d / window mint OPEN)
 *   ddi residual     — GJ_SYS_DDI thin wrappers (bind chain in host.c; mint OPEN)
 *   run residual     — should_run + work_flush (not NAPI / not hard-IRQ EOI)
 *   skip residual    — soft-handle unknown/-ENOSYS as residual SKIP
 * Cap residual honesty constants: UDX_GJ_SOFT_CAP_MINT=0 multi_server=0
 * confine=0 Dual DoD A/B OPEN=1. Never flip OPEN from soft residual alone.
 * product=UDX+ABI; freestanding class SKIP; G-AC-1; dual MIT OR Apache-2.0.
 * No version stamp. No stamp storms. Never bump GJ_IMAGE_VERSION here.
 *
 * greppable: udx: soft residual lean
 * greppable: udx: soft residual freestanding class SKIP
 * greppable: udx: soft residual product=UDX+ABI
 * greppable: udx: host soft residual lean
 * greppable: udx: host soft window
 * greppable: udx: host soft lookup
 * greppable: udx: dma soft slab
 * greppable: udx: mmio soft pool
 * greppable: udx: notify soft residual lean
 * greppable: udx: soft residual notify_wait
 * greppable: udx: soft residual iommu_grant
 * greppable: udx: soft residual fs_pool
 * greppable: udx: soft residual window
 * greppable: udx: soft residual badge
 * greppable: udx: soft residual ddi_syscall
 * greppable: udx: soft residual skip
 * greppable: udx: soft residual spine
 * greppable: udx: soft residual cap_mint
 * greppable: udx: soft residual dual_dod
 * greppable: udx: soft ddi bind residual
 * Law: dual MIT OR Apache-2.0; Soft!=product; G-AC-1; Dual DoD A/B OPEN;
 *      no GPL; no Linux .ko product; no version stamp; no stamp storms.
 */
#pragma once

#include <udx/types.h>

/*
 * Soft Cap / Dual DoD residual honesty (spine eng only; Soft!=product).
 * Always-0 mint/server/confine flags match sibling host residual lean.
 * Dual DoD A/B remain OPEN (1) until USB path / interactive SSH login — lamps never close.
 * greppable: udx: soft residual cap_mint / udx: soft residual dual_dod
 * Grep: UDX_GJ_SOFT_CAP_MINT / UDX_GJ_SOFT_DOD_A_OPEN / DOD_B_OPEN
 */
#define UDX_GJ_SOFT_CAP_MINT        0u /* no MMIO/IRQ/DMA cap mint today */
#define UDX_GJ_SOFT_MULTI_SERVER    0u /* soft residual; not multi-server */
#define UDX_GJ_SOFT_CONFINE         0u /* soft residual; not product confine */
#define UDX_GJ_SOFT_DOD_A_OPEN      1u /* xhci_udx Dual DoD A still OPEN */
#define UDX_GJ_SOFT_DOD_B_OPEN      1u /* rtl8168_udx Dual DoD B still OPEN */
/*
 * Product path residual honesty (spine eng only; Soft!=product; G-AC-1).
 * product = userspace UDX/DDI + hot/cold Linux ABI (not freestanding .ko).
 * Dual DoD B residual direction names the rtl8168_udx hop; close is host
 * interactive SSH login. Lamps / accept residual never close Dual DoD.
 * greppable: udx: soft residual product=UDX+ABI
 * greppable: udx: soft residual dual_dod
 * greppable: udx: soft residual sshd_path
 */
#define UDX_GJ_SOFT_PRODUCT_UDX_ABI 1u /* product path = UDX+ABI+DDI */
#define UDX_GJ_SOFT_FS_CLASS_SKIP   1u /* freestanding class drivers SKIP */
#define UDX_GJ_SOFT_G_AC_1          1u /* no Linux .ko product AC */
#define UDX_GJ_SOFT_SSHD_PATH_OPEN  1u /* Dual DoD B sshd hop residual OPEN (login still OPEN) */
#define UDX_GJ_SOFT_NETSTACK_OPEN   1u /* netstack / TCP residual OPEN */

/*
 * Work queue: drain pending items (called from udx_run).
 * Soft residual: SPSC consumer claim only — never product NAPI poll,
 * never hard-IRQ EOI. Product hosts schedule_work from IRQ handlers;
 * flush runs in udx_run context. Soft!=product; spsc residual lean.
 * greppable residual: udx: soft residual spine (run residual)
 */
void udx_work_flush(void);

/*
 * Host MMIO windows: phys → host VA for sim / granted maps.
 * Full GreenJade: filled when process maps MMIO FRAME caps.
 * Also used freestanding when a grant registers a window (DDI MAP_BAR
 * residual → window_register → ioremap lookup).
 *
 * Soft residual (Soft!=product; Cap mint OPEN):
 *   register / unregister / lookup are table residual only.
 *   Never mint or revoke an MMIO_FRAME cap. Product hosts
 *   (rtl8168_udx BAR0+2 / xhci_udx BAR0) install windows on bind PASS.
 * Soft greps: udx: host soft window / udx: host soft lookup
 * greppable residual: udx: soft residual window
 * Cap residual: UDX_GJ_SOFT_CAP_MINT=0 (MMIO_FRAME mint OPEN)
 */
void  udx_host_window_register(u64 u64Phys, void *pVa, u64 u64Len);
void  udx_host_window_unregister(u64 u64Phys);
void *udx_host_window_lookup(u64 u64Phys, u64 u64Len, u64 *pu64OffOut);

/*
 * True while udx_run should keep looping.
 * Soft residual: local stop/init flag residual only — not a product
 * lifecycle cap, not Dual DoD close. Soft!=product.
 * greppable residual: udx: soft residual spine (run residual)
 */
u32 udx_core_should_run(void);

/*
 * Freestanding static pools (no libc heap).
 * Host builds use malloc/calloc instead.
 *
 * Soft residual (Soft!=product; greppable: udx: soft residual fs_pool):
 *   DMA slab  = identity-cookie residual slots (not DMA window CNode mint)
 *   IOMEM pool = ioremap bookkeeping residual (not MMIO_FRAME cap mint)
 * Soft: udx: dma soft slab (DMA_SLOTS/SLOT_CB/BYTES_TOTAL);
 *       udx: mmio soft pool (IOMEM_SLOTS).
 * Product hosts on UDX_HOST_LIBC skip these pools (host soft path).
 * Dual DoD A/B freestanding residual may consume them; Soft!=product.
 * Cap residual: UDX_GJ_SOFT_CAP_MINT=0 (window mint OPEN).
 */
#if !defined(UDX_HOST_LIBC)

/*
 * Two-tier freestanding DMA slab (product UDX NIC/USB; Soft!=product):
 *   small: ring/TRB pages (4 KiB) — rtl8168_udx RING_BYTES, xhci residual
 *   large: RX pool / multi-slot bufs (32 KiB) — RTL_SOFT_RX_POOL_BYTES
 *           (16 × 2 KiB); TX bounce fits small tier
 * DUT .83: product own_handoff SKIP nodma_rx when only 4 KiB slots existed.
 * Soft!=product; identity cookie under VT-d [0,1GiB) identity.
 */
/*
 * Small tier count: Dual DoD B rtl8168_udx Own handoff uses per-slot 4 KiB
 * RX pages (16) + ring (1) + TX bounce (1) + residual ≥ 18. Dual DoD A
 * xhci_udx product scratchpad: DCBAA+ERST+array+need pages (clamp 64
 * after 0.1.150 glass SPAD_CLAMP) + cmd/evt miss — 72 = 5+64+3.
 * rtl8168_udx is a separate ELF. Soft!=product; Dual DoD A OPEN.
 */
#define UDX_FS_DMA_SLOTS           72u
#define UDX_FS_DMA_SLOT_CB         4096u
#define UDX_FS_DMA_LARGE_SLOTS     4u
#define UDX_FS_DMA_LARGE_SLOT_CB   32768u
#define UDX_FS_DMA_BYTES_TOTAL \
    ((UDX_FS_DMA_SLOTS * UDX_FS_DMA_SLOT_CB) + \
     (UDX_FS_DMA_LARGE_SLOTS * UDX_FS_DMA_LARGE_SLOT_CB))
#define UDX_FS_IOMEM_SLOTS         16u

/*
 * Freestanding DMA slot: aBytes page-aligned (4 KiB).
 *
 * Glass v0.1.97 Dual DoD B residual (own_stuck / FOVW / ROK inject=0):
 *   256-byte-only align left bus PA mid-page (e.g. dma_rx0=0x…8900 with
 *   Buffer_Size=0x1000). NIC RX DMA then spanned into the descriptor ring
 *   (dma_ring=0x…9800 inside the 4 KiB RX window) — Own writeback never
 *   visible, FOVW sticky. Page align keeps each 4 KiB DMA in its own
 *   physical page and non-overlapping with ring/slots.
 * TNPDS/RDSAR still need 256-byte align (subset of page align).
 * Soft!=product; identity cookie under VT-d [0,1GiB) identity.
 * greppable: udx: soft residual fs_pool page_align
 */
struct udx_fs_dma_slot {
    u8             aBytes[UDX_FS_DMA_SLOT_CB]
        __attribute__((aligned(4096)));
    u8             u8Used;
    size_t         cbAlloc;
    /* Bus PA after virt_to_phys (0 until resolved). clflush uses VA. */
    udx_dma_addr_t dmaCookie;
} __attribute__((aligned(4096)));

struct udx_fs_dma_large_slot {
    u8             aBytes[UDX_FS_DMA_LARGE_SLOT_CB]
        __attribute__((aligned(4096)));
    u8             u8Used;
    size_t         cbAlloc;
    udx_dma_addr_t dmaCookie;
} __attribute__((aligned(4096)));

#endif /* !UDX_HOST_LIBC */

/*
 * Freestanding GJ syscalls for the driver-host shape.
 * Numbers match kernel GJ_SYS_* (user/libgj/include/gj/syscalls.h).
 * Host (UDX_HOST_LIBC without GJ_FREESTANDING) builds never include these.
 *
 * Soft residual (Soft!=product; G-AC-1; Dual DoD A/B OPEN):
 *   Syscall wrappers are freestanding residual plumbing only.
 *   Soft-handle unknown / -ENOSYS as SKIP (never hard-gate product hosts).
 *   Product multi-server confine + live VT-d mint remain OPEN.
 *   No Linux .ko product AC; dual MIT OR Apache-2.0 only.
 *   Cap residual: multi_server=0 confine=0 (UDX_GJ_SOFT_* constants).
 *
 * SysV x86_64 SYSCALL: nr=rax; a0..a5 → rdi,rsi,rdx,r10,r8,r9.
 * Clobber residual honesty: rcx+r11 (syscall), memory barrier; no FP.
 */
#if !defined(UDX_HOST_LIBC) || defined(GJ_FREESTANDING)

#define UDX_GJ_SYS_PLATFORM_INFO  98
#define UDX_GJ_SYS_NOTIFY_WAIT    99
/*
 * Soft DDI door (GJ_SYS_DDI = 103 after GJ_SYS_SCSI=102).
 * Kernel may soft-stub; UDX bind path soft-handles unknown/-ENOSYS.
 * See include/udx/ddi.h for opcodes and grant shapes.
 *
 * Soft residual bind chain (product hosts; Soft!=product mint OPEN):
 *   SCAN → GET → OPEN → MAP_BAR* → window_register → install → probe
 * Side residual deepen honesty (never Dual DoD close):
 *   CFG_READ / MAP_REMAP / DMA_NOTE / IRQ_BIND
 * greppable: udx: soft ddi bind residual
 * greppable: udx: soft residual ddi_syscall
 * Guard: ddi.h may already define UDX_GJ_SYS_DDI / GJ_SYS_DDI.
 */
#ifndef UDX_GJ_SYS_DDI
#ifndef GJ_SYS_DDI
#define GJ_SYS_DDI 103
#endif
#define UDX_GJ_SYS_DDI GJ_SYS_DDI
#endif

/*
 * PLATFORM_INFO residual ops used by UDX freestanding spine.
 * Full plat catalog lives in libgj syscalls.h; UDX residual consumes:
 *   op5 IOMMU window grant soft note
 *   op6 user VA → PA (product freestanding DMA bus cookie; Soft!=product)
 *   op7 bus3/TE densify for rtl8168 Own-stuck under TE (Soft!=product)
 * Soft!=product mint OPEN.
 * greppable: udx: soft residual iommu_grant
 * greppable: udx: soft residual virt_to_phys
 * greppable: udx: soft residual bus3_te
 */
#define UDX_GJ_PLAT_IOMMU_GRANT   5
#define UDX_GJ_PLAT_VIRT_TO_PHYS  6
#define UDX_GJ_PLAT_BUS3_TE       7
#define UDX_GJ_PLAT_WBINVD        8 /* kernel wbinvd; never ring3 */
#define UDX_GJ_PLAT_TE_DISARM     9 /* TE off dig for Own-stuck */
#define UDX_GJ_PLAT_PHYS_READ32  10 /* kernel read u32 at bus PA */
#define UDX_GJ_PLAT_PANEL_HOLD   11 /* pin STATUS hold line (glass densify) */

/*
 * NOTIFY_WAIT residual shape (matches GJ_NOTIFY_WHICH_MSIX_GLOBAL).
 * which=0 MSI-X global only today; mask=~0 any badge; block=0 poll.
 * Badge word: bit N → IRQ line N for udx_irq_dispatch (N in [0,63]).
 * greppable: udx: soft residual notify_wait / udx: soft residual badge
 * Cap residual: Notification mint OPEN (UDX_GJ_SOFT_CAP_MINT=0).
 */
#define UDX_GJ_NOTIFY_WHICH_MSIX_GLOBAL  0
#define UDX_GJ_NOTIFY_MASK_ALL           ((long)-1)
#define UDX_GJ_NOTIFY_BLOCK_POLL         0
#define UDX_GJ_NOTIFY_BLOCK_WAIT         1
#define UDX_GJ_NOTIFY_BADGE_BITS         64u

/*
 * SYSCALL clobber set (thr=82 DUT .78-.80 class):
 *   Hardware: RCX=return RIP, R11=RFLAGS (always destroyed).
 *   Kernel C on the LSTAR path uses full SysV caller-saved scratch
 *   (rax,rdi,rsi,rdx,rcx,r8,r9,r10,r11) — must all appear here or the
 *   compiler will keep live values across the door (e.g. zero-loop end
 *   pointer in r8). After GET, bind_by_id looped back to the stack zero
 *   with stale r8 → runaway store to map high-water (CR2 slid with stack
 *   guard: top → top+4k → top+2MiB). Soft!=product; Dual DoD OPEN.
 */
#define UDX_GJ_SYSCALL_CLOBBER                                             \
    "rax", "rdi", "rsi", "rdx", "rcx", "r8", "r9", "r10", "r11", "memory", \
        "cc"

static inline long
udx_gj_syscall1(long nNr, long a0)
{
    long ret;

    __asm__ volatile(
        "mov %1, %%rax\n\t"
        "mov %2, %%rdi\n\t"
        "syscall\n\t"
        "mov %%rax, %0"
        : "=r"(ret)
        : "r"(nNr), "r"(a0)
        : UDX_GJ_SYSCALL_CLOBBER);
    return ret;
}

static inline long
udx_gj_syscall2(long nNr, long a0, long a1)
{
    long ret;

    __asm__ volatile(
        "mov %1, %%rax\n\t"
        "mov %2, %%rdi\n\t"
        "mov %3, %%rsi\n\t"
        "syscall\n\t"
        "mov %%rax, %0"
        : "=r"(ret)
        : "r"(nNr), "r"(a0), "r"(a1)
        : UDX_GJ_SYSCALL_CLOBBER);
    return ret;
}

static inline long
udx_gj_syscall3(long nNr, long a0, long a1, long a2)
{
    long ret;

    __asm__ volatile(
        "mov %1, %%rax\n\t"
        "mov %2, %%rdi\n\t"
        "mov %3, %%rsi\n\t"
        "mov %4, %%rdx\n\t"
        "syscall\n\t"
        "mov %%rax, %0"
        : "=r"(ret)
        : "r"(nNr), "r"(a0), "r"(a1), "r"(a2)
        : UDX_GJ_SYSCALL_CLOBBER);
    return ret;
}

static inline long
udx_gj_syscall4(long nNr, long a0, long a1, long a2, long a3)
{
    long ret;

    __asm__ volatile(
        "mov %1, %%rax\n\t"
        "mov %2, %%rdi\n\t"
        "mov %3, %%rsi\n\t"
        "mov %4, %%rdx\n\t"
        "mov %5, %%r10\n\t"
        "syscall\n\t"
        "mov %%rax, %0"
        : "=r"(ret)
        : "r"(nNr), "r"(a0), "r"(a1), "r"(a2), "r"(a3)
        : UDX_GJ_SYSCALL_CLOBBER);
    return ret;
}

/**
 * Soft residual SKIP classifier (freestanding door/syscall residual).
 * Product hosts soft-handle any negative return as residual fail/SKIP;
 * never hard-gates Dual DoD A/B close. Soft!=product; G-AC-1.
 * greppable: udx: soft residual skip
 */
static inline int
udx_gj_soft_residual_skip(long ret)
{
    return (ret < 0) ? 1 : 0;
}

/**
 * Soft DDI door residual wrappers (GJ_SYS_DDI nr).
 * Bind chain residual lives in host.c; DMA_BUF residual in dma.c.
 * Soft table / note only — never product CNode mint. Soft-handle
 * unknown/-ENOSYS via udx_gj_soft_residual_skip. Soft!=product.
 * greppable: udx: soft residual ddi_syscall
 * greppable: udx: soft ddi bind residual
 */
static inline long
udx_gj_ddi_syscall3(long a0, long a1, long a2)
{
    return udx_gj_syscall3(UDX_GJ_SYS_DDI, a0, a1, a2);
}

static inline long
udx_gj_ddi_syscall4(long a0, long a1, long a2, long a3)
{
    return udx_gj_syscall4(UDX_GJ_SYS_DDI, a0, a1, a2, a3);
}

/**
 * NOTIFY_WAIT residual (which / mask / block).
 * Freestanding badge delivery only — never mints an IRQ Notification
 * cap; never issues DDI_OP_IRQ_BIND. Soft!=product; Dual DoD A/B OPEN.
 * greppable: udx: soft residual notify_wait
 * greppable: udx: notify soft residual lean
 */
static inline long
udx_gj_notify_wait(long which, long mask, long block)
{
    return udx_gj_syscall3(UDX_GJ_SYS_NOTIFY_WAIT, which, mask, block);
}

/**
 * Non-blocking NOTIFY_WAIT on MSI-X global (which=0, mask=all, block=0).
 * Badge bit N → IRQ line N for udx_irq_dispatch.
 * Pumped from udx_run only — not from udx_request_irq.
 *
 * Soft residual (Soft!=product; greppable: udx: soft residual notify_wait):
 *   Freestanding badge poll residual only. Never mints an IRQ Notification
 *   cap; never issues DDI_OP_IRQ_BIND (kernel soft badge note is separate).
 *   Product hosts (rtl8168_udx / xhci_udx) stay table+fire until cap_mint.
 *   cap_mint=OPEN; notify soft residual lean; Dual DoD A/B OPEN.
 */
static inline long
udx_gj_notify_wait_poll(void)
{
    return udx_gj_notify_wait(UDX_GJ_NOTIFY_WHICH_MSIX_GLOBAL,
                              UDX_GJ_NOTIFY_MASK_ALL,
                              UDX_GJ_NOTIFY_BLOCK_POLL);
}

/**
 * Soft badge residual: non-zero if badge word has bit u32Bit set.
 * Shape: bit N → IRQ line N (N in [0, UDX_GJ_NOTIFY_BADGE_BITS)).
 * Out-of-range bit is residual miss (0). Soft!=product Notification OPEN.
 * greppable: udx: soft residual badge
 */
static inline int
udx_gj_notify_badge_bit(long badge, u32 u32Bit)
{
    if (u32Bit >= UDX_GJ_NOTIFY_BADGE_BITS) {
        return 0;
    }
    return (((unsigned long)badge >> (unsigned)u32Bit) & 1ul) ? 1 : 0;
}

/**
 * IOMMU window grant: PLATFORM_INFO(op=5, bdf, pa, size).
 *
 * Soft residual (Soft!=product; greppable: udx: soft residual iommu_grant):
 *   Soft PLATFORM_INFO op5 note only — not a DMA window CNode mint, not
 *   live full VT-d product program. dma.c may call on freestanding
 *   product-host shape (rtl8168_udx DoD B ring residual). Soft-handle
 *   unknown/-ENOSYS as residual SKIP (udx_gj_soft_residual_skip).
 *   Product DMA window cap + full VT-d = OPEN.
 *   Cap residual: UDX_GJ_SOFT_CAP_MINT=0.
 *
 * Success under TE (kernel honesty):
 *   Returns soft window count when PA sits in bring-up identity
 *   [0, 1 GiB) with tables ready, or when VT-d is not ready / TE not
 *   armed. Negative (cover_miss) when TE armed + tables ready + PA
 *   outside identity — Own may stick. Soft residual only; never product
 *   DMA window mint. residual: Own stick under TE if identity miss.
 * greppable kernel: PLATFORM_INFO op5 iommu_grant FAIL cover_miss
 */
static inline long
udx_gj_iommu_grant(u32 u32Bdf, udx_dma_addr_t dma, size_t cbSize)
{
    return udx_gj_syscall4(UDX_GJ_SYS_PLATFORM_INFO,
                           UDX_GJ_PLAT_IOMMU_GRANT,
                           (long)u32Bdf,
                           (long)dma,
                           (long)cbSize);
}

/**
 * Bus3/TE densify (PLATFORM_INFO op7): re-run bus3 identity residual;
 * return packed te/hw/ready/bus3/id1g. Soft!=product Dual DoD B.
 * greppable: udx: soft residual bus3_te
 */
static inline long
udx_gj_bus3_te_densify(void)
{
    return udx_gj_syscall1(UDX_GJ_SYS_PLATFORM_INFO, UDX_GJ_PLAT_BUS3_TE);
}

/** Kernel wbinvd (op8). Soft!=product Dual DoD B. */
static inline long
udx_gj_wbinvd(void)
{
    return udx_gj_syscall1(UDX_GJ_SYS_PLATFORM_INFO, UDX_GJ_PLAT_WBINVD);
}

/** TE disarm dig (op9). Soft!=product Dual DoD B. */
static inline long
udx_gj_te_disarm(void)
{
    return udx_gj_syscall1(UDX_GJ_SYS_PLATFORM_INFO, UDX_GJ_PLAT_TE_DISARM);
}

/**
 * Kernel read 32-bit at physical address (op10).
 * Dual DoD B dig: verify Own/cookie in DRAM at programmed bus PA.
 * Returns value as non-negative (low 32 bits) or negative errno.
 * Soft!=product.
 * greppable: udx: soft residual phys_read32
 */
static inline long
udx_gj_phys_read32(udx_dma_addr_t dmaPa)
{
    return udx_gj_syscall2(UDX_GJ_SYS_PLATFORM_INFO, UDX_GJ_PLAT_PHYS_READ32,
                           (long)dmaPa);
}

/**
 * Pin short STATUS hold line (op11). Glass densify survives LOG flood.
 * u32Line: 0..15 (prefer 14 dual DoD B residual / 15 spare).
 * Soft!=product; never Dual DoD close.
 * greppable: udx: soft residual panel_hold
 */
static inline long
udx_gj_panel_hold(u32 u32Line, const char *szText)
{
    u32 cb = 0u;

    if (szText == NULL) {
        return (long)-1;
    }
    while (szText[cb] != '\0' && cb < 95u) {
        cb++;
    }
    return udx_gj_syscall4(UDX_GJ_SYS_PLATFORM_INFO, UDX_GJ_PLAT_PANEL_HOLD,
                           (long)u32Line, (long)(uintptr_t)szText, (long)cb);
}

/**
 * User VA → PA under active CR3 (PLATFORM_INFO op6).
 * Product freestanding DMA bus cookie for rtl8168_udx / xhci residual.
 * Soft!=product: never mints DMA window; pair udx_dma_iommu_grant.
 * Returns PA on success; negative errno-class on miss.
 * greppable: udx: soft residual virt_to_phys
 */
static inline long
udx_gj_virt_to_phys(const void *pCpu)
{
    return udx_gj_syscall3(UDX_GJ_SYS_PLATFORM_INFO,
                           UDX_GJ_PLAT_VIRT_TO_PHYS,
                           (long)(uintptr_t)pCpu,
                           0);
}

#endif /* !UDX_HOST_LIBC || GJ_FREESTANDING */
