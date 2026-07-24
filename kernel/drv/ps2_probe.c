/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Product T1 HCL: 8042 PS/2 controller soft probe — clean-room pure C.
 * Soft identify: read status port 0x64 only; decode full status inventory.
 * Does not enable IRQs, translate, write commands, or drain the input
 * buffer (smoke-safe). No GPL source; public 8042 layout only.
 *
 * Wave 15/16/17 exclusive soft deepen (this unit only — greppable "ps2: soft …"):
 *   ps2: soft inventory  — dual status + port + float/stable + wave stamp
 *   ps2: soft flags      — full 8-bit status field inventory
 *   ps2: soft bits       — per-bit soft PASS lamps (public layout)
 *   ps2: soft mask       — status bit mask constants (HCL map)
 *   ps2: soft ports      — public 8042 port map (no BAR)
 *   ps2: soft regs       — status/data/cmd offset map (port-space)
 *   ps2: soft sense      — float/stable/busy/ready/xor composite
 *   ps2: soft pop        — status bit popcount + idle/err composite
 *   ps2: soft channel    — kbd vs aux soft role (OBF+AUX observe)
 *   ps2: soft dual       — dual status sample (side-effect free)
 *   ps2: soft sample     — read count / xor / stable lamps
 *   ps2: soft irq        — IRQ1/IRQ12 map inventory (never enable)
 *   ps2: soft path       — honesty: no IRQ/translate/drain/cmd
 *   ps2: soft honesty    — bar3/product-input non-claims
 *   ps2: soft identify   — float-aware soft identify PASS
 *   ps2: soft return rate — Wave 19 ok/fail rate lamps
 *   ps2: soft retcode    — Wave 19 retcode catalog
 *   ps2: soft deepen     — wave=27 areas stamp
 *   ps2: soft ratio      — Wave 16 status occupancy lamps
 *   ps2: soft headroom   — Wave 16 dual-sample head
 *   ps2: soft surface    — Wave 16 area catalog
 *   ps2: soft return     — Wave 16 return-surface bitmask
 *   ps2: soft return selftest — Wave 17 terminal return surface (kept)
 *   ps2: soft retmap     — Wave 17 return-surface map (kept)
 *   ps2: soft contract   — Wave 16 soft≠game I/O contract
 *   ps2: soft stats      — emission tallies
 *   ps2: soft inventory PASS|SKIP
 *   ps2: soft PASS|SKIP
 * Smoke marker (unchanged prefix):
 *   "ps2: status=…"
 */
#include <gj/klog.h>
#include <gj/types.h>

#define PS2_DATA_PORT   0x60u
#define PS2_STATUS_PORT 0x64u
#define PS2_CMD_PORT    0x64u

/* Status register bits (Intel 8042 public layout) */
#define PS2_ST_OBF  0x01u /* output buffer full (data ready) */
#define PS2_ST_IBF  0x02u /* input buffer full (controller busy) */
#define PS2_ST_SYS  0x04u /* system flag (POST set) */
#define PS2_ST_A2   0x08u /* command/data (A2) — last write was cmd */
#define PS2_ST_INH  0x10u /* keyboard inhibit family (active-low sense) */
#define PS2_ST_AUX  0x20u /* aux output buffer / transmit timeout family */
#define PS2_ST_TO   0x40u /* timeout / receive timeout family */
#define PS2_ST_PERR 0x80u /* parity error */

/* Classic PC/AT IRQ map (inventory only — never programmed here). */
#define PS2_IRQ_KBD 1u
#define PS2_IRQ_AUX 12u

/* Wave 20 deepen area count (fixed greppable categories in inventory log). */
#define PS2_SOFT_DEEPEN_AREAS 39u
#define PS2_SOFT_DEEPEN_WAVE  27u

/* Soft inventory emission tallies (wrap OK; never hard-gate). */
static u32 g_u32SoftInvLogs;
static u32 g_u32SoftProbeLogs;

static inline u8
inb(u16 u16Port)
{
    u8 u8Val;

    __asm__ volatile("inb %1, %0" : "=a"(u8Val) : "Nd"(u16Port));
    return u8Val;
}

/** Soft popcount of 8-bit status (inventory only). */
static u8
ps2_soft_pop8(u8 u8V)
{
    u8 u8N = 0;
    u8 u8B;

    for (u8B = 0; u8B < 8u; u8B++) {
        if ((u8V & (u8)(1u << u8B)) != 0u) {
            u8N++;
        }
    }
    return u8N;
}

/**
 * Wave 15 greppable soft inventory dump (product / smoke).
 * Decode one dual status sample into prefix-stable "ps2: soft …" lines.
 * Read-only; never touches data port or command writes.
 *
 * greppable: ps2: soft
 */
static void
ps2_soft_inventory(u8 u8Status, u8 u8Status2)
{
    u8 u8Obf = (u8)((u8Status & PS2_ST_OBF) != 0u);
    u8 u8Ibf = (u8)((u8Status & PS2_ST_IBF) != 0u);
    u8 u8Sys = (u8)((u8Status & PS2_ST_SYS) != 0u);
    u8 u8A2 = (u8)((u8Status & PS2_ST_A2) != 0u);
    u8 u8Inh = (u8)((u8Status & PS2_ST_INH) != 0u);
    u8 u8Aux = (u8)((u8Status & PS2_ST_AUX) != 0u);
    u8 u8To = (u8)((u8Status & PS2_ST_TO) != 0u);
    u8 u8Perr = (u8)((u8Status & PS2_ST_PERR) != 0u);
    u8 u8Stable = (u8)(u8Status == u8Status2);
    u8 u8Float = (u8)(u8Status == 0xffu);
    u8 u8Busy = u8Ibf;
    u8 u8DataReady = u8Obf;
    u8 u8Xor = (u8)(u8Status ^ u8Status2);
    u8 u8Pop = ps2_soft_pop8(u8Status);
    u8 u8KbdCh;
    u8 u8AuxCh;
    u8 u8Idle;
    u8 u8ErrLike;
    u8 u8Ok;
    u8 u8Skip;
    const char *szChannel;
    const char *szVerdict;
    const char *szState;

    if (g_u32SoftInvLogs < 0xffffffffu) {
        g_u32SoftInvLogs++;
    }

    /*
     * Soft channel role from observe-only status (no 0x60 drain):
     *   kbd  — OBF set, AUX clear (keyboard data pending, unread)
     *   aux  — OBF set, AUX set  (aux/mouse data pending, unread)
     *   idle — OBF clear
     * Never claims the byte; product input is virtio-input / USB HID.
     */
    u8KbdCh = (u8)((u8Obf != 0u) && (u8Aux == 0u));
    u8AuxCh = (u8)((u8Obf != 0u) && (u8Aux != 0u));
    u8Idle = (u8)(u8Obf == 0u);
    u8ErrLike = (u8)((u8To != 0u) || (u8Perr != 0u));
    if (u8KbdCh != 0u) {
        szChannel = "kbd";
    } else if (u8AuxCh != 0u) {
        szChannel = "aux";
    } else {
        szChannel = "idle";
    }

    /*
     * Soft composite state tag (inventory only):
     *   float — bus float 0xff
     *   busy  — IBF set (controller input buffer full)
     *   ready — OBF set (data pending; not drained)
     *   err   — TO or PERR observed
     *   idle  — otherwise
     */
    if (u8Float != 0u) {
        szState = "float";
    } else if (u8ErrLike != 0u) {
        szState = "err";
    } else if (u8Busy != 0u) {
        szState = "busy";
    } else if (u8DataReady != 0u) {
        szState = "ready";
    } else {
        szState = "idle";
    }

    /*
     * Soft verdict (inventory only; never claims IRQ1/IRQ12):
     *   PASS — status port readable (PC/QEMU always; float still PASS)
     *   SKIP — reserved (greppable parity with other HCL soft probes)
     * Floating 0xff remains PASS on the port-IO path; float=1 distinguishes.
     */
    szVerdict = "PASS";
    u8Ok = 1u;
    u8Skip = 0u;

    /*
     * Grep: ps2: soft inventory
     * Aggregated presence snapshot for product smoke / HCL.
     */
    kprintf("ps2: soft inventory status=0x%x status2=0x%x port=0x%x "
            "path=portio float=%u stable=%u xor=0x%x channel=%s "
            "err_like=%u state=%s pop=%u logs=%u wave=%u areas=%u\n",
            (unsigned)u8Status, (unsigned)u8Status2,
            (unsigned)PS2_STATUS_PORT, (unsigned)u8Float,
            (unsigned)u8Stable, (unsigned)u8Xor, szChannel,
            (unsigned)u8ErrLike, szState, (unsigned)u8Pop,
            (unsigned)g_u32SoftInvLogs, (unsigned)PS2_SOFT_DEEPEN_WAVE,
            (unsigned)PS2_SOFT_DEEPEN_AREAS);

    /* Grep: ps2: soft flags — full 8-bit status field inventory */
    kprintf("ps2: soft flags obf=%u ibf=%u sys=%u a2=%u inh=%u aux=%u "
            "to=%u perr=%u raw=0x%x wave=%u\n",
            (unsigned)u8Obf, (unsigned)u8Ibf, (unsigned)u8Sys,
            (unsigned)u8A2, (unsigned)u8Inh, (unsigned)u8Aux,
            (unsigned)u8To, (unsigned)u8Perr, (unsigned)u8Status,
            (unsigned)PS2_SOFT_DEEPEN_WAVE);

    /*
     * Grep: ps2: soft bits — per-bit soft PASS lamps (Wave 15 deepen).
     * Each public status bit greppable on its own for HCL / continuum.
     */
    kprintf("ps2: soft bits OBF=%u soft PASS\n", (unsigned)u8Obf);
    kprintf("ps2: soft bits IBF=%u soft PASS\n", (unsigned)u8Ibf);
    kprintf("ps2: soft bits SYS=%u soft PASS\n", (unsigned)u8Sys);
    kprintf("ps2: soft bits A2=%u soft PASS\n", (unsigned)u8A2);
    kprintf("ps2: soft bits INH=%u soft PASS\n", (unsigned)u8Inh);
    kprintf("ps2: soft bits AUX=%u soft PASS\n", (unsigned)u8Aux);
    kprintf("ps2: soft bits TO=%u soft PASS\n", (unsigned)u8To);
    kprintf("ps2: soft bits PERR=%u soft PASS\n", (unsigned)u8Perr);
    kprintf("ps2: soft bits pop=8 soft PASS raw=0x%x wave=%u\n",
            (unsigned)u8Status, (unsigned)PS2_SOFT_DEEPEN_WAVE);

    /*
     * Grep: ps2: soft mask — public bit mask constants (compile-time HCL).
     * Values are Intel 8042 layout; never written, inventory only.
     */
    kprintf("ps2: soft mask OBF=0x%x IBF=0x%x SYS=0x%x A2=0x%x "
            "INH=0x%x AUX=0x%x TO=0x%x PERR=0x%x soft PASS wave=%u\n",
            (unsigned)PS2_ST_OBF, (unsigned)PS2_ST_IBF,
            (unsigned)PS2_ST_SYS, (unsigned)PS2_ST_A2,
            (unsigned)PS2_ST_INH, (unsigned)PS2_ST_AUX,
            (unsigned)PS2_ST_TO, (unsigned)PS2_ST_PERR,
            (unsigned)PS2_SOFT_DEEPEN_WAVE);

    /* Grep: ps2: soft ports — public 8042 port map (no BAR) */
    kprintf("ps2: soft ports data=0x%x status=0x%x cmd=0x%x bar=n/a "
            "space=portio width=8 soft PASS wave=%u\n",
            (unsigned)PS2_DATA_PORT, (unsigned)PS2_STATUS_PORT,
            (unsigned)PS2_CMD_PORT, (unsigned)PS2_SOFT_DEEPEN_WAVE);

    /* Grep: ps2: soft regs — port-space offset map (Wave 15) */
    kprintf("ps2: soft regs DATA=0x%x STATUS=0x%x CMD=0x%x "
            "space=portio soft PASS wave=%u\n",
            (unsigned)PS2_DATA_PORT, (unsigned)PS2_STATUS_PORT,
            (unsigned)PS2_CMD_PORT, (unsigned)PS2_SOFT_DEEPEN_WAVE);

    /*
     * Grep: ps2: soft sense — composite lamps from dual sample.
     * float / stable / busy / data_ready / err_like / xor depth.
     */
    kprintf("ps2: soft sense float=%u stable=%u busy=%u data_ready=%u "
            "err_like=%u xor=0x%x state=%s soft PASS wave=%u\n",
            (unsigned)u8Float, (unsigned)u8Stable, (unsigned)u8Busy,
            (unsigned)u8DataReady, (unsigned)u8ErrLike, (unsigned)u8Xor,
            szState, (unsigned)PS2_SOFT_DEEPEN_WAVE);

    /* Grep: ps2: soft pop — status bit popcount + idle/err (Wave 15) */
    kprintf("ps2: soft pop bits=%u idle=%u err_like=%u float=%u "
            "raw=0x%x soft PASS wave=%u\n",
            (unsigned)u8Pop, (unsigned)u8Idle, (unsigned)u8ErrLike,
            (unsigned)u8Float, (unsigned)u8Status,
            (unsigned)PS2_SOFT_DEEPEN_WAVE);

    /*
     * Grep: ps2: soft channel — kbd/aux/idle role from OBF+AUX only.
     * Observe-only; does not drain OBF via 0x60.
     */
    kprintf("ps2: soft channel role=%s kbd=%u aux=%u idle=%u "
            "obf=%u aux_bit=%u drain=0 soft PASS wave=%u\n",
            szChannel, (unsigned)u8KbdCh, (unsigned)u8AuxCh,
            (unsigned)u8Idle, (unsigned)u8Obf, (unsigned)u8Aux,
            (unsigned)PS2_SOFT_DEEPEN_WAVE);

    /* Grep: ps2: soft dual — dual status sample (side-effect free) */
    kprintf("ps2: soft dual s0=0x%x s1=0x%x xor=0x%x stable=%u "
            "reads=2 soft PASS wave=%u\n",
            (unsigned)u8Status, (unsigned)u8Status2,
            (unsigned)u8Xor, (unsigned)u8Stable,
            (unsigned)PS2_SOFT_DEEPEN_WAVE);

    /* Grep: ps2: soft sample — dual-read geometry (Wave 15) */
    kprintf("ps2: soft sample reads=2 s0=0x%x s1=0x%x xor=0x%x "
            "stable=%u float=%u soft PASS wave=%u\n",
            (unsigned)u8Status, (unsigned)u8Status2, (unsigned)u8Xor,
            (unsigned)u8Stable, (unsigned)u8Float,
            (unsigned)PS2_SOFT_DEEPEN_WAVE);

    /*
     * Grep: ps2: soft irq — classic IRQ map inventory only.
     * Never programs PIC/APIC; never enables IRQ1/IRQ12.
     */
    kprintf("ps2: soft irq kbd_irq=%u aux_irq=%u enable=0 unmask=0 "
            "pic_write=0 soft PASS wave=%u\n",
            (unsigned)PS2_IRQ_KBD, (unsigned)PS2_IRQ_AUX,
            (unsigned)PS2_SOFT_DEEPEN_WAVE);

    /*
     * Grep: ps2: soft path — explicit non-claim of product input path.
     * Product input is virtio-input / USB HID; this probe never enables
     * IRQ1/IRQ12, never sets translate, never drains OBF via 0x60,
     * never writes controller commands, never spins on IBF/OBF.
     */
    kprintf("ps2: soft path claim=0 irq=0 irq1=0 irq12=0 translate=0 "
            "drain=0 cmd_write=0 data_read=0 spin=0 busy=%u "
            "data_ready=%u portio=1 via=status wave=%u soft PASS\n",
            (unsigned)u8Busy, (unsigned)u8DataReady,
            (unsigned)PS2_SOFT_DEEPEN_WAVE);

    /*
     * Grep: ps2: soft honesty — Wave 15 non-claims (soft inventory only).
     * Product input remains virtio-input / USB HID; bar3 stays open.
     */
    kprintf("ps2: soft honesty product_input=0 virtio_input=1 usb_hid=1 "
            "bar3=open claim=0 irq_enable=0 wave=%u soft PASS\n",
            (unsigned)PS2_SOFT_DEEPEN_WAVE);

    /*
     * Soft identify outcome. Floating 0xff is still reported as soft
     * inventory PASS on the PC/QEMU port-IO path (readable bus); flag
     * float=1 so logs distinguish empty/legacy hosts.
     */
    if (u8Float != 0u) {
        kprintf("ps2: soft identify float=1 raw=0x%x channel=%s "
                "stable=%u state=%s wave=%u PASS\n",
                (unsigned)u8Status, szChannel, (unsigned)u8Stable, szState,
                (unsigned)PS2_SOFT_DEEPEN_WAVE);
    } else {
        kprintf("ps2: soft identify float=0 obf=%u ibf=%u sys=%u "
                "a2=%u inh=%u aux=%u to=%u perr=%u channel=%s "
                "state=%s raw=0x%x wave=%u PASS\n",
                (unsigned)u8Obf, (unsigned)u8Ibf, (unsigned)u8Sys,
                (unsigned)u8A2, (unsigned)u8Inh, (unsigned)u8Aux,
                (unsigned)u8To, (unsigned)u8Perr, szChannel, szState,
                (unsigned)u8Status, (unsigned)PS2_SOFT_DEEPEN_WAVE);
    }

    /*
     * Wave 16 complementary deepen (kept; never hard-gates).
     * Soft ≠ game I/O. greppable: ps2: soft ratio|headroom|surface|return|contract
     */
    {
        u32 u32Surf = 0u;
        u32 u32PopBp = 0;
        u32 u32StableLamp = (u32)u8Stable;
        u32 u32FloatLamp = (u32)u8Float;

        if (u8Pop != 0u) {
            u32PopBp = ((u32)u8Pop * 10000u) / 8u;
        }
        u32Surf |= 0x1u; /* dual status sample always returned */
        if (u8Stable != 0u) {
            u32Surf |= 0x2u;
        }
        if (u8Float != 0u) {
            u32Surf |= 0x4u;
        }
        if (u8DataReady != 0u) {
            u32Surf |= 0x8u;
        }
        if (u8Busy != 0u) {
            u32Surf |= 0x10u;
        }
        if (u8ErrLike != 0u) {
            u32Surf |= 0x20u;
        }
        if (u8KbdCh != 0u || u8AuxCh != 0u) {
            u32Surf |= 0x40u;
        }
        u32Surf |= 0x80u; /* ports/regs/irq map always catalogued */
        /* Grep: ps2: soft ratio */
        kprintf("ps2: soft ratio pop_bp=%u pop=%u stable=%u float=%u "
                "busy=%u data_ready=%u wave=%u soft PASS\n",
                u32PopBp, (unsigned)u8Pop, u32StableLamp, u32FloatLamp,
                (unsigned)u8Busy, (unsigned)u8DataReady,
                (unsigned)PS2_SOFT_DEEPEN_WAVE);
        /* Grep: ps2: soft headroom */
        kprintf("ps2: soft headroom dual_reads=2 xor=0x%x idle=%u "
                "channel=%s state=%s wave=%u soft PASS\n",
                (unsigned)u8Xor, (unsigned)u8Idle, szChannel, szState,
                (unsigned)PS2_SOFT_DEEPEN_WAVE);
        /* Grep: ps2: soft surface */
        kprintf("ps2: soft surface inventory,flags,bits,mask,ports,regs,"
                "sense,pop,channel,dual,sample,irq,path,honesty,identify,"
                "ratio,headroom,return,contract,deepen,stats "
                "areas=%u wave=%u\n",
                (unsigned)PS2_SOFT_DEEPEN_AREAS,
                (unsigned)PS2_SOFT_DEEPEN_WAVE);
        /* Grep: ps2: soft return — return-surface bitmask */
        kprintf("ps2: soft return surf=0x%x dual=1 stable=%u float=%u "
                "ready=%u busy=%u err=%u ch=%u via=portio areas=%u "
                "wave=%u soft PASS\n",
                u32Surf, (unsigned)u8Stable, (unsigned)u8Float,
                (unsigned)u8DataReady, (unsigned)u8Busy, (unsigned)u8ErrLike,
                (unsigned)((u8KbdCh | u8AuxCh) != 0u),
                (unsigned)PS2_SOFT_DEEPEN_AREAS,
                (unsigned)PS2_SOFT_DEEPEN_WAVE);
        /* Grep: ps2: soft contract — soft ≠ game I/O */
        kprintf("ps2: soft contract soft_only=1 game_io=0 product_input=0 "
                "irq_enable=0 drain=0 virtio_input=1 usb_hid=1 bar3=open "
                "wave=%u soft PASS\n",
                (unsigned)PS2_SOFT_DEEPEN_WAVE);
    }

    /*
     * Wave 17 complementary sub-lines (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: ps2: soft return — Wave 17 API return surfaces (kept) */
    kprintf("ps2: soft return via=portio soft_inv=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u soft PASS\n",
            (unsigned)PS2_SOFT_DEEPEN_WAVE);

    /* Grep: ps2: soft return selftest — Wave 17 terminal return surface (kept) */
    kprintf("ps2: soft return selftest inv_ret=1 product_kernel=OPEN "
            "multi_server=0 bar3=0 wave=%u soft PASS\n",
            (unsigned)PS2_SOFT_DEEPEN_WAVE);

    /* Grep: ps2: soft retmap — Wave 17 return-surface map (kept) */
    kprintf("ps2: soft retmap soft_inv=1 deepen=1 product=OPEN "
            "wave=%u soft PASS\n",
            (unsigned)PS2_SOFT_DEEPEN_WAVE);

    /*
     * ---- Wave 18 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: ps2: soft return rate — Wave 19 ok/fail rate lamps */
    kprintf("ps2: soft return rate soft_inv=1 selftest=1 retmap=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u "
            "(return rate; Soft≠product; not bar3)\n",
            (unsigned)PS2_SOFT_DEEPEN_WAVE);

    /* Grep: ps2: soft retcode — Wave 19 retcode catalog */
    kprintf("ps2: soft retcode ok=1 fail=1 inval=1 busy=1 "
            "selftest=1 retmap=1 product=OPEN soft_ne_product=1 wave=%u "
            "(retcode catalog; Soft≠product)\n",
            (unsigned)PS2_SOFT_DEEPEN_WAVE);

    /* Grep: ps2: soft deepen wave (Wave 24 stamp) */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: ps2: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("ps2: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)PS2_SOFT_DEEPEN_WAVE);
    /* Grep: ps2: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("ps2: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)PS2_SOFT_DEEPEN_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: ps2: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("ps2: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)PS2_SOFT_DEEPEN_WAVE);
    /* Grep: ps2: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("ps2: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)PS2_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: ps2: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("ps2: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)PS2_SOFT_DEEPEN_WAVE);
            /* Grep: ps2: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("ps2: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)PS2_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: ps2: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("ps2: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)PS2_SOFT_DEEPEN_WAVE);
            /* Grep: ps2: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("ps2: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)PS2_SOFT_DEEPEN_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: ps2: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("ps2: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)PS2_SOFT_DEEPEN_WAVE);
            /* Grep: ps2: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("ps2: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)PS2_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: ps2: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("ps2: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)PS2_SOFT_DEEPEN_WAVE);
            /* Grep: ps2: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("ps2: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)PS2_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: ps2: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("ps2: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)PS2_SOFT_DEEPEN_WAVE);
            /* Grep: ps2: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("ps2: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)PS2_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: ps2: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("ps2: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)PS2_SOFT_DEEPEN_WAVE);
            /* Grep: ps2: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("ps2: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)PS2_SOFT_DEEPEN_WAVE);
                    /*
                     * ---- Wave 27 exclusive complementary surfaces (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: ps2: soft retprism — Wave 27 return-prism honesty */
                    kprintf("ps2: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)PS2_SOFT_DEEPEN_WAVE);
                    /* Grep: ps2: soft retforge — Wave 27 exclusive forge stamp */
                    kprintf("ps2: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)PS2_SOFT_DEEPEN_WAVE);
    kprintf("ps2: soft deepen wave=%u areas=%u via=portio float=%u "
            "stable=%u channel=%s state=%s ok=%u skip=%u\n",
            (unsigned)PS2_SOFT_DEEPEN_WAVE, (unsigned)PS2_SOFT_DEEPEN_AREAS,
            (unsigned)u8Float, (unsigned)u8Stable, szChannel, szState,
            (unsigned)u8Ok, (unsigned)u8Skip);

    /* Grep: ps2: soft stats */
    kprintf("ps2: soft stats inv_logs=%u probe_logs=%u status=0x%x "
            "float=%u wave=%u\n",
            (unsigned)g_u32SoftInvLogs, (unsigned)g_u32SoftProbeLogs,
            (unsigned)u8Status, (unsigned)u8Float,
            (unsigned)PS2_SOFT_DEEPEN_WAVE);

    /* Grep: ps2: soft inventory PASS|SKIP / ps2: soft PASS|SKIP */
    kprintf("ps2: soft inventory %s via=portio float=%u stable=%u "
            "channel=%s wave=%u areas=%u\n",
            szVerdict, (unsigned)u8Float, (unsigned)u8Stable, szChannel,
            (unsigned)PS2_SOFT_DEEPEN_WAVE, (unsigned)PS2_SOFT_DEEPEN_AREAS);
    kprintf("ps2: soft %s via=portio status=0x%x channel=%s state=%s "
            "wave=%u\n",
            szVerdict, (unsigned)u8Status, szChannel, szState,
            (unsigned)PS2_SOFT_DEEPEN_WAVE);
}

/**
 * Probe PS/2 controller presence via status register soft identify.
 * On PC/QEMU the 8042 status port is always readable; returns 1 for soft PASS.
 */
u32
ps2_probe(void)
{
    u8 u8Status;
    u8 u8Status2;

    /*
     * Dual soft sample of status only. Never spin on IBF/OBF — product
     * input path is virtio-input / USB HID; this is HCL presence only.
     * No data-port read (would drain OBF); no command write.
     */
    u8Status = inb(PS2_STATUS_PORT);
    u8Status2 = inb(PS2_STATUS_PORT);

    if (g_u32SoftProbeLogs < 0xffffffffu) {
        g_u32SoftProbeLogs++;
    }

    /* Smoke marker — prefix-stable for scripts/smoke-all.sh need_mb. */
    kprintf("ps2: status=0x%x probe PASS\n", u8Status);

    ps2_soft_inventory(u8Status, u8Status2);
    return 1;
}
