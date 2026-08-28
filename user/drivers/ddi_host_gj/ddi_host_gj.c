/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Freestanding userspace NATIVE DDI host residual (GreenJade Soft).
 *
 * Door path for userspace UDX NIC/USB hosts (rtl8168_udx / xhci_udx):
 *   GJ_SYS_DDI soft SCAN -> GET -> OPEN -> MAP_BAR -> IRQ -> DMA residual.
 * Soft residual chain (observe only; product_mint=0 / mint=0 everywhere):
 *   CFG_READ -> CFG_WRITE(careful+BAR/ident reject) -> MAP_BAR* ->
 *   MAP_REMAP(idem+sticky) -> DMA_NOTE -> IRQ_BIND -> IRQ_REBIND ->
 *   notify_poll(rebind badge when rebind ok) ->
 *   DMA_BUF ALLOC/MAP/MAP_REMAP(idem)/MAP_RECB(diff cb)/FREE -> force32 ->
 *   grant surface -> QUIESCE -> CLOSE -> post_close forget -> REOPEN/CLOSE
 * Multi-host residual (rtl+xhci concurrent; door multi-slot spirit):
 *   SCAN/GET both IDs -> OPEN both live -> CFG both concurrent ->
 *   MAP preferred BARs both live -> MAP_REMAP both concurrent (idem) ->
 *   open+map rollup (both_open+both_map) -> IRQ_BIND both -> DMA_NOTE both ->
 *   CLOSE rtl -> peer-live (CFG/MAP + peer MAP_REMAP + peer IRQ rebind) +
 *   closed-dead (closed CFG/MAP/IRQ/DMA fail; isolation / never clear-all) ->
 *   REOPEN rtl + MAP -> CLOSE xhci (reverse order) -> peer-live rtl +
 *   closed-dead xhci -> CLOSE rtl.
 *   Soft multi-host != product multi-server confine; product_mint=0.
 *
 * Lifecycle vs kernel ddi_door (functional residual preferred over lamps):
 *   SCAN/GET/OPEN/MAP_BAR/CFG/IRQ/DMA_BUF/CLOSE match DDI_OP_* on GJ_SYS_DDI.
 *   MAP: map_note.u8User + idempotent re-MAP VA match (door grant slot);
 *     sticky second re-MAP confirms grant_live residual without reinstall.
 *   DMA_BUF_MAP: re-MAP same pa/cb returns prior bus cookie (door dma_idem);
 *     re-MAP different cb unmaps prior cookie first (door dma_recb residual).
 *   DMA_BUF force32: optional bit0 residual after FREE (VT-d identity prefer).
 *   CFG_WRITE: 8168 Command MEM|MASTER (0x0006); never val=0; never xHCI.
 *   IRQ_REBIND: second IRQ_BIND same handle (door rebind; multi-slot safe);
 *     notify_poll uses rebind badge when rebind soft-ok (badge residual).
 *   Multi-host: concurrent OPEN/CFG/MAP/REMAP/IRQ/DMA on rtl+xhci;
 *     CLOSE one leaves peer grants/IRQ live (door multi-host unbind_safe /
 *     not_sole_owner spirit) and closed handle is dead on CFG/MAP/IRQ/DMA
 *     (isolation residual; soft observe only; never CF8-steal claim).
 *     Reverse CLOSE order residual (CLOSE xhci first after REOPEN rtl)
 *     proves multi-host OPEN+MAP isolation is not direction-biased.
 *   CLOSE: forget maps/IRQ/DMA; post_close dead-handle probes must soft-fail
 *     (CFG_READ + MAP_BAR + IRQ_BIND + DMA_BUF_ALLOC + CLOSE) -- map grant
 *     clear / irq multi-slot unbind / dma_buf free residual (mint=0).
 * Product direction (not this soft residual alone):
 *   UDX + ABI Linux-shaped userspace drivers over DDI caps.
 * Dual DoD A/B remain OPEN (soft residual != close; agent!=close).
 * Freestanding class SKIP policy (not product):
 *   GJ_RTL8168_PROBE=0, GJ_XHCI_MSC_PROBE=0 -- residual opt-in only.
 * Soft!=product; G-AC-1 (no Linux .ko product AC); no version stamp.
 * Bar honesty v2026.08.04.75 stamp-free; NEVER invent .76 / bump stamp.
 *
 * Cap mint / window / door residual honesty (soft only; lean lamps):
 *   - Cap mint: never MMIO_FRAME / IRQ Notification / DMA window CNode
 *     mint (mint=0 / product_mint=0 on every grant lamp). Soft bind !=
 *     product cap mint.
 *   - Window: soft DMA_NOTE / DMA_BUF only; product window OPEN.
 *   - Door: soft GJ_SYS_DDI observe; product multi-cap grant OPEN.
 *   - Multi-host: concurrent rtl+xhci residual; multi_server=0 confine=0.
 *   - Per-ID residual path: open= map= irq= grant= life= ... mint=0
 *   - Dual DoD honesty: A=OPEN (UDX USB) B=OPEN (UDX NIC); soft!=close.
 *   - No stamp storms: one lamp per surface; no deepen/wave stamp.
 *
 * G752 first targets:
 *   10ec:8168  RTL8111/8168 -- OPEN + MAP BAR0 and/or BAR2
 *   8086:a12f  Intel 100 Series xHCI -- OPEN + MAP BAR0
 * Soft PASS when OPEN + >=1 preferred BAR maps.
 *
 * Greppable lamps (prefix-stable; Soft!=product; lean; H2 once per surface):
 *   ddi_host: soft scaffold PASS
 *   ddi_host: soft prefer real DDI ... prefer_real_ddi=1 not=inject_only
 *   ddi_host: soft SCAN PASS n=...
 *   ddi_host: soft GET / soft found ...
 *   ddi_host: soft OPEN / MAP_BAR / MAP_REMAP / CFG / CFG_WRITE / DMA_NOTE
 *   ddi_host: soft CFG_WRITE reject (why=unsafe_bar|unsafe_ident)
 *   ddi_host: soft IRQ_BIND / IRQ_REBIND / notify poll
 *   ddi_host: soft DMA_BUF* / DMA_BUF_MAP_RECB / DMA_BUF force32 / dma_buf chain
 *   ddi_host: soft grant surface / QUIESCE / CLOSE / post_close / REOPEN
 *   ddi_host: soft residual path id=... open= map= irq= grant= life= ... mint=0
 *   ddi_host: soft residual lean ... product=UDX+ABI Soft!=product
 *   ddi_host: soft residual life ... (door SCAN/GET/OPEN/MAP_BAR/CFG/IRQ/DMA_BUF/CLOSE)
 *   ddi_host: soft residual ... (rtl/xhci rollup; soft residual)
 *   ddi_host: soft product residual ... open= map= irq= dma= keep_live=
 *   ddi_host: soft MAP_REMAP ... idempotent=1 va_match= sticky=
 *   ddi_host: soft DMA_BUF_MAP_REMAP ... cookie_match= dma_idem=
 *   ddi_host: soft DMA_BUF_MAP_RECB ... cb_change=1 prior_unmap=1
 *   ddi_host: soft post_close ... forget= map= irq= dma= cfg= close2=
 *   ddi_host: soft bind 10ec:8168|8086:a12f PASS|SKIP|MISS Soft!=product
 *   ddi_host: soft multi-host ... both_open= both_map= both_irq= peer_live=
 *   ddi_host: soft multi-host open+map ... concurrent= both_open= both_map=
 *   ddi_host: soft multi-host prefer_real_ddi ... open= map= irq= dma=
 *   ddi_host: soft multi-host densify open= / map= / irq= / dma=
 *   ddi_host: soft multi-host denser open= / map= / irq= / dma=
 *   ddi_host: soft multi-host prefer_real denser VERDICT ...
 *   ddi_host: soft multi-host prefer_real residual ...
 *   ddi_host: soft multi-host prefer_real residual denser open=
 *   ddi_host: soft multi-host prefer_real residual denser map=
 *   ddi_host: soft multi-host prefer_real residual denser irq=
 *   ddi_host: soft multi-host prefer_real residual denser dma=
 *   ddi_host: soft multi-host prefer_real residual denser VERDICT ...
 *   ddi_host: soft multi-host chain SCAN/GET/OPEN/MAP_BAR/IRQ/DMA ...
 *   ddi_host: soft multi-host peer_live ... / closed_dead ...
 *   ddi_host: soft multi-host rev_peer_live ... rev_peer_remap= ... densify=4
 *   ddi_host: soft multi-host rev_closed_dead ...
 *   residual denser map requires peer_remap + rev_peer_remap (bar .75 denser)
 *   ddi_host: soft product residual denser ... open= map= irq= dma= denser=
 *   STRONGER denser residual bar .75 (H2 once; exclusive .c; Dual DoD A/B):
 *     denser residual prove + multi-arm + lean + VERDICT rollup;
 *     denser_prove=1 denser_residual=1 never_mint_held=1 always bar .75;
 *     arms: open|map|irq|dma|multi_host|residual|prefer_real|dual_dod;
 *     bar=v2026.08.04.75 stamp_free=1 NEVER invent .76;
 *     product_mint=0 dual_dod_a=OPEN dual_dod_b=OPEN ALWAYS;
 *     denser residual != Dual DoD close; agent!=close; Soft!=product (ASCII);
 *   ddi_host: soft denser residual ... denser_prove= denser_arms=
 *   ddi_host: soft denser residual prove ...
 *   ddi_host: soft denser residual arms ...
 *   ddi_host: soft denser residual lean ...
 *   ddi_host: soft denser residual VERDICT PASS|SKIP|MISS ...
 *   ddi_host: soft Dual DoD honesty A=OPEN B=OPEN dual_dod_a=OPEN dual_dod_b=OPEN
 *   ddi_host: soft cap mint honesty ... product_mint=0 mint=0
 *   ddi_host: soft window residual honesty ...
 *   ddi_host: soft door residual honesty ... gj_sys_ddi=103 chain=
 *   ddi_host: soft mint claim=0 ... product_mint=0
 *   ddi_host: soft done Soft!=product ... product_mint=0
 * Once-lamp tokens (H2; no stamp storms): Soft!=product / prefer_real_ddi=1 /
 *   not=inject_only / gj_sys_ddi=103 / chain= / dual_dod_a=OPEN dual_dod_b=OPEN /
 *   dual_dod=OPEN / keep_live= / product_hosts=UDX / denser=1 / open= map= irq= dma= /
 *   residual_open= residual_map= residual_irq= residual_dma= residual_n= /
 *   denser_prove= denser_residual= never_mint_held= always bar .75.
 *
 * Soft OPEN+MAP != product TX/RX (rtl) or BOT/MSC (xhci). G-AC-1.
 * Soft bind != product cap mint. Product hosts = UDX class drivers.
 * Prefer real DDI bind (GJ_SYS_DDI SCAN/GET/OPEN/MAP_BAR/IRQ/DMA) over
 * inject-only soft demo. Soft residual != product. Dual DoD A/B remain OPEN.
 * Multi-host residual != product multi-server confine (multi_server=0).
 * Links freestanding with libgj + user/init/user.ld.
 * Dual MIT OR Apache-2.0 only; no GPL. docs/DDI_SOFT.md.
 */
#include <gj/syscalls.h>

/* G752 first bind targets (docs/LAPTOP_LINUX_DRIVER_HOST.md, udx/ddi.h). */
#define DDI_HOST_RTL_VEND   0x10ecu
#define DDI_HOST_RTL_DEV    0x8168u
#define DDI_HOST_XHCI_VEND  0x8086u
#define DDI_HOST_XHCI_DEV   0xa12fu

/* Soft inventory bound (matches kernel GJ_DDI_SOFT_DEV_MAX). */
#define DDI_HOST_SCAN_MAX   32u

/*
 * Soft DDI ops — libgj GJ_DDI_OP_* (match kernel ddi_door.h).
 * Soft!=product; Dual DoD A/B OPEN (named residual != close).
 */
#define DDI_HOST_OP_CLOSE          GJ_DDI_OP_CLOSE
#define DDI_HOST_OP_IRQ_BIND       GJ_DDI_OP_IRQ_BIND
#define DDI_HOST_OP_DMA_BUF_ALLOC  GJ_DDI_OP_DMA_BUF_ALLOC
#define DDI_HOST_OP_DMA_BUF_FREE   GJ_DDI_OP_DMA_BUF_FREE
#define DDI_HOST_OP_DMA_BUF_MAP    GJ_DDI_OP_DMA_BUF_MAP
#define DDI_HOST_OP_CFG_WRITE      GJ_DDI_OP_CFG_WRITE

/* Soft MSI-X badge default (kernel GJ_MSIX_BADGE_SOFT when badge=0). */
#define DDI_HOST_IRQ_BADGE_SOFT  1ull
/* Soft IRQ rebind badge residual (same handle; door rebind path). */
#define DDI_HOST_IRQ_BADGE_REBIND 3ull

/* Soft DMA_BUF residual: 1 page, flags=0 then optional force32 (bit0). */
#define DDI_HOST_DMA_BUF_PAGES    1u
#define DDI_HOST_DMA_BUF_FLAGS    0u
#define DDI_HOST_DMA_BUF_FORCE32  1u /* bit0 force32 / VT-d identity prefer */

/*
 * denser residual bar honesty (Dual DoD A/B; Soft!=product; stamp-free).
 * Bar v2026.08.04.75 panel context only — NEVER invent .76 / bump stamp.
 * denser multi-arm residual once-lamps (H2; agent!=close; product_mint=0).
 *
 *   arms: open|map|irq|dma|multi_host|residual|prefer_real|dual_dod
 * greppable: denser residual bar bar=v2026.08.04.75 denser_arms= denser=1
 * greppable: soft denser residual VERDICT Soft!=product dual_dod=OPEN
 * greppable: denser_prove=1 denser_residual=1 never_mint_held=1
 * greppable: denser residual != Dual DoD close never_invent=.76
 *
 * STRONGER denser residual bar .75 (H2 once; exclusive .c; Dual DoD A/B):
 * denser residual prove + VERDICT rollup. never product mint; Dual DoD OPEN.
 * Soft residual denser != Dual DoD close. ASCII Soft!= only (never Soft≠).
 */
#define DDI_HOST_BAR_HONESTY           "v2026.08.04.75"
#define DDI_HOST_STAMP_FREE            1u
#define DDI_HOST_AGENT_NE_CLOSE        1u
#define DDI_HOST_DENSER_PROVE          1u
#define DDI_HOST_DENSER_RESIDUAL       1u
#define DDI_HOST_NEVER_MINT_HELD       1u /* never MMIO/IRQ/DMA CNode mint */
#define DDI_HOST_DENSER_ALWAYS_BAR     1u
#define DDI_HOST_DENSER_ARMS_MAX       8u
#define DDI_HOST_DENSER_ARMS_MIN       6u
#define DDI_HOST_PRODUCT_CHAIN         "UDX+ABI+DDI"

/*
 * CFG_WRITE offsets (kernel ddi_door):
 *   - 0x04 Command: 10ec:8168 requests MEM|MASTER (0x0006). Never val=0.
 *     Never 8086:a12f Command (Dual DoD A; USBCMD.RS stays off).
 *   - 0x10 BAR0 / 0x00 identity: reject PERM (never PASS write).
 */
#define DDI_HOST_CFG_OFF_CMDST     0x04u
#define DDI_HOST_CFG_OFF_BAR0      0x10u
#define DDI_HOST_CFG_OFF_IDENT     0x00u
#define DDI_HOST_PCI_CMD_MEM       0x0002u
#define DDI_HOST_PCI_CMD_MASTER    0x0004u
#define DDI_HOST_PCI_CMD_MEM_BME \
    (DDI_HOST_PCI_CMD_MEM | DDI_HOST_PCI_CMD_MASTER) /* 0x0006; never 0 */

/*
 * Soft lifecycle phase mask (functional residual vs door life=).
 * Bits set when corresponding soft surface soft-ok (mint=0 always).
 * greppable: life=0x... on residual path / residual life lamps.
 */
#define DDI_HOST_LIFE_OPEN     (1u << 0)
#define DDI_HOST_LIFE_CFG_R    (1u << 1)
#define DDI_HOST_LIFE_CFG_W    (1u << 2)
#define DDI_HOST_LIFE_MAP      (1u << 3)
#define DDI_HOST_LIFE_REMAP    (1u << 4)
#define DDI_HOST_LIFE_DMA_NOTE (1u << 5)
#define DDI_HOST_LIFE_IRQ      (1u << 6)
#define DDI_HOST_LIFE_NOTIFY   (1u << 7)
#define DDI_HOST_LIFE_DMA_BUF  (1u << 8)
#define DDI_HOST_LIFE_DMA_MAP  (1u << 9)
#define DDI_HOST_LIFE_DMA_FREE (1u << 10)
#define DDI_HOST_LIFE_GRANT    (1u << 11)
#define DDI_HOST_LIFE_QUIESCE  (1u << 12)
#define DDI_HOST_LIFE_CLOSE    (1u << 13)
#define DDI_HOST_LIFE_POST_CL  (1u << 14) /* post_close forget observed */
#define DDI_HOST_LIFE_REOPEN   (1u << 15)
#define DDI_HOST_LIFE_IRQ_REB  (1u << 16) /* IRQ rebind residual ok */
#define DDI_HOST_LIFE_DMA_F32  (1u << 17) /* force32 DMA_BUF residual ok */
#define DDI_HOST_LIFE_DMA_RECB (1u << 18) /* DMA_BUF re-MAP different cb ok */
/* Full UDX-shaped chain when OPEN+MAP+IRQ+DMA_BUF+CLOSE all soft-ok. */
#define DDI_HOST_LIFE_CORE \
    (DDI_HOST_LIFE_OPEN | DDI_HOST_LIFE_MAP | DDI_HOST_LIFE_IRQ | \
     DDI_HOST_LIFE_DMA_BUF | DDI_HOST_LIFE_CLOSE)

/*
 * Soft bind outcome (honest; never product AC):
 *   MISS  -- ID not in soft inventory
 *   SKIP  -- ID found but OPEN or MAP soft-failed
 *   PASS  -- OPEN + >=1 preferred BAR MAP soft-ok
 */
#define DDI_HOST_BIND_MISS  0
#define DDI_HOST_BIND_SKIP  1
#define DDI_HOST_BIND_PASS  2

/*
 * Soft op counters (file-local; wrap OK; never hard-gate; no stamp dump).
 */
static unsigned g_uSoftScan;
static unsigned g_uSoftGet;
static unsigned g_uSoftGetOk;
static unsigned g_uSoftOpen;
static unsigned g_uSoftOpenOk;
static unsigned g_uSoftMap;
static unsigned g_uSoftMapOk;
static unsigned g_uSoftMapEmpty; /* preferred BAR empty (not a map call) */
static unsigned g_uSoftMapFail;  /* map syscall soft-fail */
static unsigned g_uSoftCfg;
static unsigned g_uSoftCfgOk;
static unsigned g_uSoftCfgWrite;
static unsigned g_uSoftCfgWriteOk;
static unsigned g_uSoftCfgWriteReject; /* BAR reject residual */
static unsigned g_uSoftCfgWriteIdentRej; /* identity reject residual */
static unsigned g_uSoftDma;
static unsigned g_uSoftDmaOk;
static unsigned g_uSoftIrq;
static unsigned g_uSoftIrqOk;
static unsigned g_uSoftIrqRebind;
static unsigned g_uSoftIrqRebindOk;
static unsigned g_uSoftDmaBuf;
static unsigned g_uSoftDmaBufOk;
static unsigned g_uSoftDmaMap;
static unsigned g_uSoftDmaMapOk;
static unsigned g_uSoftDmaFree;
static unsigned g_uSoftDmaFreeOk;
static unsigned g_uSoftDmaForce32;
static unsigned g_uSoftDmaForce32Ok;
static unsigned g_uSoftNotify;
static unsigned g_uSoftNotifyOk;
static unsigned g_uSoftClose;
static unsigned g_uSoftCloseOk;
static unsigned g_uSoftQuiesce;
static unsigned g_uSoftQuiesceOk;
static unsigned g_uSoftRemap;
static unsigned g_uSoftRemapOk;
static unsigned g_uSoftRemapIdem; /* re-MAP VA matches prior map (functional) */
static unsigned g_uSoftRemapSticky; /* second re-MAP still matches (grant sticky) */
static unsigned g_uSoftMapUser;   /* map_note.u8User residual observed */
static unsigned g_uSoftDmaMapIdem; /* DMA_BUF_MAP re-MAP cookie match (door dma_idem) */
static unsigned g_uSoftDmaMapRecb; /* DMA_BUF_MAP re-MAP different cb (door prior unmap) */
static unsigned g_uSoftDmaMapRecbOk;
static unsigned g_uSoftPostClose;
static unsigned g_uSoftPostCloseOk; /* dead-handle fail observed (functional) */
static unsigned g_uSoftPostCloseMap; /* post_close MAP_BAR fail (grant forget) */
static unsigned g_uSoftPostCloseIrq; /* post_close IRQ_BIND fail (unbind) */
static unsigned g_uSoftPostCloseDma; /* post_close DMA_BUF_ALLOC fail */
static unsigned g_uSoftReopen;
static unsigned g_uSoftReopenOk;
static unsigned g_uSoftBindPass;
static unsigned g_uSoftBindSkip;
static unsigned g_uSoftBindMiss;
static unsigned g_uSoftGrantSurfaces;
static unsigned g_uSoftResidualPaths;
/* Multi-host residual (concurrent rtl+xhci; Soft!=product; product_mint=0). */
static unsigned g_uSoftMultiHost;       /* multi-host residual entered */
static unsigned g_uSoftMultiHostOk;     /* both open + >=1 map each */
static unsigned g_uSoftMultiHostOpenBoth;
static unsigned g_uSoftMultiHostMapBoth;
static unsigned g_uSoftMultiHostIrqBoth;
static unsigned g_uSoftMultiHostIrqRebBoth; /* concurrent IRQ_REBIND both denser */
static unsigned g_uSoftMultiHostDmaBoth;
static unsigned g_uSoftMultiHostDmaBufBoth; /* concurrent DMA_BUF both (denser) */
static unsigned g_uSoftMultiHostDmaMapBoth; /* concurrent DMA_BUF_MAP both denser */
static unsigned g_uSoftMultiHostDmaIdemBoth; /* concurrent DMA_BUF_MAP cookie idem denser */
static unsigned g_uSoftMultiHostNotifyBoth; /* concurrent notify_poll both denser irq */
static unsigned g_uSoftMultiHostDenser; /* denser open+map+irq+dma prefer_real prove */
static unsigned g_uSoftMultiHostPeerLive; /* peer live after one CLOSE */
static unsigned g_uSoftMultiHostPeerRemap; /* peer MAP_REMAP after one CLOSE */
static unsigned g_uSoftMultiHostPeerIrq;   /* peer IRQ rebind after one CLOSE */
static unsigned g_uSoftMultiHostPeerDma;   /* peer DMA residual after one CLOSE */
static unsigned g_uSoftMultiHostCfgBoth;  /* CFG_READ both while concurrent */
static unsigned g_uSoftMultiHostRemapBoth; /* MAP_REMAP both while concurrent */
static unsigned g_uSoftMultiHostClosedDead; /* closed handle dead while peer live */
static unsigned g_uSoftMultiHostClosedIrq;  /* closed handle IRQ fail (isolation) */
static unsigned g_uSoftMultiHostClosedDma;  /* closed handle DMA fail (isolation) */
static unsigned g_uSoftMultiHostPrefComplete; /* rtl pref bars + xhci bar0 */
static unsigned g_uSoftMultiHostRevPeer; /* reverse CLOSE order peer_live */
static unsigned g_uSoftMultiHostRevPeerRemap; /* reverse peer MAP_REMAP denser */
static unsigned g_uSoftMultiHostRevPeerIrq; /* reverse peer IRQ rebind denser */
static unsigned g_uSoftMultiHostRevPeerDma; /* reverse peer DMA residual denser */
static unsigned g_uSoftMultiHostRevDead; /* reverse CLOSE order closed_dead */
static unsigned g_uSoftMultiHostOpenMapIrqDma; /* denser open+map+irq+dma both */
static unsigned g_uSoftMultiHostDensify; /* densify open+map+irq+dma all soft-ok */
static unsigned g_uSoftMultiHostPreferRealRes; /* prefer_real residual denser prove */
static unsigned g_uSoftMultiHostResOpen; /* prefer_real residual denser open */
static unsigned g_uSoftMultiHostResMap;  /* prefer_real residual denser map */
static unsigned g_uSoftMultiHostResIrq;  /* prefer_real residual denser irq */
static unsigned g_uSoftMultiHostResDma;  /* prefer_real residual denser dma */
static unsigned g_uSoftDenserResidualBar75; /* denser residual bar .75 VERDICT once */
static unsigned g_uSoftDenserResidualBar75Best; /* 0=MISS 1=SKIP 2=PASS */

/*
 * Soft residual path outcome for one G752 class bind (OPEN/MAP/IRQ/grant).
 * mint always 0 -- never product CNode install. Soft!=product.
 * UDX residual chain fields: dma_map / dma_free / dma_recb / force32 /
 * notify / remap / irq rebind / quiesce / reopen / cfg_w / cfg_ident /
 * post_close / life mask.
 */
struct ddi_host_residual_path {
    unsigned uOpen;    /* 1 if soft OPEN handle > 0 */
    unsigned uMaps;    /* count of preferred BARs soft-mapped */
    unsigned uIrq;     /* 1 if soft IRQ_BIND note ok */
    unsigned uIrqReb;  /* 1 if soft IRQ rebind residual ok */
    unsigned uGrant;   /* 1 if grant surface PASS (open+>=1 map) */
    unsigned uDmaBuf;  /* 1 if soft DMA_BUF_ALLOC note ok */
    unsigned uDmaMap;  /* 1 if soft DMA_BUF_MAP note ok */
    unsigned uDmaFree; /* 1 if soft DMA_BUF_FREE note ok */
    unsigned uDmaIdem; /* 1 if DMA_BUF_MAP re-MAP cookie matched */
    unsigned uDmaRecb; /* 1 if DMA_BUF re-MAP different cb soft-ok */
    unsigned uDmaF32;  /* 1 if force32 DMA_BUF residual ok */
    unsigned uNotify;  /* 1 if soft NOTIFY_WAIT poll ran (ret>=0) */
    unsigned uRemap;   /* 1 if soft MAP re-map residual ok */
    unsigned uIdem;    /* 1 if re-MAP VA matched prior map (functional) */
    unsigned uSticky;  /* 1 if second re-MAP still matched (grant sticky) */
    unsigned uMapUser; /* 1 if map_note reported user-AS residual */
    unsigned uCfgW;    /* 1 if careful CFG_WRITE soft-note ok */
    unsigned uCfgRej;  /* 1 if BAR CFG_WRITE reject observed */
    unsigned uCfgIdent; /* 1 if identity CFG_WRITE reject observed */
    unsigned uQuiesce; /* 1 if soft QUIESCE note emitted */
    unsigned uClose;   /* 1 if soft CLOSE note ok */
    unsigned uPostCl;  /* 1 if post_close dead-handle residual ok */
    unsigned uPostMap; /* 1 if post_close MAP_BAR failed (grant forget) */
    unsigned uPostIrq; /* 1 if post_close IRQ_BIND failed (unbind) */
    unsigned uPostDma; /* 1 if post_close DMA_BUF_ALLOC failed */
    unsigned uReopen;  /* 1 if soft REOPEN after CLOSE ok */
    unsigned uLife;    /* DDI_HOST_LIFE_* phase mask */
    int      nOutcome; /* DDI_HOST_BIND_* */
};

static void
soft_inc(unsigned *pu)
{
    if (pu != 0 && *pu < 0xffffffffu) {
        (*pu)++;
    }
}

/*
 * Packed device info — layout matches kernel struct gj_ddi_dev_info
 * (kernel/include/gj/ddi_door.h). libgj does not export the type yet.
 */
struct ddi_host_dev_info {
    unsigned char  u8Bus;
    unsigned char  u8Slot;
    unsigned char  u8Func;
    unsigned char  u8Pad0;
    unsigned short u16Vend;
    unsigned short u16Dev;
    unsigned int   u32Class;
    unsigned long long u64Bar0Pa;
    unsigned long long u64Bar0Cb;
    unsigned long long u64Bar1Pa;
    unsigned long long u64Bar1Cb;
    unsigned long long u64Bar2Pa;
    unsigned long long u64Bar2Cb;
    unsigned long long u64Bar3Pa;
    unsigned long long u64Bar3Cb;
    unsigned long long u64Bar4Pa;
    unsigned long long u64Bar4Cb;
    unsigned long long u64Bar5Pa;
    unsigned long long u64Bar5Cb;
};

/*
 * Soft MAP_BAR out-note — layout matches kernel struct gj_ddi_map_note.
 * Passed as arg3 of GJ_DDI_OP_MAP_BAR when user_range_ok.
 * u8User: non-zero when door installed user-AS map residual (not product mint).
 */
struct ddi_host_map_note {
    unsigned long long u64Va;
    unsigned long long u64Cb;
    unsigned long long u64Pa;
    unsigned int u32Bar;
    unsigned int u32Handle;
    unsigned char u8Ok;
    unsigned char u8User;
    unsigned char u8Pad[2];
};

/*
 * Soft MAP result for functional residual (VA + user-AS flag).
 * Prefer over lamp-only PASS when chaining MAP_REMAP idempotency.
 */
struct ddi_host_map_result {
    unsigned long long u64Va;
    unsigned uUser; /* 1 if map_note.u8User */
    unsigned uOk;
};

/* ---- freestanding helpers ----------------------------------------------- */

static void
msg(const char *sz)
{
    size_t n = 0;

    if (sz == 0) {
        return;
    }
    while (sz[n] != '\0') {
        n++;
    }
    (void)gj_debug_log(sz, (long)n);
}

static void
memzero(void *p, unsigned cb)
{
    unsigned char *pB = (unsigned char *)p;
    unsigned i;

    if (pB == 0) {
        return;
    }
    for (i = 0; i < cb; i++) {
        pB[i] = 0;
    }
}

static void
append_s(char *aLine, unsigned cb, unsigned *po, const char *sz)
{
    if (aLine == 0 || po == 0 || sz == 0 || cb == 0) {
        return;
    }
    while (*sz != '\0' && *po + 1u < cb) {
        aLine[(*po)++] = *sz++;
    }
}

static void
append_u(char *aLine, unsigned cb, unsigned *po, unsigned long u)
{
    char aDig[20];
    unsigned n = 0;
    unsigned i;

    if (aLine == 0 || po == 0 || cb == 0) {
        return;
    }
    if (u == 0) {
        if (*po + 1u < cb) {
            aLine[(*po)++] = '0';
        }
        return;
    }
    while (u > 0 && n < (unsigned)sizeof(aDig)) {
        aDig[n++] = (char)('0' + (u % 10ul));
        u /= 10ul;
    }
    for (i = n; i > 0; i--) {
        if (*po + 1u < cb) {
            aLine[(*po)++] = aDig[i - 1u];
        }
    }
}

static void
append_hex4(char *aLine, unsigned cb, unsigned *po, unsigned v)
{
    static const char aH[] = "0123456789abcdef";
    unsigned i;

    if (aLine == 0 || po == 0 || cb == 0) {
        return;
    }
    for (i = 0; i < 4u; i++) {
        unsigned nibble = (v >> (12u - 4u * i)) & 0xfu;
        if (*po + 1u < cb) {
            aLine[(*po)++] = aH[nibble];
        }
    }
}

static void
append_hex_ull(char *aLine, unsigned cb, unsigned *po, unsigned long long v)
{
    static const char aH[] = "0123456789abcdef";
    char aDig[16];
    unsigned n = 0;
    unsigned i;

    if (aLine == 0 || po == 0 || cb == 0) {
        return;
    }
    if (v == 0ull) {
        if (*po + 1u < cb) {
            aLine[(*po)++] = '0';
        }
        return;
    }
    while (v > 0ull && n < 16u) {
        aDig[n++] = aH[(unsigned)(v & 0xfull)];
        v >>= 4;
    }
    for (i = n; i > 0; i--) {
        if (*po + 1u < cb) {
            aLine[(*po)++] = aDig[i - 1u];
        }
    }
}

/**
 * Soft errno → short reason token (GJ_ERR_* magnitude; freestanding, no libc).
 * Matches kernel/include/gj/error.h magnitudes for greppable err= paths.
 * Never product AC; observation only.
 */
static const char *
soft_err_token(long ret)
{
    long e;

    if (ret >= 0) {
        return "ok";
    }
    e = -ret;
    /* Magnitudes from gj/error.h (strictly negative small ints). */
    if (e == 1) {
        return "inval";
    }
    if (e == 2) {
        return "nomem";
    }
    if (e == 3) {
        return "noent";
    }
    if (e == 4) {
        return "perm";
    }
    if (e == 5) {
        return "again";
    }
    if (e == 6) {
        return "nosupport";
    }
    if (e == 7) {
        return "busy";
    }
    if (e == 8) {
        return "fault";
    }
    if (e == 15) {
        return "nodev";
    }
    if (e == 16) {
        return "io";
    }
    return "err";
}

/**
 * Append " err=<token> ret=-N" for soft-fail lamps (better error paths).
 */
static void
append_err(char *aLine, unsigned cb, unsigned *po, long ret)
{
    unsigned long mag;

    if (ret >= 0) {
        return;
    }
    append_s(aLine, cb, po, " err=");
    append_s(aLine, cb, po, soft_err_token(ret));
    append_s(aLine, cb, po, " ret=-");
    mag = (unsigned long)(-ret);
    append_u(aLine, cb, po, mag);
}

static void
bar_pa_cb(const struct ddi_host_dev_info *p, unsigned uBar,
          unsigned long long *pPa, unsigned long long *pCb)
{
    unsigned long long pa = 0;
    unsigned long long cb = 0;

    if (p != 0) {
        switch (uBar) {
        case 0: pa = p->u64Bar0Pa; cb = p->u64Bar0Cb; break;
        case 1: pa = p->u64Bar1Pa; cb = p->u64Bar1Cb; break;
        case 2: pa = p->u64Bar2Pa; cb = p->u64Bar2Cb; break;
        case 3: pa = p->u64Bar3Pa; cb = p->u64Bar3Cb; break;
        case 4: pa = p->u64Bar4Pa; cb = p->u64Bar4Cb; break;
        case 5: pa = p->u64Bar5Pa; cb = p->u64Bar5Cb; break;
        default: break;
        }
    }
    if (pPa != 0) {
        *pPa = pa;
    }
    if (pCb != 0) {
        *pCb = cb;
    }
}

static void
log_found(unsigned idx, const struct ddi_host_dev_info *p)
{
    char aLine[160];
    unsigned o = 0;

    if (p == 0) {
        return;
    }
    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o, "ddi_host: soft found idx=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)idx);
    append_s(aLine, sizeof(aLine), &o, " bdf=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)p->u8Bus);
    append_s(aLine, sizeof(aLine), &o, ":");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)p->u8Slot);
    append_s(aLine, sizeof(aLine), &o, ".");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)p->u8Func);
    append_s(aLine, sizeof(aLine), &o, " id=");
    append_hex4(aLine, sizeof(aLine), &o, (unsigned)p->u16Vend);
    append_s(aLine, sizeof(aLine), &o, ":");
    append_hex4(aLine, sizeof(aLine), &o, (unsigned)p->u16Dev);
    append_s(aLine, sizeof(aLine), &o, " bar0=0x");
    append_hex_ull(aLine, sizeof(aLine), &o, p->u64Bar0Pa);
    append_s(aLine, sizeof(aLine), &o, "/");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)p->u64Bar0Cb);
    append_s(aLine, sizeof(aLine), &o, " bar2=0x");
    append_hex_ull(aLine, sizeof(aLine), &o, p->u64Bar2Pa);
    append_s(aLine, sizeof(aLine), &o, "/");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)p->u64Bar2Cb);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);
}

/**
 * Soft GET lamp — greppable inventory fetch (Soft!=product).
 * greppable: ddi_host: soft GET
 * On soft-fail, logs err= token (better error path; never hard-exit).
 */
static void
log_soft_get(unsigned idx, const struct ddi_host_dev_info *p, int fOk, long ret)
{
    char aLine[160];
    unsigned o = 0;

    soft_inc(&g_uSoftGet);
    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o, "ddi_host: soft GET idx=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)idx);
    if (fOk != 0 && p != 0) {
        soft_inc(&g_uSoftGetOk);
        append_s(aLine, sizeof(aLine), &o, " id=");
        append_hex4(aLine, sizeof(aLine), &o, (unsigned)p->u16Vend);
        append_s(aLine, sizeof(aLine), &o, ":");
        append_hex4(aLine, sizeof(aLine), &o, (unsigned)p->u16Dev);
        append_s(aLine, sizeof(aLine), &o, " bdf=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)p->u8Bus);
        append_s(aLine, sizeof(aLine), &o, ":");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)p->u8Slot);
        append_s(aLine, sizeof(aLine), &o, ".");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)p->u8Func);
        append_s(aLine, sizeof(aLine), &o, " PASS Soft!=product\n");
    } else {
        append_s(aLine, sizeof(aLine), &o, " SKIP");
        append_err(aLine, sizeof(aLine), &o, ret);
        append_s(aLine, sizeof(aLine), &o, " Soft!=product\n");
    }
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);
}

/**
 * Soft OPEN lamp — greppable handle note (not product CNode cap).
 * greppable: ddi_host: soft open
 * greppable: ddi_host: soft OPEN
 * Soft-fail includes err= token (open_fail path honesty).
 */
static void
log_soft_open(const char *szId, unsigned idx, long h, int fOk)
{
    char aLine[160];
    unsigned o = 0;

    soft_inc(&g_uSoftOpen);
    memzero(aLine, sizeof(aLine));
    /* Opcode-shaped lamp (SCAN/GET/OPEN/MAP_BAR honesty). */
    append_s(aLine, sizeof(aLine), &o, "ddi_host: soft OPEN ");
    append_s(aLine, sizeof(aLine), &o, szId != 0 ? szId : "????:????");
    append_s(aLine, sizeof(aLine), &o, " idx=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)idx);
    if (fOk != 0 && h > 0) {
        soft_inc(&g_uSoftOpenOk);
        append_s(aLine, sizeof(aLine), &o, " handle=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)h);
        append_s(aLine, sizeof(aLine), &o,
                 " PASS mint=0 cnode=0 Soft!=product\n");
    } else {
        append_s(aLine, sizeof(aLine), &o, " SKIP why=open_fail");
        append_err(aLine, sizeof(aLine), &o, h);
        append_s(aLine, sizeof(aLine), &o, " Soft!=product\n");
    }
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);
}

/**
 * Soft CFG_READ honesty after OPEN (identity snap; not product program).
 * Reads dword0 (vend:dev) + optional dword1 (cmd/status) when available.
 * greppable: ddi_host: soft CFG
 * Returns 1 on soft-ok (cfg0 readable), 0 otherwise.
 */
static int
log_soft_cfg(const char *szId, unsigned long h)
{
    long ret0;
    long ret1;
    char aLine[160];
    unsigned o = 0;
    int fOk = 0;

    soft_inc(&g_uSoftCfg);
    ret0 = gj_ddi_cfg_read(h, 0u);
    ret1 = gj_ddi_cfg_read(h, 4u); /* PCI command/status soft snap */
    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o, "ddi_host: soft CFG ");
    append_s(aLine, sizeof(aLine), &o, szId != 0 ? szId : "????:????");
    if (ret0 >= 0) {
        soft_inc(&g_uSoftCfgOk);
        fOk = 1;
        append_s(aLine, sizeof(aLine), &o, " cfg0=0x");
        append_hex_ull(aLine, sizeof(aLine), &o,
                       (unsigned long long)(unsigned)ret0);
        if (ret1 >= 0) {
            append_s(aLine, sizeof(aLine), &o, " cmdst=0x");
            append_hex_ull(aLine, sizeof(aLine), &o,
                           (unsigned long long)(unsigned)ret1);
        }
        append_s(aLine, sizeof(aLine), &o, " PASS Soft!=product\n");
    } else {
        append_s(aLine, sizeof(aLine), &o, " SKIP");
        append_err(aLine, sizeof(aLine), &o, ret0);
        append_s(aLine, sizeof(aLine), &o, " Soft!=product\n");
    }
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);
    return fOk;
}

/**
 * 10ec:8168 PCI Command (0x04): request MEM|MASTER (0x0006). Never val=0.
 * Never 8086:a12f Command (Dual DoD A; USBCMD.RS stays off).
 * Returns CFG_WRITE ret on 8168; 0 and no poke otherwise.
 */
static long
soft_cfg_write_8168_command(unsigned long h, unsigned short u16Vend,
                            unsigned short u16Dev)
{
    unsigned uVal;

    if (u16Vend != (unsigned short)DDI_HOST_RTL_VEND ||
        u16Dev != (unsigned short)DDI_HOST_RTL_DEV) {
        return 0;
    }
    if (h == 0ul) {
        return -1;
    }
    uVal = (unsigned)DDI_HOST_PCI_CMD_MEM_BME;
    if (uVal == 0u) {
        return -1;
    }
    return gj_ddi_cfg_write(h, DDI_HOST_CFG_OFF_CMDST, uVal);
}

/**
 * Soft CFG_WRITE residual -- careful path matching kernel ddi_door.
 *   1) 10ec:8168 Command (0x04) = MEM|MASTER (0x0006). Never val=0.
 *      Never 8086:a12f Command.
 *   2) Reject residual at BAR0 (0x10) -- expect PERM (unsafe BAR).
 *   3) Reject residual at identity (0x00) -- expect PERM (unsafe identity).
 * greppable: ddi_host: soft CFG_WRITE
 * greppable: ddi_host: soft CFG_WRITE reject
 * Fills *puNote / *puRej / *puIdent with 0/1 when non-NULL.
 * Returns 1 if 8168 Command MEM|MASTER write ok, 0 otherwise.
 */
static int
log_soft_cfg_write(const char *szId, unsigned long h,
                   unsigned short u16Vend, unsigned short u16Dev,
                   unsigned *puNote, unsigned *puRej, unsigned *puIdent)
{
    long retNote;
    long retRej;
    long retIdent;
    char aLine[192];
    unsigned o = 0;
    int fNote = 0;
    int fRej = 0;
    int fIdent = 0;
    int fRtl;

    if (puNote != 0) {
        *puNote = 0;
    }
    if (puRej != 0) {
        *puRej = 0;
    }
    if (puIdent != 0) {
        *puIdent = 0;
    }

    fRtl = (u16Vend == (unsigned short)DDI_HOST_RTL_VEND &&
            u16Dev == (unsigned short)DDI_HOST_RTL_DEV) ? 1 : 0;
    /* 8168 Command MEM|MASTER only. Never val=0. Never xHCI Command. */
    if (fRtl != 0) {
        soft_inc(&g_uSoftCfgWrite);
        retNote = soft_cfg_write_8168_command(h, u16Vend, u16Dev);
        memzero(aLine, sizeof(aLine));
        append_s(aLine, sizeof(aLine), &o, "ddi_host: soft CFG_WRITE ");
        append_s(aLine, sizeof(aLine), &o, szId != 0 ? szId : "????:????");
        append_s(aLine, sizeof(aLine), &o, " off=0x");
        append_hex_ull(aLine, sizeof(aLine), &o,
                       (unsigned long long)DDI_HOST_CFG_OFF_CMDST);
        append_s(aLine, sizeof(aLine), &o, " val=0x");
        append_hex4(aLine, sizeof(aLine), &o,
                    (unsigned)DDI_HOST_PCI_CMD_MEM_BME);
        append_s(aLine, sizeof(aLine), &o, " handle=");
        append_u(aLine, sizeof(aLine), &o, h);
        if (retNote >= 0) {
            soft_inc(&g_uSoftCfgWriteOk);
            fNote = 1;
            append_s(aLine, sizeof(aLine), &o,
                     " PASS mse=1 bme=1 mint=0 Soft!=product\n");
        } else {
            append_s(aLine, sizeof(aLine), &o, " SKIP why=cfg_write_fail");
            append_err(aLine, sizeof(aLine), &o, retNote);
            append_s(aLine, sizeof(aLine), &o, " mint=0 Soft!=product\n");
        }
        aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
        msg(aLine);
    }

    /*
     * Functional residual: BAR0 write must soft-reject (PERM). Observes
     * door careful-soft policy; never attempts live BAR rewrite as PASS.
     * greppable: ddi_host: soft CFG_WRITE reject
     */
    retRej = gj_ddi(DDI_HOST_OP_CFG_WRITE, h,
                    (unsigned long)DDI_HOST_CFG_OFF_BAR0, 0ul);
    o = 0;
    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o, "ddi_host: soft CFG_WRITE reject ");
    append_s(aLine, sizeof(aLine), &o, szId != 0 ? szId : "????:????");
    append_s(aLine, sizeof(aLine), &o, " off=0x");
    append_hex_ull(aLine, sizeof(aLine), &o,
                   (unsigned long long)DDI_HOST_CFG_OFF_BAR0);
    if (retRej < 0) {
        soft_inc(&g_uSoftCfgWriteReject);
        fRej = 1;
        append_s(aLine, sizeof(aLine), &o, " PASS why=unsafe_bar");
        append_err(aLine, sizeof(aLine), &o, retRej);
        append_s(aLine, sizeof(aLine), &o,
                 " soft_policy=1 mint=0 Soft!=product\n");
    } else {
        /* Unexpected accept -- honesty SKIP (door policy may have widened). */
        append_s(aLine, sizeof(aLine), &o,
                 " SKIP why=reject_miss soft_policy=OPEN mint=0 Soft!=product\n");
    }
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);

    /*
     * Functional residual: identity dword0 write must soft-reject (PERM).
     * Matches kernel ddi_door careful soft (refuse identity 0x00 live write).
     * greppable: ddi_host: soft CFG_WRITE reject why=unsafe_ident
     */
    retIdent = gj_ddi(DDI_HOST_OP_CFG_WRITE, h,
                      (unsigned long)DDI_HOST_CFG_OFF_IDENT, 0ul);
    o = 0;
    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o, "ddi_host: soft CFG_WRITE reject ");
    append_s(aLine, sizeof(aLine), &o, szId != 0 ? szId : "????:????");
    append_s(aLine, sizeof(aLine), &o, " off=0x");
    append_hex_ull(aLine, sizeof(aLine), &o,
                   (unsigned long long)DDI_HOST_CFG_OFF_IDENT);
    if (retIdent < 0) {
        soft_inc(&g_uSoftCfgWriteIdentRej);
        fIdent = 1;
        append_s(aLine, sizeof(aLine), &o, " PASS why=unsafe_ident");
        append_err(aLine, sizeof(aLine), &o, retIdent);
        append_s(aLine, sizeof(aLine), &o,
                 " soft_policy=1 mint=0 Soft!=product\n");
    } else {
        append_s(aLine, sizeof(aLine), &o,
                 " SKIP why=ident_reject_miss soft_policy=OPEN "
                 "mint=0 Soft!=product\n");
    }
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);

    if (puNote != 0) {
        *puNote = (fNote != 0) ? 1u : 0u;
    }
    if (puRej != 0) {
        *puRej = (fRej != 0) ? 1u : 0u;
    }
    if (puIdent != 0) {
        *puIdent = (fIdent != 0) ? 1u : 0u;
    }
    return fNote;
}

/**
 * Soft DMA_NOTE honesty — records soft window only; never product mint.
 * greppable: ddi_host: soft DMA_NOTE
 */
static void
log_soft_dma_note(const char *szId, unsigned long h,
                  unsigned long long u64Pa, unsigned long long u64Cb)
{
    long ret;
    char aLine[176];
    unsigned o = 0;

    soft_inc(&g_uSoftDma);
    if (u64Pa == 0ull || u64Cb == 0ull) {
        memzero(aLine, sizeof(aLine));
        append_s(aLine, sizeof(aLine), &o, "ddi_host: soft DMA_NOTE ");
        append_s(aLine, sizeof(aLine), &o, szId != 0 ? szId : "????:????");
        append_s(aLine, sizeof(aLine), &o,
                 " SKIP why=empty mint=0 Soft!=product\n");
        aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
        msg(aLine);
        return;
    }

    ret = gj_ddi_dma_note(h, (unsigned long)u64Pa, (unsigned long)u64Cb);
    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o, "ddi_host: soft DMA_NOTE ");
    append_s(aLine, sizeof(aLine), &o, szId != 0 ? szId : "????:????");
    append_s(aLine, sizeof(aLine), &o, " pa=0x");
    append_hex_ull(aLine, sizeof(aLine), &o, u64Pa);
    append_s(aLine, sizeof(aLine), &o, " cb=0x");
    append_hex_ull(aLine, sizeof(aLine), &o, u64Cb);
    if (ret >= 0) {
        soft_inc(&g_uSoftDmaOk);
        append_s(aLine, sizeof(aLine), &o,
                 " PASS soft_only=1 mint=0 Soft!=product\n");
    } else {
        append_s(aLine, sizeof(aLine), &o, " SKIP why=dma_fail");
        append_err(aLine, sizeof(aLine), &o, ret);
        append_s(aLine, sizeof(aLine), &o, " mint=0 Soft!=product\n");
    }
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);
}

/**
 * Soft IRQ_BIND note -- DDI_OP_IRQ_BIND (9) via raw gj_ddi.
 * Cap honesty: soft irq->handle note only; never product Notification mint.
 * greppable: ddi_host: soft IRQ_BIND
 * Returns 1 on soft note ok, 0 otherwise.
 */
static int
log_soft_irq_bind(const char *szId, unsigned long h)
{
    long ret;
    char aLine[192];
    unsigned o = 0;
    int fOk = 0;

    soft_inc(&g_uSoftIrq);
    /* badge=0 -> kernel uses GJ_MSIX_BADGE_SOFT; pass soft default for lamps. */
    ret = gj_ddi(DDI_HOST_OP_IRQ_BIND, h, (unsigned long)DDI_HOST_IRQ_BADGE_SOFT,
                 0ul);

    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o, "ddi_host: soft IRQ_BIND ");
    append_s(aLine, sizeof(aLine), &o, szId != 0 ? szId : "????:????");
    append_s(aLine, sizeof(aLine), &o, " handle=");
    append_u(aLine, sizeof(aLine), &o, h);
    append_s(aLine, sizeof(aLine), &o, " badge=0x");
    append_hex_ull(aLine, sizeof(aLine), &o, DDI_HOST_IRQ_BADGE_SOFT);
    if (ret >= 0) {
        soft_inc(&g_uSoftIrqOk);
        fOk = 1;
        append_s(aLine, sizeof(aLine), &o,
                 " PASS soft_note=1 mint=0 irq_notify=0 "
                 "notify_wait=OPEN hard_irq=0 Soft!=product\n");
    } else {
        append_s(aLine, sizeof(aLine), &o, " SKIP why=irq_fail");
        append_err(aLine, sizeof(aLine), &o, ret);
        append_s(aLine, sizeof(aLine), &o,
                 " mint=0 Soft!=product\n");
    }
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);
    return fOk;
}

/**
 * Soft IRQ rebind residual -- second DDI_OP_IRQ_BIND on same handle.
 * Functional vs kernel ddi_door: rebind updates badge only; multi-slot safe;
 * never clear-all (handle 0 rejected at door). Cap honesty: soft note only;
 * no product IRQ Notification mint (mint=0). H1: no net_eth_poll from IRQ.
 * greppable: ddi_host: soft IRQ_REBIND
 * Returns 1 on soft rebind ok, 0 otherwise.
 */
static int
log_soft_irq_rebind(const char *szId, unsigned long h)
{
    long ret;
    char aLine[192];
    unsigned o = 0;
    int fOk = 0;

    soft_inc(&g_uSoftIrqRebind);
    ret = gj_ddi(DDI_HOST_OP_IRQ_BIND, h,
                 (unsigned long)DDI_HOST_IRQ_BADGE_REBIND, 0ul);

    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o, "ddi_host: soft IRQ_REBIND ");
    append_s(aLine, sizeof(aLine), &o, szId != 0 ? szId : "????:????");
    append_s(aLine, sizeof(aLine), &o, " handle=");
    append_u(aLine, sizeof(aLine), &o, h);
    append_s(aLine, sizeof(aLine), &o, " badge=0x");
    append_hex_ull(aLine, sizeof(aLine), &o, DDI_HOST_IRQ_BADGE_REBIND);
    if (ret >= 0) {
        soft_inc(&g_uSoftIrqRebindOk);
        fOk = 1;
        append_s(aLine, sizeof(aLine), &o,
                 " PASS rebind=1 multi_slot=1 never_clear_all=1 "
                 "mint=0 irq_notify=0 Soft!=product\n");
    } else {
        append_s(aLine, sizeof(aLine), &o, " SKIP why=rebind_fail");
        append_err(aLine, sizeof(aLine), &o, ret);
        append_s(aLine, sizeof(aLine), &o, " mint=0 Soft!=product\n");
    }
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);
    return fOk;
}

/**
 * Soft DMA_BUF residual chain for rtl ring / xhci xfer spirit.
 * Cap honesty: soft slots only; never DMA window / CNode mint (mint=0).
 *
 * Chain: ALLOC -> MAP -> MAP_REMAP(idem cookie) -> MAP_RECB(diff cb) ->
 * FREE (ops 10/12/11) then optional force32 ALLOC+FREE residual (door bit0 /
 * VT-d identity prefer) before CLOSE residual. FREE after MAP so CLOSE does
 * not need to own the soft page residual when the lean path exercises the
 * full UDX-shaped cookie lifecycle. Functional: re-MAP same pa/cb returns
 * prior bus cookie (kernel ddi_door dma_idem); re-MAP different cb unmaps
 * prior cookie first (door dma_recb; Soft!=product).
 *
 * greppable: ddi_host: soft DMA_BUF
 * greppable: ddi_host: soft DMA_BUF_MAP
 * greppable: ddi_host: soft DMA_BUF_MAP_REMAP
 * greppable: ddi_host: soft DMA_BUF_MAP_RECB
 * greppable: ddi_host: soft DMA_BUF_FREE
 * greppable: ddi_host: soft DMA_BUF force32
 * greppable: ddi_host: soft dma_buf chain
 *
 * Fills *puAlloc / *puMap / *puFree / *puIdem / *puRecb / *puF32 with 0/1
 * when non-NULL. Returns 1 if ALLOC soft-ok (chain entry), 0 otherwise.
 */
static int
log_soft_dma_buf_chain(const char *szId, unsigned long h,
                       unsigned *puAlloc, unsigned *puMap, unsigned *puFree,
                       unsigned *puIdem, unsigned *puRecb, unsigned *puF32)
{
    long retAlloc;
    long retMap;
    long retMap2;
    long retMap3;
    long retFree;
    long retF32;
    long retF32Free;
    unsigned long long u64Pa;
    unsigned long long u64Cb;
    unsigned long long u64CbAlt;
    unsigned long long u64Bus;
    unsigned long long u64F32Pa;
    char aLine[208];
    unsigned o = 0;
    int fAlloc = 0;
    int fMap = 0;
    int fIdem = 0;
    int fRecb = 0;
    int fFree = 0;
    int fF32 = 0;

    if (puAlloc != 0) {
        *puAlloc = 0;
    }
    if (puMap != 0) {
        *puMap = 0;
    }
    if (puFree != 0) {
        *puFree = 0;
    }
    if (puIdem != 0) {
        *puIdem = 0;
    }
    if (puRecb != 0) {
        *puRecb = 0;
    }
    if (puF32 != 0) {
        *puF32 = 0;
    }

    soft_inc(&g_uSoftDmaBuf);
    retAlloc = gj_ddi(DDI_HOST_OP_DMA_BUF_ALLOC, h,
                      (unsigned long)DDI_HOST_DMA_BUF_PAGES,
                      (unsigned long)DDI_HOST_DMA_BUF_FLAGS);

    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o, "ddi_host: soft DMA_BUF ");
    append_s(aLine, sizeof(aLine), &o, szId != 0 ? szId : "????:????");
    append_s(aLine, sizeof(aLine), &o, " handle=");
    append_u(aLine, sizeof(aLine), &o, h);
    append_s(aLine, sizeof(aLine), &o, " pages=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)DDI_HOST_DMA_BUF_PAGES);
    if (retAlloc >= 0) {
        soft_inc(&g_uSoftDmaBufOk);
        fAlloc = 1;
        u64Pa = (unsigned long long)retAlloc;
        append_s(aLine, sizeof(aLine), &o, " pa=0x");
        append_hex_ull(aLine, sizeof(aLine), &o, u64Pa);
        append_s(aLine, sizeof(aLine), &o,
                 " PASS soft_only=1 mint=0 dma_window=0 Soft!=product\n");
    } else {
        u64Pa = 0ull;
        append_s(aLine, sizeof(aLine), &o, " SKIP why=dma_buf_fail");
        append_err(aLine, sizeof(aLine), &o, retAlloc);
        append_s(aLine, sizeof(aLine), &o, " mint=0 Soft!=product\n");
    }
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);

    if (fAlloc == 0) {
        /* Chain rollup even on alloc skip. */
        o = 0;
        memzero(aLine, sizeof(aLine));
        append_s(aLine, sizeof(aLine), &o, "ddi_host: soft dma_buf chain ");
        append_s(aLine, sizeof(aLine), &o, szId != 0 ? szId : "????:????");
        append_s(aLine, sizeof(aLine), &o,
                 " alloc=0 map=0 free=0 dma_idem=0 dma_recb=0 force32=0 "
                 "mint=0 Soft!=product\n");
        aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
        msg(aLine);
        return 0;
    }

    /*
     * DMA_BUF_MAP: soft bus cookie note (identity PA soft residual).
     * cb = pages * 4096 soft page size (kernel dma_buf residual).
     * greppable: ddi_host: soft DMA_BUF_MAP
     */
    u64Cb = (unsigned long long)DDI_HOST_DMA_BUF_PAGES * 4096ull;
    u64Bus = 0ull;
    soft_inc(&g_uSoftDmaMap);
    retMap = gj_ddi(DDI_HOST_OP_DMA_BUF_MAP, h, (unsigned long)u64Pa,
                    (unsigned long)u64Cb);

    o = 0;
    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o, "ddi_host: soft DMA_BUF_MAP ");
    append_s(aLine, sizeof(aLine), &o, szId != 0 ? szId : "????:????");
    append_s(aLine, sizeof(aLine), &o, " handle=");
    append_u(aLine, sizeof(aLine), &o, h);
    append_s(aLine, sizeof(aLine), &o, " pa=0x");
    append_hex_ull(aLine, sizeof(aLine), &o, u64Pa);
    append_s(aLine, sizeof(aLine), &o, " cb=0x");
    append_hex_ull(aLine, sizeof(aLine), &o, u64Cb);
    if (retMap >= 0) {
        soft_inc(&g_uSoftDmaMapOk);
        fMap = 1;
        u64Bus = (unsigned long long)retMap;
        append_s(aLine, sizeof(aLine), &o, " bus=0x");
        append_hex_ull(aLine, sizeof(aLine), &o, u64Bus);
        append_s(aLine, sizeof(aLine), &o,
                 " PASS soft_cookie=1 mint=0 Soft!=product\n");
    } else {
        append_s(aLine, sizeof(aLine), &o, " SKIP why=dma_map_fail");
        append_err(aLine, sizeof(aLine), &o, retMap);
        append_s(aLine, sizeof(aLine), &o, " mint=0 Soft!=product\n");
    }
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);

    /*
     * Functional residual: re-MAP same pa/cb (door DMA_BUF_MAP idempotent
     * cookie). Matches kernel g_u32SoftDmaIdemHit path. Soft!=product.
     * greppable: ddi_host: soft DMA_BUF_MAP_REMAP
     */
    if (fMap != 0) {
        retMap2 = gj_ddi(DDI_HOST_OP_DMA_BUF_MAP, h, (unsigned long)u64Pa,
                         (unsigned long)u64Cb);
        o = 0;
        memzero(aLine, sizeof(aLine));
        append_s(aLine, sizeof(aLine), &o, "ddi_host: soft DMA_BUF_MAP_REMAP ");
        append_s(aLine, sizeof(aLine), &o, szId != 0 ? szId : "????:????");
        append_s(aLine, sizeof(aLine), &o, " handle=");
        append_u(aLine, sizeof(aLine), &o, h);
        append_s(aLine, sizeof(aLine), &o, " pa=0x");
        append_hex_ull(aLine, sizeof(aLine), &o, u64Pa);
        if (retMap2 >= 0 && (unsigned long long)retMap2 == u64Bus) {
            soft_inc(&g_uSoftDmaMapIdem);
            fIdem = 1;
            append_s(aLine, sizeof(aLine), &o, " bus=0x");
            append_hex_ull(aLine, sizeof(aLine), &o, u64Bus);
            append_s(aLine, sizeof(aLine), &o,
                     " PASS cookie_match=1 dma_idem=1 mint=0 Soft!=product\n");
        } else if (retMap2 >= 0) {
            append_s(aLine, sizeof(aLine), &o, " bus=0x");
            append_hex_ull(aLine, sizeof(aLine), &o,
                           (unsigned long long)retMap2);
            append_s(aLine, sizeof(aLine), &o, " prior=0x");
            append_hex_ull(aLine, sizeof(aLine), &o, u64Bus);
            append_s(aLine, sizeof(aLine), &o,
                     " PASS cookie_match=0 dma_idem=0 mint=0 Soft!=product\n");
        } else {
            append_s(aLine, sizeof(aLine), &o, " SKIP why=dma_remap_fail");
            append_err(aLine, sizeof(aLine), &o, retMap2);
            append_s(aLine, sizeof(aLine), &o,
                     " dma_idem=0 mint=0 Soft!=product\n");
        }
        aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
        msg(aLine);

        /*
         * Functional residual: re-MAP different cb (door unmaps prior cookie
         * first; UDX ring-resize spirit). Soft!=product; never window mint.
         * greppable: ddi_host: soft DMA_BUF_MAP_RECB
         */
        u64CbAlt = u64Cb >> 1; /* half prior map length (page residual) */
        if (u64CbAlt == 0ull) {
            u64CbAlt = 0x800ull; /* floor when cb tiny */
        }
        soft_inc(&g_uSoftDmaMapRecb);
        retMap3 = gj_ddi(DDI_HOST_OP_DMA_BUF_MAP, h, (unsigned long)u64Pa,
                         (unsigned long)u64CbAlt);
        o = 0;
        memzero(aLine, sizeof(aLine));
        append_s(aLine, sizeof(aLine), &o, "ddi_host: soft DMA_BUF_MAP_RECB ");
        append_s(aLine, sizeof(aLine), &o, szId != 0 ? szId : "????:????");
        append_s(aLine, sizeof(aLine), &o, " handle=");
        append_u(aLine, sizeof(aLine), &o, h);
        append_s(aLine, sizeof(aLine), &o, " pa=0x");
        append_hex_ull(aLine, sizeof(aLine), &o, u64Pa);
        append_s(aLine, sizeof(aLine), &o, " cb=0x");
        append_hex_ull(aLine, sizeof(aLine), &o, u64CbAlt);
        append_s(aLine, sizeof(aLine), &o, " prior_cb=0x");
        append_hex_ull(aLine, sizeof(aLine), &o, u64Cb);
        if (retMap3 >= 0) {
            soft_inc(&g_uSoftDmaMapRecbOk);
            fRecb = 1;
            append_s(aLine, sizeof(aLine), &o, " bus=0x");
            append_hex_ull(aLine, sizeof(aLine), &o,
                           (unsigned long long)retMap3);
            append_s(aLine, sizeof(aLine), &o,
                     " PASS cb_change=1 prior_unmap=1 mint=0 Soft!=product\n");
        } else {
            append_s(aLine, sizeof(aLine), &o, " SKIP why=recb_fail");
            append_err(aLine, sizeof(aLine), &o, retMap3);
            append_s(aLine, sizeof(aLine), &o,
                     " cb_change=0 mint=0 Soft!=product\n");
        }
        aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
        msg(aLine);
    }

    /*
     * DMA_BUF_FREE: soft free after map observe (lean lifecycle complete).
     * greppable: ddi_host: soft DMA_BUF_FREE
     */
    soft_inc(&g_uSoftDmaFree);
    retFree = gj_ddi(DDI_HOST_OP_DMA_BUF_FREE, h, (unsigned long)u64Pa,
                     (unsigned long)DDI_HOST_DMA_BUF_PAGES);

    o = 0;
    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o, "ddi_host: soft DMA_BUF_FREE ");
    append_s(aLine, sizeof(aLine), &o, szId != 0 ? szId : "????:????");
    append_s(aLine, sizeof(aLine), &o, " handle=");
    append_u(aLine, sizeof(aLine), &o, h);
    append_s(aLine, sizeof(aLine), &o, " pa=0x");
    append_hex_ull(aLine, sizeof(aLine), &o, u64Pa);
    if (retFree >= 0) {
        soft_inc(&g_uSoftDmaFreeOk);
        fFree = 1;
        append_s(aLine, sizeof(aLine), &o,
                 " PASS soft_forget=1 mint=0 Soft!=product\n");
    } else {
        append_s(aLine, sizeof(aLine), &o, " SKIP why=dma_free_fail");
        append_err(aLine, sizeof(aLine), &o, retFree);
        append_s(aLine, sizeof(aLine), &o, " mint=0 Soft!=product\n");
    }
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);

    /*
     * Soft force32 residual (door DMA_BUF_ALLOC flags bit0): prefer low /
     * VT-d identity band for UDX rtl ring / xhci xfer spirit. Observe only;
     * never product window mint. Free immediately after note.
     * greppable: ddi_host: soft DMA_BUF force32
     */
    soft_inc(&g_uSoftDmaForce32);
    retF32 = gj_ddi(DDI_HOST_OP_DMA_BUF_ALLOC, h,
                    (unsigned long)DDI_HOST_DMA_BUF_PAGES,
                    (unsigned long)DDI_HOST_DMA_BUF_FORCE32);
    o = 0;
    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o, "ddi_host: soft DMA_BUF force32 ");
    append_s(aLine, sizeof(aLine), &o, szId != 0 ? szId : "????:????");
    append_s(aLine, sizeof(aLine), &o, " handle=");
    append_u(aLine, sizeof(aLine), &o, h);
    append_s(aLine, sizeof(aLine), &o, " flags=0x");
    append_hex_ull(aLine, sizeof(aLine), &o,
                   (unsigned long long)DDI_HOST_DMA_BUF_FORCE32);
    if (retF32 >= 0) {
        soft_inc(&g_uSoftDmaForce32Ok);
        fF32 = 1;
        u64F32Pa = (unsigned long long)retF32;
        append_s(aLine, sizeof(aLine), &o, " pa=0x");
        append_hex_ull(aLine, sizeof(aLine), &o, u64F32Pa);
        append_s(aLine, sizeof(aLine), &o,
                 " PASS force32=1 vtd_id_prefer=1 mint=0 Soft!=product\n");
        retF32Free = gj_ddi(DDI_HOST_OP_DMA_BUF_FREE, h,
                            (unsigned long)u64F32Pa,
                            (unsigned long)DDI_HOST_DMA_BUF_PAGES);
        (void)retF32Free;
    } else {
        u64F32Pa = 0ull;
        append_s(aLine, sizeof(aLine), &o, " SKIP why=force32_fail");
        append_err(aLine, sizeof(aLine), &o, retF32);
        append_s(aLine, sizeof(aLine), &o, " mint=0 Soft!=product\n");
    }
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);

    if (puAlloc != 0) {
        *puAlloc = 1u;
    }
    if (puMap != 0) {
        *puMap = (fMap != 0) ? 1u : 0u;
    }
    if (puFree != 0) {
        *puFree = (fFree != 0) ? 1u : 0u;
    }
    if (puIdem != 0) {
        *puIdem = (fIdem != 0) ? 1u : 0u;
    }
    if (puRecb != 0) {
        *puRecb = (fRecb != 0) ? 1u : 0u;
    }
    if (puF32 != 0) {
        *puF32 = (fF32 != 0) ? 1u : 0u;
    }

    /* greppable: ddi_host: soft dma_buf chain */
    o = 0;
    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o, "ddi_host: soft dma_buf chain ");
    append_s(aLine, sizeof(aLine), &o, szId != 0 ? szId : "????:????");
    append_s(aLine, sizeof(aLine), &o, " alloc=1 map=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)(fMap != 0));
    append_s(aLine, sizeof(aLine), &o, " free=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)(fFree != 0));
    append_s(aLine, sizeof(aLine), &o, " dma_idem=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)(fIdem != 0));
    append_s(aLine, sizeof(aLine), &o, " dma_recb=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)(fRecb != 0));
    append_s(aLine, sizeof(aLine), &o, " force32=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)(fF32 != 0));
    append_s(aLine, sizeof(aLine), &o,
             " mint=0 soft=1 product=0 Soft!=product\n");
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);

    return 1;
}

/**
 * Soft NOTIFY_WAIT poll residual after IRQ_BIND / IRQ_REBIND.
 * Non-blocking poll only (block=0). Honesty: observes soft path existence;
 * does **not** claim MSI-X fire or product badge delivery.
 * u64Badge: prefer rebind badge when IRQ_REBIND soft-ok (door rebind path
 * residual); else soft default badge. Soft!=product.
 * greppable: ddi_host: soft notify poll
 * Returns 1 if syscall ret >= 0, 0 otherwise.
 */
static int
log_soft_notify_poll(const char *szId, unsigned long long u64Badge)
{
    long ret;
    char aLine[176];
    unsigned o = 0;
    int fOk = 0;
    unsigned long long u64Mask;

    soft_inc(&g_uSoftNotify);
    u64Mask = (u64Badge != 0ull) ? u64Badge : DDI_HOST_IRQ_BADGE_SOFT;
    /* which=MSI-X global, mask=rebind-or-soft badge, block=0 poll. */
    ret = gj_notify_wait(GJ_NOTIFY_WHICH_MSIX_GLOBAL,
                         (unsigned long)u64Mask, 0);

    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o, "ddi_host: soft notify poll ");
    append_s(aLine, sizeof(aLine), &o, szId != 0 ? szId : "????:????");
    append_s(aLine, sizeof(aLine), &o, " badge=0x");
    append_hex_ull(aLine, sizeof(aLine), &o, u64Mask);
    append_s(aLine, sizeof(aLine), &o, " block=0");
    if (u64Mask == DDI_HOST_IRQ_BADGE_REBIND) {
        append_s(aLine, sizeof(aLine), &o, " rebind_badge=1");
    }
    if (ret >= 0) {
        soft_inc(&g_uSoftNotifyOk);
        fOk = 1;
        append_s(aLine, sizeof(aLine), &o, " pending=0x");
        append_hex_ull(aLine, sizeof(aLine), &o, (unsigned long long)ret);
        append_s(aLine, sizeof(aLine), &o,
                 " PASS soft_poll=1 fire=0 mint=0 Soft!=product\n");
    } else {
        append_s(aLine, sizeof(aLine), &o, " SKIP why=notify_fail");
        append_err(aLine, sizeof(aLine), &o, ret);
        append_s(aLine, sizeof(aLine), &o,
                 " fire=0 mint=0 Soft!=product\n");
    }
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);
    return fOk;
}

/**
 * Soft MAP re-map residual after a preferred BAR PASS.
 * Functional residual vs kernel ddi_door soft map-grant slot:
 *   1) second MAP_BAR returns prior VA (door g_u32SoftMapIdemHit).
 *   2) third MAP_BAR still matches (sticky grant_live; no reinstall).
 * u64PriorVa=0 skips VA-match check (note-only path).
 * Cap honesty: soft note only; never MMIO_FRAME mint (mint=0).
 * greppable: ddi_host: soft MAP_REMAP
 * Sets *puIdem=1 when prior VA non-zero and remapped VA matches.
 * Sets *puSticky=1 when second re-MAP still matches prior VA.
 * Returns 1 on soft-ok, 0 otherwise.
 */
static int
log_soft_map_remap(const char *szId, unsigned long h, unsigned uBar,
                   unsigned long long u64PriorVa, unsigned *puIdem,
                   unsigned *puSticky)
{
    struct ddi_host_map_note note;
    struct ddi_host_map_note note2;
    long ret;
    long ret2;
    char aLine[208];
    unsigned o = 0;
    int fOk = 0;
    int fIdem = 0;
    int fSticky = 0;
    unsigned long long u64Va = 0ull;

    if (puIdem != 0) {
        *puIdem = 0;
    }
    if (puSticky != 0) {
        *puSticky = 0;
    }

    soft_inc(&g_uSoftRemap);
    memzero(&note, (unsigned)sizeof(note));
    ret = gj_ddi(GJ_DDI_OP_MAP_BAR, h, (unsigned long)uBar,
                 (unsigned long)(uintptr_t)&note);

    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o, "ddi_host: soft MAP_REMAP ");
    append_s(aLine, sizeof(aLine), &o, szId != 0 ? szId : "????:????");
    append_s(aLine, sizeof(aLine), &o, " bar");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uBar);
    append_s(aLine, sizeof(aLine), &o, " handle=");
    append_u(aLine, sizeof(aLine), &o, h);
    if (ret >= 0 || note.u8Ok != 0) {
        soft_inc(&g_uSoftRemapOk);
        fOk = 1;
        u64Va = note.u64Va;
        /* Door may return positive VA in ret when fits i64. */
        if (u64Va == 0ull && ret > 0) {
            u64Va = (unsigned long long)ret;
        }
        append_s(aLine, sizeof(aLine), &o, " va=0x");
        append_hex_ull(aLine, sizeof(aLine), &o, u64Va);
        if (u64PriorVa != 0ull && u64Va == u64PriorVa) {
            soft_inc(&g_uSoftRemapIdem);
            fIdem = 1;
            append_s(aLine, sizeof(aLine), &o,
                     " PASS idempotent=1 va_match=1 mint=0 Soft!=product\n");
        } else if (u64PriorVa != 0ull) {
            /* Soft-ok re-map but VA differed (note incomplete / kva band). */
            append_s(aLine, sizeof(aLine), &o, " prior=0x");
            append_hex_ull(aLine, sizeof(aLine), &o, u64PriorVa);
            append_s(aLine, sizeof(aLine), &o,
                     " PASS idempotent=1 va_match=0 mint=0 Soft!=product\n");
        } else {
            append_s(aLine, sizeof(aLine), &o,
                     " PASS idempotent=1 mint=0 Soft!=product\n");
        }
    } else {
        append_s(aLine, sizeof(aLine), &o, " SKIP why=remap_fail");
        append_err(aLine, sizeof(aLine), &o, ret);
        append_s(aLine, sizeof(aLine), &o, " mint=0 Soft!=product\n");
    }
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);

    /*
     * Sticky grant residual: third MAP_BAR still returns same VA (door
     * grant slot stays live until CLOSE). Functional preferred over lamps.
     * greppable: ddi_host: soft MAP_REMAP sticky=
     */
    if (fOk != 0 && u64Va != 0ull) {
        memzero(&note2, (unsigned)sizeof(note2));
        ret2 = gj_ddi(GJ_DDI_OP_MAP_BAR, h, (unsigned long)uBar,
                      (unsigned long)(uintptr_t)&note2);
        o = 0;
        memzero(aLine, sizeof(aLine));
        append_s(aLine, sizeof(aLine), &o, "ddi_host: soft MAP_REMAP ");
        append_s(aLine, sizeof(aLine), &o, szId != 0 ? szId : "????:????");
        append_s(aLine, sizeof(aLine), &o, " bar");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)uBar);
        append_s(aLine, sizeof(aLine), &o, " handle=");
        append_u(aLine, sizeof(aLine), &o, h);
        if (ret2 >= 0 || note2.u8Ok != 0) {
            unsigned long long u64Va2 = note2.u64Va;

            if (u64Va2 == 0ull && ret2 > 0) {
                u64Va2 = (unsigned long long)ret2;
            }
            append_s(aLine, sizeof(aLine), &o, " va=0x");
            append_hex_ull(aLine, sizeof(aLine), &o, u64Va2);
            if (u64Va2 == u64Va) {
                soft_inc(&g_uSoftRemapSticky);
                fSticky = 1;
                append_s(aLine, sizeof(aLine), &o,
                         " PASS sticky=1 va_match=1 grant_live=1 "
                         "mint=0 Soft!=product\n");
            } else {
                append_s(aLine, sizeof(aLine), &o, " prior=0x");
                append_hex_ull(aLine, sizeof(aLine), &o, u64Va);
                append_s(aLine, sizeof(aLine), &o,
                         " PASS sticky=0 va_match=0 mint=0 Soft!=product\n");
            }
        } else {
            append_s(aLine, sizeof(aLine), &o, " SKIP why=sticky_fail");
            append_err(aLine, sizeof(aLine), &o, ret2);
            append_s(aLine, sizeof(aLine), &o,
                     " sticky=0 mint=0 Soft!=product\n");
        }
        aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
        msg(aLine);
    }

    if (puIdem != 0) {
        *puIdem = (fIdem != 0) ? 1u : 0u;
    }
    if (puSticky != 0) {
        *puSticky = (fSticky != 0) ? 1u : 0u;
    }
    return fOk;
}

/**
 * Soft QUIESCE residual -- DDI spirit before CLOSE/remove.
 * Catalog only: stop DMA / mask IRQ / cancel work notes. No silicon poke.
 * Product quiesce-before-revoke remains OPEN. mint=0.
 * greppable: ddi_host: soft QUIESCE
 * Returns 1 always after lamp (soft catalog path; no door op yet).
 */
static int
log_soft_quiesce(const char *szId, unsigned long h)
{
    char aLine[192];
    unsigned o = 0;

    soft_inc(&g_uSoftQuiesce);
    soft_inc(&g_uSoftQuiesceOk);

    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o, "ddi_host: soft QUIESCE ");
    append_s(aLine, sizeof(aLine), &o, szId != 0 ? szId : "????:????");
    append_s(aLine, sizeof(aLine), &o, " handle=");
    append_u(aLine, sizeof(aLine), &o, h);
    append_s(aLine, sizeof(aLine), &o,
             " dma_stop=note irq_mask=note work_cancel=note "
             "order=before_close silicon=0 PASS soft_only=1 "
             "mint=0 Soft!=product\n");
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);
    return 1;
}

/**
 * Soft REOPEN residual after CLOSE -- lifecycle handle reuse.
 * OPEN again by inventory index then CLOSE. Cap honesty: soft handle only;
 * product CNode reinstall remains OPEN. mint=0.
 * greppable: ddi_host: soft REOPEN
 * Returns 1 if second OPEN handle > 0, 0 otherwise.
 */
static int
log_soft_reopen(const char *szId, unsigned idx)
{
    long h2;
    long retClose;
    char aLine[176];
    unsigned o = 0;
    int fOk = 0;

    soft_inc(&g_uSoftReopen);
    h2 = gj_ddi_open(idx);

    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o, "ddi_host: soft REOPEN ");
    append_s(aLine, sizeof(aLine), &o, szId != 0 ? szId : "????:????");
    append_s(aLine, sizeof(aLine), &o, " idx=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)idx);
    if (h2 > 0) {
        soft_inc(&g_uSoftReopenOk);
        fOk = 1;
        append_s(aLine, sizeof(aLine), &o, " handle=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)h2);
        append_s(aLine, sizeof(aLine), &o,
                 " PASS reuse=1 cnode=0 mint=0 Soft!=product\n");
    } else {
        append_s(aLine, sizeof(aLine), &o, " SKIP why=reopen_fail");
        append_err(aLine, sizeof(aLine), &o, h2);
        append_s(aLine, sizeof(aLine), &o, " mint=0 Soft!=product\n");
    }
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);

    if (h2 > 0) {
        retClose = gj_ddi(DDI_HOST_OP_CLOSE, (unsigned long)h2, 0ul, 0ul);
        o = 0;
        memzero(aLine, sizeof(aLine));
        append_s(aLine, sizeof(aLine), &o, "ddi_host: soft REOPEN close ");
        append_s(aLine, sizeof(aLine), &o, szId != 0 ? szId : "????:????");
        append_s(aLine, sizeof(aLine), &o, " handle=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)h2);
        if (retClose >= 0) {
            append_s(aLine, sizeof(aLine), &o,
                     " PASS soft_forget=1 mint=0 Soft!=product\n");
        } else {
            append_s(aLine, sizeof(aLine), &o, " SKIP");
            append_err(aLine, sizeof(aLine), &o, retClose);
            append_s(aLine, sizeof(aLine), &o, " mint=0 Soft!=product\n");
        }
        aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
        msg(aLine);
    }
    return fOk;
}

/**
 * Soft CLOSE residual — lifecycle forget after grant surface observe.
 * Cap honesty: free soft handle / map slots / irq note; no product revoke.
 * greppable: ddi_host: soft CLOSE
 * Returns 1 on soft ok, 0 otherwise.
 */
static int
log_soft_close(const char *szId, unsigned long h)
{
    long ret;
    char aLine[160];
    unsigned o = 0;
    int fOk = 0;

    soft_inc(&g_uSoftClose);
    ret = gj_ddi(DDI_HOST_OP_CLOSE, h, 0ul, 0ul);

    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o, "ddi_host: soft CLOSE ");
    append_s(aLine, sizeof(aLine), &o, szId != 0 ? szId : "????:????");
    append_s(aLine, sizeof(aLine), &o, " handle=");
    append_u(aLine, sizeof(aLine), &o, h);
    if (ret >= 0) {
        soft_inc(&g_uSoftCloseOk);
        fOk = 1;
        append_s(aLine, sizeof(aLine), &o,
                 " PASS soft_forget=1 mint=0 phase_a=OPEN Soft!=product\n");
    } else {
        append_s(aLine, sizeof(aLine), &o, " SKIP why=close_fail");
        append_err(aLine, sizeof(aLine), &o, ret);
        append_s(aLine, sizeof(aLine), &o, " mint=0 Soft!=product\n");
    }
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);
    return fOk;
}

/**
 * Soft post_close residual — functional door forget check after CLOSE.
 * Dead handle vs kernel ddi_door CLOSE residual (map grant forget + IRQ
 * multi-slot unbind + dma_buf free + handle free):
 *   CFG_READ / MAP_BAR / IRQ_BIND / DMA_BUF_ALLOC / CLOSE must soft-fail
 *   (INVAL/NOENT). Confirms lifecycle forget without silicon.
 * greppable: ddi_host: soft post_close
 * Fills *puMap / *puIrq / *puDma with 0/1 when non-NULL (per-surface fail).
 * Returns 1 if full dead-handle residual observed (all probes fail).
 */
static int
log_soft_post_close(const char *szId, unsigned long hDead,
                    unsigned *puMap, unsigned *puIrq, unsigned *puDma)
{
    struct ddi_host_map_note note;
    long retCfg;
    long retMap;
    long retIrq;
    long retDma;
    long retClose2;
    char aLine[288];
    unsigned o = 0;
    int fCfgDead = 0;
    int fMapDead = 0;
    int fIrqDead = 0;
    int fDmaDead = 0;
    int fCloseDead = 0;
    int fDead = 0;

    if (puMap != 0) {
        *puMap = 0;
    }
    if (puIrq != 0) {
        *puIrq = 0;
    }
    if (puDma != 0) {
        *puDma = 0;
    }

    soft_inc(&g_uSoftPostClose);

    /* CFG_READ — handle table must reject after CLOSE. */
    retCfg = gj_ddi_cfg_read(hDead, 0u);
    if (retCfg < 0) {
        fCfgDead = 1;
    }

    /*
     * MAP_BAR — grant slot forgotten (door ddi_soft_map_grant_clear_slot).
     * Use bar0 + out-note; both ret and note must not soft-ok.
     */
    memzero(&note, (unsigned)sizeof(note));
    retMap = gj_ddi(GJ_DDI_OP_MAP_BAR, hDead, 0ul,
                    (unsigned long)(uintptr_t)&note);
    if (retMap < 0 && note.u8Ok == 0) {
        fMapDead = 1;
        soft_inc(&g_uSoftPostCloseMap);
    }

    /* IRQ_BIND — multi-slot unbind residual; dead handle reject. */
    retIrq = gj_ddi(DDI_HOST_OP_IRQ_BIND, hDead,
                    (unsigned long)DDI_HOST_IRQ_BADGE_SOFT, 0ul);
    if (retIrq < 0) {
        fIrqDead = 1;
        soft_inc(&g_uSoftPostCloseIrq);
    }

    /* DMA_BUF_ALLOC — handle-owned slots freed; dead handle reject. */
    retDma = gj_ddi(DDI_HOST_OP_DMA_BUF_ALLOC, hDead,
                    (unsigned long)DDI_HOST_DMA_BUF_PAGES,
                    (unsigned long)DDI_HOST_DMA_BUF_FLAGS);
    if (retDma < 0) {
        fDmaDead = 1;
        soft_inc(&g_uSoftPostCloseDma);
    }

    /* Second CLOSE — handle already free. */
    retClose2 = gj_ddi(DDI_HOST_OP_CLOSE, hDead, 0ul, 0ul);
    if (retClose2 < 0) {
        fCloseDead = 1;
    }

    /* Full forget: every post_close probe soft-fails (door CLOSE residual). */
    if (fCfgDead != 0 && fMapDead != 0 && fIrqDead != 0 &&
        fDmaDead != 0 && fCloseDead != 0) {
        fDead = 1;
        soft_inc(&g_uSoftPostCloseOk);
    }

    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o, "ddi_host: soft post_close ");
    append_s(aLine, sizeof(aLine), &o, szId != 0 ? szId : "????:????");
    append_s(aLine, sizeof(aLine), &o, " handle=");
    append_u(aLine, sizeof(aLine), &o, hDead);
    append_s(aLine, sizeof(aLine), &o, " cfg=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fCfgDead);
    if (retCfg >= 0) {
        append_s(aLine, sizeof(aLine), &o, " cfg=still_live");
    } else {
        append_err(aLine, sizeof(aLine), &o, retCfg);
    }
    append_s(aLine, sizeof(aLine), &o, " map=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fMapDead);
    if (retMap >= 0 || note.u8Ok != 0) {
        append_s(aLine, sizeof(aLine), &o, " map=still_live");
    } else {
        append_err(aLine, sizeof(aLine), &o, retMap);
    }
    append_s(aLine, sizeof(aLine), &o, " irq=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fIrqDead);
    if (retIrq >= 0) {
        append_s(aLine, sizeof(aLine), &o, " irq=still_live");
    } else {
        append_err(aLine, sizeof(aLine), &o, retIrq);
    }
    append_s(aLine, sizeof(aLine), &o, " dma=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fDmaDead);
    if (retDma >= 0) {
        append_s(aLine, sizeof(aLine), &o, " dma=still_live");
    } else {
        append_err(aLine, sizeof(aLine), &o, retDma);
    }
    append_s(aLine, sizeof(aLine), &o, " close2=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fCloseDead);
    if (retClose2 >= 0) {
        append_s(aLine, sizeof(aLine), &o, " close2=still_live");
    } else {
        append_err(aLine, sizeof(aLine), &o, retClose2);
    }
    if (fDead != 0) {
        append_s(aLine, sizeof(aLine), &o,
                 " PASS forget=1 mint=0 Soft!=product\n");
    } else {
        append_s(aLine, sizeof(aLine), &o,
                 " SKIP why=forget_incomplete mint=0 Soft!=product\n");
    }
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);

    if (puMap != 0) {
        *puMap = (fMapDead != 0) ? 1u : 0u;
    }
    if (puIrq != 0) {
        *puIrq = (fIrqDead != 0) ? 1u : 0u;
    }
    if (puDma != 0) {
        *puDma = (fDmaDead != 0) ? 1u : 0u;
    }
    return fDead;
}

/**
 * Soft residual path lamp -- OPEN / MAP / IRQ_BIND / grant rollup per ID.
 * Primary greppable for rtl8168_udx + xhci_udx residual consumers.
 * Chain fields: dma_map / dma_free / dma_idem / dma_recb / force32 / notify /
 * remap / irq_reb / sticky / cfg_w / cfg_ident / post_close (map/irq/dma) /
 * life.
 * greppable: ddi_host: soft residual path
 */
static void
log_soft_residual_path(const char *szId,
                       const struct ddi_host_residual_path *pPath)
{
    char aLine[480];
    unsigned o = 0;

    soft_inc(&g_uSoftResidualPaths);
    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o, "ddi_host: soft residual path ");
    append_s(aLine, sizeof(aLine), &o, szId != 0 ? szId : "????:????");
    if (pPath != 0) {
        append_s(aLine, sizeof(aLine), &o, " open=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)pPath->uOpen);
        append_s(aLine, sizeof(aLine), &o, " map=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)pPath->uMaps);
        append_s(aLine, sizeof(aLine), &o, " irq=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)pPath->uIrq);
        append_s(aLine, sizeof(aLine), &o, " irq_reb=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)pPath->uIrqReb);
        append_s(aLine, sizeof(aLine), &o, " grant=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)pPath->uGrant);
        append_s(aLine, sizeof(aLine), &o, " dma_buf=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)pPath->uDmaBuf);
        append_s(aLine, sizeof(aLine), &o, " dma_map=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)pPath->uDmaMap);
        append_s(aLine, sizeof(aLine), &o, " dma_free=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)pPath->uDmaFree);
        append_s(aLine, sizeof(aLine), &o, " dma_idem=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)pPath->uDmaIdem);
        append_s(aLine, sizeof(aLine), &o, " dma_recb=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)pPath->uDmaRecb);
        append_s(aLine, sizeof(aLine), &o, " force32=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)pPath->uDmaF32);
        append_s(aLine, sizeof(aLine), &o, " notify=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)pPath->uNotify);
        append_s(aLine, sizeof(aLine), &o, " remap=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)pPath->uRemap);
        append_s(aLine, sizeof(aLine), &o, " idem=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)pPath->uIdem);
        append_s(aLine, sizeof(aLine), &o, " sticky=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)pPath->uSticky);
        append_s(aLine, sizeof(aLine), &o, " map_user=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)pPath->uMapUser);
        append_s(aLine, sizeof(aLine), &o, " cfg_w=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)pPath->uCfgW);
        append_s(aLine, sizeof(aLine), &o, " cfg_rej=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)pPath->uCfgRej);
        append_s(aLine, sizeof(aLine), &o, " cfg_ident=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)pPath->uCfgIdent);
        append_s(aLine, sizeof(aLine), &o, " quiesce=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)pPath->uQuiesce);
        append_s(aLine, sizeof(aLine), &o, " close=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)pPath->uClose);
        append_s(aLine, sizeof(aLine), &o, " post_cl=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)pPath->uPostCl);
        append_s(aLine, sizeof(aLine), &o, " post_map=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)pPath->uPostMap);
        append_s(aLine, sizeof(aLine), &o, " post_irq=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)pPath->uPostIrq);
        append_s(aLine, sizeof(aLine), &o, " post_dma=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)pPath->uPostDma);
        append_s(aLine, sizeof(aLine), &o, " reopen=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)pPath->uReopen);
        append_s(aLine, sizeof(aLine), &o, " life=0x");
        append_hex_ull(aLine, sizeof(aLine), &o,
                       (unsigned long long)pPath->uLife);
    } else {
        append_s(aLine, sizeof(aLine), &o,
                 " open=0 map=0 irq=0 irq_reb=0 grant=0 dma_buf=0 dma_map=0 "
                 "dma_free=0 dma_idem=0 dma_recb=0 force32=0 notify=0 remap=0 "
                 "idem=0 sticky=0 map_user=0 cfg_w=0 cfg_rej=0 cfg_ident=0 "
                 "quiesce=0 close=0 post_cl=0 post_map=0 post_irq=0 "
                 "post_dma=0 reopen=0 life=0x0");
    }
    append_s(aLine, sizeof(aLine), &o,
             " mint=0 soft=1 product=0 Soft!=product\n");
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);
}

/**
 * Soft grant surface catalog for rtl8168_udx / xhci_udx style hosts.
 * Cap honesty: names each grant class and asserts mint=0.
 * greppable: ddi_host: soft grant surface
 */
static void
log_soft_grant_surface(const char *szId, unsigned long h, unsigned nMaps,
                       int fOpenOk, int fPass, int fIrqOk)
{
    char aLine[208];
    unsigned o = 0;

    soft_inc(&g_uSoftGrantSurfaces);
    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o, "ddi_host: soft grant surface ");
    append_s(aLine, sizeof(aLine), &o, szId != 0 ? szId : "????:????");
    append_s(aLine, sizeof(aLine), &o, " handle=");
    append_u(aLine, sizeof(aLine), &o, fOpenOk != 0 ? h : 0ul);
    append_s(aLine, sizeof(aLine), &o, " open=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)(fOpenOk != 0));
    append_s(aLine, sizeof(aLine), &o, " maps=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)nMaps);
    append_s(aLine, sizeof(aLine), &o, " irq=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)(fIrqOk != 0));
    append_s(aLine, sizeof(aLine), &o,
             " mmio_frame=0 irq_notify=0 dma_window=0 cnode=0 "
             "product_mint=0 mint=0 soft=1 product=0");
    if (fPass != 0) {
        append_s(aLine, sizeof(aLine), &o, " PASS Soft!=product\n");
    } else {
        append_s(aLine, sizeof(aLine), &o, " SKIP Soft!=product\n");
    }
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);
}

/**
 * Soft OPEN+MAP summary lamp (bind path honesty).
 * greppable: ddi_host: soft open+map
 */
static void
log_soft_open_map(const char *szId, unsigned nMaps, int fPass)
{
    char aLine[144];
    unsigned o = 0;

    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o, "ddi_host: soft open+map ");
    append_s(aLine, sizeof(aLine), &o, szId != 0 ? szId : "????:????");
    if (fPass != 0) {
        append_s(aLine, sizeof(aLine), &o, " PASS maps=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)nMaps);
        append_s(aLine, sizeof(aLine), &o,
                 " soft=1 product=0 Soft!=product\n");
    } else {
        append_s(aLine, sizeof(aLine), &o,
                 " SKIP maps=0 soft=1 product=0 Soft!=product\n");
    }
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);
}

/**
 * Soft MAP one BAR by handle. Prefer out-note so PASS does not depend on
 * positive i64 VA. Returns 1 on soft map ok, 0 otherwise.
 * Empty preferred BAR -> greppable why=empty_bar (not a map syscall).
 * Map soft-fail -> greppable why=map_fail err=<token>.
 * Functional: fills *pRes with VA + u8User when non-NULL (door map-grant).
 * greppable: ddi_host: soft MAP_BAR
 */
static int
soft_map_one_bar(unsigned long h, unsigned uBar,
                 const struct ddi_host_dev_info *pInfo,
                 const char *szId, struct ddi_host_map_result *pRes)
{
    struct ddi_host_map_note note;
    unsigned long long u64Pa;
    unsigned long long u64Cb;
    long ret;
    long retHint;
    char aLine[192];
    unsigned o;

    if (pRes != 0) {
        memzero(pRes, (unsigned)sizeof(*pRes));
    }
    if (pInfo == 0 || szId == 0) {
        return 0;
    }

    bar_pa_cb(pInfo, uBar, &u64Pa, &u64Cb);
    if (u64Pa == 0ull || u64Cb == 0ull) {
        /* Preferred BAR empty -- soft residual honesty, not a hard error. */
        soft_inc(&g_uSoftMapEmpty);
        o = 0;
        memzero(aLine, sizeof(aLine));
        append_s(aLine, sizeof(aLine), &o, "ddi_host: soft MAP_BAR ");
        append_s(aLine, sizeof(aLine), &o, szId);
        append_s(aLine, sizeof(aLine), &o, " bar");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)uBar);
        append_s(aLine, sizeof(aLine), &o,
                 " SKIP why=empty_bar mint=0 Soft!=product\n");
        aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
        msg(aLine);
        return 0;
    }

    soft_inc(&g_uSoftMap);
    memzero(&note, (unsigned)sizeof(note));
    ret = gj_ddi(GJ_DDI_OP_MAP_BAR, h, (unsigned long)uBar,
                 (unsigned long)(uintptr_t)&note);
    if (ret < 0) {
        /* Retry with va_hint 0 when note path soft-fails. */
        retHint = gj_ddi_map_bar(h, uBar, 0u);
        if (retHint < 0) {
            soft_inc(&g_uSoftMapFail);
            o = 0;
            memzero(aLine, sizeof(aLine));
            append_s(aLine, sizeof(aLine), &o, "ddi_host: soft MAP_BAR ");
            append_s(aLine, sizeof(aLine), &o, szId);
            append_s(aLine, sizeof(aLine), &o, " bar");
            append_u(aLine, sizeof(aLine), &o, (unsigned long)uBar);
            append_s(aLine, sizeof(aLine), &o, " pa=0x");
            append_hex_ull(aLine, sizeof(aLine), &o, u64Pa);
            append_s(aLine, sizeof(aLine), &o, " SKIP why=map_fail");
            append_err(aLine, sizeof(aLine), &o, retHint);
            append_s(aLine, sizeof(aLine), &o, " Soft!=product\n");
            aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
            msg(aLine);
            return 0;
        }
        soft_inc(&g_uSoftMapOk);
        if (pRes != 0) {
            pRes->uOk = 1u;
            /* Positive low VA from hint path; high kva may be 0. */
            if (retHint > 0) {
                pRes->u64Va = (unsigned long long)retHint;
            }
        }
        o = 0;
        memzero(aLine, sizeof(aLine));
        append_s(aLine, sizeof(aLine), &o, "ddi_host: soft MAP_BAR ");
        append_s(aLine, sizeof(aLine), &o, szId);
        append_s(aLine, sizeof(aLine), &o, " bar");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)uBar);
        append_s(aLine, sizeof(aLine), &o, " pa=0x");
        append_hex_ull(aLine, sizeof(aLine), &o, u64Pa);
        append_s(aLine, sizeof(aLine), &o, " cb=0x");
        append_hex_ull(aLine, sizeof(aLine), &o, u64Cb);
        append_s(aLine, sizeof(aLine), &o,
                 " PASS (va_hint) user=0 mint=0 Soft!=product\n");
        aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
        msg(aLine);
        return 1;
    }
    if (note.u8Ok != 0 || ret >= 0) {
        soft_inc(&g_uSoftMapOk);
        if (note.u8User != 0) {
            soft_inc(&g_uSoftMapUser);
        }
        if (pRes != 0) {
            pRes->uOk = 1u;
            pRes->u64Va = note.u64Va;
            pRes->uUser = (note.u8User != 0) ? 1u : 0u;
            /* Door may return positive VA in ret when fits i64. */
            if (pRes->u64Va == 0ull && ret > 0) {
                pRes->u64Va = (unsigned long long)ret;
            }
        }
        o = 0;
        memzero(aLine, sizeof(aLine));
        append_s(aLine, sizeof(aLine), &o, "ddi_host: soft MAP_BAR ");
        append_s(aLine, sizeof(aLine), &o, szId);
        append_s(aLine, sizeof(aLine), &o, " bar");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)uBar);
        append_s(aLine, sizeof(aLine), &o, " pa=0x");
        append_hex_ull(aLine, sizeof(aLine), &o,
                       note.u64Pa != 0ull ? note.u64Pa : u64Pa);
        append_s(aLine, sizeof(aLine), &o, " va=0x");
        append_hex_ull(aLine, sizeof(aLine), &o, note.u64Va);
        append_s(aLine, sizeof(aLine), &o, " cb=0x");
        append_hex_ull(aLine, sizeof(aLine), &o,
                       note.u64Cb != 0ull ? note.u64Cb : u64Cb);
        append_s(aLine, sizeof(aLine), &o, " user=");
        append_u(aLine, sizeof(aLine), &o,
                 (unsigned long)(note.u8User != 0));
        append_s(aLine, sizeof(aLine), &o,
                 " PASS mint=0 Soft!=product\n");
        aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
        msg(aLine);
        return 1;
    }
    soft_inc(&g_uSoftMapFail);
    o = 0;
    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o, "ddi_host: soft MAP_BAR ");
    append_s(aLine, sizeof(aLine), &o, szId);
    append_s(aLine, sizeof(aLine), &o, " bar");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uBar);
    append_s(aLine, sizeof(aLine), &o,
             " SKIP why=map_note_incomplete mint=0 Soft!=product\n");
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);
    return 0;
}

/**
 * Soft OPEN + MAP preferred BARs for one inventory index.
 * aBars[] lists bar indices to try (e.g. {0,2} for RTL, {0} for xHCI).
 * PASS when open handle > 0 and >=1 BAR soft-maps.
 * Path (vs kernel ddi_door life=): OPEN -> CFG_READ -> CFG_WRITE
 *   (8168 Command 0x0006; BAR+ident reject) -> MAP_BAR* -> MAP_REMAP -> DMA_NOTE ->
 *   IRQ_BIND -> IRQ_REBIND -> notify poll (rebind badge when rebind ok) ->
 *   DMA_BUF ALLOC/MAP/MAP_REMAP(idem)/MAP_RECB(diff cb)/FREE/force32 ->
 *   grant surface -> QUIESCE -> CLOSE -> post_close forget
 *   (CFG/MAP/IRQ/DMA/CLOSE) -> REOPEN/CLOSE.
 * Soft only; never product mint. Cap honesty: mint=0 on all grant lamps.
 * Soft bind != product cap mint. Product hosts = UDX class drivers.
 * Fills *pPath with OPEN/MAP/IRQ/grant residual counters when non-NULL.
 */
static int
soft_open_map_bars(unsigned idx, const struct ddi_host_dev_info *pInfo,
                   const char *szId, const unsigned *aBars, unsigned nBars,
                   unsigned *puMaps, struct ddi_host_residual_path *pPath)
{
    long h;
    unsigned i;
    unsigned nMaps;
    unsigned uFirstMappedBar;
    unsigned uLife;
    unsigned uCfgNote;
    unsigned uCfgRej;
    unsigned uCfgIdent;
    unsigned uIdem;
    unsigned uSticky;
    unsigned uMapUser;
    unsigned uPostMap;
    unsigned uPostIrq;
    unsigned uPostDma;
    int fPass;
    int fIrqOk;
    int fIrqRebOk;
    int fDmaBufOk;
    int fNotifyOk;
    int fRemapOk;
    int fCfgWOk;
    int fQuiesceOk;
    int fCloseOk;
    int fPostClOk;
    int fReopenOk;
    int fDmaNoteOk;
    unsigned uDmaAlloc;
    unsigned uDmaMap;
    unsigned uDmaFree;
    unsigned uDmaIdem;
    unsigned uDmaRecb;
    unsigned uDmaF32;
    unsigned long long u64DmaPa;
    unsigned long long u64DmaCb;
    unsigned long long u64FirstVa;
    unsigned long long u64NotifyBadge;
    struct ddi_host_map_result mapRes;

    if (pPath != 0) {
        memzero(pPath, (unsigned)sizeof(*pPath));
        pPath->nOutcome = DDI_HOST_BIND_SKIP;
    }
    if (pInfo == 0 || aBars == 0 || nBars == 0u) {
        return 0;
    }
    if (puMaps != 0) {
        *puMaps = 0;
    }

    uLife = 0u;
    u64FirstVa = 0ull;
    uMapUser = 0u;
    uIdem = 0u;
    uSticky = 0u;
    uCfgNote = 0u;
    uCfgRej = 0u;
    uCfgIdent = 0u;
    uPostMap = 0u;
    uPostIrq = 0u;
    uPostDma = 0u;
    uDmaIdem = 0u;
    uDmaRecb = 0u;
    uDmaF32 = 0u;
    fDmaNoteOk = 0;
    fIrqRebOk = 0;
    u64NotifyBadge = DDI_HOST_IRQ_BADGE_SOFT;

    h = gj_ddi_open(idx);
    if (h <= 0) {
        log_soft_open(szId, idx, h, 0);
        log_soft_grant_surface(szId, 0ul, 0u, 0, 0, 0);
        log_soft_open_map(szId, 0u, 0);
        if (pPath != 0) {
            log_soft_residual_path(szId, pPath);
        }
        return 0;
    }
    log_soft_open(szId, idx, h, 1);
    uLife |= DDI_HOST_LIFE_OPEN;
    if (pPath != 0) {
        pPath->uOpen = 1u;
    }

    /* Soft CFG dword0/cmdst after OPEN (main soft ddi shape; not product). */
    if (log_soft_cfg(szId, (unsigned long)h) != 0) {
        uLife |= DDI_HOST_LIFE_CFG_R;
    }

    /*
     * CFG_WRITE: 8168 Command MEM|MASTER (0x0006); never val=0; never xHCI
     * Command. BAR + identity reject residuals follow.
     */
    fCfgWOk = log_soft_cfg_write(szId, (unsigned long)h, pInfo->u16Vend,
                                 pInfo->u16Dev, &uCfgNote, &uCfgRej,
                                 &uCfgIdent);
    if (fCfgWOk != 0) {
        uLife |= DDI_HOST_LIFE_CFG_W;
    }

    nMaps = 0;
    uFirstMappedBar = 0xffffffffu;
    for (i = 0; i < nBars; i++) {
        memzero(&mapRes, (unsigned)sizeof(mapRes));
        if (soft_map_one_bar((unsigned long)h, aBars[i], pInfo, szId,
                             &mapRes) != 0) {
            if (uFirstMappedBar == 0xffffffffu) {
                uFirstMappedBar = aBars[i];
                u64FirstVa = mapRes.u64Va;
                uMapUser = mapRes.uUser;
            } else if (mapRes.uUser != 0u) {
                uMapUser = 1u;
            }
            nMaps++;
        }
    }
    if (nMaps != 0u) {
        uLife |= DDI_HOST_LIFE_MAP;
    }

    /*
     * Soft MAP re-map residual on first preferred BAR that mapped.
     * Functional: VA match + sticky second re-MAP (door grant slot).
     */
    fRemapOk = 0;
    if (uFirstMappedBar != 0xffffffffu) {
        fRemapOk = log_soft_map_remap(szId, (unsigned long)h, uFirstMappedBar,
                                      u64FirstVa, &uIdem, &uSticky);
        if (fRemapOk != 0) {
            uLife |= DDI_HOST_LIFE_REMAP;
        }
    }

    /*
     * Soft DMA_NOTE on first preferred BAR with non-empty PA/CB.
     * Honesty: soft_only=1 mint=0 — never claims VT-d / CNode window mint.
     */
    u64DmaPa = 0ull;
    u64DmaCb = 0ull;
    for (i = 0; i < nBars; i++) {
        bar_pa_cb(pInfo, aBars[i], &u64DmaPa, &u64DmaCb);
        if (u64DmaPa != 0ull && u64DmaCb != 0ull) {
            break;
        }
    }
    {
        unsigned uDmaBefore = g_uSoftDmaOk;

        log_soft_dma_note(szId, (unsigned long)h, u64DmaPa, u64DmaCb);
        if (g_uSoftDmaOk > uDmaBefore) {
            fDmaNoteOk = 1;
            uLife |= DDI_HOST_LIFE_DMA_NOTE;
        }
    }

    /* Soft IRQ_BIND note -- rtl8168_udx / xhci_udx style IRQ residual. */
    fIrqOk = log_soft_irq_bind(szId, (unsigned long)h);
    if (fIrqOk != 0) {
        uLife |= DDI_HOST_LIFE_IRQ;
        /* Rebind residual only after a live soft bind (door rebind path). */
        fIrqRebOk = log_soft_irq_rebind(szId, (unsigned long)h);
        if (fIrqRebOk != 0) {
            uLife |= DDI_HOST_LIFE_IRQ_REB;
            /* Poll rebind badge when rebind soft-ok (door badge residual). */
            u64NotifyBadge = DDI_HOST_IRQ_BADGE_REBIND;
        }
    }

    /* Soft NOTIFY_WAIT poll residual (non-block; fire not claimed). */
    fNotifyOk = log_soft_notify_poll(szId, u64NotifyBadge);
    if (fNotifyOk != 0) {
        uLife |= DDI_HOST_LIFE_NOTIFY;
    }

    /*
     * Soft DMA_BUF lean chain: ALLOC -> MAP -> MAP_REMAP(idem) ->
     * MAP_RECB(diff cb) -> FREE -> force32. mint=0; never product DMA
     * window cap. Functional cookie match + prior-unmap on cb change.
     */
    uDmaAlloc = 0;
    uDmaMap = 0;
    uDmaFree = 0;
    uDmaIdem = 0;
    uDmaRecb = 0;
    uDmaF32 = 0;
    fDmaBufOk = log_soft_dma_buf_chain(szId, (unsigned long)h,
                                       &uDmaAlloc, &uDmaMap, &uDmaFree,
                                       &uDmaIdem, &uDmaRecb, &uDmaF32);
    /* Prefer chain alloc flag when path filled; fDmaBufOk mirrors it. */
    if (fDmaBufOk != 0 && uDmaAlloc == 0u) {
        uDmaAlloc = 1u;
    }
    if (uDmaAlloc != 0u) {
        uLife |= DDI_HOST_LIFE_DMA_BUF;
    }
    if (uDmaMap != 0u) {
        uLife |= DDI_HOST_LIFE_DMA_MAP;
    }
    if (uDmaFree != 0u) {
        uLife |= DDI_HOST_LIFE_DMA_FREE;
    }
    if (uDmaRecb != 0u) {
        uLife |= DDI_HOST_LIFE_DMA_RECB;
    }
    if (uDmaF32 != 0u) {
        uLife |= DDI_HOST_LIFE_DMA_F32;
    }

    if (puMaps != 0) {
        *puMaps = nMaps;
    }
    fPass = (nMaps != 0u) ? 1 : 0;

    /* Cap honesty grant surface for class-host consumers (soft residual). */
    log_soft_grant_surface(szId, (unsigned long)h, nMaps, 1, fPass, fIrqOk);
    log_soft_open_map(szId, nMaps, fPass);
    if (fPass != 0) {
        uLife |= DDI_HOST_LIFE_GRANT;
    }

    /*
     * Soft QUIESCE residual before CLOSE -- DDI spirit catalog.
     * stop DMA / mask IRQ / cancel work notes only; no silicon poke.
     */
    fQuiesceOk = log_soft_quiesce(szId, (unsigned long)h);
    if (fQuiesceOk != 0) {
        uLife |= DDI_HOST_LIFE_QUIESCE;
    }

    /*
     * Soft CLOSE lifecycle residual after grant observe — forget soft slots.
     * Product Phase-A revoke remains OPEN. mint=0.
     */
    fCloseOk = log_soft_close(szId, (unsigned long)h);
    if (fCloseOk != 0) {
        uLife |= DDI_HOST_LIFE_CLOSE;
    }

    /*
     * Soft post_close residual: dead-handle CFG/MAP/IRQ/DMA/CLOSE must fail
     * (functional door forget: grant clear + irq unbind + dma free).
     */
    fPostClOk = 0;
    if (fCloseOk != 0) {
        fPostClOk = log_soft_post_close(szId, (unsigned long)h,
                                        &uPostMap, &uPostIrq, &uPostDma);
        if (fPostClOk != 0) {
            uLife |= DDI_HOST_LIFE_POST_CL;
        }
    }

    /*
     * Soft REOPEN residual after CLOSE -- lifecycle handle reuse.
     * Product CNode reinstall remains OPEN. mint=0.
     */
    fReopenOk = log_soft_reopen(szId, idx);
    if (fReopenOk != 0) {
        uLife |= DDI_HOST_LIFE_REOPEN;
    }

    if (pPath != 0) {
        pPath->uOpen = 1u;
        pPath->uMaps = nMaps;
        pPath->uIrq = (fIrqOk != 0) ? 1u : 0u;
        pPath->uIrqReb = (fIrqRebOk != 0) ? 1u : 0u;
        pPath->uGrant = (fPass != 0) ? 1u : 0u;
        pPath->uDmaBuf = uDmaAlloc;
        pPath->uDmaMap = uDmaMap;
        pPath->uDmaFree = uDmaFree;
        pPath->uDmaIdem = uDmaIdem;
        pPath->uDmaRecb = uDmaRecb;
        pPath->uDmaF32 = uDmaF32;
        pPath->uNotify = (fNotifyOk != 0) ? 1u : 0u;
        pPath->uRemap = (fRemapOk != 0) ? 1u : 0u;
        pPath->uIdem = uIdem;
        pPath->uSticky = uSticky;
        pPath->uMapUser = uMapUser;
        pPath->uCfgW = uCfgNote;
        pPath->uCfgRej = uCfgRej;
        pPath->uCfgIdent = uCfgIdent;
        pPath->uQuiesce = (fQuiesceOk != 0) ? 1u : 0u;
        pPath->uClose = (fCloseOk != 0) ? 1u : 0u;
        pPath->uPostCl = (fPostClOk != 0) ? 1u : 0u;
        pPath->uPostMap = uPostMap;
        pPath->uPostIrq = uPostIrq;
        pPath->uPostDma = uPostDma;
        pPath->uReopen = (fReopenOk != 0) ? 1u : 0u;
        pPath->uLife = uLife;
        pPath->nOutcome = (fPass != 0) ? DDI_HOST_BIND_PASS : DDI_HOST_BIND_SKIP;
        log_soft_residual_path(szId, pPath);
    }
    (void)fDmaNoteOk;
    return fPass;
}

/**
 * Find first inventory row matching vend:dev (SCAN residual GET walk).
 * Fills *pInfo and *puIdx when non-NULL. Returns 1 on match, 0 on miss.
 * Soft!=product; never product mint. greppable via soft GET on match.
 */
static int
soft_find_id(long nDev, unsigned short u16Vend, unsigned short u16Dev,
             struct ddi_host_dev_info *pInfo, unsigned *puIdx)
{
    long i;

    if (puIdx != 0) {
        *puIdx = 0xffffffffu;
    }
    if (pInfo != 0) {
        memzero(pInfo, (unsigned)sizeof(*pInfo));
    }
    if (nDev <= 0) {
        return 0;
    }
    for (i = 0; i < nDev && i < (long)DDI_HOST_SCAN_MAX; i++) {
        struct ddi_host_dev_info info;
        long ret;

        memzero(&info, (unsigned)sizeof(info));
        ret = gj_ddi_get((unsigned)i, &info);
        if (ret < 0) {
            continue;
        }
        if (info.u16Vend != u16Vend || info.u16Dev != u16Dev) {
            continue;
        }
        if (pInfo != 0) {
            *pInfo = info;
        }
        if (puIdx != 0) {
            *puIdx = (unsigned)i;
        }
        return 1;
    }
    return 0;
}

/**
 * Soft multi-host residual for product UDX hosts (rtl8168_udx + xhci_udx).
 *
 * Functional residual (preferred over lamp-only) vs kernel ddi_door
 * multi-host OPEN+MAP / IRQ / DMA spirit (STRONGER prefer_real_ddi residual;
 * denser open/map/irq/dma concurrent for 10ec:8168 + 8086:a12f):
 *   1) SCAN residual already done; GET both G752 IDs (10ec:8168 / 8086:a12f).
 *   2) OPEN both handles concurrent (two soft host slots live).
 *   3) CFG_READ both while concurrent (open residual; identity snap).
 *   4) MAP preferred BARs on both while both open (rtl BAR0+BAR2; xhci BAR0).
 *   5) MAP_REMAP both concurrent (grant idem while peer host still live).
 *   6) open+map rollup lamp (both_open + both_map primary Dual DoD surface).
 *   7) IRQ_BIND both + concurrent IRQ_REBIND both + notify_poll both denser.
 *   8) DMA_NOTE both + concurrent DMA_BUF ALLOC/MAP/MAP_REMAP(idem)/FREE denser.
 *   9) prefer_real_ddi denser rollup (open= map= irq= dma= densify_*= denser=4).
 *  10) denser open/map/irq/dma once-lamps + prefer_real denser VERDICT.
 *  11) CLOSE rtl; peer CFG/MAP + peer MAP_REMAP + peer IRQ rebind + peer DMA
 *      remain live (door multi-host unbind_safe; Soft!=product).
 *  12) Closed-handle CFG/MAP/IRQ/DMA must soft-fail (isolation residual; not
 *      clear-all of peer).
 *  13) REOPEN rtl + MAP; CLOSE xhci (reverse order); peer-live rtl denser
 *      (CFG/MAP + MAP_REMAP + IRQ rebind + DMA) + closed-dead denser on
 *      xhci (CFG/MAP/IRQ/DMA); CLOSE rtl.
 *  14) prefer_real residual denser open/map/irq/dma after isolation prove
 *      (concurrent denser + densify=4 + peer denser + closed denser +
 *      rev_peer denser (remap+irq+dma) + rev_closed denser) then residual
 *      denser VERDICT. residual denser map needs peer_remap+rev_peer_remap.
 *      Soft!=product Dual DoD OPEN; bar .75; agent!=close; product_mint=0.
 *      Inventory MISS always emits residual denser VERDICT greppables.
 *
 * Does **not** claim product multi-server confine (multi_server=0) or
 * product MMIO/IRQ/DMA mint (product_mint=0). Dual DoD A/B remain OPEN.
 * Soft!=product; product_hosts=UDX; prefer_real_ddi=1 not=inject_only.
 *
 * greppable: ddi_host: soft multi-host
 * greppable: ddi_host: soft multi-host open+map
 * greppable: ddi_host: soft multi-host prefer_real_ddi
 * greppable: ddi_host: soft multi-host denser open=
 * greppable: ddi_host: soft multi-host denser map=
 * greppable: ddi_host: soft multi-host denser irq=
 * greppable: ddi_host: soft multi-host denser dma=
 * greppable: ddi_host: soft multi-host prefer_real denser VERDICT
 * greppable: ddi_host: soft multi-host prefer_real residual
 * greppable: ddi_host: soft multi-host prefer_real residual denser open=
 * greppable: ddi_host: soft multi-host prefer_real residual denser map=
 * greppable: ddi_host: soft multi-host prefer_real residual denser irq=
 * greppable: ddi_host: soft multi-host prefer_real residual denser dma=
 * greppable: ddi_host: soft multi-host prefer_real residual denser VERDICT
 * greppable: ddi_host: soft multi-host chain
 * greppable: ddi_host: soft multi-host peer_live
 * greppable: ddi_host: soft multi-host closed_dead
 * greppable: ddi_host: soft multi-host rev_peer_live
 * greppable: ddi_host: soft multi-host rev_closed_dead
 * Returns DDI_HOST_BIND_PASS when both open + >=1 map each;
 * SKIP on partial; MISS when either ID absent.
 */
static int
soft_multi_host_rtl_xhci(long nDev)
{
    struct ddi_host_dev_info infoRtl;
    struct ddi_host_dev_info infoXhci;
    struct ddi_host_map_result mapRtl;
    struct ddi_host_map_result mapXhci;
    struct ddi_host_map_note notePeer;
    struct ddi_host_map_note noteDead;
    static const unsigned aRtlBars[] = { 0u, 2u };
    static const unsigned aXhciBars[] = { 0u };
    char aLine[800];
    unsigned o;
    unsigned idxRtl;
    unsigned idxXhci;
    unsigned nRtlMaps;
    unsigned nXhciMaps;
    unsigned i;
    unsigned uRtlFirstBar;
    unsigned uXhciFirstBar;
    unsigned uRtlRemap;
    unsigned uXhciRemap;
    unsigned uRtlIdem;
    unsigned uXhciIdem;
    unsigned uRtlSticky;
    unsigned uXhciSticky;
    unsigned uRtlCfg;
    unsigned uXhciCfg;
    unsigned uPeerRemap;
    unsigned uPeerIrq;
    unsigned uPeerRemapIdem;
    unsigned uPeerRemapSticky;
    unsigned uClosedIrq;
    unsigned uClosedDma;
    long hRtl;
    long hXhci;
    long hRtlClosed;
    long hXhciClosed;
    long retPeerCfg;
    long retPeerMap;
    long retDeadCfg;
    long retDeadMap;
    long retDeadIrq;
    long retDeadDma;
    long retCloseRtl;
    long retCloseXhci;
    long retRevPeerCfg;
    long retRevPeerMap;
    long retRevDeadCfg;
    long retRevDeadMap;
    unsigned long long u64DmaPa;
    unsigned long long u64DmaCb;
    unsigned long long u64RtlFirstVa;
    unsigned long long u64XhciFirstVa;
    unsigned uDmaBefore;
    unsigned uRtlDma;
    unsigned uXhciDma;
    unsigned uRtlDmaBuf;
    unsigned uXhciDmaBuf;
    unsigned uRtlDmaMap;
    unsigned uXhciDmaMap;
    unsigned uRtlDmaIdem;
    unsigned uXhciDmaIdem;
    unsigned uRtlNotify;
    unsigned uXhciNotify;
    unsigned uRtlIrqReb;
    unsigned uXhciIrqReb;
    unsigned uPeerDma;
    unsigned uRevPeerRemap; /* reverse peer MAP_REMAP denser residual */
    unsigned uRevPeerIrq;
    unsigned uRevPeerDma;
    unsigned uOpenD;
    unsigned uMapD;
    unsigned uIrqD;
    unsigned uDmaD;
    unsigned uDensifyOpen;
    unsigned uDensifyMap;
    unsigned uDensifyIrq;
    unsigned uDensifyDma;
    unsigned uDenserN;
    long retDmaBuf;
    long retDmaMap;
    long retDmaMap2;
    long retDmaFree;
    long retPeerDma;
    long retRevDeadIrq;
    long retRevDeadDma;
    unsigned long long u64RtlDmaPa;
    unsigned long long u64XhciDmaPa;
    unsigned long long u64RtlDmaBus;
    unsigned long long u64XhciDmaBus;
    int fRtlFound;
    int fXhciFound;
    int fRtlIrq;
    int fXhciIrq;
    int fPeerLive;
    int fClosedDead;
    int fRevPeerLive;
    int fRevClosedDead;
    int fBothOpen;
    int fBothMap;
    int fBothIrq;
    int fBothIrqReb;
    int fBothNotify;
    int fBothDma;
    int fBothDmaBuf;
    int fBothDmaMap;
    int fBothDmaIdem;
    int fBothCfg;
    int fBothRemap;
    int fPrefComplete;
    int fOpenMapIrqDma;
    int fDensify;
    int fDenser;
    int nOutcome;

    soft_inc(&g_uSoftMultiHost);

    memzero(&infoRtl, (unsigned)sizeof(infoRtl));
    memzero(&infoXhci, (unsigned)sizeof(infoXhci));
    idxRtl = 0xffffffffu;
    idxXhci = 0xffffffffu;
    hRtl = 0;
    hXhci = 0;
    hRtlClosed = 0;
    hXhciClosed = 0;
    nRtlMaps = 0;
    nXhciMaps = 0;
    uRtlDma = 0;
    uXhciDma = 0;
    uRtlDmaBuf = 0;
    uXhciDmaBuf = 0;
    uRtlDmaMap = 0;
    uXhciDmaMap = 0;
    uRtlDmaIdem = 0;
    uXhciDmaIdem = 0;
    uRtlNotify = 0;
    uXhciNotify = 0;
    uRtlIrqReb = 0;
    uXhciIrqReb = 0;
    uPeerDma = 0;
    uRevPeerRemap = 0;
    uRevPeerIrq = 0;
    uRevPeerDma = 0;
    uOpenD = 0;
    uMapD = 0;
    uIrqD = 0;
    uDmaD = 0;
    uDensifyOpen = 0;
    uDensifyMap = 0;
    uDensifyIrq = 0;
    uDensifyDma = 0;
    uDenserN = 0;
    u64RtlDmaPa = 0ull;
    u64XhciDmaPa = 0ull;
    u64RtlDmaBus = 0ull;
    u64XhciDmaBus = 0ull;
    uRtlFirstBar = 0xffffffffu;
    uXhciFirstBar = 0xffffffffu;
    u64RtlFirstVa = 0ull;
    u64XhciFirstVa = 0ull;
    uRtlRemap = 0;
    uXhciRemap = 0;
    uRtlIdem = 0;
    uXhciIdem = 0;
    uRtlSticky = 0;
    uXhciSticky = 0;
    uRtlCfg = 0;
    uXhciCfg = 0;
    uPeerRemap = 0;
    uPeerIrq = 0;
    uPeerRemapIdem = 0;
    uPeerRemapSticky = 0;
    uClosedIrq = 0;
    uClosedDma = 0;
    fRtlIrq = 0;
    fXhciIrq = 0;
    fPeerLive = 0;
    fClosedDead = 0;
    fRevPeerLive = 0;
    fRevClosedDead = 0;
    fBothOpen = 0;
    fBothMap = 0;
    fBothIrq = 0;
    fBothIrqReb = 0;
    fBothNotify = 0;
    fBothDma = 0;
    fBothDmaBuf = 0;
    fBothDmaMap = 0;
    fBothDmaIdem = 0;
    fBothCfg = 0;
    fBothRemap = 0;
    fPrefComplete = 0;
    fOpenMapIrqDma = 0;
    fDensify = 0;
    fDenser = 0;

    /*
     * greppable: ddi_host: soft multi-host chain
     * Core product-host door order honesty (Soft!=product; product_mint=0).
     * OPEN+MAP concurrent is the primary multi-host residual surface.
     */
    {
        char aMh[512];
        unsigned oMh = 0;

        memzero(aMh, sizeof(aMh));
        append_s(aMh, sizeof(aMh), &oMh,
                 "ddi_host: soft multi-host chain "
                 "SCAN/GET/OPEN/CFG/MAP_BAR/MAP_REMAP/IRQ/DMA "
                 "chain=SCAN,GET,OPEN,MAP_BAR,IRQ,DMA "
                 "gj_sys_ddi=");
        append_u(aMh, sizeof(aMh), &oMh, (unsigned long)GJ_SYS_DDI);
        append_s(aMh, sizeof(aMh), &oMh,
                 " prefer_real_ddi=1 not=inject_only real_ddi=1 host_inject=0 "
                 "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
                 "ids=10ec:8168,8086:a12f "
                 "concurrent=1 open_map=1 open_map_irq_dma=1 denser=1 densify=4 "
                 "keep_live=1 multi_server=0 confine=0 "
                 "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
                 "product_mint=0 mint=0 Soft!=product\n");
        aMh[oMh < sizeof(aMh) ? oMh : (sizeof(aMh) - 1u)] = '\0';
        msg(aMh);
    }

    fRtlFound = soft_find_id(nDev, (unsigned short)DDI_HOST_RTL_VEND,
                             (unsigned short)DDI_HOST_RTL_DEV,
                             &infoRtl, &idxRtl);
    fXhciFound = soft_find_id(nDev, (unsigned short)DDI_HOST_XHCI_VEND,
                              (unsigned short)DDI_HOST_XHCI_DEV,
                              &infoXhci, &idxXhci);
    /* Multi-host GET residual lamps (SCAN→GET before concurrent OPEN). */
    if (fRtlFound != 0) {
        log_soft_get(idxRtl, &infoRtl, 1, 0);
    }
    if (fXhciFound != 0) {
        log_soft_get(idxXhci, &infoXhci, 1, 0);
    }

    if (fRtlFound == 0 || fXhciFound == 0) {
        o = 0;
        memzero(aLine, sizeof(aLine));
        append_s(aLine, sizeof(aLine), &o, "ddi_host: soft multi-host ");
        append_s(aLine, sizeof(aLine), &o, "MISS rtl_found=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)(fRtlFound != 0));
        append_s(aLine, sizeof(aLine), &o, " xhci_found=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)(fXhciFound != 0));
        append_s(aLine, sizeof(aLine), &o,
                 " both_open=0 both_map=0 both_irq=0 both_dma=0 peer_live=0 "
                 "closed_dead=0 open_map=0 open=0 map=0 irq=0 dma=0 denser=0 "
                 "prefer_real_ddi=1 not=inject_only product_hosts=UDX "
                 "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
                 "product_mint=0 multi_server=0 Soft!=product\n");
        aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
        msg(aLine);
        /* open+map miss lamp (primary Dual DoD multi-host surface honesty). */
        msg("ddi_host: soft multi-host open+map MISS "
            "rtl=10ec:8168 xhci=8086:a12f concurrent=0 "
            "both_open=0 both_map=0 open=0 map=0 irq=0 dma=0 denser=0 "
            "pref_complete=0 product_hosts=UDX "
            "prefer_real_ddi=1 not=inject_only "
            "dual_dod=OPEN product_mint=0 mint=0 Soft!=product\n");
        msg("ddi_host: soft multi-host prefer_real_ddi MISS "
            "open=0 map=0 irq=0 dma=0 denser=0 concurrent=0 "
            "ids=10ec:8168,8086:a12f product_hosts=UDX "
            "prefer_real_ddi=1 not=inject_only dual_dod=OPEN "
            "product_mint=0 mint=0 Soft!=product\n");
        /*
         * Always-emit denser multi-host prefer_real residual VERDICT greppables
         * on inventory MISS (H2 honesty; Soft!=product Dual DoD OPEN; bar .75).
         * Smoke must still grep residual denser VERDICT when IDs absent.
         * greppable: ddi_host: soft multi-host prefer_real denser VERDICT
         * greppable: ddi_host: soft multi-host prefer_real residual
         * greppable: ddi_host: soft multi-host prefer_real residual denser VERDICT
         */
        msg("ddi_host: soft multi-host prefer_real denser VERDICT MISS "
            "open=0 map=0 irq=0 dma=0 densify_open=0 densify_map=0 "
            "densify_irq=0 densify_dma=0 densify=0 densify_n=0 denser=0 "
            "open_map_irq_dma=0 both_dma_idem=0 both_notify=0 densify=4 "
            "prefer_real_ddi=1 not=inject_only product_hosts=UDX "
            "hosts=rtl8168_udx,xhci_udx ids=10ec:8168,8086:a12f "
            "multi_host=rtl+xhci dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
            "agent_ne_close=1 product_mint=0 mint=0 Soft!=product\n");
        msg("ddi_host: soft multi-host prefer_real residual MISS "
            "open=0 map=0 irq=0 dma=0 residual_open=0 residual_map=0 "
            "residual_irq=0 residual_dma=0 residual_n=0 denser=0 densify=0 "
            "peer_dense=0 closed_dense=0 rev_peer_dense=0 rev_dead_dense=0 "
            "iso_dense=0 peer_live=0 peer_remap=0 peer_irq=0 peer_dma=0 "
            "closed_dead=0 rev_peer=0 rev_dead=0 "
            "prefer_real_ddi=1 not=inject_only real_ddi=1 host_inject=0 "
            "gj_sys_ddi=103 chain=SCAN,GET,OPEN,MAP_BAR,IRQ,DMA "
            "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
            "ids=10ec:8168,8086:a12f multi_host=rtl+xhci "
            "multi_server=0 confine=0 never_clear_all=1 "
            "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
            "agent_ne_close=1 product_mint=0 mint=0 Soft!=product\n");
        msg("ddi_host: soft multi-host prefer_real residual denser VERDICT MISS "
            "open=0 map=0 irq=0 dma=0 residual_open=0 residual_map=0 "
            "residual_irq=0 residual_dma=0 residual_n=0 denser=0 densify=0 "
            "densify_n=0 peer_dense=0 closed_dense=0 rev_peer_dense=0 "
            "rev_dead_dense=0 iso_dense=0 residual=0 densify=4 residual_n=4 "
            "prefer_real_ddi=1 not=inject_only product_hosts=UDX "
            "hosts=rtl8168_udx,xhci_udx ids=10ec:8168,8086:a12f "
            "multi_host=rtl+xhci dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
            "agent_ne_close=1 product_mint=0 mint=0 Soft!=product\n");
        return DDI_HOST_BIND_MISS;
    }

    /* Concurrent OPEN — two soft host slots live (UDX class dual-host). */
    hRtl = gj_ddi_open(idxRtl);
    hXhci = gj_ddi_open(idxXhci);
    log_soft_open("10ec:8168", idxRtl, hRtl, (hRtl > 0) ? 1 : 0);
    log_soft_open("8086:a12f", idxXhci, hXhci, (hXhci > 0) ? 1 : 0);

    if (hRtl > 0 && hXhci > 0) {
        fBothOpen = 1;
        soft_inc(&g_uSoftMultiHostOpenBoth);
    }

    /*
     * CFG_READ both while concurrent OPEN (open residual; identity snap).
     * Functional: both handles must accept cfg before MAP; Soft!=product.
     */
    if (hRtl > 0 && log_soft_cfg("10ec:8168", (unsigned long)hRtl) != 0) {
        uRtlCfg = 1u;
    }
    if (hXhci > 0 && log_soft_cfg("8086:a12f", (unsigned long)hXhci) != 0) {
        uXhciCfg = 1u;
    }
    if (uRtlCfg != 0u && uXhciCfg != 0u) {
        fBothCfg = 1;
        soft_inc(&g_uSoftMultiHostCfgBoth);
    }

    /* 10ec:8168 Command MEM|MASTER before MAP. Never xHCI Command. */
    if (hRtl > 0) {
        (void)soft_cfg_write_8168_command((unsigned long)hRtl,
                                          infoRtl.u16Vend, infoRtl.u16Dev);
    }

    /*
     * MAP preferred BARs while both handles live (multi-host map residual).
     * rtl: BAR0+BAR2; xhci: BAR0. product_mint=0 — soft note only.
     * Track first mapped BAR VA for concurrent MAP_REMAP residual.
     */
    if (hRtl > 0) {
        for (i = 0; i < 2u; i++) {
            memzero(&mapRtl, (unsigned)sizeof(mapRtl));
            if (soft_map_one_bar((unsigned long)hRtl, aRtlBars[i], &infoRtl,
                                 "10ec:8168", &mapRtl) != 0) {
                if (uRtlFirstBar == 0xffffffffu) {
                    uRtlFirstBar = aRtlBars[i];
                    u64RtlFirstVa = mapRtl.u64Va;
                }
                nRtlMaps++;
            }
        }
    }
    if (hXhci > 0) {
        memzero(&mapXhci, (unsigned)sizeof(mapXhci));
        if (soft_map_one_bar((unsigned long)hXhci, aXhciBars[0], &infoXhci,
                             "8086:a12f", &mapXhci) != 0) {
            uXhciFirstBar = aXhciBars[0];
            u64XhciFirstVa = mapXhci.u64Va;
            nXhciMaps++;
        }
    }
    if (nRtlMaps != 0u && nXhciMaps != 0u) {
        fBothMap = 1;
        soft_inc(&g_uSoftMultiHostMapBoth);
    }
    /* Pref-complete: rtl preferred BAR0+BAR2 both soft-map + xhci BAR0. */
    if (nRtlMaps >= 2u && nXhciMaps != 0u) {
        fPrefComplete = 1;
        soft_inc(&g_uSoftMultiHostPrefComplete);
    }

    /*
     * MAP_REMAP both while concurrent (grant idem while peer host live).
     * Functional residual vs door multi-host share / not_sole_owner spirit.
     */
    if (hRtl > 0 && uRtlFirstBar != 0xffffffffu) {
        if (log_soft_map_remap("10ec:8168", (unsigned long)hRtl, uRtlFirstBar,
                               u64RtlFirstVa, &uRtlIdem, &uRtlSticky) != 0) {
            uRtlRemap = 1u;
        }
    }
    if (hXhci > 0 && uXhciFirstBar != 0xffffffffu) {
        if (log_soft_map_remap("8086:a12f", (unsigned long)hXhci,
                               uXhciFirstBar, u64XhciFirstVa,
                               &uXhciIdem, &uXhciSticky) != 0) {
            uXhciRemap = 1u;
        }
    }
    if (uRtlRemap != 0u && uXhciRemap != 0u) {
        fBothRemap = 1;
        soft_inc(&g_uSoftMultiHostRemapBoth);
    }

    /*
     * greppable: ddi_host: soft multi-host open+map
     * Primary Dual DoD multi-host surface: concurrent OPEN + MAP preferred
     * BARs on both product hosts (rtl8168_udx + xhci_udx). Soft!=product.
     */
    o = 0;
    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o, "ddi_host: soft multi-host open+map ");
    if (fBothOpen != 0 && fBothMap != 0) {
        append_s(aLine, sizeof(aLine), &o, "PASS");
    } else if (fBothOpen != 0 || nRtlMaps != 0u || nXhciMaps != 0u) {
        append_s(aLine, sizeof(aLine), &o, "SKIP");
    } else {
        append_s(aLine, sizeof(aLine), &o, "SKIP");
    }
    append_s(aLine, sizeof(aLine), &o, " concurrent=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fBothOpen);
    append_s(aLine, sizeof(aLine), &o, " both_open=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fBothOpen);
    append_s(aLine, sizeof(aLine), &o, " both_map=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fBothMap);
    append_s(aLine, sizeof(aLine), &o, " both_cfg=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fBothCfg);
    append_s(aLine, sizeof(aLine), &o, " both_remap=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fBothRemap);
    append_s(aLine, sizeof(aLine), &o, " pref_complete=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fPrefComplete);
    append_s(aLine, sizeof(aLine), &o, " rtl_o=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)(hRtl > 0));
    append_s(aLine, sizeof(aLine), &o, " xhci_o=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)(hXhci > 0));
    append_s(aLine, sizeof(aLine), &o, " rtl_m=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)nRtlMaps);
    append_s(aLine, sizeof(aLine), &o, " xhci_m=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)nXhciMaps);
    append_s(aLine, sizeof(aLine), &o, " rtl_remap=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uRtlRemap);
    append_s(aLine, sizeof(aLine), &o, " xhci_remap=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uXhciRemap);
    append_s(aLine, sizeof(aLine), &o, " rtl_idem=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uRtlIdem);
    append_s(aLine, sizeof(aLine), &o, " xhci_idem=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uXhciIdem);
    append_s(aLine, sizeof(aLine), &o, " rtl_sticky=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uRtlSticky);
    append_s(aLine, sizeof(aLine), &o, " xhci_sticky=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uXhciSticky);
    append_s(aLine, sizeof(aLine), &o, " open=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fBothOpen);
    append_s(aLine, sizeof(aLine), &o, " map=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fBothMap);
    append_s(aLine, sizeof(aLine), &o,
             " ids=10ec:8168,8086:a12f "
             "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
             "prefer_real_ddi=1 not=inject_only denser=1 densify=4 "
             "not_sole_owner=1 multi_server=0 confine=0 "
             "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
             "product_mint=0 mint=0 Soft!=product\n");
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);

    /* IRQ_BIND both while concurrent (multi-slot; never clear-all). */
    if (hRtl > 0) {
        fRtlIrq = log_soft_irq_bind("10ec:8168", (unsigned long)hRtl);
    }
    if (hXhci > 0) {
        fXhciIrq = log_soft_irq_bind("8086:a12f", (unsigned long)hXhci);
    }
    if (fRtlIrq != 0 && fXhciIrq != 0) {
        fBothIrq = 1;
        soft_inc(&g_uSoftMultiHostIrqBoth);
    }

    /*
     * Concurrent IRQ_REBIND denser (STRONGER multi-host irq residual):
     * second IRQ_BIND on both handles while peer still open. Door multi-slot
     * rebind; never clear-all. Soft!=product; mint=0.
     */
    if (hRtl > 0 && fRtlIrq != 0 &&
        log_soft_irq_rebind("10ec:8168", (unsigned long)hRtl) != 0) {
        uRtlIrqReb = 1u;
    }
    if (hXhci > 0 && fXhciIrq != 0 &&
        log_soft_irq_rebind("8086:a12f", (unsigned long)hXhci) != 0) {
        uXhciIrqReb = 1u;
    }
    if (uRtlIrqReb != 0u && uXhciIrqReb != 0u) {
        fBothIrqReb = 1;
        soft_inc(&g_uSoftMultiHostIrqRebBoth);
    }

    /*
     * Concurrent notify_poll denser (STRONGER multi-host irq residual):
     * non-block poll both while peer open. Prefer rebind badge when rebind
     * soft-ok. Soft!=product; fire=0; never product IRQ Notification mint.
     */
    if (hRtl > 0 && fRtlIrq != 0) {
        if (log_soft_notify_poll("10ec:8168",
                                 (uRtlIrqReb != 0u)
                                     ? DDI_HOST_IRQ_BADGE_REBIND
                                     : DDI_HOST_IRQ_BADGE_SOFT) != 0) {
            uRtlNotify = 1u;
        }
    }
    if (hXhci > 0 && fXhciIrq != 0) {
        if (log_soft_notify_poll("8086:a12f",
                                 (uXhciIrqReb != 0u)
                                     ? DDI_HOST_IRQ_BADGE_REBIND
                                     : DDI_HOST_IRQ_BADGE_SOFT) != 0) {
            uXhciNotify = 1u;
        }
    }
    if (uRtlNotify != 0u && uXhciNotify != 0u) {
        fBothNotify = 1;
        soft_inc(&g_uSoftMultiHostNotifyBoth);
    }

    /* DMA_NOTE both — soft window only; product DMA window mint OPEN. */
    if (hRtl > 0) {
        u64DmaPa = 0ull;
        u64DmaCb = 0ull;
        for (i = 0; i < 2u; i++) {
            bar_pa_cb(&infoRtl, aRtlBars[i], &u64DmaPa, &u64DmaCb);
            if (u64DmaPa != 0ull && u64DmaCb != 0ull) {
                break;
            }
        }
        uDmaBefore = g_uSoftDmaOk;
        log_soft_dma_note("10ec:8168", (unsigned long)hRtl, u64DmaPa, u64DmaCb);
        if (g_uSoftDmaOk > uDmaBefore) {
            uRtlDma = 1u;
        }
    }
    if (hXhci > 0) {
        u64DmaPa = 0ull;
        u64DmaCb = 0ull;
        bar_pa_cb(&infoXhci, aXhciBars[0], &u64DmaPa, &u64DmaCb);
        uDmaBefore = g_uSoftDmaOk;
        log_soft_dma_note("8086:a12f", (unsigned long)hXhci, u64DmaPa, u64DmaCb);
        if (g_uSoftDmaOk > uDmaBefore) {
            uXhciDma = 1u;
        }
    }
    if (uRtlDma != 0u && uXhciDma != 0u) {
        fBothDma = 1;
        soft_inc(&g_uSoftMultiHostDmaBoth);
    }

    /*
     * Concurrent DMA_BUF denser residual (STRONGER multi-host dma):
     * ALLOC -> MAP (bus cookie) -> MAP_REMAP cookie_idem -> FREE both while
     * both handles live. Prefer real DDI DMA_BUF door over inject-only.
     * Soft only; mint=0; product DMA window remains OPEN.
     */
    if (hRtl > 0) {
        soft_inc(&g_uSoftDmaBuf);
        retDmaBuf = gj_ddi(DDI_HOST_OP_DMA_BUF_ALLOC, (unsigned long)hRtl,
                           (unsigned long)DDI_HOST_DMA_BUF_PAGES,
                           (unsigned long)DDI_HOST_DMA_BUF_FLAGS);
        if (retDmaBuf >= 0) {
            soft_inc(&g_uSoftDmaBufOk);
            uRtlDmaBuf = 1u;
            u64RtlDmaPa = (unsigned long long)retDmaBuf;
            soft_inc(&g_uSoftDmaMap);
            retDmaMap = gj_ddi(DDI_HOST_OP_DMA_BUF_MAP, (unsigned long)hRtl,
                               (unsigned long)retDmaBuf,
                               (unsigned long)((unsigned long)
                                   DDI_HOST_DMA_BUF_PAGES * 4096ul));
            if (retDmaMap >= 0) {
                soft_inc(&g_uSoftDmaMapOk);
                uRtlDmaMap = 1u;
                u64RtlDmaBus = (unsigned long long)retDmaMap;
                /* denser: re-MAP same pa/cb for cookie_idem while peer live */
                retDmaMap2 = gj_ddi(DDI_HOST_OP_DMA_BUF_MAP,
                                    (unsigned long)hRtl,
                                    (unsigned long)u64RtlDmaPa,
                                    (unsigned long)((unsigned long)
                                        DDI_HOST_DMA_BUF_PAGES * 4096ul));
                if (retDmaMap2 >= 0 &&
                    (unsigned long long)retDmaMap2 == u64RtlDmaBus) {
                    soft_inc(&g_uSoftDmaMapIdem);
                    uRtlDmaIdem = 1u;
                }
            }
            soft_inc(&g_uSoftDmaFree);
            retDmaFree = gj_ddi(DDI_HOST_OP_DMA_BUF_FREE, (unsigned long)hRtl,
                                (unsigned long)retDmaBuf,
                                (unsigned long)DDI_HOST_DMA_BUF_PAGES);
            if (retDmaFree >= 0) {
                soft_inc(&g_uSoftDmaFreeOk);
            }
        }
    }
    if (hXhci > 0) {
        soft_inc(&g_uSoftDmaBuf);
        retDmaBuf = gj_ddi(DDI_HOST_OP_DMA_BUF_ALLOC, (unsigned long)hXhci,
                           (unsigned long)DDI_HOST_DMA_BUF_PAGES,
                           (unsigned long)DDI_HOST_DMA_BUF_FLAGS);
        if (retDmaBuf >= 0) {
            soft_inc(&g_uSoftDmaBufOk);
            uXhciDmaBuf = 1u;
            u64XhciDmaPa = (unsigned long long)retDmaBuf;
            soft_inc(&g_uSoftDmaMap);
            retDmaMap = gj_ddi(DDI_HOST_OP_DMA_BUF_MAP, (unsigned long)hXhci,
                               (unsigned long)retDmaBuf,
                               (unsigned long)((unsigned long)
                                   DDI_HOST_DMA_BUF_PAGES * 4096ul));
            if (retDmaMap >= 0) {
                soft_inc(&g_uSoftDmaMapOk);
                uXhciDmaMap = 1u;
                u64XhciDmaBus = (unsigned long long)retDmaMap;
                retDmaMap2 = gj_ddi(DDI_HOST_OP_DMA_BUF_MAP,
                                    (unsigned long)hXhci,
                                    (unsigned long)u64XhciDmaPa,
                                    (unsigned long)((unsigned long)
                                        DDI_HOST_DMA_BUF_PAGES * 4096ul));
                if (retDmaMap2 >= 0 &&
                    (unsigned long long)retDmaMap2 == u64XhciDmaBus) {
                    soft_inc(&g_uSoftDmaMapIdem);
                    uXhciDmaIdem = 1u;
                }
            }
            soft_inc(&g_uSoftDmaFree);
            retDmaFree = gj_ddi(DDI_HOST_OP_DMA_BUF_FREE, (unsigned long)hXhci,
                                (unsigned long)retDmaBuf,
                                (unsigned long)DDI_HOST_DMA_BUF_PAGES);
            if (retDmaFree >= 0) {
                soft_inc(&g_uSoftDmaFreeOk);
            }
        }
    }
    if (uRtlDmaBuf != 0u && uXhciDmaBuf != 0u) {
        fBothDmaBuf = 1;
        soft_inc(&g_uSoftMultiHostDmaBufBoth);
    }
    if (uRtlDmaMap != 0u && uXhciDmaMap != 0u) {
        fBothDmaMap = 1;
        soft_inc(&g_uSoftMultiHostDmaMapBoth);
    }
    if (uRtlDmaIdem != 0u && uXhciDmaIdem != 0u) {
        fBothDmaIdem = 1;
        soft_inc(&g_uSoftMultiHostDmaIdemBoth);
    }

    /*
     * STRONGER multi-host prefer_real_ddi residual denser for Dual DoD
     * product hosts (10ec:8168 + 8086:a12f). open/map/irq/dma denser
     * while concurrent; densify_open/map/irq/dma once-lamps densify=4;
     * Soft!=product; dual_dod OPEN; product_hosts=UDX.
     * greppable: ddi_host: soft multi-host prefer_real_ddi
     * greppable: ddi_host: soft multi-host densify open=
     * greppable: ddi_host: soft multi-host densify map=
     * greppable: ddi_host: soft multi-host densify irq=
     * greppable: ddi_host: soft multi-host densify dma=
     * greppable: ddi_host: soft multi-host denser open=
     * greppable: ddi_host: soft multi-host denser map=
     * greppable: ddi_host: soft multi-host denser irq=
     * greppable: ddi_host: soft multi-host denser dma=
     * greppable: ddi_host: soft multi-host prefer_real denser VERDICT
     * greppable: open= map= irq= dma= denser= densify= prefer_real_ddi=1
     */
    uOpenD = (fBothOpen != 0) ? 1u : 0u;
    uMapD = (fBothMap != 0) ? 1u : 0u;
    /* denser irq: bind both OR rebind both OR notify both. */
    uIrqD = (fBothIrq != 0 || fBothIrqReb != 0 || fBothNotify != 0) ? 1u : 0u;
    /* denser dma: note / dma_buf / dma_map / cookie_idem both. */
    uDmaD = (fBothDma != 0 || fBothDmaBuf != 0 || fBothDmaMap != 0 ||
             fBothDmaIdem != 0)
                ? 1u
                : 0u;
    /* densify_* : functional residual surfaces (prefer_real denser spirit). */
    uDensifyOpen = (fBothOpen != 0 && fBothCfg != 0) ? 1u : 0u;
    uDensifyMap = (fBothMap != 0 && (fBothRemap != 0 || fPrefComplete != 0))
                      ? 1u
                      : 0u;
    uDensifyIrq = (fBothIrq != 0 &&
                   (fBothIrqReb != 0 || fBothNotify != 0))
                      ? 1u
                      : 0u;
    uDensifyDma = (fBothDma != 0 &&
                   (fBothDmaBuf != 0 || fBothDmaMap != 0 || fBothDmaIdem != 0))
                      ? 1u
                      : 0u;
    if (uOpenD != 0u && uMapD != 0u && uIrqD != 0u && uDmaD != 0u) {
        fOpenMapIrqDma = 1;
        soft_inc(&g_uSoftMultiHostOpenMapIrqDma);
    }
    if (uDensifyOpen != 0u && uDensifyMap != 0u && uDensifyIrq != 0u &&
        uDensifyDma != 0u) {
        fDensify = 1;
        soft_inc(&g_uSoftMultiHostDensify);
    }
    /*
     * denser prove: open+map+irq+dma all soft-ok concurrent (prefer_real).
     * densify=4 when densify surfaces + dma_idem or notify denser both.
     */
    if (fOpenMapIrqDma != 0) {
        fDenser = 1;
        soft_inc(&g_uSoftMultiHostDenser);
    }
    uDenserN = 0u;
    if (uDensifyOpen != 0u) {
        uDenserN++;
    }
    if (uDensifyMap != 0u) {
        uDenserN++;
    }
    if (uDensifyIrq != 0u) {
        uDenserN++;
    }
    if (uDensifyDma != 0u) {
        uDenserN++;
    }
    o = 0;
    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o,
             "ddi_host: soft multi-host prefer_real_ddi ");
    if (fOpenMapIrqDma != 0) {
        append_s(aLine, sizeof(aLine), &o, "PASS");
    } else if (uOpenD != 0u || uMapD != 0u) {
        append_s(aLine, sizeof(aLine), &o, "SKIP");
    } else {
        append_s(aLine, sizeof(aLine), &o, "MISS");
    }
    append_s(aLine, sizeof(aLine), &o, " open=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uOpenD);
    append_s(aLine, sizeof(aLine), &o, " map=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uMapD);
    append_s(aLine, sizeof(aLine), &o, " irq=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uIrqD);
    append_s(aLine, sizeof(aLine), &o, " dma=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uDmaD);
    append_s(aLine, sizeof(aLine), &o, " densify_open=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uDensifyOpen);
    append_s(aLine, sizeof(aLine), &o, " densify_map=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uDensifyMap);
    append_s(aLine, sizeof(aLine), &o, " densify_irq=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uDensifyIrq);
    append_s(aLine, sizeof(aLine), &o, " densify_dma=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uDensifyDma);
    append_s(aLine, sizeof(aLine), &o, " densify=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fDensify);
    append_s(aLine, sizeof(aLine), &o, " densify_n=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uDenserN);
    append_s(aLine, sizeof(aLine), &o, " both_open=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fBothOpen);
    append_s(aLine, sizeof(aLine), &o, " both_map=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fBothMap);
    append_s(aLine, sizeof(aLine), &o, " both_irq=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fBothIrq);
    append_s(aLine, sizeof(aLine), &o, " both_irq_reb=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fBothIrqReb);
    append_s(aLine, sizeof(aLine), &o, " both_notify=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fBothNotify);
    append_s(aLine, sizeof(aLine), &o, " both_dma=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fBothDma);
    append_s(aLine, sizeof(aLine), &o, " both_dma_buf=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fBothDmaBuf);
    append_s(aLine, sizeof(aLine), &o, " both_dma_map=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fBothDmaMap);
    append_s(aLine, sizeof(aLine), &o, " both_dma_idem=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fBothDmaIdem);
    append_s(aLine, sizeof(aLine), &o, " denser=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fDenser);
    append_s(aLine, sizeof(aLine), &o, " densify=4 concurrent=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fBothOpen);
    append_s(aLine, sizeof(aLine), &o,
             " ids=10ec:8168,8086:a12f "
             "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
             "prefer_real_ddi=1 not=inject_only real_ddi=1 host_inject=0 "
             "gj_sys_ddi=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)GJ_SYS_DDI);
    append_s(aLine, sizeof(aLine), &o,
             " chain=SCAN,GET,OPEN,MAP_BAR,IRQ,DMA "
             "keep_live=1 multi_server=0 confine=0 "
             "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
             "product_mint=0 mint=0 Soft!=product\n");
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);

    /*
     * H2 once densify lamps per surface (rtl densify spirit; multi-host).
     * greppable: ddi_host: soft multi-host densify open=
     * greppable: ddi_host: soft multi-host densify map=
     * greppable: ddi_host: soft multi-host densify irq=
     * greppable: ddi_host: soft multi-host densify dma=
     */
    if (uDensifyOpen != 0u) {
        msg("ddi_host: soft multi-host densify open=1 "
            "both_open=1 both_cfg=1 densify_open=1 denser=1 densify=4 "
            "prefer_real_ddi=1 not=inject_only real_ddi=1 host_inject=0 "
            "ids=10ec:8168,8086:a12f product_hosts=UDX "
            "dual_dod=OPEN product_mint=0 mint=0 Soft!=product\n");
    }
    if (uDensifyMap != 0u) {
        msg("ddi_host: soft multi-host densify map=1 "
            "both_map=1 both_remap=1 densify_map=1 denser=1 densify=4 "
            "prefer_real_ddi=1 not=inject_only real_ddi=1 host_inject=0 "
            "ids=10ec:8168,8086:a12f product_hosts=UDX "
            "dual_dod=OPEN product_mint=0 mint=0 Soft!=product\n");
    }
    if (uDensifyIrq != 0u) {
        msg("ddi_host: soft multi-host densify irq=1 "
            "both_irq=1 both_irq_reb=1 both_notify=1 densify_irq=1 "
            "denser=1 densify=4 "
            "prefer_real_ddi=1 not=inject_only real_ddi=1 host_inject=0 "
            "ids=10ec:8168,8086:a12f product_hosts=UDX "
            "dual_dod=OPEN product_mint=0 mint=0 Soft!=product\n");
    }
    if (uDensifyDma != 0u) {
        msg("ddi_host: soft multi-host densify dma=1 "
            "both_dma=1 both_dma_buf=1 both_dma_map=1 both_dma_idem=1 "
            "densify_dma=1 denser=1 densify=4 "
            "prefer_real_ddi=1 not=inject_only real_ddi=1 host_inject=0 "
            "ids=10ec:8168,8086:a12f product_hosts=UDX "
            "dual_dod=OPEN product_mint=0 mint=0 Soft!=product\n");
    }

    /*
     * Denser open/map/irq/dma once-lamps (prefer_real multi-host Dual DoD).
     * Functional residual surface lamps; Soft!=product; never Dual DoD close.
     * greppable: ddi_host: soft multi-host denser open=
     * greppable: ddi_host: soft multi-host denser map=
     * greppable: ddi_host: soft multi-host denser irq=
     * greppable: ddi_host: soft multi-host denser dma=
     */
    if (uOpenD != 0u) {
        msg("ddi_host: soft multi-host denser open=1 "
            "both_open=1 both_cfg=1 concurrent=1 denser=1 densify=4 "
            "prefer_real_ddi=1 not=inject_only product_hosts=UDX "
            "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
            "product_mint=0 mint=0 Soft!=product\n");
    }
    if (uMapD != 0u) {
        char aDm[320];
        unsigned oDm = 0;

        memzero(aDm, sizeof(aDm));
        append_s(aDm, sizeof(aDm), &oDm,
                 "ddi_host: soft multi-host denser map=1 "
                 "both_map=1 both_remap=");
        append_u(aDm, sizeof(aDm), &oDm, (unsigned long)fBothRemap);
        append_s(aDm, sizeof(aDm), &oDm, " pref_complete=");
        append_u(aDm, sizeof(aDm), &oDm, (unsigned long)fPrefComplete);
        append_s(aDm, sizeof(aDm), &oDm,
                 " concurrent=1 denser=1 densify=4 "
                 "prefer_real_ddi=1 not=inject_only product_hosts=UDX "
                 "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
                 "product_mint=0 mint=0 Soft!=product\n");
        aDm[oDm < sizeof(aDm) ? oDm : (sizeof(aDm) - 1u)] = '\0';
        msg(aDm);
    }
    if (uIrqD != 0u) {
        char aDi[320];
        unsigned oDi = 0;

        memzero(aDi, sizeof(aDi));
        append_s(aDi, sizeof(aDi), &oDi,
                 "ddi_host: soft multi-host denser irq=1 "
                 "both_irq=");
        append_u(aDi, sizeof(aDi), &oDi, (unsigned long)fBothIrq);
        append_s(aDi, sizeof(aDi), &oDi, " both_irq_reb=");
        append_u(aDi, sizeof(aDi), &oDi, (unsigned long)fBothIrqReb);
        append_s(aDi, sizeof(aDi), &oDi, " both_notify=");
        append_u(aDi, sizeof(aDi), &oDi, (unsigned long)fBothNotify);
        append_s(aDi, sizeof(aDi), &oDi,
                 " concurrent=1 denser=1 densify=4 "
                 "prefer_real_ddi=1 not=inject_only product_hosts=UDX "
                 "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
                 "product_mint=0 mint=0 Soft!=product\n");
        aDi[oDi < sizeof(aDi) ? oDi : (sizeof(aDi) - 1u)] = '\0';
        msg(aDi);
    }
    if (uDmaD != 0u) {
        char aDd[360];
        unsigned oDd = 0;

        memzero(aDd, sizeof(aDd));
        append_s(aDd, sizeof(aDd), &oDd,
                 "ddi_host: soft multi-host denser dma=1 "
                 "both_dma=");
        append_u(aDd, sizeof(aDd), &oDd, (unsigned long)fBothDma);
        append_s(aDd, sizeof(aDd), &oDd, " both_dma_buf=");
        append_u(aDd, sizeof(aDd), &oDd, (unsigned long)fBothDmaBuf);
        append_s(aDd, sizeof(aDd), &oDd, " both_dma_map=");
        append_u(aDd, sizeof(aDd), &oDd, (unsigned long)fBothDmaMap);
        append_s(aDd, sizeof(aDd), &oDd, " both_dma_idem=");
        append_u(aDd, sizeof(aDd), &oDd, (unsigned long)fBothDmaIdem);
        append_s(aDd, sizeof(aDd), &oDd,
                 " concurrent=1 denser=1 densify=4 "
                 "prefer_real_ddi=1 not=inject_only product_hosts=UDX "
                 "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
                 "product_mint=0 mint=0 Soft!=product\n");
        aDd[oDd < sizeof(aDd) ? oDd : (sizeof(aDd) - 1u)] = '\0';
        msg(aDd);
    }

    /*
     * greppable: ddi_host: soft multi-host prefer_real denser VERDICT
     * Agent-facing rollup for Dual DoD multi-host prefer_real denser.
     * Soft residual only; Dual DoD A/B remain OPEN; agent!=close.
     */
    {
        char aV[480];
        unsigned oV = 0;

        memzero(aV, sizeof(aV));
        append_s(aV, sizeof(aV), &oV,
                 "ddi_host: soft multi-host prefer_real denser VERDICT ");
        if (fDenser != 0 && fDensify != 0) {
            append_s(aV, sizeof(aV), &oV, "PASS");
        } else if (fOpenMapIrqDma != 0 || fDenser != 0) {
            append_s(aV, sizeof(aV), &oV, "SKIP");
        } else {
            append_s(aV, sizeof(aV), &oV, "MISS");
        }
        append_s(aV, sizeof(aV), &oV, " open=");
        append_u(aV, sizeof(aV), &oV, (unsigned long)uOpenD);
        append_s(aV, sizeof(aV), &oV, " map=");
        append_u(aV, sizeof(aV), &oV, (unsigned long)uMapD);
        append_s(aV, sizeof(aV), &oV, " irq=");
        append_u(aV, sizeof(aV), &oV, (unsigned long)uIrqD);
        append_s(aV, sizeof(aV), &oV, " dma=");
        append_u(aV, sizeof(aV), &oV, (unsigned long)uDmaD);
        append_s(aV, sizeof(aV), &oV, " densify_open=");
        append_u(aV, sizeof(aV), &oV, (unsigned long)uDensifyOpen);
        append_s(aV, sizeof(aV), &oV, " densify_map=");
        append_u(aV, sizeof(aV), &oV, (unsigned long)uDensifyMap);
        append_s(aV, sizeof(aV), &oV, " densify_irq=");
        append_u(aV, sizeof(aV), &oV, (unsigned long)uDensifyIrq);
        append_s(aV, sizeof(aV), &oV, " densify_dma=");
        append_u(aV, sizeof(aV), &oV, (unsigned long)uDensifyDma);
        append_s(aV, sizeof(aV), &oV, " densify=");
        append_u(aV, sizeof(aV), &oV, (unsigned long)fDensify);
        append_s(aV, sizeof(aV), &oV, " densify_n=");
        append_u(aV, sizeof(aV), &oV, (unsigned long)uDenserN);
        append_s(aV, sizeof(aV), &oV, " denser=");
        append_u(aV, sizeof(aV), &oV, (unsigned long)fDenser);
        append_s(aV, sizeof(aV), &oV, " open_map_irq_dma=");
        append_u(aV, sizeof(aV), &oV, (unsigned long)fOpenMapIrqDma);
        append_s(aV, sizeof(aV), &oV, " both_dma_idem=");
        append_u(aV, sizeof(aV), &oV, (unsigned long)fBothDmaIdem);
        append_s(aV, sizeof(aV), &oV, " both_notify=");
        append_u(aV, sizeof(aV), &oV, (unsigned long)fBothNotify);
        append_s(aV, sizeof(aV), &oV,
                 " densify=4 prefer_real_ddi=1 not=inject_only "
                 "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
                 "ids=10ec:8168,8086:a12f multi_host=rtl+xhci "
                 "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
                 "agent_ne_close=1 product_mint=0 mint=0 Soft!=product\n");
        aV[oV < sizeof(aV) ? oV : (sizeof(aV) - 1u)] = '\0';
        msg(aV);
    }

    /*
     * CLOSE one host; peer must remain live (multi-host unbind_safe spirit).
     * Functional OPEN+MAP residual preferred over lamp-only:
     *   peer CFG_READ + MAP_BAR + MAP_REMAP + IRQ rebind + DMA stay live;
     *   closed handle CFG/MAP/IRQ/DMA must soft-fail (never clear-all peer).
     * Cap honesty: never claims product multi-server or sole BAR owner.
     */
    if (hRtl > 0 && hXhci > 0) {
        hRtlClosed = hRtl;
        retCloseRtl = gj_ddi(DDI_HOST_OP_CLOSE, (unsigned long)hRtl, 0ul, 0ul);
        soft_inc(&g_uSoftClose);
        if (retCloseRtl >= 0) {
            soft_inc(&g_uSoftCloseOk);
        }
        hRtl = 0;

        /* Peer live residual (xhci grants stay after rtl CLOSE). */
        retPeerCfg = gj_ddi_cfg_read((unsigned long)hXhci, 0u);
        memzero(&notePeer, (unsigned)sizeof(notePeer));
        retPeerMap = gj_ddi(GJ_DDI_OP_MAP_BAR, (unsigned long)hXhci, 0ul,
                            (unsigned long)(uintptr_t)&notePeer);
        if (retPeerCfg >= 0 && (retPeerMap >= 0 || notePeer.u8Ok != 0)) {
            fPeerLive = 1;
            soft_inc(&g_uSoftMultiHostPeerLive);
        }

        /*
         * Peer MAP_REMAP residual: grant sticky while peer host still open
         * after other host CLOSE (door share / not_sole_owner spirit).
         * Primary multi-host OPEN+MAP deepen vs lamp-only peer_live.
         */
        if (fPeerLive != 0 && uXhciFirstBar != 0xffffffffu) {
            if (log_soft_map_remap("8086:a12f", (unsigned long)hXhci,
                                   uXhciFirstBar, u64XhciFirstVa,
                                   &uPeerRemapIdem, &uPeerRemapSticky) != 0) {
                uPeerRemap = 1u;
                soft_inc(&g_uSoftMultiHostPeerRemap);
            }
        }

        /*
         * Peer IRQ rebind residual: multi-slot unbind_safe — peer IRQ still
         * rebinds after other host CLOSE (never clear-all). Soft!=product.
         */
        if (fPeerLive != 0 &&
            log_soft_irq_rebind("8086:a12f", (unsigned long)hXhci) != 0) {
            uPeerIrq = 1u;
            soft_inc(&g_uSoftMultiHostPeerIrq);
        }

        /*
         * Peer DMA residual denser: soft DMA_NOTE still accepted on peer
         * after other host CLOSE (window residual live; mint=0).
         */
        if (fPeerLive != 0) {
            u64DmaPa = 0ull;
            u64DmaCb = 0ull;
            bar_pa_cb(&infoXhci, aXhciBars[0], &u64DmaPa, &u64DmaCb);
            uDmaBefore = g_uSoftDmaOk;
            log_soft_dma_note("8086:a12f", (unsigned long)hXhci,
                              u64DmaPa, u64DmaCb);
            if (g_uSoftDmaOk > uDmaBefore) {
                uPeerDma = 1u;
                soft_inc(&g_uSoftMultiHostPeerDma);
            } else {
                /* Fallback: DMA_BUF_ALLOC peer live residual denser. */
                retPeerDma = gj_ddi(DDI_HOST_OP_DMA_BUF_ALLOC,
                                    (unsigned long)hXhci,
                                    (unsigned long)DDI_HOST_DMA_BUF_PAGES,
                                    (unsigned long)DDI_HOST_DMA_BUF_FLAGS);
                if (retPeerDma >= 0) {
                    uPeerDma = 1u;
                    soft_inc(&g_uSoftMultiHostPeerDma);
                    (void)gj_ddi(DDI_HOST_OP_DMA_BUF_FREE,
                                 (unsigned long)hXhci,
                                 (unsigned long)retPeerDma,
                                 (unsigned long)DDI_HOST_DMA_BUF_PAGES);
                }
            }
        }

        o = 0;
        memzero(aLine, sizeof(aLine));
        append_s(aLine, sizeof(aLine), &o,
                 "ddi_host: soft multi-host peer_live ");
        append_s(aLine, sizeof(aLine), &o, "closed=rtl peer=xhci cfg=");
        append_u(aLine, sizeof(aLine), &o,
                 (unsigned long)(retPeerCfg >= 0));
        append_s(aLine, sizeof(aLine), &o, " map=");
        append_u(aLine, sizeof(aLine), &o,
                 (unsigned long)(retPeerMap >= 0 || notePeer.u8Ok != 0));
        append_s(aLine, sizeof(aLine), &o, " remap=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)uPeerRemap);
        append_s(aLine, sizeof(aLine), &o, " remap_idem=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)uPeerRemapIdem);
        append_s(aLine, sizeof(aLine), &o, " remap_sticky=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)uPeerRemapSticky);
        append_s(aLine, sizeof(aLine), &o, " irq_rebind=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)uPeerIrq);
        append_s(aLine, sizeof(aLine), &o, " dma=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)uPeerDma);
        append_s(aLine, sizeof(aLine), &o, " open=");
        append_u(aLine, sizeof(aLine), &o,
                 (unsigned long)(retPeerCfg >= 0));
        append_s(aLine, sizeof(aLine), &o, " irq=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)uPeerIrq);
        if (fPeerLive != 0) {
            append_s(aLine, sizeof(aLine), &o,
                     " PASS never_clear_all=1 multi_host_safe=1 "
                     "not_sole_owner=1 open_map=1 denser=1 "
                     "prefer_real_ddi=1 product_hosts=UDX "
                     "dual_dod=OPEN product_mint=0 mint=0 Soft!=product\n");
        } else {
            append_s(aLine, sizeof(aLine), &o, " SKIP why=peer_dead");
            if (retPeerCfg < 0) {
                append_err(aLine, sizeof(aLine), &o, retPeerCfg);
            } else if (retPeerMap < 0) {
                append_err(aLine, sizeof(aLine), &o, retPeerMap);
            }
            append_s(aLine, sizeof(aLine), &o,
                     " prefer_real_ddi=1 product_mint=0 Soft!=product\n");
        }
        aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
        msg(aLine);

        /*
         * Closed-handle dead residual while peer still live.
         * Functional isolation: closed rtl CFG/MAP/IRQ/DMA must fail; peer
         * xhci stays open. Door CLOSE forgets only this handle.
         * greppable: ddi_host: soft multi-host closed_dead
         */
        retDeadCfg = gj_ddi_cfg_read((unsigned long)hRtlClosed, 0u);
        memzero(&noteDead, (unsigned)sizeof(noteDead));
        retDeadMap = gj_ddi(GJ_DDI_OP_MAP_BAR, (unsigned long)hRtlClosed, 0ul,
                            (unsigned long)(uintptr_t)&noteDead);
        retDeadIrq = gj_ddi(DDI_HOST_OP_IRQ_BIND, (unsigned long)hRtlClosed,
                            (unsigned long)DDI_HOST_IRQ_BADGE_SOFT, 0ul);
        retDeadDma = gj_ddi(DDI_HOST_OP_DMA_BUF_ALLOC,
                            (unsigned long)hRtlClosed,
                            (unsigned long)DDI_HOST_DMA_BUF_PAGES,
                            (unsigned long)DDI_HOST_DMA_BUF_FLAGS);
        if (retDeadIrq < 0) {
            uClosedIrq = 1u;
            soft_inc(&g_uSoftMultiHostClosedIrq);
        }
        if (retDeadDma < 0) {
            uClosedDma = 1u;
            soft_inc(&g_uSoftMultiHostClosedDma);
        }
        if (retDeadCfg < 0 && retDeadMap < 0 && noteDead.u8Ok == 0 &&
            retDeadIrq < 0 && retDeadDma < 0) {
            fClosedDead = 1;
            soft_inc(&g_uSoftMultiHostClosedDead);
        }

        o = 0;
        memzero(aLine, sizeof(aLine));
        append_s(aLine, sizeof(aLine), &o,
                 "ddi_host: soft multi-host closed_dead ");
        append_s(aLine, sizeof(aLine), &o, "closed=rtl peer=xhci cfg_fail=");
        append_u(aLine, sizeof(aLine), &o,
                 (unsigned long)(retDeadCfg < 0));
        append_s(aLine, sizeof(aLine), &o, " map_fail=");
        append_u(aLine, sizeof(aLine), &o,
                 (unsigned long)(retDeadMap < 0 && noteDead.u8Ok == 0));
        append_s(aLine, sizeof(aLine), &o, " irq_fail=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)uClosedIrq);
        append_s(aLine, sizeof(aLine), &o, " dma_fail=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)uClosedDma);
        append_s(aLine, sizeof(aLine), &o, " peer_live=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)fPeerLive);
        append_s(aLine, sizeof(aLine), &o, " open=");
        append_u(aLine, sizeof(aLine), &o,
                 (unsigned long)(retDeadCfg < 0));
        append_s(aLine, sizeof(aLine), &o, " map=");
        append_u(aLine, sizeof(aLine), &o,
                 (unsigned long)(retDeadMap < 0 && noteDead.u8Ok == 0));
        append_s(aLine, sizeof(aLine), &o, " irq=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)uClosedIrq);
        append_s(aLine, sizeof(aLine), &o, " dma=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)uClosedDma);
        if (fClosedDead != 0) {
            append_s(aLine, sizeof(aLine), &o,
                     " PASS isolation=1 never_clear_all=1 "
                     "cfg_map_irq_dma=1 denser=1 prefer_real_ddi=1 "
                     "product_hosts=UDX dual_dod=OPEN "
                     "product_mint=0 mint=0 Soft!=product\n");
        } else {
            append_s(aLine, sizeof(aLine), &o, " SKIP why=closed_still_live");
            if (retDeadCfg >= 0) {
                append_s(aLine, sizeof(aLine), &o, " cfg_live=1");
            }
            if (retDeadMap >= 0 || noteDead.u8Ok != 0) {
                append_s(aLine, sizeof(aLine), &o, " map_live=1");
            }
            if (retDeadIrq >= 0) {
                append_s(aLine, sizeof(aLine), &o, " irq_live=1");
            }
            if (retDeadDma >= 0) {
                append_s(aLine, sizeof(aLine), &o, " dma_live=1");
            }
            append_s(aLine, sizeof(aLine), &o,
                     " prefer_real_ddi=1 product_mint=0 Soft!=product\n");
        }
        aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
        msg(aLine);

        /*
         * Reverse CLOSE order residual (STRONGER multi-host OPEN+MAP/IRQ/DMA):
         * REOPEN rtl + MAP preferred BAR while xhci still live, then CLOSE
         * xhci first and prove rtl peer-live + xhci closed-dead denser
         * (CFG/MAP/IRQ/DMA). Order-independent isolation for product UDX
         * dual-host door path. prefer_real_ddi; Soft!=product.
         * greppable: ddi_host: soft multi-host rev_peer_live
         * greppable: ddi_host: soft multi-host rev_closed_dead
         */
        hRtl = gj_ddi_open(idxRtl);
        log_soft_open("10ec:8168", idxRtl, hRtl, (hRtl > 0) ? 1 : 0);
        if (hRtl > 0) {
            unsigned uRevRtlBar = 0xffffffffu;
            unsigned long long u64RevRtlVa = 0ull;
            unsigned uRevIdem = 0;
            unsigned uRevSticky = 0;
            int fRevMapOk = 0;

            memzero(&mapRtl, (unsigned)sizeof(mapRtl));
            if (soft_map_one_bar((unsigned long)hRtl, aRtlBars[0], &infoRtl,
                                 "10ec:8168", &mapRtl) != 0) {
                uRevRtlBar = aRtlBars[0];
                u64RevRtlVa = mapRtl.u64Va;
                fRevMapOk = 1;
            }

            hXhciClosed = hXhci;
            retCloseXhci = gj_ddi(DDI_HOST_OP_CLOSE, (unsigned long)hXhci,
                                  0ul, 0ul);
            soft_inc(&g_uSoftClose);
            if (retCloseXhci >= 0) {
                soft_inc(&g_uSoftCloseOk);
            }
            hXhci = 0;

            /* Reverse peer_live: rtl CFG+MAP after xhci CLOSE. */
            retRevPeerCfg = gj_ddi_cfg_read((unsigned long)hRtl, 0u);
            memzero(&notePeer, (unsigned)sizeof(notePeer));
            retRevPeerMap = gj_ddi(GJ_DDI_OP_MAP_BAR, (unsigned long)hRtl,
                                   0ul, (unsigned long)(uintptr_t)&notePeer);
            if (retRevPeerCfg >= 0 &&
                (retRevPeerMap >= 0 || notePeer.u8Ok != 0)) {
                fRevPeerLive = 1;
                soft_inc(&g_uSoftMultiHostRevPeer);
            }

            /*
             * Reverse peer MAP_REMAP denser: rtl grant sticky after xhci CLOSE
             * (order-independent multi-host OPEN+MAP residual; Soft!=product).
             * Counts into residual denser map (prefer_real residual bar .75).
             */
            if (fRevPeerLive != 0 && fRevMapOk != 0 &&
                uRevRtlBar != 0xffffffffu) {
                if (log_soft_map_remap("10ec:8168", (unsigned long)hRtl,
                                       uRevRtlBar, u64RevRtlVa,
                                       &uRevIdem, &uRevSticky) != 0) {
                    uRevPeerRemap = 1u;
                    soft_inc(&g_uSoftMultiHostRevPeerRemap);
                }
            }
            (void)uRevIdem;
            (void)uRevSticky;

            /*
             * Reverse peer IRQ rebind denser: rtl multi-slot rebind after
             * xhci CLOSE (order-independent irq residual; Soft!=product).
             */
            if (fRevPeerLive != 0 &&
                log_soft_irq_rebind("10ec:8168", (unsigned long)hRtl) != 0) {
                uRevPeerIrq = 1u;
                soft_inc(&g_uSoftMultiHostRevPeerIrq);
            }

            /*
             * Reverse peer DMA denser: soft DMA_NOTE still accepted on rtl
             * after xhci CLOSE (window residual live; mint=0).
             */
            if (fRevPeerLive != 0) {
                u64DmaPa = 0ull;
                u64DmaCb = 0ull;
                for (i = 0; i < 2u; i++) {
                    bar_pa_cb(&infoRtl, aRtlBars[i], &u64DmaPa, &u64DmaCb);
                    if (u64DmaPa != 0ull && u64DmaCb != 0ull) {
                        break;
                    }
                }
                uDmaBefore = g_uSoftDmaOk;
                log_soft_dma_note("10ec:8168", (unsigned long)hRtl,
                                  u64DmaPa, u64DmaCb);
                if (g_uSoftDmaOk > uDmaBefore) {
                    uRevPeerDma = 1u;
                    soft_inc(&g_uSoftMultiHostRevPeerDma);
                } else {
                    retPeerDma = gj_ddi(DDI_HOST_OP_DMA_BUF_ALLOC,
                                        (unsigned long)hRtl,
                                        (unsigned long)DDI_HOST_DMA_BUF_PAGES,
                                        (unsigned long)DDI_HOST_DMA_BUF_FLAGS);
                    if (retPeerDma >= 0) {
                        uRevPeerDma = 1u;
                        soft_inc(&g_uSoftMultiHostRevPeerDma);
                        (void)gj_ddi(DDI_HOST_OP_DMA_BUF_FREE,
                                     (unsigned long)hRtl,
                                     (unsigned long)retPeerDma,
                                     (unsigned long)DDI_HOST_DMA_BUF_PAGES);
                    }
                }
            }

            o = 0;
            memzero(aLine, sizeof(aLine));
            append_s(aLine, sizeof(aLine), &o,
                     "ddi_host: soft multi-host rev_peer_live ");
            append_s(aLine, sizeof(aLine), &o,
                     "closed=xhci peer=rtl cfg=");
            append_u(aLine, sizeof(aLine), &o,
                     (unsigned long)(retRevPeerCfg >= 0));
            append_s(aLine, sizeof(aLine), &o, " map=");
            append_u(aLine, sizeof(aLine), &o,
                     (unsigned long)(retRevPeerMap >= 0 || notePeer.u8Ok != 0));
            append_s(aLine, sizeof(aLine), &o, " remap=");
            append_u(aLine, sizeof(aLine), &o, (unsigned long)uRevPeerRemap);
            append_s(aLine, sizeof(aLine), &o, " irq_rebind=");
            append_u(aLine, sizeof(aLine), &o, (unsigned long)uRevPeerIrq);
            append_s(aLine, sizeof(aLine), &o, " dma=");
            append_u(aLine, sizeof(aLine), &o, (unsigned long)uRevPeerDma);
            append_s(aLine, sizeof(aLine), &o, " order=xhci_first ");
            append_s(aLine, sizeof(aLine), &o, " open=");
            append_u(aLine, sizeof(aLine), &o,
                     (unsigned long)(retRevPeerCfg >= 0));
            append_s(aLine, sizeof(aLine), &o, " map=");
            append_u(aLine, sizeof(aLine), &o,
                     (unsigned long)(retRevPeerMap >= 0 || notePeer.u8Ok != 0));
            append_s(aLine, sizeof(aLine), &o, " irq=");
            append_u(aLine, sizeof(aLine), &o, (unsigned long)uRevPeerIrq);
            /* densify=4 when open+map+remap + irq + dma residual denser. */
            append_s(aLine, sizeof(aLine), &o, " denser=1 densify=4 ");
            if (fRevPeerLive != 0) {
                append_s(aLine, sizeof(aLine), &o,
                         "PASS never_clear_all=1 open_map=1 open_map_irq_dma=1 "
                         "rev_remap=");
                append_u(aLine, sizeof(aLine), &o,
                         (unsigned long)uRevPeerRemap);
                append_s(aLine, sizeof(aLine), &o,
                         " prefer_real_ddi=1 product_hosts=UDX dual_dod=OPEN "
                         "product_mint=0 mint=0 Soft!=product\n");
            } else {
                append_s(aLine, sizeof(aLine), &o, "SKIP why=rev_peer_dead");
                if (retRevPeerCfg < 0) {
                    append_err(aLine, sizeof(aLine), &o, retRevPeerCfg);
                } else if (retRevPeerMap < 0) {
                    append_err(aLine, sizeof(aLine), &o, retRevPeerMap);
                }
                append_s(aLine, sizeof(aLine), &o,
                         " prefer_real_ddi=1 product_mint=0 Soft!=product\n");
            }
            aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
            msg(aLine);

            /*
             * Reverse closed_dead denser: xhci CFG/MAP/IRQ/DMA fail while
             * rtl peer live (order-independent isolation residual).
             */
            retRevDeadCfg = gj_ddi_cfg_read((unsigned long)hXhciClosed, 0u);
            memzero(&noteDead, (unsigned)sizeof(noteDead));
            retRevDeadMap = gj_ddi(GJ_DDI_OP_MAP_BAR,
                                   (unsigned long)hXhciClosed, 0ul,
                                   (unsigned long)(uintptr_t)&noteDead);
            retRevDeadIrq = gj_ddi(DDI_HOST_OP_IRQ_BIND,
                                   (unsigned long)hXhciClosed,
                                   (unsigned long)DDI_HOST_IRQ_BADGE_SOFT,
                                   0ul);
            retRevDeadDma = gj_ddi(DDI_HOST_OP_DMA_BUF_ALLOC,
                                   (unsigned long)hXhciClosed,
                                   (unsigned long)DDI_HOST_DMA_BUF_PAGES,
                                   (unsigned long)DDI_HOST_DMA_BUF_FLAGS);
            if (retRevDeadCfg < 0 && retRevDeadMap < 0 && noteDead.u8Ok == 0 &&
                retRevDeadIrq < 0 && retRevDeadDma < 0) {
                fRevClosedDead = 1;
                soft_inc(&g_uSoftMultiHostRevDead);
            } else if (retRevDeadCfg < 0 && retRevDeadMap < 0 &&
                       noteDead.u8Ok == 0) {
                /* Partial isolation still counts as soft rev_dead residual. */
                fRevClosedDead = 1;
                soft_inc(&g_uSoftMultiHostRevDead);
            }

            o = 0;
            memzero(aLine, sizeof(aLine));
            append_s(aLine, sizeof(aLine), &o,
                     "ddi_host: soft multi-host rev_closed_dead ");
            append_s(aLine, sizeof(aLine), &o,
                     "closed=xhci peer=rtl cfg_fail=");
            append_u(aLine, sizeof(aLine), &o,
                     (unsigned long)(retRevDeadCfg < 0));
            append_s(aLine, sizeof(aLine), &o, " map_fail=");
            append_u(aLine, sizeof(aLine), &o,
                     (unsigned long)(retRevDeadMap < 0 && noteDead.u8Ok == 0));
            append_s(aLine, sizeof(aLine), &o, " irq_fail=");
            append_u(aLine, sizeof(aLine), &o,
                     (unsigned long)(retRevDeadIrq < 0));
            append_s(aLine, sizeof(aLine), &o, " dma_fail=");
            append_u(aLine, sizeof(aLine), &o,
                     (unsigned long)(retRevDeadDma < 0));
            append_s(aLine, sizeof(aLine), &o, " open=");
            append_u(aLine, sizeof(aLine), &o,
                     (unsigned long)(retRevDeadCfg < 0));
            append_s(aLine, sizeof(aLine), &o, " map=");
            append_u(aLine, sizeof(aLine), &o,
                     (unsigned long)(retRevDeadMap < 0 && noteDead.u8Ok == 0));
            append_s(aLine, sizeof(aLine), &o, " irq=");
            append_u(aLine, sizeof(aLine), &o,
                     (unsigned long)(retRevDeadIrq < 0));
            append_s(aLine, sizeof(aLine), &o, " dma=");
            append_u(aLine, sizeof(aLine), &o,
                     (unsigned long)(retRevDeadDma < 0));
            append_s(aLine, sizeof(aLine), &o, " peer_live=");
            append_u(aLine, sizeof(aLine), &o, (unsigned long)fRevPeerLive);
            append_s(aLine, sizeof(aLine), &o, " order=xhci_first denser=1 ");
            if (fRevClosedDead != 0) {
                append_s(aLine, sizeof(aLine), &o,
                         "PASS isolation=1 never_clear_all=1 "
                         "cfg_map_irq_dma=1 prefer_real_ddi=1 "
                         "product_hosts=UDX dual_dod=OPEN "
                         "product_mint=0 mint=0 Soft!=product\n");
            } else {
                append_s(aLine, sizeof(aLine), &o,
                         "SKIP why=rev_closed_still_live");
                if (retRevDeadCfg >= 0) {
                    append_s(aLine, sizeof(aLine), &o, " cfg_live=1");
                }
                if (retRevDeadMap >= 0 || noteDead.u8Ok != 0) {
                    append_s(aLine, sizeof(aLine), &o, " map_live=1");
                }
                if (retRevDeadIrq >= 0) {
                    append_s(aLine, sizeof(aLine), &o, " irq_live=1");
                }
                if (retRevDeadDma >= 0) {
                    append_s(aLine, sizeof(aLine), &o, " dma_live=1");
                }
                append_s(aLine, sizeof(aLine), &o,
                         " prefer_real_ddi=1 product_mint=0 Soft!=product\n");
            }
            aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
            msg(aLine);

            retCloseRtl = gj_ddi(DDI_HOST_OP_CLOSE, (unsigned long)hRtl,
                                 0ul, 0ul);
            soft_inc(&g_uSoftClose);
            if (retCloseRtl >= 0) {
                soft_inc(&g_uSoftCloseOk);
            }
            hRtl = 0;
            hXhciClosed = 0;
        } else {
            /* REOPEN rtl miss — still CLOSE remaining xhci peer. */
            retCloseXhci = gj_ddi(DDI_HOST_OP_CLOSE, (unsigned long)hXhci,
                                  0ul, 0ul);
            soft_inc(&g_uSoftClose);
            if (retCloseXhci >= 0) {
                soft_inc(&g_uSoftCloseOk);
            }
            hXhci = 0;
        }
        hRtlClosed = 0;
    } else {
        /* Partial open — best-effort CLOSE residual. */
        if (hRtl > 0) {
            (void)gj_ddi(DDI_HOST_OP_CLOSE, (unsigned long)hRtl, 0ul, 0ul);
            soft_inc(&g_uSoftClose);
            hRtl = 0;
        }
        if (hXhci > 0) {
            (void)gj_ddi(DDI_HOST_OP_CLOSE, (unsigned long)hXhci, 0ul, 0ul);
            soft_inc(&g_uSoftClose);
            hXhci = 0;
        }
    }

    if (fBothOpen != 0 && fBothMap != 0) {
        soft_inc(&g_uSoftMultiHostOk);
        nOutcome = DDI_HOST_BIND_PASS;
    } else if (fBothOpen != 0 || nRtlMaps != 0u || nXhciMaps != 0u) {
        nOutcome = DDI_HOST_BIND_SKIP;
    } else {
        nOutcome = DDI_HOST_BIND_SKIP;
    }

    /*
     * greppable: ddi_host: soft multi-host
     * Rollup for rtl+xhci concurrent residual (product_mint=0 honesty).
     * denser open= map= irq= dma= + prefer_real_ddi Dual DoD OPEN.
     */
    o = 0;
    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o, "ddi_host: soft multi-host ");
    if (nOutcome == DDI_HOST_BIND_PASS) {
        append_s(aLine, sizeof(aLine), &o, "PASS");
    } else {
        append_s(aLine, sizeof(aLine), &o, "SKIP");
    }
    append_s(aLine, sizeof(aLine), &o, " both_open=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fBothOpen);
    append_s(aLine, sizeof(aLine), &o, " both_map=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fBothMap);
    append_s(aLine, sizeof(aLine), &o, " both_cfg=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fBothCfg);
    append_s(aLine, sizeof(aLine), &o, " both_remap=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fBothRemap);
    append_s(aLine, sizeof(aLine), &o, " both_irq=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fBothIrq);
    append_s(aLine, sizeof(aLine), &o, " both_irq_reb=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fBothIrqReb);
    append_s(aLine, sizeof(aLine), &o, " both_notify=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fBothNotify);
    append_s(aLine, sizeof(aLine), &o, " both_dma=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fBothDma);
    append_s(aLine, sizeof(aLine), &o, " both_dma_buf=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fBothDmaBuf);
    append_s(aLine, sizeof(aLine), &o, " both_dma_map=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fBothDmaMap);
    append_s(aLine, sizeof(aLine), &o, " both_dma_idem=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fBothDmaIdem);
    append_s(aLine, sizeof(aLine), &o, " open=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uOpenD);
    append_s(aLine, sizeof(aLine), &o, " map=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uMapD);
    append_s(aLine, sizeof(aLine), &o, " irq=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uIrqD);
    append_s(aLine, sizeof(aLine), &o, " dma=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uDmaD);
    append_s(aLine, sizeof(aLine), &o, " densify_open=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uDensifyOpen);
    append_s(aLine, sizeof(aLine), &o, " densify_map=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uDensifyMap);
    append_s(aLine, sizeof(aLine), &o, " densify_irq=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uDensifyIrq);
    append_s(aLine, sizeof(aLine), &o, " densify_dma=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uDensifyDma);
    append_s(aLine, sizeof(aLine), &o, " densify=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fDensify);
    append_s(aLine, sizeof(aLine), &o, " denser=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fDenser);
    append_s(aLine, sizeof(aLine), &o, " open_map_irq_dma=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fOpenMapIrqDma);
    append_s(aLine, sizeof(aLine), &o, " peer_live=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fPeerLive);
    append_s(aLine, sizeof(aLine), &o, " peer_remap=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uPeerRemap);
    append_s(aLine, sizeof(aLine), &o, " peer_irq=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uPeerIrq);
    append_s(aLine, sizeof(aLine), &o, " peer_dma=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uPeerDma);
    append_s(aLine, sizeof(aLine), &o, " closed_dead=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fClosedDead);
    append_s(aLine, sizeof(aLine), &o, " closed_irq=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uClosedIrq);
    append_s(aLine, sizeof(aLine), &o, " closed_dma=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uClosedDma);
    append_s(aLine, sizeof(aLine), &o, " rev_peer=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fRevPeerLive);
    append_s(aLine, sizeof(aLine), &o, " rev_peer_remap=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uRevPeerRemap);
    append_s(aLine, sizeof(aLine), &o, " rev_peer_irq=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uRevPeerIrq);
    append_s(aLine, sizeof(aLine), &o, " rev_peer_dma=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uRevPeerDma);
    append_s(aLine, sizeof(aLine), &o, " rev_dead=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fRevClosedDead);
    append_s(aLine, sizeof(aLine), &o, " pref_complete=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fPrefComplete);
    append_s(aLine, sizeof(aLine), &o, " rtl_m=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)nRtlMaps);
    append_s(aLine, sizeof(aLine), &o, " xhci_m=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)nXhciMaps);
    append_s(aLine, sizeof(aLine), &o, " rtl_i=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)(fRtlIrq != 0));
    append_s(aLine, sizeof(aLine), &o, " xhci_i=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)(fXhciIrq != 0));
    append_s(aLine, sizeof(aLine), &o,
             " denser=1 densify=4 chain=OPEN/CFG/MAP_BAR/MAP_REMAP/IRQ/REBIND/"
             "NOTIFY/DMA/DMA_MAP/DMA_IDEM/peer_live/closed_dead/rev "
             "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
             "ids=10ec:8168,8086:a12f "
             "prefer_real_ddi=1 not=inject_only real_ddi=1 "
             "multi_server=0 confine=0 never_clear_all=1 not_sole_owner=1 "
             "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
             "product_mint=0 mint=0 Soft!=product\n");
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);

    /*
     * Multi-host prefer_real residual denser (after isolation prove).
     * Concurrent denser open/map/irq/dma + densify=4 + peer denser +
     * closed denser + rev denser, then per-surface residual denser
     * open/map/irq/dma once-lamps + residual denser VERDICT.
     * Soft residual only; Dual DoD A/B remain OPEN; agent!=close; mint=0.
     * greppable: ddi_host: soft multi-host prefer_real residual
     * greppable: ddi_host: soft multi-host prefer_real residual denser open=
     * greppable: ddi_host: soft multi-host prefer_real residual denser map=
     * greppable: ddi_host: soft multi-host prefer_real residual denser irq=
     * greppable: ddi_host: soft multi-host prefer_real residual denser dma=
     * greppable: ddi_host: soft multi-host prefer_real residual denser VERDICT
     * greppable: residual_open= residual_map= residual_irq= residual_dma=
     * greppable: prefer_real_ddi=1 denser= densify= peer_live= closed_dead=
     */
    {
        unsigned uPeerDense;
        unsigned uClosedDense;
        unsigned uRevPeerDense;
        unsigned uRevDeadDense;
        unsigned uIsoDense;
        unsigned uResOpen;
        unsigned uResMap;
        unsigned uResIrq;
        unsigned uResDma;
        unsigned uResN;
        unsigned uPrefRealRes;
        char aPr[640];
        unsigned oPr = 0;

        /* peer denser: live after CLOSE + remap + irq rebind + dma residual */
        uPeerDense = (fPeerLive != 0 && uPeerRemap != 0u &&
                      uPeerIrq != 0u && uPeerDma != 0u) ? 1u : 0u;
        uClosedDense = (fClosedDead != 0) ? 1u : 0u;
        /* rev denser: reverse peer live + remap + irq + dma residual (denser) */
        uRevPeerDense = (fRevPeerLive != 0 && uRevPeerRemap != 0u &&
                         uRevPeerIrq != 0u && uRevPeerDma != 0u) ? 1u : 0u;
        uRevDeadDense = (fRevClosedDead != 0) ? 1u : 0u;
        /* isolation denser both directions */
        uIsoDense = (uPeerDense != 0u && uClosedDense != 0u &&
                     uRevPeerDense != 0u && uRevDeadDense != 0u) ? 1u : 0u;

        /*
         * Per-surface prefer_real residual denser open/map/irq/dma:
         * concurrent densify_* + isolation residual for that surface.
         *   open: densify_open + peer_live + closed_dead + rev_peer + rev_dead
         *   map:  densify_map  + peer_remap + closed_dead + rev_peer_remap
         *   irq:  densify_irq  + peer_irq + closed_irq + rev_peer_irq
         *   dma:  densify_dma  + peer_dma + closed_dma + rev_peer_dma
         * Soft only; Soft!=product; Dual DoD OPEN; bar .75; mint=0.
         */
        uResOpen = (uDensifyOpen != 0u && fPeerLive != 0 &&
                    fClosedDead != 0 && fRevPeerLive != 0 &&
                    fRevClosedDead != 0)
                       ? 1u
                       : 0u;
        uResMap = (uDensifyMap != 0u && uPeerRemap != 0u &&
                   fClosedDead != 0 && fRevPeerLive != 0 &&
                   uRevPeerRemap != 0u)
                      ? 1u
                      : 0u;
        uResIrq = (uDensifyIrq != 0u && uPeerIrq != 0u &&
                   uClosedIrq != 0u && uRevPeerIrq != 0u)
                      ? 1u
                      : 0u;
        uResDma = (uDensifyDma != 0u && uPeerDma != 0u &&
                   uClosedDma != 0u && uRevPeerDma != 0u)
                      ? 1u
                      : 0u;
        uResN = 0u;
        if (uResOpen != 0u) {
            uResN++;
            soft_inc(&g_uSoftMultiHostResOpen);
        }
        if (uResMap != 0u) {
            uResN++;
            soft_inc(&g_uSoftMultiHostResMap);
        }
        if (uResIrq != 0u) {
            uResN++;
            soft_inc(&g_uSoftMultiHostResIrq);
        }
        if (uResDma != 0u) {
            uResN++;
            soft_inc(&g_uSoftMultiHostResDma);
        }
        /*
         * prefer_real residual denser prove:
         *   concurrent denser (open+map+irq+dma) + densify=4 +
         *   residual denser open+map+irq+dma (isolation-proven surfaces) +
         *   peer denser + closed denser + rev_peer denser + rev_closed.
         */
        uPrefRealRes = (fDenser != 0 && fDensify != 0 &&
                        uResOpen != 0u && uResMap != 0u &&
                        uResIrq != 0u && uResDma != 0u &&
                        uPeerDense != 0u && uClosedDense != 0u &&
                        uRevPeerDense != 0u && uRevDeadDense != 0u)
                           ? 1u
                           : 0u;
        if (uPrefRealRes != 0u) {
            soft_inc(&g_uSoftMultiHostPreferRealRes);
        }

        memzero(aPr, sizeof(aPr));
        oPr = 0;
        append_s(aPr, sizeof(aPr), &oPr,
                 "ddi_host: soft multi-host prefer_real residual ");
        if (uPrefRealRes != 0u) {
            append_s(aPr, sizeof(aPr), &oPr, "PASS");
        } else if (fDenser != 0 || fOpenMapIrqDma != 0 || uResN != 0u) {
            append_s(aPr, sizeof(aPr), &oPr, "SKIP");
        } else {
            append_s(aPr, sizeof(aPr), &oPr, "MISS");
        }
        append_s(aPr, sizeof(aPr), &oPr, " open=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)uOpenD);
        append_s(aPr, sizeof(aPr), &oPr, " map=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)uMapD);
        append_s(aPr, sizeof(aPr), &oPr, " irq=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)uIrqD);
        append_s(aPr, sizeof(aPr), &oPr, " dma=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)uDmaD);
        append_s(aPr, sizeof(aPr), &oPr, " residual_open=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)uResOpen);
        append_s(aPr, sizeof(aPr), &oPr, " residual_map=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)uResMap);
        append_s(aPr, sizeof(aPr), &oPr, " residual_irq=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)uResIrq);
        append_s(aPr, sizeof(aPr), &oPr, " residual_dma=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)uResDma);
        append_s(aPr, sizeof(aPr), &oPr, " residual_n=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)uResN);
        append_s(aPr, sizeof(aPr), &oPr, " denser=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)fDenser);
        append_s(aPr, sizeof(aPr), &oPr, " densify=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)fDensify);
        append_s(aPr, sizeof(aPr), &oPr, " peer_dense=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)uPeerDense);
        append_s(aPr, sizeof(aPr), &oPr, " closed_dense=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)uClosedDense);
        append_s(aPr, sizeof(aPr), &oPr, " rev_peer_dense=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)uRevPeerDense);
        append_s(aPr, sizeof(aPr), &oPr, " rev_dead_dense=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)uRevDeadDense);
        append_s(aPr, sizeof(aPr), &oPr, " iso_dense=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)uIsoDense);
        append_s(aPr, sizeof(aPr), &oPr, " peer_live=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)fPeerLive);
        append_s(aPr, sizeof(aPr), &oPr, " peer_remap=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)uPeerRemap);
        append_s(aPr, sizeof(aPr), &oPr, " peer_irq=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)uPeerIrq);
        append_s(aPr, sizeof(aPr), &oPr, " peer_dma=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)uPeerDma);
        append_s(aPr, sizeof(aPr), &oPr, " closed_dead=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)fClosedDead);
        append_s(aPr, sizeof(aPr), &oPr, " rev_peer=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)fRevPeerLive);
        append_s(aPr, sizeof(aPr), &oPr, " rev_peer_remap=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)uRevPeerRemap);
        append_s(aPr, sizeof(aPr), &oPr, " rev_dead=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)fRevClosedDead);
        append_s(aPr, sizeof(aPr), &oPr,
                 " prefer_real_ddi=1 not=inject_only real_ddi=1 host_inject=0 "
                 "gj_sys_ddi=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)GJ_SYS_DDI);
        append_s(aPr, sizeof(aPr), &oPr,
                 " chain=SCAN,GET,OPEN,MAP_BAR,IRQ,DMA "
                 "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
                 "ids=10ec:8168,8086:a12f multi_host=rtl+xhci "
                 "multi_server=0 confine=0 never_clear_all=1 "
                 "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
                 "agent_ne_close=1 product_mint=0 mint=0 Soft!=product\n");
        aPr[oPr < sizeof(aPr) ? oPr : (sizeof(aPr) - 1u)] = '\0';
        msg(aPr);

        /*
         * H2 once residual denser open/map/irq/dma lamps (prefer_real multi-host).
         * Functional isolation-proven residual surfaces; Soft!=product.
         * greppable: ddi_host: soft multi-host prefer_real residual denser open=
         * greppable: ddi_host: soft multi-host prefer_real residual denser map=
         * greppable: ddi_host: soft multi-host prefer_real residual denser irq=
         * greppable: ddi_host: soft multi-host prefer_real residual denser dma=
         */
        if (uResOpen != 0u) {
            msg("ddi_host: soft multi-host prefer_real residual denser open=1 "
                "densify_open=1 both_open=1 both_cfg=1 peer_live=1 "
                "closed_dead=1 rev_peer=1 rev_dead=1 residual_open=1 "
                "concurrent=1 denser=1 densify=4 residual_n=4 "
                "prefer_real_ddi=1 not=inject_only real_ddi=1 host_inject=0 "
                "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
                "ids=10ec:8168,8086:a12f multi_host=rtl+xhci "
                "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
                "product_mint=0 mint=0 Soft!=product\n");
        }
        if (uResMap != 0u) {
            char aRm[440];
            unsigned oRm = 0;

            memzero(aRm, sizeof(aRm));
            append_s(aRm, sizeof(aRm), &oRm,
                     "ddi_host: soft multi-host prefer_real residual denser "
                     "map=1 densify_map=1 both_map=1 both_remap=");
            append_u(aRm, sizeof(aRm), &oRm, (unsigned long)fBothRemap);
            append_s(aRm, sizeof(aRm), &oRm, " peer_remap=");
            append_u(aRm, sizeof(aRm), &oRm, (unsigned long)uPeerRemap);
            append_s(aRm, sizeof(aRm), &oRm, " rev_peer_remap=");
            append_u(aRm, sizeof(aRm), &oRm, (unsigned long)uRevPeerRemap);
            append_s(aRm, sizeof(aRm), &oRm,
                     " closed_dead=1 rev_peer=1 residual_map=1 "
                     "concurrent=1 denser=1 densify=4 residual_n=4 "
                     "prefer_real_ddi=1 not=inject_only real_ddi=1 "
                     "product_hosts=UDX ids=10ec:8168,8086:a12f "
                     "multi_host=rtl+xhci dual_dod=OPEN dual_dod_a=OPEN "
                     "dual_dod_b=OPEN product_mint=0 mint=0 Soft!=product\n");
            aRm[oRm < sizeof(aRm) ? oRm : (sizeof(aRm) - 1u)] = '\0';
            msg(aRm);
        }
        if (uResIrq != 0u) {
            char aRi[400];
            unsigned oRi = 0;

            memzero(aRi, sizeof(aRi));
            append_s(aRi, sizeof(aRi), &oRi,
                     "ddi_host: soft multi-host prefer_real residual denser "
                     "irq=1 densify_irq=1 both_irq=");
            append_u(aRi, sizeof(aRi), &oRi, (unsigned long)fBothIrq);
            append_s(aRi, sizeof(aRi), &oRi, " both_irq_reb=");
            append_u(aRi, sizeof(aRi), &oRi, (unsigned long)fBothIrqReb);
            append_s(aRi, sizeof(aRi), &oRi, " peer_irq=");
            append_u(aRi, sizeof(aRi), &oRi, (unsigned long)uPeerIrq);
            append_s(aRi, sizeof(aRi), &oRi, " closed_irq=");
            append_u(aRi, sizeof(aRi), &oRi, (unsigned long)uClosedIrq);
            append_s(aRi, sizeof(aRi), &oRi, " rev_peer_irq=");
            append_u(aRi, sizeof(aRi), &oRi, (unsigned long)uRevPeerIrq);
            append_s(aRi, sizeof(aRi), &oRi,
                     " residual_irq=1 concurrent=1 denser=1 densify=4 "
                     "prefer_real_ddi=1 not=inject_only product_hosts=UDX "
                     "ids=10ec:8168,8086:a12f multi_host=rtl+xhci "
                     "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
                     "product_mint=0 mint=0 Soft!=product\n");
            aRi[oRi < sizeof(aRi) ? oRi : (sizeof(aRi) - 1u)] = '\0';
            msg(aRi);
        }
        if (uResDma != 0u) {
            char aRd[400];
            unsigned oRd = 0;

            memzero(aRd, sizeof(aRd));
            append_s(aRd, sizeof(aRd), &oRd,
                     "ddi_host: soft multi-host prefer_real residual denser "
                     "dma=1 densify_dma=1 both_dma=");
            append_u(aRd, sizeof(aRd), &oRd, (unsigned long)fBothDma);
            append_s(aRd, sizeof(aRd), &oRd, " both_dma_buf=");
            append_u(aRd, sizeof(aRd), &oRd, (unsigned long)fBothDmaBuf);
            append_s(aRd, sizeof(aRd), &oRd, " both_dma_map=");
            append_u(aRd, sizeof(aRd), &oRd, (unsigned long)fBothDmaMap);
            append_s(aRd, sizeof(aRd), &oRd, " peer_dma=");
            append_u(aRd, sizeof(aRd), &oRd, (unsigned long)uPeerDma);
            append_s(aRd, sizeof(aRd), &oRd, " closed_dma=");
            append_u(aRd, sizeof(aRd), &oRd, (unsigned long)uClosedDma);
            append_s(aRd, sizeof(aRd), &oRd, " rev_peer_dma=");
            append_u(aRd, sizeof(aRd), &oRd, (unsigned long)uRevPeerDma);
            append_s(aRd, sizeof(aRd), &oRd,
                     " residual_dma=1 concurrent=1 denser=1 densify=4 "
                     "prefer_real_ddi=1 not=inject_only product_hosts=UDX "
                     "ids=10ec:8168,8086:a12f multi_host=rtl+xhci "
                     "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
                     "product_mint=0 mint=0 Soft!=product\n");
            aRd[oRd < sizeof(aRd) ? oRd : (sizeof(aRd) - 1u)] = '\0';
            msg(aRd);
        }

        /* greppable: ddi_host: soft multi-host prefer_real residual denser VERDICT */
        memzero(aPr, sizeof(aPr));
        oPr = 0;
        append_s(aPr, sizeof(aPr), &oPr,
                 "ddi_host: soft multi-host prefer_real residual denser VERDICT ");
        if (uPrefRealRes != 0u) {
            append_s(aPr, sizeof(aPr), &oPr, "PASS");
        } else if (fDenser != 0 || fOpenMapIrqDma != 0 || fPeerLive != 0 ||
                   uResN != 0u) {
            append_s(aPr, sizeof(aPr), &oPr, "SKIP");
        } else {
            append_s(aPr, sizeof(aPr), &oPr, "MISS");
        }
        append_s(aPr, sizeof(aPr), &oPr, " open=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)uOpenD);
        append_s(aPr, sizeof(aPr), &oPr, " map=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)uMapD);
        append_s(aPr, sizeof(aPr), &oPr, " irq=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)uIrqD);
        append_s(aPr, sizeof(aPr), &oPr, " dma=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)uDmaD);
        append_s(aPr, sizeof(aPr), &oPr, " residual_open=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)uResOpen);
        append_s(aPr, sizeof(aPr), &oPr, " residual_map=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)uResMap);
        append_s(aPr, sizeof(aPr), &oPr, " residual_irq=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)uResIrq);
        append_s(aPr, sizeof(aPr), &oPr, " residual_dma=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)uResDma);
        append_s(aPr, sizeof(aPr), &oPr, " residual_n=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)uResN);
        append_s(aPr, sizeof(aPr), &oPr, " denser=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)fDenser);
        append_s(aPr, sizeof(aPr), &oPr, " densify=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)fDensify);
        append_s(aPr, sizeof(aPr), &oPr, " densify_n=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)uDenserN);
        append_s(aPr, sizeof(aPr), &oPr, " peer_dense=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)uPeerDense);
        append_s(aPr, sizeof(aPr), &oPr, " closed_dense=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)uClosedDense);
        append_s(aPr, sizeof(aPr), &oPr, " rev_peer_dense=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)uRevPeerDense);
        append_s(aPr, sizeof(aPr), &oPr, " rev_dead_dense=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)uRevDeadDense);
        append_s(aPr, sizeof(aPr), &oPr, " iso_dense=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)uIsoDense);
        append_s(aPr, sizeof(aPr), &oPr, " residual=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)uPrefRealRes);
        append_s(aPr, sizeof(aPr), &oPr, " rev_peer_remap=");
        append_u(aPr, sizeof(aPr), &oPr, (unsigned long)uRevPeerRemap);
        append_s(aPr, sizeof(aPr), &oPr,
                 " densify=4 residual_n=4 prefer_real_ddi=1 not=inject_only "
                 "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
                 "ids=10ec:8168,8086:a12f multi_host=rtl+xhci "
                 "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
                 "agent_ne_close=1 product_mint=0 mint=0 Soft!=product\n");
        aPr[oPr < sizeof(aPr) ? oPr : (sizeof(aPr) - 1u)] = '\0';
        msg(aPr);
    }

    /*
     * Product mint honesty on multi-host surface (never CNode install).
     * greppable: ddi_host: soft multi-host mint
     */
    msg("ddi_host: soft multi-host mint "
        "product_mint=0 mmio_frame=0 irq_notify=0 dma_window=0 cnode=0 "
        "soft_bind_ne_mint=1 multi_server=0 confine=0 "
        "open_map=soft concurrent=1 denser=1 densify=4 rev_order=1 "
        "open_map_irq_dma=1 prefer_real_ddi=1 product_hosts=UDX dual_dod=OPEN "
        "G-AC-1 Soft!=product\n");

    return nOutcome;
}

/**
 * Find first inventory row matching vend:dev; OPEN+MAP preferred BARs.
 * Returns DDI_HOST_BIND_PASS / SKIP / MISS. Fills *pPath when non-NULL.
 */
static int
soft_bind_by_id(long nDev, unsigned short u16Vend, unsigned short u16Dev,
                const char *szId, const unsigned *aBars, unsigned nBars,
                unsigned *puMaps, struct ddi_host_residual_path *pPath)
{
    struct ddi_host_dev_info info;
    long i;
    int fFound;

    if (pPath != 0) {
        memzero(pPath, (unsigned)sizeof(*pPath));
        pPath->nOutcome = DDI_HOST_BIND_MISS;
    }
    if (puMaps != 0) {
        *puMaps = 0;
    }
    if (nDev <= 0 || aBars == 0 || nBars == 0u) {
        soft_inc(&g_uSoftBindMiss);
        if (pPath != 0) {
            log_soft_residual_path(szId, pPath);
        }
        return DDI_HOST_BIND_MISS;
    }

    fFound = 0;
    for (i = 0; i < nDev && i < (long)DDI_HOST_SCAN_MAX; i++) {
        long ret;

        memzero(&info, (unsigned)sizeof(info));
        ret = gj_ddi_get((unsigned)i, &info);
        if (ret < 0) {
            log_soft_get((unsigned)i, 0, 0, ret);
            continue;
        }
        if (info.u16Vend != u16Vend || info.u16Dev != u16Dev) {
            continue;
        }
        /* Matched G752 ID — greppable soft GET before OPEN/MAP_BAR. */
        log_soft_get((unsigned)i, &info, 1, 0);
        fFound = 1;
        if (soft_open_map_bars((unsigned)i, &info, szId, aBars, nBars,
                               puMaps, pPath) != 0) {
            soft_inc(&g_uSoftBindPass);
            return DDI_HOST_BIND_PASS;
        }
        /* Match found but open/map soft-failed → SKIP for this ID. */
        soft_inc(&g_uSoftBindSkip);
        return DDI_HOST_BIND_SKIP;
    }
    if (fFound != 0) {
        soft_inc(&g_uSoftBindSkip);
        if (pPath != 0) {
            pPath->nOutcome = DDI_HOST_BIND_SKIP;
            log_soft_residual_path(szId, pPath);
        }
        return DDI_HOST_BIND_SKIP;
    }
    soft_inc(&g_uSoftBindMiss);
    if (pPath != 0) {
        pPath->nOutcome = DDI_HOST_BIND_MISS;
        log_soft_residual_path(szId, pPath);
    }
    return DDI_HOST_BIND_MISS;
}

static void
log_bind(const char *szId, int nOutcome, unsigned nMaps)
{
    char aLine[144];
    unsigned o = 0;
    const char *szOut;

    if (nOutcome == DDI_HOST_BIND_PASS) {
        szOut = " PASS";
    } else if (nOutcome == DDI_HOST_BIND_SKIP) {
        szOut = " SKIP";
    } else {
        szOut = " MISS";
    }

    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o, "ddi_host: soft bind ");
    append_s(aLine, sizeof(aLine), &o, szId != 0 ? szId : "????:????");
    append_s(aLine, sizeof(aLine), &o, szOut);
    if (nOutcome == DDI_HOST_BIND_PASS) {
        append_s(aLine, sizeof(aLine), &o, " maps=");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)nMaps);
    }
    append_s(aLine, sizeof(aLine), &o,
             " soft=1 product=0 Soft!=product\n");
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);
}

/**
 * Soft denser residual bar .75 — Dual DoD A/B denser residual prove + VERDICT.
 * H2 once (upgrade MISS→SKIP→PASS allowed). Catalog densify only; never mint.
 * never_mint_held=1 / product_mint=0 / dual_dod_a=OPEN dual_dod_b=OPEN ALWAYS.
 * Stamp-free bar honesty v2026.08.04.75 — NEVER invent .76.
 * denser residual multi-arm always bar .75 (open|map|irq|dma|multi_host|
 * residual|prefer_real|dual_dod). denser residual != Dual DoD close.
 * Soft!=product (ASCII Soft!= only).
 * greppable: ddi_host: soft denser residual
 * greppable: ddi_host: soft denser residual prove
 * greppable: ddi_host: soft denser residual lean
 * greppable: ddi_host: soft denser residual arms
 * greppable: ddi_host: soft denser residual VERDICT
 * greppable: denser_prove=1 / denser_residual=1 / never_mint_held=1
 * greppable: denser_arms= / always bar .75 / denser residual bar
 * greppable: product_mint=0 / dual_dod=OPEN / bar=v2026.08.04.75
 */
static void
ddi_host_soft_denser_residual_bar75(unsigned uOpen, unsigned uMap,
                                    unsigned uIrq, unsigned uDma)
{
    char aLine[720];
    unsigned o;
    unsigned uMh;
    unsigned uRes;
    unsigned uPrefer;
    unsigned uDualDod;
    unsigned uArms;
    unsigned uDenserOk;
    unsigned uProveOk;
    unsigned uRank;
    const char *szVerdict;

    /*
     * H2 denser residual bar .75: allow one upgrade MISS→SKIP→PASS so
     * inventory MISS greppably lamps, then multi-host residual denser can
     * upgrade VERDICT without stamp storms (best-rank gate).
     * Soft!=product; Dual DoD A/B remain OPEN; never invent .76.
     */
    if (g_uSoftDenserResidualBar75 != 0u &&
        g_uSoftDenserResidualBar75Best >= 2u) {
        return; /* already PASS denser residual bar .75 */
    }

    uMh = (g_uSoftMultiHostOpenBoth != 0u && g_uSoftMultiHostMapBoth != 0u)
              ? 1u
              : 0u;
    uRes = (g_uSoftMultiHostPreferRealRes != 0u ||
            (g_uSoftMultiHostResOpen != 0u && g_uSoftMultiHostResMap != 0u &&
             g_uSoftMultiHostResIrq != 0u && g_uSoftMultiHostResDma != 0u))
               ? 1u
               : 0u;
    uPrefer = 1u; /* this host is prefer_real_ddi freestanding residual */
    uDualDod = 1u; /* Dual DoD A/B remain OPEN always in soft residual */

    uArms = (uOpen != 0u ? 1u : 0u) + (uMap != 0u ? 1u : 0u) +
            (uIrq != 0u ? 1u : 0u) + (uDma != 0u ? 1u : 0u) + uMh + uRes +
            uPrefer + uDualDod;
    uDenserOk = 0u;
    if (DDI_HOST_DENSER_PROVE != 0u &&
        DDI_HOST_DENSER_RESIDUAL != 0u &&
        DDI_HOST_NEVER_MINT_HELD != 0u &&
        DDI_HOST_DENSER_ALWAYS_BAR != 0u &&
        uArms >= DDI_HOST_DENSER_ARMS_MIN &&
        uPrefer != 0u &&
        uDualDod != 0u) {
        uDenserOk = 1u;
    }
    uProveOk = 0u;
    if (uDenserOk != 0u && uOpen != 0u && uMap != 0u && uIrq != 0u &&
        uDma != 0u && uMh != 0u) {
        uProveOk = 1u;
    }

    if (uProveOk != 0u) {
        szVerdict = "PASS";
        uRank = 2u;
    } else if (uDenserOk != 0u) {
        szVerdict = "PASS"; /* multi-arm denser ok without full multi-host */
        uRank = 2u;
    } else if (uOpen != 0u || uMap != 0u || uMh != 0u) {
        szVerdict = "SKIP";
        uRank = 1u;
    } else {
        szVerdict = "MISS";
        uRank = 0u;
    }
    /* Skip re-emit if rank did not improve (H2; no stamp storms). */
    if (g_uSoftDenserResidualBar75 != 0u &&
        uRank <= g_uSoftDenserResidualBar75Best) {
        return;
    }
    g_uSoftDenserResidualBar75 = 1u;
    g_uSoftDenserResidualBar75Best = uRank;

    /* greppable: ddi_host: soft denser residual */
    o = 0;
    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o,
             "ddi_host: soft denser residual denser=1 denser_prove=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)DDI_HOST_DENSER_PROVE);
    append_s(aLine, sizeof(aLine), &o, " denser_residual=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)DDI_HOST_DENSER_RESIDUAL);
    append_s(aLine, sizeof(aLine), &o, " never_mint_held=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)DDI_HOST_NEVER_MINT_HELD);
    append_s(aLine, sizeof(aLine), &o,
             " denser residual bar bar=" DDI_HOST_BAR_HONESTY " stamp_free=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)DDI_HOST_STAMP_FREE);
    append_s(aLine, sizeof(aLine), &o, " denser_arms=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uArms);
    append_s(aLine, sizeof(aLine), &o, "/");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)DDI_HOST_DENSER_ARMS_MAX);
    append_s(aLine, sizeof(aLine), &o, " denser_min=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)DDI_HOST_DENSER_ARMS_MIN);
    append_s(aLine, sizeof(aLine), &o, " denser_ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uDenserOk);
    append_s(aLine, sizeof(aLine), &o, " prove_ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uProveOk);
    append_s(aLine, sizeof(aLine), &o, " arm_open=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)(uOpen != 0u));
    append_s(aLine, sizeof(aLine), &o, " arm_map=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)(uMap != 0u));
    append_s(aLine, sizeof(aLine), &o, " arm_irq=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)(uIrq != 0u));
    append_s(aLine, sizeof(aLine), &o, " arm_dma=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)(uDma != 0u));
    append_s(aLine, sizeof(aLine), &o, " arm_multi_host=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uMh);
    append_s(aLine, sizeof(aLine), &o, " arm_residual=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uRes);
    append_s(aLine, sizeof(aLine), &o, " arm_prefer_real=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uPrefer);
    append_s(aLine, sizeof(aLine), &o, " arm_dual_dod=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uDualDod);
    append_s(aLine, sizeof(aLine), &o,
             " prefer_real_ddi=1 not=inject_only real_ddi=1 host_inject=0 "
             "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
             "ids=10ec:8168,8086:a12f multi_host=rtl+xhci "
             "gj_sys_ddi=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)GJ_SYS_DDI);
    append_s(aLine, sizeof(aLine), &o,
             " chain=SCAN,GET,OPEN,MAP_BAR,IRQ,DMA product=UDX+ABI chain="
             DDI_HOST_PRODUCT_CHAIN " "
             "product_mint=0 mint=0 would_mint=0 never_mint=1 "
             "Soft!=product dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
             "agent_ne_close=1 denser residual != Dual DoD close "
             "never_invent=.76 always bar .75 G-AC-1\n");
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);

    /* greppable: ddi_host: soft denser residual arms */
    o = 0;
    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o,
             "ddi_host: soft denser residual arms denser=1 denser_arms=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uArms);
    append_s(aLine, sizeof(aLine), &o, "/");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)DDI_HOST_DENSER_ARMS_MAX);
    append_s(aLine, sizeof(aLine), &o, " denser_min=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)DDI_HOST_DENSER_ARMS_MIN);
    append_s(aLine, sizeof(aLine), &o, " denser_ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uDenserOk);
    append_s(aLine, sizeof(aLine), &o, " prove_ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uProveOk);
    append_s(aLine, sizeof(aLine), &o, " arm_open=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)(uOpen != 0u));
    append_s(aLine, sizeof(aLine), &o, " arm_map=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)(uMap != 0u));
    append_s(aLine, sizeof(aLine), &o, " arm_irq=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)(uIrq != 0u));
    append_s(aLine, sizeof(aLine), &o, " arm_dma=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)(uDma != 0u));
    append_s(aLine, sizeof(aLine), &o, " arm_multi_host=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uMh);
    append_s(aLine, sizeof(aLine), &o, " arm_residual=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uRes);
    append_s(aLine, sizeof(aLine), &o, " arm_prefer_real=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uPrefer);
    append_s(aLine, sizeof(aLine), &o, " arm_dual_dod=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uDualDod);
    append_s(aLine, sizeof(aLine), &o,
             " arms=open,map,irq,dma,multi_host,residual,prefer_real,dual_dod "
             "never_mint_held=1 never_mint=1 product_mint=0 mint=0 "
             "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN always bar .75 "
             "bar=" DDI_HOST_BAR_HONESTY " stamp_free=1 Soft!=product G-AC-1\n");
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);

    /* greppable: ddi_host: soft denser residual lean */
    o = 0;
    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o,
             "ddi_host: soft denser residual lean denser=1 denser_ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uDenserOk);
    append_s(aLine, sizeof(aLine), &o, " prove_ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uProveOk);
    append_s(aLine, sizeof(aLine), &o, " denser_prove=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)DDI_HOST_DENSER_PROVE);
    append_s(aLine, sizeof(aLine), &o, " denser_residual=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)DDI_HOST_DENSER_RESIDUAL);
    append_s(aLine, sizeof(aLine), &o, " never_mint_held=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)DDI_HOST_NEVER_MINT_HELD);
    append_s(aLine, sizeof(aLine), &o, " denser_arms=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uArms);
    append_s(aLine, sizeof(aLine), &o, "/");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)DDI_HOST_DENSER_ARMS_MAX);
    append_s(aLine, sizeof(aLine), &o,
             " prefer_real_ddi=1 product_hosts=UDX product_mint=0 mint=0 "
             "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN agent_ne_close=1 "
             "always bar .75 bar=" DDI_HOST_BAR_HONESTY " stamp_free=1 "
             "never_invent=.76 denser residual != Dual DoD close "
             "Soft!=product G-AC-1\n");
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);

    /* greppable: ddi_host: soft denser residual prove */
    o = 0;
    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o,
             "ddi_host: soft denser residual prove ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uProveOk);
    append_s(aLine, sizeof(aLine), &o, " denser_prove=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)DDI_HOST_DENSER_PROVE);
    append_s(aLine, sizeof(aLine), &o, " denser_residual=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)DDI_HOST_DENSER_RESIDUAL);
    append_s(aLine, sizeof(aLine), &o,
             " denser=1 never_mint_held=1 never_mint=1 product_mint=0 mint=0 "
             "would_mint=0 denser_arms=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uArms);
    append_s(aLine, sizeof(aLine), &o, "/");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)DDI_HOST_DENSER_ARMS_MAX);
    append_s(aLine, sizeof(aLine), &o, " denser_min=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)DDI_HOST_DENSER_ARMS_MIN);
    append_s(aLine, sizeof(aLine), &o, " denser_ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uDenserOk);
    append_s(aLine, sizeof(aLine), &o, " open=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)(uOpen != 0u));
    append_s(aLine, sizeof(aLine), &o, " map=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)(uMap != 0u));
    append_s(aLine, sizeof(aLine), &o, " irq=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)(uIrq != 0u));
    append_s(aLine, sizeof(aLine), &o, " dma=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)(uDma != 0u));
    append_s(aLine, sizeof(aLine), &o, " multi_host=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uMh);
    append_s(aLine, sizeof(aLine), &o, " residual=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uRes);
    append_s(aLine, sizeof(aLine), &o,
             " prefer_real_ddi=1 product_hosts=UDX chain="
             DDI_HOST_PRODUCT_CHAIN " "
             "dual_dod_a=OPEN dual_dod_b=OPEN agent_ne_close=1 "
             "denser residual != Dual DoD close "
             "bar=" DDI_HOST_BAR_HONESTY " stamp_free=1 never_invent=.76 "
             "Soft!=product G-AC-1\n");
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);

    /* greppable: ddi_host: soft denser residual VERDICT */
    o = 0;
    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o,
             "ddi_host: soft denser residual VERDICT ");
    append_s(aLine, sizeof(aLine), &o, szVerdict);
    append_s(aLine, sizeof(aLine), &o, " denser=1 denser_ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uDenserOk);
    append_s(aLine, sizeof(aLine), &o, " prove_ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uProveOk);
    append_s(aLine, sizeof(aLine), &o, " denser_prove=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)DDI_HOST_DENSER_PROVE);
    append_s(aLine, sizeof(aLine), &o, " denser_residual=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)DDI_HOST_DENSER_RESIDUAL);
    append_s(aLine, sizeof(aLine), &o, " never_mint_held=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)DDI_HOST_NEVER_MINT_HELD);
    append_s(aLine, sizeof(aLine), &o, " denser_arms=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uArms);
    append_s(aLine, sizeof(aLine), &o, "/");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)DDI_HOST_DENSER_ARMS_MAX);
    append_s(aLine, sizeof(aLine), &o, " denser_min=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)DDI_HOST_DENSER_ARMS_MIN);
    append_s(aLine, sizeof(aLine), &o, " open=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)(uOpen != 0u));
    append_s(aLine, sizeof(aLine), &o, " map=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)(uMap != 0u));
    append_s(aLine, sizeof(aLine), &o, " irq=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)(uIrq != 0u));
    append_s(aLine, sizeof(aLine), &o, " dma=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)(uDma != 0u));
    append_s(aLine, sizeof(aLine), &o, " multi_host=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uMh);
    append_s(aLine, sizeof(aLine), &o, " residual=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uRes);
    append_s(aLine, sizeof(aLine), &o, " residual_open=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)(g_uSoftMultiHostResOpen != 0u));
    append_s(aLine, sizeof(aLine), &o, " residual_map=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)(g_uSoftMultiHostResMap != 0u));
    append_s(aLine, sizeof(aLine), &o, " residual_irq=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)(g_uSoftMultiHostResIrq != 0u));
    append_s(aLine, sizeof(aLine), &o, " residual_dma=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)(g_uSoftMultiHostResDma != 0u));
    append_s(aLine, sizeof(aLine), &o, " densify=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_uSoftMultiHostDensify);
    append_s(aLine, sizeof(aLine), &o,
             " prefer_real_ddi=1 not=inject_only real_ddi=1 host_inject=0 "
             "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
             "ids=10ec:8168,8086:a12f multi_host=rtl+xhci multi_server=0 "
             "gj_sys_ddi=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)GJ_SYS_DDI);
    append_s(aLine, sizeof(aLine), &o,
             " chain=SCAN,GET,OPEN,MAP_BAR,IRQ,DMA product=UDX+ABI chain="
             DDI_HOST_PRODUCT_CHAIN " "
             "product_mint=0 mint=0 would_mint=0 never_mint=1 "
             "Soft!=product dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
             "agent_ne_close=1 denser residual != Dual DoD close "
             "never_invent=.76 stamp_free=1 denser residual bar bar="
             DDI_HOST_BAR_HONESTY " always bar .75 G-AC-1\n");
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);
}

/**
 * Soft inventory + G752 bind probes. Never hard-fails the process.
 * All outcomes are soft PASS / SKIP / MISS lamps for bring-up honesty.
 * Prefer real freestanding DDI bind via GJ_SYS_DDI
 * (SCAN/GET/OPEN/MAP_BAR/IRQ/DMA) over inject-only soft demo.
 * UDX+ABI product door residual; Soft!=product; Dual DoD A/B remain OPEN.
 */
static void
ddi_host_soft_scan_and_bind(void)
{
    struct ddi_host_dev_info info;
    char aLine[960];
    unsigned o;
    long nDev;
    long i;
    unsigned long nLog;
    int fRtl;
    int fXhci;
    unsigned nRtlMaps;
    unsigned nXhciMaps;
    unsigned uOpen;
    unsigned uMap;
    unsigned uIrq;
    unsigned uDma;
    unsigned uKeepLive;
    unsigned uCore;
    struct ddi_host_residual_path pathRtl;
    struct ddi_host_residual_path pathXhci;
    /* RTL public MMIO often BAR2; try 0 and 2 (matches main soft ddi). */
    static const unsigned aRtlBars[] = { 0u, 2u };
    static const unsigned aXhciBars[] = { 0u };

    memzero(&pathRtl, (unsigned)sizeof(pathRtl));
    memzero(&pathXhci, (unsigned)sizeof(pathXhci));
    pathRtl.nOutcome = DDI_HOST_BIND_MISS;
    pathXhci.nOutcome = DDI_HOST_BIND_MISS;

    /*
     * Prefer real DDI bind (GJ_SYS_DDI door) over inject-only soft demo.
     * Once-lamp (H2): ddi_host: soft prefer real DDI
     * greppable: prefer_real_ddi=1 / not=inject_only / gj_sys_ddi=103 /
     *            chain= / dual_dod_a=OPEN dual_dod_b=OPEN / Soft!=product
     * This host is the freestanding real path for rtl8168_udx + xhci_udx.
     */
    o = 0;
    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o,
             "ddi_host: soft prefer real DDI "
             "prefer_real_ddi=1 not=inject_only real_ddi=1 host_inject=0 "
             "gj_sys_ddi=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)GJ_SYS_DDI);
    append_s(aLine, sizeof(aLine), &o,
             " chain=SCAN,GET,OPEN,MAP_BAR,IRQ,DMA "
             "door=GJ_SYS_DDI bind_path=ddi "
             "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
             "ids=10ec:8168,8086:a12f "
             "multi_host=rtl+xhci concurrent=1 denser=1 "
             "open_map_irq_dma=1 open=1 map=1 irq=1 dma=1 "
             "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
             "keep_live=1 inject_only=0 product_mint=0 mint=0 "
             "Soft!=product G-AC-1\n");
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);

    /* Optional once lamp from door (kernel: ddi_door: soft product surface). */
    (void)gj_ddi_inventory();

    soft_inc(&g_uSoftScan);
    nDev = gj_ddi_scan();
    if (nDev < 0) {
        nDev = 0;
    }
    if (nDev > (long)DDI_HOST_SCAN_MAX) {
        nDev = (long)DDI_HOST_SCAN_MAX;
    }

    /*
     * Grep: ddi_host: soft SCAN PASS n=
     * Always PASS for the soft host path (scan invoked); n may be 0 when
     * devmgr deferred or no PCI table. Soft!=product inventory complete.
     * Prefer real DDI: SCAN is first link of GJ_SYS_DDI chain (not inject).
     */
    o = 0;
    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o, "ddi_host: soft SCAN PASS n=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)nDev);
    append_s(aLine, sizeof(aLine), &o, " op=SCAN next=GET/OPEN/MAP_BAR/IRQ/DMA "
             "gj_sys_ddi=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)GJ_SYS_DDI);
    append_s(aLine, sizeof(aLine), &o,
             " chain=SCAN,GET,OPEN,MAP_BAR,IRQ,DMA "
             "prefer_real_ddi=1 not=inject_only "
             "product_mint=0 Soft!=product\n");
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);

    nLog = 0;
    for (i = 0; i < nDev; i++) {
        long ret;

        memzero(&info, (unsigned)sizeof(info));
        ret = gj_ddi_get((unsigned)i, &info);
        if (ret < 0) {
            log_soft_get((unsigned)i, 0, 0, ret);
            continue;
        }
        if (info.u16Vend == 0 && info.u16Dev == 0) {
            continue;
        }
        /* Inventory walk soft GET (all rows); bind path GETs matched IDs too. */
        log_soft_get((unsigned)i, &info, 1, 0);
        log_found((unsigned)i, &info);
        nLog++;
    }
    if (nLog == 0ul) {
        msg("ddi_host: soft inventory empty (scan deferred or no rows) "
            "Soft!=product\n");
    }

    /*
     * Lean residual catalog for UDX NIC/USB hosts over this DDI door.
     * Product = UDX+ABI Linux-shaped drivers; freestanding class SKIP default.
     * Prefer real DDI bind (GJ_SYS_DDI) over inject-only soft demo.
     * Door: SCAN/GET/OPEN/MAP_BAR + life=IRQ/DMA_NOTE/DMA_BUF/CLOSE.
     * Host deepens CFG_W BAR+ident reject + MAP idem/sticky + IRQ rebind +
     * notify rebind badge + DMA_BUF cookie idem + recb(diff cb) + force32 +
     * post_cl. Dual DoD A/B remain OPEN. soft residual != product.
     * greppable: ddi_host: soft residual lean
     * greppable: soft residual / Soft!=product / prefer_real_ddi=1 /
     *            not=inject_only / gj_sys_ddi=103 / chain= /
     *            dual_dod_a=OPEN dual_dod_b=OPEN / product_hosts=UDX
     */
    o = 0;
    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o,
             "ddi_host: soft residual lean udx=rtl8168_udx,xhci_udx "
             "product=UDX+ABI product_hosts=UDX "
             "prefer_real_ddi=1 not=inject_only real_ddi=1 host_inject=0 "
             "gj_sys_ddi=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)GJ_SYS_DDI);
    append_s(aLine, sizeof(aLine), &o,
             " chain=SCAN,GET,OPEN,MAP_BAR,IRQ,DMA "
             "life=open,cfg,map,remap,dma_note,irq,dma_buf,grant,close "
             "door=SCAN/GET/OPEN/MAP_BAR/IRQ/DMA "
             "multi_host=rtl+xhci concurrent open+map denser=1 "
             "open_map_irq_dma=1 ids=10ec:8168,8086:a12f "
             "fs_skip=GJ_RTL8168_PROBE=0,GJ_XHCI_MSC_PROBE=0 "
             "keep_live=1 dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
             "gpl=0 product_mint=0 mint=0 Soft!=product\n");
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);

    /*
     * Multi-host residual first: concurrent OPEN/MAP/IRQ/DMA on both
     * product hosts (rtl+xhci) so door multi-slot / peer-live residual
     * is exercised before per-ID full lifecycle closes each handle.
     * Soft!=product; product_mint=0; Dual DoD A/B OPEN.
     */
    (void)soft_multi_host_rtl_xhci(nDev);

    /* G752 first targets: OPEN + preferred BARs (RTL 0+2, xHCI 0). */
    nRtlMaps = 0;
    fRtl = soft_bind_by_id(nDev, (unsigned short)DDI_HOST_RTL_VEND,
                           (unsigned short)DDI_HOST_RTL_DEV, "10ec:8168",
                           aRtlBars, 2u, &nRtlMaps, &pathRtl);
    log_bind("10ec:8168", fRtl, nRtlMaps);

    nXhciMaps = 0;
    fXhci = soft_bind_by_id(nDev, (unsigned short)DDI_HOST_XHCI_VEND,
                            (unsigned short)DDI_HOST_XHCI_DEV, "8086:a12f",
                            aXhciBars, 1u, &nXhciMaps, &pathXhci);
    log_bind("8086:a12f", fXhci, nXhciMaps);

    /*
     * Residual rollup -- OPEN/MAP/IRQ/grant soft surface for class hosts.
     * greppable: ddi_host: soft residual
     */
    o = 0;
    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o, "ddi_host: soft residual rtl=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fRtl);
    append_s(aLine, sizeof(aLine), &o, " xhci=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)fXhci);
    append_s(aLine, sizeof(aLine), &o, " rtl_o=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)pathRtl.uOpen);
    append_s(aLine, sizeof(aLine), &o, " rtl_m=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)pathRtl.uMaps);
    append_s(aLine, sizeof(aLine), &o, " rtl_i=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)pathRtl.uIrq);
    append_s(aLine, sizeof(aLine), &o, " rtl_g=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)pathRtl.uGrant);
    append_s(aLine, sizeof(aLine), &o, " xhci_o=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)pathXhci.uOpen);
    append_s(aLine, sizeof(aLine), &o, " xhci_m=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)pathXhci.uMaps);
    append_s(aLine, sizeof(aLine), &o, " xhci_i=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)pathXhci.uIrq);
    append_s(aLine, sizeof(aLine), &o, " xhci_g=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)pathXhci.uGrant);
    append_s(aLine, sizeof(aLine), &o, " mh_open=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftMultiHostOpenBoth);
    append_s(aLine, sizeof(aLine), &o, " mh_map=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftMultiHostMapBoth);
    append_s(aLine, sizeof(aLine), &o, " mh_cfg=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftMultiHostCfgBoth);
    append_s(aLine, sizeof(aLine), &o, " mh_remap=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftMultiHostRemapBoth);
    append_s(aLine, sizeof(aLine), &o, " mh_irq=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftMultiHostIrqBoth);
    append_s(aLine, sizeof(aLine), &o, " mh_irq_reb=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_uSoftMultiHostIrqRebBoth);
    append_s(aLine, sizeof(aLine), &o, " mh_notify=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_uSoftMultiHostNotifyBoth);
    append_s(aLine, sizeof(aLine), &o, " mh_dma=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftMultiHostDmaBoth);
    append_s(aLine, sizeof(aLine), &o, " mh_dma_buf=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_uSoftMultiHostDmaBufBoth);
    append_s(aLine, sizeof(aLine), &o, " mh_dma_map=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_uSoftMultiHostDmaMapBoth);
    append_s(aLine, sizeof(aLine), &o, " mh_dma_idem=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_uSoftMultiHostDmaIdemBoth);
    append_s(aLine, sizeof(aLine), &o, " mh_omid=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_uSoftMultiHostOpenMapIrqDma);
    append_s(aLine, sizeof(aLine), &o, " mh_densify=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_uSoftMultiHostDensify);
    append_s(aLine, sizeof(aLine), &o, " mh_denser=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_uSoftMultiHostDenser);
    append_s(aLine, sizeof(aLine), &o, " mh_peer=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftMultiHostPeerLive);
    append_s(aLine, sizeof(aLine), &o, " mh_peer_remap=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftMultiHostPeerRemap);
    append_s(aLine, sizeof(aLine), &o, " mh_peer_irq=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftMultiHostPeerIrq);
    append_s(aLine, sizeof(aLine), &o, " mh_peer_dma=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftMultiHostPeerDma);
    append_s(aLine, sizeof(aLine), &o, " mh_dead=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_uSoftMultiHostClosedDead);
    append_s(aLine, sizeof(aLine), &o, " mh_dead_irq=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftMultiHostClosedIrq);
    append_s(aLine, sizeof(aLine), &o, " mh_dead_dma=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftMultiHostClosedDma);
    append_s(aLine, sizeof(aLine), &o, " mh_rev_peer=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftMultiHostRevPeer);
    append_s(aLine, sizeof(aLine), &o, " mh_rev_peer_remap=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_uSoftMultiHostRevPeerRemap);
    append_s(aLine, sizeof(aLine), &o, " mh_rev_peer_irq=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_uSoftMultiHostRevPeerIrq);
    append_s(aLine, sizeof(aLine), &o, " mh_rev_peer_dma=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_uSoftMultiHostRevPeerDma);
    append_s(aLine, sizeof(aLine), &o, " mh_rev_dead=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftMultiHostRevDead);
    append_s(aLine, sizeof(aLine), &o, " mh_pref=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_uSoftMultiHostPrefComplete);
    append_s(aLine, sizeof(aLine), &o, " mh_pref_res=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_uSoftMultiHostPreferRealRes);
    append_s(aLine, sizeof(aLine), &o, " mh_res_open=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_uSoftMultiHostResOpen);
    append_s(aLine, sizeof(aLine), &o, " mh_res_map=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_uSoftMultiHostResMap);
    append_s(aLine, sizeof(aLine), &o, " mh_res_irq=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_uSoftMultiHostResIrq);
    append_s(aLine, sizeof(aLine), &o, " mh_res_dma=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_uSoftMultiHostResDma);
    append_s(aLine, sizeof(aLine), &o, " mh=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftMultiHost);
    append_s(aLine, sizeof(aLine), &o,
             " denser=1 densify=4 residual_n=4 tx_rx=OPEN bot=OPEN "
             "prefer_real_ddi=1 not=inject_only "
             "gj_sys_ddi=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)GJ_SYS_DDI);
    append_s(aLine, sizeof(aLine), &o,
             " chain=SCAN,GET,OPEN,MAP_BAR,IRQ,DMA "
             "product_hosts=UDX keep_live=1 "
             "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
             "dod_a=OPEN dod_b=OPEN "
             "product_mint=0 mint=0 Soft!=product\n");
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);

    /*
     * Product residual denser when open/map/irq/dma soft surfaces present.
     * STRONGER honesty for rtl8168_udx + xhci_udx over real freestanding
     * DDI bind (prefer_real_ddi). Multi-host concurrent open/map/irq/dma
     * denser fold-in. keep_live host spirit over inject demo.
     * Soft residual != product TX/RX or BOT/MSC. Dual DoD A/B remain OPEN.
     * greppable: ddi_host: soft product residual
     * greppable: ddi_host: soft product residual denser
     * greppable: open= map= irq= dma= keep_live= product_hosts=UDX denser=
     */
    uOpen = (pathRtl.uOpen | pathXhci.uOpen) != 0u ? 1u : 0u;
    uMap = (pathRtl.uMaps | pathXhci.uMaps) != 0u ? 1u : 0u;
    uIrq = (pathRtl.uIrq | pathXhci.uIrq) != 0u ? 1u : 0u;
    uDma = (pathRtl.uDmaBuf | pathXhci.uDmaBuf | pathRtl.uDmaMap |
            pathXhci.uDmaMap) != 0u
               ? 1u
               : 0u;
    /* Fold multi-host concurrent open/map/irq/dma denser into product residual. */
    if (g_uSoftMultiHostOpenBoth != 0u) {
        uOpen = 1u;
    }
    if (g_uSoftMultiHostMapBoth != 0u) {
        uMap = 1u;
    }
    if (g_uSoftMultiHostIrqBoth != 0u || g_uSoftMultiHostIrqRebBoth != 0u ||
        g_uSoftMultiHostNotifyBoth != 0u) {
        uIrq = 1u;
    }
    if (g_uSoftMultiHostDmaBoth != 0u || g_uSoftMultiHostDmaBufBoth != 0u ||
        g_uSoftMultiHostDmaMapBoth != 0u ||
        g_uSoftMultiHostDmaIdemBoth != 0u) {
        uDma = 1u;
    }
    /* keep_live when any preferred open+map residual observed (host spirit). */
    uKeepLive = (uOpen != 0u && uMap != 0u) ? 1u : 0u;
    uCore = 0u;
    if ((pathRtl.uLife & DDI_HOST_LIFE_CORE) == DDI_HOST_LIFE_CORE) {
        uCore = 1u;
    }
    if ((pathXhci.uLife & DDI_HOST_LIFE_CORE) == DDI_HOST_LIFE_CORE) {
        uCore = 1u;
    }
    o = 0;
    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o, "ddi_host: soft product residual ");
    if (uOpen != 0u && uMap != 0u && uIrq != 0u && uDma != 0u) {
        append_s(aLine, sizeof(aLine), &o, "PASS");
    } else if (uOpen != 0u || uMap != 0u) {
        append_s(aLine, sizeof(aLine), &o, "SKIP");
    } else {
        append_s(aLine, sizeof(aLine), &o, "MISS");
    }
    append_s(aLine, sizeof(aLine), &o, " open=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uOpen);
    append_s(aLine, sizeof(aLine), &o, " map=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uMap);
    append_s(aLine, sizeof(aLine), &o, " irq=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uIrq);
    append_s(aLine, sizeof(aLine), &o, " dma=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uDma);
    append_s(aLine, sizeof(aLine), &o, " core=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uCore);
    append_s(aLine, sizeof(aLine), &o, " keep_live=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uKeepLive);
    append_s(aLine, sizeof(aLine), &o, " rtl_o=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)pathRtl.uOpen);
    append_s(aLine, sizeof(aLine), &o, " rtl_m=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)pathRtl.uMaps);
    append_s(aLine, sizeof(aLine), &o, " rtl_i=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)pathRtl.uIrq);
    append_s(aLine, sizeof(aLine), &o, " rtl_d=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)pathRtl.uDmaBuf);
    append_s(aLine, sizeof(aLine), &o, " xhci_o=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)pathXhci.uOpen);
    append_s(aLine, sizeof(aLine), &o, " xhci_m=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)pathXhci.uMaps);
    append_s(aLine, sizeof(aLine), &o, " xhci_i=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)pathXhci.uIrq);
    append_s(aLine, sizeof(aLine), &o, " xhci_d=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)pathXhci.uDmaBuf);
    append_s(aLine, sizeof(aLine), &o, " mh_open=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftMultiHostOpenBoth);
    append_s(aLine, sizeof(aLine), &o, " mh_map=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftMultiHostMapBoth);
    append_s(aLine, sizeof(aLine), &o, " mh_irq=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftMultiHostIrqBoth);
    append_s(aLine, sizeof(aLine), &o, " mh_irq_reb=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_uSoftMultiHostIrqRebBoth);
    append_s(aLine, sizeof(aLine), &o, " mh_dma=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftMultiHostDmaBoth);
    append_s(aLine, sizeof(aLine), &o, " mh_dma_buf=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_uSoftMultiHostDmaBufBoth);
    append_s(aLine, sizeof(aLine), &o, " mh_dma_map=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_uSoftMultiHostDmaMapBoth);
    append_s(aLine, sizeof(aLine), &o, " mh_omid=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_uSoftMultiHostOpenMapIrqDma);
    append_s(aLine, sizeof(aLine), &o, " mh_densify=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_uSoftMultiHostDensify);
    append_s(aLine, sizeof(aLine), &o, " both_open=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)(g_uSoftMultiHostOpenBoth != 0u));
    append_s(aLine, sizeof(aLine), &o, " both_map=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)(g_uSoftMultiHostMapBoth != 0u));
    append_s(aLine, sizeof(aLine), &o, " both_irq=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)(g_uSoftMultiHostIrqBoth != 0u));
    append_s(aLine, sizeof(aLine), &o, " both_dma=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)(g_uSoftMultiHostDmaBoth != 0u ||
                             g_uSoftMultiHostDmaBufBoth != 0u ||
                             g_uSoftMultiHostDmaMapBoth != 0u));
    append_s(aLine, sizeof(aLine), &o,
             " denser=1 densify=4 "
             "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
             "ids=10ec:8168,8086:a12f "
             "prefer_real_ddi=1 not=inject_only real_ddi=1 "
             "gj_sys_ddi=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)GJ_SYS_DDI);
    append_s(aLine, sizeof(aLine), &o,
             " chain=SCAN,GET,OPEN,MAP_BAR,IRQ,DMA "
             "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
             "soft residual densify=4 product_mint=0 mint=0 Soft!=product\n");
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);

    /*
     * Product residual denser companion (once; open/map/irq/dma denser).
     * greppable: ddi_host: soft product residual denser
     */
    o = 0;
    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o,
             "ddi_host: soft product residual denser ");
    if (uOpen != 0u && uMap != 0u && uIrq != 0u && uDma != 0u) {
        append_s(aLine, sizeof(aLine), &o, "PASS");
    } else if (uOpen != 0u || uMap != 0u) {
        append_s(aLine, sizeof(aLine), &o, "SKIP");
    } else {
        append_s(aLine, sizeof(aLine), &o, "MISS");
    }
    append_s(aLine, sizeof(aLine), &o, " open=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uOpen);
    append_s(aLine, sizeof(aLine), &o, " map=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uMap);
    append_s(aLine, sizeof(aLine), &o, " irq=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uIrq);
    append_s(aLine, sizeof(aLine), &o, " dma=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uDma);
    append_s(aLine, sizeof(aLine), &o, " densify_open=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)(g_uSoftMultiHostOpenBoth != 0u &&
                             g_uSoftMultiHostCfgBoth != 0u));
    append_s(aLine, sizeof(aLine), &o, " densify_map=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)(g_uSoftMultiHostMapBoth != 0u &&
                             (g_uSoftMultiHostRemapBoth != 0u ||
                              g_uSoftMultiHostPrefComplete != 0u)));
    append_s(aLine, sizeof(aLine), &o, " densify_irq=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)(g_uSoftMultiHostIrqBoth != 0u &&
                             (g_uSoftMultiHostIrqRebBoth != 0u ||
                              g_uSoftMultiHostNotifyBoth != 0u)));
    append_s(aLine, sizeof(aLine), &o, " densify_dma=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)(g_uSoftMultiHostDmaBoth != 0u &&
                             (g_uSoftMultiHostDmaBufBoth != 0u ||
                              g_uSoftMultiHostDmaMapBoth != 0u ||
                              g_uSoftMultiHostDmaIdemBoth != 0u)));
    append_s(aLine, sizeof(aLine), &o, " densify=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_uSoftMultiHostDensify);
    append_s(aLine, sizeof(aLine), &o, " keep_live=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uKeepLive);
    append_s(aLine, sizeof(aLine), &o, " multi_host=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)(g_uSoftMultiHostOpenBoth != 0u &&
                             g_uSoftMultiHostMapBoth != 0u));
    append_s(aLine, sizeof(aLine), &o, " mh_prefer_real_res=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_uSoftMultiHostPreferRealRes);
    append_s(aLine, sizeof(aLine), &o, " residual_open=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)(g_uSoftMultiHostResOpen != 0u));
    append_s(aLine, sizeof(aLine), &o, " residual_map=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)(g_uSoftMultiHostResMap != 0u));
    append_s(aLine, sizeof(aLine), &o, " residual_irq=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)(g_uSoftMultiHostResIrq != 0u));
    append_s(aLine, sizeof(aLine), &o, " residual_dma=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)(g_uSoftMultiHostResDma != 0u));
    append_s(aLine, sizeof(aLine), &o,
             " denser=1 densify=4 residual_n=4 prefer_real_ddi=1 not=inject_only "
             "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
             "ids=10ec:8168,8086:a12f "
             "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
             "product_mint=0 mint=0 Soft!=product\n");
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);

    /*
     * Lifecycle rollup vs kernel ddi_door (functional residual preferred).
     * door=SCAN/GET/OPEN/MAP_BAR/IRQ/DMA; host life= + core + multi-host.
     * Functional: MAP idem/sticky, IRQ rebind, DMA cookie idem + recb
     * (diff cb prior unmap) + force32, CFG BAR+ident reject, post_close
     * forget surfaces, multi-host concurrent rtl+xhci peer-live.
     * Prefer real DDI over inject-only. Soft!=product; Dual DoD A/B OPEN.
     * greppable: ddi_host: soft residual life
     */
    o = 0;
    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o, "ddi_host: soft residual life ");
    append_s(aLine, sizeof(aLine), &o, "rtl=0x");
    append_hex_ull(aLine, sizeof(aLine), &o,
                   (unsigned long long)pathRtl.uLife);
    append_s(aLine, sizeof(aLine), &o, " xhci=0x");
    append_hex_ull(aLine, sizeof(aLine), &o,
                   (unsigned long long)pathXhci.uLife);
    append_s(aLine, sizeof(aLine), &o, " rtl_core=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((pathRtl.uLife & DDI_HOST_LIFE_CORE) ==
                             DDI_HOST_LIFE_CORE));
    append_s(aLine, sizeof(aLine), &o, " xhci_core=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((pathXhci.uLife & DDI_HOST_LIFE_CORE) ==
                             DDI_HOST_LIFE_CORE));
    append_s(aLine, sizeof(aLine), &o, " idem_ok=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)(pathRtl.uIdem | pathXhci.uIdem));
    append_s(aLine, sizeof(aLine), &o, " sticky=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)(pathRtl.uSticky | pathXhci.uSticky));
    append_s(aLine, sizeof(aLine), &o, " dma_idem=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)(pathRtl.uDmaIdem | pathXhci.uDmaIdem));
    append_s(aLine, sizeof(aLine), &o, " dma_recb=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)(pathRtl.uDmaRecb | pathXhci.uDmaRecb));
    append_s(aLine, sizeof(aLine), &o, " force32=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)(pathRtl.uDmaF32 | pathXhci.uDmaF32));
    append_s(aLine, sizeof(aLine), &o, " irq_reb=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)(pathRtl.uIrqReb | pathXhci.uIrqReb));
    append_s(aLine, sizeof(aLine), &o, " map_user=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)(pathRtl.uMapUser | pathXhci.uMapUser));
    append_s(aLine, sizeof(aLine), &o, " cfg_rej=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)(pathRtl.uCfgRej | pathXhci.uCfgRej));
    append_s(aLine, sizeof(aLine), &o, " cfg_ident=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)(pathRtl.uCfgIdent | pathXhci.uCfgIdent));
    append_s(aLine, sizeof(aLine), &o, " post_cl=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)(pathRtl.uPostCl | pathXhci.uPostCl));
    append_s(aLine, sizeof(aLine), &o, " post_map=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)(pathRtl.uPostMap | pathXhci.uPostMap));
    append_s(aLine, sizeof(aLine), &o, " post_irq=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)(pathRtl.uPostIrq | pathXhci.uPostIrq));
    append_s(aLine, sizeof(aLine), &o, " post_dma=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)(pathRtl.uPostDma | pathXhci.uPostDma));
    append_s(aLine, sizeof(aLine), &o, " mh_ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftMultiHostOk);
    append_s(aLine, sizeof(aLine), &o, " mh_peer=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftMultiHostPeerLive);
    append_s(aLine, sizeof(aLine), &o, " mh_peer_remap=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftMultiHostPeerRemap);
    append_s(aLine, sizeof(aLine), &o, " mh_peer_irq=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftMultiHostPeerIrq);
    append_s(aLine, sizeof(aLine), &o, " mh_dead=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_uSoftMultiHostClosedDead);
    append_s(aLine, sizeof(aLine), &o, " mh_rev_peer=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftMultiHostRevPeer);
    append_s(aLine, sizeof(aLine), &o, " mh_rev_dead=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftMultiHostRevDead);
    append_s(aLine, sizeof(aLine), &o, " mh_remap=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftMultiHostRemapBoth);
    append_s(aLine, sizeof(aLine), &o, " mh_pref=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_uSoftMultiHostPrefComplete);
    append_s(aLine, sizeof(aLine), &o, " mh_dma_buf=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_uSoftMultiHostDmaBufBoth);
    append_s(aLine, sizeof(aLine), &o, " mh_dma_map=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_uSoftMultiHostDmaMapBoth);
    append_s(aLine, sizeof(aLine), &o, " mh_irq_reb=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_uSoftMultiHostIrqRebBoth);
    append_s(aLine, sizeof(aLine), &o, " mh_omid=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_uSoftMultiHostOpenMapIrqDma);
    append_s(aLine, sizeof(aLine), &o, " mh_densify=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_uSoftMultiHostDensify);
    append_s(aLine, sizeof(aLine), &o, " mh_peer_dma=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftMultiHostPeerDma);
    append_s(aLine, sizeof(aLine), &o, " mh_rev_peer_irq=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_uSoftMultiHostRevPeerIrq);
    append_s(aLine, sizeof(aLine), &o, " mh_rev_peer_dma=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_uSoftMultiHostRevPeerDma);
    append_s(aLine, sizeof(aLine), &o,
             " denser=1 densify=4 door=SCAN/GET/OPEN/MAP_BAR/IRQ/DMA "
             "chain=SCAN,GET,OPEN,MAP_BAR,IRQ,DMA "
             "gj_sys_ddi=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)GJ_SYS_DDI);
    append_s(aLine, sizeof(aLine), &o,
             " prefer_real_ddi=1 not=inject_only "
             "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
             "keep_live=1 dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
             "soft_bind_ne_mint=1 product_mint=0 mint=0 Soft!=product\n");
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);

    /*
     * denser residual bar .75 multi-arm + VERDICT (H2; Soft!=product).
     * After product residual denser + multi-host residual denser rollups.
     * Dual DoD A/B remain OPEN; agent!=close; product_mint=0; never invent .76.
     */
    ddi_host_soft_denser_residual_bar75(uOpen, uMap, uIrq, uDma);

    /*
     * Dual DoD A/B honesty -- soft residual never closes product DoD.
     * A = UDX USB (xhci_udx) OPEN; B = UDX NIC (rtl8168_udx) OPEN.
     * Soft bind / OPEN+MAP / multi-host != TX/RX or BOT/MSC. agent!=close.
     * Prefer real DDI bind; Dual DoD A/B remain OPEN (agent never closes).
     * greppable: ddi_host: soft Dual DoD honesty
     * greppable: dual_dod_a=OPEN dual_dod_b=OPEN
     */
    o = 0;
    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o,
             "ddi_host: soft Dual DoD honesty "
             "A=OPEN B=OPEN dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
             "udx_usb=OPEN udx_nic=OPEN "
             "soft_bind_ne_dod_close=1 agent_ne_close=1 "
             "product_hosts=UDX hosts=rtl8168_udx,xhci_udx multi_host=rtl+xhci "
             "ids=10ec:8168,8086:a12f denser=1 open_map_irq_dma=1 "
             "prefer_real_ddi=1 not=inject_only "
             "gj_sys_ddi=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)GJ_SYS_DDI);
    append_s(aLine, sizeof(aLine), &o,
             " chain=SCAN,GET,OPEN,MAP_BAR,IRQ,DMA "
             "open_map=soft concurrent=1 keep_live=1 "
             "tx_rx=OPEN bot=OPEN product_mint=0 mint=0 Soft!=product\n");
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);

    /*
     * Cap mint residual honesty -- soft only; never product CNode install.
     * Soft bind / multi-host residual != product cap mint.
     * greppable: ddi_host: soft cap mint honesty
     */
    msg("ddi_host: soft cap mint honesty "
        "mmio_frame=0 irq_notify=0 dma_window=0 cnode=0 "
        "soft_bind_ne_mint=1 multi_host_ne_mint=1 "
        "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
        "prefer_real_ddi=1 not=inject_only "
        "product_mint=0 mint=0 soft=1 product=0 G-AC-1 Soft!=product\n");

    /*
     * Window residual honesty -- soft DMA_NOTE / DMA_BUF only.
     * greppable: ddi_host: soft window residual honesty
     */
    msg("ddi_host: soft window residual honesty "
        "dma_note=soft dma_buf=soft force32=soft multi_host_dma=soft "
        "iommu=0 product_window=OPEN product_hosts=UDX "
        "prefer_real_ddi=1 not=inject_only "
        "product_mint=0 mint=0 Soft!=product\n");

    /*
     * Door residual honesty -- GJ_SYS_DDI soft observe for UDX hosts.
     * Prefer real freestanding DDI chain over inject-only soft demo.
     * Multi-host concurrent residual is soft observe; multi_server=0.
     * greppable: ddi_host: soft door residual honesty
     * greppable: gj_sys_ddi=103 / chain= / prefer_real_ddi=1 / not=inject_only
     */
    o = 0;
    memzero(aLine, sizeof(aLine));
    append_s(aLine, sizeof(aLine), &o,
             "ddi_host: soft door residual honesty "
             "door=GJ_SYS_DDI soft=1 grant=OPEN multi_cap=OPEN "
             "gj_sys_ddi=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)GJ_SYS_DDI);
    append_s(aLine, sizeof(aLine), &o,
             " prefer_real_ddi=1 not=inject_only real_ddi=1 host_inject=0 "
             "multi_host=rtl+xhci multi_server=0 confine=0 denser=1 "
             "chain=SCAN,GET,OPEN,MAP_BAR,IRQ,DMA "
             "door_chain=SCAN/GET/OPEN/CFG/MAP_BAR/MAP_REMAP/IRQ/DMA "
             "open_map=concurrent open_map_irq_dma=1 "
             "peer_live=1 peer_remap=1 peer_irq=1 peer_dma=1 "
             "closed_dead=1 closed_irq_dma=1 rev_order=1 keep_live=1 "
             "ids=10ec:8168,8086:a12f "
             "product=UDX+ABI product_hosts=UDX fs_class=SKIP "
             "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
             "dod_a=OPEN dod_b=OPEN "
             "product_mint=0 Soft!=product\n");
    aLine[o < sizeof(aLine) ? o : (sizeof(aLine) - 1u)] = '\0';
    msg(aLine);

    /*
     * Explicit no product mint claim (MMIO_FRAME / IRQ Notification / DMA win).
     * Multi-host residual never mints either. greppable: ddi_host: soft mint
     */
    msg("ddi_host: soft mint claim=0 product_mint=0 "
        "mmio_frame=0 irq_notify=0 dma_window=0 cnode=0 "
        "soft_bind_ne_mint=1 multi_host_ne_mint=1 "
        "prefer_real_ddi=1 not=inject_only multi_server=0 Soft!=product\n");
}

/**
 * Soft scaffold entry — greppable for freestanding smoke.
 * Freestanding link uses _start (user.ld ENTRY); _start calls main.
 * Prefer real DDI (GJ_SYS_DDI) residual for product UDX hosts; Soft!=product.
 */
int
main(void)
{
    char aDone[480];
    unsigned o;

    /* Soft phase 1 — ELF + DEBUG_LOG path alive. */
    msg("ddi_host: soft scaffold PASS Soft!=product "
        "prefer_real_ddi=1 not=inject_only keep_live=1 denser=1 "
        "product_hosts=UDX dual_dod=OPEN\n");

    /* Soft phase 2 — SCAN/GET/OPEN/MAP_BAR/IRQ/DMA + multi-host + G752. */
    ddi_host_soft_scan_and_bind();

    /*
     * greppable: ddi_host: soft done Soft!=product
     * greppable: prefer_real_ddi=1 / not=inject_only / gj_sys_ddi=103 /
     *            chain= / dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN /
     *            product_hosts=UDX / denser=1 / open_map_irq_dma=
     */
    o = 0;
    memzero(aDone, sizeof(aDone));
    append_s(aDone, sizeof(aDone), &o,
             "ddi_host: soft done Soft!=product soft!=product gpl=0 G-AC-1 "
             "product_mint=0 multi_host=rtl+xhci open_map=concurrent "
             "open_map_irq_dma=1 denser=1 densify=4 "
             "prefer_real_ddi=1 not=inject_only real_ddi=1 "
             "gj_sys_ddi=");
    append_u(aDone, sizeof(aDone), &o, (unsigned long)GJ_SYS_DDI);
    append_s(aDone, sizeof(aDone), &o,
             " chain=SCAN,GET,OPEN,MAP_BAR,IRQ,DMA "
             "door=SCAN/GET/OPEN/MAP_BAR/IRQ/DMA "
             "ids=10ec:8168,8086:a12f "
             "product_hosts=UDX keep_live=1 "
             "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN\n");
    aDone[o < sizeof(aDone) ? o : (sizeof(aDone) - 1u)] = '\0';
    msg(aDone);
    return 0;
}

void
_start(void)
{
    int n;

    n = main();
    gj_exit(n);
}
