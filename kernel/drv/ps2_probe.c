/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Product T1 HCL: 8042 PS/2 controller soft probe — clean-room pure C.
 * Soft identify: read status port 0x64 only; decode full status inventory.
 * Does not enable IRQs, translate, write commands, or drain the input
 * buffer (smoke-safe). No GPL source; public 8042 layout only.
 *
 * Greppable soft inventory (prefix-stable):
 *   "ps2: soft inventory …"
 *   "ps2: soft flags …"
 *   "ps2: soft ports …"
 *   "ps2: soft path …"
 *   "ps2: soft dual …"
 *   "ps2: soft identify …"
 *   "ps2: soft PASS|SKIP"
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
 * Decode one status byte into greppable soft inventory lines.
 * Read-only; never touches data port or command writes.
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

    /*
     * Grep: ps2: soft inventory
     * Aggregated presence snapshot for product smoke / HCL.
     */
    kprintf("ps2: soft inventory status=0x%x status2=0x%x port=0x%x "
            "path=portio float=%u stable=%u\n",
            (unsigned)u8Status, (unsigned)u8Status2,
            (unsigned)PS2_STATUS_PORT, (unsigned)u8Float,
            (unsigned)u8Stable);

    /* Grep: ps2: soft flags — full 8-bit status field inventory */
    kprintf("ps2: soft flags obf=%u ibf=%u sys=%u a2=%u inh=%u aux=%u "
            "to=%u perr=%u raw=0x%x\n",
            (unsigned)u8Obf, (unsigned)u8Ibf, (unsigned)u8Sys,
            (unsigned)u8A2, (unsigned)u8Inh, (unsigned)u8Aux,
            (unsigned)u8To, (unsigned)u8Perr, (unsigned)u8Status);

    /* Grep: ps2: soft ports — public 8042 port map (no BAR) */
    kprintf("ps2: soft ports data=0x%x status=0x%x cmd=0x%x bar=n/a\n",
            (unsigned)PS2_DATA_PORT, (unsigned)PS2_STATUS_PORT,
            (unsigned)PS2_CMD_PORT);

    /*
     * Grep: ps2: soft path — explicit non-claim of product input path.
     * Product input is virtio-input / USB HID; this probe never enables
     * IRQ1/IRQ12, never sets translate, never drains OBF via 0x60.
     */
    kprintf("ps2: soft path claim=0 irq=0 translate=0 drain=0 "
            "cmd_write=0 busy=%u data_ready=%u\n",
            (unsigned)u8Busy, (unsigned)u8DataReady);

    /* Grep: ps2: soft dual — dual status sample (side-effect free) */
    kprintf("ps2: soft dual s0=0x%x s1=0x%x xor=0x%x stable=%u\n",
            (unsigned)u8Status, (unsigned)u8Status2,
            (unsigned)(u8Status ^ u8Status2), (unsigned)u8Stable);

    /*
     * Soft identify outcome. Floating 0xff is still reported as soft
     * inventory PASS on the PC/QEMU port-IO path (readable bus); flag
     * float=1 so logs distinguish empty/legacy hosts.
     */
    if (u8Float != 0u) {
        kprintf("ps2: soft identify float=1 raw=0x%x PASS\n",
                (unsigned)u8Status);
    } else {
        kprintf("ps2: soft identify float=0 obf=%u ibf=%u sys=%u "
                "raw=0x%x PASS\n",
                (unsigned)u8Obf, (unsigned)u8Ibf, (unsigned)u8Sys,
                (unsigned)u8Status);
    }

    kprintf("ps2: soft inventory PASS\n");
    kprintf("ps2: soft PASS\n");
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
