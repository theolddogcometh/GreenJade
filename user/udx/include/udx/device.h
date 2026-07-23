/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Device soft surface (Linux struct device mental model).
 * Caps / MMIO / IRQ / DMA windows stay in pBackend — never in driver .c.
 */
#pragma once

#include <udx/types.h>

struct udx_device {
    const char *szName;
    void       *pDriverData; /* probe soft state (drvdata) */
    /* Opaque backend: MMIO/IRQ/DMA caps held by UDX, not driver */
    void       *pBackend;
};

static inline void *
udx_get_drvdata(struct udx_device *pDev)
{
    return pDev ? pDev->pDriverData : NULL;
}

static inline void
udx_set_drvdata(struct udx_device *pDev, void *pData)
{
    if (pDev) {
        pDev->pDriverData = pData;
    }
}

/** Linux dev_name() analogue — never NULL (falls back to "udx"). */
static inline const char *
udx_dev_name(const struct udx_device *pDev)
{
    if (pDev != NULL && pDev->szName != NULL && pDev->szName[0] != '\0') {
        return pDev->szName;
    }
    return "udx";
}

/** True if drvdata has been installed (probe completed soft attach). */
static inline int
udx_dev_has_drvdata(const struct udx_device *pDev)
{
    return pDev != NULL && pDev->pDriverData != NULL;
}
