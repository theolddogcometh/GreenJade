/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * IDT load + gate install. Soft gate observability: exception / int80 /
 * IRQ install counters, last-gate snapshot, present/DPL/type inventory,
 * base-layout soft verify, greppable product logs.
 */
#include <gj/gdt.h>
#include <gj/idt.h>
#include <gj/klog.h>
#include <gj/string.h>
#include <gj/types.h>

struct idt_entry {
    u16 u16OffLo;
    u16 u16Sel;
    u8  u8Ist;
    u8  u8Type;
    u16 u16OffMid;
    u32 u32OffHi;
    u32 u32Zero;
} __attribute__((packed));

struct idt_ptr {
    u16 u16Limit;
    u64 u64Base;
} __attribute__((packed));

static struct idt_entry g_aIdt[256];
static struct idt_ptr   g_IdtPtr;
static int g_fIdtReady;

/* Soft IDT gate observability (monotonic; no locks — BSP/AP soft). */
static volatile u32 g_u32SoftInits;
static volatile u32 g_u32SoftApLoads;
static volatile u32 g_u32SoftInstalls;
static volatile u32 g_u32SoftException;
static volatile u32 g_u32SoftInt80;
static volatile u32 g_u32SoftIrq;
static volatile u32 g_u32SoftReject;
static volatile u32 g_u32SoftVerifyOk;
static volatile u32 g_u32SoftVerifyBad;
static struct gj_idt_gate_soft g_SoftSnap;
static int g_fSoftSnapLive;

/* Generated stubs */
extern void isr_stub_0(void);
extern void isr_stub_1(void);
extern void isr_stub_2(void);
extern void isr_stub_3(void);
extern void isr_stub_4(void);
extern void isr_stub_5(void);
extern void isr_stub_6(void);
extern void isr_stub_7(void);
extern void isr_stub_8(void);
extern void isr_stub_9(void);
extern void isr_stub_10(void);
extern void isr_stub_11(void);
extern void isr_stub_12(void);
extern void isr_stub_13(void);
extern void isr_stub_14(void);
extern void isr_stub_15(void);
extern void isr_stub_16(void);
extern void isr_stub_17(void);
extern void isr_stub_18(void);
extern void isr_stub_19(void);
extern void isr_stub_20(void);
extern void isr_stub_21(void);
extern void isr_stub_22(void);
extern void isr_stub_23(void);
extern void isr_stub_24(void);
extern void isr_stub_25(void);
extern void isr_stub_26(void);
extern void isr_stub_27(void);
extern void isr_stub_28(void);
extern void isr_stub_29(void);
extern void isr_stub_30(void);
extern void isr_stub_31(void);
extern void isr_stub_128(void);

/**
 * Soft: note one gate install — counters + last-gate snapshot.
 * Pure observability; never changes gate semantics.
 */
static void
idt_gate_soft_note(u32 u32Vec, u64 u64Off, u8 u8Type, u8 u8Ist)
{
    u8 u8Dpl;
    u8 u8TypeNibble;

    g_u32SoftInstalls++;
    if (u32Vec < 32u) {
        g_u32SoftException++;
    } else if (u32Vec == 128u) {
        g_u32SoftInt80++;
    } else {
        g_u32SoftIrq++;
    }

    u8Dpl = (u8)((u8Type & GJ_IDT_SOFT_ATTR_DPL_MASK) >>
                 GJ_IDT_SOFT_ATTR_DPL_SHIFT);
    u8TypeNibble = (u8)(u8Type & 0x0fu);

    g_SoftSnap.u32LastVec = u32Vec;
    g_SoftSnap.u64LastOff = u64Off;
    g_SoftSnap.u16LastSel = (u16)GJ_GDT_KERNEL_CS;
    g_SoftSnap.u8LastType = u8Type;
    g_SoftSnap.u8LastIst = u8Ist;
    g_SoftSnap.u8LastDpl = u8Dpl;
    g_SoftSnap.u8LastTrap =
        (u8TypeNibble == GJ_IDT_SOFT_TYPE_TRAP_GATE) ? 1u : 0u;
    g_fSoftSnapLive = 1;
}

/**
 * Soft: walk live IDT and tally present / DPL / type. Updates g_SoftSnap.
 */
static u32
idt_gate_soft_inventory_inner(void)
{
    u32 i;
    u32 u32Present = 0;
    u32 u32Dpl0 = 0;
    u32 u32Dpl3 = 0;
    u32 u32IntStyle = 0;
    u32 u32TrapStyle = 0;

    for (i = 0; i < 256u; i++) {
        u8 u8Type = g_aIdt[i].u8Type;
        u8 u8Dpl;
        u8 u8TypeNibble;

        if ((u8Type & GJ_IDT_SOFT_ATTR_P) == 0) {
            continue;
        }
        u32Present++;
        u8Dpl = (u8)((u8Type & GJ_IDT_SOFT_ATTR_DPL_MASK) >>
                     GJ_IDT_SOFT_ATTR_DPL_SHIFT);
        if (u8Dpl == 0) {
            u32Dpl0++;
        } else if (u8Dpl == 3) {
            u32Dpl3++;
        }
        u8TypeNibble = (u8)(u8Type & 0x0fu);
        if (u8TypeNibble == GJ_IDT_SOFT_TYPE_INT_GATE) {
            u32IntStyle++;
        } else if (u8TypeNibble == GJ_IDT_SOFT_TYPE_TRAP_GATE) {
            u32TrapStyle++;
        }
    }

    g_SoftSnap.u32Present = u32Present;
    g_SoftSnap.u32Dpl0 = u32Dpl0;
    g_SoftSnap.u32Dpl3 = u32Dpl3;
    g_SoftSnap.u32IntStyle = u32IntStyle;
    g_SoftSnap.u32TrapStyle = u32TrapStyle;
    return u32Present;
}

/**
 * Soft: verify base exception + int80 layout against product contract.
 */
static int
idt_gate_soft_verify_inner(void)
{
    u32 i;
    int fOk = 1;
    u64 u64Off;

    if (!g_fIdtReady) {
        g_SoftSnap.u8VerifyOk = 0;
        g_u32SoftVerifyBad++;
        return 0;
    }

    for (i = 0; i < 32u; i++) {
        u8 u8Type = g_aIdt[i].u8Type;
        u8 u8Expect;

        if ((u8Type & GJ_IDT_SOFT_ATTR_P) == 0) {
            fOk = 0;
            break;
        }
        if (g_aIdt[i].u16Sel != (u16)GJ_GDT_KERNEL_CS) {
            fOk = 0;
            break;
        }
        /* #BP (3) and #OF (4): DPL3 trap-style 0xEF */
        if (i == 3u || i == 4u) {
            u8Expect = 0xEFu;
        } else {
            u8Expect = 0x8Eu;
        }
        if (u8Type != u8Expect) {
            fOk = 0;
            break;
        }
        u64Off = ((u64)g_aIdt[i].u32OffHi << 32) |
                 ((u64)g_aIdt[i].u16OffMid << 16) |
                 (u64)g_aIdt[i].u16OffLo;
        if (u64Off == 0) {
            fOk = 0;
            break;
        }
    }

    /* int 0x80 — DPL3 interrupt gate 0xEE */
    if (fOk) {
        if ((g_aIdt[128].u8Type & GJ_IDT_SOFT_ATTR_P) == 0 ||
            g_aIdt[128].u8Type != 0xEEu ||
            g_aIdt[128].u16Sel != (u16)GJ_GDT_KERNEL_CS) {
            fOk = 0;
        } else {
            u64Off = ((u64)g_aIdt[128].u32OffHi << 32) |
                     ((u64)g_aIdt[128].u16OffMid << 16) |
                     (u64)g_aIdt[128].u16OffLo;
            if (u64Off == 0) {
                fOk = 0;
            }
        }
    }

    (void)idt_gate_soft_inventory_inner();
    g_SoftSnap.u8BaseReady = fOk ? 1u : 0u;
    g_SoftSnap.u8VerifyOk = fOk ? 1u : 0u;

    if (fOk) {
        g_u32SoftVerifyOk++;
    } else {
        g_u32SoftVerifyBad++;
    }
    return fOk;
}

static void
idt_set(u32 u32Vec, void *pHandler, u8 u8Type)
{
    u64 u64Off = (u64)(gj_vaddr_t)pHandler;

    g_aIdt[u32Vec].u16OffLo = (u16)(u64Off & 0xffff);
    g_aIdt[u32Vec].u16Sel = (u16)GJ_GDT_KERNEL_CS;
    g_aIdt[u32Vec].u8Ist = 0;
    g_aIdt[u32Vec].u8Type = u8Type;
    g_aIdt[u32Vec].u16OffMid = (u16)((u64Off >> 16) & 0xffff);
    g_aIdt[u32Vec].u32OffHi = (u32)((u64Off >> 32) & 0xffffffffu);
    g_aIdt[u32Vec].u32Zero = 0;

    idt_gate_soft_note(u32Vec, u64Off, u8Type, 0);
}

void
idt_set_gate(u32 u32Vec, void *pHandler, u8 u8Type)
{
    if (u32Vec >= 256 || pHandler == NULL) {
        g_u32SoftReject++;
        return;
    }
    idt_set(u32Vec, pHandler, u8Type);
}

void
idt_init(void)
{
    void *const apfn[] = {
        isr_stub_0,  isr_stub_1,  isr_stub_2,  isr_stub_3,
        isr_stub_4,  isr_stub_5,  isr_stub_6,  isr_stub_7,
        isr_stub_8,  isr_stub_9,  isr_stub_10, isr_stub_11,
        isr_stub_12, isr_stub_13, isr_stub_14, isr_stub_15,
        isr_stub_16, isr_stub_17, isr_stub_18, isr_stub_19,
        isr_stub_20, isr_stub_21, isr_stub_22, isr_stub_23,
        isr_stub_24, isr_stub_25, isr_stub_26, isr_stub_27,
        isr_stub_28, isr_stub_29, isr_stub_30, isr_stub_31,
    };
    u32 i;

    memset(g_aIdt, 0, sizeof(g_aIdt));
    for (i = 0; i < 32; i++) {
        /* 0x8E = present, DPL0, 64-bit interrupt gate */
        /*
         * 0x8E = P|DPL0|64-bit interrupt gate.
         * #BP (3) and #OF (4) get DPL3 trap-style (0xEF) so software
         * INT3/INTO from ring3/compat can deliver cleanly.
         */
        if (i == 3 || i == 4) {
            idt_set(i, apfn[i], 0xEF);
        } else {
            idt_set(i, apfn[i], 0x8E);
        }
    }
    /* int 0x80 — DPL3 interrupt gate for PE32/compat syscalls */
    idt_set(128, isr_stub_128, 0xEE);

    g_IdtPtr.u16Limit = (u16)(sizeof(g_aIdt) - 1);
    g_IdtPtr.u64Base = (u64)(gj_vaddr_t)&g_aIdt[0];
    __asm__ volatile ("lidt %0" : : "m"(g_IdtPtr));
    g_fIdtReady = 1;
    g_u32SoftInits++;

    kprintf("idt: loaded 32 exception gates + int80\n");
    /* Soft: greppable install inventory at base load. */
    kprintf("idt: gate soft program installs=%u exc=%u int80=%u "
            "last vec=%u type=0x%x\n",
            g_u32SoftInstalls, g_u32SoftException, g_u32SoftInt80,
            g_SoftSnap.u32LastVec, (unsigned)g_SoftSnap.u8LastType);

    if (idt_gate_soft_verify_inner()) {
        kprintf("idt: gate soft verify PASS present=%u dpl0=%u dpl3=%u "
                "int=%u trap=%u\n",
                g_SoftSnap.u32Present, g_SoftSnap.u32Dpl0,
                g_SoftSnap.u32Dpl3, g_SoftSnap.u32IntStyle,
                g_SoftSnap.u32TrapStyle);
    } else {
        kprintf("idt: gate soft verify FAIL installs=%u\n",
                g_u32SoftInstalls);
    }
}

int
idt_ready(void)
{
    return g_fIdtReady;
}

void
idt_load_ap(void)
{
    if (!g_fIdtReady) {
        return;
    }
    __asm__ volatile ("lidt %0" : : "m"(g_IdtPtr));
    g_u32SoftApLoads++;
}

/* ---- Soft IDT gate observability API ------------------------------- */

u32
idt_gate_soft_inits(void)
{
    return g_u32SoftInits;
}

u32
idt_gate_soft_ap_loads(void)
{
    return g_u32SoftApLoads;
}

u32
idt_gate_soft_installs(void)
{
    return g_u32SoftInstalls;
}

u32
idt_gate_soft_exception(void)
{
    return g_u32SoftException;
}

u32
idt_gate_soft_int80(void)
{
    return g_u32SoftInt80;
}

u32
idt_gate_soft_irq(void)
{
    return g_u32SoftIrq;
}

u32
idt_gate_soft_reject(void)
{
    return g_u32SoftReject;
}

u32
idt_gate_soft_verify_ok(void)
{
    return g_u32SoftVerifyOk;
}

u32
idt_gate_soft_verify_bad(void)
{
    return g_u32SoftVerifyBad;
}

u32
idt_gate_soft_last_vec(void)
{
    return g_SoftSnap.u32LastVec;
}

u8
idt_gate_soft_last_type(void)
{
    return g_SoftSnap.u8LastType;
}

u64
idt_gate_soft_last_off(void)
{
    return g_SoftSnap.u64LastOff;
}

u32
idt_gate_soft_inventory(void)
{
    return idt_gate_soft_inventory_inner();
}

int
idt_gate_soft_verify(void)
{
    return idt_gate_soft_verify_inner();
}

int
idt_gate_soft_info_get(struct gj_idt_gate_soft *pOut)
{
    if (pOut == NULL) {
        return g_fIdtReady ? 1 : 0;
    }
    memset(pOut, 0, sizeof(*pOut));
    if (!g_fSoftSnapLive && !g_fIdtReady) {
        return 0;
    }
    if (g_fIdtReady) {
        (void)idt_gate_soft_inventory_inner();
    }
    *pOut = g_SoftSnap;
    return g_fIdtReady ? 1 : 0;
}

void
idt_gate_soft_log(void)
{
    /*
     * Greppable soft summary (product / smoke inventory):
     *   idt: gate soft inits=… ap=… install=… exc=… int80=… irq=… reject=…
     *   idt: gate soft present=… dpl0=… dpl3=… int=… trap=… verify_ok=…
     *   idt: gate soft last vec=… type=0x… dpl=… trap=… off=0x… sel=0x…
     */
    if (g_fIdtReady) {
        (void)idt_gate_soft_inventory_inner();
    }
    kprintf("idt: gate soft inits=%u ap=%u install=%u exc=%u int80=%u "
            "irq=%u reject=%u\n",
            g_u32SoftInits, g_u32SoftApLoads, g_u32SoftInstalls,
            g_u32SoftException, g_u32SoftInt80, g_u32SoftIrq,
            g_u32SoftReject);
    kprintf("idt: gate soft present=%u dpl0=%u dpl3=%u int=%u trap=%u "
            "verify_ok=%u verify_bad=%u\n",
            g_SoftSnap.u32Present, g_SoftSnap.u32Dpl0, g_SoftSnap.u32Dpl3,
            g_SoftSnap.u32IntStyle, g_SoftSnap.u32TrapStyle,
            g_u32SoftVerifyOk, g_u32SoftVerifyBad);
    kprintf("idt: gate soft last vec=%u type=0x%x dpl=%u trap=%u "
            "off=0x%lx sel=0x%x\n",
            g_SoftSnap.u32LastVec, (unsigned)g_SoftSnap.u8LastType,
            (unsigned)g_SoftSnap.u8LastDpl, (unsigned)g_SoftSnap.u8LastTrap,
            (unsigned long)g_SoftSnap.u64LastOff,
            (unsigned)g_SoftSnap.u16LastSel);
    if (!g_fIdtReady) {
        kprintf("idt: gate soft idle (IDT not loaded)\n");
    } else if (g_SoftSnap.u8VerifyOk || g_SoftSnap.u8BaseReady) {
        kprintf("idt: gate soft verify PASS\n");
    } else if (g_u32SoftVerifyBad != 0) {
        kprintf("idt: gate soft verify FAIL\n");
    } else {
        kprintf("idt: gate soft armed (no re-verify yet)\n");
    }
}
