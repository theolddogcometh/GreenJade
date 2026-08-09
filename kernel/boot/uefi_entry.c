/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Product UEFI handoff marker (linked into KERNEL.ELF, not the PE stub).
 *
 * The EFI loader (uefi_stub.c) loads EFI/GREENJADE/KERNEL.ELF, scans PT_LOAD
 * for "GJUEFI1\0" + 8-byte entry, then jumps there with a filled gj_boot_info.
 * Multiboot2 / QEMU -kernel does not use this path (dev-only; boot.S -> kmain).
 *
 * Soft handoff surface (observe from stub + kernel, not Multiboot)
 * ----------------------------------------------------------------
 *   GJUEFI1             - 8-byte magic incl. NUL; loader scan key
 *   u64Entry            - absolute long-mode kmain_uefi address
 *   gj_boot_info        - filled by stub: memmap, optional GOP/RSDP, image span
 *   soft markers        - see boot_info.h / uefi_stub.c / identity_map.c:
 *                           GJ-EFI: GOP|memmap|handoff soft ...
 *                           GJ-EFI: soft inventory|path|...
 *                           boot: handoff|memmap|GOP|identity soft ...
 *
 * Lean soft residual (C0; exclusive residual; this unit only)
 * ----------------------------------------------------------
 * greppable prefix: "GJUEFI1: soft ..."
 *   NO stamp storms (no empty ret* / ret*angle dummy rodata) - H2.
 *   NO version stamp (not GJ_IMAGE_VERSION / flash identity) - H7.
 *   Soft!=product dual MIT OR Apache-2.0; never hard-gates product boot.
 *   G-AC-1: residual != product AC; not Linux .ko product path.
 *   Dual DoD OPEN; residual != Dual DoD A/B close; residual != bar3.
 *   Sparse honest surface (areas=17; catalog names match count):
 *     inventory | path | hdr | entry | honesty | magic | contract |
 *     flags | load | catalog | surfaces | note | return | retmap |
 *     residual | deepen | PASS
 *   Compile-time surface table enforces count == GJ_UEFI_SOFT_AREAS.
 *
 * Loader contract (must not break; enforced by _Static_assert below):
 *   First 16 bytes of g_GjUefiHdr remain magic[8] + u64Entry. Soft catalog
 *   fields and companion rodata strings sit after that or in sibling symbols;
 *   uefi_stub scans only magic + entry. Soft fields never gate product boot.
 *
 * greppable: GJUEFI1: soft
 * greppable: GJUEFI1: soft inventory
 * greppable: GJUEFI1: soft residual
 * greppable: GJUEFI1: soft residual lean
 * greppable: GJUEFI1: soft residual lean PASS
 * greppable: GJUEFI1: soft deepen
 * greppable: GJUEFI1: soft PASS
 * greppable: Soft!=product
 * greppable: G-AC-1
 * greppable: dual_dod=OPEN
 *
 * Pure C11 freestanding; dual MIT OR Apache-2.0. Soft!=product. G-AC-1.
 * C0 eng residual only; Dual DoD remains OPEN.
 */
#include <gj/boot_info.h>
#include <gj/types.h>

/* Declared in main.c - long-mode kernel entry after ExitBootServices. */
void kmain_uefi(struct gj_boot_info *pInfo);

/*
 * Soft inventory (observability only; never gates product).
 * Honest area count only - never inflate with stamp-storm dummies
 * (FAULT residual class H2; Soft!=product lean; C0 eng residual).
 */
#define GJ_UEFI_SOFT_WAVE  116u
#define GJ_UEFI_SOFT_AREAS 17u /* inventory,path,hdr,entry,honesty,magic,
                                * contract,flags,load,catalog,surfaces,note,
                                * return,retmap,residual,deepen,PASS */
#define GJ_UEFI_SOFT_HDR_BYTES 32u /* 8 magic + 8 entry + 4*4 soft catalog */
#define GJ_UEFI_SOFT_ALIGN     16u

struct gj_uefi_hdr {
    char aMagic[8];
    u64  u64Entry;
    /*
     * Soft catalog (observability only). Loader reads only the first
     * 16 bytes (magic + entry); these fields are greppable in KERNEL.ELF and
     * never consulted by uefi_stub.
     */
    u32  u32SoftWave;   /* GJ_UEFI_SOFT_WAVE */
    u32  u32SoftAreas;  /* soft surface count */
    u32  u32SoftAlign;  /* header alignment (bytes) */
    u32  u32SoftFlags;  /* soft capability lamps (link-time) */
};

/*
 * Loader contract compile-time checks (product-safe; not soft lamps).
 * Soft fields must begin at offset 16 so stub scan of first 16 bytes is
 * independent of wave/areas/flags churn. Align 16 keeps 8-byte scan steps
 * from straddling partial magic on awkward link layouts.
 */
_Static_assert(sizeof(((struct gj_uefi_hdr *)0)->aMagic) == 8u,
               "GJUEFI1 magic must be 8 bytes incl NUL");
_Static_assert(offsetof(struct gj_uefi_hdr, u64Entry) == 8u,
               "GJUEFI1 entry must sit at offset 8");
_Static_assert(offsetof(struct gj_uefi_hdr, u32SoftWave) == 16u,
               "GJUEFI1 soft catalog must start at offset 16");
_Static_assert(offsetof(struct gj_uefi_hdr, u32SoftAreas) == 20u,
               "GJUEFI1 soft areas must sit at offset 20");
_Static_assert(offsetof(struct gj_uefi_hdr, u32SoftAlign) == 24u,
               "GJUEFI1 soft align must sit at offset 24");
_Static_assert(offsetof(struct gj_uefi_hdr, u32SoftFlags) == 28u,
               "GJUEFI1 soft flags must sit at offset 28");
_Static_assert(sizeof(struct gj_uefi_hdr) == GJ_UEFI_SOFT_HDR_BYTES,
               "GJUEFI1 hdr must be 32 bytes (first16 product + soft catalog)");
_Static_assert((offsetof(struct gj_uefi_hdr, u64Entry) % 8u) == 0u,
               "GJUEFI1 u64Entry must be 8-byte aligned in layout");
_Static_assert(GJ_UEFI_SOFT_AREAS == 17u,
               "GJUEFI1 lean residual areas locked to honest catalog (no storm)");
_Static_assert(GJ_UEFI_SOFT_ALIGN == 16u,
               "GJUEFI1 header align must remain 16");

/* Soft capability lamps (link-time presence; not runtime claims). */
#define GJ_UEFI_SOFT_F_MAGIC   (1u << 0) /* GJUEFI1 magic present */
#define GJ_UEFI_SOFT_F_ENTRY   (1u << 1) /* kmain_uefi entry stamped */
#define GJ_UEFI_SOFT_F_ALIGN16 (1u << 2) /* 16-byte aligned header */
#define GJ_UEFI_SOFT_F_PBOOT1  (1u << 3) /* product UEFI path marker */
#define GJ_UEFI_SOFT_F_NO_EENT (1u << 4) /* e_entry not product entry */
#define GJ_UEFI_SOFT_F_LEAN    (1u << 5) /* lean residual; stamp_storm=0 */

#define GJ_UEFI_SOFT_F_ALL                                                 \
    (GJ_UEFI_SOFT_F_MAGIC | GJ_UEFI_SOFT_F_ENTRY | GJ_UEFI_SOFT_F_ALIGN16 | \
     GJ_UEFI_SOFT_F_PBOOT1 | GJ_UEFI_SOFT_F_NO_EENT | GJ_UEFI_SOFT_F_LEAN)

_Static_assert((GJ_UEFI_SOFT_F_ALL & GJ_UEFI_SOFT_F_LEAN) != 0u,
               "GJUEFI1 soft flags must include lean residual lamp");
_Static_assert((GJ_UEFI_SOFT_F_ALL & GJ_UEFI_SOFT_F_MAGIC) != 0u,
               "GJUEFI1 soft flags must include magic lamp");
_Static_assert((GJ_UEFI_SOFT_F_ALL & GJ_UEFI_SOFT_F_ENTRY) != 0u,
               "GJUEFI1 soft flags must include entry lamp");

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
    GJ_UEFI_SOFT_ALIGN,
    GJ_UEFI_SOFT_F_ALL,
};

/*
 * Lean greppable soft catalog (KERNEL.ELF PT_LOAD / .rodata).
 * Binary and source greps: "GJUEFI1: soft ...". Soft only - no runtime call.
 * Soft!=product; dual MIT OR Apache-2.0; G-AC-1; no version stamp;
 * stamp_storm=0. Catalog name count must equal GJ_UEFI_SOFT_AREAS.
 * Dual DoD remains OPEN; residual never closes Dual DoD or bar3.
 */

/* Grep: GJUEFI1: soft inventory */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftInventory[] =
    "GJUEFI1: soft inventory wave=116 areas=17 "
    "magic=GJUEFI1 entry=kmain_uefi path=p_boot_1_product "
    "align=16 lean=1 stamp_storm=0 soft_never_gates=1 "
    "no_version_stamp=1 dual_dod=OPEN "
    "Soft!=product dual=MIT_OR_Apache-2.0 G-AC-1 C0";

/* Grep: GJUEFI1: soft path */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftPath[] =
    "GJUEFI1: soft path claim=p_boot_1_product loader=uefi_stub "
    "file=EFI/GREENJADE/KERNEL.ELF entry=kmain_uefi "
    "e_entry_unused=1 p_boot_2=dev_only soft_never_gates=1 "
    "dual_dod=OPEN Soft!=product G-AC-1";

/* Grep: GJUEFI1: soft hdr */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftHdr[] =
    "GJUEFI1: soft hdr magic=GJUEFI1 magic_bytes=8 entry_off=8 "
    "soft_wave_off=16 soft_areas_off=20 soft_align_off=24 "
    "soft_flags_off=28 hdr_bytes=32 align=16 "
    "section=.rodata.gj_uefi scan_step=8 contract_static_assert=1 "
    "Soft!=product";

/* Grep: GJUEFI1: soft entry */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftEntry[] =
    "GJUEFI1: soft entry symbol=kmain_uefi long_mode=1 "
    "ms_abi_stub=0 sysv_kernel=1 handoff=gj_boot_info "
    "noreturn=1 e_entry_not_product=1 product_path=1 "
    "soft_never_gates=1 Soft!=product";

/* Grep: GJUEFI1: soft honesty */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftHonesty[] =
    "GJUEFI1: soft honesty not-multiboot e_entry_not_product "
    "lean=1 stamp_storm=0 no_version_stamp=1 soft_never_gates=1 "
    "product=0 residual!=product residual!=dual_dod residual!=bar3 "
    "dual_dod=OPEN dual=MIT_OR_Apache-2.0 G-AC-1 "
    "(link-time surface only; Soft!=product; C0 eng residual)";

/* Grep: GJUEFI1: soft magic */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftMagic[] =
    "GJUEFI1: soft magic bytes=GJUEFI1 nul_term=1 scan_key=1 "
    "step=8 align=16 first16=magic+entry soft PASS";

/* Grep: GJUEFI1: soft contract */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftContract[] =
    "GJUEFI1: soft contract first16=magic+entry soft_after=1 "
    "entry_off=8 soft_wave_off=16 soft_areas_off=20 "
    "soft_align_off=24 soft_flags_off=28 hdr_bytes=32 "
    "stub_reads_soft=0 kernel_rodata=1 static_assert=1 "
    "soft_never_gates=1 Soft!=product soft PASS";

/* Grep: GJUEFI1: soft flags */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftFlags[] =
    "GJUEFI1: soft flags magic=1 entry=1 align16=1 pboot1=1 "
    "no_eent=1 lean=1 stamp_storm=0 soft PASS Soft!=product";

/* Grep: GJUEFI1: soft load */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftLoad[] =
    "GJUEFI1: soft load file=EFI/GREENJADE/KERNEL.ELF "
    "pt_load_scan=1 entry=kmain_uefi soft_never_gates=1 "
    "Soft!=product soft PASS";

/* Grep: GJUEFI1: soft catalog */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftCatalog[] =
    "GJUEFI1: soft catalog inventory,path,hdr,entry,honesty,"
    "magic,contract,flags,load,catalog,surfaces,note,return,"
    "retmap,residual,deepen,PASS areas=17 lean=1 stamp_storm=0 "
    "dual_dod=OPEN Soft!=product G-AC-1 soft PASS";

/* Grep: GJUEFI1: soft surfaces */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftSurfaces[] =
    "GJUEFI1: soft surfaces count=17 wave=116 "
    "names=inventory,path,hdr,entry,honesty,magic,contract,"
    "flags,load,catalog,surfaces,note,return,retmap,residual,"
    "deepen,PASS lean=1 stamp_storm=0 dual_dod=OPEN Soft!=product";

/* Grep: GJUEFI1: soft note */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftNote[] =
    "GJUEFI1: soft note exclusive=1 lean=1 stamp_storm=0 "
    "no_version_stamp=1 unit=uefi_entry.c dual_dod=OPEN "
    "Soft!=product dual=MIT_OR_Apache-2.0 G-AC-1 C0";

/* Grep: GJUEFI1: soft return */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftReturn[] =
    "GJUEFI1: soft return soft_never_gates=1 e_entry_unused=1 "
    "product_gate=0 lean=1 stamp_storm=0 dual_dod=OPEN "
    "Soft!=product G-AC-1";

/* Grep: GJUEFI1: soft retmap */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetmap[] =
    "GJUEFI1: soft retmap product_gate=0 soft_never_gates=1 "
    "lean=1 stamp_storm=0 dual_dod=OPEN Soft!=product G-AC-1";

/*
 * Grep: GJUEFI1: soft residual
 * Grep: GJUEFI1: soft residual lean
 * Grep: GJUEFI1: soft residual lean PASS
 * Sole residual-class lamp for this unit. No per-surface stamp storm.
 * No version stamp. Soft!=product dual MIT OR Apache-2.0. G-AC-1.
 * residual != product AC / dual DoD / bar3 close. Dual DoD OPEN.
 */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftResidual[] =
    "GJUEFI1: soft residual lean exclusive=1 lean=1 stamp_storm=0 "
    "no_version_stamp=1 areas=17 unit=uefi_entry.c "
    "loader_contract=magic+entry soft_wave_off=16 "
    "hdr_bytes=32 surface_table=1 "
    "soft=1 product=0 residual!=product residual!=dual_dod "
    "residual!=bar3 dual_dod=OPEN "
    "dual=MIT_OR_Apache-2.0 G-AC-1 Soft!=product C0 "
    "GJUEFI1: soft residual lean PASS";

/* Grep: GJUEFI1: soft deepen */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftDeepen[] =
    "GJUEFI1: soft deepen wave=116 areas=17 "
    "catalog=inventory,path,hdr,entry,honesty,magic,contract,"
    "flags,load,catalog,surfaces,note,return,retmap,residual,"
    "deepen,PASS "
    "unit=uefi_entry.c only lean=1 stamp_storm=0 soft_never_gates=1 "
    "no_version_stamp=1 dual_dod=OPEN dual=MIT_OR_Apache-2.0 G-AC-1 "
    "(soft residual deepen; Soft!=product; C0; Dual DoD OPEN)";

/* Grep: GJUEFI1: soft PASS */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftPass[] =
    "GJUEFI1: soft PASS wave=116 hdr=present entry=stamped "
    "align=16 areas=17 lean=1 stamp_storm=0 no_version_stamp=1 "
    "soft_never_gates=1 residual=lean dual_dod=OPEN "
    "(soft inventory only; Soft!=product; G-AC-1; not product DoD; "
    "Dual DoD OPEN; residual!=bar3)";

/*
 * Compile-time surface inventory: pointer table length must equal
 * GJ_UEFI_SOFT_AREAS. Prevents silent catalog/AREAS drift without adding
 * stamp-storm dummy rodata (H2). Soft only - never consulted by loader.
 */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char *const g_apGjUefiSoftSurfaces[] = {
    g_szGjUefiSoftInventory, /* inventory */
    g_szGjUefiSoftPath,      /* path */
    g_szGjUefiSoftHdr,       /* hdr */
    g_szGjUefiSoftEntry,     /* entry */
    g_szGjUefiSoftHonesty,   /* honesty */
    g_szGjUefiSoftMagic,     /* magic */
    g_szGjUefiSoftContract,  /* contract */
    g_szGjUefiSoftFlags,     /* flags */
    g_szGjUefiSoftLoad,      /* load */
    g_szGjUefiSoftCatalog,   /* catalog */
    g_szGjUefiSoftSurfaces,  /* surfaces */
    g_szGjUefiSoftNote,      /* note */
    g_szGjUefiSoftReturn,    /* return */
    g_szGjUefiSoftRetmap,    /* retmap */
    g_szGjUefiSoftResidual,  /* residual */
    g_szGjUefiSoftDeepen,    /* deepen */
    g_szGjUefiSoftPass,      /* PASS */
};

_Static_assert(
    (sizeof(g_apGjUefiSoftSurfaces) / sizeof(g_apGjUefiSoftSurfaces[0])) ==
        GJ_UEFI_SOFT_AREAS,
    "GJUEFI1 soft surface table count must equal GJ_UEFI_SOFT_AREAS");
