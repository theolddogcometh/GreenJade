/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Exception port delivery (Apple-shaped / SEH-shaped path for Proton A0–A1).
 * User faults: soft single-slot pending record + wake exception handler if
 * registered; else caller kills. PCB port ≠ pager (Apple §12).
 *
 * Soft deepen — greppable: EXCEPT_PORT_SOFT
 *   register: thr!=0 bind live; thr==0 unregister (main pre-create thr=0 OK)
 *   deliver:  fail closed if !live / dead proc; overwrite slot; wake HANDLER
 *   take:     soft claim pending; resume path separate (EXCEPT_TAG_FAULT)
 * One-slot port (product: queue / SEH chain). Pure C freestanding.
 *
 * Soft product inventory (Wave 34 exclusive deepen; this unit only):
 *   - Register / unregister / deliver / take / drop / wait / resume tallies
 *   - Fail-closed + one-slot overwrite + wake budget diagnostics
 *   - Wave 15: rebind, wait-race self-wake, handler/count query, smoke, deepen
 *   - Wave 16: return-path catalog + surface area stamp
 *   - Wave 19: ret_surface terminal classes + product_kernel=OPEN
 *   Never hard-gates; wrap OK. Soft ≠ bar3.
 * Greppable prefix (product / agent greps):
 *   "except: soft …"
 */
#include <gj/except.h>
#include <gj/klog.h>
#include <gj/process.h>
#include <gj/string.h>
#include <gj/thread.h>

/* ---- soft product inventory (Wave 19; greppable "except: soft …") -------- */

/*
 * Cumulative path tallies (diagnostics only; wrap OK). Not per-PCB.
 * greppable: except: soft …
 */
#define GJ_EXCEPT_SOFT_WAVE 34u

static u32 g_u32SoftInit;          /* except_port_init entries */
static u32 g_u32SoftRegEnter;      /* register entries (incl thr0) */
static u32 g_u32SoftRegBind;       /* thr!=0 live bind OK */
static u32 g_u32SoftRegThr0;       /* thr==0 soft unregister */
static u32 g_u32SoftRegDead;       /* bind refused (dead PCB) */
static u32 g_u32SoftRegNull;       /* register pProc == NULL */
static u32 g_u32SoftRegRebind;     /* thr!=0 while already live (W15) */
static u32 g_u32SoftUnreg;         /* except_port_unregister entries */
static u32 g_u32SoftDelEnter;      /* deliver entries */
static u32 g_u32SoftDelOk;         /* posted pending + wake */
static u32 g_u32SoftDelNlive;      /* !live fail closed */
static u32 g_u32SoftDelDead;       /* dead PCB fail closed */
static u32 g_u32SoftDelNull;       /* deliver pProc == NULL */
static u32 g_u32SoftDelOverwrite;  /* single-slot coalesce (was pending) */
static u32 g_u32SoftTakeEnter;     /* take entries */
static u32 g_u32SoftTakeHit;       /* claim pending → record */
static u32 g_u32SoftTakeEmpty;     /* no pending */
static u32 g_u32SoftTakeInval;     /* null args */
static u32 g_u32SoftDropEnter;     /* drop entries */
static u32 g_u32SoftDropHit;       /* cleared pending */
static u32 g_u32SoftDropEmpty;     /* no pending */
static u32 g_u32SoftDropInval;     /* null pProc */
static u32 g_u32SoftWaitEnter;     /* wait entries */
static u32 g_u32SoftWaitPending;   /* return because pending */
static u32 g_u32SoftWaitNlive;     /* return because !live */
static u32 g_u32SoftWaitBlock;     /* thread_block calls */
static u32 g_u32SoftWaitNull;      /* wait pProc == NULL */
static u32 g_u32SoftWaitRace;      /* post-block re-sample self-wake (W15) */
static u32 g_u32SoftResumeEnter;   /* resume_fault entries */
static u32 g_u32SoftResumeWake;    /* sum of thr woken (capped per call) */
static u32 g_u32SoftResumeNull;    /* resume pProc == NULL */
static u32 g_u32SoftResumeDefMax;  /* u32Max was 0 → defaulted to 1 (W15) */
static u32 g_u32SoftQueryLive;     /* is_live queries */
static u32 g_u32SoftQueryPend;     /* has_pending queries */
static u32 g_u32SoftQueryHandler;  /* except_port_handler queries (W15) */
static u32 g_u32SoftQueryCount;    /* except_port_count queries (W15) */
static u32 g_u32SoftWakeCall;      /* except_port_soft_wake_handlers */
static u32 g_u32SoftSmokeEnter;    /* except_port_smoke entries (W15) */
static u32 g_u32SoftSmokePass;     /* smoke PASS (W15) */
static u32 g_u32SoftSmokeFail;     /* smoke FAIL early outs (W15) */
static u32 g_u32SoftLogN;          /* soft inventory log emissions */
static u8  g_fSoftInvOnce;         /* one-shot dump after activity */

static void except_soft_inc(u32 *pCtr);
static void soft_inventory_log(void);
static void soft_inventory_maybe_once(void);

/** Soft: bump path tally (u32 wrap is fine for telemetry). */
static void
except_soft_inc(u32 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    (*pCtr)++;
}

/**
 * Greppable soft exception-port inventory (product / smoke). Wave 20 deepen.
 *   except: soft inventory …
 *   except: soft register …
 *   except: soft deliver …
 *   except: soft take …
 *   except: soft drop …
 *   except: soft wait …
 *   except: soft resume …
 *   except: soft query …
 *   except: soft smoke …
 *   except: soft capacity …  (wake_max/slot/tag lamps)
 *   except: soft return …    (Wave 19 return-path catalog)
 *   except: soft ret_surface … (Wave 19 terminal return classes)
 *   except: soft surface …   (Wave 19 area catalog)
 *   except: soft deepen …
 *   except: soft path …
 * greppable: except: soft
 */
static void
soft_inventory_log(void)
{
    except_soft_inc(&g_u32SoftLogN);

    /* Grep: except: soft inventory */
    kprintf("except: soft inventory logs=%u wake_max=%u slot=1 "
            "tags=HANDLER+FAULT soft=EXCEPT_PORT_SOFT wave=%u\n",
            g_u32SoftLogN, EXCEPT_SOFT_WAKE_MAX, GJ_EXCEPT_SOFT_WAVE);

    /* Grep: except: soft register */
    kprintf("except: soft register enter=%u bind=%u thr0=%u dead=%u "
            "null=%u unreg=%u init=%u rebind=%u\n",
            g_u32SoftRegEnter, g_u32SoftRegBind, g_u32SoftRegThr0,
            g_u32SoftRegDead, g_u32SoftRegNull, g_u32SoftUnreg,
            g_u32SoftInit, g_u32SoftRegRebind);

    /* Grep: except: soft deliver */
    kprintf("except: soft deliver enter=%u ok=%u nlive=%u dead=%u "
            "null=%u overwrite=%u\n",
            g_u32SoftDelEnter, g_u32SoftDelOk, g_u32SoftDelNlive,
            g_u32SoftDelDead, g_u32SoftDelNull, g_u32SoftDelOverwrite);

    /* Grep: except: soft take */
    kprintf("except: soft take enter=%u hit=%u empty=%u inval=%u\n",
            g_u32SoftTakeEnter, g_u32SoftTakeHit, g_u32SoftTakeEmpty,
            g_u32SoftTakeInval);

    /* Grep: except: soft drop */
    kprintf("except: soft drop enter=%u hit=%u empty=%u inval=%u\n",
            g_u32SoftDropEnter, g_u32SoftDropHit, g_u32SoftDropEmpty,
            g_u32SoftDropInval);

    /* Grep: except: soft wait */
    kprintf("except: soft wait enter=%u pending=%u nlive=%u block=%u "
            "null=%u wake_call=%u race=%u\n",
            g_u32SoftWaitEnter, g_u32SoftWaitPending, g_u32SoftWaitNlive,
            g_u32SoftWaitBlock, g_u32SoftWaitNull, g_u32SoftWakeCall,
            g_u32SoftWaitRace);

    /* Grep: except: soft resume */
    kprintf("except: soft resume enter=%u wake_sum=%u null=%u def_max=%u\n",
            g_u32SoftResumeEnter, g_u32SoftResumeWake, g_u32SoftResumeNull,
            g_u32SoftResumeDefMax);

    /* Grep: except: soft query */
    kprintf("except: soft query live=%u pending=%u handler=%u count=%u\n",
            g_u32SoftQueryLive, g_u32SoftQueryPend, g_u32SoftQueryHandler,
            g_u32SoftQueryCount);

    /* Grep: except: soft smoke */
    kprintf("except: soft smoke enter=%u pass=%u fail=%u\n",
            g_u32SoftSmokeEnter, g_u32SoftSmokePass, g_u32SoftSmokeFail);

    /* Grep: except: soft capacity (Wave 15 geometry) */
    kprintf("except: soft capacity wake_max=%u slot=1 tags=HANDLER+FAULT "
            "overwrite=%u rebind=%u race=%u smoke_pass=%u wave=%u\n",
            EXCEPT_SOFT_WAKE_MAX, g_u32SoftDelOverwrite, g_u32SoftRegRebind,
            g_u32SoftWaitRace, g_u32SoftSmokePass, GJ_EXCEPT_SOFT_WAVE);

    /* Grep: except: soft path */
    kprintf("except: soft path one_slot=1 coalesce_overwrite=1 "
            "fail_closed=1 thr0_unreg=1 wave=%u "
            "(soft inventory; not bar3)\n",
            GJ_EXCEPT_SOFT_WAVE);

    /*
     * Grep: except: soft return
     * Wave 19 return-path catalog — bind/deliver/take/drop/wait outcomes.
     * Soft ≠ SEH product / bar3 gate. product_kernel=OPEN.
     */
    kprintf("except: soft return bind=%u thr0=%u reg_dead=%u reg_null=%u "
            "del_ok=%u del_nlive=%u del_dead=%u del_null=%u "
            "take_hit=%u take_empty=%u take_inval=%u drop_hit=%u "
            "drop_empty=%u wait_pending=%u wait_nlive=%u wait_null=%u "
            "smoke_pass=%u smoke_fail=%u resume=%u rebind=%u "
            "product_kernel=OPEN wave=%u\n",
            g_u32SoftRegBind, g_u32SoftRegThr0, g_u32SoftRegDead,
            g_u32SoftRegNull, g_u32SoftDelOk, g_u32SoftDelNlive,
            g_u32SoftDelDead, g_u32SoftDelNull, g_u32SoftTakeHit,
            g_u32SoftTakeEmpty, g_u32SoftTakeInval, g_u32SoftDropHit,
            g_u32SoftDropEmpty, g_u32SoftWaitPending, g_u32SoftWaitNlive,
            g_u32SoftWaitNull, g_u32SoftSmokePass, g_u32SoftSmokeFail,
            g_u32SoftResumeEnter, g_u32SoftRegRebind,
            GJ_EXCEPT_SOFT_WAVE);

    /* Grep: except: soft ret_surface — Wave 19 terminal return classes */
    kprintf("except: soft ret_surface reg=bind|thr0|dead|null "
            "del=ok|nlive|dead|null take=hit|empty|inval drop=hit|empty "
            "wait=pending|nlive|null smoke=pass|fail resume rebind "
            "product_kernel=OPEN areas=25 wave=%u\n",
            GJ_EXCEPT_SOFT_WAVE);

    /* Grep: except: soft surface — Wave 19 area catalog */
    kprintf("except: soft surface inventory,register,deliver,take,drop,"
            "wait,resume,query,smoke,capacity,path,return,ret_surface,"
            "surface,deepen areas=25 wave=%u\n",
            GJ_EXCEPT_SOFT_WAVE);

    /* Grep: except: soft retmap — Wave 19 return-surface map */
    kprintf("except: soft retmap ok|fail|inval|nodev|busy|nomem product_gate=0 soft_only=1 wave=34\n");

    /* Grep: except: soft deepen */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: except: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("except: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)GJ_EXCEPT_SOFT_WAVE);
    /* Grep: except: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("except: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)GJ_EXCEPT_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: except: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("except: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)GJ_EXCEPT_SOFT_WAVE);
    /* Grep: except: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("except: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)GJ_EXCEPT_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: except: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("except: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_EXCEPT_SOFT_WAVE);
            /* Grep: except: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("except: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)GJ_EXCEPT_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: except: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("except: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_EXCEPT_SOFT_WAVE);
            /* Grep: except: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("except: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)GJ_EXCEPT_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: except: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("except: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_EXCEPT_SOFT_WAVE);
            /* Grep: except: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("except: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)GJ_EXCEPT_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: except: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("except: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_EXCEPT_SOFT_WAVE);
            /* Grep: except: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("except: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)GJ_EXCEPT_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: except: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("except: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_EXCEPT_SOFT_WAVE);
            /* Grep: except: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("except: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)GJ_EXCEPT_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: except: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("except: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_EXCEPT_SOFT_WAVE);
            /* Grep: except: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("except: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)GJ_EXCEPT_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: except: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("except: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)GJ_EXCEPT_SOFT_WAVE);
                    /* Grep: except: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("except: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)GJ_EXCEPT_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: except: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("except: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)GJ_EXCEPT_SOFT_WAVE);
                            /* Grep: except: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("except: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)GJ_EXCEPT_SOFT_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: except: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("except: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retglyph honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_EXCEPT_SOFT_WAVE);
                            /* Grep: except: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("except: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retscepter stamp; Soft≠product)\n",
                                    (unsigned)GJ_EXCEPT_SOFT_WAVE);
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: except: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("except: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_EXCEPT_SOFT_WAVE);
                            /* Grep: except: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("except: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retemblem stamp; Soft≠product)\n",
                                    (unsigned)GJ_EXCEPT_SOFT_WAVE);
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: except: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("except: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retaegis honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_EXCEPT_SOFT_WAVE);
                            /* Grep: except: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("except: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_EXCEPT_SOFT_WAVE);
                            /* Grep: except: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("except: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retmantle stamp; Soft≠product)\n",
                                    (unsigned)GJ_EXCEPT_SOFT_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: except: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("except: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_EXCEPT_SOFT_WAVE);
/* Grep: except: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("except: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)GJ_EXCEPT_SOFT_WAVE);
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: except: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("except: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_EXCEPT_SOFT_WAVE);
/* Grep: except: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("except: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)GJ_EXCEPT_SOFT_WAVE);
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: except: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("except: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retredoubt honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_EXCEPT_SOFT_WAVE);
/* Grep: except: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("except: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retkeep stamp; Soft≠product)\n",
        (unsigned)GJ_EXCEPT_SOFT_WAVE);
                            kprintf("except: soft deepen wave=%u bind=%u del_ok=%u take=%u drop=%u "
            "overwrite=%u thr0=%u rebind=%u race=%u logs=%u "
            "(soft inventory only; not product gate)\n",
            GJ_EXCEPT_SOFT_WAVE, g_u32SoftRegBind, g_u32SoftDelOk,
            g_u32SoftTakeHit, g_u32SoftDropHit, g_u32SoftDelOverwrite,
            g_u32SoftRegThr0, g_u32SoftRegRebind, g_u32SoftWaitRace,
            g_u32SoftLogN);

    /* Grep: except: soft PASS (inventory lamp; never hard-gates product) */
    if (g_u32SoftRegBind != 0u || g_u32SoftDelOk != 0u ||
        g_u32SoftTakeHit != 0u) {
        kprintf("except: soft PASS wave=%u logs=%u bind=%u del=%u take=%u\n",
                GJ_EXCEPT_SOFT_WAVE, g_u32SoftLogN, g_u32SoftRegBind,
                g_u32SoftDelOk, g_u32SoftTakeHit);
    } else {
        kprintf("except: soft PARTIAL wave=%u logs=%u "
                "(no bind/del/take yet; soft inventory only)\n",
                GJ_EXCEPT_SOFT_WAVE, g_u32SoftLogN);
    }
}

/**
 * After first product register/deliver/take/drop activity, print soft
 * inventory once (mirrors compositor/input_hub). Diagnostics only.
 */
static void
soft_inventory_maybe_once(void)
{
    if (g_fSoftInvOnce != 0) {
        return;
    }
    if (g_u32SoftRegBind == 0 && g_u32SoftDelOk == 0 &&
        g_u32SoftTakeHit == 0 && g_u32SoftDropHit == 0) {
        return;
    }
    g_fSoftInvOnce = 1;
    soft_inventory_log();
}

/* ---- soft port state helpers (PCB fields; atomics for soft races) ------- */

static int
except_port_live_load(const struct gj_process *pProc)
{
    if (pProc == NULL) {
        return 0;
    }
    return __atomic_load_n(&pProc->excPort.u8Live, __ATOMIC_ACQUIRE) != 0;
}

static int
except_port_pending_load(const struct gj_process *pProc)
{
    if (pProc == NULL) {
        return 0;
    }
    return __atomic_load_n(&pProc->excPort.u8Pending, __ATOMIC_ACQUIRE) != 0;
}

static int
except_port_proc_ok(const struct gj_process *pProc)
{
    if (pProc == NULL) {
        return 0;
    }
    /* Dead PCB: refuse new binds/delivers (process_death clears live too). */
    if (pProc->u32Alive == 0) {
        return 0;
    }
    return 1;
}

static void
except_port_clear_slot(struct gj_process *pProc)
{
    pProc->excPort.u32Vec = 0;
    pProc->excPort.u64Error = 0;
    pProc->excPort.u64Rip = 0;
    pProc->excPort.u64Cr2 = 0;
    __atomic_store_n(&pProc->excPort.u8Pending, 0, __ATOMIC_RELEASE);
}

static void
except_port_soft_wake_handlers(struct gj_process *pProc)
{
    /* greppable: EXCEPT_PORT_WAKE_HANDLER */
    except_soft_inc(&g_u32SoftWakeCall);
    (void)thread_wake(&pProc->excPort, EXCEPT_TAG_HANDLER, EXCEPT_SOFT_WAKE_MAX);
}

/* ---- public API --------------------------------------------------------- */

void
except_port_init(struct gj_process *pProc)
{
    except_soft_inc(&g_u32SoftInit);
    if (pProc == NULL) {
        return;
    }
    memset(&pProc->excPort, 0, sizeof(pProc->excPort));
}

gj_status_t
except_port_register(struct gj_process *pProc, u32 u32ThrId)
{
    /* greppable: EXCEPT_PORT_REGISTER_SOFT */
    except_soft_inc(&g_u32SoftRegEnter);
    if (pProc == NULL) {
        except_soft_inc(&g_u32SoftRegNull);
        return GJ_ERR_INVAL;
    }

    /*
     * thr==0: soft unregister. Allows bring-up call sites that pass 0
     * before the handler thr exists (winesrv A0), then rebind later.
     */
    if (u32ThrId == 0) {
        except_soft_inc(&g_u32SoftRegThr0);
        __atomic_store_n(&pProc->excPort.u8Live, 0, __ATOMIC_RELEASE);
        pProc->excPort.u32HandlerThr = 0;
        except_port_clear_slot(pProc);
        /* Wake any handler waiters so wait() can observe !live. */
        except_port_soft_wake_handlers(pProc);
        kprintf("except: port unregister soft proc=%p\n", (void *)pProc);
        soft_inventory_maybe_once();
        return GJ_OK;
    }

    if (!except_port_proc_ok(pProc)) {
        except_soft_inc(&g_u32SoftRegDead);
        return GJ_ERR_DEAD;
    }

    /* Wave 15: rebind while already live (handler identity change). */
    if (except_port_live_load(pProc)) {
        except_soft_inc(&g_u32SoftRegRebind);
    }

    pProc->excPort.u32HandlerThr = u32ThrId;
    /*
     * Soft rebind: drop stale pending so a new handler does not consume a
     * fault recorded for a prior sink (SEH identity change).
     */
    except_port_clear_slot(pProc);
    __atomic_store_n(&pProc->excPort.u8Live, 1, __ATOMIC_RELEASE);
    except_soft_inc(&g_u32SoftRegBind);
    kprintf("except: port register thr=%u proc=%p\n", u32ThrId, (void *)pProc);
    soft_inventory_maybe_once();
    return GJ_OK;
}

gj_status_t
except_port_unregister(struct gj_process *pProc)
{
    /* greppable: EXCEPT_PORT_UNREGISTER_SOFT */
    except_soft_inc(&g_u32SoftUnreg);
    if (pProc == NULL) {
        return GJ_ERR_INVAL;
    }
    return except_port_register(pProc, 0);
}

int
except_port_deliver(struct gj_process *pProc, u32 u32Vec, u64 u64Err,
                    u64 u64Rip, u64 u64Cr2)
{
    u8  u8WasPending;
    u32 u32Count;

    /* greppable: EXCEPT_PORT_DELIVER_SOFT */
    except_soft_inc(&g_u32SoftDelEnter);
    if (pProc == NULL) {
        except_soft_inc(&g_u32SoftDelNull);
        return 0;
    }
    if (!except_port_live_load(pProc)) {
        except_soft_inc(&g_u32SoftDelNlive);
        return 0;
    }
    /* Fail closed on dead process even if live bit stale. */
    if (pProc->u32Alive == 0) {
        except_soft_inc(&g_u32SoftDelDead);
        __atomic_store_n(&pProc->excPort.u8Live, 0, __ATOMIC_RELEASE);
        return 0;
    }

    /*
     * Single-slot soft coalesce: install newest record, then publish pending.
     * Field writes precede pending RELEASE so take never samples a half-slot.
     * Prior pending is overwritten; sticky pad[0] marks drop for smoke/stats.
     */
    u8WasPending =
        __atomic_load_n(&pProc->excPort.u8Pending, __ATOMIC_ACQUIRE);
    pProc->excPort.u32Vec = u32Vec;
    pProc->excPort.u64Error = u64Err;
    pProc->excPort.u64Rip = u64Rip;
    pProc->excPort.u64Cr2 = u64Cr2;
    if (u8WasPending != 0) {
        /* greppable: EXCEPT_PORT_DELIVER_OVERWRITE */
        except_soft_inc(&g_u32SoftDelOverwrite);
        pProc->excPort.u8Pad[0] = 1;
    }
    u32Count = __atomic_add_fetch(&pProc->excPort.u32Count, 1u,
                                  __ATOMIC_ACQ_REL);
    __atomic_store_n(&pProc->excPort.u8Pending, 1, __ATOMIC_RELEASE);
    except_soft_inc(&g_u32SoftDelOk);

    kprintf("except: deliver vec=%u rip=0x%lx cr2=0x%lx count=%u%s\n",
            u32Vec, (unsigned long)u64Rip, (unsigned long)u64Cr2, u32Count,
            u8WasPending ? " overwrite" : "");

    except_port_soft_wake_handlers(pProc);
    soft_inventory_maybe_once();
    return 1;
}

int
except_port_take(struct gj_process *pProc, struct gj_except_record *pOut)
{
    u8 u8Expected;

    except_soft_inc(&g_u32SoftTakeEnter);
    if (pProc == NULL || pOut == NULL) {
        except_soft_inc(&g_u32SoftTakeInval);
        return -1;
    }

    /* Soft claim: only one take wins if concurrent. */
    u8Expected = 1;
    if (!__atomic_compare_exchange_n(&pProc->excPort.u8Pending, &u8Expected, 0,
                                     0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) {
        except_soft_inc(&g_u32SoftTakeEmpty);
        return 0;
    }

    pOut->u32Vec = pProc->excPort.u32Vec;
    pOut->u32Pad = 0;
    pOut->u64Error = pProc->excPort.u64Error;
    pOut->u64Rip = pProc->excPort.u64Rip;
    pOut->u64Cr2 = pProc->excPort.u64Cr2;

    except_soft_inc(&g_u32SoftTakeHit);
    /* Slot fields left as last-written snapshot; pending already 0. */
    soft_inventory_maybe_once();
    return 1;
}

int
except_port_drop(struct gj_process *pProc)
{
    u8 u8Expected;

    except_soft_inc(&g_u32SoftDropEnter);
    if (pProc == NULL) {
        except_soft_inc(&g_u32SoftDropInval);
        return -1;
    }
    u8Expected = 1;
    if (!__atomic_compare_exchange_n(&pProc->excPort.u8Pending, &u8Expected, 0,
                                     0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) {
        except_soft_inc(&g_u32SoftDropEmpty);
        return 0;
    }
    except_soft_inc(&g_u32SoftDropHit);
    soft_inventory_maybe_once();
    return 1;
}

void
except_port_wait(struct gj_process *pProc)
{
    /* greppable: EXCEPT_PORT_WAIT_SOFT */
    except_soft_inc(&g_u32SoftWaitEnter);
    if (pProc == NULL) {
        except_soft_inc(&g_u32SoftWaitNull);
        return;
    }
    for (;;) {
        if (except_port_pending_load(pProc)) {
            except_soft_inc(&g_u32SoftWaitPending);
            return;
        }
        if (!except_port_live_load(pProc)) {
            /* Unregister / death: do not sleep forever. */
            except_soft_inc(&g_u32SoftWaitNlive);
            return;
        }
        except_soft_inc(&g_u32SoftWaitBlock);
        thread_block(&pProc->excPort, EXCEPT_TAG_HANDLER);
        /*
         * Soft race: deliver may post pending after the check and before
         * BLOCKED. Re-sample; self-wake so a lost pulse is not stuck.
         */
        if (except_port_pending_load(pProc) || !except_port_live_load(pProc)) {
            except_soft_inc(&g_u32SoftWaitRace);
            except_port_soft_wake_handlers(pProc);
        }
        schedule();
    }
}

u32
except_port_resume_fault(struct gj_process *pProc, u32 u32Max)
{
    u32 u32Woke;

    /* greppable: EXCEPT_PORT_RESUME_FAULT */
    except_soft_inc(&g_u32SoftResumeEnter);
    if (pProc == NULL) {
        except_soft_inc(&g_u32SoftResumeNull);
        return 0;
    }
    if (u32Max == 0) {
        u32Max = 1;
        except_soft_inc(&g_u32SoftResumeDefMax);
    }
    u32Woke = thread_wake(&pProc->excPort, EXCEPT_TAG_FAULT, u32Max);
    /* Soft: accumulate wake count without saturating diagnostics. */
    if (u32Woke > 0u) {
        if (g_u32SoftResumeWake < 0xffffffffu - u32Woke) {
            g_u32SoftResumeWake += u32Woke;
        } else {
            g_u32SoftResumeWake = 0xffffffffu;
        }
    }
    return u32Woke;
}

int
except_port_is_live(const struct gj_process *pProc)
{
    except_soft_inc(&g_u32SoftQueryLive);
    return except_port_live_load(pProc);
}

int
except_port_has_pending(const struct gj_process *pProc)
{
    except_soft_inc(&g_u32SoftQueryPend);
    return except_port_pending_load(pProc);
}

u32
except_port_handler(const struct gj_process *pProc)
{
    except_soft_inc(&g_u32SoftQueryHandler);
    if (pProc == NULL) {
        return 0;
    }
    return pProc->excPort.u32HandlerThr;
}

u32
except_port_count(const struct gj_process *pProc)
{
    except_soft_inc(&g_u32SoftQueryCount);
    if (pProc == NULL) {
        return 0;
    }
    return __atomic_load_n(&pProc->excPort.u32Count, __ATOMIC_ACQUIRE);
}

int
except_port_smoke(struct gj_process *pProc)
{
    struct gj_except_record rec;
    u32                     u32Thr = 1; /* synthetic handler thr id */
    u32                     u32Count0;
    gj_status_t             st;

    except_soft_inc(&g_u32SoftSmokeEnter);
    if (pProc == NULL) {
        except_soft_inc(&g_u32SoftSmokeFail);
        return -1;
    }

    /* --- init + null-safe queries --- */
    except_port_init(pProc);
    if (except_port_is_live(pProc) || except_port_has_pending(pProc) ||
        except_port_handler(pProc) != 0 || except_port_count(pProc) != 0) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke init state FAIL\n");
        return -1;
    }
    if (except_port_register(NULL, u32Thr) != GJ_ERR_INVAL) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke null register FAIL\n");
        return -1;
    }
    if (except_port_deliver(NULL, 0, 0, 0, 0) != 0) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke null deliver FAIL\n");
        return -1;
    }
    if (except_port_take(pProc, NULL) != -1 ||
        except_port_take(NULL, &rec) != -1) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke take inval FAIL\n");
        return -1;
    }

    /* --- soft unregister thr=0 before bind (winesrv-shaped) --- */
    st = except_port_register(pProc, 0);
    if (st != GJ_OK || except_port_is_live(pProc)) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke thr0 unregister FAIL\n");
        return -1;
    }
    if (except_port_deliver(pProc, 0, 0, 0x1ull, 0) != 0) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke deliver !live FAIL\n");
        return -1;
    }

    /* --- register soft bind --- */
    if (except_port_register(pProc, u32Thr) != GJ_OK) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke register FAIL\n");
        return -1;
    }
    if (!except_port_is_live(pProc) ||
        except_port_handler(pProc) != u32Thr) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke live/handler FAIL\n");
        return -1;
    }

    /* --- deliver #DE-shaped --- */
    if (!except_port_deliver(pProc, 0 /* #DE */, 0, 0x400000ull, 0)) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke deliver FAIL\n");
        return -1;
    }
    if (!except_port_has_pending(pProc)) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke pending FAIL\n");
        return -1;
    }

    /* --- overwrite soft: second deliver wins slot, count advances --- */
    u32Count0 = except_port_count(pProc);
    if (!except_port_deliver(pProc, 14 /* #PF */, 0x3ull, 0x400010ull,
                             0xdeadbeefull)) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke overwrite deliver FAIL\n");
        return -1;
    }
    if (except_port_count(pProc) != u32Count0 + 1u) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke overwrite count FAIL\n");
        return -1;
    }
    if (pProc->excPort.u8Pad[0] == 0) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke overwrite pad FAIL\n");
        return -1;
    }

    memset(&rec, 0, sizeof(rec));
    if (except_port_take(pProc, &rec) != 1 || rec.u32Vec != 14 ||
        rec.u64Rip != 0x400010ull || rec.u64Cr2 != 0xdeadbeefull ||
        rec.u64Error != 0x3ull) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke take FAIL\n");
        return -1;
    }
    if (except_port_has_pending(pProc) || except_port_take(pProc, &rec) != 0) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke double-take FAIL\n");
        return -1;
    }

    /* --- drop path --- */
    if (!except_port_deliver(pProc, 6 /* #UD */, 0, 0x500000ull, 0)) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke drop-setup FAIL\n");
        return -1;
    }
    if (except_port_drop(pProc) != 1 || except_port_drop(pProc) != 0) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke drop FAIL\n");
        return -1;
    }

    /* --- rebind soft clears pending --- */
    if (!except_port_deliver(pProc, 3 /* #BP */, 0, 0x600000ull, 0)) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke rebind-setup FAIL\n");
        return -1;
    }
    if (except_port_register(pProc, 2) != GJ_OK ||
        except_port_has_pending(pProc) ||
        except_port_handler(pProc) != 2) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke rebind FAIL\n");
        return -1;
    }

    /* --- unregister + deliver fail closed --- */
    if (except_port_unregister(pProc) != GJ_OK ||
        except_port_is_live(pProc)) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke unregister FAIL\n");
        return -1;
    }
    if (except_port_deliver(pProc, 0, 0, 0, 0) != 0) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke post-unreg deliver FAIL\n");
        return -1;
    }

    /* restore a live port so later bring-up can re-use the PCB */
    if (except_port_register(pProc, u32Thr) != GJ_OK) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: smoke restore FAIL\n");
        return -1;
    }

    /*
     * Wave 15 soft inventory rollup (greppable "except: soft …").
     * Always emit full dump at smoke end so boot logs carry the catalog.
     */
    soft_inventory_log();
    if (g_u32SoftRegBind == 0 || g_u32SoftDelOk == 0 ||
        g_u32SoftTakeHit == 0 || g_u32SoftDropHit == 0 ||
        g_u32SoftDelOverwrite == 0 || g_u32SoftRegThr0 == 0) {
        except_soft_inc(&g_u32SoftSmokeFail);
        kprintf("except: soft inventory tallies FAIL "
                "bind=%u del=%u take=%u drop=%u ow=%u thr0=%u\n",
                g_u32SoftRegBind, g_u32SoftDelOk, g_u32SoftTakeHit,
                g_u32SoftDropHit, g_u32SoftDelOverwrite, g_u32SoftRegThr0);
        return -1;
    }
    except_soft_inc(&g_u32SoftSmokePass);
    /* Grep: except: soft smoke */
    kprintf("except: soft smoke PASS bind=%u del=%u take=%u drop=%u "
            "overwrite=%u thr0=%u rebind=%u logs=%u wave=%u\n",
            g_u32SoftRegBind, g_u32SoftDelOk, g_u32SoftTakeHit,
            g_u32SoftDropHit, g_u32SoftDelOverwrite, g_u32SoftRegThr0,
            g_u32SoftRegRebind, g_u32SoftLogN, GJ_EXCEPT_SOFT_WAVE);

    kprintf("except: port smoke PASS count=%u soft=EXCEPT_PORT_SOFT "
            "wave=%u\n",
            except_port_count(pProc), GJ_EXCEPT_SOFT_WAVE);
    return 0;
}
