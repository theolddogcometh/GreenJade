/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * ioremap / iounmap. Host: malloc iomem + window table.
 * Freestanding: static iomem pool (no libc heap).
 */
#include "udx_internal.h"

#include <udx/mmio.h>
#include <udx/udx.h>

#if defined(UDX_HOST_LIBC)
#include <stdlib.h>
#else
static struct udx_iomem g_aFsIomem[UDX_FS_IOMEM_SLOTS];
static u8               g_aFsIomemUsed[UDX_FS_IOMEM_SLOTS];

static struct udx_iomem *
fs_iomem_alloc(void)
{
    u32 iSlot;

    for (iSlot = 0; iSlot < UDX_FS_IOMEM_SLOTS; iSlot++) {
        if (!g_aFsIomemUsed[iSlot]) {
            g_aFsIomemUsed[iSlot] = 1;
            g_aFsIomem[iSlot].pVa = NULL;
            g_aFsIomem[iSlot].u64Len = 0;
            g_aFsIomem[iSlot].u64Phys = 0;
            return &g_aFsIomem[iSlot];
        }
    }
    return NULL;
}

static void
fs_iomem_free(struct udx_iomem *pIo)
{
    u32 iSlot;

    if (pIo == NULL) {
        return;
    }
    for (iSlot = 0; iSlot < UDX_FS_IOMEM_SLOTS; iSlot++) {
        if (&g_aFsIomem[iSlot] == pIo) {
            g_aFsIomemUsed[iSlot] = 0;
            g_aFsIomem[iSlot].pVa = NULL;
            return;
        }
    }
}
#endif

struct udx_iomem *
udx_ioremap(u64 u64Phys, u64 u64Len)
{
    struct udx_iomem *pIo;
    void *pVa;

    if (u64Len == 0) {
        return NULL;
    }

    /* lookup returns VA already advanced by any sub-window offset */
    pVa = udx_host_window_lookup(u64Phys, u64Len, NULL);
    if (pVa == NULL) {
        udx_printk("udx: ioremap fail phys=%llx len=%llx\n",
                   (unsigned long long)u64Phys,
                   (unsigned long long)u64Len);
        return NULL;
    }

#if defined(UDX_HOST_LIBC)
    pIo = (struct udx_iomem *)malloc(sizeof(*pIo));
    if (pIo == NULL) {
        return NULL;
    }
#else
    pIo = fs_iomem_alloc();
    if (pIo == NULL) {
        udx_printk("udx: ioremap freestanding pool full\n");
        return NULL;
    }
#endif
    pIo->pVa = pVa;
    pIo->u64Len = u64Len;
    pIo->u64Phys = u64Phys;
    return pIo;
}

void
udx_iounmap(struct udx_iomem *pIo)
{
    if (pIo == NULL) {
        return;
    }
#if defined(UDX_HOST_LIBC)
    free(pIo);
#else
    fs_iomem_free(pIo);
#endif
}
