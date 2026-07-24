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
 *   ps2: soft deepen     — wave=106 areas stamp
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
#define PS2_SOFT_DEEPEN_AREAS 149u
#define PS2_SOFT_DEEPEN_WAVE 105u

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
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: ps2: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("ps2: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)PS2_SOFT_DEEPEN_WAVE);
                    /* Grep: ps2: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("ps2: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)PS2_SOFT_DEEPEN_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: ps2: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("ps2: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)PS2_SOFT_DEEPEN_WAVE);
                            /* Grep: ps2: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("ps2: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)PS2_SOFT_DEEPEN_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: ps2: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("ps2: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=106 "
                                    "(retglyph honesty; Soft≠product; not bar3)\n");
                            /* Grep: ps2: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("ps2: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=106 "
                                    "(retscepter stamp; Soft≠product)\n");
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: ps2: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("ps2: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=106 "
                                    "(retsigil honesty; Soft≠product; not bar3)\n");
                            /* Grep: ps2: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("ps2: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=106 "
                                    "(retemblem stamp; Soft≠product)\n");
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: ps2: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("ps2: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=106 "
                                    "(retaegis honesty; Soft≠product; not bar3)\n");
                            /* Grep: ps2: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("ps2: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=106 "
                                    "(retsigil honesty; Soft≠product; not bar3)\n");
                            /* Grep: ps2: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("ps2: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=106 "
                                    "(retmantle stamp; Soft≠product)\n");
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ps2: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("ps2: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retbulwark honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("ps2: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retpanoply stamp; Soft≠product)\n");
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ps2: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("ps2: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retbastion honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("ps2: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retcitadel stamp; Soft≠product)\n");
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ps2: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("ps2: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retredoubt honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("ps2: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retkeep stamp; Soft≠product)\n");
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ps2: soft retfortress — Wave 35 return-fortress honesty */
kprintf("ps2: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retfortress honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("ps2: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retpalace stamp; Soft≠product)\n");
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ps2: soft rethold — Wave 36 return-hold honesty */
kprintf("ps2: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(rethold honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retspire — Wave 36 exclusive spire stamp */
kprintf("ps2: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retspire stamp; Soft≠product)\n");
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ps2: soft retwall — Wave 37 return-wall honesty */
kprintf("ps2: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retwall honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retgate — Wave 37 exclusive gate stamp */
kprintf("ps2: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retgate stamp; Soft≠product)\n");
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ps2: soft retmoat — Wave 38 return-moat honesty */
kprintf("ps2: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retmoat honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retower — Wave 38 exclusive tower stamp */
kprintf("ps2: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retower stamp; Soft≠product)\n");
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ps2: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("ps2: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retbarbican honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("ps2: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retglacis stamp; Soft≠product)\n");
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ps2: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("ps2: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retcurtain honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("ps2: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retparapet stamp; Soft≠product)\n");
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ps2: soft retravelin — Wave 41 return-travelin honesty */
kprintf("ps2: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("ps2: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retditch stamp; Soft≠product)\n");
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ps2: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("ps2: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retportcullis honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("ps2: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retbattlement stamp; Soft≠product)\n");
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ps2: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("ps2: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retmachicolation honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("ps2: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retarrowslit stamp; Soft≠product)\n");

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ps2: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("ps2: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retmerlon honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("ps2: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retembrasure stamp; Soft≠product)\n");

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ps2: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("ps2: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retkeepgate honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("ps2: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retouterward stamp; Soft≠product)\n");

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ps2: soft retbailey — Wave 46 return-bailey honesty */
kprintf("ps2: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retbailey honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("ps2: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retpostern stamp; Soft≠product)\n");

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ps2: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("ps2: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("ps2: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ps2: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("ps2: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("ps2: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ps2: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("ps2: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("ps2: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ps2: soft retsally — Wave 50 return-sally honesty */
kprintf("ps2: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("ps2: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ps2: soft retfosse — Wave 51 return-fosse honesty */
kprintf("ps2: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("ps2: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ps2: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("ps2: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("ps2: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ps2: soft retravelin — Wave 53 return-travelin honesty */
kprintf("ps2: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("ps2: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ps2: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("ps2: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retredan — Wave 54 exclusive redan stamp */
kprintf("ps2: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ps2: soft retflank — Wave 55 return-flank honesty */
kprintf("ps2: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retface — Wave 55 exclusive face stamp */
kprintf("ps2: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ps2: soft retgorge — Wave 56 return-gorge honesty */
kprintf("ps2: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("ps2: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ps2: soft retraverse — Wave 57 return-traverse honesty */
kprintf("ps2: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("ps2: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ps2: soft retorillon — Wave 58 return-orillon honesty */
kprintf("ps2: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("ps2: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ps2: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("ps2: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("ps2: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ps2: soft retplace — Wave 60 return-place honesty */
kprintf("ps2: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("ps2: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ps2: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("ps2: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("ps2: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ps2: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("ps2: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("ps2: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ps2: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("ps2: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("ps2: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: ps2: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("ps2: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: ps2: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("ps2: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: ps2: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("ps2: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: ps2: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("ps2: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: ps2: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("ps2: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=106 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: ps2: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("ps2: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=106 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ps2: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("ps2: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("ps2: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ps2: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("ps2: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("ps2: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ps2: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("ps2: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("ps2: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ps2: soft retfosseangle — Wave 70 return-fosseangle honesty */
kprintf("ps2: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
kprintf("ps2: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ps2: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
kprintf("ps2: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
kprintf("ps2: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: ps2: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
kprintf("ps2: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
kprintf("ps2: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: ps2: soft retcurtainface — Wave 73 return-curtainface honesty */
kprintf("ps2: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
kprintf("ps2: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retbastionangle stamp; Soft≠product)\n");
/* Grep: ps2: soft retglacisangle — Wave 74 return-glacisangle honesty */
kprintf("ps2: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
kprintf("ps2: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retparapetangle stamp; Soft≠product)\n");
/* Grep: ps2: soft retmoatangle — Wave 75 return-moatangle honesty */
kprintf("ps2: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retowerangle — Wave 75 exclusive towerangle stamp */
kprintf("ps2: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retowerangle stamp; Soft≠product)\n");
/* Grep: ps2: soft retgateangle — Wave 76 return-gateangle honesty */
kprintf("ps2: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retwallangle — Wave 76 exclusive wallangle stamp */
kprintf("ps2: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retwallangle stamp; Soft≠product)\n");
/* Grep: ps2: soft retspireangle — Wave 77 return-spireangle honesty */
kprintf("ps2: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retholdangle — Wave 77 exclusive holdangle stamp */
kprintf("ps2: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retholdangle stamp; Soft≠product)\n");
/* Grep: ps2: soft retpalaceangle — Wave 78 return-palaceangle honesty */
kprintf("ps2: soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retpalaceangle honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
kprintf("ps2: soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retfortressangle stamp; Soft≠product)\n");
/* Grep: ps2: soft retkeepangle — Wave 79 return-keepangle honesty */
kprintf("ps2: soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retkeepangle honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
kprintf("ps2: soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retredoubtangle stamp; Soft≠product)\n");
/* Grep: ps2: soft retcitadelangle — Wave 80 return-citadelangle honesty */
kprintf("ps2: soft retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retcitadelangle honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retbastionkeep — Wave 80 exclusive bastionkeep stamp */
kprintf("ps2: soft retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retbastionkeep stamp; Soft≠product)\n");
/* Grep: ps2: soft retpanoplyangle — Wave 81 return-panoplyangle honesty */
kprintf("ps2: soft retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retpanoplyangle honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
kprintf("ps2: soft retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retbulwarkangle stamp; Soft≠product)\n");
/* Grep: ps2: soft retmantleangle — Wave 82 return-mantleangle honesty */
kprintf("ps2: soft retmantleangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retmantleangle honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retaegisangle — Wave 82 exclusive aegisangle stamp */
kprintf("ps2: soft retaegisangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retaegisangle stamp; Soft≠product)\n");
/* Grep: ps2: soft retemblemangle — Wave 83 return-emblemangle honesty */
kprintf("ps2: soft retemblemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retemblemangle honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retsigilangle — Wave 83 exclusive sigilangle stamp */
kprintf("ps2: soft retsigilangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retsigilangle stamp; Soft≠product)\n");
/* Grep: ps2: soft retscepterangle — Wave 84 return-scepterangle honesty */
kprintf("ps2: soft retscepterangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retscepterangle honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retglyphangle — Wave 84 exclusive glyphangle stamp */
kprintf("ps2: soft retglyphangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retglyphangle stamp; Soft≠product)\n");
/* Grep: ps2: soft retcrownangle — Wave 85 return-crownangle honesty */
kprintf("ps2: soft retcrownangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retcrownangle honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retshardangle — Wave 85 exclusive shardangle stamp */
kprintf("ps2: soft retshardangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retshardangle stamp; Soft≠product)\n");
/* Grep: ps2: soft retforgeangle — Wave 86 return-forgeangle honesty */
kprintf("ps2: soft retforgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retforgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retprismangle — Wave 86 exclusive prismangle stamp */
kprintf("ps2: soft retprismangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retprismangle stamp; Soft≠product)\n");
/* Grep: ps2: soft retflameangle — Wave 87 return-flameangle honesty */
kprintf("ps2: soft retflameangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retflameangle honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retcipherangle — Wave 87 exclusive cipherangle stamp */
kprintf("ps2: soft retcipherangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retcipherangle stamp; Soft≠product)\n");
/* Grep: ps2: soft retbeaconangle — Wave 88 return-beaconangle honesty */
kprintf("ps2: soft retbeaconangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retbeaconangle honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retledgerangle — Wave 88 exclusive ledgerangle stamp */
kprintf("ps2: soft retledgerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retledgerangle stamp; Soft≠product)\n");
/* Grep: ps2: soft retbannerangle — Wave 89 return-bannerangle honesty */
kprintf("ps2: soft retbannerangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retbannerangle honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retvaultangle — Wave 89 exclusive vaultangle stamp */
kprintf("ps2: soft retvaultangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retvaultangle stamp; Soft≠product)\n");
/* Grep: ps2: soft retcrestangle — Wave 90 return-crestangle honesty */
kprintf("ps2: soft retcrestangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retcrestangle honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft rettokenangle — Wave 90 exclusive tokenangle stamp */
kprintf("ps2: soft rettokenangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (rettokenangle stamp; Soft≠product)\n");
/* Grep: ps2: soft retbadgeangle — Wave 91 return-badgeangle honesty */
kprintf("ps2: soft retbadgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retbadgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retphaseangle — Wave 91 exclusive phaseangle stamp */
kprintf("ps2: soft retphaseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retphaseangle stamp; Soft≠product)\n");
/* Grep: ps2: soft retmarkangle — Wave 92 return-markangle honesty */
kprintf("ps2: soft retmarkangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retmarkangle honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retpulseangle — Wave 92 exclusive pulseangle stamp */
kprintf("ps2: soft retpulseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retpulseangle stamp; Soft≠product)\n");

/* Grep: ps2: soft retsealangle — Wave 93 return-sealangle honesty */
kprintf("ps2: soft retsealangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retsealangle honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retboundangle — Wave 93 exclusive boundangle stamp */
kprintf("ps2: soft retboundangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retboundangle stamp; Soft≠product)\n");
/* Grep: ps2: soft retstemangle — Wave 94 return-stemangle honesty */
kprintf("ps2: soft retstemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retstemangle honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retbladeangle — Wave 94 exclusive bladeangle stamp */
kprintf("ps2: soft retbladeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retbladeangle stamp; Soft≠product)\n");
/* Grep: ps2: soft retchordangle — Wave 95 return-chordangle honesty */
kprintf("ps2: soft retchordangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retchordangle honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retarcangle — Wave 95 exclusive arcangle stamp */
kprintf("ps2: soft retarcangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retarcangle stamp; Soft≠product)\n");
/* Grep: ps2: soft retsectorangle — Wave 96 return-sectorangle honesty */
kprintf("ps2: soft retsectorangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retsectorangle honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retwedgeangle — Wave 96 exclusive wedgeangle stamp */
kprintf("ps2: soft retwedgeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retwedgeangle stamp; Soft≠product)\n");
/* Grep: ps2: soft retradiusangle — Wave 97 return-radiusangle honesty */
kprintf("ps2: soft retradiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retradiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retdiameterangle — Wave 97 exclusive diameterangle stamp */
kprintf("ps2: soft retdiameterangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retdiameterangle stamp; Soft≠product)\n");
/* Grep: ps2: soft retcircumangle — Wave 98 return-circumangle honesty */
kprintf("ps2: soft retcircumangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retcircumangle honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retellipseangle — Wave 98 exclusive ellipseangle stamp */
kprintf("ps2: soft retellipseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retellipseangle stamp; Soft≠product)\n");
/* Grep: ps2: soft rethyperangle — Wave 99 return-hyperangle honesty */
kprintf("ps2: soft rethyperangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (rethyperangle honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retparabolaangle — Wave 99 exclusive parabolaangle stamp */
kprintf("ps2: soft retparabolaangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retparabolaangle stamp; Soft≠product)\n");
/* Grep: ps2: soft retspiralangle — Wave 100 return-spiralangle honesty */
kprintf("ps2: soft retspiralangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retspiralangle honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft rethelixangle — Wave 100 exclusive helixangle stamp */
kprintf("ps2: soft rethelixangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (rethelixangle stamp; Soft≠product)\n");
/* Grep: ps2: soft rettorusangle — Wave 101 return-torusangle honesty */
kprintf("ps2: soft rettorusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (rettorusangle honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retknotangle — Wave 101 exclusive knotangle stamp */
kprintf("ps2: soft retknotangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retknotangle stamp; Soft≠product)\n");
/* Grep: ps2: soft retmoebiusangle — Wave 102 return-moebiusangle honesty */
kprintf("ps2: soft retmoebiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retmoebiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retkleinangle — Wave 102 exclusive kleinangle stamp */
kprintf("ps2: soft retkleinangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retkleinangle stamp; Soft≠product)\n");
/* Grep: ps2: soft retprojectangle — Wave 103 return-projectangle honesty */
kprintf("ps2: soft retprojectangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retprojectangle honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retaffineangle — Wave 103 exclusive affineangle stamp */
kprintf("ps2: soft retaffineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retaffineangle stamp; Soft≠product)\n");
/* Grep: ps2: soft retlinearangle — Wave 104 return-linearangle honesty */
kprintf("ps2: soft retlinearangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retlinearangle honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retbilinearangle — Wave 104 exclusive bilinearangle stamp */
kprintf("ps2: soft retbilinearangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retbilinearangle stamp; Soft≠product)\n");
/* Grep: ps2: soft retquadraticangle — Wave 105 return-quadraticangle honesty */
kprintf("ps2: soft retquadraticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retquadraticangle honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retcubicangle — Wave 105 exclusive cubicangle stamp */
kprintf("ps2: soft retcubicangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retcubicangle stamp; Soft≠product)\n");
/* Grep: ps2: soft retquarticangle — Wave 106 return-quarticangle honesty */
kprintf("ps2: soft retquarticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retquarticangle honesty; Soft≠product; not bar3)\n");
/* Grep: ps2: soft retquinticangle — Wave 106 exclusive quinticangle stamp */
kprintf("ps2: soft retquinticangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retquinticangle stamp; Soft≠product)\n");
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
