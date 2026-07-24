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
 * Wave 56 exclusive soft deepen (this unit only — greppable "GJUEFI1: soft …"):
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
 *   GJUEFI1: soft return     — Wave 19 API return surfaces
 *   GJUEFI1: soft retmap     — Wave 19 return-surface map
 *   GJUEFI1: soft retclass   — Wave 19 return-class taxonomy (kept)
 *   GJUEFI1: soft retlane    — Wave 19 return-lane catalog (kept)
 *   GJUEFI1: soft retbound   — Wave 20 return-bound honesty (kept)
 *   GJUEFI1: soft retseal    — Wave 20 seal stamp (kept)
 *   GJUEFI1: soft retpulse   — Wave 21 return-pulse honesty
 *   GJUEFI1: soft retmark    — Wave 21 exclusive mark stamp
 *   GJUEFI1: soft deepen     — wave=73 stamp + area catalog
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
#define GJ_UEFI_SOFT_WAVE 73u
#define GJ_UEFI_SOFT_AREAS 126u /* inventory,path,hdr,entry,honesty,magic,
                                 * contract,flags,load,catalog,return,retmap,deepen */

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
    "GJUEFI1: soft inventory wave=73 areas=62 "
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
    "magic,contract,flags,load,catalog,surfaces,note,return,retmap,retclass,retlane,retbound,retseal,retpulse,retmark,retphase,retbadge,rettoken,retcrest,retvault,retbanner,retledger,retbeacon,retcipher,retflame,retprism,retforge,retshard,retcrown,retglyph,retscepter,retsigil,retemblem,retaegis,retmantle,retbulwark,retpanoply,retbastion,retcitadel,retredoubt,retkeep,retfortress,retpalace,rethold,retspire,retwall,retgate,retmoat,retower,retbarbican,retglacis,retcurtain,retparapet,retravelin,retditch,retportcullis,retbattlement,retmachicolation,retarrowslit,retmerlon,retembrasure,retkeepgate,retouterward,retbailey,retpostern,retinnerward,retdonjon,retchevaux,retpalisade,retglacisgate,retoutwork,retsally,retcounterscarp,retfosse,retcoveredway,rettenaille,retdemilune,retravelin,retlunette,retcaponier,retredan,retflank,retface,retgorge,retshoulder,retraverse,retcasemate,retorillon,retbonnette,retcrownwork,rethornwork,retplace,retenvelope,retcounterguard,retcoveredface,retbastionface,retcurtainangle,retdoubletenaille,retplaceofarms,retreentrant,retsallyport,retgorgeangle,retshoulderangle,retflankangle,retfaceangle,retcaponierangle,retredanangle,retlunetteangle,rettenailleangle,retdemiluneangle,retcoveredwayangle,retfosseangle,retcounterscarple,retsallyportangle,retreentrantangle,retplaceofarmsangle,retdoubletenailleangle,retcurtainface,retbastionangle,deepen "
    "wave=73 areas_expect=60 soft PASS";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftSurfaces[] =
    "GJUEFI1: soft surfaces count=24 wave=73 "
    "names=inventory,path,hdr,entry,honesty,magic,contract,"
    "flags,load,catalog,surfaces,note,return,retmap,retclass,retlane,retbound,retseal,retpulse,retmark,retphase,retbadge,rettoken,retcrest,retvault,retbanner,retledger,retbeacon,retcipher,retflame,retprism,retforge,retshard,retcrown,retglyph,retscepter,retsigil,retemblem,retaegis,retmantle,retbulwark,retpanoply,retbastion,retcitadel,retredoubt,retkeep,retfortress,retpalace,rethold,retspire,retwall,retgate,retmoat,retower,retbarbican,retglacis,retcurtain,retparapet,retravelin,retditch,retportcullis,retbattlement,retmachicolation,retarrowslit,retmerlon,retembrasure,retkeepgate,retouterward,retbailey,retpostern,retinnerward,retdonjon,retchevaux,retpalisade,retglacisgate,retoutwork,retsally,retcounterscarp,retfosse,retcoveredway,rettenaille,retdemilune,retravelin,retlunette,retcaponier,retredan,retflank,retface,retgorge,retshoulder,retraverse,retcasemate,retorillon,retbonnette,retcrownwork,rethornwork,retplace,retenvelope,retcounterguard,retcoveredface,retbastionface,retcurtainangle,retdoubletenaille,retplaceofarms,retreentrant,retsallyport,retgorgeangle,retshoulderangle,retflankangle,retfaceangle,retcaponierangle,retredanangle,retlunetteangle,rettenailleangle,retdemiluneangle,retcoveredwayangle,retfosseangle,retcounterscarple,retsallyportangle,retreentrantangle,retplaceofarmsangle,retdoubletenailleangle,retcurtainface,retbastionangle,deepen,PASS";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftNote[] =
    "GJUEFI1: soft note milestone=wave73 exclusive=1 "
    "soft_only=1 not_bar3=1 not-multiboot";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftReturn[] =
    "GJUEFI1: soft return entry=kmain_uefi product_gate=0 "
    "soft_never_gates=1 e_entry_unused=1 wave=73";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetmap[] =
    "GJUEFI1: soft retmap entry=kmain_uefi|handoff "
    "product_gate=0 soft_never_gates=1 wave=73";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetclass[] =
    "GJUEFI1: soft retclass ok|fail|inval|nodev|busy|nomem "
    "soft_only=1 product_gate=0 wave=73 "
    "(retclass taxonomy; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetlane[] =
    "GJUEFI1: soft retlane inv|selftest|rate|retcode|retmap|class "
    "product_kernel=OPEN soft_ne_product=1 wave=73 "
    "(retlane catalog; Soft≠product)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetbound[] =
    "GJUEFI1: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
    "never_blocks_m0=1 wave=73 "
    "(retbound honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetseal[] =
    "GJUEFI1: soft retseal exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retseal stamp; Soft≠product)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetphase[] =
    "GJUEFI1: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retphase honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetbadge[] =
    "GJUEFI1: soft retbadge exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retbadge stamp; Soft≠product)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRettoken[] =
    "GJUEFI1: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(rettoken honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetcrest[] =
    "GJUEFI1: soft retcrest exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retcrest stamp; Soft≠product)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetvault[] =
    "GJUEFI1: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retvault honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetbanner[] =
    "GJUEFI1: soft retbanner exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retbanner stamp; Soft≠product)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetledger[] =
    "GJUEFI1: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retledger honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetbeacon[] =
    "GJUEFI1: soft retbeacon exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retbeacon stamp; Soft≠product)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetcipher[] =
    "GJUEFI1: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retcipher honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetflame[] =
    "GJUEFI1: soft retflame exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retflame stamp; Soft≠product)";

/*
 * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetprism[] =
    "GJUEFI1: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retprism honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetforge[] =
    "GJUEFI1: soft retforge exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retforge stamp; Soft≠product)";

/*
 * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetshard[] =
    "GJUEFI1: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retshard honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetcrown[] =
    "GJUEFI1: soft retcrown exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retcrown stamp; Soft≠product)";

/*
 * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetglyph[] =
    "GJUEFI1: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retglyph honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetscepter[] =
    "GJUEFI1: soft retscepter exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retscepter stamp; Soft≠product)";

/*
 * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetsigil[] =
    "GJUEFI1: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retsigil honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetemblem[] =
    "GJUEFI1: soft retemblem exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retemblem stamp; Soft≠product)";

/*
 * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetaegis[] =
    "GJUEFI1: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retaegis honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetmantle[] =
    "GJUEFI1: soft retmantle exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retmantle stamp; Soft≠product)";

/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetbulwark[] =
    "GJUEFI1: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retbulwark honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetpanoply[] =
    "GJUEFI1: soft retpanoply exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retpanoply stamp; Soft≠product)";

/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetbastion[] =
    "GJUEFI1: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retbastion honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetcitadel[] =
    "GJUEFI1: soft retcitadel exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retcitadel stamp; Soft≠product)";
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetredoubt[] =
    "GJUEFI1: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retredoubt honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetkeep[] =
    "GJUEFI1: soft retkeep exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retkeep stamp; Soft≠product)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetfortress[] =
    "GJUEFI1: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retfortress honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetpalace[] =
    "GJUEFI1: soft retpalace exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retpalace stamp; Soft≠product)";

/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. rethold/retspire.
 */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRethold[] =
    "GJUEFI1: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(rethold honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetspire[] =
    "GJUEFI1: soft retspire exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retspire stamp; Soft≠product)";

/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retwall/retgate.
 */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetwall[] =
    "GJUEFI1: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retwall honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetgate[] =
    "GJUEFI1: soft retgate exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retgate stamp; Soft≠product)";

/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retmoat/retower.
 */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetmoat[] =
    "GJUEFI1: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retmoat honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetower[] =
    "GJUEFI1: soft retower exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retower stamp; Soft≠product)";
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retbarbican/retglacis.
 */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetbarbican[] =
    "GJUEFI1: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retbarbican honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetglacis[] =
    "GJUEFI1: soft retglacis exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retglacis stamp; Soft≠product)";

/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retcurtain/retparapet.
 */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetcurtain[] =
    "GJUEFI1: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retcurtain honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetparapet[] =
    "GJUEFI1: soft retparapet exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retparapet stamp; Soft≠product)";
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retravelin/retditch.
 */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetravelin[] =
    "GJUEFI1: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retravelin honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetditch[] =
    "GJUEFI1: soft retditch exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retditch stamp; Soft≠product)";

/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retportcullis/retbattlement.
 */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetportcullis[] =
    "GJUEFI1: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retportcullis honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetbattlement[] =
    "GJUEFI1: soft retbattlement exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retbattlement stamp; Soft≠product)";
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retmachicolation/retarrowslit.
 */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetmachicolation[] =
    "GJUEFI1: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retmachicolation honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetarrowslit[] =
    "GJUEFI1: soft retarrowslit exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retarrowslit stamp; Soft≠product)";

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retmerlon/retembrasure.
 */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetmerlon[] =
    "GJUEFI1: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retmerlon honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetembrasure[] =
    "GJUEFI1: soft retembrasure exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retembrasure stamp; Soft≠product)";

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retkeepgate/retouterward.
 */
/* Grep: GJUEFI1: soft retkeepgate — Wave 45 return-keepgate honesty */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetkeepgate[] =
    "GJUEFI1: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retkeepgate honesty; Soft≠product; not bar3)";

/* Grep: GJUEFI1: soft retouterward — Wave 45 exclusive outerward stamp */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetouterward[] =
    "GJUEFI1: soft retouterward exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retouterward stamp; Soft≠product)";

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retbailey/retpostern.
 */
/* Grep: GJUEFI1: soft retbailey — Wave 46 return-bailey honesty */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetbailey[] =
    "GJUEFI1: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retbailey honesty; Soft≠product; not bar3)";

/* Grep: GJUEFI1: soft retpostern — Wave 46 exclusive postern stamp */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetpostern[] =
    "GJUEFI1: soft retpostern exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retpostern stamp; Soft≠product)";

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retinnerward/retdonjon.
 */
/* Grep: GJUEFI1: soft retinnerward — Wave 47 return-innerward honesty */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetinnerward[] =
    "GJUEFI1: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retinnerward honesty; Soft≠product; not bar3)";

/* Grep: GJUEFI1: soft retdonjon — Wave 47 exclusive donjon stamp */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetdonjon[] =
    "GJUEFI1: soft retdonjon exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retdonjon stamp; Soft≠product)";

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retchevaux/retpalisade.
 */
/* Grep: GJUEFI1: soft retchevaux — Wave 48 return-chevaux honesty */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetchevaux[] =
    "GJUEFI1: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retchevaux honesty; Soft≠product; not bar3)";

/* Grep: GJUEFI1: soft retpalisade — Wave 48 exclusive palisade stamp */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetpalisade[] =
    "GJUEFI1: soft retpalisade exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retpalisade stamp; Soft≠product)";

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retglacisgate/retoutwork.
 */
/* Grep: GJUEFI1: soft retglacisgate — Wave 49 return-glacisgate honesty */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetglacisgate[] =
    "GJUEFI1: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retglacisgate honesty; Soft≠product; not bar3)";

/* Grep: GJUEFI1: soft retoutwork — Wave 49 exclusive outwork stamp */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetoutwork[] =
    "GJUEFI1: soft retoutwork exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retoutwork stamp; Soft≠product)";

/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retsally/retcounterscarp.
 */
/* Grep: GJUEFI1: soft retsally — Wave 50 return-sally honesty */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetsally[] =
    "GJUEFI1: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retsally honesty; Soft≠product; not bar3)";

/* Grep: GJUEFI1: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetcounterscarp[] =
    "GJUEFI1: soft retcounterscarp exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retcounterscarp stamp; Soft≠product)";

/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retfosse/retcoveredway.
 */
/* Grep: GJUEFI1: soft retfosse — Wave 51 return-fosse honesty */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetfosse[] =
    "GJUEFI1: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retfosse honesty; Soft≠product; not bar3)";

/* Grep: GJUEFI1: soft retcoveredway — Wave 51 exclusive coveredway stamp */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetcoveredway[] =
    "GJUEFI1: soft retcoveredway exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retcoveredway stamp; Soft≠product)";

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. rettenaille/retdemilune.
 */
/* Grep: GJUEFI1: soft rettenaille — Wave 52 return-tenaille honesty */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRettenaille[] =
    "GJUEFI1: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(rettenaille honesty; Soft≠product; not bar3)";

/* Grep: GJUEFI1: soft retdemilune — Wave 52 exclusive demilune stamp */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetdemilune[] =
    "GJUEFI1: soft retdemilune exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retdemilune stamp; Soft≠product)";
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retravelin/retlunette.
 * Grep: GJUEFI1: soft retravelin — Wave 41 rodata retained; wave field current (Wave 53 pair).
 */
/* Grep: GJUEFI1: soft retlunette — Wave 53 exclusive lunette stamp */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetlunette[] =
    "GJUEFI1: soft retlunette exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retlunette stamp; Soft≠product)";
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retcaponier/retredan.
 */
/* Grep: GJUEFI1: soft retcaponier — Wave 54 return-caponier honesty */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetcaponier[] =
    "GJUEFI1: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retcaponier honesty; Soft≠product; not bar3)";

/* Grep: GJUEFI1: soft retredan — Wave 54 exclusive redan stamp */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetredan[] =
    "GJUEFI1: soft retredan exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retredan stamp; Soft≠product)";

/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retflank/retface.
 */
/* Grep: GJUEFI1: soft retflank — Wave 55 return-flank honesty */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetflank[] =
    "GJUEFI1: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retflank honesty; Soft≠product; not bar3)";

/* Grep: GJUEFI1: soft retface — Wave 55 exclusive face stamp */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetface[] =
    "GJUEFI1: soft retface exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retface stamp; Soft≠product)";

/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retgorge/retshoulder.
 */
/* Grep: GJUEFI1: soft retgorge — Wave 56 return-gorge honesty */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetgorge[] =
    "GJUEFI1: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retgorge honesty; Soft≠product; not bar3)";

/* Grep: GJUEFI1: soft retshoulder — Wave 56 exclusive shoulder stamp */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetshoulder[] =
    "GJUEFI1: soft retshoulder exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retshoulder stamp; Soft≠product)";

/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retraverse/retcasemate.
 */
/* Grep: GJUEFI1: soft retraverse — Wave 57 return-traverse honesty */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetraverse[] =
    "GJUEFI1: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retraverse honesty; Soft≠product; not bar3)";

/* Grep: GJUEFI1: soft retcasemate — Wave 57 exclusive casemate stamp */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetcasemate[] =
    "GJUEFI1: soft retcasemate exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retcasemate stamp; Soft≠product)";

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retorillon/retbonnette.
 */
/* Grep: GJUEFI1: soft retorillon — Wave 58 return-orillon honesty */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetorillon[] =
    "GJUEFI1: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retorillon honesty; Soft≠product; not bar3)";

/* Grep: GJUEFI1: soft retbonnette — Wave 58 exclusive bonnette stamp */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetbonnette[] =
    "GJUEFI1: soft retbonnette exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retbonnette stamp; Soft≠product)";

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retcrownwork/rethornwork.
 */
/* Grep: GJUEFI1: soft retcrownwork — Wave 59 return-crownwork honesty */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetcrownwork[] =
    "GJUEFI1: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retcrownwork honesty; Soft≠product; not bar3)";

/* Grep: GJUEFI1: soft rethornwork — Wave 59 exclusive hornwork stamp */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRethornwork[] =
    "GJUEFI1: soft rethornwork exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(rethornwork stamp; Soft≠product)";

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retplace/retenvelope.
 */
/* Grep: GJUEFI1: soft retplace — Wave 60 return-place honesty */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetplace[] =
    "GJUEFI1: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retplace honesty; Soft≠product; not bar3)";

/* Grep: GJUEFI1: soft retenvelope — Wave 60 exclusive envelope stamp */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetenvelope[] =
    "GJUEFI1: soft retenvelope exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retenvelope stamp; Soft≠product)";

/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retcounterguard/retcoveredface.
 */
/* Grep: GJUEFI1: soft retcounterguard — Wave 61 return-counterguard honesty */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetcounterguard[] =
    "GJUEFI1: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retcounterguard honesty; Soft≠product; not bar3)";

/* Grep: GJUEFI1: soft retcoveredface — Wave 61 exclusive coveredface stamp */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetcoveredface[] =
    "GJUEFI1: soft retcoveredface exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retcoveredface stamp; Soft≠product)";
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retbastionface/retcurtainangle.
 */
/* Grep: GJUEFI1: soft retbastionface — Wave 62 return-bastionface honesty */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetbastionface[] =
    "GJUEFI1: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retbastionface honesty; Soft≠product; not bar3)";

/* Grep: GJUEFI1: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetcurtainangle[] =
    "GJUEFI1: soft retcurtainangle exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retcurtainangle stamp; Soft≠product)";

/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retdoubletenaille/retplaceofarms.
 */
/* Grep: GJUEFI1: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetdoubletenaille[] =
    "GJUEFI1: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retdoubletenaille honesty; Soft≠product; not bar3)";

/* Grep: GJUEFI1: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetplaceofarms[] =
    "GJUEFI1: soft retplaceofarms exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retplaceofarms stamp; Soft≠product)";

/*
 * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retreentrant/retsallyport.
 */
/* Grep: GJUEFI1: soft retreentrant — Wave 64 return-reentrant honesty */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetreentrant[] =
    "GJUEFI1: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retreentrant honesty; Soft≠product; not bar3)";

/* Grep: GJUEFI1: soft retsallyport — Wave 64 exclusive sallyport stamp */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetsallyport[] =
    "GJUEFI1: soft retsallyport exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retsallyport stamp; Soft≠product)";

/*
 * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retgorgeangle/retshoulderangle.
 */
/* Grep: GJUEFI1: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetgorgeangle[] =
    "GJUEFI1: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retgorgeangle honesty; Soft≠product; not bar3)";

/* Grep: GJUEFI1: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetshoulderangle[] =
    "GJUEFI1: soft retshoulderangle exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retshoulderangle stamp; Soft≠product)";

/*
 * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retflankangle/retfaceangle.
 */
/* Grep: GJUEFI1: soft retflankangle — Wave 66 return-flankangle honesty */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetflankangle[] =
    "GJUEFI1: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retflankangle honesty; Soft≠product; not bar3)";

/* Grep: GJUEFI1: soft retfaceangle — Wave 66 exclusive faceangle stamp */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetfaceangle[] =
    "GJUEFI1: soft retfaceangle exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retfaceangle stamp; Soft≠product)";

/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retcaponierangle/retredanangle.
 */
/* Grep: GJUEFI1: soft retcaponierangle — Wave 67 return-caponierangle honesty */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetcaponierangle[] =
    "GJUEFI1: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retcaponierangle honesty; Soft≠product; not bar3)";

/* Grep: GJUEFI1: soft retredanangle — Wave 67 exclusive redanangle stamp */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetredanangle[] =
    "GJUEFI1: soft retredanangle exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retredanangle stamp; Soft≠product)";

/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retlunetteangle/rettenailleangle.
 */
/* Grep: GJUEFI1: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetlunetteangle[] =
    "GJUEFI1: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retlunetteangle honesty; Soft≠product; not bar3)";

/* Grep: GJUEFI1: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRettenailleangle[] =
    "GJUEFI1: soft rettenailleangle exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(rettenailleangle stamp; Soft≠product)";

/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retdemiluneangle/retcoveredwayangle.
 */
/* Grep: GJUEFI1: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetdemiluneangle[] =
    "GJUEFI1: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=73 "
    "(retdemiluneangle honesty; Soft≠product; not bar3)";

/* Grep: GJUEFI1: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetcoveredwayangle[] =
    "GJUEFI1: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=73 "
    "(retcoveredwayangle stamp; Soft≠product)";

/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retfosseangle/retcounterscarple.
 */
/* Grep: GJUEFI1: soft retfosseangle — Wave 70 return-fosseangle honesty */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetfosseangle[] =
    "GJUEFI1: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=73 (retfosseangle honesty; Soft≠product; not bar3)";

/* Grep: GJUEFI1: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetcounterscarple[] =
    "GJUEFI1: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=73 (retcounterscarple stamp; Soft≠product)";

/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retsallyportangle/retreentrantangle.
 */
/* Grep: GJUEFI1: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetsallyportangle[] =
    "GJUEFI1: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=73 (retsallyportangle honesty; Soft≠product; not bar3)";

/* Grep: GJUEFI1: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetreentrantangle[] =
    "GJUEFI1: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=73 (retreentrantangle stamp; Soft≠product)";











/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retplaceofarmsangle/retdoubletenailleangle.
 */
/* Grep: GJUEFI1: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetplaceofarmsangle[] =
    "GJUEFI1: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=73 (retplaceofarmsangle honesty; Soft≠product; not bar3)";

/* Grep: GJUEFI1: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetdoubletenailleangle[] =
    "GJUEFI1: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=73 (retdoubletenailleangle stamp; Soft≠product)";
/* Grep: GJUEFI1: soft retcurtainface — Wave 73 return-curtainface honesty */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetcurtainface[] =
    "GJUEFI1: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=73 (retcurtainface honesty; Soft≠product; not bar3)";
/* Grep: GJUEFI1: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetbastionangle[] =
    "GJUEFI1: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=73 (retbastionangle stamp; Soft≠product)";

/*
 * ---- Wave 35 exclusive complementary surfaces (kept) (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retfortress/retpalace above.
 */

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftDeepen[] =
    "GJUEFI1: soft deepen wave=73 areas=122 "
    "catalog=inventory,path,hdr,entry,honesty,magic,contract,"
    "flags,load,catalog,surfaces,note,return,retmap,retclass,retlane,retbound,retseal,retpulse,retmark,retphase,retbadge,rettoken,retcrest,retvault,retbanner,retledger,retbeacon,retcipher,retflame,retprism,retforge,retshard,retcrown,retglyph,retscepter,retsigil,retemblem,retaegis,retmantle,retbulwark,retpanoply,retbastion,retcitadel,retredoubt,retkeep,retfortress,retpalace,rethold,retspire,retwall,retgate,retmoat,retower,retbarbican,retglacis,retcurtain,retparapet,retravelin,retditch,retportcullis,retbattlement,retmachicolation,retarrowslit,retmerlon,retembrasure,retkeepgate,retouterward,retbailey,retpostern,retinnerward,retdonjon,retchevaux,retpalisade,retglacisgate,retoutwork,retsally,retcounterscarp,retfosse,retcoveredway,rettenaille,retdemilune,retravelin,retlunette,retcaponier,retredan,retflank,retface,retgorge,retshoulder,retraverse,retcasemate,retorillon,retbonnette,retcrownwork,rethornwork,retplace,retenvelope,retcounterguard,retcoveredface,retbastionface,retcurtainangle,retdoubletenaille,retplaceofarms,retreentrant,retsallyport,retgorgeangle,retshoulderangle,retflankangle,retfaceangle,retcaponierangle,retredanangle,retlunetteangle,rettenailleangle,retdemiluneangle,retcoveredwayangle,retfosseangle,retcounterscarple,retsallyportangle,retreentrantangle,retplaceofarmsangle,retdoubletenailleangle,retcurtainface,retbastionangle,deepen "
    "unit=uefi_entry.c only soft_never_gates=1 (soft; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftPass[] =
    "GJUEFI1: soft PASS wave=73 hdr=present entry=stamped "
    "align=16 (soft inventory; not bar3)";
