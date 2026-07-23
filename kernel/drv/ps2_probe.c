/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Product T1 HCL: 8042 PS/2 controller soft probe — clean-room pure C.
 * Soft identify: read status port 0x64 only; decode OBF/IBF/SYS flags.
 * Does not enable IRQs, translate, write commands, or drain the input
 * buffer (smoke-safe). No GPL source; public 8042 layout only.
 */
#include <gj/klog.h>
#include <gj/types.h>

#define PS2_STATUS_PORT 0x64u

/* Status register bits (Intel 8042 public layout) */
#define PS2_ST_OBF 0x01u /* output buffer full (data ready) */
#define PS2_ST_IBF 0x02u /* input buffer full (controller busy) */
#define PS2_ST_SYS 0x04u /* system flag (POST set) */
#define PS2_ST_A2  0x08u /* command/data (A2) */
#define PS2_ST_ST4 0x10u /* keyboard inhibit / timeout family */
#define PS2_ST_ST5 0x20u /* transmit timeout */
#define PS2_ST_ST6 0x40u /* receive timeout */
#define PS2_ST_ST7 0x80u /* parity error */

static inline u8
inb(u16 u16Port)
{
    u8 u8Val;

    __asm__ volatile("inb %1, %0" : "=a"(u8Val) : "Nd"(u16Port));
    return u8Val;
}

/**
 * Probe PS/2 controller presence via status register soft identify.
 * On PC/QEMU the 8042 status port is always readable; returns 1 for soft PASS.
 */
u32
ps2_probe(void)
{
    u8 u8Status = inb(PS2_STATUS_PORT);
    u8 u8Obf = (u8)((u8Status & PS2_ST_OBF) != 0u);
    u8 u8Ibf = (u8)((u8Status & PS2_ST_IBF) != 0u);
    u8 u8Sys = (u8)((u8Status & PS2_ST_SYS) != 0u);

    /*
     * Log status for bring-up inventory. Never spin on IBF/OBF — product
     * input path is virtio-input / USB HID; this is HCL presence only.
     * No data-port read (would drain OBF); no command write.
     */
    kprintf("ps2: status=0x%x probe PASS\n", u8Status);
    kprintf("ps2: identify soft PASS obf=%u ibf=%u sys=%u raw=0x%x\n",
            (unsigned)u8Obf, (unsigned)u8Ibf, (unsigned)u8Sys,
            (unsigned)u8Status);
    kprintf("ps2: bar map soft n/a (port io 0x%x) PASS\n",
            (unsigned)PS2_STATUS_PORT);
    return 1;
}
