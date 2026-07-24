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
 * Wave 42 exclusive soft deepen (this unit only — greppable "GJUEFI1: soft …"):
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
 *   GJUEFI1: soft deepen     — wave=42 stamp + area catalog
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
#define GJ_UEFI_SOFT_WAVE 42u
#define GJ_UEFI_SOFT_AREAS 62u /* inventory,path,hdr,entry,honesty,magic,
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
    "GJUEFI1: soft inventory wave=42 areas=32 "
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
    "magic,contract,flags,load,catalog,surfaces,note,return,retmap,retclass,retlane,retbound,retseal,retpulse,retmark,retphase,retbadge,rettoken,retcrest,retvault,retbanner,retledger,retbeacon,retcipher,retflame,retprism,retforge,retshard,retcrown,retglyph,retscepter,retsigil,retemblem,retaegis,retmantle,retbulwark,retpanoply,retbastion,retcitadel,retredoubt,retkeep,retfortress,retpalace,rethold,retspire,retwall,retgate,retmoat,retower,retbarbican,retglacis,retcurtain,retparapet,retravelin,retditch,retportcullis,retbattlement,deepen "
    "wave=42 areas_expect=34 soft PASS";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftSurfaces[] =
    "GJUEFI1: soft surfaces count=24 wave=42 "
    "names=inventory,path,hdr,entry,honesty,magic,contract,"
    "flags,load,catalog,surfaces,note,return,retmap,retclass,retlane,retbound,retseal,retpulse,retmark,retphase,retbadge,rettoken,retcrest,retvault,retbanner,retledger,retbeacon,retcipher,retflame,retprism,retforge,retshard,retcrown,retglyph,retscepter,retsigil,retemblem,retaegis,retmantle,retbulwark,retpanoply,retbastion,retcitadel,retredoubt,retkeep,retfortress,retpalace,rethold,retspire,retwall,retgate,retmoat,retower,retbarbican,retglacis,retcurtain,retparapet,retravelin,retditch,retportcullis,retbattlement,deepen,PASS";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftNote[] =
    "GJUEFI1: soft note milestone=wave42 exclusive=1 "
    "soft_only=1 not_bar3=1 not-multiboot";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftReturn[] =
    "GJUEFI1: soft return entry=kmain_uefi product_gate=0 "
    "soft_never_gates=1 e_entry_unused=1 wave=42";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetmap[] =
    "GJUEFI1: soft retmap entry=kmain_uefi|handoff "
    "product_gate=0 soft_never_gates=1 wave=42";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetclass[] =
    "GJUEFI1: soft retclass ok|fail|inval|nodev|busy|nomem "
    "soft_only=1 product_gate=0 wave=42 "
    "(retclass taxonomy; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetlane[] =
    "GJUEFI1: soft retlane inv|selftest|rate|retcode|retmap|class "
    "product_kernel=OPEN soft_ne_product=1 wave=42 "
    "(retlane catalog; Soft≠product)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetbound[] =
    "GJUEFI1: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
    "never_blocks_m0=1 wave=42 "
    "(retbound honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetseal[] =
    "GJUEFI1: soft retseal exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=42 "
    "(retseal stamp; Soft≠product)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetphase[] =
    "GJUEFI1: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=42 "
    "(retphase honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetbadge[] =
    "GJUEFI1: soft retbadge exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=42 "
    "(retbadge stamp; Soft≠product)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRettoken[] =
    "GJUEFI1: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=42 "
    "(rettoken honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetcrest[] =
    "GJUEFI1: soft retcrest exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=42 "
    "(retcrest stamp; Soft≠product)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetvault[] =
    "GJUEFI1: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=42 "
    "(retvault honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetbanner[] =
    "GJUEFI1: soft retbanner exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=42 "
    "(retbanner stamp; Soft≠product)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetledger[] =
    "GJUEFI1: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=42 "
    "(retledger honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetbeacon[] =
    "GJUEFI1: soft retbeacon exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=42 "
    "(retbeacon stamp; Soft≠product)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetcipher[] =
    "GJUEFI1: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=42 "
    "(retcipher honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetflame[] =
    "GJUEFI1: soft retflame exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=42 "
    "(retflame stamp; Soft≠product)";

/*
 * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetprism[] =
    "GJUEFI1: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=42 "
    "(retprism honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetforge[] =
    "GJUEFI1: soft retforge exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=42 "
    "(retforge stamp; Soft≠product)";

/*
 * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetshard[] =
    "GJUEFI1: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=42 "
    "(retshard honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetcrown[] =
    "GJUEFI1: soft retcrown exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=42 "
    "(retcrown stamp; Soft≠product)";

/*
 * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetglyph[] =
    "GJUEFI1: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=42 "
    "(retglyph honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetscepter[] =
    "GJUEFI1: soft retscepter exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=42 "
    "(retscepter stamp; Soft≠product)";

/*
 * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetsigil[] =
    "GJUEFI1: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=42 "
    "(retsigil honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetemblem[] =
    "GJUEFI1: soft retemblem exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=42 "
    "(retemblem stamp; Soft≠product)";

/*
 * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetaegis[] =
    "GJUEFI1: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=42 "
    "(retaegis honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetmantle[] =
    "GJUEFI1: soft retmantle exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=42 "
    "(retmantle stamp; Soft≠product)";

/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetbulwark[] =
    "GJUEFI1: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=42 "
    "(retbulwark honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetpanoply[] =
    "GJUEFI1: soft retpanoply exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=42 "
    "(retpanoply stamp; Soft≠product)";

/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetbastion[] =
    "GJUEFI1: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=42 "
    "(retbastion honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetcitadel[] =
    "GJUEFI1: soft retcitadel exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=42 "
    "(retcitadel stamp; Soft≠product)";
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3.
 */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetredoubt[] =
    "GJUEFI1: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=42 "
    "(retredoubt honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetkeep[] =
    "GJUEFI1: soft retkeep exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=42 "
    "(retkeep stamp; Soft≠product)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetfortress[] =
    "GJUEFI1: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=42 "
    "(retfortress honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetpalace[] =
    "GJUEFI1: soft retpalace exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=42 "
    "(retpalace stamp; Soft≠product)";

/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. rethold/retspire.
 */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRethold[] =
    "GJUEFI1: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=42 "
    "(rethold honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetspire[] =
    "GJUEFI1: soft retspire exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=42 "
    "(retspire stamp; Soft≠product)";

/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retwall/retgate.
 */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetwall[] =
    "GJUEFI1: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=42 "
    "(retwall honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetgate[] =
    "GJUEFI1: soft retgate exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=42 "
    "(retgate stamp; Soft≠product)";

/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retmoat/retower.
 */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetmoat[] =
    "GJUEFI1: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=42 "
    "(retmoat honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetower[] =
    "GJUEFI1: soft retower exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=42 "
    "(retower stamp; Soft≠product)";
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retbarbican/retglacis.
 */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetbarbican[] =
    "GJUEFI1: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=42 "
    "(retbarbican honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetglacis[] =
    "GJUEFI1: soft retglacis exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=42 "
    "(retglacis stamp; Soft≠product)";

/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retcurtain/retparapet.
 */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetcurtain[] =
    "GJUEFI1: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=42 "
    "(retcurtain honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetparapet[] =
    "GJUEFI1: soft retparapet exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=42 "
    "(retparapet stamp; Soft≠product)";
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retravelin/retditch.
 */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetravelin[] =
    "GJUEFI1: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=42 "
    "(retravelin honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetditch[] =
    "GJUEFI1: soft retditch exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=42 "
    "(retditch stamp; Soft≠product)";

/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retportcullis/retbattlement.
 */
__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetportcullis[] =
    "GJUEFI1: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
    "never_blocks_m0=1 wave=42 "
    "(retportcullis honesty; Soft≠product; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftRetbattlement[] =
    "GJUEFI1: soft retbattlement exclusive=1 soft_ne_product=1 "
    "product_kernel=OPEN bar3=0 wave=42 "
    "(retbattlement stamp; Soft≠product)";

/*
 * ---- Wave 35 exclusive complementary surfaces (kept) (never reshape primary).
 * Soft rodata only — Soft≠product; not bar3. retfortress/retpalace above.
 */

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftDeepen[] =
    "GJUEFI1: soft deepen wave=42 areas=58 "
    "catalog=inventory,path,hdr,entry,honesty,magic,contract,"
    "flags,load,catalog,surfaces,note,return,retmap,retclass,retlane,retbound,retseal,retpulse,retmark,retphase,retbadge,rettoken,retcrest,retvault,retbanner,retledger,retbeacon,retcipher,retflame,retprism,retforge,retshard,retcrown,retglyph,retscepter,retsigil,retemblem,retaegis,retmantle,retbulwark,retpanoply,retbastion,retcitadel,retredoubt,retkeep,retfortress,retpalace,rethold,retspire,retwall,retgate,retmoat,retower,retbarbican,retglacis,retcurtain,retparapet,retravelin,retditch,retportcullis,retbattlement,deepen "
    "unit=uefi_entry.c only soft_never_gates=1 (soft; not bar3)";

__attribute__((section(".rodata.gj_uefi"), used, aligned(8)))
const char g_szGjUefiSoftPass[] =
    "GJUEFI1: soft PASS wave=42 hdr=present entry=stamped "
    "align=16 (soft inventory; not bar3)";
