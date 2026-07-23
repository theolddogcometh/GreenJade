/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Unified firmware handoff (Multiboot2 and UEFI fill this).
 *
 * Contract
 * --------
 * Both product (UEFI) and dev (Multiboot2) paths publish one snapshot via
 * boot_info_set_global() so late boot (kmain, SMP, IOMMU, FB) never keeps
 * live firmware protocol pointers after ExitBootServices / loader return.
 *
 * Field ownership by source (P-BOOT-1 product UEFI; P-BOOT-2 Multiboot dev):
 *
 *   Field              UEFI (GJ_BOOT_SRC_UEFI)          Multiboot2 (dev)
 *   ------------------ -------------------------------- -------------------------
 *   u64MemMap          phys ptr to EFI_MEMORY_DESCRIPTOR phys of MB2 mmap *only
 *                      array after ExitBootServices      if filled; else 0
 *   u64MemMapBytes     byte length of that buffer        tag payload size or 0
 *   u64MemDescSize     EFI descriptor stride (>0)        0 (MB2 entry size is
 *                                                        inside the mmap tag)
 *   u64Rsdp            ACPI 2.0 (preferred) or 1.0       optional; often 0
 *                      configuration-table pointer
 *   u64Fb*             optional GOP capture              optional FB tag
 *   u64KernelPhys/Bytes loaded KERNEL.ELF span           0 (image is the ELF)
 *   u32Mb2InfoPhys     0                                 Multiboot2 info phys
 *
 * u32Flags may be stamped by the filler or derived in boot_info_set_global().
 * Consumers should prefer flags when present, but still tolerate flags==0 and
 * probe non-zero fields (older fillers).
 *
 * Pure C11 freestanding; dual MIT OR Apache-2.0.
 */
#pragma once

#include <gj/types.h>

#define GJ_BOOT_INFO_MAGIC   0x49424a47u /* 'GJBI' little-endian */
#define GJ_BOOT_INFO_VERSION 1u

#define GJ_BOOT_SRC_UNKNOWN    0u
#define GJ_BOOT_SRC_MULTIBOOT2 1u
#define GJ_BOOT_SRC_UEFI       2u

/* Optional capability bits in u32Flags (OR-able). */
#define GJ_BOOT_F_MEMMAP     (1u << 0) /* u64MemMap + u64MemMapBytes valid */
#define GJ_BOOT_F_RSDP       (1u << 1) /* u64Rsdp is a usable RSDP phys */
#define GJ_BOOT_F_FB         (1u << 2) /* u64FbBase + geometry valid */
#define GJ_BOOT_F_KERNEL_IMG (1u << 3) /* u64KernelPhys/Bytes span valid */
#define GJ_BOOT_F_MB2_INFO   (1u << 4) /* u32Mb2InfoPhys is Multiboot2 info */

struct gj_boot_info {
    u32  u32Magic;       /* GJ_BOOT_INFO_MAGIC */
    u32  u32Version;     /* GJ_BOOT_INFO_VERSION */
    u32  u32Source;      /* GJ_BOOT_SRC_* */
    u32  u32Flags;       /* GJ_BOOT_F_* (optional; may be derived) */
    u64  u64MemMap;      /* phys ptr: EFI MD array or MB2 mmap (see above) */
    u64  u64MemMapBytes; /* byte length of buffer at u64MemMap */
    u64  u64MemDescSize; /* EFI MD size; 0 if Multiboot (variable tags) */
    u64  u64Rsdp;        /* phys RSDP (0 if unknown) */
    u64  u64FbBase;      /* phys framebuffer base (0 if none) */
    u32  u32FbWidth;
    u32  u32FbHeight;
    u32  u32FbPitch;     /* bytes per scanline */
    u32  u32FbBpp;
    u64  u64KernelPhys;  /* optional loaded kernel image phys base */
    u64  u64KernelBytes;
    u32  u32Mb2InfoPhys; /* Multiboot2 info phys (0 if UEFI) */
    u32  u32Pad;         /* keep struct 8-byte aligned */
};

/** Zero buffer and stamp magic/version. No-op if pInfo is NULL. */
void boot_info_clear(struct gj_boot_info *pInfo);

/**
 * Publish a snapshot into the process-global handoff.
 * Copies *pInfo, then ORs derived GJ_BOOT_F_* from non-zero fields so
 * consumers can trust flags even if the firmware path omitted them.
 * Ignores NULL. Does not require boot_info_valid() (partial UEFI halt paths
 * may publish a source-only stub).
 */
void boot_info_set_global(const struct gj_boot_info *pInfo);

/** Always returns the static global (never NULL). */
const struct gj_boot_info *boot_info_get(void);

/**
 * True (1) if magic and version match this header; false (0) if pInfo is
 * NULL or the snapshot is not a GreenJade handoff.
 */
int boot_info_valid(const struct gj_boot_info *pInfo);

/**
 * Product UEFI entry (long mode, identity map from firmware).
 * Filled gj_boot_info: magic, EFI memory map, optional RSDP/FB.
 * Does not return.
 */
void kmain_uefi(struct gj_boot_info *pInfo) __attribute__((noreturn));

/**
 * Install kernel-owned 4 GiB identity map (UEFI path).
 * Multiboot builds the same layout in arch boot.S before kmain.
 */
void boot_install_identity_4gib(void);
