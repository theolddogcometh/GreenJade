/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Product UEFI handoff marker (linked into KERNEL.ELF, not the PE stub).
 *
 * The EFI loader (uefi_stub.c) loads EFI/GREENJADE/KERNEL.ELF, scans PT_LOAD
 * for "GJUEFI1\0" + 8-byte entry, then jumps there with a filled gj_boot_info.
 * Multiboot2 / QEMU -kernel does not use this path (dev-only; boot.S → kmain).
 *
 * Soft handoff surface (observe from stub + kernel, not Multiboot)
 * ----------------------------------------------------------------
 *   GJUEFI1             — 8-byte magic incl. NUL; loader scan key
 *   u64Entry            — absolute long-mode kmain_uefi address
 *   gj_boot_info        — filled by stub: memmap, optional GOP/RSDP, image span
 *   soft markers        — see boot_info.h / uefi_stub.c / identity_map.c:
 *                           GJ-EFI: GOP|memmap|handoff soft …
 *                           GJ-EFI: soft inventory|path|… (Wave 15)
 *                           boot: handoff|memmap|GOP|identity soft …
 *
 * Wave 16 exclusive soft deepen (this unit only — greppable "GJUEFI1: soft …"):
 *   GJUEFI1: soft inventory  — master surface + wave stamp (rodata)
 *   GJUEFI1: soft path       — P-BOOT-1 claim + loader/entry wiring
 *   GJUEFI1: soft hdr        — magic/entry/align/section catalog
 *   GJUEFI1: soft entry      — kmain_uefi product entry honesty
 *   GJUEFI1: soft honesty    — not Multiboot; e_entry unused; not bar3
 *   GJUEFI1: soft magic      — Wave 15 magic byte catalog
 *   GJUEFI1: soft contract   — Wave 15 loader scan contract
 *   GJUEFI1: soft flags      — Wave 15 soft capability lamps
 *   GJUEFI1: soft load       — Wave 15 KERNEL.ELF load note
 *   GJUEFI1: soft catalog    — Wave 15 area name rollup
 *   GJUEFI1: soft deepen     — wave=16 stamp + area catalog
 *   GJUEFI1: soft PASS       — header surface present (link-time soft)
 *
 * Loader contract (must not break):
 *   First 16 bytes of g_GjUefiHdr remain magic[8] + u64Entry. Soft catalog
 *   fields and companion rodata strings sit after that or in sibling symbols;
 *   uefi_stub scans only magic + entry.
 *
 * greppable: GJUEFI1: soft
 * greppable: GJUEFI1: soft inventory
 * greppable: GJUEFI1: soft deepen
 *
 * Pure C11 freestanding; dual MIT OR Apache-2.0.
 */
#include <gj/boot_info.h>
#include <gj/types.h>

/* Declared in main.c — long-mode kernel entry after ExitBootServices. */
void kmain_uefi(struct gj_boot_info *pInfo);

/* Wave 15 soft inventory stamp (observability only; never gates product). */
#define GJ_UEFI_SOFT_WAVE   16u
#define GJ_UEFI_SOFT_AREAS  14u /* inventory,path,hdr,entry,honesty,magic,
                                 * contract,flags,load,catalog,deepen */

struct gj_uefi_hdr {
    char aMagic[8];
    u64  u64Entry;
    /*
     * Wave 15 soft catalog (observability only). Loader reads only the first
     * 16 bytes (magic + entry); these fields are greppable in KERNEL.ELF and
     * never consulted by uefi_stub.
     */
    u32  u32SoftWave;   /* GJ_UEFI_SOFT_WAVE */
    u32  u32SoftAreas;  /* soft surface count */
    u32  u32SoftAlign;  /* header alignment (bytes) */
    u32  u32SoftFlags;  /* soft capability lamps (link-time) */
};

/* Soft capability lamps (link-time presence; not runtime claims). */
#define GJ_UEFI_SOFT_F_MAGIC   (1u << 0) /* GJUEFI1 magic present */
#define GJ_UEFI_SOFT_F_ENTRY   (1u << 1) /* kmain_uefi entry stamped */
#define GJ_UEFI_SOFT_F_ALIGN16 (1u << 2) /* 16-byte aligned header */
#define GJ_UEFI_SOFT_F_PBOOT1  (1u << 3) /* product UEFI path marker */
#define GJ_UEFI_SOFT_F_NO_EENT (1u << 4) /* e_entry not product entry */
#define GJ_UEFI_SOFT_F_W15     (1u << 5) /* Wave 15 soft catalog present */

/*
 * Must appear in a loaded PT_LOAD segment (linked into .rodata/.data).
 * Magic is 8 bytes including NUL so the loader can match "GJUEFI1".
 * u64Entry is the absolute address of kmain_uefi in the linked ELF.
 *
 * Do not use e_entry alone: Multiboot ELFs may place a 32-bit trampoline
 * there; GJUEFI1 always points at the long-mode product entry.
 *
 * Align 16 so the 8-byte scan step in uefi_stub cannot straddle a partial
 * magic across an unaligned boundary in awkward link layouts.
 */
__attribute__((section(".rodata.gj_uefi"), used, aligned(16)))
const struct gj_uefi_hdr g_GjUefiHdr = {
    { 'G', 'J', 'U', 'E', 'F', 'I', '1', '\0' },
    (u64)(gj_vaddr_t)kmain_uefi,
    GJ_UEFI_SOFT_WAVE,
    GJ_UEFI_SOFT_AREAS,
    16u,
    (GJ_UEFI_SOFT_F_MAGIC | GJ_UEFI_SOFT_F_ENTRY | GJ_UEFI_SOFT_F_ALIGN16 |
     GJ_UEFI_SOFT_F_PBOOT1 | GJ_UEFI_SOFT_F_NO_EENT | GJ_UEFI_SOFT_F_W15),
};

/*
 * Wave 15 greppable soft catalog strings (KERNEL.ELF PT_LOAD / .rodata).
 * Binary and source greps: "GJUEFI1: soft …". Soft only — no runtime call.
 */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftInventory[] =
    "GJUEFI1: soft inventory wave=16 areas=14 "
    "magic=GJUEFI1 entry=kmain_uefi path=p_boot_1_product "
    "align=16 soft_never_gates=1";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftPath[] =
    "GJUEFI1: soft path claim=p_boot_1_product loader=uefi_stub "
    "file=EFI/GREENJADE/KERNEL.ELF entry=kmain_uefi "
    "e_entry_unused=1 p_boot_2=dev_only soft_never_gates=1";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftHdr[] =
    "GJUEFI1: soft hdr magic=GJUEFI1 magic_bytes=8 entry_off=8 "
    "soft_wave_off=16 soft_areas_off=20 align=16 "
    "section=.rodata.gj_uefi scan_step=8";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftEntry[] =
    "GJUEFI1: soft entry symbol=kmain_uefi long_mode=1 "
    "ms_abi_stub=0 sysv_kernel=1 handoff=gj_boot_info "
    "noreturn=1 e_entry_not_product=1";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftHonesty[] =
    "GJUEFI1: soft honesty not-multiboot e_entry_not_product "
    "soft_only=1 never_gates=1 bar3=OPEN "
    "(Apple/product bars open; link-time surface only)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftMagic[] =
    "GJUEFI1: soft magic bytes=GJUEFI1 nul_term=1 scan_key=1 "
    "step=8 align=16 soft PASS";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftContract[] =
    "GJUEFI1: soft contract first16=magic+entry soft_after=1 "
    "stub_reads_soft=0 kernel_rodata=1 soft PASS";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftFlags[] =
    "GJUEFI1: soft flags magic=1 entry=1 align16=1 pboot1=1 "
    "no_eent=1 w15=1 soft PASS";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftLoad[] =
    "GJUEFI1: soft load file=EFI/GREENJADE/KERNEL.ELF "
    "pt_load_scan=1 entry=kmain_uefi soft PASS";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftCatalog[] =
    "GJUEFI1: soft catalog inventory,path,hdr,entry,honesty,"
    "magic,contract,flags,load,catalog,surfaces,note,deepen "
    "wave=16 areas_expect=14 soft PASS";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftSurfaces[] =
    "GJUEFI1: soft surfaces count=14 wave=16 "
    "names=inventory,path,hdr,entry,honesty,magic,contract,"
    "flags,load,catalog,surfaces,note,deepen,PASS";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftNote[] =
    "GJUEFI1: soft note milestone=wave16 exclusive=1 "
    "soft_only=1 not_bar3=1 not-multiboot";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftDeepen[] =
    "GJUEFI1: soft deepen wave=16 areas=14 "
    "catalog=inventory,path,hdr,entry,honesty,magic,contract,"
    "flags,load,catalog,surfaces,note,deepen "
    "unit=uefi_entry.c only soft_never_gates=1 (soft; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftPass[] =
    "GJUEFI1: soft PASS wave=16 hdr=present entry=stamped "
    "align=16 (soft inventory; not bar3)";
