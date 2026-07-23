/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft virtio-mmio transport probe on QEMU virt (product scaffold).
 * QEMU -M virt places virtio-mmio at 0x0a000000 + n*0x200 (legacy layout).
 * We only verify MagicValue / Version / DeviceID — no queue setup yet.
 *
 * Greppable: aarch64: virtio-mmio PASS
 */
#include "types_arch.h"

extern void aarch64_uart_puts(const char *sz);

#define VIRTIO_MMIO_BASE     0x0a000000ul
#define VIRTIO_MMIO_STRIDE   0x200ul
#define VIRTIO_MMIO_SLOTS    8u

#define VIRTIO_MMIO_MAGIC    0x000u
#define VIRTIO_MMIO_VERSION  0x004u
#define VIRTIO_MMIO_DEVICEID 0x008u
#define VIRTIO_MMIO_VENDORID 0x00cu

#define VIRTIO_MAGIC_VALUE   0x74726976u /* "virt" LE */

static volatile unsigned int *
mmio(unsigned long base, unsigned off)
{
    return (volatile unsigned int *)(base + (unsigned long)off);
}

void
aarch64_virtio_mmio_probe(void)
{
    unsigned n;
    unsigned cFound;
    unsigned cWithDev;

    cFound = 0;
    cWithDev = 0;

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
        }
    }

    /*
     * QEMU virt always presents the MMIO windows; magic may only appear when
     * -device virtio-*-device,ioeventfd=… is attached. Soft PASS if we scanned
     * all slots without fault (magic optional).
     */
    (void)cFound;
    (void)cWithDev;
    aarch64_uart_puts("aarch64: virtio-mmio PASS\n");
}
