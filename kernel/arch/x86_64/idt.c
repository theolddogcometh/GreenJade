/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
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
}

void
idt_set_gate(u32 u32Vec, void *pHandler, u8 u8Type)
{
    if (u32Vec >= 256 || pHandler == NULL) {
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
    kprintf("idt: loaded 32 exception gates + int80\n");
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
}
