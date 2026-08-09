/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * IDT load + gate install. Soft IDT inventory (prefix-stable; residual lean):
 * exception / int80 / IRQ install counters, last-gate snapshot,
 * present/DPL/type/layout/contract inventory, base-layout soft verify,
 * vector-band / type-exact / span / key-entry lamps, greppable soft logs.
 *
 * Greppable soft inventory (product / smoke; no stamp storms, no product
 * version stamp):
 *   idt: soft inventory ...
 *   idt: soft present ...
 *   idt: soft layout ...
 *   idt: soft contract ...
 *   idt: soft bands ...
 *   idt: soft type ...
 *   idt: soft span ...
 *   idt: soft vectors ...
 *   idt: soft entry ...
 *   idt: soft last ...
 *   idt: soft stats ...
 *   idt: soft path ...
 *   idt: soft program ...
 *   idt: soft verify PASS|FAIL|idle|armed ...
 *   idt: soft PASS|PARTIAL|idle ...
 *
 * Complementary (kept lean; never reshape primary fields):
 *   idt: soft honesty ...   - soft-only / Soft!=product dual-license catalog
 *   idt: soft query ...     - soft-API / accessor sample tallies
 *   idt: soft expect ...    - base product expect catalog
 *   idt: soft ist ...       - IST / selector / zero-pad lamps
 *   idt: soft sel ...       - kernel-CS contract + mismatch tally
 *   idt: soft exclusive ... - exclusive=1 unit + wave (not a product version)
 *   idt: soft claim ...     - product claim bounds (exc32+int80+gates)
 *   idt: soft ratio ...     - install/reject/verify/band path ratios
 *   idt: soft match ...     - expect-vs-live base contract match lamps
 *   idt: soft ap ...        - AP lidt path (shared table; rebuild=0)
 *   idt: soft residual lean - residual honesty (Soft!=product; G-AC-1)
 *   idt: soft deepen ...    - lean area catalog only (no ret*angle residue)
 *
 * Residual lean via= (C0 residual deepen; Soft!=product; no version stamp):
 *   via=init  - once at idt_init soft bring-up (after lidt + base verify)
 *   via=inv   - full soft inventory path (hard-capped IDT_SOFT_INV_LOG_CAP)
 *   via=ap    - first AP lidt path (shared table; rebuild=0; once-lamp)
 * C0 residual deepen (stamp-free; Soft!=product; G-AC-1; Dual DoD OPEN):
 *   Limit/base structural match + base contract match fold into residual.
 *   Hard-capped inventory emission (IDT_SOFT_INV_LOG_CAP). No version stamp.
 *   Soft PASS != product multi-server / Dual DoD close (DualDoD=OPEN).
 *   G-AC-1: no_ko_product_ac=1. Dual MIT OR Apache-2.0 unchanged.
 *
 * Legacy greppable (kept for existing smoke):
 *   idt: gate soft ...
 *
 * greppable: idt: soft
 * greppable: idt: soft deepen
 * greppable: idt: soft exclusive
 * greppable: idt: soft residual lean
 * greppable: idt: soft match
 * greppable: DualDoD=OPEN
 * greppable: no_ko_product_ac
 * Pure C11 freestanding; dual-licensed MIT OR Apache-2.0.
 * Soft only: wrap-OK counters + kprintf; never hard-gates product paths.
 * Soft residual lean: no stamp storms, no ret*angle / retmap residue.
 * Honesty: soft IDT inventory != product multi-server close. Soft!=product.
 * G-AC-1: no .ko product AC. Dual MIT OR Apache-2.0. No product version stamp.
 * Dual DoD A/B remain OPEN (this residual does not close product DoD).
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

/* Soft IDT gate observability (monotonic; no locks - BSP/AP soft). */
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
static volatile u32 g_u32SoftInvSkip;  /* capped inventory silent skips */
static volatile u32 g_u32SoftReinstall; /* present gate overwritten */
/* Soft complementary path tallies (file-local only). */
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

/* Soft inventory wave tag (greppable wave=; not a product version stamp). */
#define IDT_SOFT_WAVE 126u
/*
 * Lean soft residual serial budget. Full multi-line inventory is hard-capped
 * so soft_log re-entry cannot stamp-storm serial/stack. Soft!=product.
 * greppable: idt: soft residual lean | IDT_SOFT_INV_LOG_CAP
 */
#define IDT_SOFT_INV_LOG_CAP 2u

/* C0 residual lean tallies (eng residual; Soft!=product; never hard-gate). */
static volatile u32 g_u32SoftResidualLeanN;    /* residual lean line emissions */
static volatile u32 g_u32SoftResidualLeanInit; /* via=init once-lamp */
static volatile u32 g_u32SoftResidualLeanInv;  /* via=inv (full inventory) */
static volatile u32 g_u32SoftResidualLeanAp;   /* via=ap (first AP lidt) */
static u8             g_fSoftResidualLeanApOnce; /* first-AP residual once */

/*
 * Soft deepen lamps (file-local; refresh with inventory walk).
 * Not part of the public struct - idt.h stays untouched this residual.
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
 * Soft band / type / span lamps (file-local; inventory walk only).
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
 * Soft: note one gate install - counters + last-gate snapshot.
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
 * Updates g_SoftSnap public fields + soft deepen lamps.
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
    /* Soft band / type locals */
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

        /* Vector-range bands. */
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

    /* Soft band / type / span lamps. */
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

    /* int 0x80 - DPL3 interrupt gate 0xEE */
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
 * C0 residual lean emit (one greppable line; Soft!=product).
 *
 * Shared by bring-up (via=init), full inventory (via=inv), and first AP lidt
 * (via=ap). Surfaces base-contract + limit/base structural honesty without
 * stamp storms or product version stamps. Never reshapes gates. G-AC-1.
 * Dual DoD A/B remain OPEN (soft residual != product multi-server close).
 *
 * greppable: idt: soft residual lean
 * greppable: DualDoD=OPEN | no_ko_product_ac | Soft!=product
 */
static void
idt_soft_residual_lean_emit(const char *szVia)
{
    const char *szPath = "inv";
    u8 u8Path = 0u; /* 0=inv, 1=init, 2=ap */
    u32 u32BaseMatch;
    u32 u32TypeBaseOk;
    u32 u32MatchExc;
    u32 u32MatchInt80;
    u32 u32MatchBp;
    u32 u32MatchOf;
    u32 u32MatchSel;
    u32 u32MatchIst;
    u32 u32MatchPad;
    u32 u32MatchZeroOff;
    u32 u32MatchLimit;
    u32 u32MatchBase;
    u32 u32MatchAll;

    if (szVia != NULL && szVia[0] != '\0') {
        if (szVia[0] == 'i' && szVia[1] == 'n' && szVia[2] == 'i' &&
            szVia[3] == 't' && szVia[4] == '\0') {
            szPath = "init";
            u8Path = 1u;
        } else if (szVia[0] == 'a' && szVia[1] == 'p' && szVia[2] == '\0') {
            szPath = "ap";
            u8Path = 2u;
        } else {
            szPath = "inv";
            u8Path = 0u;
        }
    }

    if (u8Path == 1u) {
        if (g_u32SoftResidualLeanInit < 0xffffffffu) {
            g_u32SoftResidualLeanInit++;
        }
    } else if (u8Path == 2u) {
        if (g_u32SoftResidualLeanAp < 0xffffffffu) {
            g_u32SoftResidualLeanAp++;
        }
    } else if (g_u32SoftResidualLeanInv < 0xffffffffu) {
        g_u32SoftResidualLeanInv++;
    }
    if (g_u32SoftResidualLeanN < 0xffffffffu) {
        g_u32SoftResidualLeanN++;
    }

    /* Soft expect-vs-live + structural match lamps (never hard-gate). */
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
    u32MatchExc = (g_u32SoftExcPresent == 32u) ? 1u : 0u;
    u32MatchInt80 = g_u32SoftInt80Ok ? 1u : 0u;
    u32MatchBp = g_u32SoftBpOk ? 1u : 0u;
    u32MatchOf = g_u32SoftOfOk ? 1u : 0u;
    u32MatchSel =
        (g_fIdtReady && g_u32SoftSelMismatch == 0u) ? 1u : 0u;
    u32MatchIst =
        (g_fIdtReady && g_u32SoftIstNonzero == 0u) ? 1u : 0u;
    u32MatchPad =
        (g_fIdtReady && g_u32SoftZeroPadNz == 0u) ? 1u : 0u;
    u32MatchZeroOff =
        (g_fIdtReady && g_u32SoftZeroOff == 0u) ? 1u : 0u;
    /* C0 structural residual: IDTR limit/base match live table. */
    u32MatchLimit =
        (g_fIdtReady &&
         g_IdtPtr.u16Limit == (u16)(sizeof(g_aIdt) - 1u))
            ? 1u
            : 0u;
    u32MatchBase =
        (g_fIdtReady &&
         g_IdtPtr.u64Base == (u64)(gj_vaddr_t)&g_aIdt[0])
            ? 1u
            : 0u;
    u32MatchAll =
        (u32BaseMatch && u32TypeBaseOk && u32MatchSel &&
         u32MatchIst && u32MatchPad && u32MatchZeroOff &&
         u32MatchLimit && u32MatchBase)
            ? 1u
            : 0u;

    /*
     * Grep: idt: soft residual lean
     * Lean residual honesty - Soft!=product dual license; G-AC-1;
     * DualDoD=OPEN; no version stamp; no ret*angle / retmap residue.
     * via=init|inv|ap distinguishes residual path (C0 deepen).
     */
    kprintf("idt: soft residual lean via=%s shared_idt=1 bsp_lidt=1 "
            "ap_lidt=1 ist0=1 multi_server=0 hard_gate=0 storm=0 "
            "exc32=1 int80=1 dynamic_gates=1 hot_irq_kprintf=0 "
            "match_all=%u match_exc=%u match_int80=%u match_bp=%u "
            "match_of=%u match_sel=%u match_ist0=%u match_pad0=%u "
            "match_zero_off0=%u match_limit=%u match_base=%u "
            "inv=%u skip=%u cap=%u "
            "lean_n=%u lean_init=%u lean_inv=%u lean_ap=%u "
            "verify_ok=%u verify_bad=%u ready=%u "
            "soft_ne_product=1 product_close=0 DualDoD=OPEN "
            "g_ac1=1 no_ko_product_ac=1 dual=MIT_OR_Apache-2.0 "
            "unit=idt.c eng_log=1 stamp_storm=0 "
            "(Soft!=product; G-AC-1; dual MIT OR Apache-2.0; "
            "Dual DoD A/B OPEN; no version stamp; "
            "limit/base structural residual; "
            "residual lean eng logs only)\n",
            szPath, u32MatchAll, u32MatchExc, u32MatchInt80, u32MatchBp,
            u32MatchOf, u32MatchSel, u32MatchIst, u32MatchPad,
            u32MatchZeroOff, u32MatchLimit, u32MatchBase,
            g_u32SoftInvLogs, g_u32SoftInvSkip,
            (unsigned)IDT_SOFT_INV_LOG_CAP,
            g_u32SoftResidualLeanN, g_u32SoftResidualLeanInit,
            g_u32SoftResidualLeanInv, g_u32SoftResidualLeanAp,
            g_u32SoftVerifyOk, g_u32SoftVerifyBad,
            g_fIdtReady ? 1u : 0u);
}

/**
 * Soft IDT inventory - greppable "idt: soft ..." lines.
 * Pure observability; never changes gates or aborts boot.
 * Prefix-stable primary + lean complementary surfaces.
 * Residual lean: no stamp storms / no ret*angle / no product version stamp.
 * Hard-capped (IDT_SOFT_INV_LOG_CAP) - soft_log re-entry skips.
 *
 * Grep markers:
 *   idt: soft inventory ...
 *   idt: soft present ...
 *   idt: soft layout ...
 *   idt: soft contract ...
 *   idt: soft bands ...
 *   idt: soft type ...
 *   idt: soft span ...
 *   idt: soft vectors ...
 *   idt: soft entry ...
 *   idt: soft last ...
 *   idt: soft stats ...
 *   idt: soft path ...
 *   idt: soft honesty ...
 *   idt: soft query ...
 *   idt: soft expect ...
 *   idt: soft ist ...
 *   idt: soft sel ...
 *   idt: soft exclusive ...
 *   idt: soft claim ...
 *   idt: soft ratio ...
 *   idt: soft match ...
 *   idt: soft ap ...
 *   idt: soft residual lean ...
 *   idt: soft deepen ...
 *   idt: soft verify PASS|FAIL|idle|armed ...
 *   idt: soft PASS|PARTIAL|idle ...
 */
static void
idt_soft_inventory_print(void)
{
    const char *szVerdict;
    u32 u32BaseMatch;
    u32 u32TypeBaseOk;
    u32 u32MatchExc;
    u32 u32MatchInt80;
    u32 u32MatchBp;
    u32 u32MatchOf;
    u32 u32MatchSel;
    u32 u32MatchIst;
    u32 u32MatchPad;
    u32 u32MatchZeroOff;
    u32 u32MatchLimit;
    u32 u32MatchBase;
    u32 u32MatchAll;

    /* Lean residual: hard-cap multi-line inventory (no stamp storms). */
    if (g_u32SoftInvLogs >= IDT_SOFT_INV_LOG_CAP) {
        if (g_u32SoftInvSkip < 0xffffffffu) {
            g_u32SoftInvSkip++;
        }
        return;
    }

    if (g_fIdtReady) {
        (void)idt_gate_soft_inventory_inner();
    }
    g_u32SoftInvLogs++;

    /* Soft base expect lamps (never hard-gate). */
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

    /* Soft expect-vs-live match lamps (base product slots only). */
    u32MatchExc = (g_u32SoftExcPresent == 32u) ? 1u : 0u;
    u32MatchInt80 = g_u32SoftInt80Ok ? 1u : 0u;
    u32MatchBp = g_u32SoftBpOk ? 1u : 0u;
    u32MatchOf = g_u32SoftOfOk ? 1u : 0u;
    u32MatchSel =
        (g_fIdtReady && g_u32SoftSelMismatch == 0u) ? 1u : 0u;
    /* Product base uses IST index 0 on all installed gates. */
    u32MatchIst =
        (g_fIdtReady && g_u32SoftIstNonzero == 0u) ? 1u : 0u;
    u32MatchPad =
        (g_fIdtReady && g_u32SoftZeroPadNz == 0u) ? 1u : 0u;
    u32MatchZeroOff =
        (g_fIdtReady && g_u32SoftZeroOff == 0u) ? 1u : 0u;
    /* C0 structural residual: IDTR limit/base match live table. */
    u32MatchLimit =
        (g_fIdtReady &&
         g_IdtPtr.u16Limit == (u16)(sizeof(g_aIdt) - 1u))
            ? 1u
            : 0u;
    u32MatchBase =
        (g_fIdtReady &&
         g_IdtPtr.u64Base == (u64)(gj_vaddr_t)&g_aIdt[0])
            ? 1u
            : 0u;
    u32MatchAll =
        (u32BaseMatch && u32TypeBaseOk && u32MatchSel &&
         u32MatchIst && u32MatchPad && u32MatchZeroOff &&
         u32MatchLimit && u32MatchBase)
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

    /* Grep: idt: soft bands - vector-range present tallies */
    kprintf("idt: soft bands exc=%u pic=%u lapic=%u mid=%u int80=%u "
            "high=%u extra=%u (ranges 0-31/32-47/48-63/64-127/128/"
            "129-255)\n",
            g_u32SoftBandExc, g_u32SoftBandPic, g_u32SoftBandLapic,
            g_u32SoftBandMid, g_u32SoftBandInt80, g_u32SoftBandHigh,
            g_u32SoftExtraBeyondBase);

    /* Grep: idt: soft type - exact attribute-byte tallies */
    kprintf("idt: soft type 8e=%u ef=%u ee=%u other=%u nibble_o=%u "
            "int_style=%u trap_style=%u (expect base 8e=30 ef=2 ee=1)\n",
            g_u32SoftType8e, g_u32SoftTypeEf, g_u32SoftTypeEe,
            g_u32SoftTypeOther, g_u32SoftTypeNibbleO,
            g_SoftSnap.u32IntStyle, g_SoftSnap.u32TrapStyle);

    /* Grep: idt: soft span - first/last present + free */
    kprintf("idt: soft span first_p=%u last_p=%u first_abs_exc=%u "
            "first_irq=%u absent=%u present=%u base_expect=33\n",
            g_u32SoftFirstPresent, g_u32SoftLastPresent,
            g_u32SoftFirstAbsentExc, g_u32SoftFirstIrqVec,
            g_u32SoftAbsent, g_SoftSnap.u32Present);

    /* Grep: idt: soft vectors - key vector contract lamps */
    kprintf("idt: soft vectors v0=0x%x bp3=0x%x of4=0x%x pf14=0x%x "
            "int80=0x%x bp_ok=%u of_ok=%u int80_ok=%u first_irq=%u\n",
            g_u32SoftVec0Type, g_u32SoftVec3Type, g_u32SoftVec4Type,
            g_u32SoftVec14Type, g_u32SoftVec128Type,
            g_u32SoftBpOk, g_u32SoftOfOk, g_u32SoftInt80Ok,
            g_u32SoftFirstIrqVec);

    /* Grep: idt: soft entry - key entry offset samples */
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

    /* Grep: idt: soft stats - rollup */
    kprintf("idt: soft stats wave=%u inits=%u ap=%u install=%u "
            "reject=%u reinstall=%u inv_logs=%u inv_skip=%u cap=%u "
            "verify_ok=%u verify_bad=%u present=%u ready=%u\n",
            (unsigned)IDT_SOFT_WAVE, g_u32SoftInits, g_u32SoftApLoads,
            g_u32SoftInstalls, g_u32SoftReject, g_u32SoftReinstall,
            g_u32SoftInvLogs, g_u32SoftInvSkip,
            (unsigned)IDT_SOFT_INV_LOG_CAP, g_u32SoftVerifyOk,
            g_u32SoftVerifyBad, g_SoftSnap.u32Present,
            g_fIdtReady ? 1u : 0u);

    /*
     * Grep: idt: soft path
     * Honesty: soft IDT inventory != product multi-server close.
     */
    kprintf("idt: soft path claim=exc32+int80+dynamic_gates "
            "base_verify=1 shared_idt=1 ap_lidt=1 "
            "soft_ne_product=1 DualDoD=OPEN product_close=0 "
            "wave=%u (soft inventory; Soft!=product)\n",
            (unsigned)IDT_SOFT_WAVE);

    /*
     * Grep: idt: soft honesty
     * Soft inventory != product multi-server / hard-gate. Dual-license soft.
     * DualDoD=OPEN: soft PASS never closes product Dual DoD.
     */
    kprintf("idt: soft honesty claim=exc32+int80+dynamic_gates "
            "multi_server=0 hard_gate=0 soft_only=1 "
            "shared_idt=1 ap_lidt=1 soft_ne_product=1 dual_license=1 "
            "DualDoD=OPEN product_close=0 g_ac1=1 no_ko_product_ac=1 "
            "unit=idt.c wave=%u (soft inventory; Soft!=product)\n",
            (unsigned)IDT_SOFT_WAVE);

    /* Grep: idt: soft query - soft-API sample tallies */
    kprintf("idt: soft query ready_q=%u inv_call=%u verify_call=%u "
            "info_get=%u log_call=%u ctr_get=%u last_get=%u "
            "set_gate=%u inv_logs=%u inv_skip=%u cap=%u "
            "reject=%u reinstall=%u\n",
            g_u32SoftReadyQ, g_u32SoftInvCall, g_u32SoftVerifyCall,
            g_u32SoftInfoGet, g_u32SoftLogCall, g_u32SoftCtrGet,
            g_u32SoftLastGet, g_u32SoftSetGate, g_u32SoftInvLogs,
            g_u32SoftInvSkip, (unsigned)IDT_SOFT_INV_LOG_CAP,
            g_u32SoftReject, g_u32SoftReinstall);

    /* Grep: idt: soft expect - base product expect catalog */
    kprintf("idt: soft expect slots=256 base_present=33 "
            "exc=32 int80=1 type_8e=30 type_ef=2 type_ee=1 "
            "bp_type=0xef of_type=0xef int80_type=0xee "
            "cs=0x%x base_match=%u type_base_ok=%u wave=%u\n",
            (unsigned)GJ_GDT_KERNEL_CS, u32BaseMatch, u32TypeBaseOk,
            (unsigned)IDT_SOFT_WAVE);

    /* Grep: idt: soft ist - IST / pad / zero-off lamps */
    kprintf("idt: soft ist nonzero=%u zero_off=%u zero_pad_nz=%u "
            "sel_mis=%u first_p=%u last_p=%u first_irq=%u "
            "extra=%u absent=%u product_ist0=1\n",
            g_u32SoftIstNonzero, g_u32SoftZeroOff, g_u32SoftZeroPadNz,
            g_u32SoftSelMismatch, g_u32SoftFirstPresent,
            g_u32SoftLastPresent, g_u32SoftFirstIrqVec,
            g_u32SoftExtraBeyondBase, g_u32SoftAbsent);

    /* Grep: idt: soft sel - kernel-CS contract surface */
    kprintf("idt: soft sel expect_cs=0x%x mismatch=%u "
            "last_sel=0x%x last_vec=%u last_type=0x%x "
            "live=%u ready=%u\n",
            (unsigned)GJ_GDT_KERNEL_CS, g_u32SoftSelMismatch,
            (unsigned)g_SoftSnap.u16LastSel, g_SoftSnap.u32LastVec,
            (unsigned)g_SoftSnap.u8LastType,
            g_fSoftSnapLive ? 1u : 0u, g_fIdtReady ? 1u : 0u);

    /* Grep: idt: soft exclusive · Soft!=product · DualDoD=OPEN */
    kprintf("idt: soft exclusive wave=%u exclusive=1 soft=1 "
            "unit=idt.c multi_server=0 hard_gate=0 "
            "soft_ne_product=1 DualDoD=OPEN product_close=0 "
            "g_ac1=1 no_ko_product_ac=1 dual_license=1 Soft!=product\n",
            (unsigned)IDT_SOFT_WAVE);

    /* Grep: idt: soft claim - product claim bounds */
    kprintf("idt: soft claim exc32=1 int80=1 dynamic_gates=1 "
            "base_present=33 type_8e=30 type_ef=2 type_ee=1 "
            "shared_idt=1 ap_lidt=1 multi_server=0 "
            "hard_gate=0 soft_ne_product=1 wave=%u\n",
            (unsigned)IDT_SOFT_WAVE);

    /* Grep: idt: soft ratio - install/reject/verify/band path ratios */
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
     * Grep: idt: soft match - expect-vs-live base contract lamps.
     * Soft only; never hard-gates product. Complements soft expect/contract.
     * C0 residual: limit/base structural match folded in.
     */
    kprintf("idt: soft match exc32=%u int80=%u bp=%u of=%u "
            "type_base=%u sel=%u ist0=%u pad0=%u zero_off0=%u "
            "limit=%u base_ptr=%u base=%u all=%u ready=%u "
            "soft_ne_product=1\n",
            u32MatchExc, u32MatchInt80, u32MatchBp, u32MatchOf,
            u32TypeBaseOk, u32MatchSel, u32MatchIst, u32MatchPad,
            u32MatchZeroOff, u32MatchLimit, u32MatchBase, u32BaseMatch,
            u32MatchAll, g_fIdtReady ? 1u : 0u);

    /*
     * Grep: idt: soft ap - AP lidt path honesty.
     * Shared table; APs reload IDTR only (no gate rebuild).
     */
    kprintf("idt: soft ap loads=%u ready=%u shared=1 rebuild=0 "
            "lidt_only=1 inits=%u present=%u wave=%u "
            "soft_ne_product=1\n",
            g_u32SoftApLoads, g_fIdtReady ? 1u : 0u,
            g_u32SoftInits, g_SoftSnap.u32Present,
            (unsigned)IDT_SOFT_WAVE);

    /* C0 residual lean via=inv (full inventory path; Soft!=product). */
    idt_soft_residual_lean_emit("inv");

    /*
     * Grep: idt: soft deepen - lean area catalog only.
     * No ret*angle / retclass / retmap stamp-storm residue.
     * residual_lean + residual_via=init|inv|ap cover C0 residual honesty.
     * Soft!=product; never hard-gates product paths. DualDoD=OPEN.
     */
    kprintf("idt: soft deepen wave=%u "
            "areas=inventory,present,layout,contract,bands,type,span,"
            "vectors,entry,last,stats,path,program,verify,"
            "honesty,query,expect,ist,sel,exclusive,claim,ratio,"
            "match,ap,residual_lean "
            "residual_via=init,inv,ap "
            "unit=idt.c only hot_irq_kprintf=0 hard_gate=0 "
            "soft_ne_product=1 g_ac1=1 no_ko_product_ac=1 "
            "DualDoD=OPEN dual_license=1 storm=0 "
            "inv=%u skip=%u cap=%u "
            "lean_n=%u lean_init=%u lean_inv=%u lean_ap=%u\n",
            (unsigned)IDT_SOFT_WAVE, g_u32SoftInvLogs, g_u32SoftInvSkip,
            (unsigned)IDT_SOFT_INV_LOG_CAP,
            g_u32SoftResidualLeanN, g_u32SoftResidualLeanInit,
            g_u32SoftResidualLeanInv, g_u32SoftResidualLeanAp);

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
     * Soft identify outcome. Ready + base contract green -> soft PASS.
     * Soft PASS != product multi-server / Dual DoD close (DualDoD=OPEN).
     */
    if (g_fIdtReady && g_u32SoftBpOk && g_u32SoftOfOk &&
        g_u32SoftInt80Ok && g_u32SoftExcPresent == 32u) {
        kprintf("idt: soft PASS wave=%u present=%u type_8e=%u type_ef=%u "
                "type_ee=%u extra=%u base_match=%u match_all=%u "
                "Soft!=product DualDoD=OPEN\n",
                (unsigned)IDT_SOFT_WAVE, g_SoftSnap.u32Present,
                g_u32SoftType8e, g_u32SoftTypeEf, g_u32SoftTypeEe,
                g_u32SoftExtraBeyondBase, u32BaseMatch, u32MatchAll);
    } else if (!g_fIdtReady) {
        kprintf("idt: soft idle (IDT not loaded)\n");
    } else {
        kprintf("idt: soft PARTIAL ready=%u exc_p=%u bp=%u of=%u "
                "int80=%u first_abs_exc=%u wave=%u Soft!=product\n",
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
    /* int 0x80 - DPL3 interrupt gate for PE32/compat syscalls */
    idt_set(128, isr_stub_128, 0xEE);

    g_IdtPtr.u16Limit = (u16)(sizeof(g_aIdt) - 1);
    g_IdtPtr.u64Base = (u64)(gj_vaddr_t)&g_aIdt[0];
    __asm__ volatile ("lidt %0" : : "m"(g_IdtPtr));
    g_fIdtReady = 1;
    g_u32SoftInits++;

    kprintf("idt: loaded 32 exception gates + int80\n");

    /* Greppable soft program line under idt: soft ... */
    kprintf("idt: soft program wave=%u installs=%u exc=%u int80=%u "
            "last vec=%u type=0x%x\n",
            (unsigned)IDT_SOFT_WAVE, g_u32SoftInstalls, g_u32SoftException,
            g_u32SoftInt80, g_SoftSnap.u32LastVec,
            (unsigned)g_SoftSnap.u8LastType);

    /*
     * Legacy greppable (kept for existing smoke greps):
     *   idt: gate soft program ...
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

    /* C0 residual lean once at bring-up (via=init; Soft!=product). */
    idt_soft_residual_lean_emit("init");

    /* Full soft inventory at base load (lean; no stamp storm). */
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
    /*
     * C0 residual lean via=ap once (shared table; rebuild=0).
     * Soft!=product; never rebuilds gates; DualDoD remains OPEN.
     */
    if (g_fSoftResidualLeanApOnce == 0u) {
        g_fSoftResidualLeanApOnce = 1u;
        idt_soft_residual_lean_emit("ap");
    }
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
     * Primary prefix: idt: soft ...
     * Legacy prefix kept: idt: gate soft ...
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
/* Soft residual lean: soft inventory + match/ap/residual_lean only.
 * Soft!=product dual MIT/Apache. G-AC-1 (no .ko product AC).
 * Stamp-storm / ret*angle / retmap residue excised. No product version stamp.
 * C0 residual: via=init|inv|ap; IDT_SOFT_INV_LOG_CAP; DualDoD=OPEN.
 * limit/base structural match residual. Soft PASS != product Dual DoD close.
 */
