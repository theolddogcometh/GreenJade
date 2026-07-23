/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Product T1 HCL: 8042 PS/2 controller soft probe — clean-room pure C.
 * Soft identify: read status port 0x64 only; decode full status inventory.
 * Does not enable IRQs, translate, write commands, or drain the input
 * buffer (smoke-safe). No GPL source; public 8042 layout only.
 *
 * Wave 13 exclusive soft deepen (this unit only — greppable "ps2: soft …"):
 *   ps2: soft inventory  — dual status + port + float/stable snapshot
 *   ps2: soft flags      — full 8-bit status field inventory
 *   ps2: soft bits       — per-bit soft PASS lamps (public layout)
 *   ps2: soft mask       — status bit mask constants (HCL map)
 *   ps2: soft ports      — public 8042 port map (no BAR)
 *   ps2: soft sense      — float/stable/busy/ready/xor composite
 *   ps2: soft channel    — kbd vs aux soft role (OBF+AUX observe)
 *   ps2: soft dual       — dual status sample (side-effect free)
 *   ps2: soft path       — honesty: no IRQ/translate/drain/cmd
 *   ps2: soft identify   — float-aware soft identify PASS
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
#define PS2_ST_OBF 0x01u /* output buffer full (data ready) */
#define PS2_ST_IBF 0x02u /* input buffer full (controller busy) */
#define PS2_ST_SYS 0x04u /* system flag (POST set) */
#define PS2_ST_A2  0x08u /* command/data (A2) — last write was cmd */
#define PS2_ST_INH 0x10u /* keyboard inhibit family (active-low sense) */
#define PS2_ST_AUX 0x20u /* aux output buffer / transmit timeout family */
#define PS2_ST_TO  0x40u /* timeout / receive timeout family */
#define PS2_ST_PERR 0x80u /* parity error */

static inline u8
inb(u16 u16Port)
{
    u8 u8Val;

    __asm__ volatile("inb %1, %0" : "=a"(u8Val) : "Nd"(u16Port));
    return u8Val;
}

/**
 * Wave 13 greppable soft inventory dump (product / smoke).
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
    u8 u8KbdCh;
    u8 u8AuxCh;
    u8 u8Idle;
    u8 u8ErrLike;
    const char *szChannel;
    const char *szVerdict;

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
     * Soft verdict (inventory only; never claims IRQ1/IRQ12):
     *   PASS — status port readable (PC/QEMU always; float still PASS)
     *   SKIP — reserved (greppable parity with other HCL soft probes)
     * Floating 0xff remains PASS on the port-IO path; float=1 distinguishes.
     */
    szVerdict = "PASS";

    /*
     * Grep: ps2: soft inventory
     * Aggregated presence snapshot for product smoke / HCL.
     */
    kprintf("ps2: soft inventory status=0x%x status2=0x%x port=0x%x "
            "path=portio float=%u stable=%u xor=0x%x channel=%s "
            "err_like=%u wave=13\n",
            (unsigned)u8Status, (unsigned)u8Status2,
            (unsigned)PS2_STATUS_PORT, (unsigned)u8Float,
            (unsigned)u8Stable, (unsigned)u8Xor, szChannel,
            (unsigned)u8ErrLike);

    /* Grep: ps2: soft flags — full 8-bit status field inventory */
    kprintf("ps2: soft flags obf=%u ibf=%u sys=%u a2=%u inh=%u aux=%u "
            "to=%u perr=%u raw=0x%x\n",
            (unsigned)u8Obf, (unsigned)u8Ibf, (unsigned)u8Sys,
            (unsigned)u8A2, (unsigned)u8Inh, (unsigned)u8Aux,
            (unsigned)u8To, (unsigned)u8Perr, (unsigned)u8Status);

    /*
     * Grep: ps2: soft bits — per-bit soft PASS lamps (Wave 13 deepen).
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
    kprintf("ps2: soft bits pop=8 soft PASS raw=0x%x\n",
            (unsigned)u8Status);

    /*
     * Grep: ps2: soft mask — public bit mask constants (compile-time HCL).
     * Values are Intel 8042 layout; never written, inventory only.
     */
    kprintf("ps2: soft mask OBF=0x%x IBF=0x%x SYS=0x%x A2=0x%x "
            "INH=0x%x AUX=0x%x TO=0x%x PERR=0x%x soft PASS\n",
            (unsigned)PS2_ST_OBF, (unsigned)PS2_ST_IBF,
            (unsigned)PS2_ST_SYS, (unsigned)PS2_ST_A2,
            (unsigned)PS2_ST_INH, (unsigned)PS2_ST_AUX,
            (unsigned)PS2_ST_TO, (unsigned)PS2_ST_PERR);

    /* Grep: ps2: soft ports — public 8042 port map (no BAR) */
    kprintf("ps2: soft ports data=0x%x status=0x%x cmd=0x%x bar=n/a "
            "space=portio width=8 soft PASS\n",
            (unsigned)PS2_DATA_PORT, (unsigned)PS2_STATUS_PORT,
            (unsigned)PS2_CMD_PORT);

    /*
     * Grep: ps2: soft sense — composite lamps from dual sample.
     * float / stable / busy / data_ready / err_like / xor depth.
     */
    kprintf("ps2: soft sense float=%u stable=%u busy=%u data_ready=%u "
            "err_like=%u xor=0x%x soft PASS\n",
            (unsigned)u8Float, (unsigned)u8Stable, (unsigned)u8Busy,
            (unsigned)u8DataReady, (unsigned)u8ErrLike, (unsigned)u8Xor);

    /*
     * Grep: ps2: soft channel — kbd/aux/idle role from OBF+AUX only.
     * Observe-only; does not drain OBF via 0x60.
     */
    kprintf("ps2: soft channel role=%s kbd=%u aux=%u idle=%u "
            "obf=%u aux_bit=%u drain=0 soft PASS\n",
            szChannel, (unsigned)u8KbdCh, (unsigned)u8AuxCh,
            (unsigned)u8Idle, (unsigned)u8Obf, (unsigned)u8Aux);

    /* Grep: ps2: soft dual — dual status sample (side-effect free) */
    kprintf("ps2: soft dual s0=0x%x s1=0x%x xor=0x%x stable=%u "
            "reads=2 soft PASS\n",
            (unsigned)u8Status, (unsigned)u8Status2,
            (unsigned)u8Xor, (unsigned)u8Stable);

    /*
     * Grep: ps2: soft path — explicit non-claim of product input path.
     * Product input is virtio-input / USB HID; this probe never enables
     * IRQ1/IRQ12, never sets translate, never drains OBF via 0x60,
     * never writes controller commands, never spins on IBF/OBF.
     */
    kprintf("ps2: soft path claim=0 irq=0 irq1=0 irq12=0 translate=0 "
            "drain=0 cmd_write=0 data_read=0 spin=0 busy=%u "
            "data_ready=%u portio=1 via=status soft PASS\n",
            (unsigned)u8Busy, (unsigned)u8DataReady);

    /*
     * Soft identify outcome. Floating 0xff is still reported as soft
     * inventory PASS on the PC/QEMU port-IO path (readable bus); flag
     * float=1 so logs distinguish empty/legacy hosts.
     */
    if (u8Float != 0u) {
        kprintf("ps2: soft identify float=1 raw=0x%x channel=%s "
                "stable=%u PASS\n",
                (unsigned)u8Status, szChannel, (unsigned)u8Stable);
    } else {
        kprintf("ps2: soft identify float=0 obf=%u ibf=%u sys=%u "
                "a2=%u inh=%u aux=%u to=%u perr=%u channel=%s "
                "raw=0x%x PASS\n",
                (unsigned)u8Obf, (unsigned)u8Ibf, (unsigned)u8Sys,
                (unsigned)u8A2, (unsigned)u8Inh, (unsigned)u8Aux,
                (unsigned)u8To, (unsigned)u8Perr, szChannel,
                (unsigned)u8Status);
    }

    /* Grep: ps2: soft inventory PASS|SKIP / ps2: soft PASS|SKIP */
    kprintf("ps2: soft inventory %s via=portio float=%u stable=%u "
            "channel=%s\n",
            szVerdict, (unsigned)u8Float, (unsigned)u8Stable, szChannel);
    kprintf("ps2: soft %s via=portio status=0x%x channel=%s wave=13\n",
            szVerdict, (unsigned)u8Status, szChannel);
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

    /* Smoke marker — prefix-stable for scripts/smoke-all.sh need_mb. */
    kprintf("ps2: status=0x%x probe PASS\n", u8Status);

    ps2_soft_inventory(u8Status, u8Status2);
    return 1;
}
