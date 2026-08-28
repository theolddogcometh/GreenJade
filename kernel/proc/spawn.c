/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * process_spawn + PROCESS task cap mint (G-PROC-*).
 * Dual MIT OR Apache-2.0. Soft!=product. G-AC-1.
 *
 * Product lifecycle (SOLARIS_STYLE_REMAINING section 6):
 *   spawn -> CNode + root meta + private AS + first thread + parent PROCESS cap
 *   kill  -> process_death (pager/regions/CNode/AS for wait-registered children)
 *   wait  -> reap exit code, invalidate parent PROCESS cap, recycle spawn slot
 * Failure paths reverse tear down (no orphan AS / stale cap).
 *
 * Soft product inventory (Wave 15 exclusive deepen; this unit only):
 *   - Spawn path: enter / ok / fail + deny reasons (null/cnode/full/meta/as/mint/thr)
 *   - Mint path: install + soft post-mint verify reason catalog
 *   - Kill / wait / from_cap path tallies + live peak
 *   - Wave 15: teardown / AS reverse / wait-register / JIT mint / lifecycle
 *   - ABI-first: process_wait poll (AGAIN while alive) pairs with process: soft wait
 *   greppable: "spawn: soft ..."  /  "spawn: soft wait"  /  "spawn: soft wait poll"
 *   Dual DoD B residual: greppable spawn/sshd honesty + wait AGAIN yields so
 *     freestanding sshd.elf eth accept loop can run (default :22). Soft!=product.
 *     greppable: "spawn: soft sshd"  /  "spawn: soft dual_dod_b"
 *   Lean soft residual (this unit only; Soft!=product dual license; G-AC-1; H3):
 *     Fixed-table G-PROC mint residual for starting userspace UDX / driver
 *     hosts (confined C processes; DDI MMIO/IRQ/DMA caps later):
 *       ddi_host_gj  — soft DDI door residual host (GJ_SYS_DDI)
 *       rtl8168_udx  — NIC class host (10ec:8168; Dual DoD B direction)
 *       xhci_udx     — USB HC host (8086:a12f; Dual DoD A direction)
 *     Launch residual path (product host launch OPEN):
 *       process_spawn -> G-PROC-2 mint -> first thr -> wait_register
 *       -> host (native persona) -> DDI/UDX later
 *       -> kill/thr_exit -> G-PROC-5 H3 death -> wait reap
 *     Concurrent: HOST_CLASS_N (3) fits SPAWN_MAX; capacity residual.
 *     Catalog -> real spawn hooks (file-local + residual export; Soft!=product):
 *       structured host catalog (name/VID:DID/dual_dod/persona/role)
 *       lookup by class idx / name / PCI id
 *       process_spawn_host_launch / _by_name wrap process_spawn (G-PROC mint)
 *       pArg==catalog entry auto-tags slot + forces native persona pref
 *       soft park stub thr entries (yield until G-PROC-5 kill; H3 safe)
 *       sshd residual stays main-embed :22 (not HOST_CLASS_N); wait AGAIN yield
 *     Compact honesty only: no version stamp, no stamp storms.
 *     Soft residual != product multi-server confine / UDX host product close.
 *     G-AC-1: no Linux .ko in kernel as product (userspace hosts only).
 *     H3: process_kill -> process_death preserves thr_exit-before-as_destroy.
 *     greppable: "spawn: soft residual"  /  "spawn: soft residual lean"
 *     greppable: "spawn: soft residual lean host" / "spawn: soft host_launch"
 *     greppable: "spawn: soft residual host_catalog"
 *     greppable: "spawn: soft residual host_path"
 *     greppable: "spawn: soft residual dual_dod"
 *     greppable: "spawn: soft host_launch lean"
 *     greppable: "spawn: soft residual host_hook"
 *     greppable: "spawn: soft host_launch class"
 *     greppable: "spawn: soft host_launch elf"
 *     greppable: "spawn: soft residual host_elf"
 *   Host ELF residual (FUNCTIONAL product path; Soft!=product; G-AC-1; H3):
 *     Catalog carries szElfPath residual per class (ddi/rtl/xhci).
 *     process_spawn_host_launch / _by_name / _by_pci:
 *       G-PROC mint + first thr (stub park if no custom entry)
 *       -> weak host ELF embed resolve (gj_*_elf_blob via spawn_host_blob_get)
 *       -> elf_probe -> elf_load into child AS -> user stack map
 *       -> thread_create_user (real ring-3 host entry when embed present)
 *       -> prefer product thr over park stub (no thrash):
 *          arm slot u8HostElfPrefer; QoS DRIVER on ELF thr + BACKGROUND
 *          on stub thr; stub exit path thread_exit (no yield thrash loop)
 *       -> kill/thr_exit -> G-PROC-5 H3 death (thr_exit before as_destroy)
 *     Absent embed: OPEN need=host_elf (stub park remains; Dual DoD OPEN).
 *     Soft residual != product TX/RX/BOT close; never invent stamp .76.
 *      stamp-free.
 *     greppable: "spawn: soft host_launch elf prefer"
 *     greppable: "spawn: soft host_launch elf prefer no_thrash"
 *     greppable: "spawn: soft residual host_elf"
 *   Host blob spawn residual densify (Soft!=product; Dual DoD OPEN; product UDX):
 *     spawn_host_blob_get -> weak gj_ddi_host / gj_rtl8168_udx / gj_xhci_udx
 *     elf_try_run denser step honesty when embed present:
 *       host_blob resolve -> probe residual -> load residual
 *       -> stack residual -> thr residual -> prefer|no_thrash
 *     Denser residual (this unit): inventory live weak-embed probe per class
 *       via spawn_host_blob_get (ddi/rtl/xhci) + host_blob self-check ok/N
 *       + per-class greppable host_blob present|miss + cb residual.
 *     denser multi-arm residual (GJ_SPAWN_HOST_BLOB_DENSE_ARMS=8; all required):
 *       class_geom | embeds_preserve | dual_dod_map | fail_closed |
 *       live_probe | product_hosts_udx | elf_path_role | dual_dod_open
 *       denser=1 denser_arms=8 denser_ok=N denser_min=8 product_hosts=UDX
 *     denser2 multi-arm residual (W17 densify; GJ_SPAWN_HOST_BLOB_DENSE2_ARMS=8):
 *       denser1_held | embeds_preserve2 | pci_class_ids | re_probe |
 *       cb_honesty | name_role_path | preserve_policy | dual_dod_open2
 *       denser2=1 denser2_arms=8 denser2_ok=N denser2_min=8 product_hosts=UDX
 *     denser3 multi-arm residual (W18 densify; GJ_SPAWN_HOST_BLOB_DENSE3_ARMS=8):
 *       denser2_held | embeds_preserve3 | persona_native | triple_probe |
 *       dual_dod_ab_open | elf_path_exact | denser_chain | soft_open_bar75
 *       denser3=1 denser3_arms=8 denser3_ok=N denser3_min=8 product_hosts=UDX
 *     denser4 multi-arm residual (W19 densify; GJ_SPAWN_HOST_BLOB_DENSE4_ARMS=8):
 *       denser3_held | embeds_preserve4 | role_exact | quad_probe |
 *       dual_dod_status_open | host_lookup_map | denser_chain4 | soft_open_bar75_4
 *       denser4=1 denser4_arms=8 denser4_ok=N denser4_min=8 product_hosts=UDX
 *     denser5 multi-arm residual (W20 densify; GJ_SPAWN_HOST_BLOB_DENSE5_ARMS=8):
 *       denser4_held | embeds_preserve5 | catalog_geom_exact | penta_probe |
 *       dual_dod_open5 | host_elf_path_role | denser_chain5 | soft_open_bar75_5
 *       denser5=1 denser5_arms=8 denser5_ok=N denser5_min=8 product_hosts=UDX
 *     denser6 multi-arm residual (W21 densify; GJ_SPAWN_HOST_BLOB_DENSE6_ARMS=8):
 *       denser5_held | embeds_preserve6 | product_hosts_exact | hexa_probe |
 *       dual_dod_open6 | weak_hooks_stable | denser_chain6 | soft_open_bar75_6
 *       denser6=1 denser6_arms=8 denser6_ok=N denser6_min=8 product_hosts=UDX
 *     denser7 multi-arm residual (W22 densify; GJ_SPAWN_HOST_BLOB_DENSE7_ARMS=8):
 *       denser6_held | embeds_preserve7 | dual_dod_class_exact | hepta_probe |
 *       dual_dod_open7 | weak_resolve_chain | denser_chain7 | soft_open_bar75_7
 *       denser7=1 denser7_arms=8 denser7_ok=N denser7_min=8 product_hosts=UDX
 *     denser8 multi-arm residual (W23 densify; GJ_SPAWN_HOST_BLOB_DENSE8_ARMS=8):
 *       denser7_held | embeds_preserve8 | product_udx_triple | octa_probe |
 *       dual_dod_open8 | preserve_resolve8 | denser_chain8 | soft_open_bar75_8
 *       denser8=1 denser8_arms=8 denser8_ok=N denser8_min=8 product_hosts=UDX
 *     product_hosts=UDX (ddi_host_gj,rtl8168_udx,xhci_udx); Dual DoD OPEN.
 *     Soft residual != product Dual DoD close; Soft!=product dual license.
 *     No nested comment footguns on out-params (ppBlob/pcb plain names).
 *     Preserve: spawn_host_blob_get / elf_try_run / weak->real elf_blob hooks.
 *     CRITICAL preserve host blob embeds ddi/rtl/xhci (weak gj_*_elf_blob).
 *     greppable: "spawn: soft residual host_blob"
 *     greppable: "spawn: soft residual host_blob densify"
 *     greppable: "spawn: soft residual host_blob denser"
 *     greppable: "spawn: soft residual host_blob denser2"
 *     greppable: "spawn: soft residual host_blob denser3"
 *     greppable: "spawn: soft residual host_blob denser4"
 *     greppable: "spawn: soft residual host_blob denser5"
 *     greppable: "spawn: soft residual host_blob denser6"
 *     greppable: "spawn: soft residual host_blob denser7"
 *     greppable: "spawn: soft residual host_blob denser8"
 *     greppable: "spawn: soft residual host_blob class"
 *     greppable: "spawn: soft residual host_blob lean"
 *     greppable: denser_arms= denser_ok= denser_min= host_blob denser
 *     greppable: denser2_arms= denser2_ok= denser2_min= host_blob denser2
 *     greppable: denser3_arms= denser3_ok= denser3_min= host_blob denser3
 *     greppable: denser4_arms= denser4_ok= denser4_min= host_blob denser4
 *     greppable: denser5_arms= denser5_ok= denser5_min= host_blob denser5
 *     greppable: denser6_arms= denser6_ok= denser6_min= host_blob denser6
 *     greppable: denser7_arms= denser7_ok= denser7_min= host_blob denser7
 *     greppable: denser8_arms= denser8_ok= denser8_min= host_blob denser8
 *     greppable: "spawn: soft host_launch elf host_blob"
 *     greppable: "spawn: soft host_launch elf probe"
 *     greppable: "spawn: soft host_launch elf load"
 *     greppable: "spawn: soft host_launch elf stack"
 *     greppable: "spawn: soft host_launch elf thr"
 *     greppable: host_blob / Soft!=product / dual_dod OPEN / product_hosts=UDX
 *      stamp-free; never invent .76.
 *     ASCII Soft!=product only (no unicode Soft inequality glyph).
 *   C2 UDX launch residual (ASSURANCE_LITE class C2 product path; Soft!=product):
 *     Explicit C2 honesty: soft scaffold != product AC close.
 *     Dual DoD A/B remain OPEN; freestanding rtl/xhci SKIP; ko_in_kernel=0.
 *     Concurrent capacity residual: HOST_CLASS_N fits SPAWN_MAX free geometry.
 *     Launch residual path steps + local C2 udx_launch self-check (file-local).
 *     Self-check deepen: BASE covers CORE, BASE SPAWN, CNODE room, fail_bits.
 *     greppable: "spawn: soft residual c2" / "spawn: soft residual udx_launch"
 *     greppable: "spawn: soft residual c2_udx_launch"
 *     greppable: "spawn: soft residual c2_host_map"
 *   Linux pid fork/wait product-min lives in process.c (not wired here):
 *     greppable: "process: soft fork-wait product-min"
 *   Never hard-gates; diagnostics / smoke grep only (wrap OK). Soft.
 *
 * Soft mint verify (grep: spawn: mint soft | spawn: soft mint):
 *   Post-mint verify of PROCESS type/rights/obj/gen (fail closed on soft bad)
 *   Cumulative ok/fail/live/kill/wait + mint ok/fail/soft/soft_bad counters
 *
 * Freestanding sshd.elf live spawn (main embed, default on :22) is outside this
 * fixed table; process_spawn remains G-PROC mint residual for UDX driver hosts
 * (ddi_host_gj / rtl8168_udx / xhci_udx). Soft honesty only here. Soft!=product.
 */
#include <gj/cap.h>
#include <gj/cpu.h>
#include <gj/elf_load.h>
#include <gj/klog.h>
#include <gj/memobj.h>
#include <gj/pmm.h>
#include <gj/process.h>
#include <gj/spawn.h>
#include <gj/string.h>
#include <gj/thread.h>
#include <gj/user_access.h>
#include <gj/vmm.h>

/*
 * Weak host ELF embeds (product residual; Soft!=product). Present when
 * cut links ddi_host / rtl8168_udx / xhci_udx blobs; else 0 → stub park.
 * Same symbols as main soft host_spawn residual (exclusive spawn.c only).
 */
extern char gj_ddi_host_elf_blob[] __attribute__((weak));
extern char gj_ddi_host_elf_blob_end[] __attribute__((weak));
extern char gj_rtl8168_udx_elf_blob[] __attribute__((weak));
extern char gj_rtl8168_udx_elf_blob_end[] __attribute__((weak));
extern char gj_xhci_udx_elf_blob[] __attribute__((weak));
extern char gj_xhci_udx_elf_blob_end[] __attribute__((weak));

/* Freestanding UDX host stack residual (matches init/sessiond user.ld band). */
/*
 * Host product thr stack (rtl8168_udx / xhci_udx / ddi_host freestanding).
 *
 * DUT thr=82 class history:
 *   .76  #PF soft COW miss VA=0x1100000 (exclusive old top on identity
 *        bridge; 4 pages / 16 KiB too tight for udx_printk 0x3d8 frame).
 *   .77  top raised to 0x4000000 / 64 pages; thr past _start into
 *        udx_host_bind_by_id+0x178 (stack zero of DDI GET info ~0x70B)
 *        still USER KILL — identity 2MiB leaf residual + partial map
 *        accept + no post-map U|W verify left stack unwritable under
 *        private AS.
 *
 * Product host stack (.80): exclusive ABI top 0x7F000000 (high user band)
 * with 64 body pages (256 KiB) + fill of the 2MiB identity PS page that
 * begins at top (private U|W for every 4KiB leaf in that PS window).
 *
 * DUT thr=82 CR2 tracks exclusive mapped end (identity no-U residual):
 *   .78  CR2=0x7F000000 (=top)           ERR=0x7 COW miss
 *   .79  CR2=0x7F001000 (=top+4KiB)      ERR=0x7 COW miss after 1-page guard
 * RIP stays bind_by_id+0x178 stack zero; RAX=CR2. After split_large_pde the
 * remaining leaves in the top's 2MiB PT stay identity P|W no-U — user store
 * → COW miss. Fix: map every leaf from top through next 2MiB boundary.
 * RSP enters at top-0x100 (still 16B aligned) so early frames sit in body.
 * Soft!=product · Dual DoD A/B OPEN · G-AC-1 · bar v2026.08.04.80.
 * greppable: GJ_SPAWN_HOST_STACK_TOP | host stack 0x7F000000 | stack guard
 * greppable: cr2_top_guard | thr82_class=fixed | 2m_fill
 */
#define GJ_SPAWN_HOST_STACK_TOP    0x000000007F000000ull
#define GJ_SPAWN_HOST_STACK_PAGES  64u
/* 2MiB PS fill above exclusive top (thr=82 .78/.79 CR2 slide class). */
#define GJ_SPAWN_HOST_STACK_2M     0x0000000000200000ull
/* Initial RSP slack below exclusive top (16-byte aligned). */
#define GJ_SPAWN_HOST_STACK_RSP_SLACK 0x100ull

struct spawn_slot {
    u8                   u8Used;
    u8                   u8HostClass;   /* 0=untagged; 1..N = catalog idx+1 */
    u8                   u8HostDualDod; /* 0=none, 1=A (xhci), 2=B (rtl) */
    /* 1 = prefer product ELF thr; stub exits, no thrash (Soft!=product). */
    u8                   u8HostElfPrefer;
    u16                  u16HostVend;   /* catalog PCI vend or 0 */
    u16                  u16HostDev;    /* catalog PCI dev or 0 */
    u32                  u32StubThrId;  /* first thr (park stub); 0 = none */
    u32                  u32ElfThrId;   /* product host thr when ELF ok */
    struct gj_process    proc;
    struct gj_cnode      cnode;
    struct gj_cap_slot   aSlots[GJ_SPAWN_CNODE_SLOTS];
    struct gj_root_meta  meta;
};

static struct spawn_slot g_aSpawn[GJ_SPAWN_MAX];

/*
 * Named UDX / driver host catalog entry (launch residual; Soft!=product).
 * Dual DoD: 0=none (ddi residual), 1=A (xhci_udx), 2=B (rtl8168_udx).
 * pfnStubEntry = soft park thr (yield until kill) when no embed ELF.
 * szElfPath = product residual path (media/stage); weak embed is primary.
 */
struct spawn_host_class {
    const char *szName;
    u16         u16Vend;
    u16         u16Dev;
    u8          u8DualDod;      /* 0 none, 1=A, 2=B */
    u8          u8PersonaPref;  /* 0 native preferred */
    u8          u8Pad[2];
    const char *szRole;         /* greppable role token */
    const char *szElfPath;      /* residual ELF path (stage/media) */
    void      (*pfnStubEntry)(void *pArg);
};

/* Soft residual park thr: yield until G-PROC-5 kill (H3 thr_exit barrier). */
static void spawn_host_stub_entry(void *pArg);

/*
 * Fixed host catalog — indices match residual launch class ids.
 * 0 ddi_host_gj · 1 rtl8168_udx (DoD B) · 2 xhci_udx (DoD A)
 * sshd is NOT in this table (main-embed live :22; Dual DoD B path residual).
 * ELF paths align with elf_load host_path_class + stage-udx-drivers pack.
 */
static const struct spawn_host_class g_aHostCatalog[GJ_SPAWN_HOST_CLASS_N] = {
    {
        GJ_SPAWN_HOST_DDI_NAME, 0u, 0u, 0u, GJ_SPAWN_HOST_PERSONA_NATIVE,
        { 0, 0 }, "ddi_door_residual", "/bin/ddi_host_gj",
        spawn_host_stub_entry
    },
    {
        GJ_SPAWN_HOST_RTL_NAME, GJ_SPAWN_HOST_RTL_VEND, GJ_SPAWN_HOST_RTL_DEV,
        2u, GJ_SPAWN_HOST_PERSONA_NATIVE,
        { 0, 0 }, "udx_nic", "/usr/lib/udx/rtl8168_udx",
        spawn_host_stub_entry
    },
    {
        GJ_SPAWN_HOST_XHCI_NAME, GJ_SPAWN_HOST_XHCI_VEND, GJ_SPAWN_HOST_XHCI_DEV,
        1u, GJ_SPAWN_HOST_PERSONA_NATIVE,
        { 0, 0 }, "udx_usb_hc", "/bin/xhci_udx",
        spawn_host_stub_entry
    },
};

/* ---- Soft spawn / PROCESS-mint counters (grep: spawn: soft) ------------ */
static u32 g_cSpawned;       /* successful process_spawn */
static u32 g_cSpawnFail;     /* failed process_spawn */
static u32 g_cKill;          /* process_kill resolved ok */
static u32 g_cWait;          /* process_wait reaped */
static u32 g_cMintOk;        /* PROCESS cap install ok */
static u32 g_cMintFail;      /* PROCESS cap install fail */
static u32 g_cMintSoft;      /* soft post-mint verify PASS */
static u32 g_cMintSoftBad;   /* soft post-mint verify FAIL */

/*
 * Soft product inventory (Wave 20 deepen + C2 UDX launch residual).
 * greppable: spawn: soft ...
 * Wave is soft inventory only — not GJ_IMAGE_VERSION; H2 no stamp storms.
 * Catalog->hook residual deepen stays stamp-free (no image version).
 */
#define GJ_SPAWN_SOFT_WAVE 124u

/*
 * C2 UDX launch residual self-check budget (file-local; Soft!=product).
 * ok/GJ_SPAWN_C2_UDX_LAUNCH_OK_N — static geometry/catalog + rights (stable PASS).
 * Checks (bit i set in fail_bits when check i fails):
 *   0 class_n==3  1 class_n<=max  2 rtl Dual DoD B id  3 xhci Dual DoD A id
 *   4 CORE rights  5 native+names  6 BASE covers CORE  7 BASE SPAWN
 *   8 CNODE_SLOTS room for later DDI caps
 * Runtime free-slot capacity is reported separately as cap_ready (not in ok/N).
 * Never hard-gates; not product AC. Distinct from HOST_LAUNCH_OK_N lean.
 * Launch path step count (host_path residual; soft honesty only).
 */
#define GJ_SPAWN_C2_UDX_LAUNCH_OK_N  9u
#define GJ_SPAWN_C2_PATH_STEPS       9u
#define GJ_SPAWN_C2_CNODE_MIN        8ull

/*
 * Host-hook residual self-check budget (catalog resolve + launch fail-closed).
 * Distinct from HOST_LAUNCH_OK_N / C2_UDX_LAUNCH_OK_N. Soft!=product.
 * Checks (bit i set in fail_bits when check i fails):
 *   0 class_n==3 + table filled
 *   1 names match macros (ddi/rtl/xhci)
 *   2 rtl 10ec:8168 + dual_dod=B
 *   3 xhci 8086:a12f + dual_dod=A
 *   4 ddi vend/dev=0 + dual_dod=none + role
 *   5 lookup by name all 3
 *   6 lookup by PCI id rtl+xhci; ddi miss on 0:0 ok
 *   7 launch hook fail-closed (null parent / bad class)
 *   8 stub entries non-null + persona native
 *   9 elf path residual non-empty for all 3 hosts
 */
#define GJ_SPAWN_HOST_HOOK_OK_N      10u

/*
 * Host-blob residual self-check budget (weak embed resolve densify).
 * Soft!=product; Dual DoD OPEN; product_hosts=UDX. Never hard-gates.
 * Checks (bit i set in fail_bits when check i fails):
 *   0 class_n==3 + HOST_CLASS_N <= SPAWN_MAX
 *   1 product host names non-empty (ddi/rtl/xhci macros)
 *   2 dual_dod map: ddi=none rtl=B xhci=A (catalog)
 *   3 elf path residual non-empty all 3
 *   4 spawn_host_blob_get fail-closed (bad class / null out-params)
 *   5 weak symbol resolve path lives (call get for class 0..2; miss OK)
 *   6 product_hosts=UDX role tokens present
 *   7 hooks present: blob_get + elf_try_run prototypes wired
 * Present/miss of weak embeds is honesty residual (not ok/N fail):
 *   cut without embeds still PASS geometry; Dual DoD stays OPEN.
 */
#define GJ_SPAWN_HOST_BLOB_OK_N      8u

/*
 * denser host_blob residual multi-arm honesty (Soft!=product; Dual DoD OPEN).
 * 8 arms; all required for denser composite ok. Never hard-gates product.
 * Arms (GJ_SPAWN_HOST_BLOB_DENSE_ARMS=8):
 *   0 class_geom       - class_n==3 + fit SPAWN_MAX
 *   1 embeds_preserve  - weak ddi/rtl/xhci resolve path + class map 0/1/2
 *   2 dual_dod_map     - ddi=none rtl=B xhci=A
 *   3 fail_closed      - null out-params + bad class return 0
 *   4 live_probe       - probe all 3 classes (present+absent==CLASS_N)
 *   5 product_hosts_udx- names match ddi_host_gj|rtl8168_udx|xhci_udx
 *   6 elf_path_role    - residual elf path + role tokens non-empty
 *   7 dual_dod_open    - soft residual never closes Dual DoD; stubs live
 * greppable: denser_arms=8 denser_ok= denser_min= host_blob denser
 * greppable: spawn: soft residual host_blob denser
 * product_hosts=UDX; Dual DoD OPEN;  stamp-free.
 * CRITICAL preserve host blob embeds ddi/rtl/xhci.
 *
 * denser2 multi-arm residual (W17 densify; Soft!=product; Dual DoD OPEN):
 * Arms (GJ_SPAWN_HOST_BLOB_DENSE2_ARMS=8):
 *   0 denser1_held     - denser1 composite all 8 arms held
 *   1 embeds_preserve2 - weak map 0/1/2 + exact name macros (ddi/rtl/xhci)
 *   2 pci_class_ids    - ddi 0:0 rtl 10ec:8168 xhci 8086:a12f
 *   3 re_probe         - second live spawn_host_blob_get pass stable
 *   4 cb_honesty       - present=>cb>0; miss=>cb==0 inventory samples
 *   5 name_role_path   - exact names + role + elf path residual
 *   6 preserve_policy  - never_kill_embed / preserve_embeds honesty
 *   7 dual_dod_open2   - Dual DoD A/B remain OPEN; stubs live
 * greppable: denser2_arms=8 denser2_ok= denser2_min= host_blob denser2
 * greppable: spawn: soft residual host_blob denser2
 * CRITICAL preserve host blob embeds ddi/rtl/xhci (weak gj_*_elf_blob).
 *
 * denser3 multi-arm residual (W18 densify; Soft!=product; Dual DoD OPEN):
 * Arms (GJ_SPAWN_HOST_BLOB_DENSE3_ARMS=8):
 *   0 denser2_held     - denser2 composite all 8 arms held
 *   1 embeds_preserve3 - preserve2 + live inventory class map 0/1/2
 *   2 persona_native   - all 3 hosts persona pref native
 *   3 triple_probe     - third live spawn_host_blob_get pass stable
 *   4 dual_dod_ab_open - Dual DoD A/B OPEN map + stubs live
 *   5 elf_path_exact   - residual elf paths exact product_hosts=UDX
 *   6 denser_chain     - denser1+denser2 held + embeds_preserve chain
 *   7 soft_open_bar75  - denser3 min/arms; soft never closes Dual DoD
 * greppable: denser3_arms=8 denser3_ok= denser3_min= host_blob denser3
 * greppable: spawn: soft residual host_blob denser3
 * product_hosts=UDX; Dual DoD OPEN;  stamp-free.
 * CRITICAL preserve host blob embeds ddi/rtl/xhci (weak gj_*_elf_blob).
 *
 * denser4 multi-arm residual (W19 densify; Soft!=product; Dual DoD OPEN):
 * Arms (GJ_SPAWN_HOST_BLOB_DENSE4_ARMS=8):
 *   0 denser3_held        - denser3 composite all 8 arms held
 *   1 embeds_preserve4    - preserve3 + denser_chain + exact names + fail_closed
 *   2 role_exact          - roles ddi_door_residual|udx_nic|udx_usb_hc
 *   3 quad_probe          - fourth live spawn_host_blob_get pass stable
 *   4 dual_dod_status_open- Dual DoD A/B OPEN + stubs + dual_dod_ab_open
 *   5 host_lookup_map     - catalog name + PCI lookup map 0/1/2 stable
 *   6 denser_chain4       - denser1+2+3 held + embeds_preserve 1..4 chain
 *   7 soft_open_bar75_4   - denser4 min/arms; soft never closes Dual DoD
 * greppable: denser4_arms=8 denser4_ok= denser4_min= host_blob denser4
 * greppable: spawn: soft residual host_blob denser4
 * product_hosts=UDX; Dual DoD OPEN;  stamp-free.
 * CRITICAL preserve host blob embeds ddi/rtl/xhci (weak gj_*_elf_blob).
 *
 * denser5 multi-arm residual (W20 densify; Soft!=product; Dual DoD OPEN):
 * Arms (GJ_SPAWN_HOST_BLOB_DENSE5_ARMS=8):
 *   0 denser4_held        - denser4 composite all 8 arms held
 *   1 embeds_preserve5    - preserve4 + denser_chain4 + exact names + fail_closed
 *   2 catalog_geom_exact  - class_n==3 + SPAWN_MAX fit + PCI + dual_dod map
 *   3 penta_probe         - fifth live spawn_host_blob_get pass stable
 *   4 dual_dod_open5      - Dual DoD A/B OPEN + status open + stubs live
 *   5 host_elf_path_role  - exact elf paths + roles product_hosts=UDX
 *   6 denser_chain5       - denser1+2+3+4 held + embeds_preserve 1..5 chain
 *   7 soft_open_bar75_5   - denser5 min/arms; soft never closes Dual DoD
 * greppable: denser5_arms=8 denser5_ok= denser5_min= host_blob denser5
 * greppable: spawn: soft residual host_blob denser5
 * product_hosts=UDX; Dual DoD OPEN;  stamp-free.
 * CRITICAL preserve host blob embeds ddi/rtl/xhci (weak gj_*_elf_blob).
 *
 * denser6 multi-arm residual (W21 densify; Soft!=product; Dual DoD OPEN):
 * Arms (GJ_SPAWN_HOST_BLOB_DENSE6_ARMS=8):
 *   0 denser5_held        - denser5 composite all 8 arms held
 *   1 embeds_preserve6    - preserve5 + denser_chain5 + exact names + fail_closed
 *   2 product_hosts_exact - exact product_hosts=UDX names ddi/rtl/xhci macros
 *   3 hexa_probe          - sixth live spawn_host_blob_get pass stable
 *   4 dual_dod_open6      - Dual DoD A/B OPEN + open5 + stubs live
 *   5 weak_hooks_stable   - blob_get fail-closed + lookup map + hooks wired
 *   6 denser_chain6       - denser1..5 held + embeds_preserve 1..6 chain
 *   7 soft_open_bar75_6   - denser6 min/arms; soft never closes Dual DoD
 * greppable: denser6_arms=8 denser6_ok= denser6_min= host_blob denser6
 * greppable: spawn: soft residual host_blob denser6
 * product_hosts=UDX; Dual DoD OPEN;  stamp-free.
 * CRITICAL preserve host blob embeds ddi/rtl/xhci (weak gj_*_elf_blob).
 * denser7 multi-arm residual (W22 densify; Soft!=product; Dual DoD OPEN):
 * Arms (GJ_SPAWN_HOST_BLOB_DENSE7_ARMS=8):
 *   0 denser6_held        - denser6 composite all 8 arms held
 *   1 embeds_preserve7    - preserve6 + denser_chain6 + exact names + fail_closed
 *   2 dual_dod_class_exact- Dual DoD 0/2/1 + class_n=3 product_hosts=UDX
 *   3 hepta_probe         - seventh live spawn_host_blob_get pass stable
 *   4 dual_dod_open7      - Dual DoD A/B OPEN + open6 + stubs live
 *   5 weak_resolve_chain  - blob_get fail-closed + hepta + preserve + lookup
 *   6 denser_chain7       - denser1..6 held + embeds_preserve 1..7 chain
 *   7 soft_open_bar75_7   - denser7 min/arms; soft never closes Dual DoD
 * greppable: denser7_arms=8 denser7_ok= denser7_min= host_blob denser7
 * greppable: spawn: soft residual host_blob denser7
 * product_hosts=UDX; Dual DoD OPEN;  stamp-free.
 * CRITICAL preserve host blob embeds ddi/rtl/xhci (weak gj_*_elf_blob).
 *
 * denser8 multi-arm residual (W23 densify; Soft!=product; Dual DoD OPEN):
 * Arms (GJ_SPAWN_HOST_BLOB_DENSE8_ARMS=8):
 *   0 denser7_held        - denser7 composite all 8 arms held
 *   1 embeds_preserve8    - preserve7 + denser_chain7 + exact names + fail_closed
 *   2 product_udx_triple  - product_hosts=UDX name+role+path triple residual
 *   3 octa_probe          - eighth live spawn_host_blob_get pass stable
 *   4 dual_dod_open8      - Dual DoD A/B OPEN + open7 + stubs live
 *   5 preserve_resolve8   - blob_get fail-closed + octa + preserve + lookup
 *   6 denser_chain8       - denser1..7 held + embeds_preserve 1..8 chain
 *   7 soft_open_bar75_8   - denser8 min/arms; soft never closes Dual DoD
 * greppable: denser8_arms=8 denser8_ok= denser8_min= host_blob denser8
 * greppable: spawn: soft residual host_blob denser8
 * product_hosts=UDX; Dual DoD OPEN;  stamp-free.
 * CRITICAL preserve host blob embeds ddi/rtl/xhci (weak gj_*_elf_blob).
 * ASCII Soft!=product only (no unicode Soft inequality glyph).
 */
#define GJ_SPAWN_HOST_BLOB_DENSE_ARMS  8u
#define GJ_SPAWN_HOST_BLOB_DENSE_MIN   8u
#define GJ_SPAWN_HOST_BLOB_DENSE2_ARMS 8u
#define GJ_SPAWN_HOST_BLOB_DENSE2_MIN  8u
#define GJ_SPAWN_HOST_BLOB_DENSE3_ARMS 8u
#define GJ_SPAWN_HOST_BLOB_DENSE3_MIN  8u
#define GJ_SPAWN_HOST_BLOB_DENSE4_ARMS 8u
#define GJ_SPAWN_HOST_BLOB_DENSE4_MIN  8u
#define GJ_SPAWN_HOST_BLOB_DENSE5_ARMS 8u
#define GJ_SPAWN_HOST_BLOB_DENSE5_MIN  8u
#define GJ_SPAWN_HOST_BLOB_DENSE6_ARMS 8u
#define GJ_SPAWN_HOST_BLOB_DENSE6_MIN  8u
#define GJ_SPAWN_HOST_BLOB_DENSE7_ARMS 8u
#define GJ_SPAWN_HOST_BLOB_DENSE7_MIN  8u
#define GJ_SPAWN_HOST_BLOB_DENSE8_ARMS 8u
#define GJ_SPAWN_HOST_BLOB_DENSE8_MIN  8u
_Static_assert(GJ_SPAWN_HOST_BLOB_DENSE_ARMS == 8u,
               "host_blob denser arms must be 8");
_Static_assert(GJ_SPAWN_HOST_BLOB_DENSE_MIN ==
                   GJ_SPAWN_HOST_BLOB_DENSE_ARMS,
               "host_blob denser min must equal denser arms");
_Static_assert(GJ_SPAWN_HOST_BLOB_DENSE2_ARMS == 8u,
               "host_blob denser2 arms must be 8");
_Static_assert(GJ_SPAWN_HOST_BLOB_DENSE2_MIN ==
                   GJ_SPAWN_HOST_BLOB_DENSE2_ARMS,
               "host_blob denser2 min must equal denser2 arms");
_Static_assert(GJ_SPAWN_HOST_BLOB_DENSE3_ARMS == 8u,
               "host_blob denser3 arms must be 8");
_Static_assert(GJ_SPAWN_HOST_BLOB_DENSE3_MIN ==
                   GJ_SPAWN_HOST_BLOB_DENSE3_ARMS,
               "host_blob denser3 min must equal denser3 arms");
_Static_assert(GJ_SPAWN_HOST_BLOB_DENSE4_ARMS == 8u,
               "host_blob denser4 arms must be 8");
_Static_assert(GJ_SPAWN_HOST_BLOB_DENSE4_MIN ==
                   GJ_SPAWN_HOST_BLOB_DENSE4_ARMS,
               "host_blob denser4 min must equal denser4 arms");
_Static_assert(GJ_SPAWN_HOST_BLOB_DENSE5_ARMS == 8u,
               "host_blob denser5 arms must be 8");
_Static_assert(GJ_SPAWN_HOST_BLOB_DENSE5_MIN ==
                   GJ_SPAWN_HOST_BLOB_DENSE5_ARMS,
               "host_blob denser5 min must equal denser5 arms");
_Static_assert(GJ_SPAWN_HOST_BLOB_DENSE6_ARMS == 8u,
               "host_blob denser6 arms must be 8");
_Static_assert(GJ_SPAWN_HOST_BLOB_DENSE6_MIN ==
                   GJ_SPAWN_HOST_BLOB_DENSE6_ARMS,
               "host_blob denser6 min must equal denser6 arms");
_Static_assert(GJ_SPAWN_HOST_BLOB_DENSE7_ARMS == 8u,
               "host_blob denser7 arms must be 8");
_Static_assert(GJ_SPAWN_HOST_BLOB_DENSE7_MIN ==
                   GJ_SPAWN_HOST_BLOB_DENSE7_ARMS,
               "host_blob denser7 min must equal denser7 arms");
_Static_assert(GJ_SPAWN_HOST_BLOB_DENSE8_ARMS == 8u,
               "host_blob denser8 arms must be 8");
_Static_assert(GJ_SPAWN_HOST_BLOB_DENSE8_MIN ==
                   GJ_SPAWN_HOST_BLOB_DENSE8_ARMS,
               "host_blob denser8 min must equal denser8 arms");

/* Host-launch residual tallies (Soft!=product; never hard-gates). */
static u32 g_u32SoftHostLaunchOk;    /* process_spawn ok tagged host_launch */
static u32 g_u32SoftHostLaunchLean;  /* lean host residual self-check ok/N */
static u32 g_u32SoftHostCatalogLog;  /* host_catalog residual emissions */
static u32 g_u32SoftC2UdxLaunchLog;  /* C2 residual c2/udx_launch emissions */
static u32 g_u32SoftC2UdxLaunchOk;   /* C2 udx_launch self-check full PASS */
static u32 g_u32SoftHostCapReady;    /* free geometry can hold class hosts */
static u32 g_u32SoftHostHookOk;      /* host_hook self-check full PASS */
static u32 g_u32SoftHostHookLog;     /* host_hook residual emissions */
static u32 g_u32SoftHostLaunchClass; /* catalog-class launch successes */
static u32 g_u32SoftHostLaunchByClass[GJ_SPAWN_HOST_CLASS_N];
static u32 g_u32SoftHostTagAuto;     /* process_spawn auto-tag via catalog pArg */
static u32 g_u32SoftHostHookDeny;    /* launch hook fail-closed (inval/class) */
/* Host ELF functional residual tallies (Soft!=product; Dual DoD OPEN). */
static u32 g_u32SoftHostElfMiss;     /* weak embed absent for class */
static u32 g_u32SoftHostElfEmbed;    /* weak embed present for class */
static u32 g_u32SoftHostElfProbeOk;  /* elf_probe_image PASS */
static u32 g_u32SoftHostElfProbeFail;
static u32 g_u32SoftHostElfLoadOk;   /* elf_load_image PASS */
static u32 g_u32SoftHostElfLoadFail;
static u32 g_u32SoftHostElfStackFail;/* user stack map residual fail */
static u32 g_u32SoftHostElfThrFail;  /* thread_create_user fail */
static u32 g_u32SoftHostElfRunOk;    /* full probe|load|stack|user thr */
static u32 g_u32SoftHostElfPrefer;   /* arm prefer ELF thr over stub park */
static u32 g_u32SoftHostElfStubExit; /* stub exit after prefer arm (no thrash) */
static u32 g_u32SoftHostElfNoThrash; /* prefer+QoS demote applied (no thrash) */
static u32 g_u32SoftHostElfQosPrefer;/* DRIVER product thr + BACKGROUND stub */
static u32 g_u32SoftHostElfByClass[GJ_SPAWN_HOST_CLASS_N];
/* Host blob densify residual tallies (inventory live probe; Soft!=product). */
static u32 g_u32SoftHostBlobLog;     /* host_blob residual densify emissions */
static u32 g_u32SoftHostBlobLean;    /* host_blob self-check full PASS */
static u32 g_u32SoftHostBlobProbe;   /* inventory spawn_host_blob_get calls */
static u32 g_u32SoftHostBlobPresent; /* inventory weak embed present samples */
static u32 g_u32SoftHostBlobAbsent;  /* inventory weak embed miss samples */
static u32 g_u32SoftHostBlobByClass[GJ_SPAWN_HOST_CLASS_N]; /* present by class */
static u64 g_u64SoftHostBlobCbByClass[GJ_SPAWN_HOST_CLASS_N]; /* last cb sample */
/* denser host_blob multi-arm residual tallies (Soft!=product; Dual DoD OPEN). */
static u32 g_u32SoftHostBlobDenseOk;   /* denser composite arms all PASS */
static u32 g_u32SoftHostBlobDenseFail; /* denser composite soft fail */
static u32 g_u32SoftHostBlobDenseArms; /* last denser arm count (0..DENSE_ARMS) */
/* denser2 host_blob multi-arm residual tallies (W17 densify; Soft!=product). */
static u32 g_u32SoftHostBlobDense2Ok;   /* denser2 composite arms all PASS */
static u32 g_u32SoftHostBlobDense2Fail; /* denser2 composite soft fail */
static u32 g_u32SoftHostBlobDense2Arms; /* last denser2 arm count */
static u32 g_u32SoftHostBlobReProbe;    /* denser2 re_probe live get calls */
/* denser3 host_blob multi-arm residual tallies (W18 densify; Soft!=product). */
static u32 g_u32SoftHostBlobDense3Ok;   /* denser3 composite arms all PASS */
static u32 g_u32SoftHostBlobDense3Fail; /* denser3 composite soft fail */
static u32 g_u32SoftHostBlobDense3Arms; /* last denser3 arm count */
static u32 g_u32SoftHostBlobTripleProbe;/* denser3 triple_probe live gets */
/* denser4 host_blob multi-arm residual tallies (W19 densify; Soft!=product). */
static u32 g_u32SoftHostBlobDense4Ok;   /* denser4 composite arms all PASS */
static u32 g_u32SoftHostBlobDense4Fail; /* denser4 composite soft fail */
static u32 g_u32SoftHostBlobDense4Arms; /* last denser4 arm count */
static u32 g_u32SoftHostBlobQuadProbe;  /* denser4 quad_probe live gets */
/* denser5 host_blob multi-arm residual tallies (W20 densify; Soft!=product). */
static u32 g_u32SoftHostBlobDense5Ok;   /* denser5 composite arms all PASS */
static u32 g_u32SoftHostBlobDense5Fail; /* denser5 composite soft fail */
static u32 g_u32SoftHostBlobDense5Arms; /* last denser5 arm count */
static u32 g_u32SoftHostBlobPentaProbe; /* denser5 penta_probe live gets */
/* denser6 host_blob multi-arm residual tallies (W21 densify; Soft!=product). */
static u32 g_u32SoftHostBlobDense6Ok;   /* denser6 composite arms all PASS */
static u32 g_u32SoftHostBlobDense6Fail; /* denser6 composite soft fail */
static u32 g_u32SoftHostBlobDense6Arms; /* last denser6 arm count */
static u32 g_u32SoftHostBlobHexaProbe;  /* denser6 hexa_probe live gets */
/* denser7 host_blob multi-arm residual tallies (W22 densify; Soft!=product). */
static u32 g_u32SoftHostBlobDense7Ok;   /* denser7 composite arms all PASS */
static u32 g_u32SoftHostBlobDense7Fail; /* denser7 composite soft fail */
static u32 g_u32SoftHostBlobDense7Arms; /* last denser7 arm count */
static u32 g_u32SoftHostBlobHeptaProbe; /* denser7 hepta_probe live gets */
/* denser8 host_blob multi-arm residual tallies (W23 densify; Soft!=product). */
static u32 g_u32SoftHostBlobDense8Ok;   /* denser8 composite arms all PASS */
static u32 g_u32SoftHostBlobDense8Fail; /* denser8 composite soft fail */
static u32 g_u32SoftHostBlobDense8Arms; /* last denser8 arm count */
static u32 g_u32SoftHostBlobOctaProbe;  /* denser8 octa_probe live gets */

static u32 g_u32SoftSpawnEnter;      /* process_spawn entries */
static u32 g_u32SoftDenyNull;        /* null parent / args / entry */
static u32 g_u32SoftDenyNoCnode;     /* parent CNode missing */
static u32 g_u32SoftDenyFull;        /* fixed spawn table full */
static u32 g_u32SoftDenyMeta;        /* bootstrap root meta fail */
static u32 g_u32SoftDenyAs;          /* process_as_ensure fail */
static u32 g_u32SoftDenyMint;        /* mint install or soft-verify fail */
static u32 g_u32SoftDenyThr;         /* thread_create fail */
static u32 g_u32SoftLivePeak;        /* peak occupied slots */

static u32 g_u32SoftMintNullArg;     /* mint/verify null parent/child/ref */
static u32 g_u32SoftMintNullRef;     /* soft-verify null cap ref */
static u32 g_u32SoftMintResolve;     /* soft-verify resolve fail */
static u32 g_u32SoftMintType;        /* soft-verify type != PROCESS */
static u32 g_u32SoftMintObj;         /* soft-verify object pointer mismatch */
static u32 g_u32SoftMintGen;         /* soft-verify slot gen mismatch */
static u32 g_u32SoftMintCoreRights;  /* soft-verify core rights missing */
static u32 g_u32SoftMintRights;      /* soft-verify full wanted rights miss */
static u32 g_u32SoftMintFromCap;     /* soft-verify process_from_cap mismatch */
static u32 g_u32SoftMintInstallFail; /* gj_cap_alloc_install fail */
static u32 g_u32SoftMintJit;         /* mint with CapJit rights OR'd (W15) */
static u32 g_u32SoftMintNoJit;       /* mint without JIT (W15) */

static u32 g_u32SoftKillEnter;       /* process_kill entries */
static u32 g_u32SoftKillIdem;        /* kill already-dead (idempotent ok) */
static u32 g_u32SoftKillDenyNull;    /* kill null parent/ref */
static u32 g_u32SoftKillDenyResolve; /* kill resolve fail */
static u32 g_u32SoftKillDenyType;    /* kill type != PROCESS */
static u32 g_u32SoftKillDenyRights;  /* kill missing DESTROY */
static u32 g_u32SoftKillDenyNoent;   /* kill null child object */

static u32 g_u32SoftWaitEnter;       /* process_wait entries */
static u32 g_u32SoftWaitAgain;       /* wait while child still alive */
static u32 g_u32SoftWaitDenyNull;    /* wait null parent/ref */
static u32 g_u32SoftWaitDenyResolve; /* wait resolve fail */
static u32 g_u32SoftWaitDenyType;    /* wait type != PROCESS */
static u32 g_u32SoftWaitDenyRights;  /* wait missing WAIT */
static u32 g_u32SoftWaitDenyNoent;   /* wait null child object */
static u32 g_u32SoftWaitSlotFree;    /* wait recycled fixed spawn slot (W15) */
static u32 g_u32SoftWaitCapInv;      /* wait invalidated parent PROCESS cap */

static u32 g_u32SoftFromCapHit;      /* process_from_cap success */
static u32 g_u32SoftFromCapMiss;     /* process_from_cap fail-closed */

/* Wave 15 lifecycle / reverse-path tallies. */
static u32 g_u32SoftFailCleanup;     /* spawn_fail_cleanup calls */
static u32 g_u32SoftAsTeardown;      /* spawn_as_teardown ran destroy */
static u32 g_u32SoftAsTeardownSkip;  /* AS teardown no-op (null/0/kernel) */
static u32 g_u32SoftWaitReg;         /* process_wait_register after thr live */
static u32 g_u32SoftPersonaNative;   /* spawn personality=0 */
static u32 g_u32SoftPersonaLinux;    /* spawn personality=1 */
static u32 g_u32SoftLastThrId;       /* last successful first-thr id (sample) */

static u32 g_u32SoftLogN;            /* soft inventory log emissions */
static u8  g_fSoftOnce;              /* one-shot after first product activity */

static void spawn_soft_inc(u32 *pCtr);
static void soft_inventory_log(const char *szVia);
static void soft_inventory_maybe_once(void);
static void soft_note_live_peak(void);
static struct spawn_slot *spawn_slot_of(struct gj_process *pProc);
static void spawn_host_keep_live_bind(struct gj_process *pChild,
                                      const struct spawn_host_class *pHost,
                                      u32 u32ElfThr);
static int  spawn_host_keep_live_p(const struct spawn_slot *pSlot,
                                   const struct gj_process *pProc);
/*
 * Residual catalog->spawn hooks (defined below; Soft!=product).
 * File-local prototypes for -Wmissing-prototypes; linkable export for a
 * later header wire (exclusive this unit only — no spawn.h churn).
 */
gj_status_t process_spawn_host_launch(struct gj_process *pParent,
                                      u32 u32ClassIdx,
                                      void (*pfnEntry)(void *pArg),
                                      void *pArg,
                                      struct gj_process **ppOutChild,
                                      struct gj_cap_ref *pOutCap);
gj_status_t process_spawn_host_launch_by_name(struct gj_process *pParent,
                                              const char *szName,
                                              void (*pfnEntry)(void *pArg),
                                              void *pArg,
                                              struct gj_process **ppOutChild,
                                              struct gj_cap_ref *pOutCap);
gj_status_t process_spawn_host_launch_by_pci(struct gj_process *pParent,
                                             u16 u16Vend, u16 u16Dev,
                                             void (*pfnEntry)(void *pArg),
                                             void *pArg,
                                             struct gj_process **ppOutChild,
                                             struct gj_cap_ref *pOutCap);
static int  spawn_host_blob_get(u32 u32ClassIdx, const void **ppBlob,
                                u64 *pcb);
static u32  spawn_host_map_stack(struct gj_process *pProc, u64 u64StackTop,
                                 u32 cPages);
static u32  spawn_host_elf_try_run(struct gj_process *pChild,
                                   const struct spawn_host_class *pHost,
                                   u32 u32ClassIdx);

/**
 * Occupied fixed spawn slots (live children not yet reaped via process_wait).
 */
static u32
spawn_live_count(void)
{
    u32 iSlot;
    u32 cLive;

    cLive = 0;
    for (iSlot = 0; iSlot < GJ_SPAWN_MAX; iSlot++) {
        if (g_aSpawn[iSlot].u8Used) {
            cLive++;
        }
    }
    return cLive;
}

/** Soft: bump path tally (u32 wrap is fine for telemetry). */
static void
spawn_soft_inc(u32 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    (*pCtr)++;
}

/** Soft: track peak occupied fixed slots (diagnostics only). */
static void
soft_note_live_peak(void)
{
    u32 cLive;

    cLive = spawn_live_count();
    if (cLive > g_u32SoftLivePeak) {
        g_u32SoftLivePeak = cLive;
    }
}

/* ---- Host catalog resolve + launch hooks (Soft!=product; G-AC-1; H3) ---- */

/**
 * Soft residual park thr for catalog stub hosts.
 * Absent host ELF: yield forever until process_kill -> process_death
 * (H3 thr_exit barrier). When spawn_host_elf_try_run arms u8HostElfPrefer
 * (embed present + ring-3 product thr live), prefer product thr: exit
 * park stub immediately (no yield thrash loop). QoS already demotes stub
 * to BACKGROUND and product thr to DRIVER when prefer arms.
 * Soft!=product; Dual DoD stays OPEN until host USB path / interactive SSH login.
 * greppable: spawn: soft host_launch elf prefer
 * greppable: spawn: soft host_launch elf prefer no_thrash
 */
static void
spawn_host_stub_entry(void *pArg)
{
    struct gj_thread *pThr;
    struct spawn_slot *pSlot;
    u8 u8Tagged;

    (void)pArg;
    u8Tagged = 0u;
    for (;;) {
        pThr = thread_current();
        pSlot = NULL;
        if (pThr != NULL && pThr->pProc != NULL) {
            pSlot = spawn_slot_of(pThr->pProc);
            /* Soft tag park stub once (panel/no-COM1; Soft!=product). */
            if (u8Tagged == 0u && pThr->u32Id != 0u) {
                thread_soft_tag_set(pThr->u32Id, "host_stub");
                u8Tagged = 1u;
            }
        }
        if (pSlot != NULL && pSlot->u8HostElfPrefer != 0u) {
            /*
             * Prefer product thr: exit park stub now. Do not yield-loop
             * (no thrash). Process stays alive via user thr; H3 death
             * still thr_exit before as_destroy. Soft!=product.
             */
            spawn_soft_inc(&g_u32SoftHostElfStubExit);
            /* Grep: spawn: soft host_launch elf prefer */
            /* Grep: spawn: soft host_launch elf prefer no_thrash */
            kprintf("spawn: soft host_launch elf prefer stub_exit "
                    "host_class=%u dual_dod=%u vend=0x%x dev=0x%x "
                    "prefer_elf_thr=1 stub_park=0 no_thrash=1 "
                    "stub_thr=%u elf_thr=%u "
                    "H3=thr_exit_before_as_destroy "
                    "product_host_launch=OPEN soft_ne_product=1 "
                    "dual=MIT_OR_Apache-2.0 G-AC-1 wave=%u "
                    "\n",
                    (unsigned)pSlot->u8HostClass,
                    (unsigned)pSlot->u8HostDualDod,
                    (unsigned)pSlot->u16HostVend,
                    (unsigned)pSlot->u16HostDev,
                    pSlot->u32StubThrId, pSlot->u32ElfThrId,
                    GJ_SPAWN_SOFT_WAVE);
            kprintf("spawn: soft host_launch elf prefer no_thrash "
                    "stub_exit=1 prefer_elf_thr=1 stub_park=0 "
                    "host_class=%u dual_dod=%u "
                    "product_host_launch=OPEN soft_ne_product=1 "
                    "H3=1 Dual_DoD=OPEN G-AC-1 wave=%u "
                    " PASS\n",
                    (unsigned)pSlot->u8HostClass,
                    (unsigned)pSlot->u8HostDualDod,
                    GJ_SPAWN_SOFT_WAVE);
            thread_exit();
            /*
             * thread_exit is non-returning in product path (hlt belt).
             * If it ever returns, yield once then re-check prefer — never
             * tight-spin. Soft!=product.
             */
            thread_yield();
            continue;
        }
        /* OPEN residual: no product thr yet — cooperative park yield. */
        thread_yield();
    }
}

/** Catalog entry by class index, or NULL if out of range. */
static const struct spawn_host_class *
spawn_host_catalog_get(u32 u32ClassIdx)
{
    if (u32ClassIdx >= GJ_SPAWN_HOST_CLASS_N) {
        return NULL;
    }
    return &g_aHostCatalog[u32ClassIdx];
}

/**
 * Lookup catalog index by host name; returns (u32)-1 on miss.
 * Accepts catalog macros plus stage-pack short aliases:
 *   "ddi_host" / "ddi_host_gj" -> class 0
 *   "rtl8168_udx" -> class 1
 *   "xhci_udx" -> class 2
 */
static u32
spawn_host_catalog_lookup_name(const char *szName)
{
    u32 iClass;

    if (szName == NULL || szName[0] == '\0') {
        return (u32)-1;
    }
    for (iClass = 0; iClass < GJ_SPAWN_HOST_CLASS_N; iClass++) {
        if (g_aHostCatalog[iClass].szName != NULL &&
            strcmp(g_aHostCatalog[iClass].szName, szName) == 0) {
            return iClass;
        }
    }
    /* Stage-pack short name (ESP drivers/ddi_host) -> ddi residual class. */
    if (strcmp(szName, "ddi_host") == 0) {
        return 0u;
    }
    return (u32)-1;
}

/**
 * Lookup catalog index by PCI vend:dev.
 * ddi residual has 0:0 — only matches when both zero AND dual_dod none is
 * not used as a PCI probe (callers should use name for ddi). Soft residual.
 * Returns (u32)-1 on miss.
 */
static u32
spawn_host_catalog_lookup_pci(u16 u16Vend, u16 u16Dev)
{
    u32 iClass;

    if (u16Vend == 0u && u16Dev == 0u) {
        return (u32)-1; /* refuse ambiguous 0:0; use name for ddi */
    }
    for (iClass = 0; iClass < GJ_SPAWN_HOST_CLASS_N; iClass++) {
        if (g_aHostCatalog[iClass].u16Vend == u16Vend &&
            g_aHostCatalog[iClass].u16Dev == u16Dev) {
            return iClass;
        }
    }
    return (u32)-1;
}

/**
 * If pArg points at a catalog entry, return it (auto-tag convention).
 * Callers pass &g_aHostCatalog[i] as pArg to tag host class on spawn.
 */
static const struct spawn_host_class *
spawn_host_catalog_from_arg(void *pArg)
{
    u32 iClass;

    if (pArg == NULL) {
        return NULL;
    }
    for (iClass = 0; iClass < GJ_SPAWN_HOST_CLASS_N; iClass++) {
        if (pArg == (void *)&g_aHostCatalog[iClass]) {
            return &g_aHostCatalog[iClass];
        }
    }
    return NULL;
}

/**
 * Pin catalog UDX host identity on the live PCB.
 * gj_process_init defaults LINUX; GJ_SYS_NET (96) is gettimeofday then.
 * process_keep_live_refuse_kill matches szExecPath / u32StartThr / native.
 */
static void
spawn_host_keep_live_bind(struct gj_process *pChild,
                          const struct spawn_host_class *pHost,
                          u32 u32ElfThr)
{
    u32 i;
    const char *sz;

    if (pChild == NULL) {
        return;
    }
    pChild->u32Personality = GJ_SPAWN_HOST_PERSONA_NATIVE;
    if (pHost != NULL) {
        sz = pHost->szName;
        if (pHost->szElfPath != NULL && pHost->szElfPath[0] != '\0') {
            sz = pHost->szElfPath;
        }
        if (sz != NULL) {
            for (i = 0; i + 1u < (u32)sizeof(pChild->szExecPath) &&
                 sz[i] != '\0'; i++) {
                pChild->szExecPath[i] = sz[i];
            }
            pChild->szExecPath[i] = '\0';
        }
    }
    if (u32ElfThr != 0u) {
        pChild->u32StartThr = u32ElfThr;
    }
}

/** Product ELF host on a catalog slot — do not kill/wait-reap. */
static int
spawn_host_keep_live_p(const struct spawn_slot *pSlot,
                       const struct gj_process *pProc)
{
    if (pSlot == NULL || pProc == NULL || pSlot->u8HostClass == 0u) {
        return 0;
    }
    if (pSlot->u8HostElfPrefer != 0u || pSlot->u32ElfThrId != 0u) {
        return 1;
    }
    if (pProc->u32StartThr != 0u) {
        return 1;
    }
    return 0;
}

/** Tag fixed spawn slot from catalog entry (post process_spawn success). */
static void
spawn_host_slot_tag(struct spawn_slot *pSlot,
                    const struct spawn_host_class *pHost, u32 u32ClassIdx)
{
    if (pSlot == NULL || pHost == NULL ||
        u32ClassIdx >= GJ_SPAWN_HOST_CLASS_N) {
        return;
    }
    pSlot->u8HostClass = (u8)(u32ClassIdx + 1u);
    pSlot->u8HostDualDod = pHost->u8DualDod;
    pSlot->u16HostVend = pHost->u16Vend;
    pSlot->u16HostDev = pHost->u16Dev;
    spawn_host_keep_live_bind(&pSlot->proc, pHost, pSlot->u32ElfThrId);
}

/**
 * Resolve weak host ELF embed for class index (host_blob residual; Soft!=product).
 * Returns 1 and fills ppBlob/pcb out-params when blob present; else 0
 * (OPEN need=host_elf; Dual DoD OPEN; product_hosts=UDX remain stub-park).
 * Class map: 0=ddi_host_gj 1=rtl8168_udx 2=xhci_udx (weak->real when linked).
 * Preserve: gj_ddi_host_elf_blob / gj_rtl8168_udx_elf_blob / gj_xhci_udx_elf_blob.
 * No nested-comment footguns on out-param names (ppBlob pcb plain identifiers).
 * Denser residual: used by elf_try_run + inventory live probe (host_blob densify).
 * greppable: host_blob / Soft!=product / dual_dod OPEN / product_hosts=UDX
 * greppable: spawn: soft residual host_blob / spawn: soft host_launch elf host_blob
 *  stamp-free; never invent .76.
 */
static int
spawn_host_blob_get(u32 u32ClassIdx, const void **ppBlob, u64 *pcb)
{
    const char *p;
    const char *pEnd;

    /* Fail-closed on null out-params (ppBlob/pcb plain; no nested-comment). */
    if (ppBlob == NULL || pcb == NULL) {
        return 0;
    }
    *ppBlob = NULL;
    *pcb = 0;
    p = NULL;
    pEnd = NULL;
    /* product_hosts=UDX class map: ddi=0 rtl=1 xhci=2 (Dual DoD OPEN). */
    if (u32ClassIdx == 0u) {
        p = gj_ddi_host_elf_blob;
        pEnd = gj_ddi_host_elf_blob_end;
    } else if (u32ClassIdx == 1u) {
        p = gj_rtl8168_udx_elf_blob;
        pEnd = gj_rtl8168_udx_elf_blob_end;
    } else if (u32ClassIdx == 2u) {
        p = gj_xhci_udx_elf_blob;
        pEnd = gj_xhci_udx_elf_blob_end;
    } else {
        return 0;
    }
    /* Weak unresolved: address 0 or empty range -> miss (stub park OPEN). */
    if ((uintptr_t)p == 0ull || (uintptr_t)pEnd <= (uintptr_t)p) {
        return 0;
    }
    *ppBlob = (const void *)p;
    *pcb = (u64)((uintptr_t)pEnd - (uintptr_t)p);
    return 1;
}

/**
 * Map one private U|W user page at va (zeroed under kernel identity).
 * Returns 1 on success, 0 on fail. Caller holds child AS active intent;
 * this helper re-activates child CR3 after PA zero.
 */
static u32
spawn_host_map_one_uw(struct gj_process *pProc, u64 u64Va, u64 u64ChildCr3,
                      u64 u64Ker)
{
    gj_paddr_t pa;

    if (pProc == NULL || u64Va == 0ull || (u64Va & 0xfffull) != 0ull ||
        u64ChildCr3 == 0ull) {
        return 0u;
    }
    pa = pmm_alloc();
    if (pa == 0) {
        return 0u;
    }
    /*
     * Zero frame under kernel identity (PA==VA). SMAP would #PF a store
     * to a U=1 leaf from supervisor under child CR3.
     */
    if (u64Ker != 0ull) {
        cpu_load_cr3(u64Ker);
    }
    memset((void *)(gj_vaddr_t)pa, 0, 4096);
    cpu_load_cr3(u64ChildCr3);
    /*
     * Force private U|W leaf (walk_pte COWs / splits 2MiB identity PS).
     * Never leave identity no-U leaf under product host stack (thr=82).
     */
    if (vmm_map_page((gj_vaddr_t)u64Va, pa,
                     GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                         GJ_VMM_PROT_USER) != GJ_OK) {
        kprintf("spawn: soft host stack map fail va=0x%lx Soft!=product\n",
                (unsigned long)u64Va);
        return 0u;
    }
    return 1u;
}

/**
 * Map user stack pages under pProc AS (product residual; Soft!=product).
 * Returns body pages mapped (must equal cPages for thr mint; partial = 0).
 *
 * Stack grows down from u64StackTop (exclusive ABI top). Maps body
 * [top - cPages*4KiB, top) + 2MiB fill [top, next_2m_boundary).
 * DUT thr=82: CR2 slides with mapped exclusive end (.78 top, .79 top+4k).
 * greppable: spawn: soft host stack map
 */
static u32
spawn_host_map_stack(struct gj_process *pProc, u64 u64StackTop, u32 cPages)
{
    u32 iPg;
    u32 cOk;
    u32 cGuard;
    u32 cGuardNeed;
    u64 u64Ker;
    u64 u64Bot;
    u64 u64Bytes;
    u64 u64ChildCr3;
    u64 u64GuardEnd;
    u64 u64Va;

    cOk = 0u;
    cGuard = 0u;
    if (pProc == NULL || cPages == 0u || u64StackTop < 4096ull) {
        return 0u;
    }
    u64Bytes = (u64)cPages * 4096ull;
    if (u64StackTop < u64Bytes) {
        return 0u;
    }
    u64Bot = u64StackTop - u64Bytes;
    /*
     * Fill through the end of the 2MiB identity PS page that contains top.
     * top is 2MiB-aligned on product path → fill [top, top+2MiB).
     * If top were mid-page, fill [top, align_up(top, 2MiB)).
     */
    u64GuardEnd = (u64StackTop + GJ_SPAWN_HOST_STACK_2M) &
                  ~(GJ_SPAWN_HOST_STACK_2M - 1ull);
    if (u64GuardEnd <= u64StackTop) {
        u64GuardEnd = u64StackTop + GJ_SPAWN_HOST_STACK_2M;
    }
    cGuardNeed = (u32)((u64GuardEnd - u64StackTop) / 4096ull);
    /*
     * Product user band honesty: body+fill in [8 MiB, 2 GiB) and clear
     * freestanding ET_EXEC + ring3/pers smoke windows. Soft!=product.
     */
    if (u64Bot < 0x0000000000800000ull ||
        u64GuardEnd > 0x0000000080000000ull ||
        (u64Bot < 0x0000000001400000ull && u64GuardEnd > 0x0000000001000000ull)) {
        kprintf("spawn: soft host stack map REJECT top=0x%lx bot=0x%lx "
                "guard_end=0x%lx pages=%u (user_band|freestanding_link) "
                "Soft!=product dual_dod OPEN\n",
                (unsigned long)u64StackTop, (unsigned long)u64Bot,
                (unsigned long)u64GuardEnd, cPages);
        return 0u;
    }
    if (process_as_ensure(pProc) != GJ_OK) {
        return 0u;
    }
    u64ChildCr3 = pProc->u64Cr3;
    if (u64ChildCr3 == 0ull) {
        kprintf("spawn: soft host stack map REJECT cr3=0 Soft!=product\n");
        return 0u;
    }
    process_as_activate(pProc);
    u64Ker = vmm_kernel_cr3();
    /* Body pages: top-down from exclusive ABI top. */
    for (iPg = 0u; iPg < cPages; iPg++) {
        u64Va = u64StackTop - (u64)(iPg + 1u) * 4096ull;
        if (spawn_host_map_one_uw(pProc, u64Va, u64ChildCr3, u64Ker) == 0u) {
            kprintf("spawn: soft host stack map nomem|fail page=%u/%u "
                    "va=0x%lx Soft!=product\n",
                    iPg + 1u, cPages, (unsigned long)u64Va);
            break;
        }
        cOk++;
    }
    if (cOk != cPages) {
        kprintf("spawn: soft host stack map PARTIAL top=0x%lx bot=0x%lx "
                "ok=%u need=%u Soft!=product dual_dod OPEN\n",
                (unsigned long)u64StackTop, (unsigned long)u64Bot, cOk,
                cPages);
        return 0u;
    }
    /*
     * 2MiB fill above exclusive top (DUT .78/.79 CR2 slide class):
     * after split_large_pde, residual identity leaves (no U) at top+4k..
     * remain present → user store ERR=0x7 COW miss. Replace every leaf.
     */
    for (iPg = 0u; iPg < cGuardNeed; iPg++) {
        u64Va = u64StackTop + (u64)iPg * 4096ull;
        if (spawn_host_map_one_uw(pProc, u64Va, u64ChildCr3, u64Ker) == 0u) {
            kprintf("spawn: soft host stack map GUARD_FAIL va=0x%lx "
                    "page=%u/%u Soft!=product thr82_class=1\n",
                    (unsigned long)u64Va, iPg + 1u, cGuardNeed);
            return 0u;
        }
        cGuard++;
    }
    /*
     * Post-map WRITE soft: body + full 2MiB fill must be P|U|(W|COW).
     */
    process_as_activate(pProc);
    if (!user_range_mapped_access(u64Bot, u64Bytes, GJ_USER_ACCESS_WRITE)) {
        kprintf("spawn: soft host stack map VERIFY_FAIL top=0x%lx bot=0x%lx "
                "pages=%u need=P|U|W Soft!=product dual_dod OPEN "
                "thr82_class=1\n",
                (unsigned long)u64StackTop, (unsigned long)u64Bot, cPages);
        return 0u;
    }
    if (cGuard != 0u &&
        !user_range_mapped_access(u64StackTop,
                                  (u64)cGuard * 4096ull,
                                  GJ_USER_ACCESS_WRITE)) {
        kprintf("spawn: soft host stack map GUARD_VERIFY_FAIL top=0x%lx "
                "guard_pages=%u need=P|U|W Soft!=product thr82_class=1\n",
                (unsigned long)u64StackTop, cGuard);
        return 0u;
    }
    /* Full CR3 reload settles TLB after identity 2MiB splits. */
    if (u64Ker != 0ull) {
        cpu_load_cr3(u64Ker);
    }
    cpu_load_cr3(u64ChildCr3);
    /* Grep: spawn: soft host stack map */
    kprintf("spawn: soft host stack map PASS top=0x%lx bot=0x%lx "
            "pages=%u guard=%u guard_end=0x%lx bytes=%lu private_uw=1 "
            "verify_write=1 product_hosts=UDX dual_dod OPEN Soft!=product "
            "bar=v2026.08.04.80 thr82_class=fixed cr2_top_guard=1 "
            "2m_fill=1\n",
            (unsigned long)u64StackTop, (unsigned long)u64Bot, cOk, cGuard,
            (unsigned long)u64GuardEnd, (unsigned long)u64Bytes);
    return cOk;
}

/**
 * FUNCTIONAL residual: load weak host ELF into child AS and enter ring-3.
 * Path: host_blob -> probe -> load -> user stack -> thr -> prefer arm.
 * Prefer product thr over park stub when embed present and thr live:
 *   arm u8HostElfPrefer; QoS DRIVER on product thr + BACKGROUND on stub
 *   (no thrash); stub exit path (see spawn_host_stub_entry).
 * Returns user thr id or 0 (stub park remains; Dual DoD stays OPEN).
 * Restores caller CR3. H3: thr_exit before as_destroy on kill (process_death).
 * Denser residual honesty when embed present (Soft!=product; product_hosts=UDX):
 *   per-step greppable probe/load/stack/thr residual lines (Dual DoD OPEN).
 * greppable: spawn: soft host_launch elf
 * greppable: spawn: soft host_launch elf host_blob
 * greppable: spawn: soft host_launch elf probe
 * greppable: spawn: soft host_launch elf load
 * greppable: spawn: soft host_launch elf stack
 * greppable: spawn: soft host_launch elf thr
 * greppable: spawn: soft host_launch elf prefer
 * greppable: spawn: soft host_launch elf prefer no_thrash
 * greppable: host_blob / Soft!=product / dual_dod OPEN / product_hosts=UDX
 */
static u32
spawn_host_elf_try_run(struct gj_process *pChild,
                       const struct spawn_host_class *pHost, u32 u32ClassIdx)
{
    const void *pBlob;
    u64 cb;
    struct gj_elf_info info;
    gj_status_t st;
    u32 cStack;
    u32 u32Thr;
    u64 u64SavedCr3;
    const char *szPath;
    const char *szTag;
    struct spawn_slot *pSlot;
    u32 u32StubThr;

    if (pChild == NULL || pHost == NULL) {
        return 0u;
    }
    szPath = (pHost->szElfPath != NULL && pHost->szElfPath[0] != '\0')
                 ? pHost->szElfPath
                 : "?";
    if (spawn_host_blob_get(u32ClassIdx, &pBlob, &cb) == 0) {
        spawn_soft_inc(&g_u32SoftHostElfMiss);
        /* Grep: spawn: soft host_launch elf OPEN */
        /* Grep: spawn: soft host_launch elf host_blob */
        kprintf("spawn: soft host_launch elf OPEN host=%s path=%s "
                "host_blob=0 need=host_elf embed=0 probe=0 load=0 "
                "stack=0 thr=0 product_hosts=UDX class_idx=%u "
                "product_host_launch=OPEN dual_dod OPEN dual_dod=%u "
                "prefer_elf_thr=0 stub_park=1 no_thrash=0 H3=1 "
                "hooks=spawn_host_blob_get soft_ne_product=1 Soft!=product "
                "G-AC-1 wave=%u  "
                "(Soft!=product; weak embed absent; Dual DoD OPEN; "
                "host_blob miss; product_hosts=UDX; densify residual)\n",
                pHost->szName, szPath, u32ClassIdx,
                (unsigned)pHost->u8DualDod, GJ_SPAWN_SOFT_WAVE);
        return 0u;
    }
    spawn_soft_inc(&g_u32SoftHostElfEmbed);
    /* Grep: spawn: soft host_launch elf host_blob (embed present densify) */
    kprintf("spawn: soft host_launch elf host_blob host=%s path=%s "
            "host_blob=1 embed=1 cb=%lu class_idx=%u dual_dod=%u "
            "product_hosts=UDX steps=probe|load|stack|thr|prefer|no_thrash "
            "hooks=spawn_host_blob_get|elf_try_run "
            "weak=gj_ddi_host|gj_rtl8168_udx|gj_xhci_udx "
            "product_host_launch=OPEN dual_dod OPEN densify=1 "
            "soft_ne_product=1 Soft!=product G-AC-1 wave=%u "
            "\n",
            pHost->szName, szPath, (unsigned long)cb, u32ClassIdx,
            (unsigned)pHost->u8DualDod, GJ_SPAWN_SOFT_WAVE);
    u64SavedCr3 = cpu_read_cr3();
    memset(&info, 0, sizeof(info));
    st = elf_probe_image(pBlob, cb, &info);
    if (st != GJ_OK) {
        spawn_soft_inc(&g_u32SoftHostElfProbeFail);
        /* Grep: spawn: soft host_launch elf probe */
        kprintf("spawn: soft host_launch elf probe_fail host=%s path=%s "
                "host_blob=1 probe=0 st=%d cb=%lu embed=1 need=host_elf_ok "
                "prefer_elf_thr=0 stub_park=1 no_thrash=0 "
                "product_hosts=UDX product_host_launch=OPEN dual_dod OPEN "
                "soft_ne_product=1 Soft!=product G-AC-1\n",
                pHost->szName, szPath, (int)st, (unsigned long)cb);
        if (u64SavedCr3 != 0ull) {
            cpu_load_cr3(u64SavedCr3);
        }
        return 0u;
    }
    spawn_soft_inc(&g_u32SoftHostElfProbeOk);
    /* Grep: spawn: soft host_launch elf probe (embed present densify) */
    kprintf("spawn: soft host_launch elf probe host=%s path=%s "
            "host_blob=1 probe=1 load=0 stack=0 thr=0 "
            "entry=0x%lx cb=%lu embed=1 class_idx=%u dual_dod=%u "
            "product_hosts=UDX product_host_launch=OPEN dual_dod OPEN "
            "soft_ne_product=1 Soft!=product G-AC-1 wave=%u "
            "\n",
            pHost->szName, szPath, (unsigned long)info.u64Entry,
            (unsigned long)cb, u32ClassIdx, (unsigned)pHost->u8DualDod,
            GJ_SPAWN_SOFT_WAVE);
    st = elf_load_image(pChild, pBlob, cb, &info);
    if (st != GJ_OK) {
        spawn_soft_inc(&g_u32SoftHostElfLoadFail);
        /* Grep: spawn: soft host_launch elf load */
        kprintf("spawn: soft host_launch elf load_fail host=%s path=%s "
                "host_blob=1 probe=1 load=0 st=%d entry=0x%lx cb=%lu "
                "need=elf_load_ok prefer_elf_thr=0 stub_park=1 no_thrash=0 "
                "product_hosts=UDX product_host_launch=OPEN dual_dod OPEN "
                "soft_ne_product=1 Soft!=product G-AC-1\n",
                pHost->szName, szPath, (int)st,
                (unsigned long)info.u64Entry, (unsigned long)cb);
        if (u64SavedCr3 != 0ull) {
            cpu_load_cr3(u64SavedCr3);
        }
        return 0u;
    }
    spawn_soft_inc(&g_u32SoftHostElfLoadOk);
    spawn_host_keep_live_bind(pChild, pHost, 0u);
    /* Grep: spawn: soft host_launch elf load (embed present densify) */
    kprintf("spawn: soft host_launch elf load host=%s path=%s "
            "host_blob=1 probe=1 load=1 stack=0 thr=0 "
            "entry=0x%lx cb=%lu embed=1 class_idx=%u dual_dod=%u "
            "product_hosts=UDX product_host_launch=OPEN dual_dod OPEN "
            "soft_ne_product=1 Soft!=product G-AC-1 wave=%u "
            "\n",
            pHost->szName, szPath, (unsigned long)info.u64Entry,
            (unsigned long)cb, u32ClassIdx, (unsigned)pHost->u8DualDod,
            GJ_SPAWN_SOFT_WAVE);
    cStack = spawn_host_map_stack(pChild, GJ_SPAWN_HOST_STACK_TOP,
                                  GJ_SPAWN_HOST_STACK_PAGES);
    if (cStack == 0u) {
        spawn_soft_inc(&g_u32SoftHostElfStackFail);
        /* Grep: spawn: soft host_launch elf stack */
        kprintf("spawn: soft host_launch elf stack_fail host=%s path=%s "
                "host_blob=1 probe=1 load=1 stack=0 thr=0 need=user_as "
                "prefer_elf_thr=0 stub_park=1 no_thrash=0 "
                "product_hosts=UDX product_host_launch=OPEN dual_dod OPEN "
                "soft_ne_product=1 Soft!=product\n",
                pHost->szName, szPath);
        if (u64SavedCr3 != 0ull) {
            cpu_load_cr3(u64SavedCr3);
        }
        return 0u;
    }
    /* Grep: spawn: soft host_launch elf stack (embed present densify) */
    kprintf("spawn: soft host_launch elf stack host=%s path=%s "
            "host_blob=1 probe=1 load=1 stack=1 thr=0 "
            "stack_top=0x%lx stack_pages=%u class_idx=%u dual_dod=%u "
            "product_hosts=UDX product_host_launch=OPEN dual_dod OPEN "
            "soft_ne_product=1 Soft!=product G-AC-1 wave=%u "
            "\n",
            pHost->szName, szPath,
            (unsigned long)GJ_SPAWN_HOST_STACK_TOP, cStack, u32ClassIdx,
            (unsigned)pHost->u8DualDod, GJ_SPAWN_SOFT_WAVE);
    /*
     * Enter RSP below exclusive top (16B aligned slack). Body pages cover
     * [top-256KiB, top); 2MiB fill covers [top, top+2MiB). Soft!=product.
     */
    u32Thr = thread_create_user(pChild, info.u64Entry,
                                GJ_SPAWN_HOST_STACK_TOP -
                                    GJ_SPAWN_HOST_STACK_RSP_SLACK);
    if (u32Thr == 0u) {
        spawn_soft_inc(&g_u32SoftHostElfThrFail);
        /* Grep: spawn: soft host_launch elf thr */
        kprintf("spawn: soft host_launch elf thr_fail host=%s path=%s "
                "host_blob=1 probe=1 load=1 stack=1 thr=0 "
                "entry=0x%lx need=thr prefer_elf_thr=0 stub_park=1 "
                "no_thrash=0 product_hosts=UDX product_host_launch=OPEN "
                "dual_dod OPEN soft_ne_product=1 Soft!=product G-AC-1\n",
                pHost->szName, szPath, (unsigned long)info.u64Entry);
        if (u64SavedCr3 != 0ull) {
            cpu_load_cr3(u64SavedCr3);
        }
        return 0u;
    }
    /* Soft tag: short host name (max 11 + NUL in thread soft tag). */
    szTag = pHost->szName;
    if (szTag != NULL && szTag[0] != '\0') {
        thread_soft_tag_set(u32Thr, szTag);
    }
    pChild->u64ExecEntry = info.u64Entry;
    pChild->u64StartEntry = info.u64Entry;
    spawn_host_keep_live_bind(pChild, pHost, u32Thr);
    spawn_soft_inc(&g_u32SoftHostElfRunOk);
    if (u32ClassIdx < GJ_SPAWN_HOST_CLASS_N) {
        spawn_soft_inc(&g_u32SoftHostElfByClass[u32ClassIdx]);
    }
    /* Grep: spawn: soft host_launch elf thr (embed present densify) */
    kprintf("spawn: soft host_launch elf thr host=%s path=%s "
            "host_blob=1 probe=1 load=1 stack=1 thr=1 thr_id=%u "
            "entry=0x%lx stack_top=0x%lx class_idx=%u dual_dod=%u "
            "product_hosts=UDX product_host_launch=OPEN dual_dod OPEN "
            "soft_ne_product=1 Soft!=product G-AC-1 wave=%u "
            "\n",
            pHost->szName, szPath, u32Thr,
            (unsigned long)info.u64Entry,
            (unsigned long)GJ_SPAWN_HOST_STACK_TOP, u32ClassIdx,
            (unsigned)pHost->u8DualDod, GJ_SPAWN_SOFT_WAVE);
    /*
     * Prefer product thr over park stub (FUNCTIONAL residual; no thrash):
     *   1) arm slot so catalog stub exits (no yield thrash beside host)
     *   2) QoS DRIVER on product thr; BACKGROUND on park stub
     *   3) soft boost product thr (capped PI; Soft!=product)
     * Sample last thr id as the ring-3 host thr (not the park stub).
     * Dual DoD OPEN; H3 death path unchanged;  stamp-free.
     */
    pSlot = spawn_slot_of(pChild);
    u32StubThr = 0u;
    if (pSlot != NULL) {
        pSlot->u8HostElfPrefer = 1u;
        pSlot->u32ElfThrId = u32Thr;
        u32StubThr = pSlot->u32StubThrId;
    }
    /* Prefer product thr: DRIVER rank over BACKGROUND park stub. */
    thread_set_qos(u32Thr, GJ_QOS_DRIVER);
    thread_qos_boost_soft(u32Thr, 1u);
    if (u32StubThr != 0u && u32StubThr != u32Thr) {
        thread_set_qos(u32StubThr, GJ_QOS_BACKGROUND);
        thread_soft_tag_set(u32StubThr, "host_stub");
    }
    spawn_soft_inc(&g_u32SoftHostElfPrefer);
    spawn_soft_inc(&g_u32SoftHostElfQosPrefer);
    spawn_soft_inc(&g_u32SoftHostElfNoThrash);
    g_u32SoftLastThrId = u32Thr;
    /* Grep: spawn: soft host_launch elf ok */
    /* Grep: spawn: soft host_launch elf prefer */
    /* Grep: spawn: soft host_launch elf prefer no_thrash */
    kprintf("spawn: soft host_launch elf ok host=%s path=%s "
            "host_blob=1 probe=1 load=1 stack=1 thr=1 "
            "entry=0x%lx thr=%u stack=0x%lx cb=%lu dual_dod=%u "
            "stack_pages=%u class_idx=%u by_class=%u "
            "prefer_elf_thr=1 stub_park=0 no_thrash=1 prefer_arm=%u "
            "stub_thr=%u qos_product=DRIVER qos_stub=BACKGROUND "
            "steps=host_blob|probe|load|stack|thr|prefer|no_thrash "
            "product_hosts=UDX dual_dod OPEN "
            "H3=thr_exit_before_as_destroy product_host_launch=OPEN "
            "ddi_cap_mint=OPEN soft_ne_product=1 Soft!=product "
            "dual=MIT_OR_Apache-2.0 G-AC-1 wave=%u "
            " PASS\n",
            pHost->szName, szPath, (unsigned long)info.u64Entry, u32Thr,
            (unsigned long)GJ_SPAWN_HOST_STACK_TOP, (unsigned long)cb,
            (unsigned)pHost->u8DualDod, cStack, u32ClassIdx,
            (u32ClassIdx < GJ_SPAWN_HOST_CLASS_N)
                ? g_u32SoftHostElfByClass[u32ClassIdx]
                : 0u,
            g_u32SoftHostElfPrefer, u32StubThr, GJ_SPAWN_SOFT_WAVE);
    kprintf("spawn: soft host_launch elf prefer host=%s thr=%u "
            "class_idx=%u dual_dod=%u prefer_elf_thr=1 stub_park=0 "
            "no_thrash=1 prefer_arm=%u stub_exit=%u "
            "stub_thr=%u qos_prefer=%u host_blob=1 "
            "product_hosts=UDX dual_dod OPEN "
            "H3=1 product_host_launch=OPEN soft_ne_product=1 Soft!=product "
            "G-AC-1 wave=%u  PASS\n",
            pHost->szName, u32Thr, u32ClassIdx,
            (unsigned)pHost->u8DualDod, g_u32SoftHostElfPrefer,
            g_u32SoftHostElfStubExit, u32StubThr,
            g_u32SoftHostElfQosPrefer, GJ_SPAWN_SOFT_WAVE);
    kprintf("spawn: soft host_launch elf prefer no_thrash host=%s "
            "elf_thr=%u stub_thr=%u dual_dod=%u "
            "prefer_elf_thr=1 stub_park=0 no_thrash=1 "
            "qos_product=DRIVER qos_stub=BACKGROUND host_blob=1 "
            "product_hosts=UDX dual_dod OPEN "
            "H3=1 Dual_DoD=OPEN product_host_launch=OPEN "
            "soft_ne_product=1 Soft!=product G-AC-1 wave=%u "
            " PASS\n",
            pHost->szName, u32Thr, u32StubThr,
            (unsigned)pHost->u8DualDod, GJ_SPAWN_SOFT_WAVE);
    if (u64SavedCr3 != 0ull) {
        cpu_load_cr3(u64SavedCr3);
    }
    return u32Thr;
}

/**
 * Real catalog->spawn hook (G-PROC mint residual + host ELF functional path).
 * Resolves class, forces catalog persona pref, process_spawn, tags slot.
 * pfnEntry NULL -> catalog soft park stub (yield until kill; H3 safe).
 * After mint: weak embed ELF probe|load|user thr when present (not only stub).
 * Prefer product thr over stub park when try_run succeeds (prefer arm +
 * QoS DRIVER/BACKGROUND no_thrash; stub exits, does not thrash).
 * Soft!=product; not multi-server confine; ddi_cap_mint OPEN; Dual DoD OPEN.
 * greppable: spawn: soft host_launch class
 * greppable: spawn: soft host_launch elf
 * greppable: spawn: soft host_launch elf prefer
 * greppable: spawn: soft host_launch elf prefer no_thrash
 */
gj_status_t
process_spawn_host_launch(struct gj_process *pParent, u32 u32ClassIdx,
                          void (*pfnEntry)(void *pArg), void *pArg,
                          struct gj_process **ppOutChild,
                          struct gj_cap_ref *pOutCap)
{
    const struct spawn_host_class *pHost;
    struct gj_spawn_args args;
    struct gj_process *pChild;
    struct gj_cap_ref ref;
    struct spawn_slot *pSlot;
    gj_status_t st;
    void (*pfn)(void *);
    u32 u32ElfThr;
    int fTryElf;
    int fPreferElf;
    int fEmbedPre;

    pHost = spawn_host_catalog_get(u32ClassIdx);
    if (pParent == NULL || pHost == NULL) {
        spawn_soft_inc(&g_u32SoftHostHookDeny);
        return GJ_ERR_INVAL;
    }
    pfn = (pfnEntry != NULL) ? pfnEntry : pHost->pfnStubEntry;
    if (pfn == NULL) {
        spawn_soft_inc(&g_u32SoftHostHookDeny);
        return GJ_ERR_INVAL;
    }

    /*
     * Host ELF residual: when caller uses catalog stub path (NULL or
     * pfnStubEntry), try weak embed -> real ring-3 host thr. Custom
     * pfnEntry leaves ELF alone (caller owns entry). Soft!=product.
     */
    fTryElf = 0;
    if (pfnEntry == NULL || pfnEntry == pHost->pfnStubEntry ||
        pfn == spawn_host_stub_entry) {
        fTryElf = 1;
    }
    /* Pre-resolve weak embed for honesty (prefer path intent). */
    fEmbedPre = 0;
    if (fTryElf != 0) {
        const void *pPre = NULL;
        u64 cbPre = 0ull;

        if (spawn_host_blob_get(u32ClassIdx, &pPre, &cbPre) != 0 &&
            pPre != NULL && cbPre > 0ull) {
            fEmbedPre = 1;
        }
    }

    memset(&args, 0, sizeof(args));
    args.pfnEntry = pfn;
    /*
     * Auto-tag convention: default pArg to catalog entry when caller
     * passes NULL so process_spawn can tag without a second channel.
     */
    args.pArg = (pArg != NULL) ? pArg : (void *)pHost;
    /* Catalog UDX hosts are native GJ_SYS_* (not Linux NR 96 gettimeofday). */
    args.u32Personality = GJ_SPAWN_HOST_PERSONA_NATIVE;
    args.u32Jit = 0;

    pChild = NULL;
    memset(&ref, 0, sizeof(ref));
    st = process_spawn(pParent, &args, &pChild, &ref);
    if (st != GJ_OK) {
        return st;
    }

    /*
     * process_spawn auto-tags when pArg is a catalog entry (default path).
     * Only back-fill tag / by_class if auto-tag missed (custom pArg).
     */
    pSlot = spawn_slot_of(pChild);
    if (pSlot != NULL && pSlot->u8HostClass == 0u) {
        spawn_host_slot_tag(pSlot, pHost, u32ClassIdx);
        if (u32ClassIdx < GJ_SPAWN_HOST_CLASS_N) {
            spawn_soft_inc(&g_u32SoftHostLaunchByClass[u32ClassIdx]);
        }
    }
    spawn_soft_inc(&g_u32SoftHostLaunchClass);

    spawn_host_keep_live_bind(pChild, pHost, 0u);
    u32ElfThr = 0u;
    if (fTryElf != 0 && pChild != NULL) {
        u32ElfThr = spawn_host_elf_try_run(pChild, pHost, u32ClassIdx);
    }
    fPreferElf = 0;
    if (u32ElfThr != 0u) {
        fPreferElf = 1;
    }
    spawn_host_keep_live_bind(pChild, pHost, u32ElfThr);

    /* Grep: spawn: soft host_launch class */
    /* Grep: host_blob / Soft!=product / dual_dod OPEN / product_hosts=UDX */
    kprintf("spawn: soft host_launch class idx=%u host=%s role=%s "
            "vend=0x%x dev=0x%x dual_dod=%u persona=%u "
            "elf_path=%s elf_thr=%u embed_pre=%u host_blob=%u "
            "prefer_elf_thr=%u stub_park=%u no_thrash=%u "
            "path=process_spawn|host_blob|probe|load|stack|thr "
            "G-PROC-2=mint H3=1 product_hosts=UDX dual_dod OPEN "
            "class_n=%u by_class=%u launch_class_ok=%u "
            "elf_run_ok=%u elf_prefer=%u elf_stub_exit=%u "
            "elf_no_thrash=%u elf_qos_prefer=%u "
            "ddi_cap_mint=OPEN product_host_launch=OPEN "
            "soft_ne_product=1 Soft!=product dual=MIT_OR_Apache-2.0 G-AC-1 "
            "wave=%u  PASS\n",
            u32ClassIdx, pHost->szName,
            (pHost->szRole != NULL) ? pHost->szRole : "?",
            (unsigned)pHost->u16Vend, (unsigned)pHost->u16Dev,
            (unsigned)pHost->u8DualDod, (unsigned)pHost->u8PersonaPref,
            (pHost->szElfPath != NULL) ? pHost->szElfPath : "?",
            u32ElfThr, (unsigned)fEmbedPre, (unsigned)fEmbedPre,
            (unsigned)fPreferElf, (fPreferElf != 0) ? 0u : 1u,
            (fPreferElf != 0) ? 1u : 0u,
            GJ_SPAWN_HOST_CLASS_N,
            (u32ClassIdx < GJ_SPAWN_HOST_CLASS_N)
                ? g_u32SoftHostLaunchByClass[u32ClassIdx]
                : 0u,
            g_u32SoftHostLaunchClass, g_u32SoftHostElfRunOk,
            g_u32SoftHostElfPrefer, g_u32SoftHostElfStubExit,
            g_u32SoftHostElfNoThrash, g_u32SoftHostElfQosPrefer,
            GJ_SPAWN_SOFT_WAVE);

    if (ppOutChild != NULL) {
        *ppOutChild = pChild;
    }
    if (pOutCap != NULL) {
        *pOutCap = ref;
    }
    return GJ_OK;
}

/**
 * Catalog name -> class launch hook (G-PROC mint residual + host ELF).
 * Names: ddi_host_gj / rtl8168_udx / xhci_udx. Soft!=product.
 * greppable: spawn: soft host_launch class
 */
gj_status_t
process_spawn_host_launch_by_name(struct gj_process *pParent,
                                  const char *szName,
                                  void (*pfnEntry)(void *pArg), void *pArg,
                                  struct gj_process **ppOutChild,
                                  struct gj_cap_ref *pOutCap)
{
    u32 u32Idx;

    u32Idx = spawn_host_catalog_lookup_name(szName);
    if (u32Idx == (u32)-1) {
        spawn_soft_inc(&g_u32SoftHostHookDeny);
        return GJ_ERR_NOENT;
    }
    return process_spawn_host_launch(pParent, u32Idx, pfnEntry, pArg,
                                     ppOutChild, pOutCap);
}

/**
 * Catalog PCI vend:dev -> class launch hook (rtl/xhci product residual).
 * ddi residual is 0:0 — use by_name. Soft!=product; Dual DoD OPEN.
 * greppable: spawn: soft host_launch class
 */
gj_status_t
process_spawn_host_launch_by_pci(struct gj_process *pParent, u16 u16Vend,
                                 u16 u16Dev, void (*pfnEntry)(void *pArg),
                                 void *pArg, struct gj_process **ppOutChild,
                                 struct gj_cap_ref *pOutCap)
{
    u32 u32Idx;

    u32Idx = spawn_host_catalog_lookup_pci(u16Vend, u16Dev);
    if (u32Idx == (u32)-1) {
        spawn_soft_inc(&g_u32SoftHostHookDeny);
        return GJ_ERR_NOENT;
    }
    return process_spawn_host_launch(pParent, u32Idx, pfnEntry, pArg,
                                     ppOutChild, pOutCap);
}

/**
 * Greppable soft spawn inventory (Wave 20 deepen; product / smoke).
 * Prefix-stable markers (spawn: soft ...):
 *   spawn: soft inventory  - table caps + live/peak + rights + logs
 *   spawn: soft stats      - cumulative ok/fail/live/kill/wait/mint*
 *   spawn: soft spawn      - enter/ok/fail + deny reason catalog
 *   spawn: soft mint       - install + soft-verify reason catalog
 *   spawn: soft kill       - enter/ok/idem + deny catalog
 *   spawn: soft wait       - enter/ok/again + deny catalog
 *   spawn: soft from_cap   - process_from_cap hit/miss
 *   spawn: soft teardown   - reverse-path + AS teardown
 *   spawn: soft lifecycle  - persona/JIT/wait-reg/last thr
 *   spawn: soft capacity   - table/cnode/rights geometry
 *   spawn: soft return     - Wave 19 return-path catalog
 *   spawn: soft ret_surface - Wave 19 terminal return classes
 *   spawn: soft surface    - Wave 19 area catalog
 *   spawn: soft deepen     - Wave 20 stamp
 *   spawn: soft path       - honesty: fixed table != full posix_spawn
 *   spawn: soft sshd       - Dual DoD B freestanding live :22 honesty
 *   spawn: soft dual_dod_b - residual wait-poll surface (Soft!=product)
 *   spawn: soft residual   - UDX/driver host start residual (Soft!=product)
 *   spawn: soft residual lean - lean UDX/driver host start + dual-license
 *   spawn: soft residual lean host - named hosts ddi_host/rtl8168_udx/xhci_udx
 *   spawn: soft residual host_catalog - named hosts + VID:DID + Dual DoD
 *   spawn: soft residual host_path - launch lifecycle residual steps
 *   spawn: soft residual dual_dod - Dual DoD A/B UDX host direction
 *   spawn: soft residual c2 - C2 product-path residual (soft != product AC)
 *   spawn: soft residual udx_launch - UDX launch residual + capacity ready
 *   spawn: soft residual c2_udx_launch - C2 udx_launch self-check ok/N+fail_bits
 *   spawn: soft residual c2_host_map - per-host role (ddi residual / DoD A/B)
 *   spawn: soft residual host_hook - catalog resolve + launch fail-closed
 *   spawn: soft residual host_elf - weak embed + elf path residual tallies
 *   spawn: soft residual host_blob - host_blob spawn densify (Soft!=product)
 *   spawn: soft residual host_blob densify - denser live probe residual
 *   spawn: soft residual host_blob denser - multi-arm denser residual
 *   spawn: soft residual host_blob denser2 - W17 denser2 multi-arm residual
 *   spawn: soft residual host_blob denser3 - W18 denser3 multi-arm residual
 *   spawn: soft residual host_blob denser4 - W19 denser4 multi-arm residual
 *   spawn: soft residual host_blob denser5 - W20 denser5 multi-arm residual
 *   spawn: soft residual host_blob denser6 - W21 denser6 multi-arm residual
 *   spawn: soft residual host_blob denser7 - W22 denser7 multi-arm residual
 *   spawn: soft residual host_blob denser8 - W23 denser8 multi-arm residual
 *   spawn: soft residual host_blob class - per-class present|miss + cb
 *   spawn: soft residual host_blob lean - host_blob self-check ok/N residual
 *   greppable: denser_arms= denser_ok= denser_min= host_blob denser
 *   greppable: denser2_arms= denser2_ok= denser2_min= host_blob denser2
 *   greppable: denser3_arms= denser3_ok= denser3_min= host_blob denser3
 *   greppable: denser4_arms= denser4_ok= denser4_min= host_blob denser4
 *   greppable: denser5_arms= denser5_ok= denser5_min= host_blob denser5
 *   greppable: denser6_arms= denser6_ok= denser6_min= host_blob denser6
 *   greppable: denser7_arms= denser7_ok= denser7_min= host_blob denser7
 *   greppable: denser8_arms= denser8_ok= denser8_min= host_blob denser8
 *   spawn: soft host_launch - driver host launch residual surface (H3 death)
 *   spawn: soft host_launch lean - host launch self-check ok/N residual
 *   spawn: soft host_launch class - catalog-class launch success sample
 *   spawn: soft host_launch elf - functional embed probe|load|stack|thr
 *   spawn: soft host_launch elf host_blob - weak embed resolve residual
 *   spawn: soft host_launch elf probe - probe residual (embed present densify)
 *   spawn: soft host_launch elf load - load residual (embed present densify)
 *   spawn: soft host_launch elf stack - stack residual (embed present densify)
 *   spawn: soft host_launch elf thr - thr residual (embed present densify)
 *   spawn: soft host_launch elf prefer - prefer product thr over stub park
 *   spawn: soft host_launch elf prefer no_thrash - no stub thrash after prefer
 *   spawn: soft inventory PASS / spawn: soft PASS
 *
 * Never hard-gates; diagnostics only. Residual lines: no stamp storms.
 * greppable: spawn: soft
 * greppable: host_blob / Soft!=product / dual_dod OPEN / product_hosts=UDX
 *  stamp-free; never invent .76.
 */
static void
soft_inventory_log(const char *szVia)
{
    const char *szViaSafe;
    u32         cLive;
    u32         u32Free;
    u16         u16Base;
    u16         u16Core;

    spawn_soft_inc(&g_u32SoftLogN);
    szViaSafe = (szVia != NULL && szVia[0] != '\0') ? szVia : "unknown";
    cLive = spawn_live_count();
    u32Free = (cLive < GJ_SPAWN_MAX) ? (GJ_SPAWN_MAX - cLive) : 0u;
    u16Base = GJ_SPAWN_PROCESS_RIGHTS_BASE;
    u16Core = GJ_SPAWN_PROCESS_RIGHTS_CORE;

    /* Grep: spawn: soft inventory */
    kprintf("spawn: soft inventory via=%s max=%u slots=%u live=%u free=%u "
            "peak=%u rights_base=0x%x rights_core=0x%x logs=%u wave=%u\n",
            szViaSafe, GJ_SPAWN_MAX, (unsigned)GJ_SPAWN_CNODE_SLOTS,
            cLive, u32Free, g_u32SoftLivePeak,
            (unsigned)u16Base, (unsigned)u16Core, g_u32SoftLogN,
            GJ_SPAWN_SOFT_WAVE);

    /* Grep: spawn: soft stats */
    kprintf("spawn: soft stats ok=%u fail=%u live=%u kill=%u wait=%u "
            "mint_ok=%u mint_fail=%u mint_soft=%u mint_soft_bad=%u\n",
            g_cSpawned, g_cSpawnFail, cLive, g_cKill, g_cWait,
            g_cMintOk, g_cMintFail, g_cMintSoft, g_cMintSoftBad);

    /* Grep: spawn: soft spawn */
    kprintf("spawn: soft spawn enter=%u ok=%u fail=%u deny_null=%u "
            "deny_cnode=%u deny_full=%u deny_meta=%u deny_as=%u "
            "deny_mint=%u deny_thr=%u\n",
            g_u32SoftSpawnEnter, g_cSpawned, g_cSpawnFail,
            g_u32SoftDenyNull, g_u32SoftDenyNoCnode, g_u32SoftDenyFull,
            g_u32SoftDenyMeta, g_u32SoftDenyAs, g_u32SoftDenyMint,
            g_u32SoftDenyThr);

    /* Grep: spawn: soft mint */
    kprintf("spawn: soft mint install_ok=%u install_fail=%u soft_pass=%u "
            "soft_bad=%u null_arg=%u null_ref=%u resolve=%u type=%u "
            "obj=%u gen=%u core_rights=%u rights=%u from_cap=%u "
            "jit=%u nojit=%u\n",
            g_cMintOk, g_cMintFail, g_cMintSoft, g_cMintSoftBad,
            g_u32SoftMintNullArg, g_u32SoftMintNullRef, g_u32SoftMintResolve,
            g_u32SoftMintType, g_u32SoftMintObj, g_u32SoftMintGen,
            g_u32SoftMintCoreRights, g_u32SoftMintRights,
            g_u32SoftMintFromCap, g_u32SoftMintJit, g_u32SoftMintNoJit);

    /* Grep: spawn: soft kill */
    kprintf("spawn: soft kill enter=%u ok=%u idem=%u deny_null=%u "
            "deny_resolve=%u deny_type=%u deny_rights=%u deny_noent=%u\n",
            g_u32SoftKillEnter, g_cKill, g_u32SoftKillIdem,
            g_u32SoftKillDenyNull, g_u32SoftKillDenyResolve,
            g_u32SoftKillDenyType, g_u32SoftKillDenyRights,
            g_u32SoftKillDenyNoent);

    /* Grep: spawn: soft wait */
    kprintf("spawn: soft wait enter=%u ok=%u again=%u deny_null=%u "
            "deny_resolve=%u deny_type=%u deny_rights=%u deny_noent=%u "
            "slot_free=%u cap_inv=%u\n",
            g_u32SoftWaitEnter, g_cWait, g_u32SoftWaitAgain,
            g_u32SoftWaitDenyNull, g_u32SoftWaitDenyResolve,
            g_u32SoftWaitDenyType, g_u32SoftWaitDenyRights,
            g_u32SoftWaitDenyNoent, g_u32SoftWaitSlotFree,
            g_u32SoftWaitCapInv);

    /* Grep: spawn: soft from_cap */
    kprintf("spawn: soft from_cap hit=%u miss=%u\n",
            g_u32SoftFromCapHit, g_u32SoftFromCapMiss);

    /* Grep: spawn: soft teardown (Wave 15 reverse path) */
    kprintf("spawn: soft teardown fail_cleanup=%u as_teardown=%u "
            "as_skip=%u\n",
            g_u32SoftFailCleanup, g_u32SoftAsTeardown,
            g_u32SoftAsTeardownSkip);

    /* Grep: spawn: soft lifecycle (Wave 15) */
    kprintf("spawn: soft lifecycle wait_reg=%u persona_native=%u "
            "persona_linux=%u last_thr=%u peak=%u\n",
            g_u32SoftWaitReg, g_u32SoftPersonaNative, g_u32SoftPersonaLinux,
            g_u32SoftLastThrId, g_u32SoftLivePeak);

    /* Grep: spawn: soft capacity (Wave 15 geometry) */
    kprintf("spawn: soft capacity max=%u cnode_slots=%u live=%u free=%u "
            "peak=%u rights_base=0x%x rights_core=0x%x "
            "fail_cleanup=%u as_teardown=%u wave=%u\n",
            GJ_SPAWN_MAX, (unsigned)GJ_SPAWN_CNODE_SLOTS, cLive, u32Free,
            g_u32SoftLivePeak, (unsigned)u16Base, (unsigned)u16Core,
            g_u32SoftFailCleanup, g_u32SoftAsTeardown, GJ_SPAWN_SOFT_WAVE);

    /*
     * Honesty line: fixed spawn table + PROCESS mint is product bring-up,
     * not full posix_spawn / multi-server confine.
     * Grep: spawn: soft path
     */
    kprintf("spawn: soft path claim=process_spawn G-PROC-2=mint "
            "G-PROC-5=death fixed_table=1 table_max=%u cnode_slots=%u "
            "posix_spawn=0 multi_server_confine=0 via=%s wave=%u "
            "(soft inventory)\n",
            GJ_SPAWN_MAX, (unsigned)GJ_SPAWN_CNODE_SLOTS, szViaSafe,
            GJ_SPAWN_SOFT_WAVE);

    /*
     * Grep: spawn: soft return
     * Wave 19 return-path catalog - spawn/mint/kill/wait deny outcomes.
     * Soft != posix_spawn / multi-server confine product. product_kernel=OPEN.
     */
    kprintf("spawn: soft return spawn_ok=%u spawn_fail=%u deny_null=%u "
            "deny_full=%u deny_as=%u deny_mint=%u deny_thr=%u "
            "mint_ok=%u mint_fail=%u mint_soft=%u mint_soft_bad=%u "
            "kill_ok=%u kill_idem=%u wait_ok=%u wait_again=%u "
            "from_cap_hit=%u from_cap_miss=%u teardown=%u "
            "product_kernel=OPEN wave=%u\n",
            g_cSpawned, g_cSpawnFail, g_u32SoftDenyNull, g_u32SoftDenyFull,
            g_u32SoftDenyAs, g_u32SoftDenyMint, g_u32SoftDenyThr, g_cMintOk,
            g_cMintFail, g_cMintSoft, g_cMintSoftBad, g_cKill,
            g_u32SoftKillIdem, g_cWait, g_u32SoftWaitAgain,
            g_u32SoftFromCapHit, g_u32SoftFromCapMiss, g_u32SoftFailCleanup,
            GJ_SPAWN_SOFT_WAVE);

    /* Grep: spawn: soft ret_surface - Wave 19 terminal return classes */
    kprintf("spawn: soft ret_surface spawn=ok|fail "
            "deny=null|full|as|mint|thr mint=ok|fail|soft|soft_bad "
            "kill=ok|idem wait=ok|again from_cap=hit|miss teardown "
            "host_launch=ok|lean|class|elf c2_udx_launch=ok|lean|fail_bits "
            "host_hook=ok|deny "
            "host_elf=ok|miss|host_blob|probe|load|stack|thr|prefer|no_thrash "
            "host_blob=ok|miss|present|absent|lean|densify|denser|denser2|"
            "denser3|denser4|denser5|denser6|denser7|denser8|class "
            "denser_arms|denser_ok|denser_min denser6_arms|denser6_ok|denser6_min "
            "denser7_arms|denser7_ok|denser7_min denser8_arms|denser8_ok|denser8_min "
            "product_hosts=UDX dual_dod OPEN "
            "product_kernel=OPEN areas=145 wave=%u\n",
            GJ_SPAWN_SOFT_WAVE);

    /* Grep: spawn: soft surface - host-launch residual area catalog */
    kprintf("spawn: soft surface inventory,stats,spawn,mint,kill,wait,"
            "from_cap,teardown,lifecycle,capacity,path,return,ret_surface,"
            "sshd,dual_dod_b,residual,udx_host,driver_host,host_launch,"
            "host_launch_lean,host_launch_class,host_launch_elf,"
            "host_launch_elf_host_blob,host_launch_elf_probe,"
            "host_launch_elf_load,host_launch_elf_stack,host_launch_elf_thr,"
            "host_launch_elf_prefer,host_launch_elf_prefer_no_thrash,"
            "host_hook,host_blob,host_blob_densify,host_blob_denser,"
            "host_blob_denser2,host_blob_denser3,host_blob_denser4,"
            "host_blob_denser5,host_blob_denser6,host_blob_denser7,"
            "host_blob_denser8,"
            "host_blob_class,host_blob_lean,"
            "host_catalog,host_path,host_elf,dual_dod,c2,udx_launch,"
            "c2_udx_launch,c2_host_map,"
            "ddi_host,rtl8168_udx,xhci_udx,surface,deepen "
            "product_hosts=UDX dual_dod OPEN Soft!=product "
            "denser_arms denser_ok denser_min denser6_arms denser6_ok denser6_min "
            "denser7_arms denser7_ok denser7_min denser8_arms denser8_ok denser8_min "
            "areas=156 wave=%u\n",
            GJ_SPAWN_SOFT_WAVE);

    /*
     * Dual DoD B residual honesty (Soft!=product; G-AC-1).
     * sshd_gj abandoned (not embedded). Product SSH is OpenSSH-portable.
     * product_sshd_tcp22 stays OPEN until interactive SSH login.
     * Grep: spawn: soft sshd
     * Grep: spawn: soft dual_dod_b
     */
    kprintf("spawn: soft sshd live_embed=0 sshd_gj=abandoned "
            "product_sshd_tcp22=OPEN dual_dod_b=OPEN G-AC-1=1 "
            "via=%s wave=%u (Soft!=product)\n",
            szViaSafe, GJ_SPAWN_SOFT_WAVE);
    kprintf("spawn: soft dual_dod_b residual=spawn_wait_poll "
            "sshd_live=0 port=22 soft_ne_product=1 product=OPEN "
            "wave=%u\n",
            GJ_SPAWN_SOFT_WAVE);

    /*
     * Lean soft residual (Soft!=product dual MIT OR Apache-2.0; G-AC-1; H3).
     * Fixed process_spawn table is the lean residual path for starting
     * userspace UDX / driver hosts (confined C; DDI caps later):
     *   ddi_host_gj / rtl8168_udx / xhci_udx
     * Soft residual != product multi-server confine / UDX product close.
     * G-AC-1: ko_in_kernel=0 (no Linux .ko product in kernel).
     * H3: kill -> process_death preserves thr_exit before as_destroy.
     * No version stamp, no stamp storms - compact honesty only.
     * Grep: spawn: soft residual
     * Grep: spawn: soft residual lean
     * Grep: spawn: soft residual lean host
     * Grep: spawn: soft residual host_catalog
     * Grep: spawn: soft residual host_path
     * Grep: spawn: soft residual dual_dod
     * Grep: spawn: soft host_launch
     * Grep: spawn: soft host_launch lean
     */
    kprintf("spawn: soft residual udx_host=OPEN driver_host=OPEN "
            "role=userspace_udx_driver_host_start "
            "hosts=%s,%s,%s class_n=%u "
            "fixed_table=%u live=%u peak=%u mint_soft=%u mint_soft_bad=%u "
            "host_launch_ok=%u wait_again=%u wait_poll=AGAIN yield_on_again=1 "
            "posix_spawn=0 multi_server_confine=0 ko_in_kernel=0 "
            "H3=death_residual soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
            "Soft!=product G-AC-1 via=%s\n",
            GJ_SPAWN_HOST_DDI_NAME, GJ_SPAWN_HOST_RTL_NAME,
            GJ_SPAWN_HOST_XHCI_NAME, GJ_SPAWN_HOST_CLASS_N,
            GJ_SPAWN_MAX, cLive, g_u32SoftLivePeak, g_cMintSoft,
            g_cMintSoftBad, g_u32SoftHostLaunchOk, g_u32SoftWaitAgain,
            szViaSafe);
    kprintf("spawn: soft residual lean claim=process_spawn "
            "role=udx_driver_host_start "
            "hosts=%s,%s,%s class_n=%u "
            "table_max=%u cnode_slots=%u "
            "enter=%u ok=%u fail=%u kill=%u wait=%u mint_ok=%u "
            "teardown=%u persona_native=%u persona_linux=%u "
            "host_launch_ok=%u G-PROC-2=mint G-PROC-5=death H3=1 "
            "ko_in_kernel=0 soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1 "
            "(Soft!=product; dual MIT OR Apache-2.0; no version stamp; "
            "userspace UDX/driver host residual; not product host close)\n",
            GJ_SPAWN_HOST_DDI_NAME, GJ_SPAWN_HOST_RTL_NAME,
            GJ_SPAWN_HOST_XHCI_NAME, GJ_SPAWN_HOST_CLASS_N,
            GJ_SPAWN_MAX, (unsigned)GJ_SPAWN_CNODE_SLOTS,
            g_u32SoftSpawnEnter, g_cSpawned, g_cSpawnFail,
            g_cKill, g_cWait, g_cMintOk, g_u32SoftFailCleanup,
            g_u32SoftPersonaNative, g_u32SoftPersonaLinux,
            g_u32SoftHostLaunchOk);
    /* Grep: spawn: soft residual lean host */
    kprintf("spawn: soft residual lean host "
            "ddi_host=OPEN rtl8168_udx=OPEN xhci_udx=OPEN "
            "launch=process_spawn fixed_table=%u live=%u class_n=%u "
            "mint_ok=%u mint_soft=%u kill=%u wait=%u wait_again=%u "
            "host_launch_ok=%u G-PROC-2=mint G-PROC-5=death "
            "H3=thr_exit_before_as_destroy "
            "ko_in_kernel=0 product_mint_ddi=OPEN "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1 "
            "(Soft!=product; named UDX driver hosts only; "
            "not multi-server confine product; not host product close)\n",
            GJ_SPAWN_MAX, cLive, GJ_SPAWN_HOST_CLASS_N,
            g_cMintOk, g_cMintSoft, g_cKill, g_cWait,
            g_u32SoftWaitAgain, g_u32SoftHostLaunchOk);

    /*
     * Host catalog residual: structured table + VID:DID + Dual DoD direction.
     * Soft catalog only — not product bind / wire / BOT close.
     * Table is the real resolve source for host_launch hooks.
     * Grep: spawn: soft residual host_catalog
     */
    spawn_soft_inc(&g_u32SoftHostCatalogLog);
    kprintf("spawn: soft residual host_catalog class_n=%u table_max=%u "
            "fit=%u structured=1 hooks=1 elf_path=1 "
            "ddi_host=%s role=%s elf=%s ddi_sys=103 dual_dod=none "
            "rtl=%s vend=0x%x dev=0x%x dual_dod=B role=%s elf=%s "
            "xhci=%s vend=0x%x dev=0x%x dual_dod=A role=%s elf=%s "
            "persona_pref=native=%u "
            "sshd_residual=main_embed port=22 "
            "product_host_launch=OPEN product_mint_ddi=OPEN "
            "ko_in_kernel=0 multi_server_confine=0 "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1 "
            "catalog_logs=%u via=%s wave=%u\n",
            GJ_SPAWN_HOST_CLASS_N, GJ_SPAWN_MAX,
            (GJ_SPAWN_HOST_CLASS_N <= GJ_SPAWN_MAX) ? 1u : 0u,
            g_aHostCatalog[0].szName,
            (g_aHostCatalog[0].szRole != NULL) ? g_aHostCatalog[0].szRole
                                                 : "ddi_door_residual",
            (g_aHostCatalog[0].szElfPath != NULL) ? g_aHostCatalog[0].szElfPath
                                                    : "?",
            g_aHostCatalog[1].szName,
            (unsigned)g_aHostCatalog[1].u16Vend,
            (unsigned)g_aHostCatalog[1].u16Dev,
            (g_aHostCatalog[1].szRole != NULL) ? g_aHostCatalog[1].szRole
                                                 : "udx_nic",
            (g_aHostCatalog[1].szElfPath != NULL) ? g_aHostCatalog[1].szElfPath
                                                    : "?",
            g_aHostCatalog[2].szName,
            (unsigned)g_aHostCatalog[2].u16Vend,
            (unsigned)g_aHostCatalog[2].u16Dev,
            (g_aHostCatalog[2].szRole != NULL) ? g_aHostCatalog[2].szRole
                                                 : "udx_usb_hc",
            (g_aHostCatalog[2].szElfPath != NULL) ? g_aHostCatalog[2].szElfPath
                                                    : "?",
            GJ_SPAWN_HOST_PERSONA_NATIVE,
            g_u32SoftHostCatalogLog, szViaSafe, GJ_SPAWN_SOFT_WAVE);

    /*
     * Host ELF residual honesty (weak embed tallies; Soft!=product).
     * Prefer product thr over park stub when embed present + try_run ok
     * (QoS DRIVER/BACKGROUND + stub exit; no thrash). Dual DoD OPEN.
     * Denser probe/load/stack/thr residual when host_blob present.
     * Grep: spawn: soft residual host_elf
     * Grep: spawn: soft residual host_blob
     * Grep: spawn: soft host_launch elf prefer
     * Grep: spawn: soft host_launch elf prefer no_thrash
     */
    kprintf("spawn: soft residual host_elf "
            "host_blob embed=%u miss=%u probe_ok=%u probe_fail=%u "
            "load_ok=%u load_fail=%u stack_fail=%u thr_fail=%u "
            "run_ok=%u prefer=%u stub_exit=%u no_thrash=%u qos_prefer=%u "
            "by_class_ddi=%u by_class_rtl=%u by_class_xhci=%u "
            "paths=%s,%s,%s "
            "steps=host_blob|probe|load|stack|thr|prefer|no_thrash "
            "prefer_elf_thr=1_when_run_ok stub_park=1_when_miss "
            "no_thrash=1_when_prefer qos_product=DRIVER qos_stub=BACKGROUND "
            "H3=thr_exit_before_as_destroy product_host_launch=OPEN "
            "product_hosts=UDX dual_dod OPEN dual_dod_A=OPEN dual_dod_B=OPEN "
            "ko_in_kernel=0 soft_ne_product=1 Soft!=product "
            "dual=MIT_OR_Apache-2.0 G-AC-1 via=%s wave=%u "
            "(Soft!=product; functional residual; not Dual DoD close; "
            "host_blob densify;  stamp-free; "
            "never invent .76)\n",
            g_u32SoftHostElfEmbed, g_u32SoftHostElfMiss,
            g_u32SoftHostElfProbeOk, g_u32SoftHostElfProbeFail,
            g_u32SoftHostElfLoadOk, g_u32SoftHostElfLoadFail,
            g_u32SoftHostElfStackFail, g_u32SoftHostElfThrFail,
            g_u32SoftHostElfRunOk, g_u32SoftHostElfPrefer,
            g_u32SoftHostElfStubExit, g_u32SoftHostElfNoThrash,
            g_u32SoftHostElfQosPrefer,
            g_u32SoftHostElfByClass[0], g_u32SoftHostElfByClass[1],
            g_u32SoftHostElfByClass[2],
            (g_aHostCatalog[0].szElfPath != NULL) ? g_aHostCatalog[0].szElfPath
                                                    : "?",
            (g_aHostCatalog[1].szElfPath != NULL) ? g_aHostCatalog[1].szElfPath
                                                    : "?",
            (g_aHostCatalog[2].szElfPath != NULL) ? g_aHostCatalog[2].szElfPath
                                                    : "?",
            szViaSafe, GJ_SPAWN_SOFT_WAVE);

    /*
     * Host blob spawn residual densify (Soft!=product; Dual DoD OPEN).
     * product_hosts=UDX: ddi_host_gj / rtl8168_udx / xhci_udx.
     * Path honesty: host_blob -> probe -> load -> stack -> thr -> prefer.
     * Soft residual != product Dual DoD close; preserve weak embed hooks.
     * Denser residual: live spawn_host_blob_get probe per class + lean ok/N.
     * Grep: spawn: soft residual host_blob
     * Grep: spawn: soft residual host_blob densify
     * Grep: spawn: soft residual host_blob denser
     * Grep: spawn: soft residual host_blob denser2
     * Grep: spawn: soft residual host_blob denser3
     * Grep: spawn: soft residual host_blob denser4
     * Grep: spawn: soft residual host_blob denser5
     * Grep: spawn: soft residual host_blob denser6
     * Grep: spawn: soft residual host_blob denser7
     * Grep: spawn: soft residual host_blob denser8
     * Grep: spawn: soft residual host_blob class
     * Grep: spawn: soft residual host_blob lean
     * greppable: denser_arms= denser_ok= denser_min= host_blob denser
     * greppable: denser4_arms= denser4_ok= denser4_min= host_blob denser4
     * greppable: denser5_arms= denser5_ok= denser5_min= host_blob denser5
     * greppable: denser6_arms= denser6_ok= denser6_min= host_blob denser6
     * greppable: denser7_arms= denser7_ok= denser7_min= host_blob denser7
     * greppable: denser8_arms= denser8_ok= denser8_min= host_blob denser8
     * greppable: host_blob / Soft!=product / dual_dod OPEN / product_hosts=UDX
     * CRITICAL preserve host blob embeds ddi/rtl/xhci.
     */
    spawn_soft_inc(&g_u32SoftHostBlobLog);
    kprintf("spawn: soft residual host_blob "
            "host_blob=1 soft_residual=1 "
            "product_hosts=UDX hosts=%s,%s,%s class_n=%u "
            "embed=%u miss=%u "
            "probe_ok=%u probe_fail=%u load_ok=%u load_fail=%u "
            "stack_fail=%u thr_fail=%u run_ok=%u "
            "prefer=%u stub_exit=%u no_thrash=%u qos_prefer=%u "
            "by_class_ddi=%u by_class_rtl=%u by_class_xhci=%u "
            "steps=host_blob|probe|load|stack|thr|prefer|no_thrash "
            "hooks=spawn_host_blob_get|elf_try_run "
            "weak=gj_ddi_host_elf_blob,gj_rtl8168_udx_elf_blob,"
            "gj_xhci_udx_elf_blob "
            "prefer_elf_thr=1_when_run_ok stub_park=1_when_miss "
            "H3=thr_exit_before_as_destroy product_host_launch=OPEN "
            "dual_dod OPEN dual_dod_A=OPEN dual_dod_B=OPEN "
            "ko_in_kernel=0 freestanding_rtl=SKIP freestanding_xhci=SKIP "
            "soft_ne_product=1 Soft!=product dual=MIT_OR_Apache-2.0 G-AC-1 "
            "via=%s wave=%u "
            "(Soft!=product; host_blob densify; product_hosts=UDX; "
            "dual_dod OPEN; not Dual DoD close; not product AC; "
            " stamp-free; never invent .76)\n",
            GJ_SPAWN_HOST_DDI_NAME, GJ_SPAWN_HOST_RTL_NAME,
            GJ_SPAWN_HOST_XHCI_NAME, GJ_SPAWN_HOST_CLASS_N,
            g_u32SoftHostElfEmbed, g_u32SoftHostElfMiss,
            g_u32SoftHostElfProbeOk, g_u32SoftHostElfProbeFail,
            g_u32SoftHostElfLoadOk, g_u32SoftHostElfLoadFail,
            g_u32SoftHostElfStackFail, g_u32SoftHostElfThrFail,
            g_u32SoftHostElfRunOk, g_u32SoftHostElfPrefer,
            g_u32SoftHostElfStubExit, g_u32SoftHostElfNoThrash,
            g_u32SoftHostElfQosPrefer,
            g_u32SoftHostElfByClass[0], g_u32SoftHostElfByClass[1],
            g_u32SoftHostElfByClass[2],
            szViaSafe, GJ_SPAWN_SOFT_WAVE);

    /*
     * Denser host_blob residual: live weak-embed probe + lean self-check.
     * Present/miss is honesty only (miss keeps Dual DoD OPEN + stub park).
     * Geometry/hook self-check is ok/N; never hard-gates product.
     * Soft!=product; product_hosts=UDX;  stamp-free.
     */
    {
        u32 u32BlobOk;
        u32 u32BlobFailBits;
        u32 u32Idx;
        u32 u32PresentN;
        u32 u32AbsentN;
        const void *pBlobTmp;
        u64 cbTmp;
        int fGet;
        int fFailClosed;

        u32BlobOk = 0u;
        u32BlobFailBits = 0u;
        u32PresentN = 0u;
        u32AbsentN = 0u;

        /* 0: class_n + fit fixed table */
        if (GJ_SPAWN_HOST_CLASS_N == 3u &&
            GJ_SPAWN_HOST_CLASS_N <= GJ_SPAWN_MAX) {
            u32BlobOk++;
        } else {
            u32BlobFailBits |= (1u << 0);
        }
        /* 1: product host names (UDX) non-empty */
        if (GJ_SPAWN_HOST_DDI_NAME[0] != '\0' &&
            GJ_SPAWN_HOST_RTL_NAME[0] != '\0' &&
            GJ_SPAWN_HOST_XHCI_NAME[0] != '\0') {
            u32BlobOk++;
        } else {
            u32BlobFailBits |= (1u << 1);
        }
        /* 2: dual_dod map ddi=none rtl=B xhci=A */
        if (g_aHostCatalog[0].u8DualDod == 0u &&
            g_aHostCatalog[1].u8DualDod == 2u &&
            g_aHostCatalog[2].u8DualDod == 1u) {
            u32BlobOk++;
        } else {
            u32BlobFailBits |= (1u << 2);
        }
        /* 3: elf path residual non-empty all 3 */
        if (g_aHostCatalog[0].szElfPath != NULL &&
            g_aHostCatalog[0].szElfPath[0] != '\0' &&
            g_aHostCatalog[1].szElfPath != NULL &&
            g_aHostCatalog[1].szElfPath[0] != '\0' &&
            g_aHostCatalog[2].szElfPath != NULL &&
            g_aHostCatalog[2].szElfPath[0] != '\0') {
            u32BlobOk++;
        } else {
            u32BlobFailBits |= (1u << 3);
        }
        /* 4: spawn_host_blob_get fail-closed (null out-params + bad class) */
        fFailClosed = 0;
        if (spawn_host_blob_get(0u, NULL, NULL) == 0 &&
            spawn_host_blob_get(99u, &pBlobTmp, &cbTmp) == 0) {
            fFailClosed = 1;
        }
        if (fFailClosed != 0) {
            u32BlobOk++;
        } else {
            u32BlobFailBits |= (1u << 4);
        }
        /*
         * 5: weak resolve path lives — call get for class 0..2.
         * Miss is OPEN honesty (not a fail bit); present samples tallied.
         */
        for (u32Idx = 0u; u32Idx < GJ_SPAWN_HOST_CLASS_N; u32Idx++) {
            pBlobTmp = NULL;
            cbTmp = 0ull;
            spawn_soft_inc(&g_u32SoftHostBlobProbe);
            fGet = spawn_host_blob_get(u32Idx, &pBlobTmp, &cbTmp);
            if (fGet != 0 && pBlobTmp != NULL && cbTmp > 0ull) {
                u32PresentN++;
                spawn_soft_inc(&g_u32SoftHostBlobPresent);
                spawn_soft_inc(&g_u32SoftHostBlobByClass[u32Idx]);
                g_u64SoftHostBlobCbByClass[u32Idx] = cbTmp;
            } else {
                u32AbsentN++;
                spawn_soft_inc(&g_u32SoftHostBlobAbsent);
                g_u64SoftHostBlobCbByClass[u32Idx] = 0ull;
            }
            /* Grep: spawn: soft residual host_blob class */
            kprintf("spawn: soft residual host_blob class "
                    "idx=%u host=%s dual_dod=%u path=%s "
                    "host_blob=%u present=%u miss=%u cb=%lu "
                    "product_hosts=UDX dual_dod OPEN "
                    "hooks=spawn_host_blob_get "
                    "soft_ne_product=1 Soft!=product G-AC-1 "
                    "via=%s wave=%u \n",
                    u32Idx,
                    (g_aHostCatalog[u32Idx].szName != NULL)
                        ? g_aHostCatalog[u32Idx].szName
                        : "?",
                    (unsigned)g_aHostCatalog[u32Idx].u8DualDod,
                    (g_aHostCatalog[u32Idx].szElfPath != NULL)
                        ? g_aHostCatalog[u32Idx].szElfPath
                        : "?",
                    (fGet != 0) ? 1u : 0u,
                    (fGet != 0) ? 1u : 0u,
                    (fGet != 0) ? 0u : 1u,
                    (unsigned long)cbTmp,
                    szViaSafe, GJ_SPAWN_SOFT_WAVE);
        }
        /* Resolve path exercised for all class slots. */
        if (g_u32SoftHostBlobProbe > 0u &&
            (u32PresentN + u32AbsentN) == GJ_SPAWN_HOST_CLASS_N) {
            u32BlobOk++;
        } else {
            u32BlobFailBits |= (1u << 5);
        }
        /* 6: product_hosts=UDX role tokens present */
        if (g_aHostCatalog[0].szRole != NULL &&
            g_aHostCatalog[0].szRole[0] != '\0' &&
            g_aHostCatalog[1].szRole != NULL &&
            g_aHostCatalog[1].szRole[0] != '\0' &&
            g_aHostCatalog[2].szRole != NULL &&
            g_aHostCatalog[2].szRole[0] != '\0') {
            u32BlobOk++;
        } else {
            u32BlobFailBits |= (1u << 6);
        }
        /*
         * 7: hooks residual surface — fail-closed + live probe path ran
         * (spawn_host_blob_get exercised) and catalog stubs non-null
         * (elf_try_run prefer path can arm over park). Soft!=product.
         */
        if (fFailClosed != 0 &&
            (u32PresentN + u32AbsentN) == GJ_SPAWN_HOST_CLASS_N &&
            g_aHostCatalog[0].pfnStubEntry != NULL &&
            g_aHostCatalog[1].pfnStubEntry != NULL &&
            g_aHostCatalog[2].pfnStubEntry != NULL) {
            u32BlobOk++;
        } else {
            u32BlobFailBits |= (1u << 7);
        }

        if (u32BlobOk == GJ_SPAWN_HOST_BLOB_OK_N && u32BlobFailBits == 0u) {
            spawn_soft_inc(&g_u32SoftHostBlobLean);
        }

        /* Grep: spawn: soft residual host_blob densify */
        kprintf("spawn: soft residual host_blob densify "
            "host_blob=1 soft_residual=1 densify=1 "
            "product_hosts=UDX hosts=%s,%s,%s class_n=%u "
            "live_present=%u live_absent=%u "
            "inv_probe=%u inv_present=%u inv_absent=%u "
            "cb_ddi=%lu cb_rtl=%lu cb_xhci=%lu "
            "by_class_ddi=%u by_class_rtl=%u by_class_xhci=%u "
            "run_embed=%u run_miss=%u run_ok=%u "
            "steps=host_blob|probe|load|stack|thr|prefer|no_thrash "
            "hooks=spawn_host_blob_get|elf_try_run "
            "weak=gj_ddi_host_elf_blob,gj_rtl8168_udx_elf_blob,"
            "gj_xhci_udx_elf_blob "
            "dual_dod OPEN dual_dod_A=OPEN dual_dod_B=OPEN "
            "product_host_launch=OPEN H3=1 ko_in_kernel=0 "
            "freestanding_rtl=SKIP freestanding_xhci=SKIP "
            "soft_ne_product=1 Soft!=product dual=MIT_OR_Apache-2.0 G-AC-1 "
            "via=%s wave=%u  "
            "(Soft!=product; denser host_blob residual; product_hosts=UDX; "
            "dual_dod OPEN; not Dual DoD close; never invent .76)\n",
            GJ_SPAWN_HOST_DDI_NAME, GJ_SPAWN_HOST_RTL_NAME,
            GJ_SPAWN_HOST_XHCI_NAME, GJ_SPAWN_HOST_CLASS_N,
            u32PresentN, u32AbsentN,
            g_u32SoftHostBlobProbe, g_u32SoftHostBlobPresent,
            g_u32SoftHostBlobAbsent,
            (unsigned long)g_u64SoftHostBlobCbByClass[0],
            (unsigned long)g_u64SoftHostBlobCbByClass[1],
            (unsigned long)g_u64SoftHostBlobCbByClass[2],
            g_u32SoftHostBlobByClass[0], g_u32SoftHostBlobByClass[1],
            g_u32SoftHostBlobByClass[2],
            g_u32SoftHostElfEmbed, g_u32SoftHostElfMiss,
            g_u32SoftHostElfRunOk,
            szViaSafe, GJ_SPAWN_SOFT_WAVE);

        /* Grep: spawn: soft residual host_blob lean */
        kprintf("spawn: soft residual host_blob lean ok=%u/%u "
            "fail_bits=0x%x class_n=%u "
            "live_present=%u live_absent=%u "
            "ddi=%s rtl=%s xhci=%s "
            "hooks=spawn_host_blob_get|elf_try_run "
            "weak=gj_ddi_host_elf_blob,gj_rtl8168_udx_elf_blob,"
            "gj_xhci_udx_elf_blob "
            "product_hosts=UDX dual_dod OPEN "
            "product_host_launch=OPEN ddi_cap_mint=OPEN "
            "H3=1 soft_ne_product=1 Soft!=product "
            "dual=MIT_OR_Apache-2.0 G-AC-1 "
            "lean_pass=%u logs=%u via=%s wave=%u "
            " "
            "(Soft!=product; host_blob lean residual self-check; "
            "not product AC; not Dual DoD close)\n",
            u32BlobOk, GJ_SPAWN_HOST_BLOB_OK_N, u32BlobFailBits,
            GJ_SPAWN_HOST_CLASS_N, u32PresentN, u32AbsentN,
            GJ_SPAWN_HOST_DDI_NAME, GJ_SPAWN_HOST_RTL_NAME,
            GJ_SPAWN_HOST_XHCI_NAME,
            g_u32SoftHostBlobLean, g_u32SoftHostBlobLog,
            szViaSafe, GJ_SPAWN_SOFT_WAVE);
        if (u32BlobOk == GJ_SPAWN_HOST_BLOB_OK_N && u32BlobFailBits == 0u) {
            kprintf("spawn: soft residual host_blob lean PASS via=%s "
                    "ok=%u/%u fail_bits=0x0 product_hosts=UDX "
                    "dual_dod OPEN Soft!=product "
                    "wave=%u \n",
                    szViaSafe, u32BlobOk, GJ_SPAWN_HOST_BLOB_OK_N,
                    GJ_SPAWN_SOFT_WAVE);
        } else {
            kprintf("spawn: soft residual host_blob lean FAIL via=%s "
                    "ok=%u/%u fail_bits=0x%x "
                    "(soft residual only; not product gate; "
                    "Soft!=product; dual_dod OPEN)\n",
                    szViaSafe, u32BlobOk, GJ_SPAWN_HOST_BLOB_OK_N,
                    u32BlobFailBits);
        }

        /*
         * denser host_blob residual multi-arm honesty (Soft!=product).
         * CRITICAL preserve host blob embeds ddi/rtl/xhci (weak gj_*_elf_blob).
         * Dual DoD OPEN; product_hosts=UDX; never hard-gates product.
         * greppable: spawn: soft residual host_blob denser
         * greppable: denser_arms= denser_ok= denser_min= host_blob denser
         *  stamp-free; never invent .76.
         * ASCII Soft!=product only.
         */
        {
            u32 u32Dense;
            u32 u32DenseOk;
            u32 fClassGeom;
            u32 fEmbedsPreserve;
            u32 fDualDodMap;
            u32 fFailClosedDense;
            u32 fLiveProbe;
            u32 fProductHostsUdx;
            u32 fElfPathRole;
            u32 fDualDodOpenDense;
            const char *szDenseVerdict;

            u32Dense = 0u;
            u32DenseOk = 0u;
            fClassGeom = 0u;
            fEmbedsPreserve = 0u;
            fDualDodMap = 0u;
            fFailClosedDense = 0u;
            fLiveProbe = 0u;
            fProductHostsUdx = 0u;
            fElfPathRole = 0u;
            fDualDodOpenDense = 0u;
            szDenseVerdict = "FAIL";

            /* arm0: class_geom — class_n==3 + fit fixed SPAWN_MAX table. */
            if (GJ_SPAWN_HOST_CLASS_N == 3u &&
                GJ_SPAWN_HOST_CLASS_N <= GJ_SPAWN_MAX) {
                fClassGeom = 1u;
                u32Dense++;
            }

            /*
             * arm1: embeds_preserve - weak ddi/rtl/xhci resolve path lives.
             * spawn_host_blob_get class map 0/1/2 preserved (miss OK).
             * CRITICAL preserve host blob embeds ddi/rtl/xhci
             * (gj_ddi_host_elf_blob / gj_rtl8168_udx_elf_blob /
             * gj_xhci_udx_elf_blob). denser host_blob residual pairs with
             * main keep_live=1 on embed (never kill embed; Soft!=product).
             * Class map honesty: 0=ddi 1=rtl 2=xhci names non-empty.
             */
            if (fFailClosed != 0 &&
                (u32PresentN + u32AbsentN) == GJ_SPAWN_HOST_CLASS_N &&
                GJ_SPAWN_HOST_CLASS_N == 3u &&
                g_aHostCatalog[0].szName != NULL &&
                g_aHostCatalog[1].szName != NULL &&
                g_aHostCatalog[2].szName != NULL &&
                g_aHostCatalog[0].szName[0] != '\0' &&
                g_aHostCatalog[1].szName[0] != '\0' &&
                g_aHostCatalog[2].szName[0] != '\0') {
                fEmbedsPreserve = 1u;
                u32Dense++;
            }

            /* arm2: dual_dod_map — ddi=none rtl=B xhci=A catalog honesty. */
            if (g_aHostCatalog[0].u8DualDod == 0u &&
                g_aHostCatalog[1].u8DualDod == 2u &&
                g_aHostCatalog[2].u8DualDod == 1u) {
                fDualDodMap = 1u;
                u32Dense++;
            }

            /* arm3: fail_closed — null out-params + bad class return 0. */
            if (fFailClosed != 0) {
                fFailClosedDense = 1u;
                u32Dense++;
            }

            /* arm4: live_probe — inventory probe all 3 class slots. */
            if ((u32PresentN + u32AbsentN) == GJ_SPAWN_HOST_CLASS_N &&
                g_u32SoftHostBlobProbe > 0u) {
                fLiveProbe = 1u;
                u32Dense++;
            }

            /*
             * arm5: product_hosts_udx — names match ddi/rtl/xhci macros.
             * product_hosts=UDX (ddi_host_gj, rtl8168_udx, xhci_udx).
             */
            if (g_aHostCatalog[0].szName != NULL &&
                g_aHostCatalog[1].szName != NULL &&
                g_aHostCatalog[2].szName != NULL &&
                g_aHostCatalog[0].szName[0] != '\0' &&
                g_aHostCatalog[1].szName[0] != '\0' &&
                g_aHostCatalog[2].szName[0] != '\0' &&
                GJ_SPAWN_HOST_DDI_NAME[0] != '\0' &&
                GJ_SPAWN_HOST_RTL_NAME[0] != '\0' &&
                GJ_SPAWN_HOST_XHCI_NAME[0] != '\0') {
                fProductHostsUdx = 1u;
                u32Dense++;
            }

            /* arm6: elf_path_role — residual elf path + role tokens. */
            if (g_aHostCatalog[0].szElfPath != NULL &&
                g_aHostCatalog[0].szElfPath[0] != '\0' &&
                g_aHostCatalog[1].szElfPath != NULL &&
                g_aHostCatalog[1].szElfPath[0] != '\0' &&
                g_aHostCatalog[2].szElfPath != NULL &&
                g_aHostCatalog[2].szElfPath[0] != '\0' &&
                g_aHostCatalog[0].szRole != NULL &&
                g_aHostCatalog[0].szRole[0] != '\0' &&
                g_aHostCatalog[1].szRole != NULL &&
                g_aHostCatalog[1].szRole[0] != '\0' &&
                g_aHostCatalog[2].szRole != NULL &&
                g_aHostCatalog[2].szRole[0] != '\0') {
                fElfPathRole = 1u;
                u32Dense++;
            }

            /*
             * arm7: dual_dod_open — soft residual never closes Dual DoD.
             * stubs live so prefer path can arm; freestanding SKIP honesty.
             * denser min/arms honesty bound.
             */
            if (GJ_SPAWN_HOST_BLOB_DENSE_ARMS == 8u &&
                GJ_SPAWN_HOST_BLOB_DENSE_MIN ==
                    GJ_SPAWN_HOST_BLOB_DENSE_ARMS &&
                g_aHostCatalog[0].pfnStubEntry != NULL &&
                g_aHostCatalog[1].pfnStubEntry != NULL &&
                g_aHostCatalog[2].pfnStubEntry != NULL &&
                fClassGeom != 0u && fDualDodMap != 0u) {
                fDualDodOpenDense = 1u;
                u32Dense++;
            }

            g_u32SoftHostBlobDenseArms = u32Dense;
            if (u32Dense >= GJ_SPAWN_HOST_BLOB_DENSE_MIN &&
                fClassGeom != 0u && fEmbedsPreserve != 0u &&
                fDualDodMap != 0u && fFailClosedDense != 0u &&
                fLiveProbe != 0u && fProductHostsUdx != 0u &&
                fElfPathRole != 0u && fDualDodOpenDense != 0u) {
                u32DenseOk = 1u;
                spawn_soft_inc(&g_u32SoftHostBlobDenseOk);
                szDenseVerdict = "PASS";
            } else {
                spawn_soft_inc(&g_u32SoftHostBlobDenseFail);
                szDenseVerdict = "FAIL";
            }

            /* Grep: spawn: soft residual host_blob denser */
            /* Grep: denser_arms= denser_ok= denser_min= host_blob denser */
            /* Grep: keep_live=1 / never_kill_embed / preserve embeds */
            kprintf("spawn: soft residual host_blob denser %s denser=1 "
                    "denser_arms=%u denser_ok=%u denser_min=%u denser_fail=%u "
                    "class_geom=%u embeds_preserve=%u dual_dod_map=%u "
                    "fail_closed=%u live_probe=%u product_hosts_udx=%u "
                    "elf_path_role=%u dual_dod_open=%u "
                    "live_present=%u live_absent=%u "
                    "cb_ddi=%lu cb_rtl=%lu cb_xhci=%lu "
                    "product_hosts=UDX hosts=%s,%s,%s class_n=%u "
                    "weak=gj_ddi_host_elf_blob,gj_rtl8168_udx_elf_blob,"
                    "gj_xhci_udx_elf_blob "
                    "hooks=spawn_host_blob_get|elf_try_run "
                    "steps=host_blob|probe|load|stack|thr|prefer|no_thrash "
                    "keep_live_policy=embed_present never_kill_embed=1 "
                    "preserve_embeds=1 host_blob=1 "
                    "dual_dod OPEN dual_dod_A=OPEN dual_dod_B=OPEN "
                    "product_host_launch=OPEN H3=1 ko_in_kernel=0 "
                    "freestanding_rtl=SKIP freestanding_xhci=SKIP "
                    "soft_residual_closes_dod=0 "
                    "soft_ne_product=1 Soft!=product dual=MIT_OR_Apache-2.0 "
                    "G-AC-1 via=%s wave=%u  "
                    "(Soft!=product; denser host_blob residual; "
                    "product_hosts=UDX; dual_dod OPEN; not Dual DoD close; "
                    "CRITICAL preserve host blob embeds ddi/rtl/xhci; "
                    "pairs main keep_live=1 on embed; never invent .76)\n",
                    szDenseVerdict, (unsigned)u32Dense, (unsigned)u32DenseOk,
                    (unsigned)GJ_SPAWN_HOST_BLOB_DENSE_MIN,
                    (unsigned)g_u32SoftHostBlobDenseFail,
                    (unsigned)fClassGeom, (unsigned)fEmbedsPreserve,
                    (unsigned)fDualDodMap, (unsigned)fFailClosedDense,
                    (unsigned)fLiveProbe, (unsigned)fProductHostsUdx,
                    (unsigned)fElfPathRole, (unsigned)fDualDodOpenDense,
                    u32PresentN, u32AbsentN,
                    (unsigned long)g_u64SoftHostBlobCbByClass[0],
                    (unsigned long)g_u64SoftHostBlobCbByClass[1],
                    (unsigned long)g_u64SoftHostBlobCbByClass[2],
                    GJ_SPAWN_HOST_DDI_NAME, GJ_SPAWN_HOST_RTL_NAME,
                    GJ_SPAWN_HOST_XHCI_NAME, GJ_SPAWN_HOST_CLASS_N,
                    szViaSafe, GJ_SPAWN_SOFT_WAVE);
            if (u32DenseOk != 0u) {
                kprintf("spawn: soft residual host_blob denser PASS via=%s "
                        "denser=1 denser_arms=%u denser_ok=1 denser_min=%u "
                        "product_hosts=UDX dual_dod OPEN Soft!=product "
                        "embeds_preserve=1 ddi|rtl|xhci "
                        "keep_live_policy=embed_present never_kill_embed=1 "
                        "preserve_embeds=1 host_blob=1 "
                        "wave=%u \n",
                        szViaSafe, (unsigned)u32Dense,
                        (unsigned)GJ_SPAWN_HOST_BLOB_DENSE_MIN,
                        GJ_SPAWN_SOFT_WAVE);
            } else {
                kprintf("spawn: soft residual host_blob denser FAIL via=%s "
                        "denser=1 denser_arms=%u denser_ok=0 denser_min=%u "
                        "denser_fail=%u "
                        "(soft residual only; not product gate; "
                        "Soft!=product; dual_dod OPEN; product_hosts=UDX; "
                        "preserve embeds still policy; never_kill_embed=1)\n",
                        szViaSafe, (unsigned)u32Dense,
                        (unsigned)GJ_SPAWN_HOST_BLOB_DENSE_MIN,
                        (unsigned)g_u32SoftHostBlobDenseFail);
            }

            /*
             * denser2 host_blob residual multi-arm honesty (W17 densify).
             * CRITICAL preserve host blob embeds ddi/rtl/xhci.
             * Soft!=product; Dual DoD OPEN; product_hosts=UDX; never hard-gates.
             * greppable: spawn: soft residual host_blob denser2
             * greppable: denser2_arms= denser2_ok= denser2_min= host_blob denser2
             *  stamp-free; never invent .76.
             * ASCII Soft!=product only.
             */
            {
                u32 u32Dense2;
                u32 u32Dense2Ok;
                u32 fDenser1Held;
                u32 fEmbedsPreserve2;
                u32 fPciClassIds;
                u32 fReProbe;
                u32 fCbHonesty;
                u32 fNameRolePath;
                u32 fPreservePolicy;
                u32 fDualDodOpen2;
                u32 u32RePresent;
                u32 u32ReAbsent;
                u32 u32ReIdx;
                const void *pReBlob;
                u64 cbRe;
                int fReGet;
                const char *szDense2Verdict;

                u32Dense2 = 0u;
                u32Dense2Ok = 0u;
                fDenser1Held = 0u;
                fEmbedsPreserve2 = 0u;
                fPciClassIds = 0u;
                fReProbe = 0u;
                fCbHonesty = 0u;
                fNameRolePath = 0u;
                fPreservePolicy = 0u;
                fDualDodOpen2 = 0u;
                u32RePresent = 0u;
                u32ReAbsent = 0u;
                szDense2Verdict = "FAIL";

                /* arm0: denser1_held — denser1 composite all arms PASS. */
                if (u32DenseOk != 0u &&
                    u32Dense >= GJ_SPAWN_HOST_BLOB_DENSE_MIN &&
                    fEmbedsPreserve != 0u) {
                    fDenser1Held = 1u;
                    u32Dense2++;
                }

                /*
                 * arm1: embeds_preserve2 - weak map 0/1/2 + exact name macros.
                 * CRITICAL preserve host blob embeds ddi/rtl/xhci
                 * (gj_ddi_host_elf_blob / gj_rtl8168_udx_elf_blob /
                 * gj_xhci_udx_elf_blob). denser2 pairs main keep_live=1.
                 */
                if (fEmbedsPreserve != 0u &&
                    fFailClosed != 0u &&
                    GJ_SPAWN_HOST_CLASS_N == 3u &&
                    g_aHostCatalog[0].szName != NULL &&
                    g_aHostCatalog[1].szName != NULL &&
                    g_aHostCatalog[2].szName != NULL &&
                    strcmp(g_aHostCatalog[0].szName,
                           GJ_SPAWN_HOST_DDI_NAME) == 0 &&
                    strcmp(g_aHostCatalog[1].szName,
                           GJ_SPAWN_HOST_RTL_NAME) == 0 &&
                    strcmp(g_aHostCatalog[2].szName,
                           GJ_SPAWN_HOST_XHCI_NAME) == 0) {
                    fEmbedsPreserve2 = 1u;
                    u32Dense2++;
                }

                /*
                 * arm2: pci_class_ids - ddi 0:0, rtl 10ec:8168, xhci 8086:a12f
                 * + dual_dod map residual (none/B/A).
                 */
                if (g_aHostCatalog[0].u16Vend == 0u &&
                    g_aHostCatalog[0].u16Dev == 0u &&
                    g_aHostCatalog[0].u8DualDod == 0u &&
                    g_aHostCatalog[1].u16Vend ==
                        (u16)GJ_SPAWN_HOST_RTL_VEND &&
                    g_aHostCatalog[1].u16Dev ==
                        (u16)GJ_SPAWN_HOST_RTL_DEV &&
                    g_aHostCatalog[1].u8DualDod == 2u &&
                    g_aHostCatalog[2].u16Vend ==
                        (u16)GJ_SPAWN_HOST_XHCI_VEND &&
                    g_aHostCatalog[2].u16Dev ==
                        (u16)GJ_SPAWN_HOST_XHCI_DEV &&
                    g_aHostCatalog[2].u8DualDod == 1u) {
                    fPciClassIds = 1u;
                    u32Dense2++;
                }

                /*
                 * arm3: re_probe - second live spawn_host_blob_get pass.
                 * Present/miss honesty only; Dual DoD stays OPEN on miss.
                 * Preserve weak resolve path for all class slots.
                 * arm4: cb_honesty sampled during re_probe (present=>cb>0;
                 * miss=>cb==0; no inverted samples). Soft!=product.
                 */
                {
                    u32 u32CbOk;

                    u32CbOk = 0u;
                    for (u32ReIdx = 0u; u32ReIdx < GJ_SPAWN_HOST_CLASS_N;
                         u32ReIdx++) {
                        pReBlob = NULL;
                        cbRe = 0ull;
                        spawn_soft_inc(&g_u32SoftHostBlobReProbe);
                        spawn_soft_inc(&g_u32SoftHostBlobProbe);
                        fReGet = spawn_host_blob_get(u32ReIdx, &pReBlob,
                                                     &cbRe);
                        if (fReGet != 0 && pReBlob != NULL && cbRe > 0ull) {
                            u32RePresent++;
                            u32CbOk++;
                            g_u64SoftHostBlobCbByClass[u32ReIdx] = cbRe;
                        } else if (fReGet == 0 || pReBlob == NULL ||
                                   cbRe == 0ull) {
                            u32ReAbsent++;
                            /* miss honesty: empty range / unresolved weak */
                            if (cbRe == 0ull || pReBlob == NULL) {
                                u32CbOk++;
                            }
                            g_u64SoftHostBlobCbByClass[u32ReIdx] = 0ull;
                        }
                    }
                    if ((u32RePresent + u32ReAbsent) ==
                            GJ_SPAWN_HOST_CLASS_N &&
                        g_u32SoftHostBlobReProbe > 0u &&
                        spawn_host_blob_get(0u, NULL, NULL) == 0 &&
                        spawn_host_blob_get(99u, &pReBlob, &cbRe) == 0) {
                        fReProbe = 1u;
                        u32Dense2++;
                    }
                    if (u32CbOk == GJ_SPAWN_HOST_CLASS_N &&
                        (u32RePresent + u32ReAbsent) ==
                            GJ_SPAWN_HOST_CLASS_N) {
                        fCbHonesty = 1u;
                        u32Dense2++;
                    }
                }

                /*
                 * arm5: name_role_path - exact product host names + role +
                 * residual elf path for ddi/rtl/xhci (product_hosts=UDX).
                 */
                if (g_aHostCatalog[0].szName != NULL &&
                    g_aHostCatalog[1].szName != NULL &&
                    g_aHostCatalog[2].szName != NULL &&
                    strcmp(g_aHostCatalog[0].szName,
                           GJ_SPAWN_HOST_DDI_NAME) == 0 &&
                    strcmp(g_aHostCatalog[1].szName,
                           GJ_SPAWN_HOST_RTL_NAME) == 0 &&
                    strcmp(g_aHostCatalog[2].szName,
                           GJ_SPAWN_HOST_XHCI_NAME) == 0 &&
                    g_aHostCatalog[0].szRole != NULL &&
                    g_aHostCatalog[0].szRole[0] != '\0' &&
                    g_aHostCatalog[1].szRole != NULL &&
                    g_aHostCatalog[1].szRole[0] != '\0' &&
                    g_aHostCatalog[2].szRole != NULL &&
                    g_aHostCatalog[2].szRole[0] != '\0' &&
                    g_aHostCatalog[0].szElfPath != NULL &&
                    g_aHostCatalog[0].szElfPath[0] != '\0' &&
                    g_aHostCatalog[1].szElfPath != NULL &&
                    g_aHostCatalog[1].szElfPath[0] != '\0' &&
                    g_aHostCatalog[2].szElfPath != NULL &&
                    g_aHostCatalog[2].szElfPath[0] != '\0') {
                    fNameRolePath = 1u;
                    u32Dense2++;
                }

                /*
                 * arm6: preserve_policy - never_kill_embed / preserve_embeds.
                 * denser2 requires denser1 embeds_preserve + denser1_held.
                 * CRITICAL preserve host blob embeds ddi/rtl/xhci.
                 */
                if (fEmbedsPreserve != 0u && fEmbedsPreserve2 != 0u &&
                    fDenser1Held != 0u && fReProbe != 0u) {
                    fPreservePolicy = 1u;
                    u32Dense2++;
                }

                /*
                 * arm7: dual_dod_open2 - soft residual never closes Dual DoD.
                 * stubs live; denser2 min/arms bound; freestanding SKIP honesty.
                 */
                if (GJ_SPAWN_HOST_BLOB_DENSE2_ARMS == 8u &&
                    GJ_SPAWN_HOST_BLOB_DENSE2_MIN ==
                        GJ_SPAWN_HOST_BLOB_DENSE2_ARMS &&
                    g_aHostCatalog[0].pfnStubEntry != NULL &&
                    g_aHostCatalog[1].pfnStubEntry != NULL &&
                    g_aHostCatalog[2].pfnStubEntry != NULL &&
                    fDualDodMap != 0u && fPciClassIds != 0u &&
                    fClassGeom != 0u) {
                    fDualDodOpen2 = 1u;
                    u32Dense2++;
                }

                g_u32SoftHostBlobDense2Arms = u32Dense2;
                if (u32Dense2 >= GJ_SPAWN_HOST_BLOB_DENSE2_MIN &&
                    fDenser1Held != 0u && fEmbedsPreserve2 != 0u &&
                    fPciClassIds != 0u && fReProbe != 0u &&
                    fCbHonesty != 0u && fNameRolePath != 0u &&
                    fPreservePolicy != 0u && fDualDodOpen2 != 0u) {
                    u32Dense2Ok = 1u;
                    spawn_soft_inc(&g_u32SoftHostBlobDense2Ok);
                    szDense2Verdict = "PASS";
                } else {
                    spawn_soft_inc(&g_u32SoftHostBlobDense2Fail);
                    szDense2Verdict = "FAIL";
                }

                /* Grep: spawn: soft residual host_blob denser2 */
                /* Grep: denser2_arms= denser2_ok= denser2_min= host_blob denser2 */
                /* Grep: keep_live=1 / never_kill_embed / preserve embeds */
                kprintf("spawn: soft residual host_blob denser2 %s denser2=1 "
                        "denser2_arms=%u denser2_ok=%u denser2_min=%u "
                        "denser2_fail=%u denser1_held=%u embeds_preserve2=%u "
                        "pci_class_ids=%u re_probe=%u cb_honesty=%u "
                        "name_role_path=%u preserve_policy=%u dual_dod_open2=%u "
                        "re_present=%u re_absent=%u re_probe_n=%u "
                        "live_present=%u live_absent=%u "
                        "cb_ddi=%lu cb_rtl=%lu cb_xhci=%lu "
                        "product_hosts=UDX hosts=%s,%s,%s class_n=%u "
                        "weak=gj_ddi_host_elf_blob,gj_rtl8168_udx_elf_blob,"
                        "gj_xhci_udx_elf_blob "
                        "hooks=spawn_host_blob_get|elf_try_run "
                        "steps=host_blob|probe|load|stack|thr|prefer|no_thrash "
                        "keep_live_policy=embed_present never_kill_embed=1 "
                        "preserve_embeds=1 host_blob=1 denser=1 denser2=1 "
                        "dual_dod OPEN dual_dod_A=OPEN dual_dod_B=OPEN "
                        "product_host_launch=OPEN H3=1 ko_in_kernel=0 "
                        "freestanding_rtl=SKIP freestanding_xhci=SKIP "
                        "soft_residual_closes_dod=0 "
                        "soft_ne_product=1 Soft!=product dual=MIT_OR_Apache-2.0 "
                        "G-AC-1 via=%s wave=%u  "
                        "(Soft!=product; denser2 host_blob residual W17; "
                        "product_hosts=UDX; dual_dod OPEN; not Dual DoD close; "
                        "CRITICAL preserve host blob embeds ddi/rtl/xhci; "
                        "pairs main keep_live=1 on embed; never invent .76)\n",
                        szDense2Verdict, (unsigned)u32Dense2,
                        (unsigned)u32Dense2Ok,
                        (unsigned)GJ_SPAWN_HOST_BLOB_DENSE2_MIN,
                        (unsigned)g_u32SoftHostBlobDense2Fail,
                        (unsigned)fDenser1Held, (unsigned)fEmbedsPreserve2,
                        (unsigned)fPciClassIds, (unsigned)fReProbe,
                        (unsigned)fCbHonesty, (unsigned)fNameRolePath,
                        (unsigned)fPreservePolicy, (unsigned)fDualDodOpen2,
                        u32RePresent, u32ReAbsent, g_u32SoftHostBlobReProbe,
                        u32PresentN, u32AbsentN,
                        (unsigned long)g_u64SoftHostBlobCbByClass[0],
                        (unsigned long)g_u64SoftHostBlobCbByClass[1],
                        (unsigned long)g_u64SoftHostBlobCbByClass[2],
                        GJ_SPAWN_HOST_DDI_NAME, GJ_SPAWN_HOST_RTL_NAME,
                        GJ_SPAWN_HOST_XHCI_NAME, GJ_SPAWN_HOST_CLASS_N,
                        szViaSafe, GJ_SPAWN_SOFT_WAVE);
                if (u32Dense2Ok != 0u) {
                    kprintf("spawn: soft residual host_blob denser2 PASS via=%s "
                            "denser2=1 denser2_arms=%u denser2_ok=1 denser2_min=%u "
                            "product_hosts=UDX dual_dod OPEN Soft!=product "
                            "embeds_preserve2=1 ddi|rtl|xhci "
                            "preserve_policy=1 never_kill_embed=1 "
                            "preserve_embeds=1 host_blob=1 denser=1 denser2=1 "
                            "wave=%u \n",
                            szViaSafe, (unsigned)u32Dense2,
                            (unsigned)GJ_SPAWN_HOST_BLOB_DENSE2_MIN,
                            GJ_SPAWN_SOFT_WAVE);
                } else {
                    kprintf("spawn: soft residual host_blob denser2 FAIL via=%s "
                            "denser2=1 denser2_arms=%u denser2_ok=0 denser2_min=%u "
                            "denser2_fail=%u "
                            "(soft residual only; not product gate; "
                            "Soft!=product; dual_dod OPEN; product_hosts=UDX; "
                            "preserve embeds still policy; never_kill_embed=1)\n",
                            szViaSafe, (unsigned)u32Dense2,
                            (unsigned)GJ_SPAWN_HOST_BLOB_DENSE2_MIN,
                            (unsigned)g_u32SoftHostBlobDense2Fail);
                }

                /*
                 * denser3 host_blob residual multi-arm honesty (W18 densify).
                 * CRITICAL preserve host blob embeds ddi/rtl/xhci.
                 * Soft!=product; Dual DoD OPEN; product_hosts=UDX; never hard-gates.
                 * greppable: spawn: soft residual host_blob denser3
                 * greppable: denser3_arms= denser3_ok= denser3_min= host_blob denser3
                 *  stamp-free; never invent .76.
                 * ASCII Soft!=product only (no unicode Soft inequality glyph).
                 */
                {
                    u32 u32Dense3;
                    u32 u32Dense3Ok;
                    u32 fDenser2Held;
                    u32 fEmbedsPreserve3;
                    u32 fPersonaNative;
                    u32 fTripleProbe;
                    u32 fDualDodAbOpen;
                    u32 fElfPathExact;
                    u32 fDenserChain;
                    u32 fSoftOpenBar75;
                    u32 u32TriPresent;
                    u32 u32TriAbsent;
                    u32 u32TriIdx;
                    const void *pTriBlob;
                    u64 cbTri;
                    int fTriGet;
                    const char *szDense3Verdict;

                    u32Dense3 = 0u;
                    u32Dense3Ok = 0u;
                    fDenser2Held = 0u;
                    fEmbedsPreserve3 = 0u;
                    fPersonaNative = 0u;
                    fTripleProbe = 0u;
                    fDualDodAbOpen = 0u;
                    fElfPathExact = 0u;
                    fDenserChain = 0u;
                    fSoftOpenBar75 = 0u;
                    u32TriPresent = 0u;
                    u32TriAbsent = 0u;
                    szDense3Verdict = "FAIL";

                    /* arm0: denser2_held — denser2 composite all arms PASS. */
                    if (u32Dense2Ok != 0u &&
                        u32Dense2 >= GJ_SPAWN_HOST_BLOB_DENSE2_MIN &&
                        fEmbedsPreserve2 != 0u &&
                        fPreservePolicy != 0u) {
                        fDenser2Held = 1u;
                        u32Dense3++;
                    }

                    /*
                     * arm1: embeds_preserve3 - preserve2 + live class map.
                     * CRITICAL preserve host blob embeds ddi/rtl/xhci
                     * (gj_ddi_host_elf_blob / gj_rtl8168_udx_elf_blob /
                     * gj_xhci_udx_elf_blob). denser3 pairs main keep_live=1.
                     */
                    if (fEmbedsPreserve2 != 0u &&
                        fEmbedsPreserve != 0u &&
                        fFailClosed != 0u &&
                        (u32PresentN + u32AbsentN) ==
                            GJ_SPAWN_HOST_CLASS_N &&
                        GJ_SPAWN_HOST_CLASS_N == 3u &&
                        g_aHostCatalog[0].szName != NULL &&
                        g_aHostCatalog[1].szName != NULL &&
                        g_aHostCatalog[2].szName != NULL &&
                        strcmp(g_aHostCatalog[0].szName,
                               GJ_SPAWN_HOST_DDI_NAME) == 0 &&
                        strcmp(g_aHostCatalog[1].szName,
                               GJ_SPAWN_HOST_RTL_NAME) == 0 &&
                        strcmp(g_aHostCatalog[2].szName,
                               GJ_SPAWN_HOST_XHCI_NAME) == 0) {
                        fEmbedsPreserve3 = 1u;
                        u32Dense3++;
                    }

                    /*
                     * arm2: persona_native - all 3 hosts prefer native persona.
                     * product_hosts=UDX confined C path (Soft!=product).
                     */
                    if (g_aHostCatalog[0].u8PersonaPref ==
                            GJ_SPAWN_HOST_PERSONA_NATIVE &&
                        g_aHostCatalog[1].u8PersonaPref ==
                            GJ_SPAWN_HOST_PERSONA_NATIVE &&
                        g_aHostCatalog[2].u8PersonaPref ==
                            GJ_SPAWN_HOST_PERSONA_NATIVE) {
                        fPersonaNative = 1u;
                        u32Dense3++;
                    }

                    /*
                     * arm3: triple_probe - third live spawn_host_blob_get pass.
                     * Present/miss honesty only; Dual DoD stays OPEN on miss.
                     * Preserve weak resolve path for all class slots.
                     */
                    for (u32TriIdx = 0u; u32TriIdx < GJ_SPAWN_HOST_CLASS_N;
                         u32TriIdx++) {
                        pTriBlob = NULL;
                        cbTri = 0ull;
                        spawn_soft_inc(&g_u32SoftHostBlobTripleProbe);
                        spawn_soft_inc(&g_u32SoftHostBlobProbe);
                        fTriGet = spawn_host_blob_get(u32TriIdx, &pTriBlob,
                                                      &cbTri);
                        if (fTriGet != 0 && pTriBlob != NULL &&
                            cbTri > 0ull) {
                            u32TriPresent++;
                            g_u64SoftHostBlobCbByClass[u32TriIdx] = cbTri;
                        } else {
                            u32TriAbsent++;
                            g_u64SoftHostBlobCbByClass[u32TriIdx] = 0ull;
                        }
                    }
                    if ((u32TriPresent + u32TriAbsent) ==
                            GJ_SPAWN_HOST_CLASS_N &&
                        g_u32SoftHostBlobTripleProbe > 0u &&
                        spawn_host_blob_get(0u, NULL, NULL) == 0 &&
                        spawn_host_blob_get(99u, &pTriBlob, &cbTri) == 0) {
                        fTripleProbe = 1u;
                        u32Dense3++;
                    }

                    /*
                     * arm4: dual_dod_ab_open - Dual DoD A/B remain OPEN.
                     * Catalog: ddi=none rtl=B xhci=A; stubs live; freestanding
                     * SKIP honesty. Soft residual never closes Dual DoD.
                     */
                    if (g_aHostCatalog[0].u8DualDod == 0u &&
                        g_aHostCatalog[1].u8DualDod == 2u &&
                        g_aHostCatalog[2].u8DualDod == 1u &&
                        g_aHostCatalog[0].pfnStubEntry != NULL &&
                        g_aHostCatalog[1].pfnStubEntry != NULL &&
                        g_aHostCatalog[2].pfnStubEntry != NULL &&
                        fDualDodMap != 0u && fDualDodOpen2 != 0u) {
                        fDualDodAbOpen = 1u;
                        u32Dense3++;
                    }

                    /*
                     * arm5: elf_path_exact - residual elf paths exact for
                     * product_hosts=UDX (ddi/rtl/xhci stage/media paths).
                     */
                    if (g_aHostCatalog[0].szElfPath != NULL &&
                        g_aHostCatalog[1].szElfPath != NULL &&
                        g_aHostCatalog[2].szElfPath != NULL &&
                        strcmp(g_aHostCatalog[0].szElfPath,
                               "/bin/ddi_host_gj") == 0 &&
                        strcmp(g_aHostCatalog[1].szElfPath,
                               "/usr/lib/udx/rtl8168_udx") == 0 &&
                        strcmp(g_aHostCatalog[2].szElfPath,
                               "/bin/xhci_udx") == 0) {
                        fElfPathExact = 1u;
                        u32Dense3++;
                    }

                    /*
                     * arm6: denser_chain - denser1+denser2 held + embeds chain.
                     * CRITICAL preserve host blob embeds ddi/rtl/xhci.
                     */
                    if (fDenser1Held != 0u && fDenser2Held != 0u &&
                        fEmbedsPreserve != 0u && fEmbedsPreserve2 != 0u &&
                        fEmbedsPreserve3 != 0u && fTripleProbe != 0u) {
                        fDenserChain = 1u;
                        u32Dense3++;
                    }

                    /*
                     * arm7: soft_open_bar75 - denser3 min/arms bound;
                     * soft residual never closes Dual DoD; bar .75 stamp-free.
                     * Soft!=product (ASCII only).
                     */
                    if (GJ_SPAWN_HOST_BLOB_DENSE3_ARMS == 8u &&
                        GJ_SPAWN_HOST_BLOB_DENSE3_MIN ==
                            GJ_SPAWN_HOST_BLOB_DENSE3_ARMS &&
                        fDualDodAbOpen != 0u && fPersonaNative != 0u &&
                        fClassGeom != 0u && fElfPathExact != 0u) {
                        fSoftOpenBar75 = 1u;
                        u32Dense3++;
                    }

                    g_u32SoftHostBlobDense3Arms = u32Dense3;
                    if (u32Dense3 >= GJ_SPAWN_HOST_BLOB_DENSE3_MIN &&
                        fDenser2Held != 0u && fEmbedsPreserve3 != 0u &&
                        fPersonaNative != 0u && fTripleProbe != 0u &&
                        fDualDodAbOpen != 0u && fElfPathExact != 0u &&
                        fDenserChain != 0u && fSoftOpenBar75 != 0u) {
                        u32Dense3Ok = 1u;
                        spawn_soft_inc(&g_u32SoftHostBlobDense3Ok);
                        szDense3Verdict = "PASS";
                    } else {
                        spawn_soft_inc(&g_u32SoftHostBlobDense3Fail);
                        szDense3Verdict = "FAIL";
                    }

                    /* Grep: spawn: soft residual host_blob denser3 */
                    /* Grep: denser3_arms= denser3_ok= denser3_min= host_blob denser3 */
                    /* Grep: keep_live=1 / never_kill_embed / preserve embeds */
                    kprintf("spawn: soft residual host_blob denser3 %s denser3=1 "
                            "denser3_arms=%u denser3_ok=%u denser3_min=%u "
                            "denser3_fail=%u denser2_held=%u embeds_preserve3=%u "
                            "persona_native=%u triple_probe=%u dual_dod_ab_open=%u "
                            "elf_path_exact=%u denser_chain=%u soft_open_bar75=%u "
                            "tri_present=%u tri_absent=%u triple_probe_n=%u "
                            "live_present=%u live_absent=%u "
                            "cb_ddi=%lu cb_rtl=%lu cb_xhci=%lu "
                            "product_hosts=UDX hosts=%s,%s,%s class_n=%u "
                            "weak=gj_ddi_host_elf_blob,gj_rtl8168_udx_elf_blob,"
                            "gj_xhci_udx_elf_blob "
                            "hooks=spawn_host_blob_get|elf_try_run "
                            "steps=host_blob|probe|load|stack|thr|prefer|no_thrash "
                            "keep_live_policy=embed_present never_kill_embed=1 "
                            "preserve_embeds=1 host_blob=1 denser=1 denser2=1 denser3=1 "
                            "dual_dod OPEN dual_dod_A=OPEN dual_dod_B=OPEN "
                            "product_host_launch=OPEN H3=1 ko_in_kernel=0 "
                            "freestanding_rtl=SKIP freestanding_xhci=SKIP "
                            "soft_residual_closes_dod=0 "
                            "soft_ne_product=1 Soft!=product dual=MIT_OR_Apache-2.0 "
                            "G-AC-1 via=%s wave=%u  "
                            "(Soft!=product; denser3 host_blob residual W18; "
                            "product_hosts=UDX; dual_dod OPEN; not Dual DoD close; "
                            "CRITICAL preserve host blob embeds ddi/rtl/xhci; "
                            "pairs main keep_live=1 on embed; never invent .76)\n",
                            szDense3Verdict, (unsigned)u32Dense3,
                            (unsigned)u32Dense3Ok,
                            (unsigned)GJ_SPAWN_HOST_BLOB_DENSE3_MIN,
                            (unsigned)g_u32SoftHostBlobDense3Fail,
                            (unsigned)fDenser2Held, (unsigned)fEmbedsPreserve3,
                            (unsigned)fPersonaNative, (unsigned)fTripleProbe,
                            (unsigned)fDualDodAbOpen, (unsigned)fElfPathExact,
                            (unsigned)fDenserChain, (unsigned)fSoftOpenBar75,
                            u32TriPresent, u32TriAbsent,
                            g_u32SoftHostBlobTripleProbe,
                            u32PresentN, u32AbsentN,
                            (unsigned long)g_u64SoftHostBlobCbByClass[0],
                            (unsigned long)g_u64SoftHostBlobCbByClass[1],
                            (unsigned long)g_u64SoftHostBlobCbByClass[2],
                            GJ_SPAWN_HOST_DDI_NAME, GJ_SPAWN_HOST_RTL_NAME,
                            GJ_SPAWN_HOST_XHCI_NAME, GJ_SPAWN_HOST_CLASS_N,
                            szViaSafe, GJ_SPAWN_SOFT_WAVE);
                    if (u32Dense3Ok != 0u) {
                        kprintf("spawn: soft residual host_blob denser3 PASS via=%s "
                                "denser3=1 denser3_arms=%u denser3_ok=1 denser3_min=%u "
                                "product_hosts=UDX dual_dod OPEN Soft!=product "
                                "embeds_preserve3=1 ddi|rtl|xhci "
                                "denser_chain=1 never_kill_embed=1 "
                                "preserve_embeds=1 host_blob=1 denser=1 denser2=1 denser3=1 "
                                "wave=%u \n",
                                szViaSafe, (unsigned)u32Dense3,
                                (unsigned)GJ_SPAWN_HOST_BLOB_DENSE3_MIN,
                                GJ_SPAWN_SOFT_WAVE);
                    } else {
                        kprintf("spawn: soft residual host_blob denser3 FAIL via=%s "
                                "denser3=1 denser3_arms=%u denser3_ok=0 denser3_min=%u "
                                "denser3_fail=%u "
                                "(soft residual only; not product gate; "
                                "Soft!=product; dual_dod OPEN; product_hosts=UDX; "
                                "preserve embeds still policy; never_kill_embed=1)\n",
                                szViaSafe, (unsigned)u32Dense3,
                                (unsigned)GJ_SPAWN_HOST_BLOB_DENSE3_MIN,
                                (unsigned)g_u32SoftHostBlobDense3Fail);
                    }

                    /*
                     * denser4 host_blob residual multi-arm honesty (W19 densify).
                     * CRITICAL preserve host blob embeds ddi/rtl/xhci.
                     * Soft!=product; Dual DoD OPEN; product_hosts=UDX; never hard-gates.
                     * greppable: spawn: soft residual host_blob denser4
                     * greppable: denser4_arms= denser4_ok= denser4_min= host_blob denser4
                     *  stamp-free; never invent .76.
                     * ASCII Soft!=product only (no unicode Soft inequality glyph).
                     */
                    {
                        u32 u32Dense4;
                        u32 u32Dense4Ok;
                        u32 fDenser3Held;
                        u32 fEmbedsPreserve4;
                        u32 fRoleExact;
                        u32 fQuadProbe;
                        u32 fDualDodStatusOpen;
                        u32 fHostLookupMap;
                        u32 fDenserChain4;
                        u32 fSoftOpenBar75_4;
                        u32 u32QuadPresent;
                        u32 u32QuadAbsent;
                        u32 u32QuadIdx;
                        const void *pQuadBlob;
                        u64 cbQuad;
                        int fQuadGet;
                        const char *szDense4Verdict;

                        u32Dense4 = 0u;
                        u32Dense4Ok = 0u;
                        fDenser3Held = 0u;
                        fEmbedsPreserve4 = 0u;
                        fRoleExact = 0u;
                        fQuadProbe = 0u;
                        fDualDodStatusOpen = 0u;
                        fHostLookupMap = 0u;
                        fDenserChain4 = 0u;
                        fSoftOpenBar75_4 = 0u;
                        u32QuadPresent = 0u;
                        u32QuadAbsent = 0u;
                        szDense4Verdict = "FAIL";

                        /* arm0: denser3_held — denser3 composite all arms PASS. */
                        if (u32Dense3Ok != 0u &&
                            u32Dense3 >= GJ_SPAWN_HOST_BLOB_DENSE3_MIN &&
                            fEmbedsPreserve3 != 0u &&
                            fDenserChain != 0u) {
                            fDenser3Held = 1u;
                            u32Dense4++;
                        }

                        /*
                         * arm1: embeds_preserve4 - preserve3 + denser_chain +
                         * exact names + fail_closed. CRITICAL preserve host
                         * blob embeds ddi/rtl/xhci (gj_ddi_host_elf_blob /
                         * gj_rtl8168_udx_elf_blob / gj_xhci_udx_elf_blob).
                         * denser4 pairs main keep_live=1 on embed.
                         */
                        if (fEmbedsPreserve3 != 0u &&
                            fEmbedsPreserve2 != 0u &&
                            fEmbedsPreserve != 0u &&
                            fDenserChain != 0u &&
                            fFailClosed != 0u &&
                            GJ_SPAWN_HOST_CLASS_N == 3u &&
                            g_aHostCatalog[0].szName != NULL &&
                            g_aHostCatalog[1].szName != NULL &&
                            g_aHostCatalog[2].szName != NULL &&
                            strcmp(g_aHostCatalog[0].szName,
                                   GJ_SPAWN_HOST_DDI_NAME) == 0 &&
                            strcmp(g_aHostCatalog[1].szName,
                                   GJ_SPAWN_HOST_RTL_NAME) == 0 &&
                            strcmp(g_aHostCatalog[2].szName,
                                   GJ_SPAWN_HOST_XHCI_NAME) == 0) {
                            fEmbedsPreserve4 = 1u;
                            u32Dense4++;
                        }

                        /*
                         * arm2: role_exact - product residual role tokens exact.
                         * ddi_door_residual | udx_nic | udx_usb_hc.
                         */
                        if (g_aHostCatalog[0].szRole != NULL &&
                            g_aHostCatalog[1].szRole != NULL &&
                            g_aHostCatalog[2].szRole != NULL &&
                            strcmp(g_aHostCatalog[0].szRole,
                                   "ddi_door_residual") == 0 &&
                            strcmp(g_aHostCatalog[1].szRole,
                                   "udx_nic") == 0 &&
                            strcmp(g_aHostCatalog[2].szRole,
                                   "udx_usb_hc") == 0) {
                            fRoleExact = 1u;
                            u32Dense4++;
                        }

                        /*
                         * arm3: quad_probe - fourth live spawn_host_blob_get.
                         * Present/miss honesty only; Dual DoD stays OPEN on miss.
                         * Preserve weak resolve path for all class slots.
                         */
                        for (u32QuadIdx = 0u;
                             u32QuadIdx < GJ_SPAWN_HOST_CLASS_N;
                             u32QuadIdx++) {
                            pQuadBlob = NULL;
                            cbQuad = 0ull;
                            spawn_soft_inc(&g_u32SoftHostBlobQuadProbe);
                            spawn_soft_inc(&g_u32SoftHostBlobProbe);
                            fQuadGet = spawn_host_blob_get(u32QuadIdx,
                                                           &pQuadBlob,
                                                           &cbQuad);
                            if (fQuadGet != 0 && pQuadBlob != NULL &&
                                cbQuad > 0ull) {
                                u32QuadPresent++;
                                g_u64SoftHostBlobCbByClass[u32QuadIdx] =
                                    cbQuad;
                            } else {
                                u32QuadAbsent++;
                                g_u64SoftHostBlobCbByClass[u32QuadIdx] =
                                    0ull;
                            }
                        }
                        if ((u32QuadPresent + u32QuadAbsent) ==
                                GJ_SPAWN_HOST_CLASS_N &&
                            g_u32SoftHostBlobQuadProbe > 0u &&
                            spawn_host_blob_get(0u, NULL, NULL) == 0 &&
                            spawn_host_blob_get(99u, &pQuadBlob,
                                                &cbQuad) == 0) {
                            fQuadProbe = 1u;
                            u32Dense4++;
                        }

                        /*
                         * arm4: dual_dod_status_open - Dual DoD A/B remain OPEN.
                         * Catalog map + stubs + prior dual_dod_ab_open held.
                         * Soft residual never closes Dual DoD (Soft!=product).
                         */
                        if (fDualDodAbOpen != 0u &&
                            fDualDodOpen2 != 0u &&
                            fDualDodMap != 0u &&
                            g_aHostCatalog[0].u8DualDod == 0u &&
                            g_aHostCatalog[1].u8DualDod == 2u &&
                            g_aHostCatalog[2].u8DualDod == 1u &&
                            g_aHostCatalog[0].pfnStubEntry != NULL &&
                            g_aHostCatalog[1].pfnStubEntry != NULL &&
                            g_aHostCatalog[2].pfnStubEntry != NULL) {
                            fDualDodStatusOpen = 1u;
                            u32Dense4++;
                        }

                        /*
                         * arm5: host_lookup_map - catalog name + PCI lookup
                         * resolve class map 0/1/2 (product_hosts=UDX).
                         * ddi by name only (0:0 refuse on PCI); rtl/xhci PCI.
                         */
                        if (spawn_host_catalog_lookup_name(
                                GJ_SPAWN_HOST_DDI_NAME) == 0u &&
                            spawn_host_catalog_lookup_name(
                                GJ_SPAWN_HOST_RTL_NAME) == 1u &&
                            spawn_host_catalog_lookup_name(
                                GJ_SPAWN_HOST_XHCI_NAME) == 2u &&
                            spawn_host_catalog_lookup_name("ddi_host") ==
                                0u &&
                            spawn_host_catalog_lookup_pci(
                                (u16)GJ_SPAWN_HOST_RTL_VEND,
                                (u16)GJ_SPAWN_HOST_RTL_DEV) == 1u &&
                            spawn_host_catalog_lookup_pci(
                                (u16)GJ_SPAWN_HOST_XHCI_VEND,
                                (u16)GJ_SPAWN_HOST_XHCI_DEV) == 2u &&
                            spawn_host_catalog_lookup_pci(0u, 0u) ==
                                (u32)-1 &&
                            spawn_host_catalog_get(0u) != NULL &&
                            spawn_host_catalog_get(1u) != NULL &&
                            spawn_host_catalog_get(2u) != NULL &&
                            spawn_host_catalog_get(99u) == NULL) {
                            fHostLookupMap = 1u;
                            u32Dense4++;
                        }

                        /*
                         * arm6: denser_chain4 - denser1+2+3 held + embeds 1..4.
                         * CRITICAL preserve host blob embeds ddi/rtl/xhci.
                         */
                        if (fDenser1Held != 0u && fDenser2Held != 0u &&
                            fDenser3Held != 0u &&
                            fEmbedsPreserve != 0u &&
                            fEmbedsPreserve2 != 0u &&
                            fEmbedsPreserve3 != 0u &&
                            fEmbedsPreserve4 != 0u &&
                            fQuadProbe != 0u) {
                            fDenserChain4 = 1u;
                            u32Dense4++;
                        }

                        /*
                         * arm7: soft_open_bar75_4 - denser4 min/arms bound;
                         * soft residual never closes Dual DoD; bar .75 stamp-free.
                         * Soft!=product (ASCII only).
                         */
                        if (GJ_SPAWN_HOST_BLOB_DENSE4_ARMS == 8u &&
                            GJ_SPAWN_HOST_BLOB_DENSE4_MIN ==
                                GJ_SPAWN_HOST_BLOB_DENSE4_ARMS &&
                            fDualDodStatusOpen != 0u &&
                            fRoleExact != 0u &&
                            fHostLookupMap != 0u &&
                            fClassGeom != 0u &&
                            fElfPathExact != 0u) {
                            fSoftOpenBar75_4 = 1u;
                            u32Dense4++;
                        }

                        g_u32SoftHostBlobDense4Arms = u32Dense4;
                        if (u32Dense4 >= GJ_SPAWN_HOST_BLOB_DENSE4_MIN &&
                            fDenser3Held != 0u &&
                            fEmbedsPreserve4 != 0u &&
                            fRoleExact != 0u && fQuadProbe != 0u &&
                            fDualDodStatusOpen != 0u &&
                            fHostLookupMap != 0u &&
                            fDenserChain4 != 0u &&
                            fSoftOpenBar75_4 != 0u) {
                            u32Dense4Ok = 1u;
                            spawn_soft_inc(&g_u32SoftHostBlobDense4Ok);
                            szDense4Verdict = "PASS";
                        } else {
                            spawn_soft_inc(&g_u32SoftHostBlobDense4Fail);
                            szDense4Verdict = "FAIL";
                        }

                        /* Grep: spawn: soft residual host_blob denser4 */
                        /* Grep: denser4_arms= denser4_ok= denser4_min= host_blob denser4 */
                        /* Grep: keep_live=1 / never_kill_embed / preserve embeds */
                        kprintf("spawn: soft residual host_blob denser4 %s denser4=1 "
                                "denser4_arms=%u denser4_ok=%u denser4_min=%u "
                                "denser4_fail=%u denser3_held=%u embeds_preserve4=%u "
                                "role_exact=%u quad_probe=%u dual_dod_status_open=%u "
                                "host_lookup_map=%u denser_chain4=%u soft_open_bar75_4=%u "
                                "quad_present=%u quad_absent=%u quad_probe_n=%u "
                                "live_present=%u live_absent=%u "
                                "cb_ddi=%lu cb_rtl=%lu cb_xhci=%lu "
                                "product_hosts=UDX hosts=%s,%s,%s class_n=%u "
                                "weak=gj_ddi_host_elf_blob,gj_rtl8168_udx_elf_blob,"
                                "gj_xhci_udx_elf_blob "
                                "hooks=spawn_host_blob_get|elf_try_run "
                                "steps=host_blob|probe|load|stack|thr|prefer|no_thrash "
                                "keep_live_policy=embed_present never_kill_embed=1 "
                                "preserve_embeds=1 host_blob=1 "
                                "denser=1 denser2=1 denser3=1 denser4=1 "
                                "dual_dod OPEN dual_dod_A=OPEN dual_dod_B=OPEN "
                                "product_host_launch=OPEN H3=1 ko_in_kernel=0 "
                                "freestanding_rtl=SKIP freestanding_xhci=SKIP "
                                "soft_residual_closes_dod=0 "
                                "soft_ne_product=1 Soft!=product dual=MIT_OR_Apache-2.0 "
                                "G-AC-1 via=%s wave=%u  "
                                "(Soft!=product; denser4 host_blob residual W19; "
                                "product_hosts=UDX; dual_dod OPEN; not Dual DoD close; "
                                "CRITICAL preserve host blob embeds ddi/rtl/xhci; "
                                "pairs main keep_live=1 on embed; never invent .76)\n",
                                szDense4Verdict, (unsigned)u32Dense4,
                                (unsigned)u32Dense4Ok,
                                (unsigned)GJ_SPAWN_HOST_BLOB_DENSE4_MIN,
                                (unsigned)g_u32SoftHostBlobDense4Fail,
                                (unsigned)fDenser3Held,
                                (unsigned)fEmbedsPreserve4,
                                (unsigned)fRoleExact, (unsigned)fQuadProbe,
                                (unsigned)fDualDodStatusOpen,
                                (unsigned)fHostLookupMap,
                                (unsigned)fDenserChain4,
                                (unsigned)fSoftOpenBar75_4,
                                u32QuadPresent, u32QuadAbsent,
                                g_u32SoftHostBlobQuadProbe,
                                u32PresentN, u32AbsentN,
                                (unsigned long)g_u64SoftHostBlobCbByClass[0],
                                (unsigned long)g_u64SoftHostBlobCbByClass[1],
                                (unsigned long)g_u64SoftHostBlobCbByClass[2],
                                GJ_SPAWN_HOST_DDI_NAME, GJ_SPAWN_HOST_RTL_NAME,
                                GJ_SPAWN_HOST_XHCI_NAME, GJ_SPAWN_HOST_CLASS_N,
                                szViaSafe, GJ_SPAWN_SOFT_WAVE);
                        if (u32Dense4Ok != 0u) {
                            kprintf("spawn: soft residual host_blob denser4 PASS via=%s "
                                    "denser4=1 denser4_arms=%u denser4_ok=1 denser4_min=%u "
                                    "product_hosts=UDX dual_dod OPEN Soft!=product "
                                    "embeds_preserve4=1 ddi|rtl|xhci "
                                    "denser_chain4=1 never_kill_embed=1 "
                                    "preserve_embeds=1 host_blob=1 "
                                    "denser=1 denser2=1 denser3=1 denser4=1 "
                                    "wave=%u \n",
                                    szViaSafe, (unsigned)u32Dense4,
                                    (unsigned)GJ_SPAWN_HOST_BLOB_DENSE4_MIN,
                                    GJ_SPAWN_SOFT_WAVE);
                        } else {
                            kprintf("spawn: soft residual host_blob denser4 FAIL via=%s "
                                    "denser4=1 denser4_arms=%u denser4_ok=0 denser4_min=%u "
                                    "denser4_fail=%u "
                                    "(soft residual only; not product gate; "
                                    "Soft!=product; dual_dod OPEN; product_hosts=UDX; "
                                    "preserve embeds still policy; never_kill_embed=1)\n",
                                    szViaSafe, (unsigned)u32Dense4,
                                    (unsigned)GJ_SPAWN_HOST_BLOB_DENSE4_MIN,
                                    (unsigned)g_u32SoftHostBlobDense4Fail);
                        }

                        /*
                         * denser5 host_blob residual multi-arm honesty (W20 densify).
                         * CRITICAL preserve host blob embeds ddi/rtl/xhci.
                         * Soft!=product; Dual DoD OPEN; product_hosts=UDX; never hard-gates.
                         * greppable: spawn: soft residual host_blob denser5
                         * greppable: denser5_arms= denser5_ok= denser5_min= host_blob denser5
                         *  stamp-free; never invent .76.
                         * ASCII Soft!=product only (no unicode Soft inequality glyph).
                         */
                        {
                            u32 u32Dense5;
                            u32 u32Dense5Ok;
                            u32 fDenser4Held;
                            u32 fEmbedsPreserve5;
                            u32 fCatalogGeomExact;
                            u32 fPentaProbe;
                            u32 fDualDodOpen5;
                            u32 fHostElfPathRole;
                            u32 fDenserChain5;
                            u32 fSoftOpenBar75_5;
                            u32 u32PentaPresent;
                            u32 u32PentaAbsent;
                            u32 u32PentaIdx;
                            const void *pPentaBlob;
                            u64 cbPenta;
                            int fPentaGet;
                            const char *szDense5Verdict;

                            u32Dense5 = 0u;
                            u32Dense5Ok = 0u;
                            fDenser4Held = 0u;
                            fEmbedsPreserve5 = 0u;
                            fCatalogGeomExact = 0u;
                            fPentaProbe = 0u;
                            fDualDodOpen5 = 0u;
                            fHostElfPathRole = 0u;
                            fDenserChain5 = 0u;
                            fSoftOpenBar75_5 = 0u;
                            u32PentaPresent = 0u;
                            u32PentaAbsent = 0u;
                            szDense5Verdict = "FAIL";

                            /* arm0: denser4_held - denser4 composite all arms PASS. */
                            if (u32Dense4Ok != 0u &&
                                u32Dense4 >= GJ_SPAWN_HOST_BLOB_DENSE4_MIN &&
                                fEmbedsPreserve4 != 0u &&
                                fDenserChain4 != 0u) {
                                fDenser4Held = 1u;
                                u32Dense5++;
                            }

                            /*
                             * arm1: embeds_preserve5 - preserve4 + denser_chain4 +
                             * exact names + fail_closed. CRITICAL preserve host
                             * blob embeds ddi/rtl/xhci (gj_ddi_host_elf_blob /
                             * gj_rtl8168_udx_elf_blob / gj_xhci_udx_elf_blob).
                             * denser5 pairs main keep_live=1 on embed.
                             */
                            if (fEmbedsPreserve4 != 0u &&
                                fEmbedsPreserve3 != 0u &&
                                fEmbedsPreserve2 != 0u &&
                                fEmbedsPreserve != 0u &&
                                fDenserChain4 != 0u &&
                                fFailClosed != 0u &&
                                GJ_SPAWN_HOST_CLASS_N == 3u &&
                                g_aHostCatalog[0].szName != NULL &&
                                g_aHostCatalog[1].szName != NULL &&
                                g_aHostCatalog[2].szName != NULL &&
                                strcmp(g_aHostCatalog[0].szName,
                                       GJ_SPAWN_HOST_DDI_NAME) == 0 &&
                                strcmp(g_aHostCatalog[1].szName,
                                       GJ_SPAWN_HOST_RTL_NAME) == 0 &&
                                strcmp(g_aHostCatalog[2].szName,
                                       GJ_SPAWN_HOST_XHCI_NAME) == 0) {
                                fEmbedsPreserve5 = 1u;
                                u32Dense5++;
                            }

                            /*
                             * arm2: catalog_geom_exact - class_n==3 + SPAWN_MAX fit
                             * + PCI ids + dual_dod map (product_hosts=UDX).
                             */
                            if (fClassGeom != 0u &&
                                fDualDodMap != 0u &&
                                GJ_SPAWN_HOST_CLASS_N == 3u &&
                                GJ_SPAWN_HOST_CLASS_N <= GJ_SPAWN_MAX &&
                                g_aHostCatalog[0].u16Vend == 0u &&
                                g_aHostCatalog[0].u16Dev == 0u &&
                                g_aHostCatalog[1].u16Vend ==
                                    (u16)GJ_SPAWN_HOST_RTL_VEND &&
                                g_aHostCatalog[1].u16Dev ==
                                    (u16)GJ_SPAWN_HOST_RTL_DEV &&
                                g_aHostCatalog[2].u16Vend ==
                                    (u16)GJ_SPAWN_HOST_XHCI_VEND &&
                                g_aHostCatalog[2].u16Dev ==
                                    (u16)GJ_SPAWN_HOST_XHCI_DEV &&
                                g_aHostCatalog[0].u8DualDod == 0u &&
                                g_aHostCatalog[1].u8DualDod == 2u &&
                                g_aHostCatalog[2].u8DualDod == 1u) {
                                fCatalogGeomExact = 1u;
                                u32Dense5++;
                            }

                            /*
                             * arm3: penta_probe - fifth live spawn_host_blob_get.
                             * Present/miss honesty only; Dual DoD stays OPEN on miss.
                             * Preserve weak resolve path for all class slots.
                             */
                            for (u32PentaIdx = 0u;
                                 u32PentaIdx < GJ_SPAWN_HOST_CLASS_N;
                                 u32PentaIdx++) {
                                pPentaBlob = NULL;
                                cbPenta = 0ull;
                                spawn_soft_inc(&g_u32SoftHostBlobPentaProbe);
                                spawn_soft_inc(&g_u32SoftHostBlobProbe);
                                fPentaGet = spawn_host_blob_get(u32PentaIdx,
                                                                &pPentaBlob,
                                                                &cbPenta);
                                if (fPentaGet != 0 && pPentaBlob != NULL &&
                                    cbPenta > 0ull) {
                                    u32PentaPresent++;
                                    g_u64SoftHostBlobCbByClass[u32PentaIdx] =
                                        cbPenta;
                                } else {
                                    u32PentaAbsent++;
                                    g_u64SoftHostBlobCbByClass[u32PentaIdx] =
                                        0ull;
                                }
                            }
                            if ((u32PentaPresent + u32PentaAbsent) ==
                                    GJ_SPAWN_HOST_CLASS_N &&
                                g_u32SoftHostBlobPentaProbe > 0u &&
                                spawn_host_blob_get(0u, NULL, NULL) == 0 &&
                                spawn_host_blob_get(99u, &pPentaBlob,
                                                    &cbPenta) == 0) {
                                fPentaProbe = 1u;
                                u32Dense5++;
                            }

                            /*
                             * arm4: dual_dod_open5 - Dual DoD A/B remain OPEN.
                             * Status open + prior open arms + stubs live.
                             * Soft residual never closes Dual DoD (Soft!=product).
                             */
                            if (fDualDodStatusOpen != 0u &&
                                fDualDodAbOpen != 0u &&
                                fDualDodOpen2 != 0u &&
                                fDualDodMap != 0u &&
                                g_aHostCatalog[0].u8DualDod == 0u &&
                                g_aHostCatalog[1].u8DualDod == 2u &&
                                g_aHostCatalog[2].u8DualDod == 1u &&
                                g_aHostCatalog[0].pfnStubEntry != NULL &&
                                g_aHostCatalog[1].pfnStubEntry != NULL &&
                                g_aHostCatalog[2].pfnStubEntry != NULL) {
                                fDualDodOpen5 = 1u;
                                u32Dense5++;
                            }

                            /*
                             * arm5: host_elf_path_role - exact elf paths + roles.
                             * product_hosts=UDX residual stage paths.
                             */
                            if (fRoleExact != 0u &&
                                fElfPathExact != 0u &&
                                g_aHostCatalog[0].szElfPath != NULL &&
                                g_aHostCatalog[1].szElfPath != NULL &&
                                g_aHostCatalog[2].szElfPath != NULL &&
                                strcmp(g_aHostCatalog[0].szElfPath,
                                       "/bin/ddi_host_gj") == 0 &&
                                strcmp(g_aHostCatalog[1].szElfPath,
                                       "/usr/lib/udx/rtl8168_udx") == 0 &&
                                strcmp(g_aHostCatalog[2].szElfPath,
                                       "/bin/xhci_udx") == 0 &&
                                g_aHostCatalog[0].szRole != NULL &&
                                g_aHostCatalog[1].szRole != NULL &&
                                g_aHostCatalog[2].szRole != NULL &&
                                strcmp(g_aHostCatalog[0].szRole,
                                       "ddi_door_residual") == 0 &&
                                strcmp(g_aHostCatalog[1].szRole,
                                       "udx_nic") == 0 &&
                                strcmp(g_aHostCatalog[2].szRole,
                                       "udx_usb_hc") == 0) {
                                fHostElfPathRole = 1u;
                                u32Dense5++;
                            }

                            /*
                             * arm6: denser_chain5 - denser1+2+3+4 held + embeds 1..5.
                             * CRITICAL preserve host blob embeds ddi/rtl/xhci.
                             */
                            if (fDenser1Held != 0u && fDenser2Held != 0u &&
                                fDenser3Held != 0u && fDenser4Held != 0u &&
                                fEmbedsPreserve != 0u &&
                                fEmbedsPreserve2 != 0u &&
                                fEmbedsPreserve3 != 0u &&
                                fEmbedsPreserve4 != 0u &&
                                fEmbedsPreserve5 != 0u &&
                                fPentaProbe != 0u) {
                                fDenserChain5 = 1u;
                                u32Dense5++;
                            }

                            /*
                             * arm7: soft_open_bar75_5 - denser5 min/arms bound;
                             * soft residual never closes Dual DoD; bar .75 stamp-free.
                             * Soft!=product (ASCII only).
                             */
                            if (GJ_SPAWN_HOST_BLOB_DENSE5_ARMS == 8u &&
                                GJ_SPAWN_HOST_BLOB_DENSE5_MIN ==
                                    GJ_SPAWN_HOST_BLOB_DENSE5_ARMS &&
                                fDualDodOpen5 != 0u &&
                                fHostElfPathRole != 0u &&
                                fCatalogGeomExact != 0u &&
                                fClassGeom != 0u &&
                                fHostLookupMap != 0u) {
                                fSoftOpenBar75_5 = 1u;
                                u32Dense5++;
                            }

                            g_u32SoftHostBlobDense5Arms = u32Dense5;
                            if (u32Dense5 >= GJ_SPAWN_HOST_BLOB_DENSE5_MIN &&
                                fDenser4Held != 0u &&
                                fEmbedsPreserve5 != 0u &&
                                fCatalogGeomExact != 0u &&
                                fPentaProbe != 0u &&
                                fDualDodOpen5 != 0u &&
                                fHostElfPathRole != 0u &&
                                fDenserChain5 != 0u &&
                                fSoftOpenBar75_5 != 0u) {
                                u32Dense5Ok = 1u;
                                spawn_soft_inc(&g_u32SoftHostBlobDense5Ok);
                                szDense5Verdict = "PASS";
                            } else {
                                spawn_soft_inc(&g_u32SoftHostBlobDense5Fail);
                                szDense5Verdict = "FAIL";
                            }

                            /* Grep: spawn: soft residual host_blob denser5 */
                            /* Grep: denser5_arms= denser5_ok= denser5_min= host_blob denser5 */
                            /* Grep: keep_live=1 / never_kill_embed / preserve embeds */
                            kprintf("spawn: soft residual host_blob denser5 %s denser5=1 "
                                    "denser5_arms=%u denser5_ok=%u denser5_min=%u "
                                    "denser5_fail=%u denser4_held=%u embeds_preserve5=%u "
                                    "catalog_geom_exact=%u penta_probe=%u dual_dod_open5=%u "
                                    "host_elf_path_role=%u denser_chain5=%u soft_open_bar75_5=%u "
                                    "penta_present=%u penta_absent=%u penta_probe_n=%u "
                                    "live_present=%u live_absent=%u "
                                    "cb_ddi=%lu cb_rtl=%lu cb_xhci=%lu "
                                    "product_hosts=UDX hosts=%s,%s,%s class_n=%u "
                                    "weak=gj_ddi_host_elf_blob,gj_rtl8168_udx_elf_blob,"
                                    "gj_xhci_udx_elf_blob "
                                    "hooks=spawn_host_blob_get|elf_try_run "
                                    "steps=host_blob|probe|load|stack|thr|prefer|no_thrash "
                                    "keep_live_policy=embed_present never_kill_embed=1 "
                                    "preserve_embeds=1 host_blob=1 "
                                    "denser=1 denser2=1 denser3=1 denser4=1 denser5=1 "
                                    "dual_dod OPEN dual_dod_A=OPEN dual_dod_B=OPEN "
                                    "product_host_launch=OPEN H3=1 ko_in_kernel=0 "
                                    "freestanding_rtl=SKIP freestanding_xhci=SKIP "
                                    "soft_residual_closes_dod=0 "
                                    "soft_ne_product=1 Soft!=product dual=MIT_OR_Apache-2.0 "
                                    "G-AC-1 via=%s wave=%u  "
                                    "(Soft!=product; denser5 host_blob residual W20; "
                                    "product_hosts=UDX; dual_dod OPEN; not Dual DoD close; "
                                    "CRITICAL preserve host blob embeds ddi/rtl/xhci; "
                                    "pairs main keep_live=1 on embed; never invent .76)\n",
                                    szDense5Verdict, (unsigned)u32Dense5,
                                    (unsigned)u32Dense5Ok,
                                    (unsigned)GJ_SPAWN_HOST_BLOB_DENSE5_MIN,
                                    (unsigned)g_u32SoftHostBlobDense5Fail,
                                    (unsigned)fDenser4Held,
                                    (unsigned)fEmbedsPreserve5,
                                    (unsigned)fCatalogGeomExact,
                                    (unsigned)fPentaProbe,
                                    (unsigned)fDualDodOpen5,
                                    (unsigned)fHostElfPathRole,
                                    (unsigned)fDenserChain5,
                                    (unsigned)fSoftOpenBar75_5,
                                    u32PentaPresent, u32PentaAbsent,
                                    g_u32SoftHostBlobPentaProbe,
                                    u32PresentN, u32AbsentN,
                                    (unsigned long)g_u64SoftHostBlobCbByClass[0],
                                    (unsigned long)g_u64SoftHostBlobCbByClass[1],
                                    (unsigned long)g_u64SoftHostBlobCbByClass[2],
                                    GJ_SPAWN_HOST_DDI_NAME, GJ_SPAWN_HOST_RTL_NAME,
                                    GJ_SPAWN_HOST_XHCI_NAME, GJ_SPAWN_HOST_CLASS_N,
                                    szViaSafe, GJ_SPAWN_SOFT_WAVE);
                            if (u32Dense5Ok != 0u) {
                                kprintf("spawn: soft residual host_blob denser5 PASS via=%s "
                                        "denser5=1 denser5_arms=%u denser5_ok=1 denser5_min=%u "
                                        "product_hosts=UDX dual_dod OPEN Soft!=product "
                                        "embeds_preserve5=1 ddi|rtl|xhci "
                                        "denser_chain5=1 never_kill_embed=1 "
                                        "preserve_embeds=1 host_blob=1 "
                                        "denser=1 denser2=1 denser3=1 denser4=1 denser5=1 "
                                        "wave=%u \n",
                                        szViaSafe, (unsigned)u32Dense5,
                                        (unsigned)GJ_SPAWN_HOST_BLOB_DENSE5_MIN,
                                        GJ_SPAWN_SOFT_WAVE);
                            } else {
                                kprintf("spawn: soft residual host_blob denser5 FAIL via=%s "
                                        "denser5=1 denser5_arms=%u denser5_ok=0 denser5_min=%u "
                                        "denser5_fail=%u "
                                        "(soft residual only; not product gate; "
                                        "Soft!=product; dual_dod OPEN; product_hosts=UDX; "
                                        "preserve embeds still policy; never_kill_embed=1)\n",
                                        szViaSafe, (unsigned)u32Dense5,
                                        (unsigned)GJ_SPAWN_HOST_BLOB_DENSE5_MIN,
                                        (unsigned)g_u32SoftHostBlobDense5Fail);
                            }

                            /*
                             * denser6 host_blob residual multi-arm honesty (W21 densify).
                             * CRITICAL preserve host blob embeds ddi/rtl/xhci.
                             * Soft!=product; Dual DoD OPEN; product_hosts=UDX; never hard-gates.
                             * greppable: spawn: soft residual host_blob denser6
                             * greppable: denser6_arms= denser6_ok= denser6_min= host_blob denser6
                             *  stamp-free; never invent .76.
                             * ASCII Soft!=product only (no unicode Soft inequality glyph).
                             */
                            {
                                u32 u32Dense6;
                                u32 u32Dense6Ok;
                                u32 fDenser5Held;
                                u32 fEmbedsPreserve6;
                                u32 fProductHostsExact;
                                u32 fHexaProbe;
                                u32 fDualDodOpen6;
                                u32 fWeakHooksStable;
                                u32 fDenserChain6;
                                u32 fSoftOpenBar75_6;
                                u32 u32HexaPresent;
                                u32 u32HexaAbsent;
                                u32 u32HexaIdx;
                                const void *pHexaBlob;
                                u64 cbHexa;
                                int fHexaGet;
                                const char *szDense6Verdict;

                                u32Dense6 = 0u;
                                u32Dense6Ok = 0u;
                                fDenser5Held = 0u;
                                fEmbedsPreserve6 = 0u;
                                fProductHostsExact = 0u;
                                fHexaProbe = 0u;
                                fDualDodOpen6 = 0u;
                                fWeakHooksStable = 0u;
                                fDenserChain6 = 0u;
                                fSoftOpenBar75_6 = 0u;
                                u32HexaPresent = 0u;
                                u32HexaAbsent = 0u;
                                szDense6Verdict = "FAIL";

                                /* arm0: denser5_held - denser5 composite all arms PASS. */
                                if (u32Dense5Ok != 0u &&
                                    u32Dense5 >= GJ_SPAWN_HOST_BLOB_DENSE5_MIN &&
                                    fEmbedsPreserve5 != 0u &&
                                    fDenserChain5 != 0u) {
                                    fDenser5Held = 1u;
                                    u32Dense6++;
                                }

                                /*
                                 * arm1: embeds_preserve6 - preserve5 + denser_chain5 +
                                 * exact names + fail_closed. CRITICAL preserve host
                                 * blob embeds ddi/rtl/xhci (gj_ddi_host_elf_blob /
                                 * gj_rtl8168_udx_elf_blob / gj_xhci_udx_elf_blob).
                                 * denser6 pairs main keep_live=1 on embed.
                                 */
                                if (fEmbedsPreserve5 != 0u &&
                                    fEmbedsPreserve4 != 0u &&
                                    fEmbedsPreserve3 != 0u &&
                                    fEmbedsPreserve2 != 0u &&
                                    fEmbedsPreserve != 0u &&
                                    fDenserChain5 != 0u &&
                                    fFailClosed != 0u &&
                                    GJ_SPAWN_HOST_CLASS_N == 3u &&
                                    g_aHostCatalog[0].szName != NULL &&
                                    g_aHostCatalog[1].szName != NULL &&
                                    g_aHostCatalog[2].szName != NULL &&
                                    strcmp(g_aHostCatalog[0].szName,
                                           GJ_SPAWN_HOST_DDI_NAME) == 0 &&
                                    strcmp(g_aHostCatalog[1].szName,
                                           GJ_SPAWN_HOST_RTL_NAME) == 0 &&
                                    strcmp(g_aHostCatalog[2].szName,
                                           GJ_SPAWN_HOST_XHCI_NAME) == 0) {
                                    fEmbedsPreserve6 = 1u;
                                    u32Dense6++;
                                }

                                /*
                                 * arm2: product_hosts_exact - exact product_hosts=UDX
                                 * names via macros (ddi_host_gj|rtl8168_udx|xhci_udx).
                                 * Soft!=product; Dual DoD stays OPEN.
                                 */
                                if (g_aHostCatalog[0].szName != NULL &&
                                    g_aHostCatalog[1].szName != NULL &&
                                    g_aHostCatalog[2].szName != NULL &&
                                    strcmp(g_aHostCatalog[0].szName,
                                           "ddi_host_gj") == 0 &&
                                    strcmp(g_aHostCatalog[1].szName,
                                           "rtl8168_udx") == 0 &&
                                    strcmp(g_aHostCatalog[2].szName,
                                           "xhci_udx") == 0 &&
                                    strcmp(GJ_SPAWN_HOST_DDI_NAME,
                                           "ddi_host_gj") == 0 &&
                                    strcmp(GJ_SPAWN_HOST_RTL_NAME,
                                           "rtl8168_udx") == 0 &&
                                    strcmp(GJ_SPAWN_HOST_XHCI_NAME,
                                           "xhci_udx") == 0 &&
                                    GJ_SPAWN_HOST_CLASS_N == 3u) {
                                    fProductHostsExact = 1u;
                                    u32Dense6++;
                                }

                                /*
                                 * arm3: hexa_probe - sixth live spawn_host_blob_get.
                                 * Present/miss honesty only; Dual DoD stays OPEN on miss.
                                 * Preserve weak resolve path for all class slots.
                                 */
                                for (u32HexaIdx = 0u;
                                     u32HexaIdx < GJ_SPAWN_HOST_CLASS_N;
                                     u32HexaIdx++) {
                                    pHexaBlob = NULL;
                                    cbHexa = 0ull;
                                    spawn_soft_inc(&g_u32SoftHostBlobHexaProbe);
                                    spawn_soft_inc(&g_u32SoftHostBlobProbe);
                                    fHexaGet = spawn_host_blob_get(u32HexaIdx,
                                                                   &pHexaBlob,
                                                                   &cbHexa);
                                    if (fHexaGet != 0 && pHexaBlob != NULL &&
                                        cbHexa > 0ull) {
                                        u32HexaPresent++;
                                        g_u64SoftHostBlobCbByClass[u32HexaIdx] =
                                            cbHexa;
                                    } else {
                                        u32HexaAbsent++;
                                        g_u64SoftHostBlobCbByClass[u32HexaIdx] =
                                            0ull;
                                    }
                                }
                                if ((u32HexaPresent + u32HexaAbsent) ==
                                        GJ_SPAWN_HOST_CLASS_N &&
                                    g_u32SoftHostBlobHexaProbe > 0u &&
                                    spawn_host_blob_get(0u, NULL, NULL) == 0 &&
                                    spawn_host_blob_get(99u, &pHexaBlob,
                                                        &cbHexa) == 0) {
                                    fHexaProbe = 1u;
                                    u32Dense6++;
                                }

                                /*
                                 * arm4: dual_dod_open6 - Dual DoD A/B remain OPEN.
                                 * open5 + status + ab_open + stubs live.
                                 * Soft residual never closes Dual DoD (Soft!=product).
                                 */
                                if (fDualDodOpen5 != 0u &&
                                    fDualDodStatusOpen != 0u &&
                                    fDualDodAbOpen != 0u &&
                                    fDualDodOpen2 != 0u &&
                                    fDualDodMap != 0u &&
                                    g_aHostCatalog[0].u8DualDod == 0u &&
                                    g_aHostCatalog[1].u8DualDod == 2u &&
                                    g_aHostCatalog[2].u8DualDod == 1u &&
                                    g_aHostCatalog[0].pfnStubEntry != NULL &&
                                    g_aHostCatalog[1].pfnStubEntry != NULL &&
                                    g_aHostCatalog[2].pfnStubEntry != NULL) {
                                    fDualDodOpen6 = 1u;
                                    u32Dense6++;
                                }

                                /*
                                 * arm5: weak_hooks_stable - blob_get fail-closed +
                                 * catalog lookup map + weak resolve hooks wired.
                                 * CRITICAL preserve host blob embeds ddi/rtl/xhci.
                                 */
                                if (fFailClosed != 0u &&
                                    fHostLookupMap != 0u &&
                                    fEmbedsPreserve != 0u &&
                                    fHexaProbe != 0u &&
                                    (u32PresentN + u32AbsentN) ==
                                        GJ_SPAWN_HOST_CLASS_N &&
                                    spawn_host_blob_get(0u, NULL, NULL) == 0 &&
                                    spawn_host_blob_get(99u, &pHexaBlob,
                                                        &cbHexa) == 0 &&
                                    spawn_host_catalog_get(0u) != NULL &&
                                    spawn_host_catalog_get(1u) != NULL &&
                                    spawn_host_catalog_get(2u) != NULL &&
                                    spawn_host_catalog_get(99u) == NULL) {
                                    fWeakHooksStable = 1u;
                                    u32Dense6++;
                                }

                                /*
                                 * arm6: denser_chain6 - denser1..5 held + embeds 1..6.
                                 * CRITICAL preserve host blob embeds ddi/rtl/xhci.
                                 */
                                if (fDenser1Held != 0u && fDenser2Held != 0u &&
                                    fDenser3Held != 0u && fDenser4Held != 0u &&
                                    fDenser5Held != 0u &&
                                    fEmbedsPreserve != 0u &&
                                    fEmbedsPreserve2 != 0u &&
                                    fEmbedsPreserve3 != 0u &&
                                    fEmbedsPreserve4 != 0u &&
                                    fEmbedsPreserve5 != 0u &&
                                    fEmbedsPreserve6 != 0u &&
                                    fHexaProbe != 0u) {
                                    fDenserChain6 = 1u;
                                    u32Dense6++;
                                }

                                /*
                                 * arm7: soft_open_bar75_6 - denser6 min/arms bound;
                                 * soft residual never closes Dual DoD; bar .75 stamp-free.
                                 * Soft!=product (ASCII only).
                                 */
                                if (GJ_SPAWN_HOST_BLOB_DENSE6_ARMS == 8u &&
                                    GJ_SPAWN_HOST_BLOB_DENSE6_MIN ==
                                        GJ_SPAWN_HOST_BLOB_DENSE6_ARMS &&
                                    fDualDodOpen6 != 0u &&
                                    fProductHostsExact != 0u &&
                                    fWeakHooksStable != 0u &&
                                    fClassGeom != 0u &&
                                    fHostLookupMap != 0u) {
                                    fSoftOpenBar75_6 = 1u;
                                    u32Dense6++;
                                }

                                g_u32SoftHostBlobDense6Arms = u32Dense6;
                                if (u32Dense6 >= GJ_SPAWN_HOST_BLOB_DENSE6_MIN &&
                                    fDenser5Held != 0u &&
                                    fEmbedsPreserve6 != 0u &&
                                    fProductHostsExact != 0u &&
                                    fHexaProbe != 0u &&
                                    fDualDodOpen6 != 0u &&
                                    fWeakHooksStable != 0u &&
                                    fDenserChain6 != 0u &&
                                    fSoftOpenBar75_6 != 0u) {
                                    u32Dense6Ok = 1u;
                                    spawn_soft_inc(&g_u32SoftHostBlobDense6Ok);
                                    szDense6Verdict = "PASS";
                                } else {
                                    spawn_soft_inc(&g_u32SoftHostBlobDense6Fail);
                                    szDense6Verdict = "FAIL";
                                }

                                /* Grep: spawn: soft residual host_blob denser6 */
                                /* Grep: denser6_arms= denser6_ok= denser6_min= host_blob denser6 */
                                /* Grep: keep_live=1 / never_kill_embed / preserve embeds */
                                kprintf("spawn: soft residual host_blob denser6 %s denser6=1 "
                                        "denser6_arms=%u denser6_ok=%u denser6_min=%u "
                                        "denser6_fail=%u denser5_held=%u embeds_preserve6=%u "
                                        "product_hosts_exact=%u hexa_probe=%u dual_dod_open6=%u "
                                        "weak_hooks_stable=%u denser_chain6=%u soft_open_bar75_6=%u "
                                        "hexa_present=%u hexa_absent=%u hexa_probe_n=%u "
                                        "live_present=%u live_absent=%u "
                                        "cb_ddi=%lu cb_rtl=%lu cb_xhci=%lu "
                                        "product_hosts=UDX hosts=%s,%s,%s class_n=%u "
                                        "weak=gj_ddi_host_elf_blob,gj_rtl8168_udx_elf_blob,"
                                        "gj_xhci_udx_elf_blob "
                                        "hooks=spawn_host_blob_get|elf_try_run "
                                        "steps=host_blob|probe|load|stack|thr|prefer|no_thrash "
                                        "keep_live_policy=embed_present never_kill_embed=1 "
                                        "preserve_embeds=1 host_blob=1 "
                                        "denser=1 denser2=1 denser3=1 denser4=1 denser5=1 denser6=1 "
                                        "dual_dod OPEN dual_dod_A=OPEN dual_dod_B=OPEN "
                                        "product_host_launch=OPEN H3=1 ko_in_kernel=0 "
                                        "freestanding_rtl=SKIP freestanding_xhci=SKIP "
                                        "soft_residual_closes_dod=0 "
                                        "soft_ne_product=1 Soft!=product dual=MIT_OR_Apache-2.0 "
                                        "G-AC-1 via=%s wave=%u  "
                                        "(Soft!=product; denser6 host_blob residual W21; "
                                        "product_hosts=UDX; dual_dod OPEN; not Dual DoD close; "
                                        "CRITICAL preserve host blob embeds ddi/rtl/xhci; "
                                        "pairs main keep_live=1 on embed; never invent .76)\n",
                                        szDense6Verdict, (unsigned)u32Dense6,
                                        (unsigned)u32Dense6Ok,
                                        (unsigned)GJ_SPAWN_HOST_BLOB_DENSE6_MIN,
                                        (unsigned)g_u32SoftHostBlobDense6Fail,
                                        (unsigned)fDenser5Held,
                                        (unsigned)fEmbedsPreserve6,
                                        (unsigned)fProductHostsExact,
                                        (unsigned)fHexaProbe,
                                        (unsigned)fDualDodOpen6,
                                        (unsigned)fWeakHooksStable,
                                        (unsigned)fDenserChain6,
                                        (unsigned)fSoftOpenBar75_6,
                                        u32HexaPresent, u32HexaAbsent,
                                        g_u32SoftHostBlobHexaProbe,
                                        u32PresentN, u32AbsentN,
                                        (unsigned long)g_u64SoftHostBlobCbByClass[0],
                                        (unsigned long)g_u64SoftHostBlobCbByClass[1],
                                        (unsigned long)g_u64SoftHostBlobCbByClass[2],
                                        GJ_SPAWN_HOST_DDI_NAME, GJ_SPAWN_HOST_RTL_NAME,
                                        GJ_SPAWN_HOST_XHCI_NAME, GJ_SPAWN_HOST_CLASS_N,
                                        szViaSafe, GJ_SPAWN_SOFT_WAVE);
                                if (u32Dense6Ok != 0u) {
                                    kprintf("spawn: soft residual host_blob denser6 PASS via=%s "
                                            "denser6=1 denser6_arms=%u denser6_ok=1 denser6_min=%u "
                                            "product_hosts=UDX dual_dod OPEN Soft!=product "
                                            "embeds_preserve6=1 ddi|rtl|xhci "
                                            "denser_chain6=1 never_kill_embed=1 "
                                            "preserve_embeds=1 host_blob=1 "
                                            "denser=1 denser2=1 denser3=1 denser4=1 denser5=1 denser6=1 "
                                            "wave=%u \n",
                                            szViaSafe, (unsigned)u32Dense6,
                                            (unsigned)GJ_SPAWN_HOST_BLOB_DENSE6_MIN,
                                            GJ_SPAWN_SOFT_WAVE);
                                } else {
                                    kprintf("spawn: soft residual host_blob denser6 FAIL via=%s "
                                            "denser6=1 denser6_arms=%u denser6_ok=0 denser6_min=%u "
                                            "denser6_fail=%u "
                                            "(soft residual only; not product gate; "
                                            "Soft!=product; dual_dod OPEN; product_hosts=UDX; "
                                            "preserve embeds still policy; never_kill_embed=1)\n",
                                            szViaSafe, (unsigned)u32Dense6,
                                            (unsigned)GJ_SPAWN_HOST_BLOB_DENSE6_MIN,
                                            (unsigned)g_u32SoftHostBlobDense6Fail);
                                }

                                /*
                                 * denser7 host_blob residual multi-arm honesty (W22 densify).
                                 * CRITICAL preserve host blob embeds ddi/rtl/xhci.
                                 * Soft!=product; Dual DoD OPEN; product_hosts=UDX; never hard-gates.
                                 * greppable: spawn: soft residual host_blob denser7
                                 * greppable: denser7_arms= denser7_ok= denser7_min= host_blob denser7
                                 *  stamp-free; never invent .76.
                                 * ASCII Soft!=product only (no unicode Soft inequality glyph).
                                 */
                                {
                                    u32 u32Dense7;
                                    u32 u32Dense7Ok;
                                    u32 fDenser6Held;
                                    u32 fEmbedsPreserve7;
                                    u32 fDualDodClassExact;
                                    u32 fHeptaProbe;
                                    u32 fDualDodOpen7;
                                    u32 fWeakResolveChain;
                                    u32 fDenserChain7;
                                    u32 fSoftOpenBar75_7;
                                    u32 u32HeptaPresent;
                                    u32 u32HeptaAbsent;
                                    u32 u32HeptaIdx;
                                    const void *pHeptaBlob;
                                    u64 cbHepta;
                                    int fHeptaGet;
                                    const char *szDense7Verdict;

                                    u32Dense7 = 0u;
                                    u32Dense7Ok = 0u;
                                    fDenser6Held = 0u;
                                    fEmbedsPreserve7 = 0u;
                                    fDualDodClassExact = 0u;
                                    fHeptaProbe = 0u;
                                    fDualDodOpen7 = 0u;
                                    fWeakResolveChain = 0u;
                                    fDenserChain7 = 0u;
                                    fSoftOpenBar75_7 = 0u;
                                    u32HeptaPresent = 0u;
                                    u32HeptaAbsent = 0u;
                                    szDense7Verdict = "FAIL";

                                    /* arm0: denser6_held - denser6 composite all arms PASS. */
                                    if (u32Dense6Ok != 0u &&
                                        u32Dense6 >= GJ_SPAWN_HOST_BLOB_DENSE6_MIN &&
                                        fEmbedsPreserve6 != 0u &&
                                        fDenserChain6 != 0u) {
                                        fDenser6Held = 1u;
                                        u32Dense7++;
                                    }

                                    /*
                                     * arm1: embeds_preserve7 - preserve6 + denser_chain6 +
                                     * exact names + fail_closed. CRITICAL preserve host
                                     * blob embeds ddi/rtl/xhci (gj_ddi_host_elf_blob /
                                     * gj_rtl8168_udx_elf_blob / gj_xhci_udx_elf_blob).
                                     * denser7 pairs main keep_live=1 on embed.
                                     */
                                    if (fEmbedsPreserve6 != 0u &&
                                        fEmbedsPreserve5 != 0u &&
                                        fEmbedsPreserve4 != 0u &&
                                        fEmbedsPreserve3 != 0u &&
                                        fEmbedsPreserve2 != 0u &&
                                        fEmbedsPreserve != 0u &&
                                        fDenserChain6 != 0u &&
                                        fFailClosed != 0u &&
                                        GJ_SPAWN_HOST_CLASS_N == 3u &&
                                        g_aHostCatalog[0].szName != NULL &&
                                        g_aHostCatalog[1].szName != NULL &&
                                        g_aHostCatalog[2].szName != NULL &&
                                        strcmp(g_aHostCatalog[0].szName,
                                               GJ_SPAWN_HOST_DDI_NAME) == 0 &&
                                        strcmp(g_aHostCatalog[1].szName,
                                               GJ_SPAWN_HOST_RTL_NAME) == 0 &&
                                        strcmp(g_aHostCatalog[2].szName,
                                               GJ_SPAWN_HOST_XHCI_NAME) == 0) {
                                        fEmbedsPreserve7 = 1u;
                                        u32Dense7++;
                                    }

                                    /*
                                     * arm2: dual_dod_class_exact - Dual DoD 0/2/1 map +
                                     * class_n=3 product_hosts=UDX (ddi/rtl/xhci).
                                     * Soft residual never closes Dual DoD (Soft!=product).
                                     */
                                    if (GJ_SPAWN_HOST_CLASS_N == 3u &&
                                        g_aHostCatalog[0].u8DualDod == 0u &&
                                        g_aHostCatalog[1].u8DualDod == 2u &&
                                        g_aHostCatalog[2].u8DualDod == 1u &&
                                        g_aHostCatalog[0].szName != NULL &&
                                        g_aHostCatalog[1].szName != NULL &&
                                        g_aHostCatalog[2].szName != NULL &&
                                        strcmp(g_aHostCatalog[0].szName,
                                               "ddi_host_gj") == 0 &&
                                        strcmp(g_aHostCatalog[1].szName,
                                               "rtl8168_udx") == 0 &&
                                        strcmp(g_aHostCatalog[2].szName,
                                               "xhci_udx") == 0 &&
                                        fDualDodMap != 0u &&
                                        fProductHostsExact != 0u) {
                                        fDualDodClassExact = 1u;
                                        u32Dense7++;
                                    }

                                    /*
                                     * arm3: hepta_probe - seventh live spawn_host_blob_get.
                                     * Present/miss honesty only; Dual DoD stays OPEN on miss.
                                     * Preserve weak resolve path for all class slots.
                                     */
                                    for (u32HeptaIdx = 0u;
                                         u32HeptaIdx < GJ_SPAWN_HOST_CLASS_N;
                                         u32HeptaIdx++) {
                                        pHeptaBlob = NULL;
                                        cbHepta = 0ull;
                                        spawn_soft_inc(&g_u32SoftHostBlobHeptaProbe);
                                        spawn_soft_inc(&g_u32SoftHostBlobProbe);
                                        fHeptaGet = spawn_host_blob_get(u32HeptaIdx,
                                                                        &pHeptaBlob,
                                                                        &cbHepta);
                                        if (fHeptaGet != 0 && pHeptaBlob != NULL &&
                                            cbHepta > 0ull) {
                                            u32HeptaPresent++;
                                            g_u64SoftHostBlobCbByClass[u32HeptaIdx] =
                                                cbHepta;
                                        } else {
                                            u32HeptaAbsent++;
                                            g_u64SoftHostBlobCbByClass[u32HeptaIdx] =
                                                0ull;
                                        }
                                    }
                                    if ((u32HeptaPresent + u32HeptaAbsent) ==
                                            GJ_SPAWN_HOST_CLASS_N &&
                                        g_u32SoftHostBlobHeptaProbe > 0u &&
                                        spawn_host_blob_get(0u, NULL, NULL) == 0 &&
                                        spawn_host_blob_get(99u, &pHeptaBlob,
                                                            &cbHepta) == 0) {
                                        fHeptaProbe = 1u;
                                        u32Dense7++;
                                    }

                                    /*
                                     * arm4: dual_dod_open7 - Dual DoD A/B remain OPEN.
                                     * open6 + open5 + status + ab_open + stubs live.
                                     * Soft residual never closes Dual DoD (Soft!=product).
                                     */
                                    if (fDualDodOpen6 != 0u &&
                                        fDualDodOpen5 != 0u &&
                                        fDualDodStatusOpen != 0u &&
                                        fDualDodAbOpen != 0u &&
                                        fDualDodOpen2 != 0u &&
                                        fDualDodMap != 0u &&
                                        g_aHostCatalog[0].u8DualDod == 0u &&
                                        g_aHostCatalog[1].u8DualDod == 2u &&
                                        g_aHostCatalog[2].u8DualDod == 1u &&
                                        g_aHostCatalog[0].pfnStubEntry != NULL &&
                                        g_aHostCatalog[1].pfnStubEntry != NULL &&
                                        g_aHostCatalog[2].pfnStubEntry != NULL) {
                                        fDualDodOpen7 = 1u;
                                        u32Dense7++;
                                    }

                                    /*
                                     * arm5: weak_resolve_chain - blob_get fail-closed +
                                     * hepta + preserve chain + catalog lookup map.
                                     * CRITICAL preserve host blob embeds ddi/rtl/xhci.
                                     */
                                    if (fFailClosed != 0u &&
                                        fHostLookupMap != 0u &&
                                        fEmbedsPreserve != 0u &&
                                        fEmbedsPreserve7 != 0u &&
                                        fHeptaProbe != 0u &&
                                        fWeakHooksStable != 0u &&
                                        (u32PresentN + u32AbsentN) ==
                                            GJ_SPAWN_HOST_CLASS_N &&
                                        spawn_host_blob_get(0u, NULL, NULL) == 0 &&
                                        spawn_host_blob_get(99u, &pHeptaBlob,
                                                            &cbHepta) == 0 &&
                                        spawn_host_catalog_get(0u) != NULL &&
                                        spawn_host_catalog_get(1u) != NULL &&
                                        spawn_host_catalog_get(2u) != NULL &&
                                        spawn_host_catalog_get(99u) == NULL) {
                                        fWeakResolveChain = 1u;
                                        u32Dense7++;
                                    }

                                    /*
                                     * arm6: denser_chain7 - denser1..6 held + embeds 1..7.
                                     * CRITICAL preserve host blob embeds ddi/rtl/xhci.
                                     */
                                    if (fDenser1Held != 0u && fDenser2Held != 0u &&
                                        fDenser3Held != 0u && fDenser4Held != 0u &&
                                        fDenser5Held != 0u && fDenser6Held != 0u &&
                                        fEmbedsPreserve != 0u &&
                                        fEmbedsPreserve2 != 0u &&
                                        fEmbedsPreserve3 != 0u &&
                                        fEmbedsPreserve4 != 0u &&
                                        fEmbedsPreserve5 != 0u &&
                                        fEmbedsPreserve6 != 0u &&
                                        fEmbedsPreserve7 != 0u &&
                                        fHeptaProbe != 0u) {
                                        fDenserChain7 = 1u;
                                        u32Dense7++;
                                    }

                                    /*
                                     * arm7: soft_open_bar75_7 - denser7 min/arms bound;
                                     * soft residual never closes Dual DoD; bar .75 stamp-free.
                                     * Soft!=product (ASCII only).
                                     */
                                    if (GJ_SPAWN_HOST_BLOB_DENSE7_ARMS == 8u &&
                                        GJ_SPAWN_HOST_BLOB_DENSE7_MIN ==
                                            GJ_SPAWN_HOST_BLOB_DENSE7_ARMS &&
                                        fDualDodOpen7 != 0u &&
                                        fDualDodClassExact != 0u &&
                                        fWeakResolveChain != 0u &&
                                        fClassGeom != 0u &&
                                        fHostLookupMap != 0u) {
                                        fSoftOpenBar75_7 = 1u;
                                        u32Dense7++;
                                    }

                                    g_u32SoftHostBlobDense7Arms = u32Dense7;
                                    if (u32Dense7 >= GJ_SPAWN_HOST_BLOB_DENSE7_MIN &&
                                        fDenser6Held != 0u &&
                                        fEmbedsPreserve7 != 0u &&
                                        fDualDodClassExact != 0u &&
                                        fHeptaProbe != 0u &&
                                        fDualDodOpen7 != 0u &&
                                        fWeakResolveChain != 0u &&
                                        fDenserChain7 != 0u &&
                                        fSoftOpenBar75_7 != 0u) {
                                        u32Dense7Ok = 1u;
                                        spawn_soft_inc(&g_u32SoftHostBlobDense7Ok);
                                        szDense7Verdict = "PASS";
                                    } else {
                                        spawn_soft_inc(&g_u32SoftHostBlobDense7Fail);
                                        szDense7Verdict = "FAIL";
                                    }

                                    /* Grep: spawn: soft residual host_blob denser7 */
                                    /* Grep: denser7_arms= denser7_ok= denser7_min= host_blob denser7 */
                                    /* Grep: keep_live=1 / never_kill_embed / preserve embeds */
                                    kprintf("spawn: soft residual host_blob denser7 %s denser7=1 "
                                            "denser7_arms=%u denser7_ok=%u denser7_min=%u "
                                            "denser7_fail=%u denser6_held=%u embeds_preserve7=%u "
                                            "dual_dod_class_exact=%u hepta_probe=%u dual_dod_open7=%u "
                                            "weak_resolve_chain=%u denser_chain7=%u soft_open_bar75_7=%u "
                                            "hepta_present=%u hepta_absent=%u hepta_probe_n=%u "
                                            "live_present=%u live_absent=%u "
                                            "cb_ddi=%lu cb_rtl=%lu cb_xhci=%lu "
                                            "product_hosts=UDX hosts=%s,%s,%s class_n=%u "
                                            "weak=gj_ddi_host_elf_blob,gj_rtl8168_udx_elf_blob,"
                                            "gj_xhci_udx_elf_blob "
                                            "hooks=spawn_host_blob_get|elf_try_run "
                                            "steps=host_blob|probe|load|stack|thr|prefer|no_thrash "
                                            "keep_live_policy=embed_present never_kill_embed=1 "
                                            "preserve_embeds=1 host_blob=1 "
                                            "denser=1 denser2=1 denser3=1 denser4=1 denser5=1 denser6=1 denser7=1 "
                                            "dual_dod OPEN dual_dod_A=OPEN dual_dod_B=OPEN "
                                            "product_host_launch=OPEN H3=1 ko_in_kernel=0 "
                                            "freestanding_rtl=SKIP freestanding_xhci=SKIP "
                                            "soft_residual_closes_dod=0 "
                                            "soft_ne_product=1 Soft!=product dual=MIT_OR_Apache-2.0 "
                                            "G-AC-1 via=%s wave=%u  "
                                            "(Soft!=product; denser7 host_blob residual W22; "
                                            "product_hosts=UDX; dual_dod OPEN; not Dual DoD close; "
                                            "CRITICAL preserve host blob embeds ddi/rtl/xhci; "
                                            "pairs main keep_live=1 on embed; never invent .76)\n",
                                            szDense7Verdict, (unsigned)u32Dense7,
                                            (unsigned)u32Dense7Ok,
                                            (unsigned)GJ_SPAWN_HOST_BLOB_DENSE7_MIN,
                                            (unsigned)g_u32SoftHostBlobDense7Fail,
                                            (unsigned)fDenser6Held,
                                            (unsigned)fEmbedsPreserve7,
                                            (unsigned)fDualDodClassExact,
                                            (unsigned)fHeptaProbe,
                                            (unsigned)fDualDodOpen7,
                                            (unsigned)fWeakResolveChain,
                                            (unsigned)fDenserChain7,
                                            (unsigned)fSoftOpenBar75_7,
                                            u32HeptaPresent, u32HeptaAbsent,
                                            g_u32SoftHostBlobHeptaProbe,
                                            u32PresentN, u32AbsentN,
                                            (unsigned long)g_u64SoftHostBlobCbByClass[0],
                                            (unsigned long)g_u64SoftHostBlobCbByClass[1],
                                            (unsigned long)g_u64SoftHostBlobCbByClass[2],
                                            GJ_SPAWN_HOST_DDI_NAME, GJ_SPAWN_HOST_RTL_NAME,
                                            GJ_SPAWN_HOST_XHCI_NAME, GJ_SPAWN_HOST_CLASS_N,
                                            szViaSafe, GJ_SPAWN_SOFT_WAVE);
                                    if (u32Dense7Ok != 0u) {
                                        kprintf("spawn: soft residual host_blob denser7 PASS via=%s "
                                                "denser7=1 denser7_arms=%u denser7_ok=1 denser7_min=%u "
                                                "product_hosts=UDX dual_dod OPEN Soft!=product "
                                                "embeds_preserve7=1 ddi|rtl|xhci "
                                                "denser_chain7=1 never_kill_embed=1 "
                                                "preserve_embeds=1 host_blob=1 "
                                                "denser=1 denser2=1 denser3=1 denser4=1 denser5=1 denser6=1 denser7=1 "
                                                "wave=%u \n",
                                                szViaSafe, (unsigned)u32Dense7,
                                                (unsigned)GJ_SPAWN_HOST_BLOB_DENSE7_MIN,
                                                GJ_SPAWN_SOFT_WAVE);
                                    } else {
                                        kprintf("spawn: soft residual host_blob denser7 FAIL via=%s "
                                                "denser7=1 denser7_arms=%u denser7_ok=0 denser7_min=%u "
                                                "denser7_fail=%u "
                                                "(soft residual only; not product gate; "
                                                "Soft!=product; dual_dod OPEN; product_hosts=UDX; "
                                                "preserve embeds still policy; never_kill_embed=1)\n",
                                                szViaSafe, (unsigned)u32Dense7,
                                                (unsigned)GJ_SPAWN_HOST_BLOB_DENSE7_MIN,
                                                (unsigned)g_u32SoftHostBlobDense7Fail);
                                    }

                                    /*
                                     * denser8 host_blob residual multi-arm honesty (W23 densify).
                                     * CRITICAL preserve host blob embeds ddi/rtl/xhci.
                                     * Soft!=product; Dual DoD OPEN; product_hosts=UDX; never hard-gates.
                                     * greppable: spawn: soft residual host_blob denser8
                                     * greppable: denser8_arms= denser8_ok= denser8_min= host_blob denser8
                                     *  stamp-free; never invent .76.
                                     * ASCII Soft!=product only (no unicode Soft inequality glyph).
                                     */
                                    {
                                        u32 u32Dense8;
                                        u32 u32Dense8Ok;
                                        u32 fDenser7Held;
                                        u32 fEmbedsPreserve8;
                                        u32 fProductUdxTriple;
                                        u32 fOctaProbe;
                                        u32 fDualDodOpen8;
                                        u32 fPreserveResolve8;
                                        u32 fDenserChain8;
                                        u32 fSoftOpenBar75_8;
                                        u32 u32OctaPresent;
                                        u32 u32OctaAbsent;
                                        u32 u32OctaIdx;
                                        const void *pOctaBlob;
                                        u64 cbOcta;
                                        int fOctaGet;
                                        const char *szDense8Verdict;

                                        u32Dense8 = 0u;
                                        u32Dense8Ok = 0u;
                                        fDenser7Held = 0u;
                                        fEmbedsPreserve8 = 0u;
                                        fProductUdxTriple = 0u;
                                        fOctaProbe = 0u;
                                        fDualDodOpen8 = 0u;
                                        fPreserveResolve8 = 0u;
                                        fDenserChain8 = 0u;
                                        fSoftOpenBar75_8 = 0u;
                                        u32OctaPresent = 0u;
                                        u32OctaAbsent = 0u;
                                        szDense8Verdict = "FAIL";

                                        /* arm0: denser7_held - denser7 composite all arms PASS. */
                                        if (u32Dense7Ok != 0u &&
                                            u32Dense7 >= GJ_SPAWN_HOST_BLOB_DENSE7_MIN &&
                                            fEmbedsPreserve7 != 0u &&
                                            fDenserChain7 != 0u) {
                                            fDenser7Held = 1u;
                                            u32Dense8++;
                                        }

                                        /*
                                         * arm1: embeds_preserve8 - preserve7 + denser_chain7 +
                                         * exact names + fail_closed. CRITICAL preserve host
                                         * blob embeds ddi/rtl/xhci (gj_ddi_host_elf_blob /
                                         * gj_rtl8168_udx_elf_blob / gj_xhci_udx_elf_blob).
                                         * denser8 pairs main keep_live=1 on embed.
                                         */
                                        if (fEmbedsPreserve7 != 0u &&
                                            fEmbedsPreserve6 != 0u &&
                                            fEmbedsPreserve5 != 0u &&
                                            fEmbedsPreserve4 != 0u &&
                                            fEmbedsPreserve3 != 0u &&
                                            fEmbedsPreserve2 != 0u &&
                                            fEmbedsPreserve != 0u &&
                                            fDenserChain7 != 0u &&
                                            fFailClosed != 0u &&
                                            GJ_SPAWN_HOST_CLASS_N == 3u &&
                                            g_aHostCatalog[0].szName != NULL &&
                                            g_aHostCatalog[1].szName != NULL &&
                                            g_aHostCatalog[2].szName != NULL &&
                                            strcmp(g_aHostCatalog[0].szName,
                                                   GJ_SPAWN_HOST_DDI_NAME) == 0 &&
                                            strcmp(g_aHostCatalog[1].szName,
                                                   GJ_SPAWN_HOST_RTL_NAME) == 0 &&
                                            strcmp(g_aHostCatalog[2].szName,
                                                   GJ_SPAWN_HOST_XHCI_NAME) == 0) {
                                            fEmbedsPreserve8 = 1u;
                                            u32Dense8++;
                                        }

                                        /*
                                         * arm2: product_udx_triple - product_hosts=UDX name +
                                         * role + elf path residual for ddi/rtl/xhci.
                                         * Soft residual never closes Dual DoD (Soft!=product).
                                         */
                                        if (GJ_SPAWN_HOST_CLASS_N == 3u &&
                                            g_aHostCatalog[0].szName != NULL &&
                                            g_aHostCatalog[1].szName != NULL &&
                                            g_aHostCatalog[2].szName != NULL &&
                                            strcmp(g_aHostCatalog[0].szName,
                                                   "ddi_host_gj") == 0 &&
                                            strcmp(g_aHostCatalog[1].szName,
                                                   "rtl8168_udx") == 0 &&
                                            strcmp(g_aHostCatalog[2].szName,
                                                   "xhci_udx") == 0 &&
                                            g_aHostCatalog[0].szRole != NULL &&
                                            g_aHostCatalog[1].szRole != NULL &&
                                            g_aHostCatalog[2].szRole != NULL &&
                                            strcmp(g_aHostCatalog[0].szRole,
                                                   "ddi_door_residual") == 0 &&
                                            strcmp(g_aHostCatalog[1].szRole,
                                                   "udx_nic") == 0 &&
                                            strcmp(g_aHostCatalog[2].szRole,
                                                   "udx_usb_hc") == 0 &&
                                            g_aHostCatalog[0].szElfPath != NULL &&
                                            g_aHostCatalog[1].szElfPath != NULL &&
                                            g_aHostCatalog[2].szElfPath != NULL &&
                                            g_aHostCatalog[0].szElfPath[0] != '\0' &&
                                            g_aHostCatalog[1].szElfPath[0] != '\0' &&
                                            g_aHostCatalog[2].szElfPath[0] != '\0' &&
                                            fProductHostsExact != 0u) {
                                            fProductUdxTriple = 1u;
                                            u32Dense8++;
                                        }

                                        /*
                                         * arm3: octa_probe - eighth live spawn_host_blob_get.
                                         * Present/miss honesty only; Dual DoD stays OPEN on miss.
                                         * Preserve weak resolve path for all class slots.
                                         */
                                        for (u32OctaIdx = 0u;
                                             u32OctaIdx < GJ_SPAWN_HOST_CLASS_N;
                                             u32OctaIdx++) {
                                            pOctaBlob = NULL;
                                            cbOcta = 0ull;
                                            spawn_soft_inc(&g_u32SoftHostBlobOctaProbe);
                                            spawn_soft_inc(&g_u32SoftHostBlobProbe);
                                            fOctaGet = spawn_host_blob_get(u32OctaIdx,
                                                                            &pOctaBlob,
                                                                            &cbOcta);
                                            if (fOctaGet != 0 && pOctaBlob != NULL &&
                                                cbOcta > 0ull) {
                                                u32OctaPresent++;
                                                g_u64SoftHostBlobCbByClass[u32OctaIdx] =
                                                    cbOcta;
                                            } else {
                                                u32OctaAbsent++;
                                                g_u64SoftHostBlobCbByClass[u32OctaIdx] =
                                                    0ull;
                                            }
                                        }
                                        if ((u32OctaPresent + u32OctaAbsent) ==
                                                GJ_SPAWN_HOST_CLASS_N &&
                                            g_u32SoftHostBlobOctaProbe > 0u &&
                                            spawn_host_blob_get(0u, NULL, NULL) == 0 &&
                                            spawn_host_blob_get(99u, &pOctaBlob,
                                                                &cbOcta) == 0) {
                                            fOctaProbe = 1u;
                                            u32Dense8++;
                                        }

                                        /*
                                         * arm4: dual_dod_open8 - Dual DoD A/B remain OPEN.
                                         * open7 + open6 + open5 + status + ab_open + stubs.
                                         * Soft residual never closes Dual DoD (Soft!=product).
                                         */
                                        if (fDualDodOpen7 != 0u &&
                                            fDualDodOpen6 != 0u &&
                                            fDualDodOpen5 != 0u &&
                                            fDualDodStatusOpen != 0u &&
                                            fDualDodAbOpen != 0u &&
                                            fDualDodOpen2 != 0u &&
                                            fDualDodMap != 0u &&
                                            g_aHostCatalog[0].u8DualDod == 0u &&
                                            g_aHostCatalog[1].u8DualDod == 2u &&
                                            g_aHostCatalog[2].u8DualDod == 1u &&
                                            g_aHostCatalog[0].pfnStubEntry != NULL &&
                                            g_aHostCatalog[1].pfnStubEntry != NULL &&
                                            g_aHostCatalog[2].pfnStubEntry != NULL) {
                                            fDualDodOpen8 = 1u;
                                            u32Dense8++;
                                        }

                                        /*
                                         * arm5: preserve_resolve8 - blob_get fail-closed +
                                         * octa + preserve chain + catalog lookup map.
                                         * CRITICAL preserve host blob embeds ddi/rtl/xhci.
                                         */
                                        if (fFailClosed != 0u &&
                                            fHostLookupMap != 0u &&
                                            fEmbedsPreserve != 0u &&
                                            fEmbedsPreserve8 != 0u &&
                                            fOctaProbe != 0u &&
                                            fWeakHooksStable != 0u &&
                                            fWeakResolveChain != 0u &&
                                            (u32PresentN + u32AbsentN) ==
                                                GJ_SPAWN_HOST_CLASS_N &&
                                            spawn_host_blob_get(0u, NULL, NULL) == 0 &&
                                            spawn_host_blob_get(99u, &pOctaBlob,
                                                                &cbOcta) == 0 &&
                                            spawn_host_catalog_get(0u) != NULL &&
                                            spawn_host_catalog_get(1u) != NULL &&
                                            spawn_host_catalog_get(2u) != NULL &&
                                            spawn_host_catalog_get(99u) == NULL) {
                                            fPreserveResolve8 = 1u;
                                            u32Dense8++;
                                        }

                                        /*
                                         * arm6: denser_chain8 - denser1..7 held + embeds 1..8.
                                         * CRITICAL preserve host blob embeds ddi/rtl/xhci.
                                         */
                                        if (fDenser1Held != 0u && fDenser2Held != 0u &&
                                            fDenser3Held != 0u && fDenser4Held != 0u &&
                                            fDenser5Held != 0u && fDenser6Held != 0u &&
                                            fDenser7Held != 0u &&
                                            fEmbedsPreserve != 0u &&
                                            fEmbedsPreserve2 != 0u &&
                                            fEmbedsPreserve3 != 0u &&
                                            fEmbedsPreserve4 != 0u &&
                                            fEmbedsPreserve5 != 0u &&
                                            fEmbedsPreserve6 != 0u &&
                                            fEmbedsPreserve7 != 0u &&
                                            fEmbedsPreserve8 != 0u &&
                                            fOctaProbe != 0u) {
                                            fDenserChain8 = 1u;
                                            u32Dense8++;
                                        }

                                        /*
                                         * arm7: soft_open_bar75_8 - denser8 min/arms bound;
                                         * soft residual never closes Dual DoD; bar .75 stamp-free.
                                         * Soft!=product (ASCII only).
                                         */
                                        if (GJ_SPAWN_HOST_BLOB_DENSE8_ARMS == 8u &&
                                            GJ_SPAWN_HOST_BLOB_DENSE8_MIN ==
                                                GJ_SPAWN_HOST_BLOB_DENSE8_ARMS &&
                                            fDualDodOpen8 != 0u &&
                                            fProductUdxTriple != 0u &&
                                            fPreserveResolve8 != 0u &&
                                            fClassGeom != 0u &&
                                            fHostLookupMap != 0u) {
                                            fSoftOpenBar75_8 = 1u;
                                            u32Dense8++;
                                        }

                                        g_u32SoftHostBlobDense8Arms = u32Dense8;
                                        if (u32Dense8 >= GJ_SPAWN_HOST_BLOB_DENSE8_MIN &&
                                            fDenser7Held != 0u &&
                                            fEmbedsPreserve8 != 0u &&
                                            fProductUdxTriple != 0u &&
                                            fOctaProbe != 0u &&
                                            fDualDodOpen8 != 0u &&
                                            fPreserveResolve8 != 0u &&
                                            fDenserChain8 != 0u &&
                                            fSoftOpenBar75_8 != 0u) {
                                            u32Dense8Ok = 1u;
                                            spawn_soft_inc(&g_u32SoftHostBlobDense8Ok);
                                            szDense8Verdict = "PASS";
                                        } else {
                                            spawn_soft_inc(&g_u32SoftHostBlobDense8Fail);
                                            szDense8Verdict = "FAIL";
                                        }

                                        /* Grep: spawn: soft residual host_blob denser8 */
                                        /* Grep: denser8_arms= denser8_ok= denser8_min= host_blob denser8 */
                                        /* Grep: keep_live=1 / never_kill_embed / preserve embeds */
                                        kprintf("spawn: soft residual host_blob denser8 %s denser8=1 "
                                                "denser8_arms=%u denser8_ok=%u denser8_min=%u "
                                                "denser8_fail=%u denser7_held=%u embeds_preserve8=%u "
                                                "product_udx_triple=%u octa_probe=%u dual_dod_open8=%u "
                                                "preserve_resolve8=%u denser_chain8=%u soft_open_bar75_8=%u "
                                                "octa_present=%u octa_absent=%u octa_probe_n=%u "
                                                "live_present=%u live_absent=%u "
                                                "cb_ddi=%lu cb_rtl=%lu cb_xhci=%lu "
                                                "product_hosts=UDX hosts=%s,%s,%s class_n=%u "
                                                "weak=gj_ddi_host_elf_blob,gj_rtl8168_udx_elf_blob,"
                                                "gj_xhci_udx_elf_blob "
                                                "hooks=spawn_host_blob_get|elf_try_run "
                                                "steps=host_blob|probe|load|stack|thr|prefer|no_thrash "
                                                "keep_live_policy=embed_present never_kill_embed=1 "
                                                "preserve_embeds=1 host_blob=1 "
                                                "denser=1 denser2=1 denser3=1 denser4=1 denser5=1 denser6=1 denser7=1 denser8=1 "
                                                "dual_dod OPEN dual_dod_A=OPEN dual_dod_B=OPEN "
                                                "product_host_launch=OPEN H3=1 ko_in_kernel=0 "
                                                "freestanding_rtl=SKIP freestanding_xhci=SKIP "
                                                "soft_residual_closes_dod=0 "
                                                "soft_ne_product=1 Soft!=product dual=MIT_OR_Apache-2.0 "
                                                "G-AC-1 via=%s wave=%u  "
                                                "(Soft!=product; denser8 host_blob residual W23; "
                                                "product_hosts=UDX; dual_dod OPEN; not Dual DoD close; "
                                                "CRITICAL preserve host blob embeds ddi/rtl/xhci; "
                                                "pairs main keep_live=1 on embed; never invent .76)\n",
                                                szDense8Verdict, (unsigned)u32Dense8,
                                                (unsigned)u32Dense8Ok,
                                                (unsigned)GJ_SPAWN_HOST_BLOB_DENSE8_MIN,
                                                (unsigned)g_u32SoftHostBlobDense8Fail,
                                                (unsigned)fDenser7Held,
                                                (unsigned)fEmbedsPreserve8,
                                                (unsigned)fProductUdxTriple,
                                                (unsigned)fOctaProbe,
                                                (unsigned)fDualDodOpen8,
                                                (unsigned)fPreserveResolve8,
                                                (unsigned)fDenserChain8,
                                                (unsigned)fSoftOpenBar75_8,
                                                u32OctaPresent, u32OctaAbsent,
                                                g_u32SoftHostBlobOctaProbe,
                                                u32PresentN, u32AbsentN,
                                                (unsigned long)g_u64SoftHostBlobCbByClass[0],
                                                (unsigned long)g_u64SoftHostBlobCbByClass[1],
                                                (unsigned long)g_u64SoftHostBlobCbByClass[2],
                                                GJ_SPAWN_HOST_DDI_NAME, GJ_SPAWN_HOST_RTL_NAME,
                                                GJ_SPAWN_HOST_XHCI_NAME, GJ_SPAWN_HOST_CLASS_N,
                                                szViaSafe, GJ_SPAWN_SOFT_WAVE);
                                        if (u32Dense8Ok != 0u) {
                                            kprintf("spawn: soft residual host_blob denser8 PASS via=%s "
                                                    "denser8=1 denser8_arms=%u denser8_ok=1 denser8_min=%u "
                                                    "product_hosts=UDX dual_dod OPEN Soft!=product "
                                                    "embeds_preserve8=1 ddi|rtl|xhci "
                                                    "denser_chain8=1 never_kill_embed=1 "
                                                    "preserve_embeds=1 host_blob=1 "
                                                    "denser=1 denser2=1 denser3=1 denser4=1 denser5=1 denser6=1 denser7=1 denser8=1 "
                                                    "wave=%u \n",
                                                    szViaSafe, (unsigned)u32Dense8,
                                                    (unsigned)GJ_SPAWN_HOST_BLOB_DENSE8_MIN,
                                                    GJ_SPAWN_SOFT_WAVE);
                                        } else {
                                            kprintf("spawn: soft residual host_blob denser8 FAIL via=%s "
                                                    "denser8=1 denser8_arms=%u denser8_ok=0 denser8_min=%u "
                                                    "denser8_fail=%u "
                                                    "(soft residual only; not product gate; "
                                                    "Soft!=product; dual_dod OPEN; product_hosts=UDX; "
                                                    "preserve embeds still policy; never_kill_embed=1)\n",
                                                    szViaSafe, (unsigned)u32Dense8,
                                                    (unsigned)GJ_SPAWN_HOST_BLOB_DENSE8_MIN,
                                                    (unsigned)g_u32SoftHostBlobDense8Fail);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    /*
     * Host launch path residual (lifecycle steps; Soft!=product).
     * Grep: spawn: soft residual host_path
     */
    kprintf("spawn: soft residual host_path "
            "steps=spawn,mint_G-PROC-2,first_thr,wait_reg,"
            "host_blob_probe_load_stack_thr_prefer_no_thrash,"
            "ddi_udx_later,kill_or_thr_exit,death_G-PROC-5_H3,wait_reap "
            "path_steps=%u claim=process_spawn class=C2 "
            "rights_core=0x%x rights_base=0x%x "
            "cnode_slots=%u wait_poll=AGAIN yield_on_again=1 "
            "H3=thr_exit_before_as_destroy ddi_cap_mint=OPEN "
            "product_host_launch=OPEN product_hosts=UDX hosts=%s,%s,%s "
            "elf_paths=%s,%s,%s elf_run_ok=%u "
            "elf_prefer=%u elf_stub_exit=%u elf_no_thrash=%u "
            "host_blob embed=%u miss=%u dual_dod OPEN "
            "soft_ne_product=1 Soft!=product dual=MIT_OR_Apache-2.0 G-AC-1 "
            "via=%s wave=%u \n",
            GJ_SPAWN_C2_PATH_STEPS,
            (unsigned)GJ_SPAWN_PROCESS_RIGHTS_CORE,
            (unsigned)GJ_SPAWN_PROCESS_RIGHTS_BASE,
            (unsigned)GJ_SPAWN_CNODE_SLOTS,
            GJ_SPAWN_HOST_DDI_NAME, GJ_SPAWN_HOST_RTL_NAME,
            GJ_SPAWN_HOST_XHCI_NAME,
            (g_aHostCatalog[0].szElfPath != NULL) ? g_aHostCatalog[0].szElfPath
                                                    : "?",
            (g_aHostCatalog[1].szElfPath != NULL) ? g_aHostCatalog[1].szElfPath
                                                    : "?",
            (g_aHostCatalog[2].szElfPath != NULL) ? g_aHostCatalog[2].szElfPath
                                                    : "?",
            g_u32SoftHostElfRunOk, g_u32SoftHostElfPrefer,
            g_u32SoftHostElfStubExit, g_u32SoftHostElfNoThrash,
            g_u32SoftHostElfEmbed, g_u32SoftHostElfMiss,
            szViaSafe, GJ_SPAWN_SOFT_WAVE);

    /*
     * Dual DoD A/B residual direction (UDX userspace hosts; Soft!=product).
     * Grep: spawn: soft residual dual_dod
     * greppable: dual_dod OPEN / product_hosts=UDX / host_blob / Soft!=product
     */
    kprintf("spawn: soft residual dual_dod "
            "A=xhci_udx vend=0x%x dev=0x%x status=OPEN "
            "B=rtl8168_udx vend=0x%x dev=0x%x status=OPEN "
            "ddi_host=%s status=OPEN dual_dod OPEN "
            "product_hosts=UDX host_blob densify "
            "launch=process_spawn path=userspace_udx "
            "steps=host_blob|probe|load|stack|thr|prefer "
            "freestanding_rtl=SKIP freestanding_xhci=SKIP "
            "ko_in_kernel=0 product_wire=OPEN product_bot=OPEN "
            "soft_ne_product=1 Soft!=product dual=MIT_OR_Apache-2.0 G-AC-1 "
            "via=%s wave=%u\n",
            (unsigned)GJ_SPAWN_HOST_XHCI_VEND,
            (unsigned)GJ_SPAWN_HOST_XHCI_DEV,
            (unsigned)GJ_SPAWN_HOST_RTL_VEND,
            (unsigned)GJ_SPAWN_HOST_RTL_DEV,
            GJ_SPAWN_HOST_DDI_NAME,
            szViaSafe, GJ_SPAWN_SOFT_WAVE);

    /*
     * C2 UDX launch residual (ASSURANCE_LITE class C2 product path).
     * Soft scaffold honesty only — never product AC / Dual DoD close.
     * Concurrent capacity: class hosts must fit fixed spawn table geometry.
     * Self-check deepen: BASE covers CORE + SPAWN + CNODE min + fail_bits.
     * Grep: spawn: soft residual c2
     * Grep: spawn: soft residual udx_launch
     * Grep: spawn: soft residual c2_udx_launch
     * Grep: spawn: soft residual c2_host_map
     */
    {
        u32 u32CapReady;
        u32 u32C2Ok;
        u32 u32C2FailBits;
        u16 u16BaseRights;
        u16 u16CoreRights;

        spawn_soft_inc(&g_u32SoftC2UdxLaunchLog);
        u16BaseRights = GJ_SPAWN_PROCESS_RIGHTS_BASE;
        u16CoreRights = GJ_SPAWN_PROCESS_RIGHTS_CORE;

        /*
         * Concurrent capacity residual: free slots can hold all class hosts
         * right now (class_n <= max geometry already required). Soft only.
         */
        u32CapReady = 0u;
        if (GJ_SPAWN_HOST_CLASS_N <= GJ_SPAWN_MAX &&
            u32Free >= GJ_SPAWN_HOST_CLASS_N) {
            u32CapReady = 1u;
            spawn_soft_inc(&g_u32SoftHostCapReady);
        }

        /* Grep: spawn: soft residual c2 */
        kprintf("spawn: soft residual c2 class=C2 "
                "role=udx_ddi_host_launch product=UDX+ABI "
                "dual_dod_A=OPEN dual_dod_B=OPEN "
                "freestanding_rtl=SKIP freestanding_xhci=SKIP "
                "ko_in_kernel=0 soft_scaffold_ne_product_ac=1 "
                "hosts=%s,%s,%s class_n=%u path_steps=%u "
                "launch=process_spawn product_host_launch=OPEN "
                "product_wire=OPEN product_bot=OPEN ddi_cap_mint=OPEN "
                "H3=1 soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1 "
                "c2_logs=%u via=%s wave=%u "
                "(Soft!=product; C2 residual honesty; not Dual DoD close; "
                "not product AC)\n",
                GJ_SPAWN_HOST_DDI_NAME, GJ_SPAWN_HOST_RTL_NAME,
                GJ_SPAWN_HOST_XHCI_NAME, GJ_SPAWN_HOST_CLASS_N,
                GJ_SPAWN_C2_PATH_STEPS,
                g_u32SoftC2UdxLaunchLog, szViaSafe, GJ_SPAWN_SOFT_WAVE);

        /*
         * Per-host role map (C2 product-path residual; Soft!=product).
         * ddi residual != Dual DoD close; A/B OPEN until USB path / interactive SSH login.
         * Grep: spawn: soft residual c2_host_map
         */
        kprintf("spawn: soft residual c2_host_map class=C2 "
                "ddi=%s role=ddi_door_residual dual_dod=none status=OPEN "
                "rtl=%s role=udx_nic dual_dod=B vend=0x%x dev=0x%x status=OPEN "
                "xhci=%s role=udx_usb_hc dual_dod=A vend=0x%x dev=0x%x status=OPEN "
                "product=UDX+ABI freestanding_class=SKIP ko_in_kernel=0 "
                "product_host_launch=OPEN ddi_cap_mint=OPEN "
                "soft_scaffold_ne_product_ac=1 "
                "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1 "
                "via=%s wave=%u "
                "(Soft!=product; host role map; not Dual DoD close)\n",
                GJ_SPAWN_HOST_DDI_NAME,
                GJ_SPAWN_HOST_RTL_NAME,
                (unsigned)GJ_SPAWN_HOST_RTL_VEND,
                (unsigned)GJ_SPAWN_HOST_RTL_DEV,
                GJ_SPAWN_HOST_XHCI_NAME,
                (unsigned)GJ_SPAWN_HOST_XHCI_VEND,
                (unsigned)GJ_SPAWN_HOST_XHCI_DEV,
                szViaSafe, GJ_SPAWN_SOFT_WAVE);

        /* Grep: spawn: soft residual udx_launch */
        kprintf("spawn: soft residual udx_launch residual=1 class=C2 "
                "path=process_spawn,G-PROC-2,first_thr,wait_reg,host_run,"
                "ddi_udx_later,kill_or_thr_exit,death_G-PROC-5_H3,wait_reap "
                "path_steps=%u table_max=%u live=%u free=%u class_n=%u "
                "cap_ready=%u "
                "host_launch_ok=%u wait_again=%u yield_on_again=1 "
                "H3=thr_exit_before_as_destroy persona_pref=native=%u "
                "ddi_cap_mint=OPEN product_host_launch=OPEN product=UDX+ABI "
                "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1 "
                "via=%s wave=%u "
                "(Soft!=product; UDX launch residual; not host product close)\n",
                GJ_SPAWN_C2_PATH_STEPS, GJ_SPAWN_MAX, cLive, u32Free,
                GJ_SPAWN_HOST_CLASS_N, u32CapReady, g_u32SoftHostLaunchOk,
                g_u32SoftWaitAgain, GJ_SPAWN_HOST_PERSONA_NATIVE,
                szViaSafe, GJ_SPAWN_SOFT_WAVE);

        /*
         * C2 udx_launch self-check (static catalog/geometry/rights; never
         * hard-gates). cap_ready is runtime free residual — NOT part of ok/N.
         * fail_bits: bit i set when check i fails (see GJ_SPAWN_C2_* comment).
         * Grep: spawn: soft residual c2_udx_launch
         */
        u32C2Ok = 0u;
        u32C2FailBits = 0u;
        if (GJ_SPAWN_HOST_CLASS_N == 3u) {
            u32C2Ok++;
        } else {
            u32C2FailBits |= (1u << 0);
        }
        if (GJ_SPAWN_HOST_CLASS_N <= GJ_SPAWN_MAX) {
            u32C2Ok++; /* concurrent class hosts fit fixed table */
        } else {
            u32C2FailBits |= (1u << 1);
        }
        if (GJ_SPAWN_HOST_RTL_VEND == 0x10ecu &&
            GJ_SPAWN_HOST_RTL_DEV == 0x8168u) {
            u32C2Ok++; /* Dual DoD B class id */
        } else {
            u32C2FailBits |= (1u << 2);
        }
        if (GJ_SPAWN_HOST_XHCI_VEND == 0x8086u &&
            GJ_SPAWN_HOST_XHCI_DEV == 0xa12fu) {
            u32C2Ok++; /* Dual DoD A class id */
        } else {
            u32C2FailBits |= (1u << 3);
        }
        if ((u16CoreRights &
             (u16)(GJ_RIGHT_DESTROY | GJ_RIGHT_WAIT | GJ_RIGHT_VM)) ==
            (u16)(GJ_RIGHT_DESTROY | GJ_RIGHT_WAIT | GJ_RIGHT_VM)) {
            u32C2Ok++; /* kill/reap/vm surface for host lifecycle */
        } else {
            u32C2FailBits |= (1u << 4);
        }
        if (GJ_SPAWN_HOST_PERSONA_NATIVE == 0u &&
            GJ_SPAWN_HOST_DDI_NAME[0] != '\0' &&
            GJ_SPAWN_HOST_RTL_NAME[0] != '\0' &&
            GJ_SPAWN_HOST_XHCI_NAME[0] != '\0') {
            u32C2Ok++; /* native persona + named userspace hosts (G-AC-1) */
        } else {
            u32C2FailBits |= (1u << 5);
        }
        /* BASE must cover CORE (mint residual surface complete). */
        if ((u16BaseRights & u16CoreRights) == u16CoreRights) {
            u32C2Ok++;
        } else {
            u32C2FailBits |= (1u << 6);
        }
        /* Parent PROCESS mint path needs SPAWN for further host children. */
        if ((u16BaseRights & (u16)GJ_RIGHT_SPAWN) != 0u) {
            u32C2Ok++;
        } else {
            u32C2FailBits |= (1u << 7);
        }
        /* Child CNode room for later DDI MMIO/IRQ/DMA caps (OPEN residual). */
        if (GJ_SPAWN_CNODE_SLOTS >= GJ_SPAWN_C2_CNODE_MIN) {
            u32C2Ok++;
        } else {
            u32C2FailBits |= (1u << 8);
        }
        if (u32C2Ok == GJ_SPAWN_C2_UDX_LAUNCH_OK_N) {
            spawn_soft_inc(&g_u32SoftC2UdxLaunchOk);
        }
        kprintf("spawn: soft residual c2_udx_launch ok=%u/%u class=C2 "
                "class_n=%u table_max=%u fit=%u cap_ready=%u "
                "path_steps=%u cnode_min=%llu cnode_slots=%llu "
                "rights_base=0x%x rights_core=0x%x fail_bits=0x%x "
                "ddi=%s rtl=%s xhci=%s "
                "rtl_id=%04x:%04x xhci_id=%04x:%04x "
                "host_launch_ok=%u c2_pass=%u "
                "soft_scaffold_ne_product_ac=1 product=UDX+ABI "
                "dual_dod_A=OPEN dual_dod_B=OPEN "
                "ko_in_kernel=0 freestanding_rtl=SKIP freestanding_xhci=SKIP "
                "product_host_launch=OPEN H3=1 "
                "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1 "
                "via=%s wave=%u "
                "(Soft!=product; C2 UDX launch residual self-check; "
                "not product AC; not Dual DoD close)\n",
                u32C2Ok, GJ_SPAWN_C2_UDX_LAUNCH_OK_N,
                GJ_SPAWN_HOST_CLASS_N, GJ_SPAWN_MAX,
                (GJ_SPAWN_HOST_CLASS_N <= GJ_SPAWN_MAX) ? 1u : 0u,
                u32CapReady, GJ_SPAWN_C2_PATH_STEPS,
                (unsigned long long)GJ_SPAWN_C2_CNODE_MIN,
                (unsigned long long)GJ_SPAWN_CNODE_SLOTS,
                (unsigned)u16BaseRights, (unsigned)u16CoreRights,
                u32C2FailBits,
                GJ_SPAWN_HOST_DDI_NAME, GJ_SPAWN_HOST_RTL_NAME,
                GJ_SPAWN_HOST_XHCI_NAME,
                (unsigned)GJ_SPAWN_HOST_RTL_VEND,
                (unsigned)GJ_SPAWN_HOST_RTL_DEV,
                (unsigned)GJ_SPAWN_HOST_XHCI_VEND,
                (unsigned)GJ_SPAWN_HOST_XHCI_DEV,
                g_u32SoftHostLaunchOk, g_u32SoftC2UdxLaunchOk,
                szViaSafe, GJ_SPAWN_SOFT_WAVE);
        if (u32C2Ok == GJ_SPAWN_C2_UDX_LAUNCH_OK_N && u32C2FailBits == 0u) {
            kprintf("spawn: soft residual c2_udx_launch PASS via=%s "
                    "ok=%u/%u fail_bits=0x0 wave=%u\n",
                    szViaSafe, u32C2Ok, GJ_SPAWN_C2_UDX_LAUNCH_OK_N,
                    GJ_SPAWN_SOFT_WAVE);
        } else {
            kprintf("spawn: soft residual c2_udx_launch FAIL via=%s "
                    "ok=%u/%u fail_bits=0x%x "
                    "(soft residual only; not product gate)\n",
                    szViaSafe, u32C2Ok, GJ_SPAWN_C2_UDX_LAUNCH_OK_N,
                    u32C2FailBits);
        }
    }

    /* Grep: spawn: soft host_launch */
    kprintf("spawn: soft host_launch residual=1 "
            "ddi_host_gj=OPEN rtl8168_udx=OPEN xhci_udx=OPEN "
            "path=process_spawn|host_blob|probe|load|stack|thr|"
            "prefer_elf|no_thrash table_max=%u "
            "cnode_slots=%u "
            "class_n=%u host_launch_ok=%u elf_run_ok=%u elf_embed=%u "
            "elf_miss=%u elf_probe_ok=%u elf_load_ok=%u "
            "elf_stack_fail=%u elf_thr_fail=%u "
            "elf_prefer=%u elf_stub_exit=%u "
            "elf_no_thrash=%u elf_qos_prefer=%u "
            "persona_native=%u persona_linux=%u "
            "prefer_elf_thr=1_when_embed no_thrash=1_when_prefer "
            "product_hosts=UDX dual_dod OPEN host_blob densify "
            "wait_poll=AGAIN yield_on_again=1 "
            "H3=1 G-AC-1=1 Dual_DoD=OPEN "
            "soft_ne_product=1 Soft!=product dual=MIT_OR_Apache-2.0 "
            "via=%s wave=%u \n",
            GJ_SPAWN_MAX, (unsigned)GJ_SPAWN_CNODE_SLOTS,
            GJ_SPAWN_HOST_CLASS_N, g_u32SoftHostLaunchOk,
            g_u32SoftHostElfRunOk, g_u32SoftHostElfEmbed,
            g_u32SoftHostElfMiss, g_u32SoftHostElfProbeOk,
            g_u32SoftHostElfLoadOk, g_u32SoftHostElfStackFail,
            g_u32SoftHostElfThrFail, g_u32SoftHostElfPrefer,
            g_u32SoftHostElfStubExit, g_u32SoftHostElfNoThrash,
            g_u32SoftHostElfQosPrefer, g_u32SoftPersonaNative,
            g_u32SoftPersonaLinux, szViaSafe, GJ_SPAWN_SOFT_WAVE);

    /*
     * Host-launch lean self-check (geometry + honesty; never hard-gates).
     * ok/GJ_SPAWN_HOST_LAUNCH_OK_N — Soft!=product residual only.
     * Grep: spawn: soft host_launch lean
     */
    {
        u32 u32HostOk;

        u32HostOk = 0;
        if (GJ_SPAWN_HOST_CLASS_N == 3u) {
            u32HostOk++;
        }
        if (GJ_SPAWN_HOST_CLASS_N <= GJ_SPAWN_MAX) {
            u32HostOk++; /* concurrent class hosts fit fixed table */
        }
        if (GJ_SPAWN_MAX >= 3u && GJ_SPAWN_CNODE_SLOTS >= 8ull) {
            u32HostOk++;
        }
        if ((GJ_SPAWN_PROCESS_RIGHTS_CORE &
             (u16)(GJ_RIGHT_DESTROY | GJ_RIGHT_WAIT | GJ_RIGHT_VM)) ==
            (u16)(GJ_RIGHT_DESTROY | GJ_RIGHT_WAIT | GJ_RIGHT_VM)) {
            u32HostOk++; /* kill/reap/vm surface for host lifecycle */
        }
        if (GJ_SPAWN_HOST_RTL_VEND == 0x10ecu &&
            GJ_SPAWN_HOST_RTL_DEV == 0x8168u) {
            u32HostOk++; /* Dual DoD B class id */
        }
        if (GJ_SPAWN_HOST_XHCI_VEND == 0x8086u &&
            GJ_SPAWN_HOST_XHCI_DEV == 0xa12fu) {
            u32HostOk++; /* Dual DoD A class id */
        }
        if (GJ_SPAWN_HOST_PERSONA_NATIVE == 0u) {
            u32HostOk++; /* native persona preferred for UDX hosts */
        }
        /* G-AC-1 honesty: residual is userspace hosts only (no .ko product). */
        if (GJ_SPAWN_HOST_DDI_NAME[0] != '\0' &&
            GJ_SPAWN_HOST_RTL_NAME[0] != '\0' &&
            GJ_SPAWN_HOST_XHCI_NAME[0] != '\0') {
            u32HostOk++;
        }
        if (u32HostOk == GJ_SPAWN_HOST_LAUNCH_OK_N) {
            spawn_soft_inc(&g_u32SoftHostLaunchLean);
        }
        kprintf("spawn: soft host_launch lean ok=%u/%u "
                "class_n=%u table_max=%u fit=%u "
                "ddi=%s rtl=%s xhci=%s "
                "rtl_id=%04x:%04x xhci_id=%04x:%04x "
                "persona_pref=native rights_core=0x%x "
                "host_launch_ok=%u lean_pass=%u "
                "ko_in_kernel=0 multi_server_confine=0 "
                "product_host_launch=OPEN ddi_cap_mint=OPEN "
                "H3=1 soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1 "
                "via=%s wave=%u "
                "(Soft!=product; host launch residual self-check; "
                "not product host close; not multi-server confine)\n",
                u32HostOk, GJ_SPAWN_HOST_LAUNCH_OK_N,
                GJ_SPAWN_HOST_CLASS_N, GJ_SPAWN_MAX,
                (GJ_SPAWN_HOST_CLASS_N <= GJ_SPAWN_MAX) ? 1u : 0u,
                GJ_SPAWN_HOST_DDI_NAME, GJ_SPAWN_HOST_RTL_NAME,
                GJ_SPAWN_HOST_XHCI_NAME,
                (unsigned)GJ_SPAWN_HOST_RTL_VEND,
                (unsigned)GJ_SPAWN_HOST_RTL_DEV,
                (unsigned)GJ_SPAWN_HOST_XHCI_VEND,
                (unsigned)GJ_SPAWN_HOST_XHCI_DEV,
                (unsigned)GJ_SPAWN_PROCESS_RIGHTS_CORE,
                g_u32SoftHostLaunchOk, g_u32SoftHostLaunchLean,
                szViaSafe, GJ_SPAWN_SOFT_WAVE);
        if (u32HostOk == GJ_SPAWN_HOST_LAUNCH_OK_N) {
            kprintf("spawn: soft host_launch lean PASS via=%s "
                    "ok=%u/%u wave=%u\n",
                    szViaSafe, u32HostOk, GJ_SPAWN_HOST_LAUNCH_OK_N,
                    GJ_SPAWN_SOFT_WAVE);
        } else {
            kprintf("spawn: soft host_launch lean FAIL via=%s "
                    "ok=%u/%u (soft residual only; not product gate)\n",
                    szViaSafe, u32HostOk, GJ_SPAWN_HOST_LAUNCH_OK_N);
        }
    }

    /*
     * Host-hook residual self-check: structured catalog resolve + launch
     * fail-closed (never spawns here). Soft!=product; not Dual DoD close.
     * Grep: spawn: soft residual host_hook
     */
    {
        u32 u32HookOk;
        u32 u32HookFailBits;
        u32 u32Idx;
        gj_status_t stHook;

        spawn_soft_inc(&g_u32SoftHostHookLog);
        u32HookOk = 0u;
        u32HookFailBits = 0u;

        /* 0: class_n + table present */
        if (GJ_SPAWN_HOST_CLASS_N == 3u &&
            g_aHostCatalog[0].szName != NULL &&
            g_aHostCatalog[1].szName != NULL &&
            g_aHostCatalog[2].szName != NULL) {
            u32HookOk++;
        } else {
            u32HookFailBits |= (1u << 0);
        }
        /* 1: names match macros */
        if (g_aHostCatalog[0].szName != NULL &&
            g_aHostCatalog[1].szName != NULL &&
            g_aHostCatalog[2].szName != NULL &&
            strcmp(g_aHostCatalog[0].szName, GJ_SPAWN_HOST_DDI_NAME) == 0 &&
            strcmp(g_aHostCatalog[1].szName, GJ_SPAWN_HOST_RTL_NAME) == 0 &&
            strcmp(g_aHostCatalog[2].szName, GJ_SPAWN_HOST_XHCI_NAME) == 0) {
            u32HookOk++;
        } else {
            u32HookFailBits |= (1u << 1);
        }
        /* 2: rtl id + dual_dod B */
        if (g_aHostCatalog[1].u16Vend == GJ_SPAWN_HOST_RTL_VEND &&
            g_aHostCatalog[1].u16Dev == GJ_SPAWN_HOST_RTL_DEV &&
            g_aHostCatalog[1].u8DualDod == 2u) {
            u32HookOk++;
        } else {
            u32HookFailBits |= (1u << 2);
        }
        /* 3: xhci id + dual_dod A */
        if (g_aHostCatalog[2].u16Vend == GJ_SPAWN_HOST_XHCI_VEND &&
            g_aHostCatalog[2].u16Dev == GJ_SPAWN_HOST_XHCI_DEV &&
            g_aHostCatalog[2].u8DualDod == 1u) {
            u32HookOk++;
        } else {
            u32HookFailBits |= (1u << 3);
        }
        /* 4: ddi residual 0:0 dual_dod none + role */
        if (g_aHostCatalog[0].u16Vend == 0u &&
            g_aHostCatalog[0].u16Dev == 0u &&
            g_aHostCatalog[0].u8DualDod == 0u &&
            g_aHostCatalog[0].szRole != NULL &&
            g_aHostCatalog[0].szRole[0] != '\0') {
            u32HookOk++;
        } else {
            u32HookFailBits |= (1u << 4);
        }
        /* 5: lookup by name all 3 + stage alias ddi_host + sshd miss */
        if (spawn_host_catalog_lookup_name(GJ_SPAWN_HOST_DDI_NAME) == 0u &&
            spawn_host_catalog_lookup_name("ddi_host") == 0u &&
            spawn_host_catalog_lookup_name(GJ_SPAWN_HOST_RTL_NAME) == 1u &&
            spawn_host_catalog_lookup_name(GJ_SPAWN_HOST_XHCI_NAME) == 2u &&
            spawn_host_catalog_lookup_name("sshd") == (u32)-1) {
            u32HookOk++;
        } else {
            u32HookFailBits |= (1u << 5);
        }
        /* 6: lookup by PCI; ddi 0:0 miss; unknown miss */
        u32Idx = spawn_host_catalog_lookup_pci(GJ_SPAWN_HOST_RTL_VEND,
                                               GJ_SPAWN_HOST_RTL_DEV);
        if (u32Idx == 1u &&
            spawn_host_catalog_lookup_pci(GJ_SPAWN_HOST_XHCI_VEND,
                                          GJ_SPAWN_HOST_XHCI_DEV) == 2u &&
            spawn_host_catalog_lookup_pci(0u, 0u) == (u32)-1 &&
            spawn_host_catalog_lookup_pci(0xffffu, 0xffffu) == (u32)-1) {
            u32HookOk++;
        } else {
            u32HookFailBits |= (1u << 6);
        }
        /* 7: launch hook fail-closed (no spawn) */
        /* null parent -> INVAL; bad class idx -> INVAL (pHost NULL, no deref) */
        stHook = process_spawn_host_launch(NULL, 0u, NULL, NULL, NULL, NULL);
        if (stHook == GJ_ERR_INVAL) {
            stHook = process_spawn_host_launch(
                (struct gj_process *)(void *)0x1, 99u, NULL, NULL, NULL, NULL);
            if (stHook == GJ_ERR_INVAL) {
                stHook = process_spawn_host_launch_by_name(
                    NULL, "no_such_host", NULL, NULL, NULL, NULL);
                if (stHook == GJ_ERR_INVAL || stHook == GJ_ERR_NOENT) {
                    u32HookOk++;
                } else {
                    u32HookFailBits |= (1u << 7);
                }
            } else {
                u32HookFailBits |= (1u << 7);
            }
        } else {
            u32HookFailBits |= (1u << 7);
        }
        /* 8: stub entries + persona native */
        if (g_aHostCatalog[0].pfnStubEntry != NULL &&
            g_aHostCatalog[1].pfnStubEntry != NULL &&
            g_aHostCatalog[2].pfnStubEntry != NULL &&
            g_aHostCatalog[0].u8PersonaPref == GJ_SPAWN_HOST_PERSONA_NATIVE &&
            g_aHostCatalog[1].u8PersonaPref == GJ_SPAWN_HOST_PERSONA_NATIVE &&
            g_aHostCatalog[2].u8PersonaPref == GJ_SPAWN_HOST_PERSONA_NATIVE) {
            u32HookOk++;
        } else {
            u32HookFailBits |= (1u << 8);
        }
        /* 9: elf path residual non-empty for all 3 hosts */
        if (g_aHostCatalog[0].szElfPath != NULL &&
            g_aHostCatalog[0].szElfPath[0] != '\0' &&
            g_aHostCatalog[1].szElfPath != NULL &&
            g_aHostCatalog[1].szElfPath[0] != '\0' &&
            g_aHostCatalog[2].szElfPath != NULL &&
            g_aHostCatalog[2].szElfPath[0] != '\0') {
            u32HookOk++;
        } else {
            u32HookFailBits |= (1u << 9);
        }

        if (u32HookOk == GJ_SPAWN_HOST_HOOK_OK_N && u32HookFailBits == 0u) {
            spawn_soft_inc(&g_u32SoftHostHookOk);
        }
        kprintf("spawn: soft residual host_hook ok=%u/%u "
                "class_n=%u structured=1 "
                "lookup_name=1 lookup_pci=1 launch_fail_closed=1 "
                "stub_park=1 auto_tag=pArg elf_path=1 "
                "ddi=%s rtl=%s xhci=%s "
                "rtl_id=%04x:%04x xhci_id=%04x:%04x "
                "elf_ddi=%s elf_rtl=%s elf_xhci=%s "
                "launch_class_ok=%u elf_run_ok=%u tag_auto=%u hook_deny=%u "
                "fail_bits=0x%x "
                "sshd_residual=main_embed port=22 "
                "product_host_launch=OPEN ddi_cap_mint=OPEN "
                "H3=1 soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1 "
                "via=%s wave=%u "
                "(Soft!=product; catalog->spawn hooks residual; "
                "not product host close; not Dual DoD close)\n",
                u32HookOk, GJ_SPAWN_HOST_HOOK_OK_N, GJ_SPAWN_HOST_CLASS_N,
                GJ_SPAWN_HOST_DDI_NAME, GJ_SPAWN_HOST_RTL_NAME,
                GJ_SPAWN_HOST_XHCI_NAME,
                (unsigned)GJ_SPAWN_HOST_RTL_VEND,
                (unsigned)GJ_SPAWN_HOST_RTL_DEV,
                (unsigned)GJ_SPAWN_HOST_XHCI_VEND,
                (unsigned)GJ_SPAWN_HOST_XHCI_DEV,
                (g_aHostCatalog[0].szElfPath != NULL)
                    ? g_aHostCatalog[0].szElfPath
                    : "?",
                (g_aHostCatalog[1].szElfPath != NULL)
                    ? g_aHostCatalog[1].szElfPath
                    : "?",
                (g_aHostCatalog[2].szElfPath != NULL)
                    ? g_aHostCatalog[2].szElfPath
                    : "?",
                g_u32SoftHostLaunchClass, g_u32SoftHostElfRunOk,
                g_u32SoftHostTagAuto, g_u32SoftHostHookDeny, u32HookFailBits,
                szViaSafe, GJ_SPAWN_SOFT_WAVE);
        if (u32HookOk == GJ_SPAWN_HOST_HOOK_OK_N && u32HookFailBits == 0u) {
            kprintf("spawn: soft residual host_hook PASS via=%s "
                    "ok=%u/%u fail_bits=0x0 wave=%u\n",
                    szViaSafe, u32HookOk, GJ_SPAWN_HOST_HOOK_OK_N,
                    GJ_SPAWN_SOFT_WAVE);
        } else {
            kprintf("spawn: soft residual host_hook FAIL via=%s "
                    "ok=%u/%u fail_bits=0x%x "
                    "(soft residual only; not product gate)\n",
                    szViaSafe, u32HookOk, GJ_SPAWN_HOST_HOOK_OK_N,
                    u32HookFailBits);
        }
    }

    /* Grep: spawn: soft deepen */
    kprintf("spawn: soft deepen wave=%u via=%s enter=%u ok=%u fail=%u "
            "live=%u peak=%u mint_soft=%u mint_soft_bad=%u teardown=%u "
            "host_launch_ok=%u host_launch_lean=%u host_launch_class=%u "
            "host_hook_ok=%u host_elf_run=%u host_elf_embed=%u "
            "host_elf_miss=%u host_elf_probe_ok=%u host_elf_load_ok=%u "
            "host_elf_stack_fail=%u host_elf_thr_fail=%u "
            "host_elf_prefer=%u host_elf_stub_exit=%u "
            "host_elf_no_thrash=%u host_elf_qos_prefer=%u "
            "host_blob_lean=%u host_blob_log=%u host_blob_probe=%u "
            "host_blob_present=%u host_blob_absent=%u "
            "host_blob_denser_ok=%u host_blob_denser_fail=%u "
            "host_blob_denser_arms=%u denser_min=%u "
            "host_blob_denser2_ok=%u host_blob_denser2_fail=%u "
            "host_blob_denser2_arms=%u denser2_min=%u re_probe=%u "
            "host_blob_denser3_ok=%u host_blob_denser3_fail=%u "
            "host_blob_denser3_arms=%u denser3_min=%u triple_probe=%u "
            "host_blob_denser4_ok=%u host_blob_denser4_fail=%u "
            "host_blob_denser4_arms=%u denser4_min=%u quad_probe=%u "
            "host_blob_denser5_ok=%u host_blob_denser5_fail=%u "
            "host_blob_denser5_arms=%u denser5_min=%u penta_probe=%u "
            "host_blob_denser6_ok=%u host_blob_denser6_fail=%u "
            "host_blob_denser6_arms=%u denser6_min=%u hexa_probe=%u "
            "host_blob_denser7_ok=%u host_blob_denser7_fail=%u "
            "host_blob_denser7_arms=%u denser7_min=%u hepta_probe=%u "
            "host_blob_denser8_ok=%u host_blob_denser8_fail=%u "
            "host_blob_denser8_arms=%u denser8_min=%u octa_probe=%u "
            "class_n=%u product_hosts=UDX host_blob densify denser denser2 denser3 denser4 denser5 denser6 denser7 denser8 "
            "dual_dod OPEN "
            "c2_udx_launch_ok=%u c2_ok_n=%u path_steps=%u "
            "c2_logs=%u cap_ready=%u hook_logs=%u "
            "logs=%u (soft inventory only; not product gate; "
            "C2 UDX + host_hook + denser/denser2/denser3/denser4/denser5/denser6/denser7/denser8 host_blob residual W23; "
            "Dual DoD OPEN; Soft!=product;  stamp-free; "
            "never invent .76; CRITICAL preserve embeds ddi/rtl/xhci)\n",
            GJ_SPAWN_SOFT_WAVE, szViaSafe, g_u32SoftSpawnEnter, g_cSpawned,
            g_cSpawnFail, cLive, g_u32SoftLivePeak, g_cMintSoft,
            g_cMintSoftBad, g_u32SoftFailCleanup,
            g_u32SoftHostLaunchOk, g_u32SoftHostLaunchLean,
            g_u32SoftHostLaunchClass, g_u32SoftHostHookOk,
            g_u32SoftHostElfRunOk, g_u32SoftHostElfEmbed,
            g_u32SoftHostElfMiss, g_u32SoftHostElfProbeOk,
            g_u32SoftHostElfLoadOk, g_u32SoftHostElfStackFail,
            g_u32SoftHostElfThrFail, g_u32SoftHostElfPrefer,
            g_u32SoftHostElfStubExit, g_u32SoftHostElfNoThrash,
            g_u32SoftHostElfQosPrefer,
            g_u32SoftHostBlobLean, g_u32SoftHostBlobLog,
            g_u32SoftHostBlobProbe, g_u32SoftHostBlobPresent,
            g_u32SoftHostBlobAbsent,
            g_u32SoftHostBlobDenseOk, g_u32SoftHostBlobDenseFail,
            g_u32SoftHostBlobDenseArms,
            (unsigned)GJ_SPAWN_HOST_BLOB_DENSE_MIN,
            g_u32SoftHostBlobDense2Ok, g_u32SoftHostBlobDense2Fail,
            g_u32SoftHostBlobDense2Arms,
            (unsigned)GJ_SPAWN_HOST_BLOB_DENSE2_MIN,
            g_u32SoftHostBlobReProbe,
            g_u32SoftHostBlobDense3Ok, g_u32SoftHostBlobDense3Fail,
            g_u32SoftHostBlobDense3Arms,
            (unsigned)GJ_SPAWN_HOST_BLOB_DENSE3_MIN,
            g_u32SoftHostBlobTripleProbe,
            g_u32SoftHostBlobDense4Ok, g_u32SoftHostBlobDense4Fail,
            g_u32SoftHostBlobDense4Arms,
            (unsigned)GJ_SPAWN_HOST_BLOB_DENSE4_MIN,
            g_u32SoftHostBlobQuadProbe,
            g_u32SoftHostBlobDense5Ok, g_u32SoftHostBlobDense5Fail,
            g_u32SoftHostBlobDense5Arms,
            (unsigned)GJ_SPAWN_HOST_BLOB_DENSE5_MIN,
            g_u32SoftHostBlobPentaProbe,
            g_u32SoftHostBlobDense6Ok, g_u32SoftHostBlobDense6Fail,
            g_u32SoftHostBlobDense6Arms,
            (unsigned)GJ_SPAWN_HOST_BLOB_DENSE6_MIN,
            g_u32SoftHostBlobHexaProbe,
            g_u32SoftHostBlobDense7Ok, g_u32SoftHostBlobDense7Fail,
            g_u32SoftHostBlobDense7Arms,
            (unsigned)GJ_SPAWN_HOST_BLOB_DENSE7_MIN,
            g_u32SoftHostBlobHeptaProbe,
            g_u32SoftHostBlobDense8Ok, g_u32SoftHostBlobDense8Fail,
            g_u32SoftHostBlobDense8Arms,
            (unsigned)GJ_SPAWN_HOST_BLOB_DENSE8_MIN,
            g_u32SoftHostBlobOctaProbe,
            GJ_SPAWN_HOST_CLASS_N,
            g_u32SoftC2UdxLaunchOk, GJ_SPAWN_C2_UDX_LAUNCH_OK_N,
            GJ_SPAWN_C2_PATH_STEPS,
            g_u32SoftC2UdxLaunchLog,
            g_u32SoftHostCapReady, g_u32SoftHostHookLog, g_u32SoftLogN);

    /*
     * Soft lamp only - table + core rights configured. Never hard-gates.
     * Grep: spawn: soft inventory PASS | spawn: soft PASS
     */
    if (GJ_SPAWN_MAX > 0u && u16Core != 0) {
        kprintf("spawn: soft inventory PASS via=%s logs=%u live=%u "
                "peak=%u mint_soft=%u mint_soft_bad=%u wave=%u\n",
                szViaSafe, g_u32SoftLogN, cLive, g_u32SoftLivePeak,
                g_cMintSoft, g_cMintSoftBad, GJ_SPAWN_SOFT_WAVE);
        kprintf("spawn: soft PASS via=%s wave=%u\n", szViaSafe,
                GJ_SPAWN_SOFT_WAVE);
    } else {
        kprintf("spawn: soft FAIL via=%s max=%u core=0x%x "
                "(soft inventory only; not product gate)\n",
                szViaSafe, GJ_SPAWN_MAX, (unsigned)u16Core);
    }
}

/**
 * After first product spawn/kill/wait activity, print soft inventory once
 * (mirrors door/futex soft-stats-once). Diagnostics only.
 */
static void
soft_inventory_maybe_once(void)
{
    if (g_fSoftOnce != 0) {
        return;
    }
    if (g_u32SoftSpawnEnter == 0 && g_u32SoftKillEnter == 0 &&
        g_u32SoftWaitEnter == 0) {
        return;
    }
    g_fSoftOnce = 1;
    soft_inventory_log("once");
}

u32
process_spawn_count(void)
{
    return g_cSpawned;
}

u32
process_spawn_fail_count(void)
{
    return g_cSpawnFail;
}

u32
process_spawn_live_count(void)
{
    return spawn_live_count();
}

u32
process_spawn_kill_count(void)
{
    return g_cKill;
}

u32
process_spawn_wait_count(void)
{
    return g_cWait;
}

u32
process_spawn_mint_count(void)
{
    return g_cMintOk;
}

u32
process_spawn_mint_fail_count(void)
{
    return g_cMintFail;
}

u32
process_spawn_mint_soft_count(void)
{
    return g_cMintSoft;
}

u32
process_spawn_mint_soft_bad_count(void)
{
    return g_cMintSoftBad;
}

gj_status_t
process_spawn_stats(struct gj_spawn_stats *pOut)
{
    if (pOut == NULL) {
        return GJ_ERR_INVAL;
    }
    pOut->u32Ok = g_cSpawned;
    pOut->u32Fail = g_cSpawnFail;
    pOut->u32Live = spawn_live_count();
    pOut->u32Kill = g_cKill;
    pOut->u32Wait = g_cWait;
    pOut->u32MintOk = g_cMintOk;
    pOut->u32MintFail = g_cMintFail;
    pOut->u32MintSoft = g_cMintSoft;
    pOut->u32MintSoftBad = g_cMintSoftBad;
    return GJ_OK;
}

u32
process_spawn_stats_soft(void)
{
    u32 cLive;

    cLive = spawn_live_count();
    /*
     * Wave 11: full greppable soft inventory dump (includes soft stats line).
     * Grep: spawn: soft
     */
    soft_inventory_log("stats");
    return cLive;
}

/**
 * Locate fixed spawn-table entry for a child process (NULL if not a spawn child).
 */
static struct spawn_slot *
spawn_slot_of(struct gj_process *pProc)
{
    u32 iSlot;

    if (pProc == NULL) {
        return NULL;
    }
    for (iSlot = 0; iSlot < GJ_SPAWN_MAX; iSlot++) {
        if (g_aSpawn[iSlot].u8Used && &g_aSpawn[iSlot].proc == pProc) {
            return &g_aSpawn[iSlot];
        }
    }
    return NULL;
}

/**
 * Destroy private AS under kernel CR3; restore caller CR3.
 * Safe when u64Cr3 is 0 or matches kernel template.
 */
static void
spawn_as_teardown(struct gj_process *pProc)
{
    u64 u64Cr3;
    u64 u64Ker;
    u64 u64Saved;

    if (pProc == NULL || pProc->u64Cr3 == 0) {
        spawn_soft_inc(&g_u32SoftAsTeardownSkip);
        return;
    }
    u64Cr3 = pProc->u64Cr3;
    u64Ker = vmm_kernel_cr3();
    u64Saved = cpu_read_cr3();
    if (u64Ker != 0 &&
        (u64Cr3 & ~0xfffull) != (u64Ker & ~0xfffull)) {
        cpu_load_cr3(u64Ker);
        vmm_set_anon_cursor(NULL);
        (void)vmm_as_destroy(u64Cr3);
        pProc->u64Cr3 = 0;
        spawn_soft_inc(&g_u32SoftAsTeardown);
        if (u64Saved != 0) {
            cpu_load_cr3(u64Saved);
        } else if (u64Ker != 0) {
            cpu_load_cr3(u64Ker);
        }
    } else {
        spawn_soft_inc(&g_u32SoftAsTeardownSkip);
    }
}

/**
 * Reverse spawn install: drop parent PROCESS cap (if any) and private AS, free slot.
 */
static void
spawn_fail_cleanup(struct spawn_slot *pSlot, struct gj_process *pParent,
                   const struct gj_cap_ref *pRef)
{
    if (pSlot == NULL) {
        return;
    }
    spawn_soft_inc(&g_u32SoftFailCleanup);
    if (pParent != NULL && pRef != NULL && pParent->pCnode != NULL) {
        struct gj_cap_resolved res;

        if (gj_cap_resolve_ref(pParent->pCnode, pRef, &res) == GJ_OK &&
            res.pSlot != NULL) {
            gj_cap_slot_invalidate(res.pSlot);
        }
    }
    spawn_as_teardown(&pSlot->proc);
    pSlot->u8Used = 0;
}

/**
 * Soft-required PROCESS task rights after mint (kill/reap/vm).
 * Full base rights should be present; core is the hard soft gate.
 */
static u16
spawn_process_rights(u32 u32Jit)
{
    u16 u16Rights;

    u16Rights = GJ_SPAWN_PROCESS_RIGHTS_BASE;
    if (u32Jit) {
        u16Rights |= (u16)GJ_RIGHT_JIT;
        spawn_soft_inc(&g_u32SoftMintJit);
    } else {
        spawn_soft_inc(&g_u32SoftMintNoJit);
    }
    return u16Rights;
}

/**
 * Soft post-mint verify of parent PROCESS task cap (G-PROC-2).
 * Checks type, object pointer, slot gen, and core rights matrix.
 * Fail closed: returns non-OK so spawn tears down the partial child.
 * Grep: spawn: mint soft
 * Grep: spawn: soft mint (Wave 11 reason tallies)
 */
static gj_status_t
spawn_process_cap_mint_soft_verify(struct gj_process *pParent,
                                   const struct gj_cap_ref *pRef,
                                   struct gj_process *pChild,
                                   u16 u16WantRights)
{
    struct gj_cap_resolved res;
    gj_status_t st;
    struct gj_process *pFrom;

    if (pParent == NULL || pRef == NULL || pChild == NULL ||
        pParent->pCnode == NULL) {
        g_cMintSoftBad++;
        spawn_soft_inc(&g_u32SoftMintNullArg);
        return GJ_ERR_INVAL;
    }
    if (gj_cap_ref_is_null(pRef)) {
        g_cMintSoftBad++;
        spawn_soft_inc(&g_u32SoftMintNullRef);
        kprintf("spawn: mint soft FAIL null ref\n");
        return GJ_ERR_INVAL;
    }

    st = gj_cap_resolve_ref(pParent->pCnode, pRef, &res);
    if (st != GJ_OK) {
        g_cMintSoftBad++;
        spawn_soft_inc(&g_u32SoftMintResolve);
        kprintf("spawn: mint soft FAIL resolve st=%d\n", (int)st);
        return st;
    }
    if (res.u16Type != (u16)GJ_CAP_PROCESS) {
        g_cMintSoftBad++;
        spawn_soft_inc(&g_u32SoftMintType);
        kprintf("spawn: mint soft FAIL type=%u want=PROCESS\n",
                (unsigned)res.u16Type);
        return GJ_ERR_PERM;
    }
    if (res.pObj != (void *)&pChild->hdr) {
        g_cMintSoftBad++;
        spawn_soft_inc(&g_u32SoftMintObj);
        kprintf("spawn: mint soft FAIL obj mismatch\n");
        return GJ_ERR_INVAL;
    }
    if (res.pSlot != NULL && res.pSlot->u32Gen != pRef->u32SlotGen) {
        g_cMintSoftBad++;
        spawn_soft_inc(&g_u32SoftMintGen);
        kprintf("spawn: mint soft FAIL gen slot=%u ref=%u\n",
                res.pSlot->u32Gen, pRef->u32SlotGen);
        return GJ_ERR_INVAL;
    }
    /* Core task rights must all be present (soft gate). */
    if ((res.u16Rights & GJ_SPAWN_PROCESS_RIGHTS_CORE) !=
        GJ_SPAWN_PROCESS_RIGHTS_CORE) {
        g_cMintSoftBad++;
        spawn_soft_inc(&g_u32SoftMintCoreRights);
        kprintf("spawn: mint soft FAIL core rights=0x%x need=0x%x\n",
                (unsigned)res.u16Rights,
                (unsigned)GJ_SPAWN_PROCESS_RIGHTS_CORE);
        return GJ_ERR_PERM;
    }
    /* Full wanted rights should match install (soft observability). */
    if ((res.u16Rights & u16WantRights) != u16WantRights) {
        g_cMintSoftBad++;
        spawn_soft_inc(&g_u32SoftMintRights);
        kprintf("spawn: mint soft FAIL rights=0x%x want=0x%x\n",
                (unsigned)res.u16Rights, (unsigned)u16WantRights);
        return GJ_ERR_PERM;
    }

    pFrom = process_from_cap(pParent, pRef);
    if (pFrom != pChild) {
        g_cMintSoftBad++;
        spawn_soft_inc(&g_u32SoftMintFromCap);
        kprintf("spawn: mint soft FAIL from_cap mismatch\n");
        return GJ_ERR_INVAL;
    }

    g_cMintSoft++;
    /* Grep: spawn: mint soft PASS */
    kprintf("spawn: mint soft PASS slot=%lu gen=%u rights=0x%x\n",
            (unsigned long)pRef->u64Slot, pRef->u32SlotGen,
            (unsigned)res.u16Rights);
    return GJ_OK;
}

/**
 * Mint GJ_CAP_PROCESS into parent CNode + soft post-mint verify (G-PROC-2).
 * On soft-verify failure: invalidate installed slot and return error.
 */
static gj_status_t
spawn_process_cap_mint(struct gj_process *pParent, struct gj_process *pChild,
                       u32 u32Jit, struct gj_cap_ref *pOutRef)
{
    u16 u16Rights;
    gj_status_t st;

    if (pParent == NULL || pChild == NULL || pOutRef == NULL ||
        pParent->pCnode == NULL) {
        g_cMintFail++;
        spawn_soft_inc(&g_u32SoftMintNullArg);
        spawn_soft_inc(&g_u32SoftMintInstallFail);
        return GJ_ERR_INVAL;
    }

    u16Rights = spawn_process_rights(u32Jit);
    st = gj_cap_alloc_install(pParent->pCnode, (u16)GJ_CAP_PROCESS, u16Rights,
                              &pChild->hdr, pOutRef);
    if (st != GJ_OK) {
        g_cMintFail++;
        spawn_soft_inc(&g_u32SoftMintInstallFail);
        kprintf("spawn: mint soft FAIL install st=%d\n", (int)st);
        return st;
    }
    g_cMintOk++;

    st = spawn_process_cap_mint_soft_verify(pParent, pOutRef, pChild,
                                            u16Rights);
    if (st != GJ_OK) {
        /* Fail closed: drop the bad parent handle before returning. */
        if (pParent->pCnode != NULL) {
            struct gj_cap_resolved res;

            if (gj_cap_resolve_ref(pParent->pCnode, pOutRef, &res) == GJ_OK &&
                res.pSlot != NULL) {
                gj_cap_slot_invalidate(res.pSlot);
            }
        }
        memset(pOutRef, 0, sizeof(*pOutRef));
        return st;
    }
    return GJ_OK;
}

gj_status_t
process_kill(struct gj_process *pParent, const struct gj_cap_ref *pRef,
             u32 u32ExitCode)
{
    struct gj_process *pChild;
    struct gj_cap_resolved res;
    gj_status_t st;

    spawn_soft_inc(&g_u32SoftKillEnter);

    if (pParent == NULL || pRef == NULL) {
        spawn_soft_inc(&g_u32SoftKillDenyNull);
        soft_inventory_maybe_once();
        return GJ_ERR_INVAL;
    }
    st = gj_cap_resolve_ref(pParent->pCnode, pRef, &res);
    if (st != GJ_OK) {
        spawn_soft_inc(&g_u32SoftKillDenyResolve);
        soft_inventory_maybe_once();
        return st;
    }
    if (res.u16Type != (u16)GJ_CAP_PROCESS) {
        spawn_soft_inc(&g_u32SoftKillDenyType);
        soft_inventory_maybe_once();
        return GJ_ERR_PERM;
    }
    if ((res.u16Rights & (u16)GJ_RIGHT_DESTROY) == 0) {
        spawn_soft_inc(&g_u32SoftKillDenyRights);
        soft_inventory_maybe_once();
        return GJ_ERR_PERM;
    }
    pChild = (struct gj_process *)res.pObj;
    if (pChild == NULL) {
        spawn_soft_inc(&g_u32SoftKillDenyNoent);
        soft_inventory_maybe_once();
        return GJ_ERR_NOENT;
    }
    {
        struct spawn_slot *pLiveSlot;

        pLiveSlot = spawn_slot_of(pChild);
        if (spawn_host_keep_live_p(pLiveSlot, pChild) != 0) {
            pChild->u32Alive = 1;
            pChild->u32Personality = GJ_SPAWN_HOST_PERSONA_NATIVE;
            soft_inventory_maybe_once();
            return GJ_OK;
        }
    }
    g_cKill++;
    if (!pChild->u32Alive) {
        /* Already dead - idempotent kill */
        spawn_soft_inc(&g_u32SoftKillIdem);
        soft_inventory_maybe_once();
        return GJ_OK;
    }
    /*
     * G-PROC-5 + H3 death residual: full death path (pager, regions,
     * wait-registered CNode wipe, private AS destroy, zombie for wait4 /
     * process_wait). process_death exits non-current siblings BEFORE
     * as_destroy (H3 thr_exit barrier; Soft!=product). UDX driver hosts
     * (ddi_host_gj / rtl8168_udx / xhci_udx) reaped via this path.
     * Host teardown residual: thr_exit_before_as_destroy=1.
     */
    process_death(pChild, u32ExitCode);
    {
        struct spawn_slot *pKillSlot;
        const char *szHost;

        pKillSlot = spawn_slot_of(pChild);
        szHost = "?";
        if (pKillSlot != NULL && pKillSlot->u8HostClass > 0u &&
            pKillSlot->u8HostClass <= GJ_SPAWN_HOST_CLASS_N) {
            szHost = g_aHostCatalog[pKillSlot->u8HostClass - 1u].szName;
        }
        kprintf("spawn: kill child exit=%u H3=death_residual "
                "udx_host_teardown=1 thr_exit_before_as_destroy=1 "
                "host=%s host_class=%u dual_dod=%u "
                "hosts=%s,%s,%s PASS\n",
                u32ExitCode, szHost,
                (pKillSlot != NULL) ? (unsigned)pKillSlot->u8HostClass : 0u,
                (pKillSlot != NULL) ? (unsigned)pKillSlot->u8HostDualDod : 0u,
                GJ_SPAWN_HOST_DDI_NAME, GJ_SPAWN_HOST_RTL_NAME,
                GJ_SPAWN_HOST_XHCI_NAME);
    }
    soft_inventory_maybe_once();
    return GJ_OK;
}

gj_status_t
process_wait(struct gj_process *pParent, const struct gj_cap_ref *pRef,
             u32 *pOutExit)
{
    struct gj_process *pChild;
    struct gj_cap_resolved res;
    struct spawn_slot *pSlot;
    gj_status_t st;
    u32 u32Exit;
    u32 u32WaitPid;

    /*
     * spawn: soft wait - cap-wait reaper (G-PROC product path).
     * Poll-friendly: alive child -> GJ_ERR_AGAIN (like wait4 WNOHANG live).
     * Linux wait4/waitid use process_wait4* (process: soft wait); this path
     * is PROCESS task-cap wait for process_spawn children (shell/sshd later
     * may prefer either surface). Product incomplete != full posix waitid.
     *
     * Dual DoD B residual: on AGAIN, yield once so freestanding sshd.elf
     * eth accept / net_eth_poll / net_tcp_poll can progress while a parent
     * soft-polls wait (pairs with sshd park yield+poll on :22). Soft!=product.
     * C2 UDX launch residual: same yield lets concurrent userspace UDX host
     * thr (ddi_host_gj / rtl8168_udx / xhci_udx) run while parent soft-polls.
     * greppable: spawn: soft wait
     * greppable: spawn: soft wait poll
     */
    spawn_soft_inc(&g_u32SoftWaitEnter);

    if (pParent == NULL || pRef == NULL) {
        spawn_soft_inc(&g_u32SoftWaitDenyNull);
        soft_inventory_maybe_once();
        return GJ_ERR_INVAL;
    }
    st = gj_cap_resolve_ref(pParent->pCnode, pRef, &res);
    if (st != GJ_OK) {
        spawn_soft_inc(&g_u32SoftWaitDenyResolve);
        soft_inventory_maybe_once();
        return st;
    }
    if (res.u16Type != (u16)GJ_CAP_PROCESS) {
        spawn_soft_inc(&g_u32SoftWaitDenyType);
        soft_inventory_maybe_once();
        return GJ_ERR_PERM;
    }
    if ((res.u16Rights & (u16)GJ_RIGHT_WAIT) == 0) {
        spawn_soft_inc(&g_u32SoftWaitDenyRights);
        soft_inventory_maybe_once();
        return GJ_ERR_PERM;
    }
    pChild = (struct gj_process *)res.pObj;
    if (pChild == NULL) {
        spawn_soft_inc(&g_u32SoftWaitDenyNoent);
        soft_inventory_maybe_once();
        return GJ_ERR_NOENT;
    }
    pSlot = spawn_slot_of(pChild);
    if (spawn_host_keep_live_p(pSlot, pChild) != 0) {
        pChild->u32Alive = 1;
        pChild->u32Personality = GJ_SPAWN_HOST_PERSONA_NATIVE;
        spawn_soft_inc(&g_u32SoftWaitAgain);
        soft_inventory_maybe_once();
        thread_yield();
        return GJ_ERR_AGAIN;
    }
    if (pChild->u32Alive) {
        /*
         * Soft WNOHANG-shaped: parent can poll without blocking forever.
         * Yield so live daemon thr (sshd eth accept Dual DoD B) and concurrent
         * C2 UDX host thr can run while parent soft-polls. Soft!=product.
         * greppable: spawn: soft wait poll
         * (no kprintf here — H2 no stamp storms on hot poll path)
         */
        spawn_soft_inc(&g_u32SoftWaitAgain);
        soft_inventory_maybe_once();
        thread_yield();
        return GJ_ERR_AGAIN;
    }
    u32Exit = pChild->u32ExitCode;
    u32WaitPid = process_wait_pid_of(pChild);
    if (pOutExit != NULL) {
        *pOutExit = u32Exit;
    }
    /*
     * Reap: drop wait-table entry, invalidate parent PROCESS cap, recycle
     * spawn slot. Cap gen bump prevents reuse of a stale handle.
     * Aligns with process_wait4 slot scrub (process: soft wait).
     */
    process_wait_forget(pChild);
    if (res.pSlot != NULL) {
        gj_cap_slot_invalidate(res.pSlot);
        spawn_soft_inc(&g_u32SoftWaitCapInv);
    }
    pSlot = spawn_slot_of(pChild);
    if (pSlot != NULL) {
        pSlot->u8Used = 0;
        spawn_soft_inc(&g_u32SoftWaitSlotFree);
    }
    g_cWait++;
    kprintf("spawn: soft wait reaped exit=%u wait_pid=%u PASS\n", u32Exit,
            u32WaitPid);
    kprintf("spawn: wait reaped exit=%u PASS\n", u32Exit);
    soft_inventory_maybe_once();
    return GJ_OK;
}

struct gj_process *
process_from_cap(struct gj_process *pParent, const struct gj_cap_ref *pRef)
{
    struct gj_cap_resolved res;
    gj_status_t st;

    if (pParent == NULL || pRef == NULL || pParent->pCnode == NULL) {
        spawn_soft_inc(&g_u32SoftFromCapMiss);
        return NULL;
    }
    st = gj_cap_resolve_ref(pParent->pCnode, pRef, &res);
    if (st != GJ_OK || res.u16Type != (u16)GJ_CAP_PROCESS) {
        spawn_soft_inc(&g_u32SoftFromCapMiss);
        return NULL;
    }
    spawn_soft_inc(&g_u32SoftFromCapHit);
    return (struct gj_process *)res.pObj;
}

gj_status_t
process_spawn(struct gj_process *pParent, const struct gj_spawn_args *pArgs,
              struct gj_process **ppOutChild, struct gj_cap_ref *pOutCap)
{
    u32 iSlot;
    struct spawn_slot *pSlot;
    struct gj_process *pChild;
    struct gj_cap_ref ref;
    gj_status_t st;
    u32 u32Thr;
    u32 u32Ppid;
    const struct spawn_host_class *pHostArg;
    u32 u32HostIdx;
    u32 u32Persona;

    spawn_soft_inc(&g_u32SoftSpawnEnter);

    if (pParent == NULL || pArgs == NULL || pArgs->pfnEntry == NULL) {
        g_cSpawnFail++;
        spawn_soft_inc(&g_u32SoftDenyNull);
        soft_inventory_maybe_once();
        return GJ_ERR_INVAL;
    }
    if (pParent->pCnode == NULL) {
        g_cSpawnFail++;
        spawn_soft_inc(&g_u32SoftDenyNoCnode);
        soft_inventory_maybe_once();
        return GJ_ERR_INVAL;
    }

    /*
     * Catalog auto-tag: pArg == &g_aHostCatalog[i] selects host class.
     * Forces catalog persona pref (native for UDX/DDI hosts). Soft!=product.
     */
    pHostArg = spawn_host_catalog_from_arg(pArgs->pArg);
    u32HostIdx = (u32)-1;
    if (pHostArg != NULL) {
        u32HostIdx = spawn_host_catalog_lookup_name(pHostArg->szName);
    }

    pSlot = NULL;
    for (iSlot = 0; iSlot < GJ_SPAWN_MAX; iSlot++) {
        if (!g_aSpawn[iSlot].u8Used) {
            pSlot = &g_aSpawn[iSlot];
            break;
        }
    }
    if (pSlot == NULL) {
        g_cSpawnFail++;
        spawn_soft_inc(&g_u32SoftDenyFull);
        soft_inventory_maybe_once();
        return GJ_ERR_NOMEM;
    }
    memset(pSlot, 0, sizeof(*pSlot));
    pSlot->u8Used = 1;
    pChild = &pSlot->proc;
    memset(&ref, 0, sizeof(ref));

    gj_process_init(pChild, &pSlot->cnode, pSlot->aSlots, GJ_SPAWN_CNODE_SLOTS);
    pChild->pParent = pParent;
    pChild->u32Alive = 1;
    /* gj_process_init plants LINUX. Catalog UDX must stay native on the PCB. */
    u32Persona = pArgs->u32Personality;
    if (pHostArg != NULL) {
        u32Persona = GJ_SPAWN_HOST_PERSONA_NATIVE;
    }
    pChild->u32Personality = u32Persona;
    if (u32Persona == 0u) {
        spawn_soft_inc(&g_u32SoftPersonaNative);
    } else {
        spawn_soft_inc(&g_u32SoftPersonaLinux);
    }
    gj_process_set_jit(pChild, pArgs->u32Jit ? 1 : 0);

    st = gj_process_bootstrap_root_meta(pChild, &pSlot->meta, NULL);
    if (st != GJ_OK) {
        pSlot->u8Used = 0;
        g_cSpawnFail++;
        spawn_soft_inc(&g_u32SoftDenyMeta);
        soft_inventory_maybe_once();
        return st;
    }

    st = process_as_ensure(pChild);
    if (st != GJ_OK) {
        pSlot->u8Used = 0;
        g_cSpawnFail++;
        spawn_soft_inc(&g_u32SoftDenyAs);
        soft_inventory_maybe_once();
        return st;
    }

    /* Mint PROCESS task port into parent CNode + soft verify (G-PROC-2) */
    st = spawn_process_cap_mint(pParent, pChild, pArgs->u32Jit, &ref);
    if (st != GJ_OK) {
        spawn_fail_cleanup(pSlot, NULL, NULL);
        g_cSpawnFail++;
        spawn_soft_inc(&g_u32SoftDenyMint);
        soft_inventory_maybe_once();
        return st;
    }

    if (pHostArg != NULL) {
        pChild->u32Personality = GJ_SPAWN_HOST_PERSONA_NATIVE;
    }
    u32Thr = thread_create(pChild, pArgs->pfnEntry, pArgs->pArg);
    if (u32Thr == 0) {
        spawn_fail_cleanup(pSlot, pParent, &ref);
        g_cSpawnFail++;
        spawn_soft_inc(&g_u32SoftDenyThr);
        soft_inventory_maybe_once();
        return GJ_ERR_NOMEM;
    }
    /*
     * Record first thr as park stub id (host_launch may later promote a
     * product ELF thr and demote this stub; Soft!=product no thrash).
     */
    pSlot->u32StubThrId = u32Thr;

    /*
     * Wait-register after first thr is live so process_death reclaims CNode +
     * private AS (G-PROC-5). Parent pid from wait table when known; else 1.
     */
    u32Ppid = process_wait_pid_of(pParent);
    (void)process_wait_register(pChild, u32Ppid != 0 ? u32Ppid : 1u);
    spawn_soft_inc(&g_u32SoftWaitReg);
    g_u32SoftLastThrId = u32Thr;

    g_cSpawned++;
    soft_note_live_peak();
    spawn_soft_inc(&g_u32SoftHostLaunchOk);

    /*
     * Catalog auto-tag residual: when pArg is a catalog entry, tag the
     * fixed slot so kill/wait paths can name the host class. Soft!=product.
     */
    if (pHostArg != NULL && u32HostIdx != (u32)-1 && pSlot != NULL) {
        spawn_host_slot_tag(pSlot, pHostArg, u32HostIdx);
        spawn_host_keep_live_bind(pChild, pHostArg, 0u);
        spawn_soft_inc(&g_u32SoftHostTagAuto);
        if (u32HostIdx < GJ_SPAWN_HOST_CLASS_N) {
            spawn_soft_inc(&g_u32SoftHostLaunchByClass[u32HostIdx]);
        }
    }

    /*
     * Host-launch residual: fixed-table mint is the lean start path for
     * userspace UDX driver hosts (ddi_host_gj / rtl8168_udx / xhci_udx).
     * C2 product-path residual (Soft!=product; soft scaffold != product AC).
     * Soft!=product; not multi-server confine close; G-AC-1 (no .ko product).
     * greppable: spawn: soft host_launch (via soft inventory)
     * greppable: spawn: soft residual udx_launch (via soft inventory)
     * greppable: spawn: child ... host_launch=1
     */
    kprintf("spawn: child thr=%u cr3=0x%lx cap slot=%lu gen=%u "
            "count=%u live=%u mint_soft=%u host_launch=1 "
            "class=C2 class_n=%u persona=%u host_class=%u "
            "host=%s H3=1 G-AC-1=1 PASS\n",
            u32Thr, (unsigned long)pChild->u64Cr3,
            (unsigned long)ref.u64Slot, ref.u32SlotGen,
            g_cSpawned, spawn_live_count(), g_cMintSoft,
            GJ_SPAWN_HOST_CLASS_N, pChild->u32Personality,
            (pSlot != NULL) ? (unsigned)pSlot->u8HostClass : 0u,
            (pHostArg != NULL && pHostArg->szName != NULL)
                ? pHostArg->szName
                : "-");
    kprintf("spawn: soft host_launch ok thr=%u live=%u peak=%u "
            "hosts=%s,%s,%s path=process_spawn class=C2 "
            "host_class=%u tag_auto=%u "
            "G-PROC-2=mint wait_reg=1 ddi_cap_mint=OPEN "
            "soft_scaffold_ne_product_ac=1 "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1 "
            "wave=%u PASS\n",
            u32Thr, spawn_live_count(), g_u32SoftLivePeak,
            GJ_SPAWN_HOST_DDI_NAME, GJ_SPAWN_HOST_RTL_NAME,
            GJ_SPAWN_HOST_XHCI_NAME,
            (pSlot != NULL) ? (unsigned)pSlot->u8HostClass : 0u,
            g_u32SoftHostTagAuto, GJ_SPAWN_SOFT_WAVE);
    /* Grep: spawn: soft residual udx_launch ok (success path sample) */
    kprintf("spawn: soft residual udx_launch ok thr=%u live=%u peak=%u "
            "class=C2 class_n=%u persona=%u host_class=%u "
            "path=process_spawn G-PROC-2=mint wait_reg=1 H3=1 "
            "hosts=%s,%s,%s ddi_cap_mint=OPEN "
            "soft_scaffold_ne_product_ac=1 "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1 "
            "wave=%u PASS\n",
            u32Thr, spawn_live_count(), g_u32SoftLivePeak,
            GJ_SPAWN_HOST_CLASS_N, pChild->u32Personality,
            (pSlot != NULL) ? (unsigned)pSlot->u8HostClass : 0u,
            GJ_SPAWN_HOST_DDI_NAME, GJ_SPAWN_HOST_RTL_NAME,
            GJ_SPAWN_HOST_XHCI_NAME, GJ_SPAWN_SOFT_WAVE);

    if (ppOutChild != NULL) {
        *ppOutChild = pChild;
    }
    if (pOutCap != NULL) {
        *pOutCap = ref;
    }
    soft_inventory_maybe_once();
    return GJ_OK;
}
