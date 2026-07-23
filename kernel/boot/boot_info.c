/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Global gj_boot_info handoff store. Multiboot and UEFI paths fill a
 * local copy then publish here so late boot (kmain, iommu, FB) can read
 * a single stable snapshot without keeping firmware pointers live.
 *
 * Pure C11 freestanding; dual MIT OR Apache-2.0.
 */
#include <gj/boot_info.h>
#include <gj/string.h>

static struct gj_boot_info g_BootInfo;

/* OR capability bits from populated fields (idempotent with caller flags). */
static u32
boot_info_derive_flags(const struct gj_boot_info *pInfo)
{
    u32 u32Flags = 0;

    if (pInfo == NULL) {
        return 0;
    }
    if (pInfo->u64MemMap != 0 && pInfo->u64MemMapBytes != 0) {
        u32Flags |= GJ_BOOT_F_MEMMAP;
    }
    if (pInfo->u64Rsdp != 0) {
        u32Flags |= GJ_BOOT_F_RSDP;
    }
    if (pInfo->u64FbBase != 0) {
        u32Flags |= GJ_BOOT_F_FB;
    }
    if (pInfo->u64KernelPhys != 0 && pInfo->u64KernelBytes != 0) {
        u32Flags |= GJ_BOOT_F_KERNEL_IMG;
    }
    if (pInfo->u32Mb2InfoPhys != 0) {
        u32Flags |= GJ_BOOT_F_MB2_INFO;
    }
    return u32Flags;
}

/* Zero and stamp magic/version. No-op if pInfo is NULL (caller may probe). */
void
boot_info_clear(struct gj_boot_info *pInfo)
{
    if (pInfo == NULL) {
        return;
    }
    memset(pInfo, 0, sizeof(*pInfo));
    pInfo->u32Magic = GJ_BOOT_INFO_MAGIC;
    pInfo->u32Version = GJ_BOOT_INFO_VERSION;
}

/* Copy caller buffer into the process-global handoff. Ignores NULL. */
void
boot_info_set_global(const struct gj_boot_info *pInfo)
{
    if (pInfo == NULL) {
        return;
    }
    g_BootInfo = *pInfo;
    /* Ensure magic/version even if filler only set payload fields. */
    if (g_BootInfo.u32Magic == 0) {
        g_BootInfo.u32Magic = GJ_BOOT_INFO_MAGIC;
    }
    if (g_BootInfo.u32Version == 0) {
        g_BootInfo.u32Version = GJ_BOOT_INFO_VERSION;
    }
    g_BootInfo.u32Flags |= boot_info_derive_flags(&g_BootInfo);
}

/* Always returns the static global (never NULL). */
const struct gj_boot_info *
boot_info_get(void)
{
    return &g_BootInfo;
}

int
boot_info_valid(const struct gj_boot_info *pInfo)
{
    if (pInfo == NULL) {
        return 0;
    }
    if (pInfo->u32Magic != GJ_BOOT_INFO_MAGIC) {
        return 0;
    }
    if (pInfo->u32Version != GJ_BOOT_INFO_VERSION) {
        return 0;
    }
    return 1;
}
