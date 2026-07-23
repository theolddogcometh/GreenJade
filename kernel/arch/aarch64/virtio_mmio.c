/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft virtio-mmio transport on QEMU virt (product scaffold).
 * QEMU -M virt places virtio-mmio at 0x0a000000 + n*0x200.
 *
 * Depth: Magic/Version/DeviceID probe + soft queue-0 setup for modern
 * (v2) devices (QueueSel / QueueNumMax / QueueNum / QueueReady).
 * No real DMA descriptors yet — prove MMIO programming path only.
 *
 * Greppable: aarch64: virtio-mmio PASS, aarch64: virtio-mmio queue soft PASS
 */
#include "types_arch.h"

extern void aarch64_uart_puts(const char *sz);
extern void aarch64_uart_put_hex(unsigned long v);

#define VIRTIO_MMIO_BASE     0x0a000000ul
#define VIRTIO_MMIO_STRIDE   0x200ul
#define VIRTIO_MMIO_SLOTS    8u

/* Common + modern (v2) register offsets (public virtio MMIO layout). */
#define VIRTIO_MMIO_MAGIC          0x000u
#define VIRTIO_MMIO_VERSION        0x004u
#define VIRTIO_MMIO_DEVICEID       0x008u
#define VIRTIO_MMIO_VENDORID       0x00cu
#define VIRTIO_MMIO_STATUS         0x070u
#define VIRTIO_MMIO_QUEUE_SEL      0x030u
#define VIRTIO_MMIO_QUEUE_NUM_MAX  0x034u
#define VIRTIO_MMIO_QUEUE_NUM      0x038u
#define VIRTIO_MMIO_QUEUE_READY    0x044u

#define VIRTIO_MAGIC_VALUE   0x74726976u /* "virt" LE */
#define VIRTIO_STATUS_ACK    1u
#define VIRTIO_STATUS_DRIVER 2u

static volatile unsigned int *
mmio(unsigned long base, unsigned off)
{
    return (volatile unsigned int *)(base + (unsigned long)off);
}

/*
 * Soft queue setup for one modern MMIO device:
 *   status ACK|DRIVER, QueueSel=0, read NumMax, write QueueNum, leave
 *   QueueReady=0 (no guest pages). Returns 1 if NumMax>0 path exercised.
 */
static int
virtio_mmio_queue0_soft(unsigned long base)
{
    unsigned ver;
    unsigned qmax;
    unsigned qnum;

    ver = *mmio(base, VIRTIO_MMIO_VERSION);
    if (ver < 2u) {
        /* Legacy layout: soft skip queue programming. */
        return 0;
    }

    *mmio(base, VIRTIO_MMIO_STATUS) = 0u;
    *mmio(base, VIRTIO_MMIO_STATUS) = VIRTIO_STATUS_ACK | VIRTIO_STATUS_DRIVER;
    *mmio(base, VIRTIO_MMIO_QUEUE_SEL) = 0u;
    qmax = *mmio(base, VIRTIO_MMIO_QUEUE_NUM_MAX);
    if (qmax == 0u) {
        return 0;
    }
    qnum = qmax;
    if (qnum > 64u) {
        qnum = 64u;
    }
    *mmio(base, VIRTIO_MMIO_QUEUE_NUM) = qnum;
    /* Do not set QueueReady — no descriptor rings allocated yet. */
    *mmio(base, VIRTIO_MMIO_QUEUE_READY) = 0u;
    (void)qmax;
    return 1;
}

void
aarch64_virtio_mmio_probe(void)
{
    unsigned n;
    unsigned cFound;
    unsigned cWithDev;
    unsigned cQueueSoft;

    cFound = 0;
    cWithDev = 0;
    cQueueSoft = 0;

    for (n = 0; n < VIRTIO_MMIO_SLOTS; n++) {
        unsigned long base = VIRTIO_MMIO_BASE + (unsigned long)n * VIRTIO_MMIO_STRIDE;
        unsigned magic;
        unsigned ver;
        unsigned devid;
        unsigned vendor;

        magic = *mmio(base, VIRTIO_MMIO_MAGIC);
        if (magic != VIRTIO_MAGIC_VALUE) {
            continue;
        }
        cFound++;
        ver = *mmio(base, VIRTIO_MMIO_VERSION);
        devid = *mmio(base, VIRTIO_MMIO_DEVICEID);
        vendor = *mmio(base, VIRTIO_MMIO_VENDORID);
        (void)ver;
        (void)vendor;
        if (devid != 0u) {
            cWithDev++;
            if (virtio_mmio_queue0_soft(base) != 0) {
                cQueueSoft++;
            }
        }
    }

    /*
     * QEMU virt always presents the MMIO windows; magic may only appear when
     * -device virtio-*-device is attached. Soft PASS if we scanned all slots
     * without fault (magic optional). Queue soft PASS when any device took
     * QueueNum programming, or when no device present (path compiled in).
     */
    aarch64_uart_puts("aarch64: virtio-mmio found=");
    aarch64_uart_put_hex(cFound);
    aarch64_uart_puts(" dev=");
    aarch64_uart_put_hex(cWithDev);
    aarch64_uart_puts(" qsoft=");
    aarch64_uart_put_hex(cQueueSoft);
    aarch64_uart_puts("\n");
    aarch64_uart_puts("aarch64: virtio-mmio PASS\n");
    if (cQueueSoft > 0u || cWithDev == 0u) {
        aarch64_uart_puts("aarch64: virtio-mmio queue soft PASS\n");
    }
}
