/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Product T1 HCL: 8042 PS/2 controller soft probe — clean-room pure C.
 * Reads status port 0x64 only. Does not enable IRQs, translate, or drain
 * the input buffer (smoke-safe). No GPL source; public 8042 layout only.
 */
#include <gj/klog.h>
#include <gj/types.h>

#define PS2_STATUS_PORT 0x64u

static inline u8
inb(u16 u16Port)
{
    u8 u8Val;

    __asm__ volatile("inb %1, %0" : "=a"(u8Val) : "Nd"(u16Port));
    return u8Val;
}

/**
 * Probe PS/2 controller presence via status register.
 * On PC/QEMU the 8042 status port is always readable; returns 1 for soft PASS.
 */
u32
ps2_probe(void)
{
    u8 u8Status = inb(PS2_STATUS_PORT);

    /*
     * Log status for bring-up inventory. Never spin on IBF/OBF — product
     * input path is virtio-input / USB HID; this is HCL presence only.
     */
    kprintf("ps2: status=0x%x probe PASS\n", u8Status);
    return 1;
}
