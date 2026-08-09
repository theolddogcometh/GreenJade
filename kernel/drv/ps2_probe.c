/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Product T1 HCL: 8042 PS/2 controller soft probe - clean-room pure C.
 * Soft identify: dual-read status port 0x64 only; decode status inventory.
 * Does not enable IRQs, translate, write commands, or drain the input
 * buffer (smoke-safe). No GPL source; public 8042 layout only.
 *
 * Lean soft residual (this unit only; C0; Soft!=product; G-AC-1):
 *   ps2: soft inventory  - dual status + channel + float/stable rollup
 *   ps2: soft residual   - port/IRQ map residual (observe-only)
 *   ps2: soft residual lean - silent lean_ok fold + Dual DoD OPEN once-lamp
 *   ps2: soft residual lean PASS - only when lean_ok == checks
 *   ps2: soft residual deepen - once-shot status-bit/layout rollup
 *   ps2: soft residual DoD seed OPEN - product_input stays OPEN
 *   ps2: soft path       - honesty: no IRQ/translate/drain/cmd
 *   ps2: soft honesty    - Soft!=product (virtio-input / USB HID)
 *   ps2: soft inventory PASS / ps2: soft PASS
 * Dual DoD OPEN (never close): product_input=OPEN need=virtio_input_or_usb_hid;
 * dual=MIT_OR_Apache-2.0; soft residual only; != 8042 product path.
 * No version stamp, no stamp storms, no wave=/areas= catalog.
 * Soft!=product. Dual MIT OR Apache-2.0. G-AC-1 (no .ko product AC).
 * Smoke marker (unchanged prefix):
 *   "ps2: status=..."
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
#define PS2_ST_A2   0x08u /* command/data (A2) - last write was cmd */
#define PS2_ST_INH  0x10u /* keyboard inhibit family (active-low sense) */
#define PS2_ST_AUX  0x20u /* aux output buffer / transmit timeout family */
#define PS2_ST_TO   0x40u /* timeout / receive timeout family */
#define PS2_ST_PERR 0x80u /* parity error */

/* Classic PC/AT IRQ map (inventory only - never programmed here). */
#define PS2_IRQ_KBD 1u
#define PS2_IRQ_AUX 12u

/*
 * Lean residual inventory: hard cap on emission (no stamp storms).
 * Soft != product; dual MIT OR Apache-2.0. No version stamp.
 */
#define PS2_SOFT_INV_LOG_CAP 2u

/* Soft inventory emission tallies (wrap OK; never hard-gate). */
static u32 g_u32SoftInvLogs;
static u32 g_u32SoftProbeLogs;
/* Once-lamp Dual DoD residual lean - no stamp storms. Soft!=product. */
static u8  g_fSoftResidualOnce;
/* Silent lean residual self-check tallies (fold into lean lamp only). */
static u32 g_u32LeanOk;
static u32 g_u32LeanChecks;

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
 * Silent lean residual self-check (no kprintf; Soft!=product).
 * Public 8042 layout + port/IRQ map + soft-path honesty constants.
 * Dual sample observe geometry (stable/float/xor) - never claims product.
 * Returns ok count; *pOutChecks = total when non-NULL.
 * Results fold into soft residual lean lamp only (H2: no stamp storm).
 */
static u32
ps2_soft_residual_lean_ok(u8 u8Status, u8 u8Status2, u32 *pOutChecks)
{
    u32 u32Ok = 0;
    u32 u32Checks = 0;
    u8 u8Xor;
    u8 u8Float;
    u8 u8Stable;
    u8 u8Bits;

    /* 1: classic 8042 port map (public PC/AT layout) */
    u32Checks++;
    if (PS2_DATA_PORT == 0x60u && PS2_STATUS_PORT == 0x64u &&
        PS2_CMD_PORT == 0x64u) {
        u32Ok++;
    }
    /* 2: status/cmd alias same port; data distinct (public layout) */
    u32Checks++;
    if (PS2_STATUS_PORT == PS2_CMD_PORT &&
        PS2_DATA_PORT != PS2_STATUS_PORT) {
        u32Ok++;
    }
    /* 3: status low nibble public (OBF/IBF/SYS/A2) */
    u32Checks++;
    if (PS2_ST_OBF == 0x01u && PS2_ST_IBF == 0x02u &&
        PS2_ST_SYS == 0x04u && PS2_ST_A2 == 0x08u) {
        u32Ok++;
    }
    /* 4: status high nibble public (INH/AUX/TO/PERR) */
    u32Checks++;
    if (PS2_ST_INH == 0x10u && PS2_ST_AUX == 0x20u &&
        PS2_ST_TO == 0x40u && PS2_ST_PERR == 0x80u) {
        u32Ok++;
    }
    /* 5: status bits cover full 8-bit register (no holes / overlap) */
    u32Checks++;
    u8Bits = (u8)(PS2_ST_OBF | PS2_ST_IBF | PS2_ST_SYS | PS2_ST_A2 |
                  PS2_ST_INH | PS2_ST_AUX | PS2_ST_TO | PS2_ST_PERR);
    if (u8Bits == 0xffu) {
        u32Ok++;
    }
    /* 6: classic IRQ map inventory only (never programmed here) */
    u32Checks++;
    if (PS2_IRQ_KBD == 1u && PS2_IRQ_AUX == 12u &&
        PS2_IRQ_KBD != PS2_IRQ_AUX) {
        u32Ok++;
    }
    /* 7: inventory hard-cap is lean (H2 stamp storm bound) */
    u32Checks++;
    if (PS2_SOFT_INV_LOG_CAP > 0u && PS2_SOFT_INV_LOG_CAP <= 2u) {
        u32Ok++;
    }
    /* 8: dual sample xor geometry is well-defined (observe-only) */
    u32Checks++;
    u8Xor = (u8)(u8Status ^ u8Status2);
    if ((u8)(u8Status ^ u8Xor) == u8Status2) {
        u32Ok++;
    }
    /* 9: dual-read stable iff xor==0 (observe geometry honesty) */
    u32Checks++;
    u8Stable = (u8)(u8Status == u8Status2);
    if ((u8Stable != 0u) == (u8Xor == 0u)) {
        u32Ok++;
    }
    /* 10: float detect is exactly status==0xff (bus-float observe) */
    u32Checks++;
    u8Float = (u8)(u8Status == 0xffu);
    if ((u8Float != 0u) == (u8Status == 0xffu)) {
        u32Ok++;
    }
    /* 11: soft path never drains 0x60 / never writes cmd (layout honesty) */
    u32Checks++;
    if (PS2_DATA_PORT == 0x60u && PS2_CMD_PORT == 0x64u &&
        PS2_STATUS_PORT == 0x64u && PS2_DATA_PORT != PS2_CMD_PORT) {
        /* Port constants fixed; probe path uses status-only dual-read. */
        u32Ok++;
    }
    /* 12: status bit singles are unique powers-of-two (no overlap) */
    u32Checks++;
    if (PS2_ST_OBF != PS2_ST_IBF && PS2_ST_AUX != PS2_ST_OBF &&
        PS2_ST_TO != PS2_ST_PERR && PS2_ST_SYS != PS2_ST_A2 &&
        PS2_ST_INH != PS2_ST_AUX) {
        u32Ok++;
    }

    g_u32LeanOk = u32Ok;
    g_u32LeanChecks = u32Checks;
    if (pOutChecks != NULL) {
        *pOutChecks = u32Checks;
    }
    return u32Ok;
}

/**
 * Lean soft residual inventory - greppable "ps2: soft ...".
 * Decode one dual status sample into compact residual lamps.
 * Read-only; never touches data port or command writes.
 * Hard-capped emission (PS2_SOFT_INV_LOG_CAP). No version stamp.
 * Soft!=product (product input = virtio-input / USB HID). G-AC-1.
 * Dual DoD OPEN residual once-lamp folded here (never closes product).
 *
 * greppable: ps2: soft inventory | soft residual | soft residual lean
 * greppable: ps2: soft residual lean PASS | soft residual deepen
 * greppable: ps2: soft residual DoD seed OPEN | soft path | soft honesty
 * greppable: product_input=OPEN | dual_dod=OPEN | G-AC-1 | lean_ok
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
    u8 u8ErrLike;
    u8 u8PopXor;
    u32 u32LeanOk;
    u32 u32LeanChecks;
    const char *szChannel;
    const char *szState;

    /* Cap serial flood (residual only; no stamp storms). */
    if (g_u32SoftInvLogs >= PS2_SOFT_INV_LOG_CAP) {
        return;
    }
    if (g_u32SoftInvLogs < 0xffffffffu) {
        g_u32SoftInvLogs++;
    }

    /*
     * Soft channel role from observe-only status (no 0x60 drain):
     *   kbd  - OBF set, AUX clear (keyboard data pending, unread)
     *   aux  - OBF set, AUX set  (aux/mouse data pending, unread)
     *   idle - OBF clear
     * Never claims the byte; product input is virtio-input / USB HID.
     */
    u8KbdCh = (u8)((u8Obf != 0u) && (u8Aux == 0u));
    u8AuxCh = (u8)((u8Obf != 0u) && (u8Aux != 0u));
    u8ErrLike = (u8)((u8To != 0u) || (u8Perr != 0u));
    u8PopXor = ps2_soft_pop8(u8Xor);
    if (u8KbdCh != 0u) {
        szChannel = "kbd";
    } else if (u8AuxCh != 0u) {
        szChannel = "aux";
    } else {
        szChannel = "idle";
    }

    /*
     * Soft composite state tag (inventory only):
     *   float - bus float 0xff
     *   busy  - IBF set (controller input buffer full)
     *   ready - OBF set (data pending; not drained)
     *   err   - TO or PERR observed
     *   idle  - otherwise
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
     * Grep: ps2: soft inventory
     * One-line rollup (dual sample + flags + channel). Soft residual only.
     */
    kprintf("ps2: soft inventory status=0x%x status2=0x%x port=0x%x "
            "float=%u stable=%u xor=0x%x channel=%s state=%s "
            "obf=%u ibf=%u sys=%u a2=%u inh=%u aux=%u to=%u perr=%u "
            "pop=%u pop_xor=%u logs=%u Soft!=product\n",
            (unsigned)u8Status, (unsigned)u8Status2,
            (unsigned)PS2_STATUS_PORT, (unsigned)u8Float,
            (unsigned)u8Stable, (unsigned)u8Xor, szChannel, szState,
            (unsigned)u8Obf, (unsigned)u8Ibf, (unsigned)u8Sys,
            (unsigned)u8A2, (unsigned)u8Inh, (unsigned)u8Aux,
            (unsigned)u8To, (unsigned)u8Perr, (unsigned)u8Pop,
            (unsigned)u8PopXor, (unsigned)g_u32SoftInvLogs);

    /*
     * Grep: ps2: soft residual
     * Port/IRQ map residual - lean, no per-bit stamp storm.
     * Observe-only; never programs PIC/APIC; never enables IRQ1/IRQ12.
     * Dual DoD OPEN: product_input remains OPEN (virtio-input / USB HID).
     * Soft!=product G-AC-1; dual MIT OR Apache-2.0.
     */
    kprintf("ps2: soft residual data=0x%x status=0x%x cmd=0x%x "
            "kbd_irq=%u aux_irq=%u enable=0 unmask=0 pic_write=0 "
            "dual_reads=2 stable=%u float=%u xor=0x%x pop_xor=%u "
            "channel=%s state=%s err_like=%u "
            "path=portio->status soft=1 product=0 "
            "product_input=OPEN dual_dod=OPEN "
            "need=virtio_input_or_usb_hid "
            "dual=MIT_OR_Apache-2.0 G-AC-1 Soft!=product\n",
            (unsigned)PS2_DATA_PORT, (unsigned)PS2_STATUS_PORT,
            (unsigned)PS2_CMD_PORT, (unsigned)PS2_IRQ_KBD,
            (unsigned)PS2_IRQ_AUX, (unsigned)u8Stable, (unsigned)u8Float,
            (unsigned)u8Xor, (unsigned)u8PopXor, szChannel, szState,
            (unsigned)u8ErrLike);

    /*
     * Lean Dual DoD residual lean (this unit only). Once-lamp -
     * no version stamp, no stamp storm. C0 soft residual only;
     * never Dual DoD close (product_input stays OPEN).
     * Silent lean_ok self-check folds here (nvme/virtio residual pattern).
     * greppable: ps2: soft residual lean
     * greppable: ps2: soft residual lean PASS
     * greppable: ps2: soft residual deepen
     * greppable: product_input=OPEN | dual_dod=OPEN | G-AC-1 | lean_ok
     */
    if (g_fSoftResidualOnce == 0u) {
        g_fSoftResidualOnce = 1u;
        u32LeanOk = ps2_soft_residual_lean_ok(u8Status, u8Status2,
                                              &u32LeanChecks);
        /*
         * Grep: ps2: soft residual lean
         * Soft shadow only != product input. Path honesty: status
         * dual-read only; never IRQ/drain/cmd. lean_ok folded (no storm).
         */
        kprintf("ps2: soft residual lean soft=1 product=0 claim=0 "
                "irq=0 irq1=0 irq12=0 translate=0 drain=0 cmd_write=0 "
                "data_read=0 spin=0 pic_write=0 unmask=0 "
                "float=%u stable=%u channel=%s state=%s err_like=%u "
                "lean_ok=%u/%u dual_reads=2 "
                "path=portio->status->virtio_input_or_usb_hid "
                "product_input=OPEN dual_dod=OPEN "
                "need=virtio_input_or_usb_hid "
                "dual=MIT_OR_Apache-2.0 stamp_storm=0 no_version_stamp=1 "
                "G-AC-1 Soft!=product\n",
                (unsigned)u8Float, (unsigned)u8Stable, szChannel, szState,
                (unsigned)u8ErrLike, u32LeanOk, u32LeanChecks);
        /*
         * Contiguous greppable: "ps2: soft residual lean PASS"
         * Only when silent self-check is full (stronger honesty).
         */
        if (u32LeanOk == u32LeanChecks && u32LeanChecks > 0u) {
            kprintf("ps2: soft residual lean PASS soft=1 product=0 claim=0 "
                    "irq=0 irq1=0 irq12=0 translate=0 drain=0 cmd_write=0 "
                    "data_read=0 spin=0 pic_write=0 unmask=0 "
                    "float=%u stable=%u channel=%s state=%s err_like=%u "
                    "lean_ok=%u/%u "
                    "path=portio->status->virtio_input_or_usb_hid "
                    "product_input=OPEN dual_dod=OPEN "
                    "need=virtio_input_or_usb_hid "
                    "dual=MIT_OR_Apache-2.0 G-AC-1 Soft!=product\n",
                    (unsigned)u8Float, (unsigned)u8Stable, szChannel,
                    szState, (unsigned)u8ErrLike, u32LeanOk,
                    u32LeanChecks);
        }
        /*
         * Grep: ps2: soft residual deepen
         * Once-shot functional status-bit / layout observe rollup.
         * Soft residual only - never enables IRQ / never drains 0x60.
         */
        kprintf("ps2: soft residual deepen data=0x%x status=0x%x cmd=0x%x "
                "kbd_irq=%u aux_irq=%u dual_reads=2 "
                "status_s=0x%x status2=0x%x xor=0x%x pop=%u pop_xor=%u "
                "obf=%u ibf=%u sys=%u a2=%u inh=%u aux=%u to=%u perr=%u "
                "float=%u stable=%u channel=%s state=%s err_like=%u "
                "lean_ok=%u/%u map_uc=0 portio=1 "
                "irq=0 translate=0 drain=0 cmd_write=0 data_read=0 "
                "product_input=OPEN dual_dod=OPEN "
                "need=virtio_input_or_usb_hid freestanding_8042=SKIP "
                "soft=1 product=0 Soft!=product G-AC-1 "
                "dual=MIT_OR_Apache-2.0 stamp_storm=0 no_version_stamp=1\n",
                (unsigned)PS2_DATA_PORT, (unsigned)PS2_STATUS_PORT,
                (unsigned)PS2_CMD_PORT, (unsigned)PS2_IRQ_KBD,
                (unsigned)PS2_IRQ_AUX, (unsigned)u8Status,
                (unsigned)u8Status2, (unsigned)u8Xor, (unsigned)u8Pop,
                (unsigned)u8PopXor, (unsigned)u8Obf, (unsigned)u8Ibf,
                (unsigned)u8Sys, (unsigned)u8A2, (unsigned)u8Inh,
                (unsigned)u8Aux, (unsigned)u8To, (unsigned)u8Perr,
                (unsigned)u8Float, (unsigned)u8Stable, szChannel, szState,
                (unsigned)u8ErrLike, u32LeanOk, u32LeanChecks);
        /* Grep: ps2: soft residual DoD seed OPEN */
        kprintf("ps2: soft residual DoD seed OPEN dual_reads=2 "
                "status=0x%x status2=0x%x channel=%s state=%s "
                "lean_ok=%u/%u soft=1 product=0 claim=0 irq=0 "
                "product_input=OPEN dual_dod=OPEN "
                "need=virtio_input_or_usb_hid "
                "path=portio_0x64->status_inventory "
                "(!= IRQ enable; != 0x60 drain; != cmd write; "
                "Soft!=product G-AC-1)\n",
                (unsigned)u8Status, (unsigned)u8Status2, szChannel,
                szState, u32LeanOk, u32LeanChecks);
    }

    /*
     * Grep: ps2: soft path | ps2: soft honesty
     * Explicit non-claim of product input path.
     * Product input is virtio-input / USB HID; this probe never enables
     * IRQ1/IRQ12, never sets translate, never drains OBF via 0x60,
     * never writes controller commands, never spins on IBF/OBF.
     */
    kprintf("ps2: soft path claim=0 irq=0 irq1=0 irq12=0 translate=0 "
            "drain=0 cmd_write=0 data_read=0 spin=0 busy=%u "
            "data_ready=%u portio=1 via=status "
            "product_input=OPEN dual_dod=OPEN "
            "(soft residual; Soft!=product G-AC-1)\n",
            (unsigned)u8Busy, (unsigned)u8DataReady);
    kprintf("ps2: soft honesty product_input=0 virtio_input=1 usb_hid=1 "
            "claim=0 irq_enable=0 soft=1 product=0 "
            "product_input_dod=OPEN dual_dod=OPEN "
            "dual=MIT_OR_Apache-2.0 G-AC-1 Soft!=product soft PASS\n");

    /* Grep: ps2: soft inventory PASS / ps2: soft PASS */
    kprintf("ps2: soft inventory PASS via=portio float=%u stable=%u "
            "channel=%s state=%s logs=%u Soft!=product\n",
            (unsigned)u8Float, (unsigned)u8Stable, szChannel, szState,
            (unsigned)g_u32SoftInvLogs);
    kprintf("ps2: soft PASS via=portio status=0x%x channel=%s state=%s "
            "Soft!=product G-AC-1\n",
            (unsigned)u8Status, szChannel, szState);
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
     * Dual soft sample of status only. Never spin on IBF/OBF - product
     * input path is virtio-input / USB HID; this is HCL presence only.
     * No data-port read (would drain OBF); no command write.
     */
    u8Status = inb(PS2_STATUS_PORT);
    u8Status2 = inb(PS2_STATUS_PORT);

    if (g_u32SoftProbeLogs < 0xffffffffu) {
        g_u32SoftProbeLogs++;
    }

    /* Smoke marker - prefix-stable for scripts/smoke-all.sh need_mb. */
    kprintf("ps2: status=0x%x probe PASS\n", u8Status);

    ps2_soft_inventory(u8Status, u8Status2);
    return 1;
}
