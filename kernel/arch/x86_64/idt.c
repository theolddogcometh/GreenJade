/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * IDT load + gate install. Soft IDT inventory (Wave 13 base + Wave 16
 * exclusive complementary deepen): exception / int80 / IRQ install counters,
 * last-gate snapshot, present/DPL/type/layout/contract inventory, base-layout
 * soft verify, vector-band / type-exact / span / key-entry lamps, greppable
 * product logs.
 *
 * Greppable soft inventory (prefix-stable; product / smoke):
 *   idt: soft inventory …
 *   idt: soft present …
 *   idt: soft layout …
 *   idt: soft contract …
 *   idt: soft bands …
 *   idt: soft type …
 *   idt: soft span …
 *   idt: soft vectors …
 *   idt: soft entry …
 *   idt: soft last …
 *   idt: soft stats …
 *   idt: soft path …
 *   idt: soft program …
 *   idt: soft verify PASS|FAIL|idle|armed …
 *   idt: soft PASS|PARTIAL|idle …
 *
 * Wave 15 complementary surfaces (kept; never reshape primary fields):
 *   idt: soft honesty …   — soft-only / non-claim catalog
 *   idt: soft query …     — soft-API / accessor sample tallies
 *   idt: soft expect …    — base product expect catalog
 *   idt: soft ist …       — IST / selector / zero-pad lamps
 *   idt: soft sel …       — kernel-CS contract + mismatch tally
 * Wave 16 complementary surfaces (kept; never reshape primary fields):
 *   idt: soft exclusive … — exclusive=1 unit stamp + wave
 *   idt: soft claim …     — product claim bounds (exc32+int80+gates)
 *   idt: soft ratio …     — install/reject/verify/band path ratios
 *   idt: soft return rate — Wave 19 ok/fail rate lamps
 *   idt: soft retcode    — Wave 19 retcode catalog
 *   idt: soft deepen …    — wave=114 areas stamp
 * Wave 17 complementary surfaces (kept) (never reshape primary fields):
 *   idt: soft return …    — Wave 17 API return surfaces (kept)
 *   idt: soft return selftest … — Wave 17 terminal return surface (kept)
 *   idt: soft retmap …    — Wave 17 return-surface map (kept)
 *
 * Legacy greppable (kept for existing smoke):
 *   idt: gate soft …
 *
 * greppable: idt: soft
 * greppable: idt: soft deepen
 * greppable: idt: soft exclusive
 * greppable: idt: soft return
 * greppable: idt: soft return selftest
 * greppable: idt: soft retmap
 * Pure C11 freestanding; dual-licensed MIT OR Apache-2.0.
 * Soft only: wrap-OK counters + kprintf; never hard-gates product paths.
 * Honesty: soft IDT inventory ≠ product multi-server / bar3 close.
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
static volatile u32 g_u32SoftInvLogs; /* times soft inventory printed */
static volatile u32 g_u32SoftReinstall; /* present gate overwritten */
/* Wave 15 exclusive complementary path tallies (file-local only). */
static volatile u32 g_u32SoftReadyQ;     /* idt_ready samples */
static volatile u32 g_u32SoftInvCall;    /* idt_gate_soft_inventory entries */
static volatile u32 g_u32SoftVerifyCall; /* idt_gate_soft_verify entries */
static volatile u32 g_u32SoftInfoGet;    /* idt_gate_soft_info_get entries */
static volatile u32 g_u32SoftLogCall;    /* idt_gate_soft_log entries */
static volatile u32 g_u32SoftCtrGet;     /* soft counter-accessor samples */
static volatile u32 g_u32SoftLastGet;    /* last-vec/type/off accessor samples */
static volatile u32 g_u32SoftSetGate;    /* idt_set_gate public entries */
static struct gj_idt_gate_soft g_SoftSnap;
static int g_fSoftSnapLive;

/* Soft inventory wave stamp (this unit exclusive deepen). */
#define IDT_SOFT_WAVE 114u

/*
 * Wave 10+ deepen lamps (file-local; refresh with inventory walk).
 * Not part of the public struct — idt.h stays untouched this wave.
 */
static u32 g_u32SoftExcPresent;   /* present among vectors 0..31 */
static u32 g_u32SoftInt80Present; /* vector 128 P bit */
static u32 g_u32SoftIstNonzero;   /* present + IST index != 0 */
static u32 g_u32SoftSelMismatch;  /* present + selector != kernel CS */
static u32 g_u32SoftZeroOff;      /* present + handler offset 0 */
static u32 g_u32SoftDpl1;         /* present DPL1 */
static u32 g_u32SoftDpl2;         /* present DPL2 */
static u32 g_u32SoftBpOk;         /* vec3 = 0xEF contract lamp */
static u32 g_u32SoftOfOk;         /* vec4 = 0xEF contract lamp */
static u32 g_u32SoftInt80Ok;      /* vec128 = 0xEE + CS contract lamp */
static u32 g_u32SoftExcDpl0Int;   /* 0..31 present DPL0 interrupt-style */
static u32 g_u32SoftExcDpl3Trap;  /* 0..31 present DPL3 trap-style */

/*
 * Wave 13 exclusive deepen lamps (file-local; inventory walk only).
 * Bands: present counts by product-meaningful vector ranges.
 * Type-exact: full attribute byte tallies (0x8E / 0xEF / 0xEE / other).
 * Span: first/last present + absent free slots.
 * Key-entry samples: type/off for vec0 / #BP / #OF / #PF / int80.
 */
static u32 g_u32SoftBandExc;      /* present 0..31 */
static u32 g_u32SoftBandPic;      /* present 32..47 (legacy PIC window) */
static u32 g_u32SoftBandLapic;    /* present 48..63 (timer/IPI window) */
static u32 g_u32SoftBandMid;      /* present 64..127 */
static u32 g_u32SoftBandInt80;    /* present 128 (0 or 1) */
static u32 g_u32SoftBandHigh;     /* present 129..255 */
static u32 g_u32SoftAbsent;       /* 256 - present */
static u32 g_u32SoftType8e;       /* present type byte == 0x8E */
static u32 g_u32SoftTypeEf;       /* present type byte == 0xEF */
static u32 g_u32SoftTypeEe;       /* present type byte == 0xEE */
static u32 g_u32SoftTypeOther;    /* present other type bytes */
static u32 g_u32SoftTypeNibbleO;  /* present type nibble neither E nor F */
static u32 g_u32SoftZeroPadNz;    /* present + reserved u32Zero != 0 */
static u32 g_u32SoftFirstPresent; /* lowest present vector (or 256) */
static u32 g_u32SoftLastPresent;  /* highest present vector (or 0) */
static u32 g_u32SoftFirstAbsentExc; /* lowest absent in 0..31 (or 32) */
static u32 g_u32SoftFirstIrqVec;  /* lowest present >=32 except 128 (or 256) */
static u32 g_u32SoftExtraBeyondBase; /* present beyond base 33 (32+int80) */
static u32 g_u32SoftVec0Type;     /* raw type byte at vec 0 (0 if absent) */
static u32 g_u32SoftVec3Type;     /* raw type byte at vec 3 */
static u32 g_u32SoftVec4Type;     /* raw type byte at vec 4 */
static u32 g_u32SoftVec14Type;    /* raw type byte at vec 14 (#PF) */
static u32 g_u32SoftVec128Type;   /* raw type byte at vec 128 */
static u64 g_u64SoftVec0Off;
static u64 g_u64SoftVec3Off;
static u64 g_u64SoftVec4Off;
static u64 g_u64SoftVec14Off;
static u64 g_u64SoftVec128Off;

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

    /* Soft reinstall lamp: overwriting an already-present gate. */
    if (u32Vec < 256u &&
        (g_aIdt[u32Vec].u8Type & GJ_IDT_SOFT_ATTR_P) != 0) {
        g_u32SoftReinstall++;
    }

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
 * Soft: reconstruct handler offset from one live IDT entry.
 */
static u64
idt_soft_entry_off(const struct idt_entry *pEnt)
{
    return ((u64)pEnt->u32OffHi << 32) |
           ((u64)pEnt->u16OffMid << 16) |
           (u64)pEnt->u16OffLo;
}

/**
 * Soft: walk live IDT and tally present / DPL / type / contract / band lamps.
 * Updates g_SoftSnap public fields + Wave 10/13 deepen lamps.
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
    u32 u32ExcPresent = 0;
    u32 u32Int80Present = 0;
    u32 u32IstNonzero = 0;
    u32 u32SelMismatch = 0;
    u32 u32ZeroOff = 0;
    u32 u32Dpl1 = 0;
    u32 u32Dpl2 = 0;
    u32 u32ExcDpl0Int = 0;
    u32 u32ExcDpl3Trap = 0;
    u32 u32BpOk = 0;
    u32 u32OfOk = 0;
    u32 u32Int80Ok = 0;
    /* Wave 13 deepen locals */
    u32 u32BandExc = 0;
    u32 u32BandPic = 0;
    u32 u32BandLapic = 0;
    u32 u32BandMid = 0;
    u32 u32BandInt80 = 0;
    u32 u32BandHigh = 0;
    u32 u32Type8e = 0;
    u32 u32TypeEf = 0;
    u32 u32TypeEe = 0;
    u32 u32TypeOther = 0;
    u32 u32TypeNibbleO = 0;
    u32 u32ZeroPadNz = 0;
    u32 u32FirstPresent = 256u;
    u32 u32LastPresent = 0;
    u32 u32FirstAbsentExc = 32u;
    u32 u32FirstIrqVec = 256u;

    for (i = 0; i < 256u; i++) {
        u8 u8Type = g_aIdt[i].u8Type;
        u8 u8Dpl;
        u8 u8TypeNibble;
        u64 u64Off;

        if ((u8Type & GJ_IDT_SOFT_ATTR_P) == 0) {
            if (i < 32u && u32FirstAbsentExc == 32u) {
                u32FirstAbsentExc = i;
            }
            continue;
        }
        u32Present++;
        if (u32FirstPresent == 256u) {
            u32FirstPresent = i;
        }
        u32LastPresent = i;

        u8Dpl = (u8)((u8Type & GJ_IDT_SOFT_ATTR_DPL_MASK) >>
                     GJ_IDT_SOFT_ATTR_DPL_SHIFT);
        if (u8Dpl == 0) {
            u32Dpl0++;
        } else if (u8Dpl == 1) {
            u32Dpl1++;
        } else if (u8Dpl == 2) {
            u32Dpl2++;
        } else if (u8Dpl == 3) {
            u32Dpl3++;
        }
        u8TypeNibble = (u8)(u8Type & 0x0fu);
        if (u8TypeNibble == GJ_IDT_SOFT_TYPE_INT_GATE) {
            u32IntStyle++;
        } else if (u8TypeNibble == GJ_IDT_SOFT_TYPE_TRAP_GATE) {
            u32TrapStyle++;
        } else {
            u32TypeNibbleO++;
        }
        if (u8Type == 0x8Eu) {
            u32Type8e++;
        } else if (u8Type == 0xEFu) {
            u32TypeEf++;
        } else if (u8Type == 0xEEu) {
            u32TypeEe++;
        } else {
            u32TypeOther++;
        }
        if (g_aIdt[i].u8Ist != 0) {
            u32IstNonzero++;
        }
        if (g_aIdt[i].u16Sel != (u16)GJ_GDT_KERNEL_CS) {
            u32SelMismatch++;
        }
        if (g_aIdt[i].u32Zero != 0) {
            u32ZeroPadNz++;
        }
        u64Off = idt_soft_entry_off(&g_aIdt[i]);
        if (u64Off == 0) {
            u32ZeroOff++;
        }

        /* Vector-range bands (Wave 13). */
        if (i < 32u) {
            u32BandExc++;
            u32ExcPresent++;
            if (u8Dpl == 0 &&
                u8TypeNibble == GJ_IDT_SOFT_TYPE_INT_GATE) {
                u32ExcDpl0Int++;
            }
            if (u8Dpl == 3 &&
                u8TypeNibble == GJ_IDT_SOFT_TYPE_TRAP_GATE) {
                u32ExcDpl3Trap++;
            }
        } else if (i < 48u) {
            u32BandPic++;
            if (u32FirstIrqVec == 256u) {
                u32FirstIrqVec = i;
            }
        } else if (i < 64u) {
            u32BandLapic++;
            if (u32FirstIrqVec == 256u) {
                u32FirstIrqVec = i;
            }
        } else if (i < 128u) {
            u32BandMid++;
            if (u32FirstIrqVec == 256u) {
                u32FirstIrqVec = i;
            }
        } else if (i == 128u) {
            u32BandInt80 = 1;
            u32Int80Present = 1;
        } else {
            u32BandHigh++;
            if (u32FirstIrqVec == 256u) {
                u32FirstIrqVec = i;
            }
        }
    }

    /* Contract lamps (soft only; independent of full verify). */
    if ((g_aIdt[3].u8Type & GJ_IDT_SOFT_ATTR_P) != 0 &&
        g_aIdt[3].u8Type == 0xEFu &&
        g_aIdt[3].u16Sel == (u16)GJ_GDT_KERNEL_CS &&
        idt_soft_entry_off(&g_aIdt[3]) != 0) {
        u32BpOk = 1;
    }
    if ((g_aIdt[4].u8Type & GJ_IDT_SOFT_ATTR_P) != 0 &&
        g_aIdt[4].u8Type == 0xEFu &&
        g_aIdt[4].u16Sel == (u16)GJ_GDT_KERNEL_CS &&
        idt_soft_entry_off(&g_aIdt[4]) != 0) {
        u32OfOk = 1;
    }
    if ((g_aIdt[128].u8Type & GJ_IDT_SOFT_ATTR_P) != 0 &&
        g_aIdt[128].u8Type == 0xEEu &&
        g_aIdt[128].u16Sel == (u16)GJ_GDT_KERNEL_CS &&
        idt_soft_entry_off(&g_aIdt[128]) != 0) {
        u32Int80Ok = 1;
    }

    g_SoftSnap.u32Present = u32Present;
    g_SoftSnap.u32Dpl0 = u32Dpl0;
    g_SoftSnap.u32Dpl3 = u32Dpl3;
    g_SoftSnap.u32IntStyle = u32IntStyle;
    g_SoftSnap.u32TrapStyle = u32TrapStyle;

    g_u32SoftExcPresent = u32ExcPresent;
    g_u32SoftInt80Present = u32Int80Present;
    g_u32SoftIstNonzero = u32IstNonzero;
    g_u32SoftSelMismatch = u32SelMismatch;
    g_u32SoftZeroOff = u32ZeroOff;
    g_u32SoftDpl1 = u32Dpl1;
    g_u32SoftDpl2 = u32Dpl2;
    g_u32SoftBpOk = u32BpOk;
    g_u32SoftOfOk = u32OfOk;
    g_u32SoftInt80Ok = u32Int80Ok;
    g_u32SoftExcDpl0Int = u32ExcDpl0Int;
    g_u32SoftExcDpl3Trap = u32ExcDpl3Trap;

    /* Wave 13 deepen lamps. */
    g_u32SoftBandExc = u32BandExc;
    g_u32SoftBandPic = u32BandPic;
    g_u32SoftBandLapic = u32BandLapic;
    g_u32SoftBandMid = u32BandMid;
    g_u32SoftBandInt80 = u32BandInt80;
    g_u32SoftBandHigh = u32BandHigh;
    g_u32SoftAbsent = 256u - u32Present;
    g_u32SoftType8e = u32Type8e;
    g_u32SoftTypeEf = u32TypeEf;
    g_u32SoftTypeEe = u32TypeEe;
    g_u32SoftTypeOther = u32TypeOther;
    g_u32SoftTypeNibbleO = u32TypeNibbleO;
    g_u32SoftZeroPadNz = u32ZeroPadNz;
    g_u32SoftFirstPresent = u32FirstPresent;
    g_u32SoftLastPresent = u32LastPresent;
    g_u32SoftFirstAbsentExc = u32FirstAbsentExc;
    g_u32SoftFirstIrqVec = u32FirstIrqVec;
    /* Base product layout is 33 present (32 exceptions + int80). */
    g_u32SoftExtraBeyondBase =
        (u32Present > 33u) ? (u32Present - 33u) : 0u;

    /* Key-entry soft samples (type + offset; 0 type when absent). */
    g_u32SoftVec0Type =
        ((g_aIdt[0].u8Type & GJ_IDT_SOFT_ATTR_P) != 0) ?
        (u32)g_aIdt[0].u8Type : 0u;
    g_u32SoftVec3Type =
        ((g_aIdt[3].u8Type & GJ_IDT_SOFT_ATTR_P) != 0) ?
        (u32)g_aIdt[3].u8Type : 0u;
    g_u32SoftVec4Type =
        ((g_aIdt[4].u8Type & GJ_IDT_SOFT_ATTR_P) != 0) ?
        (u32)g_aIdt[4].u8Type : 0u;
    g_u32SoftVec14Type =
        ((g_aIdt[14].u8Type & GJ_IDT_SOFT_ATTR_P) != 0) ?
        (u32)g_aIdt[14].u8Type : 0u;
    g_u32SoftVec128Type =
        ((g_aIdt[128].u8Type & GJ_IDT_SOFT_ATTR_P) != 0) ?
        (u32)g_aIdt[128].u8Type : 0u;
    g_u64SoftVec0Off = idt_soft_entry_off(&g_aIdt[0]);
    g_u64SoftVec3Off = idt_soft_entry_off(&g_aIdt[3]);
    g_u64SoftVec4Off = idt_soft_entry_off(&g_aIdt[4]);
    g_u64SoftVec14Off = idt_soft_entry_off(&g_aIdt[14]);
    g_u64SoftVec128Off = idt_soft_entry_off(&g_aIdt[128]);

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
        u64Off = idt_soft_entry_off(&g_aIdt[i]);
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
            u64Off = idt_soft_entry_off(&g_aIdt[128]);
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

/**
 * Soft IDT inventory — greppable "idt: soft …" lines.
 * Pure observability; never changes gates or aborts boot.
 * Wave 13 primary surfaces kept; Wave 15 adds complementary
 * honesty/query/expect/ist/sel/deepen (prefix-stable).
 *
 * Grep markers:
 *   idt: soft inventory …
 *   idt: soft present …
 *   idt: soft layout …
 *   idt: soft contract …
 *   idt: soft bands …
 *   idt: soft type …
 *   idt: soft span …
 *   idt: soft vectors …
 *   idt: soft entry …
 *   idt: soft last …
 *   idt: soft stats …
 *   idt: soft path …
 *   idt: soft honesty …
 *   idt: soft query …
 *   idt: soft expect …
 *   idt: soft ist …
 *   idt: soft sel …
 *   idt: soft exclusive …
 *   idt: soft claim …
 *   idt: soft ratio …
 *   idt: soft deepen …
 *   idt: soft verify PASS|FAIL|idle|armed …
 *   idt: soft PASS|PARTIAL|idle …
 */
static void
idt_soft_inventory_print(void)
{
    const char *szVerdict;
    u32 u32BaseMatch;
    u32 u32TypeBaseOk;

    if (g_fIdtReady) {
        (void)idt_gate_soft_inventory_inner();
    }
    g_u32SoftInvLogs++;

    /* Soft base expect lamps (Wave 15 complementary; never hard-gate). */
    u32BaseMatch =
        (g_SoftSnap.u32Present >= 33u && g_u32SoftBandExc == 32u &&
         g_u32SoftBandInt80 == 1u && g_u32SoftBpOk && g_u32SoftOfOk &&
         g_u32SoftInt80Ok)
            ? 1u
            : 0u;
    u32TypeBaseOk =
        (g_u32SoftType8e >= 30u && g_u32SoftTypeEf >= 2u &&
         g_u32SoftTypeEe >= 1u)
            ? 1u
            : 0u;

    /* Grep: idt: soft inventory */
    kprintf("idt: soft inventory wave=%u inits=%u ap=%u install=%u "
            "exc=%u int80=%u irq=%u reject=%u reinstall=%u inv_logs=%u\n",
            (unsigned)IDT_SOFT_WAVE, g_u32SoftInits, g_u32SoftApLoads,
            g_u32SoftInstalls, g_u32SoftException, g_u32SoftInt80,
            g_u32SoftIrq, g_u32SoftReject, g_u32SoftReinstall,
            g_u32SoftInvLogs);

    /* Grep: idt: soft present */
    kprintf("idt: soft present total=%u dpl0=%u dpl1=%u dpl2=%u dpl3=%u "
            "int=%u trap=%u exc_p=%u int80_p=%u absent=%u\n",
            g_SoftSnap.u32Present, g_SoftSnap.u32Dpl0, g_u32SoftDpl1,
            g_u32SoftDpl2, g_SoftSnap.u32Dpl3, g_SoftSnap.u32IntStyle,
            g_SoftSnap.u32TrapStyle, g_u32SoftExcPresent,
            g_u32SoftInt80Present, g_u32SoftAbsent);

    /* Grep: idt: soft layout */
    kprintf("idt: soft layout limit=%u base=0x%lx ready=%u slots=256 "
            "ist_nz=%u sel_mis=%u zero_off=%u zero_pad_nz=%u cs=0x%x\n",
            (unsigned)g_IdtPtr.u16Limit,
            (unsigned long)g_IdtPtr.u64Base,
            g_fIdtReady ? 1u : 0u,
            g_u32SoftIstNonzero, g_u32SoftSelMismatch, g_u32SoftZeroOff,
            g_u32SoftZeroPadNz, (unsigned)GJ_GDT_KERNEL_CS);

    /* Grep: idt: soft contract */
    kprintf("idt: soft contract bp=%u of=%u int80=%u base_ready=%u "
            "exc_dpl0_int=%u exc_dpl3_trap=%u verify_ok=%u "
            "verify_bad=%u\n",
            g_u32SoftBpOk, g_u32SoftOfOk, g_u32SoftInt80Ok,
            (unsigned)g_SoftSnap.u8BaseReady, g_u32SoftExcDpl0Int,
            g_u32SoftExcDpl3Trap, g_u32SoftVerifyOk, g_u32SoftVerifyBad);

    /* Grep: idt: soft bands — Wave 13 vector-range present tallies */
    kprintf("idt: soft bands exc=%u pic=%u lapic=%u mid=%u int80=%u "
            "high=%u extra=%u (ranges 0-31/32-47/48-63/64-127/128/"
            "129-255)\n",
            g_u32SoftBandExc, g_u32SoftBandPic, g_u32SoftBandLapic,
            g_u32SoftBandMid, g_u32SoftBandInt80, g_u32SoftBandHigh,
            g_u32SoftExtraBeyondBase);

    /* Grep: idt: soft type — exact attribute-byte tallies */
    kprintf("idt: soft type 8e=%u ef=%u ee=%u other=%u nibble_o=%u "
            "int_style=%u trap_style=%u (expect base 8e=30 ef=2 ee=1)\n",
            g_u32SoftType8e, g_u32SoftTypeEf, g_u32SoftTypeEe,
            g_u32SoftTypeOther, g_u32SoftTypeNibbleO,
            g_SoftSnap.u32IntStyle, g_SoftSnap.u32TrapStyle);

    /* Grep: idt: soft span — first/last present + free */
    kprintf("idt: soft span first_p=%u last_p=%u first_abs_exc=%u "
            "first_irq=%u absent=%u present=%u base_expect=33\n",
            g_u32SoftFirstPresent, g_u32SoftLastPresent,
            g_u32SoftFirstAbsentExc, g_u32SoftFirstIrqVec,
            g_u32SoftAbsent, g_SoftSnap.u32Present);

    /* Grep: idt: soft vectors — key vector contract lamps */
    kprintf("idt: soft vectors v0=0x%x bp3=0x%x of4=0x%x pf14=0x%x "
            "int80=0x%x bp_ok=%u of_ok=%u int80_ok=%u first_irq=%u\n",
            g_u32SoftVec0Type, g_u32SoftVec3Type, g_u32SoftVec4Type,
            g_u32SoftVec14Type, g_u32SoftVec128Type,
            g_u32SoftBpOk, g_u32SoftOfOk, g_u32SoftInt80Ok,
            g_u32SoftFirstIrqVec);

    /* Grep: idt: soft entry — key entry offset samples */
    kprintf("idt: soft entry v0_off=0x%lx bp_off=0x%lx of_off=0x%lx "
            "pf_off=0x%lx int80_off=0x%lx last_off=0x%lx\n",
            (unsigned long)g_u64SoftVec0Off,
            (unsigned long)g_u64SoftVec3Off,
            (unsigned long)g_u64SoftVec4Off,
            (unsigned long)g_u64SoftVec14Off,
            (unsigned long)g_u64SoftVec128Off,
            (unsigned long)g_SoftSnap.u64LastOff);

    /* Grep: idt: soft last */
    kprintf("idt: soft last vec=%u type=0x%x dpl=%u trap=%u ist=%u "
            "off=0x%lx sel=0x%x live=%u\n",
            g_SoftSnap.u32LastVec, (unsigned)g_SoftSnap.u8LastType,
            (unsigned)g_SoftSnap.u8LastDpl, (unsigned)g_SoftSnap.u8LastTrap,
            (unsigned)g_SoftSnap.u8LastIst,
            (unsigned long)g_SoftSnap.u64LastOff,
            (unsigned)g_SoftSnap.u16LastSel,
            g_fSoftSnapLive ? 1u : 0u);

    /* Grep: idt: soft stats — Wave 15 rollup */
    kprintf("idt: soft stats wave=%u inits=%u ap=%u install=%u "
            "reject=%u reinstall=%u inv_logs=%u verify_ok=%u "
            "verify_bad=%u present=%u ready=%u\n",
            (unsigned)IDT_SOFT_WAVE, g_u32SoftInits, g_u32SoftApLoads,
            g_u32SoftInstalls, g_u32SoftReject, g_u32SoftReinstall,
            g_u32SoftInvLogs, g_u32SoftVerifyOk, g_u32SoftVerifyBad,
            g_SoftSnap.u32Present, g_fIdtReady ? 1u : 0u);

    /*
     * Grep: idt: soft path
     * Honesty: soft IDT inventory ≠ product multi-server / bar3 close.
     */
    kprintf("idt: soft path claim=exc32+int80+dynamic_gates "
            "base_verify=1 shared_idt=1 ap_lidt=1 wave=%u "
            "bar3=open (soft inventory; not bar3)\n",
            (unsigned)IDT_SOFT_WAVE);

    /*
     * ---- Wave 15 complementary surfaces (kept; never reshape primary).
     */

    /* Grep: idt: soft honesty */
    kprintf("idt: soft honesty claim=exc32+int80+dynamic_gates "
            "bar3=0 multi_server=0 hard_gate=0 soft_only=1 "
            "shared_idt=1 ap_lidt=1 unit=idt.c wave=%u "
            "(soft inventory; not bar3)\n",
            (unsigned)IDT_SOFT_WAVE);

    /* Grep: idt: soft query — soft-API sample tallies */
    kprintf("idt: soft query ready_q=%u inv_call=%u verify_call=%u "
            "info_get=%u log_call=%u ctr_get=%u last_get=%u "
            "set_gate=%u inv_logs=%u reject=%u reinstall=%u\n",
            g_u32SoftReadyQ, g_u32SoftInvCall, g_u32SoftVerifyCall,
            g_u32SoftInfoGet, g_u32SoftLogCall, g_u32SoftCtrGet,
            g_u32SoftLastGet, g_u32SoftSetGate, g_u32SoftInvLogs,
            g_u32SoftReject, g_u32SoftReinstall);

    /* Grep: idt: soft expect — base product expect catalog */
    kprintf("idt: soft expect slots=256 base_present=33 "
            "exc=32 int80=1 type_8e=30 type_ef=2 type_ee=1 "
            "bp_type=0xef of_type=0xef int80_type=0xee "
            "cs=0x%x base_match=%u type_base_ok=%u wave=%u\n",
            (unsigned)GJ_GDT_KERNEL_CS, u32BaseMatch, u32TypeBaseOk,
            (unsigned)IDT_SOFT_WAVE);

    /* Grep: idt: soft ist — IST / pad / zero-off lamps */
    kprintf("idt: soft ist nonzero=%u zero_off=%u zero_pad_nz=%u "
            "sel_mis=%u first_p=%u last_p=%u first_irq=%u "
            "extra=%u absent=%u product_ist0=1\n",
            g_u32SoftIstNonzero, g_u32SoftZeroOff, g_u32SoftZeroPadNz,
            g_u32SoftSelMismatch, g_u32SoftFirstPresent,
            g_u32SoftLastPresent, g_u32SoftFirstIrqVec,
            g_u32SoftExtraBeyondBase, g_u32SoftAbsent);

    /* Grep: idt: soft sel — kernel-CS contract surface */
    kprintf("idt: soft sel expect_cs=0x%x mismatch=%u "
            "last_sel=0x%x last_vec=%u last_type=0x%x "
            "live=%u ready=%u\n",
            (unsigned)GJ_GDT_KERNEL_CS, g_u32SoftSelMismatch,
            (unsigned)g_SoftSnap.u16LastSel, g_SoftSnap.u32LastVec,
            (unsigned)g_SoftSnap.u8LastType,
            g_fSoftSnapLive ? 1u : 0u, g_fIdtReady ? 1u : 0u);

    /*
     * ---- Wave 16 complementary surfaces (kept; never reshape primary).
     */

    /* Grep: idt: soft exclusive */
    kprintf("idt: soft exclusive wave=%u exclusive=1 soft=1 "
            "unit=idt.c bar3=0 multi_server=0 hard_gate=0 "
            "soft_only=1 shared_idt=1 ap_lidt=1\n",
            (unsigned)IDT_SOFT_WAVE);

    /* Grep: idt: soft claim — product claim bounds */
    kprintf("idt: soft claim exc32=1 int80=1 dynamic_gates=1 "
            "base_present=33 type_8e=30 type_ef=2 type_ee=1 "
            "shared_idt=1 ap_lidt=1 multi_server=0 bar3=0 "
            "hard_gate=0 wave=%u\n",
            (unsigned)IDT_SOFT_WAVE);

    /* Grep: idt: soft ratio — install/reject/verify/band path ratios */
    kprintf("idt: soft ratio install=%u reject=%u reinstall=%u "
            "exc=%u int80=%u irq=%u verify_ok=%u verify_bad=%u "
            "present=%u absent=%u band_exc=%u band_int80=%u "
            "inv_logs=%u wave=%u\n",
            g_u32SoftInstalls, g_u32SoftReject, g_u32SoftReinstall,
            g_u32SoftException, g_u32SoftInt80, g_u32SoftIrq,
            g_u32SoftVerifyOk, g_u32SoftVerifyBad,
            g_SoftSnap.u32Present, g_u32SoftAbsent,
            g_u32SoftBandExc, g_u32SoftBandInt80,
            g_u32SoftInvLogs, (unsigned)IDT_SOFT_WAVE);

    /*
     * Wave 17 complementary sub-lines (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: idt: soft return — Wave 17 API return surfaces (kept) */
    kprintf("idt: soft return ready=%u present=%u gates=1 soft_inv=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u soft PASS\n",
            (unsigned)(g_fIdtReady ? 1u : 0u), (unsigned)g_SoftSnap.u32Present, (unsigned)IDT_SOFT_WAVE);

    /* Grep: idt: soft return selftest — Wave 17 terminal return surface (kept) */
    kprintf("idt: soft return selftest inv_ret=1 product_kernel=OPEN "
            "multi_server=0 bar3=0 wave=%u soft PASS\n",
            (unsigned)IDT_SOFT_WAVE);

    /* Grep: idt: soft retmap — Wave 17 return-surface map (kept) */
    kprintf("idt: soft retmap soft_inv=1 deepen=1 product=OPEN "
            "wave=%u soft PASS\n",
            (unsigned)IDT_SOFT_WAVE);

    /*
     * ---- Wave 18 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: idt: soft return rate — Wave 19 ok/fail rate lamps */
    kprintf("idt: soft return rate soft_inv=1 selftest=1 retmap=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u "
            "(return rate; Soft≠product; not bar3)\n",
            (unsigned)IDT_SOFT_WAVE);

    /* Grep: idt: soft retcode — Wave 19 retcode catalog */
    kprintf("idt: soft retcode ok=1 fail=1 inval=1 busy=1 "
            "selftest=1 retmap=1 product=OPEN soft_ne_product=1 wave=%u "
            "(retcode catalog; Soft≠product)\n",
            (unsigned)IDT_SOFT_WAVE);

    /* Grep: idt: soft deepen — Wave 20 stamp + area catalog */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: idt: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("idt: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)IDT_SOFT_WAVE);
    /* Grep: idt: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("idt: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)IDT_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: idt: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("idt: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)IDT_SOFT_WAVE);
    /* Grep: idt: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("idt: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)IDT_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: idt: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("idt: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)IDT_SOFT_WAVE);
            /* Grep: idt: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("idt: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)IDT_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: idt: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("idt: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)IDT_SOFT_WAVE);
            /* Grep: idt: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("idt: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)IDT_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: idt: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("idt: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)IDT_SOFT_WAVE);
            /* Grep: idt: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("idt: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)IDT_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: idt: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("idt: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)IDT_SOFT_WAVE);
            /* Grep: idt: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("idt: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)IDT_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: idt: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("idt: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)IDT_SOFT_WAVE);
            /* Grep: idt: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("idt: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)IDT_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: idt: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("idt: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)IDT_SOFT_WAVE);
            /* Grep: idt: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("idt: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)IDT_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: idt: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("idt: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)IDT_SOFT_WAVE);
                    /* Grep: idt: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("idt: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)IDT_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: idt: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("idt: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)IDT_SOFT_WAVE);
                            /* Grep: idt: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("idt: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)IDT_SOFT_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: idt: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("idt: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retglyph honesty; Soft≠product; not bar3)\n",
                                    (unsigned)IDT_SOFT_WAVE);
                            /* Grep: idt: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("idt: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retscepter stamp; Soft≠product)\n",
                                    (unsigned)IDT_SOFT_WAVE);
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: idt: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("idt: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)IDT_SOFT_WAVE);
                            /* Grep: idt: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("idt: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retemblem stamp; Soft≠product)\n",
                                    (unsigned)IDT_SOFT_WAVE);
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: idt: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("idt: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retaegis honesty; Soft≠product; not bar3)\n",
                                    (unsigned)IDT_SOFT_WAVE);
                            /* Grep: idt: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("idt: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)IDT_SOFT_WAVE);
                            /* Grep: idt: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("idt: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retmantle stamp; Soft≠product)\n",
                                    (unsigned)IDT_SOFT_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: idt: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("idt: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)IDT_SOFT_WAVE);
/* Grep: idt: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("idt: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)IDT_SOFT_WAVE);
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: idt: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("idt: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)IDT_SOFT_WAVE);
/* Grep: idt: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("idt: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)IDT_SOFT_WAVE);
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: idt: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("idt: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retredoubt honesty; Soft≠product; not bar3)\n",
        (unsigned)IDT_SOFT_WAVE);
/* Grep: idt: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("idt: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retkeep stamp; Soft≠product)\n",
        (unsigned)IDT_SOFT_WAVE);
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: idt: soft retfortress — Wave 35 return-fortress honesty */
kprintf("idt: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retfortress honesty; Soft≠product; not bar3)\n",
        (unsigned)IDT_SOFT_WAVE);
/* Grep: idt: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("idt: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpalace stamp; Soft≠product)\n",
        (unsigned)IDT_SOFT_WAVE);
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: idt: soft rethold — Wave 36 return-hold honesty */
kprintf("idt: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(rethold honesty; Soft≠product; not bar3)\n",
        (unsigned)IDT_SOFT_WAVE);
/* Grep: idt: soft retspire — Wave 36 exclusive spire stamp */
kprintf("idt: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retspire stamp; Soft≠product)\n",
        (unsigned)IDT_SOFT_WAVE);
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: idt: soft retwall — Wave 37 return-wall honesty */
kprintf("idt: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retwall honesty; Soft≠product; not bar3)\n",
        (unsigned)IDT_SOFT_WAVE);
/* Grep: idt: soft retgate — Wave 37 exclusive gate stamp */
kprintf("idt: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retgate stamp; Soft≠product)\n",
        (unsigned)IDT_SOFT_WAVE);
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: idt: soft retmoat — Wave 38 return-moat honesty */
kprintf("idt: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmoat honesty; Soft≠product; not bar3)\n",
        (unsigned)IDT_SOFT_WAVE);
/* Grep: idt: soft retower — Wave 38 exclusive tower stamp */
kprintf("idt: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retower stamp; Soft≠product)\n",
        (unsigned)IDT_SOFT_WAVE);
                            
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: idt: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("idt: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbarbican honesty; Soft≠product; not bar3)\n",
        (unsigned)IDT_SOFT_WAVE);
/* Grep: idt: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("idt: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retglacis stamp; Soft≠product)\n",
        (unsigned)IDT_SOFT_WAVE);
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: idt: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("idt: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retcurtain honesty; Soft≠product; not bar3)\n",
        (unsigned)IDT_SOFT_WAVE);
/* Grep: idt: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("idt: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retparapet stamp; Soft≠product)\n",
        (unsigned)IDT_SOFT_WAVE);
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: idt: soft retravelin — Wave 41 return-travelin honesty */
kprintf("idt: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retravelin honesty; Soft≠product; not bar3)\n",
        (unsigned)IDT_SOFT_WAVE);
/* Grep: idt: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("idt: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retditch stamp; Soft≠product)\n",
        (unsigned)IDT_SOFT_WAVE);
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: idt: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("idt: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retportcullis honesty; Soft≠product; not bar3)\n",
        (unsigned)IDT_SOFT_WAVE);
/* Grep: idt: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("idt: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retbattlement stamp; Soft≠product)\n",
        (unsigned)IDT_SOFT_WAVE);
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: idt: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("idt: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmachicolation honesty; Soft≠product; not bar3)\n",
        (unsigned)IDT_SOFT_WAVE);
/* Grep: idt: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("idt: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retarrowslit stamp; Soft≠product)\n",
        (unsigned)IDT_SOFT_WAVE);

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: idt: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("idt: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmerlon honesty; Soft≠product; not bar3)\n",
        (unsigned)IDT_SOFT_WAVE);
/* Grep: idt: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("idt: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retembrasure stamp; Soft≠product)\n",
        (unsigned)IDT_SOFT_WAVE);

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: idt: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("idt: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retkeepgate honesty; Soft≠product; not bar3)\n",
        (unsigned)IDT_SOFT_WAVE);
/* Grep: idt: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("idt: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retouterward stamp; Soft≠product)\n",
        (unsigned)IDT_SOFT_WAVE);

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: idt: soft retbailey — Wave 46 return-bailey honesty */
kprintf("idt: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbailey honesty; Soft≠product; not bar3)\n",
        (unsigned)IDT_SOFT_WAVE);
/* Grep: idt: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("idt: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpostern stamp; Soft≠product)\n",
        (unsigned)IDT_SOFT_WAVE);

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: idt: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("idt: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=114 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("idt: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=114 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: idt: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("idt: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=114 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("idt: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=114 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: idt: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("idt: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=114 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("idt: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=114 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: idt: soft retsally — Wave 50 return-sally honesty */
kprintf("idt: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=114 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("idt: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=114 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: idt: soft retfosse — Wave 51 return-fosse honesty */
kprintf("idt: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=114 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("idt: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=114 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: idt: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("idt: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=114 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("idt: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=114 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: idt: soft retravelin — Wave 53 return-travelin honesty */
kprintf("idt: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=114 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("idt: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=114 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: idt: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("idt: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=114 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retredan — Wave 54 exclusive redan stamp */
kprintf("idt: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=114 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: idt: soft retflank — Wave 55 return-flank honesty */
kprintf("idt: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=114 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retface — Wave 55 exclusive face stamp */
kprintf("idt: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=114 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: idt: soft retgorge — Wave 56 return-gorge honesty */
kprintf("idt: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=114 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("idt: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=114 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: idt: soft retraverse — Wave 57 return-traverse honesty */
kprintf("idt: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=114 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("idt: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=114 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: idt: soft retorillon — Wave 58 return-orillon honesty */
kprintf("idt: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=114 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("idt: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=114 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: idt: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("idt: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=114 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("idt: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=114 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: idt: soft retplace — Wave 60 return-place honesty */
kprintf("idt: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=114 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("idt: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=114 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: idt: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("idt: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=114 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("idt: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=114 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: idt: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("idt: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=114 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("idt: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=114 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: idt: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("idt: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=114 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("idt: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=114 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: idt: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("idt: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=114 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: idt: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("idt: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=114 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: idt: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("idt: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=114 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: idt: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("idt: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=114 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: idt: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("idt: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=114 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: idt: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("idt: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=114 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: idt: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("idt: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=114 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("idt: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=114 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: idt: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("idt: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=114 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("idt: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=114 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: idt: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("idt: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=114 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("idt: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=114 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: idt: soft retfosseangle — Wave 70 return-fosseangle honesty */
kprintf("idt: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
kprintf("idt: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: idt: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
kprintf("idt: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
kprintf("idt: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: idt: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
kprintf("idt: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
kprintf("idt: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: idt: soft retcurtainface — Wave 73 return-curtainface honesty */
kprintf("idt: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
kprintf("idt: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retbastionangle stamp; Soft≠product)\n");
/* Grep: idt: soft retglacisangle — Wave 74 return-glacisangle honesty */
kprintf("idt: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
kprintf("idt: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retparapetangle stamp; Soft≠product)\n");
/* Grep: idt: soft retmoatangle — Wave 75 return-moatangle honesty */
kprintf("idt: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retowerangle — Wave 75 exclusive towerangle stamp */
kprintf("idt: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retowerangle stamp; Soft≠product)\n");
/* Grep: idt: soft retgateangle — Wave 76 return-gateangle honesty */
kprintf("idt: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retwallangle — Wave 76 exclusive wallangle stamp */
kprintf("idt: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retwallangle stamp; Soft≠product)\n");
/* Grep: idt: soft retspireangle — Wave 77 return-spireangle honesty */
kprintf("idt: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retholdangle — Wave 77 exclusive holdangle stamp */
kprintf("idt: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retholdangle stamp; Soft≠product)\n");
/* Grep: idt: soft retpalaceangle — Wave 78 return-palaceangle honesty */
kprintf("idt: soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retpalaceangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
kprintf("idt: soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retfortressangle stamp; Soft≠product)\n");
/* Grep: idt: soft retkeepangle — Wave 79 return-keepangle honesty */
kprintf("idt: soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retkeepangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
kprintf("idt: soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retredoubtangle stamp; Soft≠product)\n");
/* Grep: idt: soft retcitadelangle — Wave 80 return-citadelangle honesty */
kprintf("idt: soft retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retcitadelangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retbastionkeep — Wave 80 exclusive bastionkeep stamp */
kprintf("idt: soft retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retbastionkeep stamp; Soft≠product)\n");
/* Grep: idt: soft retpanoplyangle — Wave 81 return-panoplyangle honesty */
kprintf("idt: soft retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retpanoplyangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
kprintf("idt: soft retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retbulwarkangle stamp; Soft≠product)\n");
/* Grep: idt: soft retmantleangle — Wave 82 return-mantleangle honesty */
kprintf("idt: soft retmantleangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retmantleangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retaegisangle — Wave 82 exclusive aegisangle stamp */
kprintf("idt: soft retaegisangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retaegisangle stamp; Soft≠product)\n");
/* Grep: idt: soft retemblemangle — Wave 83 return-emblemangle honesty */
kprintf("idt: soft retemblemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retemblemangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retsigilangle — Wave 83 exclusive sigilangle stamp */
kprintf("idt: soft retsigilangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retsigilangle stamp; Soft≠product)\n");
/* Grep: idt: soft retscepterangle — Wave 84 return-scepterangle honesty */
kprintf("idt: soft retscepterangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retscepterangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retglyphangle — Wave 84 exclusive glyphangle stamp */
kprintf("idt: soft retglyphangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retglyphangle stamp; Soft≠product)\n");
/* Grep: idt: soft retcrownangle — Wave 85 return-crownangle honesty */
kprintf("idt: soft retcrownangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retcrownangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retshardangle — Wave 85 exclusive shardangle stamp */
kprintf("idt: soft retshardangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retshardangle stamp; Soft≠product)\n");
/* Grep: idt: soft retforgeangle — Wave 86 return-forgeangle honesty */
kprintf("idt: soft retforgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retforgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retprismangle — Wave 86 exclusive prismangle stamp */
kprintf("idt: soft retprismangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retprismangle stamp; Soft≠product)\n");
/* Grep: idt: soft retflameangle — Wave 87 return-flameangle honesty */
kprintf("idt: soft retflameangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retflameangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retcipherangle — Wave 87 exclusive cipherangle stamp */
kprintf("idt: soft retcipherangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retcipherangle stamp; Soft≠product)\n");
/* Grep: idt: soft retbeaconangle — Wave 88 return-beaconangle honesty */
kprintf("idt: soft retbeaconangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retbeaconangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retledgerangle — Wave 88 exclusive ledgerangle stamp */
kprintf("idt: soft retledgerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retledgerangle stamp; Soft≠product)\n");
/* Grep: idt: soft retbannerangle — Wave 89 return-bannerangle honesty */
kprintf("idt: soft retbannerangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retbannerangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retvaultangle — Wave 89 exclusive vaultangle stamp */
kprintf("idt: soft retvaultangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retvaultangle stamp; Soft≠product)\n");
/* Grep: idt: soft retcrestangle — Wave 90 return-crestangle honesty */
kprintf("idt: soft retcrestangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retcrestangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft rettokenangle — Wave 90 exclusive tokenangle stamp */
kprintf("idt: soft rettokenangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (rettokenangle stamp; Soft≠product)\n");
/* Grep: idt: soft retbadgeangle — Wave 91 return-badgeangle honesty */
kprintf("idt: soft retbadgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retbadgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retphaseangle — Wave 91 exclusive phaseangle stamp */
kprintf("idt: soft retphaseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retphaseangle stamp; Soft≠product)\n");
/* Grep: idt: soft retmarkangle — Wave 92 return-markangle honesty */
kprintf("idt: soft retmarkangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retmarkangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retpulseangle — Wave 92 exclusive pulseangle stamp */
kprintf("idt: soft retpulseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retpulseangle stamp; Soft≠product)\n");

/* Grep: idt: soft retsealangle — Wave 93 return-sealangle honesty */
kprintf("idt: soft retsealangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retsealangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retboundangle — Wave 93 exclusive boundangle stamp */
kprintf("idt: soft retboundangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retboundangle stamp; Soft≠product)\n");
/* Grep: idt: soft retstemangle — Wave 94 return-stemangle honesty */
kprintf("idt: soft retstemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retstemangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retbladeangle — Wave 94 exclusive bladeangle stamp */
kprintf("idt: soft retbladeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retbladeangle stamp; Soft≠product)\n");
/* Grep: idt: soft retchordangle — Wave 95 return-chordangle honesty */
kprintf("idt: soft retchordangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retchordangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retarcangle — Wave 95 exclusive arcangle stamp */
kprintf("idt: soft retarcangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retarcangle stamp; Soft≠product)\n");
/* Grep: idt: soft retsectorangle — Wave 96 return-sectorangle honesty */
kprintf("idt: soft retsectorangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retsectorangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retwedgeangle — Wave 96 exclusive wedgeangle stamp */
kprintf("idt: soft retwedgeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retwedgeangle stamp; Soft≠product)\n");
/* Grep: idt: soft retradiusangle — Wave 97 return-radiusangle honesty */
kprintf("idt: soft retradiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retradiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retdiameterangle — Wave 97 exclusive diameterangle stamp */
kprintf("idt: soft retdiameterangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retdiameterangle stamp; Soft≠product)\n");
/* Grep: idt: soft retcircumangle — Wave 98 return-circumangle honesty */
kprintf("idt: soft retcircumangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retcircumangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retellipseangle — Wave 98 exclusive ellipseangle stamp */
kprintf("idt: soft retellipseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retellipseangle stamp; Soft≠product)\n");
/* Grep: idt: soft rethyperangle — Wave 99 return-hyperangle honesty */
kprintf("idt: soft rethyperangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (rethyperangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retparabolaangle — Wave 99 exclusive parabolaangle stamp */
kprintf("idt: soft retparabolaangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retparabolaangle stamp; Soft≠product)\n");
/* Grep: idt: soft retspiralangle — Wave 100 return-spiralangle honesty */
kprintf("idt: soft retspiralangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retspiralangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft rethelixangle — Wave 100 exclusive helixangle stamp */
kprintf("idt: soft rethelixangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (rethelixangle stamp; Soft≠product)\n");
/* Grep: idt: soft rettorusangle — Wave 101 return-torusangle honesty */
kprintf("idt: soft rettorusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (rettorusangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retknotangle — Wave 101 exclusive knotangle stamp */
kprintf("idt: soft retknotangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retknotangle stamp; Soft≠product)\n");
/* Grep: idt: soft retmoebiusangle — Wave 102 return-moebiusangle honesty */
kprintf("idt: soft retmoebiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retmoebiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retkleinangle — Wave 102 exclusive kleinangle stamp */
kprintf("idt: soft retkleinangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retkleinangle stamp; Soft≠product)\n");
/* Grep: idt: soft retprojectangle — Wave 103 return-projectangle honesty */
kprintf("idt: soft retprojectangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retprojectangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retaffineangle — Wave 103 exclusive affineangle stamp */
kprintf("idt: soft retaffineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retaffineangle stamp; Soft≠product)\n");
/* Grep: idt: soft retlinearangle — Wave 104 return-linearangle honesty */
kprintf("idt: soft retlinearangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retlinearangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retbilinearangle — Wave 104 exclusive bilinearangle stamp */
kprintf("idt: soft retbilinearangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retbilinearangle stamp; Soft≠product)\n");
/* Grep: idt: soft retquadraticangle — Wave 105 return-quadraticangle honesty */
kprintf("idt: soft retquadraticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retquadraticangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retcubicangle — Wave 105 exclusive cubicangle stamp */
kprintf("idt: soft retcubicangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retcubicangle stamp; Soft≠product)\n");
/* Grep: idt: soft retquarticangle — Wave 106 return-quarticangle honesty */
kprintf("idt: soft retquarticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retquarticangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retquinticangle — Wave 106 exclusive quinticangle stamp */
kprintf("idt: soft retquinticangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retquinticangle stamp; Soft≠product)\n");
/* Grep: idt: soft retsplineangle — Wave 107 return-splineangle honesty */
kprintf("idt: soft retsplineangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retsplineangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retbezierangle — Wave 107 exclusive bezierangle stamp */
kprintf("idt: soft retbezierangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retbezierangle stamp; Soft≠product)\n");
/* Grep: idt: soft rethurmitangle — Wave 108 return-hermitangle honesty */
kprintf("idt: soft rethurmitangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (rethurmitangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retcatmullangle — Wave 108 exclusive catmullangle stamp */
kprintf("idt: soft retcatmullangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retcatmullangle stamp; Soft≠product)\n");
/* Grep: idt: soft retnurbsangle — Wave 109 return-nurbsangle honesty */
kprintf("idt: soft retnurbsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retnurbsangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retbsplineangle — Wave 109 exclusive bsplineangle stamp */
kprintf("idt: soft retbsplineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retbsplineangle stamp; Soft≠product)\n");
/* Grep: idt: soft retmeshangle — Wave 110 return-meshangle honesty */
kprintf("idt: soft retmeshangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retmeshangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retgridangle — Wave 110 exclusive gridangle stamp */
kprintf("idt: soft retgridangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retgridangle stamp; Soft≠product)\n");
/* Grep: idt: soft retvoxelangle — Wave 111 return-voxelangle honesty */
kprintf("idt: soft retvoxelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retvoxelangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft rettexelangle — Wave 111 exclusive texelangle stamp */
kprintf("idt: soft rettexelangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (rettexelangle stamp; Soft≠product)\n");
/* Grep: idt: soft retfragmentangle — Wave 112 return-fragmentangle honesty */
kprintf("idt: soft retfragmentangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retfragmentangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retvertexangle — Wave 112 exclusive vertexangle stamp */
kprintf("idt: soft retvertexangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retvertexangle stamp; Soft≠product)\n");
/* Grep: idt: soft retshaderangle — Wave 113 return-shaderangle honesty */
kprintf("idt: soft retshaderangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retshaderangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retpipelineangle — Wave 113 exclusive pipelineangle stamp */
kprintf("idt: soft retpipelineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retpipelineangle stamp; Soft≠product)\n");
/* Grep: idt: soft retframebufferangle — Wave 114 return-framebufferangle honesty */
kprintf("idt: soft retframebufferangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=114 (retframebufferangle honesty; Soft≠product; not bar3)\n");
/* Grep: idt: soft retswapchainangle — Wave 114 exclusive swapchainangle stamp */
kprintf("idt: soft retswapchainangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=114 (retswapchainangle stamp; Soft≠product)\n");
                            kprintf("idt: soft deepen wave=%u areas="
            "inventory,present,layout,contract,bands,type,span,"
            "vectors,entry,last,stats,path,program,verify,"
            "honesty,query,expect,ist,sel,exclusive,claim,ratio,return,return_selftest,retmap,return_rate,retcode "
            "unit=idt.c only hot_irq_kprintf=0\n",
            (unsigned)IDT_SOFT_WAVE);

    if (!g_fIdtReady) {
        szVerdict = "idle";
    } else if (g_SoftSnap.u8VerifyOk || g_SoftSnap.u8BaseReady) {
        szVerdict = "PASS";
    } else if (g_u32SoftVerifyBad != 0) {
        szVerdict = "FAIL";
    } else {
        szVerdict = "armed";
    }

    /* Grep: idt: soft verify */
    kprintf("idt: soft verify %s ok=%u bad=%u present=%u bands_exc=%u "
            "bands_int80=%u type_8e=%u type_ef=%u type_ee=%u wave=%u\n",
            szVerdict, g_u32SoftVerifyOk, g_u32SoftVerifyBad,
            g_SoftSnap.u32Present, g_u32SoftBandExc, g_u32SoftBandInt80,
            g_u32SoftType8e, g_u32SoftTypeEf, g_u32SoftTypeEe,
            (unsigned)IDT_SOFT_WAVE);

    /*
     * Soft identify outcome. Ready + base contract green → soft PASS.
     * Never claims bar3 / product IRQ path complete.
     */
    if (g_fIdtReady && g_u32SoftBpOk && g_u32SoftOfOk &&
        g_u32SoftInt80Ok && g_u32SoftExcPresent == 32u) {
        kprintf("idt: soft PASS wave=%u present=%u type_8e=%u type_ef=%u "
                "type_ee=%u extra=%u base_match=%u\n",
                (unsigned)IDT_SOFT_WAVE, g_SoftSnap.u32Present,
                g_u32SoftType8e, g_u32SoftTypeEf, g_u32SoftTypeEe,
                g_u32SoftExtraBeyondBase, u32BaseMatch);
    } else if (!g_fIdtReady) {
        kprintf("idt: soft idle (IDT not loaded)\n");
    } else {
        kprintf("idt: soft PARTIAL ready=%u exc_p=%u bp=%u of=%u "
                "int80=%u first_abs_exc=%u wave=%u\n",
                g_fIdtReady ? 1u : 0u, g_u32SoftExcPresent,
                g_u32SoftBpOk, g_u32SoftOfOk, g_u32SoftInt80Ok,
                g_u32SoftFirstAbsentExc, (unsigned)IDT_SOFT_WAVE);
    }
}

static void
idt_set(u32 u32Vec, void *pHandler, u8 u8Type)
{
    u64 u64Off = (u64)(gj_vaddr_t)pHandler;

    /* Soft reinstall note before overwrite (reads prior P bit). */
    idt_gate_soft_note(u32Vec, u64Off, u8Type, 0);

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
    g_u32SoftSetGate++;
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

    /* Wave 15: greppable soft program line under idt: soft … */
    kprintf("idt: soft program wave=%u installs=%u exc=%u int80=%u "
            "last vec=%u type=0x%x\n",
            (unsigned)IDT_SOFT_WAVE, g_u32SoftInstalls, g_u32SoftException,
            g_u32SoftInt80, g_SoftSnap.u32LastVec,
            (unsigned)g_SoftSnap.u8LastType);

    /*
     * Legacy greppable (kept for existing smoke greps):
     *   idt: gate soft program …
     */
    kprintf("idt: gate soft program installs=%u exc=%u int80=%u "
            "last vec=%u type=0x%x\n",
            g_u32SoftInstalls, g_u32SoftException, g_u32SoftInt80,
            g_SoftSnap.u32LastVec, (unsigned)g_SoftSnap.u8LastType);

    if (idt_gate_soft_verify_inner()) {
        kprintf("idt: soft verify PASS present=%u dpl0=%u dpl3=%u "
                "int=%u trap=%u type_8e=%u type_ef=%u type_ee=%u\n",
                g_SoftSnap.u32Present, g_SoftSnap.u32Dpl0,
                g_SoftSnap.u32Dpl3, g_SoftSnap.u32IntStyle,
                g_SoftSnap.u32TrapStyle, g_u32SoftType8e,
                g_u32SoftTypeEf, g_u32SoftTypeEe);
        kprintf("idt: gate soft verify PASS present=%u dpl0=%u dpl3=%u "
                "int=%u trap=%u\n",
                g_SoftSnap.u32Present, g_SoftSnap.u32Dpl0,
                g_SoftSnap.u32Dpl3, g_SoftSnap.u32IntStyle,
                g_SoftSnap.u32TrapStyle);
    } else {
        kprintf("idt: soft verify FAIL installs=%u\n",
                g_u32SoftInstalls);
        kprintf("idt: gate soft verify FAIL installs=%u\n",
                g_u32SoftInstalls);
    }

    /* Full Wave 15 soft inventory at base load. */
    idt_soft_inventory_print();
}

int
idt_ready(void)
{
    g_u32SoftReadyQ++;
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
    g_u32SoftCtrGet++;
    return g_u32SoftInits;
}

u32
idt_gate_soft_ap_loads(void)
{
    g_u32SoftCtrGet++;
    return g_u32SoftApLoads;
}

u32
idt_gate_soft_installs(void)
{
    g_u32SoftCtrGet++;
    return g_u32SoftInstalls;
}

u32
idt_gate_soft_exception(void)
{
    g_u32SoftCtrGet++;
    return g_u32SoftException;
}

u32
idt_gate_soft_int80(void)
{
    g_u32SoftCtrGet++;
    return g_u32SoftInt80;
}

u32
idt_gate_soft_irq(void)
{
    g_u32SoftCtrGet++;
    return g_u32SoftIrq;
}

u32
idt_gate_soft_reject(void)
{
    g_u32SoftCtrGet++;
    return g_u32SoftReject;
}

u32
idt_gate_soft_verify_ok(void)
{
    g_u32SoftCtrGet++;
    return g_u32SoftVerifyOk;
}

u32
idt_gate_soft_verify_bad(void)
{
    g_u32SoftCtrGet++;
    return g_u32SoftVerifyBad;
}

u32
idt_gate_soft_last_vec(void)
{
    g_u32SoftLastGet++;
    return g_SoftSnap.u32LastVec;
}

u8
idt_gate_soft_last_type(void)
{
    g_u32SoftLastGet++;
    return g_SoftSnap.u8LastType;
}

u64
idt_gate_soft_last_off(void)
{
    g_u32SoftLastGet++;
    return g_SoftSnap.u64LastOff;
}

u32
idt_gate_soft_inventory(void)
{
    g_u32SoftInvCall++;
    return idt_gate_soft_inventory_inner();
}

int
idt_gate_soft_verify(void)
{
    g_u32SoftVerifyCall++;
    return idt_gate_soft_verify_inner();
}

int
idt_gate_soft_info_get(struct gj_idt_gate_soft *pOut)
{
    g_u32SoftInfoGet++;
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
     * Greppable soft summary (product / smoke inventory).
     * Wave 15 primary prefix: idt: soft …
     * Legacy prefix kept:     idt: gate soft …
     */
    g_u32SoftLogCall++;
    idt_soft_inventory_print();

    /* Legacy greppable lines (existing smoke greps). */
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
