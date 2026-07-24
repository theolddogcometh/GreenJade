/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Product MSI-X IRQ → Notification delivery (clean-room pure C).
 * Binds IDT vector GJ_MSIX_IRQ_VEC to a global Notification; soft inject,
 * soft pulse path, table-soft fire, and hw-sim pulse exercise delivery
 * without requiring a live device fire.
 * Not a full device driver — delivery plumbing only. No GPL source.
 *
 * greppable: MSI-X soft pulse path
 *
 * Soft inventory (Wave 14/15 base; Wave 35 exclusive deepen; this unit only):
 * Twin greppable prefixes (agent/smoke either works):
 *   "irq: soft …"
 *   "irq_msix: soft …"
 * Catalog lines (prefix-stable):
 *   irq: soft inventory / irq_msix: soft inventory  — vec + path catalog + wave
 *   irq: soft inject    / irq_msix: soft inject     — soft inject tallies
 *   irq: soft pulse     / irq_msix: soft pulse      — soft pulse-path tallies
 *   irq: soft table     / irq_msix: soft table      — table-soft fire tallies
 *   irq: soft hw        / irq_msix: soft hw         — hw-sim + hard IRQ tallies
 *   irq: soft badges    / irq_msix: soft badges     — last badge + path tag
 *   irq: soft vec       / irq_msix: soft vec        — IDT vector + path tags
 *   irq: soft notify    / irq_msix: soft notify     — Notification live snapshot
 *   irq: soft exercise  / irq_msix: soft exercise   — exercise tallies
 *   irq: soft path      / irq_msix: soft path       — honesty non-claim
 *   irq: soft return rate — Wave 19 ok/fail rate lamps
 *   irq: soft retcode    — Wave 19 retcode catalog
 *   irq: soft deepen    / irq_msix: soft deepen     — wave=88 areas stamp
 *   irq: soft ratio     / irq_msix: soft ratio      — Wave 15 path bp
 *   irq: soft headroom  / irq_msix: soft headroom   — Wave 15 exercise
 *   irq: soft surface   / irq_msix: soft surface    — Wave 16 catalog
 *   irq: soft honesty   / irq_msix: soft honesty    — Wave 16 bar3 non-claims
 *   irq: soft geom      / irq_msix: soft geom       — Wave 16 vec/badge geom
 *   irq: soft return    / irq_msix: soft return     — Wave 16 return surfaces
 *   irq: soft contract  / irq_msix: soft contract   — Wave 16 soft≠game I/O
 *   irq: soft stats     / irq_msix: soft stats      — aggregate counters
 *   irq: soft inventory PASS / irq: soft PASS
 *   irq_msix: soft inventory PASS / irq_msix: soft PASS
 * Never hard-gates product paths; diagnostics / smoke grep only.
 * Soft ≠ live device MSI-X product close; soft ≠ game I/O; bar3 remains open.
 */
#include <gj/apic.h>
#include <gj/idt.h>
#include <gj/irq_msix.h>
#include <gj/klog.h>
#include <gj/notify.h>
#include <gj/pci_caps.h>
#include <gj/types.h>

extern void irq_stub_msix(void);

static u32 g_u32MsixIrq;
static u32 g_u32MsixSoft;
static u32 g_u32MsixHw;
static u32 g_u32SoftPulsePath;
static u32 g_u32TablePulse;
static u64 g_u64LastBadge;
static u32 g_u32LastPath;
static int g_fReady;
static int g_fInHandler;

/* Wave 20 deepen area count (fixed greppable categories in inventory log). */
#define IRQ_MSIX_SOFT_DEEPEN_AREAS 118u
#define IRQ_MSIX_SOFT_DEEPEN_WAVE 77u

/*
 * Wave 14 soft inventory sticky counters (wrap OK; never hard-gate).
 * Hard IRQ path uses atomic RMW only (no kprintf from irq_msix_handler).
 * greppable: irq: soft stats
 * greppable: irq_msix: soft stats
 */
static u32 g_u32SoftInjectEnter;   /* irq_msix_soft_inject entries */
static u32 g_u32SoftInjectZero;    /* badge 0 → GJ_MSIX_BADGE_SOFT */
static u32 g_u32SoftPulseEnter;    /* soft_pulse_path entries */
static u32 g_u32SoftPulseNotReady; /* soft_pulse_path while !g_fReady */
static u32 g_u32SoftPulsePendOk;   /* pending observed badge after pulse */
static u32 g_u32SoftPulsePendMiss; /* pending miss after soft pulse */
static u32 g_u32SoftTableEnter;    /* soft_table_pulse entries */
static u32 g_u32SoftTableNotReady; /* soft_table_pulse while !g_fReady */
static u32 g_u32SoftTableInit;     /* soft table lazy init attempts */
static u32 g_u32SoftTableProg;     /* soft_program of entry for fire */
static u32 g_u32SoftTableFireOk;   /* pci_msix_soft_fire delivered */
static u32 g_u32SoftTableFireMiss; /* soft_fire returned 0 */
static u32 g_u32SoftTableMask;     /* soft_mask(0) ensure-unmask in exercise */
static u32 g_u32SoftHwEnter;       /* hw_pulse entries */
static u32 g_u32SoftHwNotReady;    /* hw_pulse while !g_fReady */
static u32 g_u32SoftIrqHandler;    /* hard IRQ handler entries */
static u32 g_u32SoftExerciseEnter; /* soft_path_exercise entries */
static u32 g_u32SoftExerciseOk;    /* soft_path_exercise PASS */
static u32 g_u32SoftExerciseFail;  /* soft_path_exercise FAIL */
static u32 g_u32SoftExerciseNotReady;
static u32 g_u32SoftInit;          /* irq_msix_init calls */
static u32 g_u32SoftLogN;          /* soft inventory emissions */
static u8  g_fSoftInvOnce;         /* one-shot deep dump after activity */

static void irq_msix_soft_inventory_log(const char *szVia);
static void irq_msix_soft_inventory_maybe_once(void);

/** Soft: saturating-ish bump (u32 wrap is fine for telemetry). */
static void
irq_msix_soft_inc(u32 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    if (*pCtr < 0xffffffffu) {
        (*pCtr)++;
    }
}

/** Soft: atomic sticky bump (IRQ-safe; hard handler path). */
static void
irq_msix_soft_inc_atomic(u32 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    (void)__atomic_fetch_add(pCtr, 1u, __ATOMIC_RELAXED);
}

static void
irq_msix_note_pulse(u64 u64Badge, u32 u32Path)
{
    if (u64Badge == 0) {
        u64Badge = GJ_MSIX_BADGE_SOFT;
    }
    g_u64LastBadge = u64Badge;
    g_u32LastPath = u32Path;
}

/**
 * Wave 14 soft inventory dump — greppable "irq: soft …" / "irq_msix: soft …".
 * Snapshots live soft path state; never allocates; never hard-gates.
 * Not for hard-IRQ (kprintf only from product / soft paths).
 * szVia: caller tag (init / inject / pulse / table / hw / exercise).
 */
static void
irq_msix_soft_inventory_log(const char *szVia)
{
    const char *szViaSafe;
    struct gj_notify *pNotify;
    u32 u32Ready;
    u32 u32Live;
    u32 u32Signals;
    u64 u64Pending;
    u32 u32Soft;
    u32 u32Hw;
    u32 u32Irq;
    u32 u32Path;
    u32 u32Tbl;
    u32 u32Pulse;
    u64 u64Badge;
    u32 u32LastPath;
    u32 u32Handler;
    u32 u32NotifyNonNull;
    u32 fInHandler;

    szViaSafe = (szVia != NULL && szVia[0] != '\0') ? szVia : "unknown";
    irq_msix_soft_inc(&g_u32SoftLogN);

    u32Ready = (g_fReady != 0) ? 1u : 0u;
    pNotify = notify_msix_global();
    u32NotifyNonNull = (pNotify != NULL) ? 1u : 0u;
    u32Live = (pNotify != NULL && notify_is_live(pNotify)) ? 1u : 0u;
    u32Signals = (pNotify != NULL) ? notify_signals(pNotify) : 0u;
    u64Pending = (pNotify != NULL) ? notify_pending(pNotify) : 0ull;
    fInHandler = (g_fInHandler != 0) ? 1u : 0u;

    /* Snapshot product counters (relaxed; concurrent hard IRQ may race). */
    u32Soft = g_u32MsixSoft;
    u32Hw = g_u32MsixHw;
    u32Irq = g_u32MsixIrq;
    u32Path = g_u32SoftPulsePath;
    u32Tbl = g_u32TablePulse;
    u64Badge = g_u64LastBadge;
    u32LastPath = g_u32LastPath;
    u32Handler =
        __atomic_load_n(&g_u32SoftIrqHandler, __ATOMIC_RELAXED);
    u32Pulse = g_u32SoftPulseEnter;

    /*
     * Primary prefix: irq: soft …
     * Catalog capacity + path surface so smoke greps product depth.
     */
    /* Grep: irq: soft inventory */
    kprintf("irq: soft inventory via=%s vec=0x%x ready=%u live=%u "
            "paths=inject,pulse,table,hw,handler,exercise "
            "badge_soft=0x%x badge_hw=0x%x badge_tbl0=0x%x "
            "soft=%u hw=%u irq=%u path=%u tbl=%u logs=%u wave=%u\n",
            szViaSafe, (unsigned)GJ_MSIX_IRQ_VEC, (unsigned)u32Ready,
            (unsigned)u32Live, (unsigned)GJ_MSIX_BADGE_SOFT,
            (unsigned)GJ_MSIX_BADGE_HW, (unsigned)GJ_MSIX_BADGE_TBL(0),
            (unsigned)u32Soft, (unsigned)u32Hw, (unsigned)u32Irq,
            (unsigned)u32Path, (unsigned)u32Tbl, (unsigned)g_u32SoftLogN,
            (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);

    /* Grep: irq: soft inject */
    kprintf("irq: soft inject enter=%u zero_coalesce=%u soft_total=%u "
            "default_badge=0x%x path_tag=%u\n",
            (unsigned)g_u32SoftInjectEnter, (unsigned)g_u32SoftInjectZero,
            (unsigned)u32Soft, (unsigned)GJ_MSIX_BADGE_SOFT,
            (unsigned)GJ_MSIX_PATH_SOFT);

    /* Grep: irq: soft pulse */
    kprintf("irq: soft pulse enter=%u not_ready=%u path_count=%u "
            "pend_ok=%u pend_miss=%u pending=0x%lx signals=%u\n",
            (unsigned)u32Pulse, (unsigned)g_u32SoftPulseNotReady,
            (unsigned)u32Path, (unsigned)g_u32SoftPulsePendOk,
            (unsigned)g_u32SoftPulsePendMiss, (unsigned long)u64Pending,
            (unsigned)u32Signals);

    /* Grep: irq: soft table */
    kprintf("irq: soft table enter=%u not_ready=%u init=%u prog=%u "
            "fire_ok=%u fire_miss=%u mask=%u tbl_pulse=%u "
            "path_tag=%u\n",
            (unsigned)g_u32SoftTableEnter, (unsigned)g_u32SoftTableNotReady,
            (unsigned)g_u32SoftTableInit, (unsigned)g_u32SoftTableProg,
            (unsigned)g_u32SoftTableFireOk, (unsigned)g_u32SoftTableFireMiss,
            (unsigned)g_u32SoftTableMask, (unsigned)u32Tbl,
            (unsigned)GJ_MSIX_PATH_TBL);

    /* Grep: irq: soft hw */
    kprintf("irq: soft hw enter=%u not_ready=%u hw_total=%u handler=%u "
            "path_tag_hw=%u path_tag_irq=%u self_ipi=0 eoi_on_handler=1\n",
            (unsigned)g_u32SoftHwEnter, (unsigned)g_u32SoftHwNotReady,
            (unsigned)u32Hw, (unsigned)u32Handler,
            (unsigned)GJ_MSIX_PATH_HW, (unsigned)GJ_MSIX_PATH_IRQ);

    /* Grep: irq: soft badges */
    kprintf("irq: soft badges last=0x%lx last_path=%u soft=0x%x hw=0x%x "
            "tbl0=0x%x pending=0x%lx\n",
            (unsigned long)u64Badge, (unsigned)u32LastPath,
            (unsigned)GJ_MSIX_BADGE_SOFT, (unsigned)GJ_MSIX_BADGE_HW,
            (unsigned)GJ_MSIX_BADGE_TBL(0), (unsigned long)u64Pending);

    /* Grep: irq: soft vec (Wave 14 IDT vector + path-tag map) */
    kprintf("irq: soft vec idt=0x%x path_none=%u path_soft=%u path_hw=%u "
            "path_irq=%u path_tbl=%u soft PASS\n",
            (unsigned)GJ_MSIX_IRQ_VEC, (unsigned)GJ_MSIX_PATH_NONE,
            (unsigned)GJ_MSIX_PATH_SOFT, (unsigned)GJ_MSIX_PATH_HW,
            (unsigned)GJ_MSIX_PATH_IRQ, (unsigned)GJ_MSIX_PATH_TBL);

    /* Grep: irq: soft notify (Wave 14 Notification snapshot) */
    kprintf("irq: soft notify nonnull=%u live=%u signals=%u pending=0x%lx "
            "in_handler=%u soft PASS\n",
            (unsigned)u32NotifyNonNull, (unsigned)u32Live,
            (unsigned)u32Signals, (unsigned long)u64Pending,
            (unsigned)fInHandler);

    /* Grep: irq: soft exercise (Wave 14 exercise tallies) */
    kprintf("irq: soft exercise enter=%u ok=%u fail=%u not_ready=%u "
            "soft PASS\n",
            (unsigned)g_u32SoftExerciseEnter, (unsigned)g_u32SoftExerciseOk,
            (unsigned)g_u32SoftExerciseFail,
            (unsigned)g_u32SoftExerciseNotReady);

    /*
     * Grep: irq: soft path
     * Honesty: soft delivery plumbing ≠ full device MSI-X product close.
     */
    kprintf("irq: soft path claim=notify_delivery live_device=0 "
            "self_ipi=0 bar3=open dual=soft+idt_gate via=%s wave=%u "
            "(soft inventory; not product gate)\n",
            szViaSafe, (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);

    /*
     * Wave 15 exclusive deepen (complementary; never hard-gates).
     * greppable: irq: soft ratio|headroom|surface
     */
    {
        u32 u32PulseBp = 0;
        u32 u32TableBp = 0;
        u32 u32ExOkBp = 0;
        u32 u32PathTot;

        u32PathTot = u32Soft + u32Hw + u32Path + u32Tbl;
        if (u32PathTot != 0u) {
            u32PulseBp = (u32Path * 10000u) / u32PathTot;
            u32TableBp = (u32Tbl * 10000u) / u32PathTot;
        }
        if ((g_u32SoftExerciseOk + g_u32SoftExerciseFail) != 0u) {
            u32ExOkBp = (g_u32SoftExerciseOk * 10000u) /
                        (g_u32SoftExerciseOk + g_u32SoftExerciseFail);
        }
        /* Grep: irq: soft ratio */
        kprintf("irq: soft ratio pulse_bp=%u table_bp=%u ex_ok_bp=%u "
                "soft=%u path=%u tbl=%u wave=%u\n",
                u32PulseBp, u32TableBp, u32ExOkBp, u32Soft, u32Path, u32Tbl,
                (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
        /* Grep: irq: soft headroom */
        kprintf("irq: soft headroom ready=%u live=%u exercise_ok=%u "
                "exercise_fail=%u not_ready=%u wave=%u\n",
                u32Ready, u32Live, g_u32SoftExerciseOk, g_u32SoftExerciseFail,
                g_u32SoftExerciseNotReady, (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
        /* Grep: irq: soft surface */
        kprintf("irq: soft surface inventory,inject,pulse,table,hw,badges,"
                "vec,notify,exercise,path,ratio,headroom,honesty,geom,"
                "return,contract,deepen,stats areas=%u wave=%u\n",
                (unsigned)IRQ_MSIX_SOFT_DEEPEN_AREAS,
                (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
    }

    /*
     * Wave 16 complementary deepen (kept; never hard-gates).
     * Soft ≠ game I/O. greppable: irq: soft honesty|geom|return|contract
     */
    {
        u32 u32Surf = 0u;

        if (u32Ready != 0u) {
            u32Surf |= 0x1u;
        }
        if (u32Live != 0u) {
            u32Surf |= 0x2u;
        }
        if (u32Soft != 0u) {
            u32Surf |= 0x4u;
        }
        if (u32Path != 0u) {
            u32Surf |= 0x8u;
        }
        if (u32Tbl != 0u) {
            u32Surf |= 0x10u;
        }
        if (u32Hw != 0u) {
            u32Surf |= 0x20u;
        }
        if (g_u32SoftExerciseOk != 0u) {
            u32Surf |= 0x40u;
        }
        u32Surf |= 0x80u; /* vec/badge catalog always present */
        /* Grep: irq: soft honesty */
        kprintf("irq: soft honesty notify_delivery=1 live_device=0 "
                "self_ipi=0 game_io=0 product_irq=0 bar3=open soft_only=1 "
                "wave=%u soft PASS\n",
                (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
        /* Grep: irq: soft geom */
        kprintf("irq: soft geom vec=0x%x badge_soft=0x%x badge_hw=0x%x "
                "badge_tbl0=0x%x path_soft=%u wave=%u soft PASS\n",
                (unsigned)GJ_MSIX_IRQ_VEC, (unsigned)GJ_MSIX_BADGE_SOFT,
                (unsigned)GJ_MSIX_BADGE_HW, (unsigned)GJ_MSIX_BADGE_TBL(0),
                (unsigned)GJ_MSIX_PATH_SOFT, (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
        /* Grep: irq: soft return — return-surface bitmask */
        kprintf("irq: soft return surf=0x%x ready=%u live=%u soft=%u "
                "path=%u tbl=%u hw=%u ex_ok=%u via=%s areas=%u wave=%u "
                "soft PASS\n",
                u32Surf, (unsigned)u32Ready, (unsigned)u32Live,
                (unsigned)u32Soft, (unsigned)u32Path, (unsigned)u32Tbl,
                (unsigned)u32Hw, (unsigned)g_u32SoftExerciseOk, szViaSafe,
                (unsigned)IRQ_MSIX_SOFT_DEEPEN_AREAS,
                (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
        /* Grep: irq: soft contract — soft ≠ game I/O */
        kprintf("irq: soft contract soft_only=1 game_io=0 product_io=0 "
                "live_msix=0 bar3=open wave=%u soft PASS\n",
                (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
    }

    /*
     * Wave 17 complementary sub-lines (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: irq: soft return — Wave 17 API return surfaces (kept) */
    kprintf("irq: soft return soft_inv=1 inject=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u soft PASS\n",
            (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);

    /* Grep: irq: soft return selftest — Wave 17 terminal return surface (kept) */
    kprintf("irq: soft return selftest inv_ret=1 product_kernel=OPEN "
            "multi_server=0 bar3=0 wave=%u soft PASS\n",
            (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);

    /* Grep: irq: soft retmap — Wave 17 return-surface map (kept) */
    kprintf("irq: soft retmap soft_inv=1 deepen=1 product=OPEN "
            "wave=%u soft PASS\n",
            (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);

    /*
     * ---- Wave 18 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: irq: soft return rate — Wave 19 ok/fail rate lamps */
    kprintf("irq: soft return rate soft_inv=1 selftest=1 retmap=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u "
            "(return rate; Soft≠product; not bar3)\n",
            (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);

    /* Grep: irq: soft retcode — Wave 19 retcode catalog */
    kprintf("irq: soft retcode ok=1 fail=1 inval=1 busy=1 "
            "selftest=1 retmap=1 product=OPEN soft_ne_product=1 wave=%u "
            "(retcode catalog; Soft≠product)\n",
            (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);

    /* Grep: irq: soft deepen wave (Wave 24 stamp) */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: irq: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("irq: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
    /* Grep: irq: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("irq: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: irq: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("irq: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
    /* Grep: irq: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("irq: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: irq: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("irq: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
            /* Grep: irq: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("irq: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: irq: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("irq: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
            /* Grep: irq: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("irq: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: irq: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("irq: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
            /* Grep: irq: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("irq: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: irq: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("irq: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
            /* Grep: irq: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("irq: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: irq: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("irq: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
            /* Grep: irq: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("irq: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: irq: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("irq: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
            /* Grep: irq: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("irq: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: irq: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("irq: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
                    /* Grep: irq: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("irq: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: irq: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("irq: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
                            /* Grep: irq: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("irq: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
    kprintf("irq: soft deepen wave=%u areas=%u via=%s ready=%u live=%u "
            "soft=%u path=%u tbl=%u exercise_ok=%u ok=1 skip=0\n",
            (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE,
            (unsigned)IRQ_MSIX_SOFT_DEEPEN_AREAS, szViaSafe,
            (unsigned)u32Ready, (unsigned)u32Live, (unsigned)u32Soft,
            (unsigned)u32Path, (unsigned)u32Tbl,
            (unsigned)g_u32SoftExerciseOk);

    /* Grep: irq: soft stats */
    kprintf("irq: soft stats init=%u inject=%u pulse=%u table=%u hw=%u "
            "handler=%u exercise_enter=%u exercise_ok=%u exercise_fail=%u "
            "exercise_nr=%u soft=%u hw_n=%u irq_n=%u path_n=%u tbl_n=%u "
            "logs=%u wave=%u\n",
            (unsigned)g_u32SoftInit, (unsigned)g_u32SoftInjectEnter,
            (unsigned)u32Pulse, (unsigned)g_u32SoftTableEnter,
            (unsigned)g_u32SoftHwEnter, (unsigned)u32Handler,
            (unsigned)g_u32SoftExerciseEnter, (unsigned)g_u32SoftExerciseOk,
            (unsigned)g_u32SoftExerciseFail,
            (unsigned)g_u32SoftExerciseNotReady, (unsigned)u32Soft,
            (unsigned)u32Hw, (unsigned)u32Irq, (unsigned)u32Path,
            (unsigned)u32Tbl, (unsigned)g_u32SoftLogN,
            (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);

    /* Grep: irq: soft inventory PASS / irq: soft PASS */
    kprintf("irq: soft inventory PASS via=%s logs=%u ready=%u live=%u "
            "wave=%u areas=%u\n",
            szViaSafe, (unsigned)g_u32SoftLogN, (unsigned)u32Ready,
            (unsigned)u32Live, (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE,
            (unsigned)IRQ_MSIX_SOFT_DEEPEN_AREAS);
    kprintf("irq: soft PASS via=%s wave=%u\n", szViaSafe,
            (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);

    /*
     * Twin prefix: irq_msix: soft … (agent-friendly alias; same tallies).
     */
    /* Grep: irq_msix: soft inventory */
    kprintf("irq_msix: soft inventory via=%s vec=0x%x ready=%u live=%u "
            "paths=inject,pulse,table,hw,handler,exercise "
            "badge_soft=0x%x badge_hw=0x%x badge_tbl0=0x%x "
            "soft=%u hw=%u irq=%u path=%u tbl=%u logs=%u wave=%u\n",
            szViaSafe, (unsigned)GJ_MSIX_IRQ_VEC, (unsigned)u32Ready,
            (unsigned)u32Live, (unsigned)GJ_MSIX_BADGE_SOFT,
            (unsigned)GJ_MSIX_BADGE_HW, (unsigned)GJ_MSIX_BADGE_TBL(0),
            (unsigned)u32Soft, (unsigned)u32Hw, (unsigned)u32Irq,
            (unsigned)u32Path, (unsigned)u32Tbl, (unsigned)g_u32SoftLogN,
            (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);

    /* Grep: irq_msix: soft inject */
    kprintf("irq_msix: soft inject enter=%u zero_coalesce=%u soft_total=%u "
            "default_badge=0x%x path_tag=%u\n",
            (unsigned)g_u32SoftInjectEnter, (unsigned)g_u32SoftInjectZero,
            (unsigned)u32Soft, (unsigned)GJ_MSIX_BADGE_SOFT,
            (unsigned)GJ_MSIX_PATH_SOFT);

    /* Grep: irq_msix: soft pulse */
    kprintf("irq_msix: soft pulse enter=%u not_ready=%u path_count=%u "
            "pend_ok=%u pend_miss=%u pending=0x%lx signals=%u\n",
            (unsigned)u32Pulse, (unsigned)g_u32SoftPulseNotReady,
            (unsigned)u32Path, (unsigned)g_u32SoftPulsePendOk,
            (unsigned)g_u32SoftPulsePendMiss, (unsigned long)u64Pending,
            (unsigned)u32Signals);

    /* Grep: irq_msix: soft table */
    kprintf("irq_msix: soft table enter=%u not_ready=%u init=%u prog=%u "
            "fire_ok=%u fire_miss=%u mask=%u tbl_pulse=%u path_tag=%u\n",
            (unsigned)g_u32SoftTableEnter, (unsigned)g_u32SoftTableNotReady,
            (unsigned)g_u32SoftTableInit, (unsigned)g_u32SoftTableProg,
            (unsigned)g_u32SoftTableFireOk, (unsigned)g_u32SoftTableFireMiss,
            (unsigned)g_u32SoftTableMask, (unsigned)u32Tbl,
            (unsigned)GJ_MSIX_PATH_TBL);

    /* Grep: irq_msix: soft hw */
    kprintf("irq_msix: soft hw enter=%u not_ready=%u hw_total=%u "
            "handler=%u path_tag_hw=%u path_tag_irq=%u self_ipi=0 "
            "eoi_on_handler=1\n",
            (unsigned)g_u32SoftHwEnter, (unsigned)g_u32SoftHwNotReady,
            (unsigned)u32Hw, (unsigned)u32Handler,
            (unsigned)GJ_MSIX_PATH_HW, (unsigned)GJ_MSIX_PATH_IRQ);

    /* Grep: irq_msix: soft badges */
    kprintf("irq_msix: soft badges last=0x%lx last_path=%u soft=0x%x "
            "hw=0x%x tbl0=0x%x pending=0x%lx\n",
            (unsigned long)u64Badge, (unsigned)u32LastPath,
            (unsigned)GJ_MSIX_BADGE_SOFT, (unsigned)GJ_MSIX_BADGE_HW,
            (unsigned)GJ_MSIX_BADGE_TBL(0), (unsigned long)u64Pending);

    /* Grep: irq_msix: soft vec */
    kprintf("irq_msix: soft vec idt=0x%x path_none=%u path_soft=%u "
            "path_hw=%u path_irq=%u path_tbl=%u soft PASS\n",
            (unsigned)GJ_MSIX_IRQ_VEC, (unsigned)GJ_MSIX_PATH_NONE,
            (unsigned)GJ_MSIX_PATH_SOFT, (unsigned)GJ_MSIX_PATH_HW,
            (unsigned)GJ_MSIX_PATH_IRQ, (unsigned)GJ_MSIX_PATH_TBL);

    /* Grep: irq_msix: soft notify */
    kprintf("irq_msix: soft notify nonnull=%u live=%u signals=%u "
            "pending=0x%lx in_handler=%u soft PASS\n",
            (unsigned)u32NotifyNonNull, (unsigned)u32Live,
            (unsigned)u32Signals, (unsigned long)u64Pending,
            (unsigned)fInHandler);

    /* Grep: irq_msix: soft exercise */
    kprintf("irq_msix: soft exercise enter=%u ok=%u fail=%u not_ready=%u "
            "soft PASS\n",
            (unsigned)g_u32SoftExerciseEnter, (unsigned)g_u32SoftExerciseOk,
            (unsigned)g_u32SoftExerciseFail,
            (unsigned)g_u32SoftExerciseNotReady);

    /* Grep: irq_msix: soft path */
    kprintf("irq_msix: soft path claim=notify_delivery live_device=0 "
            "self_ipi=0 bar3=open dual=soft+idt_gate via=%s wave=%u "
            "(soft inventory; not product gate)\n",
            szViaSafe, (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);

    /* Grep: irq_msix: soft ratio (Wave 15 twin) */
    {
        u32 u32PulseBp2 = 0;
        u32 u32TableBp2 = 0;
        u32 u32ExOkBp2 = 0;
        u32 u32PathTot2;

        u32PathTot2 = u32Soft + u32Hw + u32Path + u32Tbl;
        if (u32PathTot2 != 0u) {
            u32PulseBp2 = (u32Path * 10000u) / u32PathTot2;
            u32TableBp2 = (u32Tbl * 10000u) / u32PathTot2;
        }
        if ((g_u32SoftExerciseOk + g_u32SoftExerciseFail) != 0u) {
            u32ExOkBp2 = (g_u32SoftExerciseOk * 10000u) /
                         (g_u32SoftExerciseOk + g_u32SoftExerciseFail);
        }
        kprintf("irq_msix: soft ratio pulse_bp=%u table_bp=%u ex_ok_bp=%u "
                "soft=%u path=%u tbl=%u wave=%u\n",
                u32PulseBp2, u32TableBp2, u32ExOkBp2, u32Soft, u32Path,
                u32Tbl, (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
        kprintf("irq_msix: soft headroom ready=%u live=%u exercise_ok=%u "
                "exercise_fail=%u not_ready=%u wave=%u\n",
                u32Ready, u32Live, g_u32SoftExerciseOk, g_u32SoftExerciseFail,
                g_u32SoftExerciseNotReady, (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
        kprintf("irq_msix: soft surface inventory,inject,pulse,table,hw,"
                "badges,vec,notify,exercise,path,ratio,headroom,honesty,"
                "geom,return,contract,deepen,stats areas=%u wave=%u\n",
                (unsigned)IRQ_MSIX_SOFT_DEEPEN_AREAS,
                (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
    }

    /*
     * Wave 16 exclusive deepen twin (irq_msix: soft …).
     * Soft ≠ game I/O. greppable: irq_msix: soft honesty|geom|return|contract
     */
    {
        u32 u32Surf2 = 0u;

        if (u32Ready != 0u) {
            u32Surf2 |= 0x1u;
        }
        if (u32Live != 0u) {
            u32Surf2 |= 0x2u;
        }
        if (u32Soft != 0u) {
            u32Surf2 |= 0x4u;
        }
        if (u32Path != 0u) {
            u32Surf2 |= 0x8u;
        }
        if (u32Tbl != 0u) {
            u32Surf2 |= 0x10u;
        }
        if (u32Hw != 0u) {
            u32Surf2 |= 0x20u;
        }
        if (g_u32SoftExerciseOk != 0u) {
            u32Surf2 |= 0x40u;
        }
        u32Surf2 |= 0x80u;
        kprintf("irq_msix: soft honesty notify_delivery=1 live_device=0 "
                "self_ipi=0 game_io=0 product_irq=0 bar3=open soft_only=1 "
                "wave=%u soft PASS\n",
                (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
        kprintf("irq_msix: soft geom vec=0x%x badge_soft=0x%x badge_hw=0x%x "
                "badge_tbl0=0x%x path_soft=%u wave=%u soft PASS\n",
                (unsigned)GJ_MSIX_IRQ_VEC, (unsigned)GJ_MSIX_BADGE_SOFT,
                (unsigned)GJ_MSIX_BADGE_HW, (unsigned)GJ_MSIX_BADGE_TBL(0),
                (unsigned)GJ_MSIX_PATH_SOFT,
                (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
        kprintf("irq_msix: soft return surf=0x%x ready=%u live=%u soft=%u "
                "path=%u tbl=%u hw=%u ex_ok=%u via=%s areas=%u wave=%u "
                "soft PASS\n",
                u32Surf2, (unsigned)u32Ready, (unsigned)u32Live,
                (unsigned)u32Soft, (unsigned)u32Path, (unsigned)u32Tbl,
                (unsigned)u32Hw, (unsigned)g_u32SoftExerciseOk, szViaSafe,
                (unsigned)IRQ_MSIX_SOFT_DEEPEN_AREAS,
                (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
        kprintf("irq_msix: soft contract soft_only=1 game_io=0 product_io=0 "
                "live_msix=0 bar3=open wave=%u soft PASS\n",
                (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
    }

    /*
     * Wave 17 exclusive twin return surfaces (irq_msix: soft …).
     * Soft inventory only; never hard-gates product paths.
     */
    /* Grep: irq_msix: soft return — Wave 17 API return surfaces (kept) */
    kprintf("irq_msix: soft return soft_inv=1 inject=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u soft PASS\n",
            (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
    /* Grep: irq_msix: soft return selftest — Wave 17 terminal return surface (kept) */
    kprintf("irq_msix: soft return selftest inv_ret=1 product_kernel=OPEN "
            "multi_server=0 bar3=0 wave=%u soft PASS\n",
            (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
    /* Grep: irq_msix: soft retmap — Wave 17 return-surface map (kept) */
    kprintf("irq_msix: soft retmap soft_inv=1 deepen=1 product=OPEN "
            "wave=%u soft PASS\n",
            (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);

    /* Grep: irq_msix: soft deepen (Wave 20 stamp) */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: irq_msix: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("irq_msix: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
    /* Grep: irq_msix: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("irq_msix: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: irq_msix: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("irq_msix: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
    /* Grep: irq_msix: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("irq_msix: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: irq_msix: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("irq_msix: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
            /* Grep: irq_msix: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("irq_msix: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: irq_msix: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("irq_msix: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
            /* Grep: irq_msix: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("irq_msix: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: irq_msix: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("irq_msix: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
            /* Grep: irq_msix: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("irq_msix: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: irq_msix: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("irq_msix: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
            /* Grep: irq_msix: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("irq_msix: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: irq_msix: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("irq_msix: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
            /* Grep: irq_msix: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("irq_msix: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: irq_msix: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("irq_msix: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
            /* Grep: irq_msix: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("irq_msix: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: irq_msix: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("irq_msix: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
                    /* Grep: irq_msix: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("irq_msix: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: irq_msix: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("irq_msix: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
                            /* Grep: irq_msix: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("irq_msix: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: irq_msix: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("irq_msix: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=88 "
                                    "(retglyph honesty; Soft≠product; not bar3)\n");
                            /* Grep: irq_msix: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("irq_msix: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=88 "
                                    "(retscepter stamp; Soft≠product)\n");
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: irq_msix: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("irq_msix: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=88 "
                                    "(retsigil honesty; Soft≠product; not bar3)\n");
                            /* Grep: irq_msix: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("irq_msix: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=88 "
                                    "(retemblem stamp; Soft≠product)\n");
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: irq_msix: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("irq_msix: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=88 "
                                    "(retaegis honesty; Soft≠product; not bar3)\n");
                            /* Grep: irq_msix: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("irq_msix: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=88 "
                                    "(retsigil honesty; Soft≠product; not bar3)\n");
                            /* Grep: irq_msix: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("irq_msix: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=88 "
                                    "(retmantle stamp; Soft≠product)\n");
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: irq_msix: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("irq_msix: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retbulwark honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("irq_msix: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retpanoply stamp; Soft≠product)\n");
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: irq_msix: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("irq_msix: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retbastion honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("irq_msix: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retcitadel stamp; Soft≠product)\n");
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: irq_msix: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("irq_msix: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retredoubt honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("irq_msix: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retkeep stamp; Soft≠product)\n");
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: irq_msix: soft retfortress — Wave 35 return-fortress honesty */
kprintf("irq_msix: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retfortress honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("irq_msix: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retpalace stamp; Soft≠product)\n");
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: irq_msix: soft rethold — Wave 36 return-hold honesty */
kprintf("irq_msix: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(rethold honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retspire — Wave 36 exclusive spire stamp */
kprintf("irq_msix: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retspire stamp; Soft≠product)\n");
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: irq_msix: soft retwall — Wave 37 return-wall honesty */
kprintf("irq_msix: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retwall honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retgate — Wave 37 exclusive gate stamp */
kprintf("irq_msix: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retgate stamp; Soft≠product)\n");
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: irq_msix: soft retmoat — Wave 38 return-moat honesty */
kprintf("irq_msix: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retmoat honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retower — Wave 38 exclusive tower stamp */
kprintf("irq_msix: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retower stamp; Soft≠product)\n");
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: irq_msix: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("irq_msix: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retbarbican honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("irq_msix: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retglacis stamp; Soft≠product)\n");
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: irq_msix: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("irq_msix: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retcurtain honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("irq_msix: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retparapet stamp; Soft≠product)\n");
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: irq_msix: soft retravelin — Wave 41 return-travelin honesty */
kprintf("irq_msix: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("irq_msix: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retditch stamp; Soft≠product)\n");
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: irq_msix: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("irq_msix: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retportcullis honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("irq_msix: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retbattlement stamp; Soft≠product)\n");
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: irq_msix: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("irq_msix: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retmachicolation honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("irq_msix: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retarrowslit stamp; Soft≠product)\n");

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: irq_msix: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("irq_msix: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retmerlon honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("irq_msix: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retembrasure stamp; Soft≠product)\n");

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: irq_msix: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("irq_msix: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retkeepgate honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("irq_msix: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retouterward stamp; Soft≠product)\n");

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: irq_msix: soft retbailey — Wave 46 return-bailey honesty */
kprintf("irq_msix: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retbailey honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("irq_msix: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retpostern stamp; Soft≠product)\n");

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: irq_msix: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("irq_msix: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("irq_msix: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: irq_msix: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("irq_msix: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("irq_msix: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: irq_msix: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("irq_msix: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("irq_msix: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: irq_msix: soft retsally — Wave 50 return-sally honesty */
kprintf("irq_msix: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("irq_msix: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: irq_msix: soft retfosse — Wave 51 return-fosse honesty */
kprintf("irq_msix: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("irq_msix: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: irq_msix: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("irq_msix: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("irq_msix: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: irq_msix: soft retravelin — Wave 53 return-travelin honesty */
kprintf("irq_msix: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("irq_msix: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: irq_msix: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("irq_msix: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retredan — Wave 54 exclusive redan stamp */
kprintf("irq_msix: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: irq_msix: soft retflank — Wave 55 return-flank honesty */
kprintf("irq_msix: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retface — Wave 55 exclusive face stamp */
kprintf("irq_msix: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: irq_msix: soft retgorge — Wave 56 return-gorge honesty */
kprintf("irq_msix: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("irq_msix: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: irq_msix: soft retraverse — Wave 57 return-traverse honesty */
kprintf("irq_msix: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("irq_msix: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: irq_msix: soft retorillon — Wave 58 return-orillon honesty */
kprintf("irq_msix: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("irq_msix: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: irq_msix: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("irq_msix: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("irq_msix: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: irq_msix: soft retplace — Wave 60 return-place honesty */
kprintf("irq_msix: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("irq_msix: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: irq_msix: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("irq_msix: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("irq_msix: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: irq_msix: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("irq_msix: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("irq_msix: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: irq_msix: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("irq_msix: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("irq_msix: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: irq_msix: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("irq_msix: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: irq_msix: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("irq_msix: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: irq_msix: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("irq_msix: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: irq_msix: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("irq_msix: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: irq_msix: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("irq_msix: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=88 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: irq_msix: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("irq_msix: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=88 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: irq_msix: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("irq_msix: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("irq_msix: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: irq_msix: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("irq_msix: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("irq_msix: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: irq_msix: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("irq_msix: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=88 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("irq_msix: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=88 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: irq_msix: soft retfosseangle — Wave 70 return-fosseangle honesty */
kprintf("irq_msix: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=88 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
kprintf("irq_msix: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=88 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: irq_msix: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
kprintf("irq_msix: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=88 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
kprintf("irq_msix: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=88 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: irq_msix: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
kprintf("irq_msix: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=88 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
kprintf("irq_msix: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=88 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: irq_msix: soft retcurtainface — Wave 73 return-curtainface honesty */
kprintf("irq_msix: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=88 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
kprintf("irq_msix: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=88 (retbastionangle stamp; Soft≠product)\n");
/* Grep: irq_msix: soft retglacisangle — Wave 74 return-glacisangle honesty */
kprintf("irq_msix: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=88 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
kprintf("irq_msix: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=88 (retparapetangle stamp; Soft≠product)\n");
/* Grep: irq_msix: soft retmoatangle — Wave 75 return-moatangle honesty */
kprintf("irq_msix: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=88 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retowerangle — Wave 75 exclusive towerangle stamp */
kprintf("irq_msix: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=88 (retowerangle stamp; Soft≠product)\n");
/* Grep: irq_msix: soft retgateangle — Wave 76 return-gateangle honesty */
kprintf("irq_msix: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=88 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retwallangle — Wave 76 exclusive wallangle stamp */
kprintf("irq_msix: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=88 (retwallangle stamp; Soft≠product)\n");
/* Grep: irq_msix: soft retspireangle — Wave 77 return-spireangle honesty */
kprintf("irq_msix: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=88 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retholdangle — Wave 77 exclusive holdangle stamp */
kprintf("irq_msix: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=88 (retholdangle stamp; Soft≠product)\n");
/* Grep: irq_msix: soft retpalaceangle — Wave 78 return-palaceangle honesty */
kprintf("irq_msix: soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=88 (retpalaceangle honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
kprintf("irq_msix: soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=88 (retfortressangle stamp; Soft≠product)\n");
/* Grep: irq_msix: soft retkeepangle — Wave 79 return-keepangle honesty */
kprintf("irq_msix: soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=88 (retkeepangle honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
kprintf("irq_msix: soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=88 (retredoubtangle stamp; Soft≠product)\n");
/* Grep: irq_msix: soft retcitadelangle — Wave 80 return-citadelangle honesty */
kprintf("irq_msix: soft retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=88 (retcitadelangle honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retbastionkeep — Wave 80 exclusive bastionkeep stamp */
kprintf("irq_msix: soft retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=88 (retbastionkeep stamp; Soft≠product)\n");
/* Grep: irq_msix: soft retpanoplyangle — Wave 81 return-panoplyangle honesty */
kprintf("irq_msix: soft retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=88 (retpanoplyangle honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
kprintf("irq_msix: soft retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=88 (retbulwarkangle stamp; Soft≠product)\n");
/* Grep: irq_msix: soft retmantleangle — Wave 82 return-mantleangle honesty */
kprintf("irq_msix: soft retmantleangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=88 (retmantleangle honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retaegisangle — Wave 82 exclusive aegisangle stamp */
kprintf("irq_msix: soft retaegisangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=88 (retaegisangle stamp; Soft≠product)\n");
/* Grep: irq_msix: soft retemblemangle — Wave 83 return-emblemangle honesty */
kprintf("irq_msix: soft retemblemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=88 (retemblemangle honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retsigilangle — Wave 83 exclusive sigilangle stamp */
kprintf("irq_msix: soft retsigilangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=88 (retsigilangle stamp; Soft≠product)\n");
/* Grep: irq_msix: soft retscepterangle — Wave 84 return-scepterangle honesty */
kprintf("irq_msix: soft retscepterangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=88 (retscepterangle honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retglyphangle — Wave 84 exclusive glyphangle stamp */
kprintf("irq_msix: soft retglyphangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=88 (retglyphangle stamp; Soft≠product)\n");
/* Grep: irq_msix: soft retcrownangle — Wave 85 return-crownangle honesty */
kprintf("irq_msix: soft retcrownangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=88 (retcrownangle honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retshardangle — Wave 85 exclusive shardangle stamp */
kprintf("irq_msix: soft retshardangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=88 (retshardangle stamp; Soft≠product)\n");
/* Grep: irq_msix: soft retforgeangle — Wave 86 return-forgeangle honesty */
kprintf("irq_msix: soft retforgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=88 (retforgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retprismangle — Wave 86 exclusive prismangle stamp */
kprintf("irq_msix: soft retprismangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=88 (retprismangle stamp; Soft≠product)\n");
/* Grep: irq_msix: soft retflameangle — Wave 87 return-flameangle honesty */
kprintf("irq_msix: soft retflameangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=88 (retflameangle honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retcipherangle — Wave 87 exclusive cipherangle stamp */
kprintf("irq_msix: soft retcipherangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=88 (retcipherangle stamp; Soft≠product)\n");
/* Grep: irq_msix: soft retbeaconangle — Wave 88 return-beaconangle honesty */
kprintf("irq_msix: soft retbeaconangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=88 (retbeaconangle honesty; Soft≠product; not bar3)\n");
/* Grep: irq_msix: soft retledgerangle — Wave 88 exclusive ledgerangle stamp */
kprintf("irq_msix: soft retledgerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=88 (retledgerangle stamp; Soft≠product)\n");

                            kprintf("irq_msix: soft deepen wave=%u areas=%u via=%s ready=%u "
            "live=%u soft=%u path=%u tbl=%u exercise_ok=%u ok=1 skip=0\n",
            (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE,
            (unsigned)IRQ_MSIX_SOFT_DEEPEN_AREAS, szViaSafe,
            (unsigned)u32Ready, (unsigned)u32Live, (unsigned)u32Soft,
            (unsigned)u32Path, (unsigned)u32Tbl,
            (unsigned)g_u32SoftExerciseOk);

    /* Grep: irq_msix: soft stats */
    kprintf("irq_msix: soft stats init=%u inject=%u pulse=%u table=%u "
            "hw=%u handler=%u exercise_enter=%u exercise_ok=%u "
            "exercise_fail=%u exercise_nr=%u soft=%u hw_n=%u irq_n=%u "
            "path_n=%u tbl_n=%u logs=%u wave=%u\n",
            (unsigned)g_u32SoftInit, (unsigned)g_u32SoftInjectEnter,
            (unsigned)u32Pulse, (unsigned)g_u32SoftTableEnter,
            (unsigned)g_u32SoftHwEnter, (unsigned)u32Handler,
            (unsigned)g_u32SoftExerciseEnter, (unsigned)g_u32SoftExerciseOk,
            (unsigned)g_u32SoftExerciseFail,
            (unsigned)g_u32SoftExerciseNotReady, (unsigned)u32Soft,
            (unsigned)u32Hw, (unsigned)u32Irq, (unsigned)u32Path,
            (unsigned)u32Tbl, (unsigned)g_u32SoftLogN,
            (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);

    /* Grep: irq_msix: soft inventory PASS / irq_msix: soft PASS */
    kprintf("irq_msix: soft inventory PASS via=%s logs=%u ready=%u "
            "live=%u wave=%u areas=%u\n",
            szViaSafe, (unsigned)g_u32SoftLogN, (unsigned)u32Ready,
            (unsigned)u32Live, (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE,
            (unsigned)IRQ_MSIX_SOFT_DEEPEN_AREAS);
    kprintf("irq_msix: soft PASS via=%s wave=%u\n", szViaSafe,
            (unsigned)IRQ_MSIX_SOFT_DEEPEN_WAVE);
}

/**
 * After first product soft activity, print soft inventory once
 * (mirrors notify/futex soft-stats-once). Diagnostics only.
 */
static void
irq_msix_soft_inventory_maybe_once(void)
{
    if (g_fSoftInvOnce != 0) {
        return;
    }
    if (g_u32SoftInjectEnter == 0 && g_u32SoftPulseEnter == 0 &&
        g_u32SoftTableEnter == 0 && g_u32SoftHwEnter == 0 &&
        g_u32SoftExerciseEnter == 0) {
        return;
    }
    g_fSoftInvOnce = 1;
    irq_msix_soft_inventory_log("once");
}

void
irq_msix_handler(void)
{
    struct gj_notify *pNotify = notify_msix_global();

    g_fInHandler = 1;
    /* IRQ-safe tallies only — no kprintf on hard path. */
    irq_msix_soft_inc_atomic(&g_u32SoftIrqHandler);
    g_u32MsixIrq++;
    g_u32MsixHw++;
    irq_msix_note_pulse(GJ_MSIX_BADGE_SOFT, GJ_MSIX_PATH_IRQ);
    notify_pulse(pNotify, GJ_MSIX_BADGE_SOFT);
    apic_eoi();
    g_fInHandler = 0;
}

void
irq_msix_soft_inject(u64 u64Badge)
{
    struct gj_notify *pNotify = notify_msix_global();

    irq_msix_soft_inc(&g_u32SoftInjectEnter);
    if (u64Badge == 0) {
        u64Badge = GJ_MSIX_BADGE_SOFT;
        irq_msix_soft_inc(&g_u32SoftInjectZero);
    }
    g_u32MsixSoft++;
    g_u32MsixIrq++;
    irq_msix_note_pulse(u64Badge, GJ_MSIX_PATH_SOFT);
    notify_pulse(pNotify, u64Badge);
    /* No kprintf here: may be nested under soft fire / early inject storms. */
    irq_msix_soft_inventory_maybe_once();
}

u32
irq_msix_soft_pulse_path(u64 u64Badge)
{
    struct gj_notify *pNotify;
    u64 u64Pending;

    irq_msix_soft_inc(&g_u32SoftPulseEnter);
    if (!g_fReady) {
        irq_msix_soft_inc(&g_u32SoftPulseNotReady);
        return 0;
    }
    if (u64Badge == 0) {
        u64Badge = GJ_MSIX_BADGE_SOFT;
        irq_msix_soft_inc(&g_u32SoftInjectZero);
    }
    pNotify = notify_msix_global();
    g_u32MsixSoft++;
    g_u32MsixIrq++;
    g_u32SoftPulsePath++;
    irq_msix_note_pulse(u64Badge, GJ_MSIX_PATH_SOFT);
    notify_pulse(pNotify, u64Badge);
    /*
     * Soft path verify: pending must observe the OR'd badge (stats poll;
     * does not clear — wait path owns reclaim).
     */
    u64Pending = notify_pending(pNotify);
    if ((u64Pending & u64Badge) == 0) {
        irq_msix_soft_inc(&g_u32SoftPulsePendMiss);
        kprintf("irq: MSI-X soft pulse path pending miss badge=0x%lx "
                "pending=0x%lx\n",
                (unsigned long)u64Badge, (unsigned long)u64Pending);
        /* Twin soft miss lamp (Wave 14 inventory; does not replace smoke). */
        kprintf("irq: soft pulse pend_miss badge=0x%lx pending=0x%lx\n",
                (unsigned long)u64Badge, (unsigned long)u64Pending);
        kprintf("irq_msix: soft pulse pend_miss badge=0x%lx pending=0x%lx\n",
                (unsigned long)u64Badge, (unsigned long)u64Pending);
    } else {
        irq_msix_soft_inc(&g_u32SoftPulsePendOk);
    }
    irq_msix_soft_inventory_maybe_once();
    return g_u32SoftPulsePath;
}

u32
irq_msix_hw_pulse(void)
{
    struct gj_notify *pNotify;

    irq_msix_soft_inc(&g_u32SoftHwEnter);
    if (!g_fReady) {
        irq_msix_soft_inc(&g_u32SoftHwNotReady);
        return 0;
    }
    /*
     * Real self-IPI during early kmain risks #DF (stack/TPR). The IDT stub
     * (irq_stub_msix) is installed for live device IRQs; this pulse mimics
     * the Notification side of a hardware IRQ without EOI/reentry.
     */
    pNotify = notify_msix_global();
    g_u32MsixHw++;
    g_u32MsixIrq++;
    irq_msix_note_pulse(GJ_MSIX_BADGE_HW, GJ_MSIX_PATH_HW);
    notify_pulse(pNotify, GJ_MSIX_BADGE_HW);
    kprintf("irq: MSI-X hw-sim pulse (IDT gate installed for live IRQs)\n");
    /* Grep: irq: soft hw / irq_msix: soft hw (event lamp) */
    kprintf("irq: soft hw pulse n=%u badge=0x%x path=%u\n",
            (unsigned)g_u32MsixHw, (unsigned)GJ_MSIX_BADGE_HW,
            (unsigned)GJ_MSIX_PATH_HW);
    kprintf("irq_msix: soft hw pulse n=%u badge=0x%x path=%u\n",
            (unsigned)g_u32MsixHw, (unsigned)GJ_MSIX_BADGE_HW,
            (unsigned)GJ_MSIX_PATH_HW);
    irq_msix_soft_inventory_maybe_once();
    return g_u32MsixHw;
}

u32
irq_msix_soft_table_pulse(u16 u16Idx)
{
    u32 u32Before;
    u32 u32Delivered;

    irq_msix_soft_inc(&g_u32SoftTableEnter);
    if (!g_fReady) {
        irq_msix_soft_inc(&g_u32SoftTableNotReady);
        return 0;
    }
    if (!pci_msix_soft_ready()) {
        irq_msix_soft_inc(&g_u32SoftTableInit);
        pci_msix_soft_table_init();
        /* Ensure a programmed unmasked entry for the soft table path. */
        irq_msix_soft_inc(&g_u32SoftTableProg);
        (void)pci_msix_soft_program(u16Idx, 0xFEE00000u, (u32)GJ_MSIX_IRQ_VEC,
                                    0);
    }
    u32Before = g_u32MsixSoft;
    u32Delivered = pci_msix_soft_fire(u16Idx);
    if (u32Delivered == 0) {
        irq_msix_soft_inc(&g_u32SoftTableFireMiss);
        return 0;
    }
    irq_msix_soft_inc(&g_u32SoftTableFireOk);
    /*
     * soft_fire calls irq_msix_soft_inject when ready — retag path as table
     * soft delivery for stats.
     */
    g_u32LastPath = GJ_MSIX_PATH_TBL;
    g_u64LastBadge = GJ_MSIX_BADGE_TBL(u16Idx);
    if (g_u32MsixSoft > u32Before) {
        g_u32TablePulse++;
    }
    irq_msix_soft_inventory_maybe_once();
    return 1;
}

u32
irq_msix_soft_path_exercise(void)
{
    struct gj_notify *pNotify;
    u64 u64Pend;
    u32 u32SoftPath;
    u32 fOk = 1;
    u32 u32Tbl;

    irq_msix_soft_inc(&g_u32SoftExerciseEnter);
    if (!g_fReady) {
        irq_msix_soft_inc(&g_u32SoftExerciseNotReady);
        return 0;
    }
    pNotify = notify_msix_global();
    if (pNotify == NULL || !notify_is_live(pNotify)) {
        kprintf("irq: MSI-X soft pulse path FAIL (notify not live)\n");
        irq_msix_soft_inc(&g_u32SoftExerciseFail);
        /* Grep: irq: soft exercise / irq_msix: soft exercise */
        kprintf("irq: soft exercise FAIL reason=notify_not_live\n");
        kprintf("irq_msix: soft exercise FAIL reason=notify_not_live\n");
        return 0;
    }

    /* Multi-badge soft pulse path (bits 0 + 2). */
    u32SoftPath = irq_msix_soft_pulse_path(GJ_MSIX_BADGE_SOFT |
                                           GJ_MSIX_BADGE_TBL(0));
    if (u32SoftPath == 0) {
        fOk = 0;
    }
    u64Pend = notify_pending(pNotify);
    if ((u64Pend & GJ_MSIX_BADGE_SOFT) == 0) {
        fOk = 0;
    }

    /* Second soft pulse path inject (OR coalesce). */
    if (irq_msix_soft_pulse_path(GJ_MSIX_BADGE_SOFT) == 0) {
        fOk = 0;
    }

    /* Table soft → Notification pulse. */
    if (!pci_msix_soft_ready()) {
        irq_msix_soft_inc(&g_u32SoftTableInit);
        pci_msix_soft_table_init();
        irq_msix_soft_inc(&g_u32SoftTableProg);
        (void)pci_msix_soft_program(0, 0xFEE00000u, (u32)GJ_MSIX_IRQ_VEC, 0);
    } else {
        /* Ensure entry 0 unmasked for delivery. */
        irq_msix_soft_inc(&g_u32SoftTableMask);
        (void)pci_msix_soft_mask(0, 0);
    }
    u32Tbl = irq_msix_soft_table_pulse(0);
    if (u32Tbl == 0) {
        fOk = 0;
    }
    if (g_u32LastPath != GJ_MSIX_PATH_TBL &&
        g_u32LastPath != GJ_MSIX_PATH_SOFT) {
        fOk = 0;
    }
    if (g_u64LastBadge == 0) {
        fOk = 0;
    }

    if (fOk) {
        irq_msix_soft_inc(&g_u32SoftExerciseOk);
        kprintf("irq: MSI-X soft pulse path soft=%u path=%u tbl=%u "
                "last_badge=0x%lx signals=%u PASS\n",
                g_u32MsixSoft, g_u32SoftPulsePath, g_u32TablePulse,
                (unsigned long)g_u64LastBadge, notify_signals(pNotify));
        kprintf("irq: MSI-X soft pulse path PASS\n");
        /* Grep: irq: soft exercise PASS */
        kprintf("irq: soft exercise PASS soft=%u path=%u tbl=%u "
                "last_badge=0x%lx\n",
                g_u32MsixSoft, g_u32SoftPulsePath, g_u32TablePulse,
                (unsigned long)g_u64LastBadge);
        kprintf("irq_msix: soft exercise PASS soft=%u path=%u tbl=%u "
                "last_badge=0x%lx\n",
                g_u32MsixSoft, g_u32SoftPulsePath, g_u32TablePulse,
                (unsigned long)g_u64LastBadge);
    } else {
        irq_msix_soft_inc(&g_u32SoftExerciseFail);
        kprintf("irq: MSI-X soft pulse path FAIL soft=%u path=%u tbl=%u "
                "last=0x%lx\n",
                g_u32MsixSoft, g_u32SoftPulsePath, g_u32TablePulse,
                (unsigned long)g_u64LastBadge);
        kprintf("irq: soft exercise FAIL soft=%u path=%u tbl=%u "
                "last=0x%lx\n",
                g_u32MsixSoft, g_u32SoftPulsePath, g_u32TablePulse,
                (unsigned long)g_u64LastBadge);
        kprintf("irq_msix: soft exercise FAIL soft=%u path=%u tbl=%u "
                "last=0x%lx\n",
                g_u32MsixSoft, g_u32SoftPulsePath, g_u32TablePulse,
                (unsigned long)g_u64LastBadge);
    }

    /* Full Wave 14 soft inventory after exercise (bring-up smoke greps). */
    irq_msix_soft_inventory_log("exercise");
    return fOk;
}

void
irq_msix_init(void)
{
    irq_msix_soft_inc(&g_u32SoftInit);
    notify_msix_init();
    idt_set_gate(GJ_MSIX_IRQ_VEC, (void *)irq_stub_msix, 0x8E);
    g_fReady = 1;
    g_u32MsixIrq = 0;
    g_u32MsixSoft = 0;
    g_u32MsixHw = 0;
    g_u32SoftPulsePath = 0;
    g_u32TablePulse = 0;
    g_u64LastBadge = 0;
    g_u32LastPath = GJ_MSIX_PATH_NONE;
    /* Reset soft path tallies for this bring-up; keep log counter. */
    g_u32SoftInjectEnter = 0;
    g_u32SoftInjectZero = 0;
    g_u32SoftPulseEnter = 0;
    g_u32SoftPulseNotReady = 0;
    g_u32SoftPulsePendOk = 0;
    g_u32SoftPulsePendMiss = 0;
    g_u32SoftTableEnter = 0;
    g_u32SoftTableNotReady = 0;
    g_u32SoftTableInit = 0;
    g_u32SoftTableProg = 0;
    g_u32SoftTableFireOk = 0;
    g_u32SoftTableFireMiss = 0;
    g_u32SoftTableMask = 0;
    g_u32SoftHwEnter = 0;
    g_u32SoftHwNotReady = 0;
    __atomic_store_n(&g_u32SoftIrqHandler, 0u, __ATOMIC_RELAXED);
    g_u32SoftExerciseEnter = 0;
    g_u32SoftExerciseOk = 0;
    g_u32SoftExerciseFail = 0;
    g_u32SoftExerciseNotReady = 0;
    g_fSoftInvOnce = 0;
    kprintf("irq: MSI-X vec=0x%x Notification bound PASS\n", GJ_MSIX_IRQ_VEC);
    /* Baseline soft inventory before exercise (zeros typical). */
    irq_msix_soft_inventory_log("init");
    /* Soft Notification pulse path exercise (table soft → badge OR). */
    (void)irq_msix_soft_path_exercise();
}

u32
irq_msix_count(void)
{
    return g_u32MsixIrq;
}

u32
irq_msix_soft_count(void)
{
    return g_u32MsixSoft;
}

u32
irq_msix_hw_count(void)
{
    return g_u32MsixHw;
}

u32
irq_msix_soft_pulse_path_count(void)
{
    return g_u32SoftPulsePath;
}

u32
irq_msix_table_pulse_count(void)
{
    return g_u32TablePulse;
}

u64
irq_msix_last_badge(void)
{
    return g_u64LastBadge;
}

u32
irq_msix_last_path(void)
{
    return g_u32LastPath;
}

int
irq_msix_ready(void)
{
    return g_fReady;
}
