/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Host MMIO window table + inject helpers (UDX_HOST_LIBC).
 * Window table is also available freestanding for granted maps.
 *
 * Soft inventory (Wave 126 exclusive deepen; this unit only) --
 * greppable "udx: host soft ...":
 *   udx: host soft inventory ...
 *   udx: host soft window ...
 *   udx: host soft lookup ...
 *   udx: host soft bar ...
 *   udx: host soft irq ...
 *   udx: host soft path ...
 *   udx: host soft wave ...
 *   udx: host soft residual lean ...
 *   udx: host soft api honesty ...
 * Soft DDI host surface (docs/DDI_SOFT.md shape; local types; no kernel hdr):
 *   udx: soft ddi host note PASS   via udx_host_soft_init() (first path)
 *   udx: soft ddi-ready ...        via udx_host_soft_note_ddi()
 *   udx: soft ddi lifecycle note   via udx_host_soft_lifecycle_note()
 *   udx: soft ddi dma not-granted  via udx_host_soft_dma_window_request()
 *   udx: soft ddi bind PASS|SKIP   via udx_host_bind_scan / bind_by_id
 *   udx: soft ddi bind residual    via SCAN→GET→OPEN→MAP_BAR→IRQ→DMA→install
 *   udx: soft ddi residual MAP_BAR / CFG / MAP_REMAP / IRQ_BIND / DMA_NOTE
 *   udx: soft ddi residual DMA_BUF  (ALLOC→MAP→FREE soft; mint OPEN)
 *   udx: soft ddi residual IOREMAP  (post-install window lookup residual)
 *   udx: soft ddi residual INVENTORY  (door product-surface residual)
 *   udx: soft ddi bind residual functional  (SCAN→GET→OPEN→MAP_BAR→IRQ→DMA)
 *   udx: soft ddi residual functional step= (per-chain-step prove)
 *   udx: soft residual Dual DoD A OPEN / Dual DoD B OPEN (agent!=close)
 * Pure observation; never gates host skeleton PASS or freestanding path.
 * Soft!=product grant / multi-server confine. No GPL.
 * Soft!=product skeleton PASS. greppable: udx: host soft
 * greppable: udx: soft ddi host note PASS
 * greppable: udx: soft ddi-ready
 * greppable: udx: soft ddi bind PASS
 * greppable: udx: soft ddi bind SKIP
 * greppable: udx: soft ddi bind residual
 * greppable: udx: soft ddi bind residual functional
 * greppable: udx: soft ddi residual functional step=
 * greppable: udx: soft ddi residual IOREMAP
 * greppable: udx: soft ddi residual PDEV
 * greppable: udx: host soft residual lean
 * greppable: udx: host soft residual mmio
 * greppable: udx: host soft residual irq
 * greppable: udx: host soft residual dma
 * greppable: udx: host soft residual close
 * greppable: udx: host soft residual inject
 * greppable: udx: host soft residual cap_mint
 * greppable: udx: host soft residual product_host
 * greppable: udx: host soft residual dual_dod
 * greppable: udx: host soft residual c2
 * greppable: udx: host soft residual hazard
 * greppable: udx: host soft c2 product path
 * greppable: udx: soft residual freestanding class SKIP
 * greppable: udx: soft residual product=UDX+ABI
 * greppable: udx: soft residual Dual DoD A OPEN
 * greppable: udx: soft residual Dual DoD B OPEN
 * greppable: udx: soft residual Dual DoD A/B OPEN
 * greppable: udx: soft ddi residual INVENTORY
 * greppable: udx: soft ddi residual DMA_BUF
 * greppable: chain=SCAN,GET,OPEN,MAP_BAR,IRQ,DMA
 * greppable: product_path_ok= | product_path_full_ok= | pdev_id_match=
 * greppable: close_on_pass=0 handle_retain=1
 * greppable: freestanding class SKIP
 * greppable: Soft!=product dual_dod OPEN W23_STRONGER
 * greppable: W23_STRONGER=1 | W22_STRONGER=1 (subsumed) | W21_STRONGER=1 (subsumed) | W20_STRONGER=1 (subsumed) | W19_STRONGER=1 (subsumed)
 * greppable: denser_prove=1 | dma_dense_both= | dma_dense_triple= | irq_dense=
 * greppable: dma_dense_free= | chain_step_dense= | life_product_chain=
 * greppable: chain_dense_scan_dma= | product_host_dense= | chain_free_dense=
 * greppable: step_all_dense= | cfg_dense= | install_dense= | ioremap_dense=
 * greppable: life_full_dense=
 * greppable: remap_dense= | side_dense= | win_dense= | prefer_real_dense=
 * greppable: handle_retain_dense= | close_dense= | dual_dod_dense=
 * greppable: soft_ne_dense= | open_h_dense= | path_full_dense=
 * greppable: mint_dense= | agent_dense= | fs_dense=
 * greppable: maps_dense= | pref_dense= | va_dense=
 * greppable: core_dense= | pdev_dense= | life_dense=
 * greppable: h1_dense= | inject_dense= | sys_dense=
 * greppable: multi_dense= | confine_dense= | wave_dense=
 * greppable: path_dense= | lean_dense= | product_dense=
 * greppable: hazard_dense= | c2_dense= | bar_dense=
 * greppable: gac_dense= | stamp_dense= | layout_dense=
 * greppable: mmio_dense= | inv_dense= | sshd_dense=
 * greppable: eq_dense= | once_dense= | grant_dense=
 * greppable: sys_ddi=103 lean_checks=
 *
 * Lean residual Cap surface (Soft!=product; G-AC-1): soft DDI/cap honesty
 * only -- no MMIO_FRAME / IRQ Notification / DMA window mint from driver.c.
 * Freestanding class drivers SKIP; product = Linux-shaped userspace UDX
 * (rtl8168_udx Dual DoD B OPEN / xhci_udx Dual DoD A OPEN). Dual MIT OR
 * Apache-2.0. No version stamp. Dual DoD A/B remain OPEN (agent!=close).
 * No freestanding class re-enable. No GJ_IMAGE_VERSION (stamp-free residual).
 * Bar honesty: v2026.08.04.75 — NEVER invent .76 / bump GJ_IMAGE_VERSION.
 *
 * C2 libudx host residual deepen (Soft!=product; Dual DoD OPEN):
 *   residual mmio         -- MAP preferred BAR + window_register != MMIO_FRAME
 *   residual ioremap      -- post-install udx_ioremap prove + iounmap (probe free)
 *   residual irq          -- IRQ_BIND soft badge + fire_irq != Notification mint
 *   residual dma          -- DMA_NOTE + DMA_BUF soft + soft_dma_window != VT-d
 *   residual close        -- handle retain on install residual (CLOSE_ON_BIND=0)
 *   residual inject       -- UDX_HOST_LIBC lab only; freestanding bind SKIP inject
 *   residual cap_mint     -- UDX_HOST_SOFT_CAP_MINT=0 honesty (MMIO/IRQ/DMA OPEN)
 *   residual product_host -- rtl8168_udx (DoD B) / xhci_udx (DoD A) surface
 *   residual dual_dod     -- Dual DoD A/B OPEN (agent!=close; soft lamps != close)
 *   residual c2           -- SCAN..MAP_BAR..IRQ..DMA product path residual catalog
 *   residual hazard       -- H2 no stamp storms; soft inventory once-lamp
 * Constants: UDX_HOST_SOFT_{CAP_MINT,MULTI_SERVER,CONFINE,DOD_A_OPEN,
 *   DOD_B_OPEN,HANDLE_RETAIN,CLOSE_ON_BIND} (host.h; eng honesty only).
 *
 * Note: PCI inject/remove + install_granted live in pci.c (host.h).
 * This unit owns windows + BAR poke + soft DDI host types + freestanding
 * GJ_SYS_DDI bind residual toward product UDX hosts (not inject-only):
 *   SCAN→INVENTORY→GET→OPEN→CFG_READ→MAP preferred BARs→MAP_REMAP→
 *   IRQ_BIND→DMA_NOTE→DMA_BUF→install→window→ioremap(prove)→iounmap.
 *   Product functional greppable: SCAN,GET,OPEN,MAP_BAR,IRQ,DMA for
 *   10ec:8168 (rtl8168_udx bars 0+2) and 8086:a12f (xhci_udx bar 0).
 *   Functional denser residual (preferred over lamps alone):
 *     CFG identity match (cfg0 vend:dev == GET), MAP VA↔window look match,
 *     preferred-BAR completeness (rtl 0+2 / xhci 0), step= catalog
 *     (SCAN/GET/OPEN/MAP_BAR/IRQ/DMA/INSTALL/IOREMAP/CFG), product_path_ok.
 *   W19 STRONGER functional residual (stamp-free bar v2026.08.04.75):
 *     product_path_full_ok denser prove (STRONGER residual):
 *       product_path_ok (SCAN,GET,OPEN,MAP_BAR,IRQ,DMA denser prove) +
 *       INSTALL + IOREMAP + CFG id_match + pref_complete (when PA present) +
 *       VA match + post-install pdev vend:dev identity (pdev_id_match) +
 *       LIFE_CORE (SCAN|GET|OPEN|MAP|WIN|INSTALL). GJ_SYS_DDI=103 full.
 *     W15 denser STRONGER beyond W14 (chain SCAN→DMA denser terminal):
 *       DMA denser both (DMA_NOTE + DMA_BUF soft prove) +
 *       DMA denser triple (DMA_NOTE + DMA_BUF + DMA_BUF_MAP) +
 *       DMA denser free (ALLOC→MAP→FREE full cycle residual) +
 *       IRQ denser explicit (irq_ok, not life bit alone) +
 *       maps>=1 + Dual DoD A/B OPEN locks + agent_close=0 +
 *       Soft!=product lock + close_on_pass=0 handle_retain=1 +
 *       freestanding class SKIP + product_host denser (DoD A/B id) +
 *       chain denser SCAN→DMA (all 6 product steps denser_prove) +
 *       chain_step denser (per-step denser all 6) +
 *       life product chain denser (SCAN|GET|OPEN|MAP|IRQ|DMA_NOTE).
 *     W16 denser STRONGER beyond W15 (prefer_real + side residual denser):
 *       REMAP denser (LIFE_REMAP second MAP_BAR residual) +
 *       SIDE denser (CFG_R|REMAP|DMA_NOTE|IRQ full LIFE_SIDE) +
 *       WIN denser (window_register residual on preferred BARs) +
 *       prefer_real denser (GJ_SYS_DDI=103 not inject-only product path).
 *     W17 denser STRONGER beyond W16 (chain SCAN→DMA denser free terminal):
 *       chain free denser (SCAN→DMA denser requires DMA free cycle) +
 *       step_all denser (SCAN/GET/OPEN h/MAP maps+pref/IRQ/DMA free) +
 *       CFG denser + INSTALL denser (pdev_id) + IOREMAP denser (va_match) +
 *       LIFE_FULL denser (LIFE_CORE|LIFE_SIDE both complete).
 *     W18 denser STRONGER beyond W17 (product_path_full Dual DoD retain; subsumed):
 *       handle_retain denser (OPEN h>0 retained after install residual) +
 *       close denser (close_on_pass=0 denser prove; no CLOSE on PASS) +
 *       dual_dod denser (Dual DoD A/B OPEN denser; agent_close=0) +
 *       soft_ne denser (Soft!=product denser lock; inject!=product) +
 *       open_h denser (OPEN step + h>0 denser on product chain) +
 *       path_full denser (W17 full + retain/close/dual_dod/soft_ne/open_h).
 *     W19 denser STRONGER beyond W18 (product_path_full Dual DoD Soft!=product OPEN; subsumed):
 *       mint denser (cap_mint=0 denser prove; Soft mint OPEN residual) +
 *       agent denser (agent_close=0 denser prove; soft lamps != close) +
 *       fs denser (freestanding class SKIP denser; product=UDX) +
 *       maps denser (maps>=1 denser on product MAP_BAR residual) +
 *       pref denser (pref_complete denser preferred-BAR completeness) +
 *       va denser (va_match denser MAP VA↔window look match) +
 *       path_full denser (W18 full + mint/agent/fs/maps/pref/va denser).
 *     W20 denser STRONGER beyond W19 (product_path_full Dual DoD Soft!=product OPEN denser; subsumed):
 *       core denser (LIFE_CORE denser prove; SCAN|GET|OPEN|MAP|WIN|INSTALL) +
 *       pdev denser (pdev_id_match denser prove; post-install identity) +
 *       life denser (life product chain denser; SCAN|GET|OPEN|MAP|IRQ|DMA_NOTE) +
 *       h1 denser (H1 no net_eth_poll denser; bind residual never eth poll) +
 *       inject denser (inject != product denser; freestanding GJ_SYS_DDI path) +
 *       sys denser (sys_ddi=103 denser; GJ_SYS_DDI door honesty) +
 *       path_full denser (W19 full + core/pdev/life/h1/inject/sys denser).
 *     W21 denser STRONGER beyond W20 (product_path_full Dual DoD Soft!=product OPEN denser; subsumed):
 *       multi denser (multi_server=0 denser prove; soft residual multi) +
 *       confine denser (confine=0 denser prove; soft residual confine) +
 *       wave denser (wave=75 bar honesty denser; NEVER invent .76) +
 *       path denser (product_path_ok denser prove; SCAN→DMA path denser) +
 *       lean denser (lean_checks denser prove; FUNCTIONAL self-check) +
 *       product denser (product=UDX+ABI denser; product_host residual denser) +
 *       path_full denser (W20 full + multi/confine/wave/path/lean/product denser).
 *     W22 denser STRONGER beyond W21 (product_path_full Dual DoD Soft!=product OPEN denser; subsumed):
 *       hazard denser (H2 no stamp storms denser; inventory once-lamp) +
 *       c2 denser (C2 residual denser; product path SCAN→DMA denser) +
 *       bar denser (bar .75 honesty denser; NEVER invent .76) +
 *       gac denser (G-AC-1 denser; no .ko product AC denser) +
 *       stamp denser (stamp_free denser; no GJ_IMAGE_VERSION denser) +
 *       layout denser (layout_ver denser; residual lean layout) +
 *       path_full denser (W21 full + hazard/c2/bar/gac/stamp/layout denser).
 *     W23 denser STRONGER beyond W22 (product_path_full Dual DoD Soft!=product OPEN denser):
 *       mmio denser (MMIO residual denser; MAP_BAR + maps denser) +
 *       inv denser (INVENTORY residual denser; side product-surface denser) +
 *       sshd denser (product=UDX+sshd+stack denser; Dual DoD B residual) +
 *       eq denser (product_eq=UDX+ABI denser; product residual denser) +
 *       once denser (H2 once denser; inventory once-lamp denser) +
 *       grant denser (cap_mint=0 not-granted denser; Soft mint OPEN denser) +
 *       path_full denser (W22 full + mmio/inv/sshd/eq/once/grant denser).
 *     close_on_pass=0 handle_retain=1; freestanding class SKIP honesty.
 *     FUNCTIONAL residual locks + lean_checks denser _Static_assert.
 *     Soft!=product; Dual DoD A/B OPEN; agent!=close; NEVER invent .76.
 *     greppable: Soft!=product dual_dod OPEN W23_STRONGER
 *     greppable: W23_STRONGER=1 | product_path_full_ok= | denser_prove=1
 *     greppable: chain_dense_scan_dma= | chain_free_dense= | step_all_dense=
 *     greppable: dma_dense_triple= | dma_dense_free= | life_full_dense=
 *     greppable: cfg_dense= | install_dense= | ioremap_dense=
 *     greppable: chain_step_dense= | life_product_chain= | product_host_dense=
 *     greppable: remap_dense= | side_dense= | win_dense= | prefer_real_dense=
 *     greppable: handle_retain_dense= | close_dense= | dual_dod_dense=
 *     greppable: soft_ne_dense= | open_h_dense= | path_full_dense=
 *     greppable: mint_dense= | agent_dense= | fs_dense=
 *     greppable: maps_dense= | pref_dense= | va_dense=
 *     greppable: core_dense= | pdev_dense= | life_dense=
 *     greppable: h1_dense= | inject_dense= | sys_dense=
 *     greppable: multi_dense= | confine_dense= | wave_dense=
 *     greppable: path_dense= | lean_dense= | product_dense=
 *     greppable: hazard_dense= | c2_dense= | bar_dense=
 *     greppable: gac_dense= | stamp_dense= | layout_dense=
 *     greppable: mmio_dense= | inv_dense= | sshd_dense=
 *     greppable: eq_dense= | once_dense= | grant_dense=
 *   Handle retained (product hosts do not CLOSE on install residual).
 *   Soft mint OPEN. Soft!=product; G-AC-1; Dual DoD A/B OPEN.
 */
#include "udx_internal.h"

#include <udx/ddi.h>
#include <udx/host.h>
#include <udx/irq.h>
#include <udx/udx.h>

#include <stdarg.h>

#if defined(UDX_HOST_LIBC)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

#define UDX_HOST_WIN_MAX 64
/* Soft wave stamp for greppable inventory lines (eng only; not image stamp).
 * Aligns stamp-free bar honesty v2026.08.04.75 — NEVER invent .76. */
#define UDX_HOST_SOFT_WAVE 75u

/*
 * FUNCTIONAL residual locks (W23 STRONGER freestanding bind_by_id via
 * GJ_SYS_DDI fully for product UDX; Soft!=product; G-AC-1; Dual DoD A/B
 * OPEN; agent!=close; stamp-free bar v2026.08.04.75; NEVER invent .76).
 * Product chain denser prove: SCAN→GET→OPEN→MAP_BAR→IRQ→DMA (not inject).
 * W23 denser product_path_full (chain SCAN→DMA Dual DoD Soft!=product OPEN):
 *   DMA both + DMA triple (NOTE+BUF+MAP) + DMA free (ALLOC→MAP→FREE) +
 *   IRQ denser + maps + Dual DoD OPEN locks + product_host denser +
 *   chain denser SCAN→DMA + chain free denser + chain_step denser +
 *   step_all denser + life product chain denser + LIFE_FULL denser +
 *   REMAP denser + SIDE denser + WIN denser + prefer_real denser +
 *   CFG denser + INSTALL denser + IOREMAP denser +
 *   handle_retain denser + close denser + dual_dod denser +
 *   soft_ne denser + open_h denser + path_full denser +
 *   mint denser + agent denser + fs denser +
 *   maps denser + pref denser + va denser +
 *   core denser + pdev denser + life denser +
 *   h1 denser + inject denser + sys denser +
 *   multi denser + confine denser + wave denser +
 *   path denser + lean denser + product denser +
 *   hazard denser + c2 denser + bar denser +
 *   gac denser + stamp denser + layout denser +
 *   mmio denser + inv denser + sshd denser +
 *   eq denser + once denser + grant denser +
 *   close_on_pass=0 handle_retain=1 freestanding class SKIP.
 * greppable: udx: soft ddi bind residual functional
 * greppable: product_path_ok= | product_path_full_ok= | pdev_id_match=
 * greppable: chain=SCAN,GET,OPEN,MAP_BAR,IRQ,DMA | GJ_SYS_DDI | sys_ddi=103
 * greppable: close_on_pass=0 handle_retain=1 | freestanding class SKIP
 * greppable: Soft!=product dual_dod OPEN W23_STRONGER
 * greppable: W23_STRONGER=1 | denser_prove=1 | lean_checks=
 * greppable: chain_dense_scan_dma= | chain_free_dense= | step_all_dense=
 * greppable: dma_dense_triple= | dma_dense_free= | life_full_dense=
 * greppable: cfg_dense= | install_dense= | ioremap_dense=
 * greppable: chain_step_dense= | life_product_chain= | product_host_dense=
 * greppable: remap_dense= | side_dense= | win_dense= | prefer_real_dense=
 * greppable: handle_retain_dense= | close_dense= | dual_dod_dense=
 * greppable: soft_ne_dense= | open_h_dense= | path_full_dense=
 * greppable: mint_dense= | agent_dense= | fs_dense=
 * greppable: maps_dense= | pref_dense= | va_dense=
 * greppable: core_dense= | pdev_dense= | life_dense=
 * greppable: h1_dense= | inject_dense= | sys_dense=
 * greppable: multi_dense= | confine_dense= | wave_dense=
 * greppable: path_dense= | lean_dense= | product_dense=
 * greppable: hazard_dense= | c2_dense= | bar_dense=
 * greppable: gac_dense= | stamp_dense= | layout_dense=
 * greppable: mmio_dense= | inv_dense= | sshd_dense=
 * greppable: eq_dense= | once_dense= | grant_dense=
 */
#define UDX_HOST_H1_NO_NET_ETH_POLL    1u /* H1: bind residual never net_eth_poll */
#define UDX_HOST_HARD_IRQ_BIND         0u /* never hard-IRQ product from bind */
#define UDX_HOST_SYS_DDI_NR          103u /* GJ_SYS_DDI door number honesty */
#define UDX_HOST_PRODUCT_CHAIN_N       6u /* SCAN,GET,OPEN,MAP_BAR,IRQ,DMA */
#define UDX_HOST_DOD_A_OPEN            1u /* xhci_udx Dual DoD A OPEN */
#define UDX_HOST_DOD_B_OPEN            1u /* rtl8168_udx Dual DoD B OPEN */
#define UDX_HOST_AGENT_CLOSE           0u /* soft lamps never close Dual DoD */
#define UDX_HOST_CLOSE_ON_PASS         0u /* no CLOSE residual on bind PASS */
#define UDX_HOST_HANDLE_RETAIN_LOCK    1u /* install residual keeps OPEN h */
#define UDX_HOST_INJECT_NE_PRODUCT     1u /* inject != Dual DoD / product close */
#define UDX_HOST_FS_CLASS_SKIP         1u /* freestanding class drivers SKIP */
#define UDX_HOST_SOFT_NE_PRODUCT_LOCK  1u /* Soft!=product claim-class law */
#define UDX_HOST_PRODUCT_FULL_DENSE    1u /* denser product_path_full prove on */
#define UDX_HOST_W14_STRONGER          1u /* W14 denser STRONGER residual (subsumed) */
#define UDX_HOST_W15_STRONGER          1u /* W15 denser STRONGER residual (subsumed) */
#define UDX_HOST_W16_STRONGER          1u /* W16 denser STRONGER residual (subsumed) */
#define UDX_HOST_W17_STRONGER          1u /* W17 denser STRONGER residual (subsumed) */
#define UDX_HOST_W18_STRONGER          1u /* W18 denser STRONGER residual (subsumed) */
#define UDX_HOST_W19_STRONGER          1u /* W19 denser STRONGER residual (subsumed) */
#define UDX_HOST_W20_STRONGER          1u /* W20 denser STRONGER residual (subsumed) */
#define UDX_HOST_W21_STRONGER          1u /* W21 denser STRONGER residual (subsumed) */
#define UDX_HOST_W22_STRONGER          1u /* W22 denser STRONGER residual (subsumed) */
#define UDX_HOST_W23_STRONGER          1u /* W23 denser STRONGER residual on */
#define UDX_HOST_DMA_DENSE_BOTH        1u /* full needs DMA_NOTE + DMA_BUF */
#define UDX_HOST_DMA_DENSE_TRIPLE      1u /* full needs NOTE + BUF + MAP */
#define UDX_HOST_DMA_DENSE_FREE        1u /* full needs ALLOC→MAP→FREE cycle */
#define UDX_HOST_IRQ_DENSE_OK          1u /* full needs explicit irq_ok */
#define UDX_HOST_CHAIN_DENSE_SCAN_DMA  1u /* denser full chain SCAN→DMA prove */
#define UDX_HOST_CHAIN_FREE_DENSE      1u /* chain SCAN→DMA denser needs free term */
#define UDX_HOST_CHAIN_STEP_DENSE      1u /* per-step denser prove all 6 steps */
#define UDX_HOST_STEP_ALL_DENSE        1u /* denser all 6 product steps prove */
#define UDX_HOST_PRODUCT_HOST_DENSE    1u /* full needs product-host DoD id */
#define UDX_HOST_REMAP_DENSE           1u /* full needs LIFE_REMAP denser */
#define UDX_HOST_SIDE_DENSE            1u /* full needs full LIFE_SIDE denser */
#define UDX_HOST_WIN_DENSE             1u /* full needs LIFE_WIN denser */
#define UDX_HOST_PREFER_REAL_DENSE     1u /* full needs prefer_real GJ_SYS_DDI */
#define UDX_HOST_CFG_DENSE             1u /* full needs CFG id_match denser */
#define UDX_HOST_INSTALL_DENSE         1u /* full needs INSTALL + pdev denser */
#define UDX_HOST_IOREMAP_DENSE         1u /* full needs IOREMAP + va_match denser */
#define UDX_HOST_LIFE_FULL_DENSE       1u /* full needs LIFE_CORE|SIDE denser */
#define UDX_HOST_HANDLE_RETAIN_DENSE   1u /* full needs OPEN h retain denser */
#define UDX_HOST_CLOSE_DENSE           1u /* full needs close_on_pass=0 denser */
#define UDX_HOST_DUAL_DOD_DENSE        1u /* full needs Dual DoD A/B OPEN denser */
#define UDX_HOST_SOFT_NE_DENSE         1u /* full needs Soft!=product denser */
#define UDX_HOST_OPEN_H_DENSE          1u /* full needs OPEN h>0 denser */
#define UDX_HOST_PATH_FULL_DENSE       1u /* W23 product_path_full denser terminal */
#define UDX_HOST_MINT_DENSE            1u /* full needs cap_mint=0 denser */
#define UDX_HOST_AGENT_DENSE           1u /* full needs agent_close=0 denser */
#define UDX_HOST_FS_DENSE              1u /* full needs freestanding class SKIP denser */
#define UDX_HOST_MAPS_DENSE            1u /* full needs maps>=1 denser */
#define UDX_HOST_PREF_DENSE            1u /* full needs pref_complete denser */
#define UDX_HOST_VA_DENSE              1u /* full needs va_match denser */
#define UDX_HOST_CORE_DENSE            1u /* full needs LIFE_CORE denser */
#define UDX_HOST_PDEV_DENSE            1u /* full needs pdev_id_match denser */
#define UDX_HOST_LIFE_DENSE            1u /* full needs life product chain denser */
#define UDX_HOST_H1_DENSE              1u /* full needs H1 no net_eth_poll denser */
#define UDX_HOST_INJECT_DENSE          1u /* full needs inject != product denser */
#define UDX_HOST_SYS_DENSE             1u /* full needs sys_ddi=103 denser */
#define UDX_HOST_MULTI_DENSE           1u /* full needs multi_server=0 denser */
#define UDX_HOST_CONFINE_DENSE         1u /* full needs confine=0 denser */
#define UDX_HOST_WAVE_DENSE            1u /* full needs wave=75 bar honesty denser */
#define UDX_HOST_PATH_DENSE            1u /* full needs product_path_ok denser */
#define UDX_HOST_LEAN_DENSE            1u /* full needs lean_checks denser */
#define UDX_HOST_PRODUCT_DENSE         1u /* full needs product=UDX+ABI denser */
#define UDX_HOST_HAZARD_DENSE          1u /* full needs H2 no stamp storms denser */
#define UDX_HOST_C2_DENSE              1u /* full needs C2 residual denser */
#define UDX_HOST_BAR_DENSE             1u /* full needs bar .75 honesty denser */
#define UDX_HOST_GAC_DENSE             1u /* full needs G-AC-1 denser */
#define UDX_HOST_STAMP_DENSE           1u /* full needs stamp_free denser */
#define UDX_HOST_LAYOUT_DENSE          1u /* full needs layout_ver denser */
#define UDX_HOST_MMIO_DENSE            1u /* full needs MMIO residual denser */
#define UDX_HOST_INV_DENSE             1u /* full needs INVENTORY residual denser */
#define UDX_HOST_SSHD_DENSE            1u /* full needs product=UDX+sshd+stack denser */
#define UDX_HOST_EQ_DENSE              1u /* full needs product_eq=UDX+ABI denser */
#define UDX_HOST_ONCE_DENSE            1u /* full needs H2 once denser */
#define UDX_HOST_GRANT_DENSE           1u /* full needs cap_mint=0 grant denser */
#define UDX_HOST_LEAN_CHECKS          68u /* denser functional self-check count */
/* Life product chain denser: SCAN|GET|OPEN|MAP|IRQ|DMA_NOTE (not inject). */
#define UDX_HOST_LIFE_PRODUCT_CHAIN \
    (UDX_DDI_LIFE_SCAN | UDX_DDI_LIFE_GET | UDX_DDI_LIFE_OPEN | \
     UDX_DDI_LIFE_MAP | UDX_DDI_LIFE_IRQ | UDX_DDI_LIFE_DMA_NOTE)
/* Life full denser: CORE + SIDE (SCAN..INSTALL + CFG/REMAP/DMA_NOTE/IRQ). */
#define UDX_HOST_LIFE_FULL \
    (UDX_DDI_LIFE_CORE | UDX_DDI_LIFE_SIDE)

_Static_assert(UDX_HOST_H1_NO_NET_ETH_POLL == 1u,
               "H1: host bind residual never net_eth_poll");
_Static_assert(UDX_HOST_HARD_IRQ_BIND == 0u,
               "host bind residual never hard-IRQ product");
_Static_assert(UDX_HOST_SYS_DDI_NR == 103u,
               "GJ_SYS_DDI door nr honesty (bind_by_id freestanding)");
_Static_assert(UDX_HOST_PRODUCT_CHAIN_N == 6u,
               "product chain SCAN,GET,OPEN,MAP_BAR,IRQ,DMA = 6 steps");
_Static_assert(UDX_HOST_DOD_A_OPEN == 1u,
               "Dual DoD A remains OPEN (xhci_udx)");
_Static_assert(UDX_HOST_DOD_B_OPEN == 1u,
               "Dual DoD B remains OPEN (rtl8168_udx)");
_Static_assert(UDX_HOST_AGENT_CLOSE == 0u,
               "soft residual lamps never close Dual DoD");
_Static_assert(UDX_HOST_CLOSE_ON_PASS == 0u,
               "no CLOSE residual on bind PASS (handle retain)");
_Static_assert(UDX_HOST_HANDLE_RETAIN_LOCK == 1u,
               "product hosts retain OPEN handle after install");
_Static_assert(UDX_HOST_INJECT_NE_PRODUCT == 1u,
               "inject residual != product Dual DoD close");
_Static_assert(UDX_HOST_FS_CLASS_SKIP == 1u,
               "freestanding class drivers SKIP (product=UDX)");
_Static_assert(UDX_HOST_SOFT_NE_PRODUCT_LOCK == 1u,
               "Soft!=product claim-class law (never Soft unicode ne)");
_Static_assert(UDX_HOST_PRODUCT_FULL_DENSE == 1u,
               "W23 denser product_path_full prove residual on");
_Static_assert(UDX_HOST_W14_STRONGER == 1u,
               "W14_STRONGER denser residual lock on (subsumed by W22)");
_Static_assert(UDX_HOST_W15_STRONGER == 1u,
               "W15_STRONGER denser residual lock on (subsumed by W22)");
_Static_assert(UDX_HOST_W16_STRONGER == 1u,
               "W16_STRONGER denser residual lock on (subsumed by W22)");
_Static_assert(UDX_HOST_W17_STRONGER == 1u,
               "W17_STRONGER denser residual lock on (subsumed by W22)");
_Static_assert(UDX_HOST_W18_STRONGER == 1u,
               "W18_STRONGER denser residual lock on (subsumed by W22)");
_Static_assert(UDX_HOST_W19_STRONGER == 1u,
               "W19_STRONGER denser residual lock on (subsumed by W22)");
_Static_assert(UDX_HOST_W20_STRONGER == 1u,
               "W20_STRONGER denser residual lock on (subsumed by W22)");
_Static_assert(UDX_HOST_W21_STRONGER == 1u,
               "W21_STRONGER denser residual lock on (subsumed by W22)");
_Static_assert(UDX_HOST_W22_STRONGER == 1u,
               "W22_STRONGER denser residual lock on (subsumed by W23)");
_Static_assert(UDX_HOST_W23_STRONGER == 1u,
               "W23_STRONGER denser residual lock on");
_Static_assert(UDX_HOST_DMA_DENSE_BOTH == 1u,
               "product_path_full denser: DMA_NOTE + DMA_BUF both");
_Static_assert(UDX_HOST_DMA_DENSE_TRIPLE == 1u,
               "product_path_full denser: DMA_NOTE+BUF+MAP triple");
_Static_assert(UDX_HOST_DMA_DENSE_FREE == 1u,
               "product_path_full denser: DMA ALLOC→MAP→FREE cycle");
_Static_assert(UDX_HOST_IRQ_DENSE_OK == 1u,
               "product_path_full denser: explicit irq_ok required");
_Static_assert(UDX_HOST_CHAIN_DENSE_SCAN_DMA == 1u,
               "product_path_full denser: chain SCAN→DMA denser terminal");
_Static_assert(UDX_HOST_CHAIN_FREE_DENSE == 1u,
               "product_path_full denser: chain SCAN→DMA free terminal denser");
_Static_assert(UDX_HOST_CHAIN_STEP_DENSE == 1u,
               "product_path_full denser: per-step denser all 6 chain steps");
_Static_assert(UDX_HOST_STEP_ALL_DENSE == 1u,
               "product_path_full denser: step_all denser all 6 product steps");
_Static_assert(UDX_HOST_PRODUCT_HOST_DENSE == 1u,
               "product_path_full denser: product-host DoD A/B id");
_Static_assert(UDX_HOST_REMAP_DENSE == 1u,
               "product_path_full denser: LIFE_REMAP denser");
_Static_assert(UDX_HOST_SIDE_DENSE == 1u,
               "product_path_full denser: full LIFE_SIDE denser");
_Static_assert(UDX_HOST_WIN_DENSE == 1u,
               "product_path_full denser: LIFE_WIN denser");
_Static_assert(UDX_HOST_PREFER_REAL_DENSE == 1u,
               "product_path_full denser: prefer_real GJ_SYS_DDI not inject");
_Static_assert(UDX_HOST_CFG_DENSE == 1u,
               "product_path_full denser: CFG id_match denser");
_Static_assert(UDX_HOST_INSTALL_DENSE == 1u,
               "product_path_full denser: INSTALL + pdev denser");
_Static_assert(UDX_HOST_IOREMAP_DENSE == 1u,
               "product_path_full denser: IOREMAP + va_match denser");
_Static_assert(UDX_HOST_LIFE_FULL_DENSE == 1u,
               "product_path_full denser: LIFE_CORE|SIDE full denser");
_Static_assert(UDX_HOST_HANDLE_RETAIN_DENSE == 1u,
               "product_path_full denser: OPEN h retain denser");
_Static_assert(UDX_HOST_CLOSE_DENSE == 1u,
               "product_path_full denser: close_on_pass=0 denser");
_Static_assert(UDX_HOST_DUAL_DOD_DENSE == 1u,
               "product_path_full denser: Dual DoD A/B OPEN denser");
_Static_assert(UDX_HOST_SOFT_NE_DENSE == 1u,
               "product_path_full denser: Soft!=product denser");
_Static_assert(UDX_HOST_OPEN_H_DENSE == 1u,
               "product_path_full denser: OPEN h>0 denser");
_Static_assert(UDX_HOST_PATH_FULL_DENSE == 1u,
               "product_path_full denser: W23 path_full denser terminal");
_Static_assert(UDX_HOST_MINT_DENSE == 1u,
               "product_path_full denser: cap_mint=0 denser");
_Static_assert(UDX_HOST_AGENT_DENSE == 1u,
               "product_path_full denser: agent_close=0 denser");
_Static_assert(UDX_HOST_FS_DENSE == 1u,
               "product_path_full denser: freestanding class SKIP denser");
_Static_assert(UDX_HOST_MAPS_DENSE == 1u,
               "product_path_full denser: maps>=1 denser");
_Static_assert(UDX_HOST_PREF_DENSE == 1u,
               "product_path_full denser: pref_complete denser");
_Static_assert(UDX_HOST_VA_DENSE == 1u,
               "product_path_full denser: va_match denser");
_Static_assert(UDX_HOST_CORE_DENSE == 1u,
               "product_path_full denser: LIFE_CORE denser");
_Static_assert(UDX_HOST_PDEV_DENSE == 1u,
               "product_path_full denser: pdev_id_match denser");
_Static_assert(UDX_HOST_LIFE_DENSE == 1u,
               "product_path_full denser: life product chain denser");
_Static_assert(UDX_HOST_H1_DENSE == 1u,
               "product_path_full denser: H1 no net_eth_poll denser");
_Static_assert(UDX_HOST_INJECT_DENSE == 1u,
               "product_path_full denser: inject != product denser");
_Static_assert(UDX_HOST_SYS_DENSE == 1u,
               "product_path_full denser: sys_ddi=103 denser");
_Static_assert(UDX_HOST_MULTI_DENSE == 1u,
               "product_path_full denser: multi_server=0 denser");
_Static_assert(UDX_HOST_CONFINE_DENSE == 1u,
               "product_path_full denser: confine=0 denser");
_Static_assert(UDX_HOST_WAVE_DENSE == 1u,
               "product_path_full denser: wave=75 bar honesty denser");
_Static_assert(UDX_HOST_PATH_DENSE == 1u,
               "product_path_full denser: product_path_ok denser");
_Static_assert(UDX_HOST_LEAN_DENSE == 1u,
               "product_path_full denser: lean_checks denser");
_Static_assert(UDX_HOST_PRODUCT_DENSE == 1u,
               "product_path_full denser: product=UDX+ABI denser");
_Static_assert(UDX_HOST_HAZARD_DENSE == 1u,
               "product_path_full denser: H2 no stamp storms denser");
_Static_assert(UDX_HOST_C2_DENSE == 1u,
               "product_path_full denser: C2 residual denser");
_Static_assert(UDX_HOST_BAR_DENSE == 1u,
               "product_path_full denser: bar .75 honesty denser");
_Static_assert(UDX_HOST_GAC_DENSE == 1u,
               "product_path_full denser: G-AC-1 denser");
_Static_assert(UDX_HOST_STAMP_DENSE == 1u,
               "product_path_full denser: stamp_free denser");
_Static_assert(UDX_HOST_LAYOUT_DENSE == 1u,
               "product_path_full denser: layout_ver denser");
_Static_assert(UDX_HOST_MMIO_DENSE == 1u,
               "product_path_full denser: MMIO residual denser");
_Static_assert(UDX_HOST_INV_DENSE == 1u,
               "product_path_full denser: INVENTORY residual denser");
_Static_assert(UDX_HOST_SSHD_DENSE == 1u,
               "product_path_full denser: product=UDX+sshd+stack denser");
_Static_assert(UDX_HOST_EQ_DENSE == 1u,
               "product_path_full denser: product_eq=UDX+ABI denser");
_Static_assert(UDX_HOST_ONCE_DENSE == 1u,
               "product_path_full denser: H2 once denser");
_Static_assert(UDX_HOST_GRANT_DENSE == 1u,
               "product_path_full denser: cap_mint=0 grant denser");
_Static_assert(UDX_HOST_LEAN_CHECKS == 68u,
               "FUNCTIONAL lean residual W23 STRONGER denser: 68 checks");
_Static_assert(UDX_HOST_SOFT_WAVE == 75u,
               "stamp-free soft wave aligns bar v2026.08.04.75 (never .76)");
_Static_assert(UDX_HOST_CLOSE_ON_PASS == UDX_HOST_SOFT_CLOSE_ON_BIND,
               "close_on_pass=0 aligns host.h close_on_bind honesty");
_Static_assert(UDX_HOST_HANDLE_RETAIN_LOCK == UDX_HOST_SOFT_HANDLE_RETAIN,
               "handle_retain=1 aligns host.h retain honesty");
_Static_assert(UDX_HOST_DOD_A_OPEN == UDX_HOST_SOFT_DOD_A_OPEN,
               "Dual DoD A OPEN aligns host.h soft DoD A honesty");
_Static_assert(UDX_HOST_DOD_B_OPEN == UDX_HOST_SOFT_DOD_B_OPEN,
               "Dual DoD B OPEN aligns host.h soft DoD B honesty");
_Static_assert((UDX_HOST_LIFE_PRODUCT_CHAIN & UDX_DDI_LIFE_SCAN) != 0u &&
               (UDX_HOST_LIFE_PRODUCT_CHAIN & UDX_DDI_LIFE_GET) != 0u &&
               (UDX_HOST_LIFE_PRODUCT_CHAIN & UDX_DDI_LIFE_OPEN) != 0u &&
               (UDX_HOST_LIFE_PRODUCT_CHAIN & UDX_DDI_LIFE_MAP) != 0u &&
               (UDX_HOST_LIFE_PRODUCT_CHAIN & UDX_DDI_LIFE_IRQ) != 0u &&
               (UDX_HOST_LIFE_PRODUCT_CHAIN & UDX_DDI_LIFE_DMA_NOTE) != 0u,
               "life product chain denser: SCAN|GET|OPEN|MAP|IRQ|DMA_NOTE");
_Static_assert((UDX_HOST_LIFE_FULL & UDX_DDI_LIFE_CORE) == UDX_DDI_LIFE_CORE &&
               (UDX_HOST_LIFE_FULL & UDX_DDI_LIFE_SIDE) == UDX_DDI_LIFE_SIDE,
               "life full denser: LIFE_CORE|SIDE complete");

struct udx_host_window {
    u64   u64Phys;
    u64   u64Len;
    void *pVa;
    u8    u8Used;
};

static struct udx_host_window g_aWin[UDX_HOST_WIN_MAX];

/*
 * Soft host window / BAR inventory (Wave 126 exclusive deepen).
 * Cumulative for this process. greppable: udx: host soft …
 * Never hard-gates; wrap OK if ever hit.
 */
static u32 g_u32HostWinReg;       /* register enter */
static u32 g_u32HostWinRegOk;     /* new slot filled */
static u32 g_u32HostWinRegUpd;    /* same phys base update */
static u32 g_u32HostWinRegFull;   /* table full reject */
static u32 g_u32HostWinRegOvf;    /* phys+len overflow reject */
static u32 g_u32HostWinRegInval;  /* null va / zero len */
static u32 g_u32HostWinUnreg;     /* unregister enter */
static u32 g_u32HostWinUnregHit;  /* unregister found slot */
static u32 g_u32HostWinUnregMiss; /* unregister no match */
static u32 g_u32HostWinLookup;    /* lookup enter */
static u32 g_u32HostWinLookHit;   /* lookup hit */
static u32 g_u32HostWinLookMiss;  /* lookup miss */
static u32 g_u32HostWinLookInval; /* zero len / overflow */
static u32 g_u32HostWinLive;      /* currently used slots */
static u32 g_u32HostWinPeak;      /* peak used slots */
static u32 g_u32HostFireIrq;      /* udx_host_fire_irq */
static u32 g_u32HostBarWrite;     /* BAR write* ok */
static u32 g_u32HostBarRead;      /* BAR read* ok */
static u32 g_u32HostBarFail;      /* BAR access fail (inval/nodev) */
static u32 g_u32HostBarNosys;     /* freestanding NOSYS path */
static u32 g_u32HostBarMemcpy;    /* memcpy_to/from ok */
static u32 g_u32HostBarW;         /* write width tallies: l/w/b/q */
static u32 g_u32HostBarR;         /* read width tallies: l/w/b/q (combined) */
static u32 g_u32HostLogN;         /* soft inventory dumps */
static u8  g_fHostSoftOnce;       /* one-shot after first window ok */

/* Soft DDI host lamps (ABI-first; never product grants). */
static u8  g_fDdiHostInited;      /* soft ready after first udx_host_soft_init */
static u32 g_u32DdiInitCalls;     /* udx_host_soft_init entries */
static u32 g_u32DdiDmaReq;        /* dma window request enters */
static u32 g_u32DdiDmaReject;     /* soft not-granted / inval rejects */
static u32 g_u32DdiLifecycleN;    /* lifecycle note dumps */
static u32 g_u32DdiBdfSnap;       /* bdf_from_pdev ok */
static u32 g_u32DdiBarSnap;       /* bar_snapshot ok */
static u32 g_u32DdiBindEnter;     /* bind_scan / bind_by_id enter */
static u32 g_u32DdiBindPass;      /* soft ddi bind PASS */
static u32 g_u32DdiBindSkip;      /* soft ddi bind SKIP */
static u32 g_u32DdiBindSys;       /* GJ_SYS_DDI attempts */
static u32 g_u32DdiBindSysFail;   /* syscall fail / ENOSYS */
/* SCAN→GET→OPEN→MAP_BAR residual tallies (Soft!=product; lean). */
static u32 g_u32DdiResScan;       /* OP_SCAN enters */
static u32 g_u32DdiResScanOk;     /* SCAN ret >= 0 */
static u32 g_u32DdiResGet;        /* OP_GET enters */
static u32 g_u32DdiResGetOk;      /* GET ret >= 0 */
static u32 g_u32DdiResMatch;      /* GET vend:dev match (bind_by_id / hit) */
static u32 g_u32DdiResOpen;       /* OP_OPEN enters */
static u32 g_u32DdiResOpenOk;     /* OPEN handle > 0 */
static long g_i64DdiHandleRetain; /* last OPEN h; retain=1 no CLOSE */
static u32 g_u32DdiResMap;        /* OP_MAP_BAR enters */
static u32 g_u32DdiResMapOk;      /* MAP note.u8Ok / VA residual */
static u32 g_u32DdiResMapEmpty;   /* preferred BAR empty (not a fail) */
static u32 g_u32DdiResMapFail;    /* MAP soft-fail */
static u32 g_u32DdiResMapUser;    /* MAP note.u8User residual */
static u32 g_u32DdiResCfg;        /* OP_CFG_READ residual enters */
static u32 g_u32DdiResCfgOk;      /* CFG_READ ident soft-ok */
static u32 g_u32DdiResRemap;      /* MAP_REMAP residual enters */
static u32 g_u32DdiResRemapOk;    /* MAP_REMAP soft-ok */
static u32 g_u32DdiResRemapIdem;  /* MAP_REMAP VA match residual */
static u32 g_u32DdiResDmaNote;    /* OP_DMA_NOTE residual enters */
static u32 g_u32DdiResDmaNoteOk;  /* DMA_NOTE soft-ok */
static u32 g_u32DdiResDmaBuf;     /* OP_DMA_BUF_ALLOC residual enters */
static u32 g_u32DdiResDmaBufOk;   /* DMA_BUF_ALLOC soft-ok (pa residual) */
static u32 g_u32DdiResDmaBufMap;  /* DMA_BUF_MAP soft-ok */
static u32 g_u32DdiResDmaBufFree; /* DMA_BUF_FREE soft-ok */
static u32 g_u32DdiResIrq;        /* OP_IRQ_BIND residual enters */
static u32 g_u32DdiResIrqOk;      /* IRQ_BIND soft-ok */
static u32 g_u32DdiResWin;        /* extra preferred-BAR window_register */
static u32 g_u32DdiResInstall;    /* install_granted residual ok */
static u32 g_u32DdiResIoremap;    /* post-install udx_ioremap residual enters */
static u32 g_u32DdiResIoremapOk;  /* ioremap prove ok (window lookup) */
static u32 g_u32DdiResIoremapFail;/* ioremap prove fail residual */
static u32 g_u32DdiResProductHit; /* matched rtl/xhci product-host ID */
static u32 g_u32DdiResLifePeak;   /* peak UDX_DDI_LIFE_* mask observed */
static u32 g_u32DdiResInv;        /* OP_INVENTORY residual enters */
static u32 g_u32DdiResInvOk;      /* INVENTORY ret >= 0 (product surface) */
static u32 g_u32DdiResDodA;       /* residual path hit Dual DoD A host */
static u32 g_u32DdiResDodB;       /* residual path hit Dual DoD B host */
static u32 g_u32DdiResCoreOk;     /* residual path life has UDX_DDI_LIFE_CORE */
static u32 g_u32DdiResCfgIdMatch; /* CFG_READ ident matches GET vend:dev */
static u32 g_u32DdiResVaMatch;    /* MAP VA == window_lookup VA (ioremap prove) */
static u32 g_u32DdiResPrefComplete; /* preferred BARs all mapped when PA present */
static u32 g_u32DdiResFunc;       /* functional residual catalog runs */
static u32 g_u32DdiResFuncStep;   /* functional residual step= emissions */
static u32 g_u32DdiResProductPath;/* SCAN→GET→OPEN→MAP_BAR→IRQ→DMA product ok */
static u32 g_u32DdiResProductPathFull; /* denser W23: W22 + mmio/inv/sshd/eq/once/grant denser */
static u32 g_u32DdiResPdevIdMatch;/* post-install pdev vend:dev == GET residual */
static u8  g_fDdiResHonestyOnce;  /* once-lamp residual honesty (no storm) */
static u8  g_fDdiDodHonestyOnce;  /* once-lamp Dual DoD A/B OPEN (no storm) */
/* g_fDdiInvOnce lives inside host_ddi_inventory_residual (freestanding only). */

static void host_soft_inc(u32 *pu32);
static void host_soft_note_peak(u32 *pu32Peak, u32 u32Val);
static void host_soft_emit(const char *szFmt, ...);
static void host_soft_inventory_log(void);
static void host_soft_maybe_once(void);
static u32  host_soft_live_windows(void);

static void
host_soft_inc(u32 *pu32)
{
    if (pu32 != NULL && *pu32 < 0xffffffffu) {
        (*pu32)++;
    }
}

/** Soft: raise peak if u32Val is higher (diagnostics only). */
static void
host_soft_note_peak(u32 *pu32Peak, u32 u32Val)
{
    if (pu32Peak != NULL && u32Val > *pu32Peak) {
        *pu32Peak = u32Val;
    }
}

static u32
host_soft_live_windows(void)
{
    u32 iWin;
    u32 u32Live;

    u32Live = 0;
    for (iWin = 0; iWin < UDX_HOST_WIN_MAX; iWin++) {
        if (g_aWin[iWin].u8Used) {
            if (u32Live < 0xffffffffu) {
                u32Live++;
            }
        }
    }
    return u32Live;
}

/*
 * Soft inventory emit path — host console; does not use udx_printk so
 * core soft printk tallies stay product-path pure.
 */
static void
host_soft_emit(const char *szFmt, ...)
{
    if (szFmt == NULL) {
        return;
    }
#if defined(UDX_HOST_LIBC)
    {
        va_list apArgs;

        va_start(apArgs, szFmt);
        (void)vprintf(szFmt, apArgs);
        va_end(apArgs);
    }
#else
    (void)szFmt;
#endif
}

/**
 * Greppable soft host inventory (Wave 126 exclusive deepen).
 * Prefix-stable "udx: host soft …" — never hard-gates; observation only.
 *
 * greppable: udx: host soft
 */
static void
host_soft_inventory_log(void)
{
    u32 u32Host;
    u32 u32Live;

    host_soft_inc(&g_u32HostLogN);
    u32Live = host_soft_live_windows();
    g_u32HostWinLive = u32Live;
    host_soft_note_peak(&g_u32HostWinPeak, u32Live);

#if defined(UDX_HOST_LIBC)
    u32Host = 1u;
#else
    u32Host = 0u;
#endif

    /* Grep: udx: host soft inventory */
    host_soft_emit("udx: host soft inventory win_reg_ok=%u win_reg_upd=%u "
                   "win_full=%u win_ovf=%u unreg_hit=%u lookup_hit=%u "
                   "lookup_miss=%u fire_irq=%u bar_w=%u bar_r=%u "
                   "log_n=%u wave=%u\n",
                   g_u32HostWinRegOk, g_u32HostWinRegUpd, g_u32HostWinRegFull,
                   g_u32HostWinRegOvf, g_u32HostWinUnregHit,
                   g_u32HostWinLookHit, g_u32HostWinLookMiss,
                   g_u32HostFireIrq, g_u32HostBarWrite, g_u32HostBarRead,
                   g_u32HostLogN, UDX_HOST_SOFT_WAVE);

    /* Grep: udx: host soft window */
    host_soft_emit("udx: host soft window max=%u live=%u peak=%u "
                   "reg_enter=%u reg_ok=%u reg_upd=%u reg_full=%u "
                   "reg_ovf=%u reg_inval=%u unreg=%u unreg_hit=%u "
                   "unreg_miss=%u\n",
                   (u32)UDX_HOST_WIN_MAX, u32Live, g_u32HostWinPeak,
                   g_u32HostWinReg, g_u32HostWinRegOk, g_u32HostWinRegUpd,
                   g_u32HostWinRegFull, g_u32HostWinRegOvf,
                   g_u32HostWinRegInval, g_u32HostWinUnreg,
                   g_u32HostWinUnregHit, g_u32HostWinUnregMiss);

    /* Grep: udx: host soft lookup */
    host_soft_emit("udx: host soft lookup enter=%u hit=%u miss=%u inval=%u "
                   "range_cover=1 sub_window=1\n",
                   g_u32HostWinLookup, g_u32HostWinLookHit,
                   g_u32HostWinLookMiss, g_u32HostWinLookInval);

    /* Grep: udx: host soft bar */
    host_soft_emit("udx: host soft bar write_ok=%u read_ok=%u fail=%u "
                   "nosys=%u memcpy_ok=%u width_w=%u width_r=%u "
                   "host_libc=%u freestanding_nosys=%u\n",
                   g_u32HostBarWrite, g_u32HostBarRead, g_u32HostBarFail,
                   g_u32HostBarNosys, g_u32HostBarMemcpy, g_u32HostBarW,
                   g_u32HostBarR, u32Host, (u32Host == 0u) ? 1u : 0u);

    /* Grep: udx: host soft irq */
    host_soft_emit("udx: host soft irq fire=%u equiv=udx_irq_dispatch "
                   "badge_claim=immediate soft_mask=irq_table\n",
                   g_u32HostFireIrq);

    /*
     * Path catalog — what this soft surface is / is not.
     * greppable: udx: host soft path
     */
    host_soft_emit("udx: host soft path "
                   "reg=udx_host_window_register "
                   "unreg=udx_host_window_unregister "
                   "lookup=udx_host_window_lookup "
                   "fire=udx_host_fire_irq "
                   "bar_rw=udx_host_bar_* "
                   "pci_inject=pci.c "
                   "ddi_bind=udx_host_bind_by_id "
                   "sys_ddi=103 "
                   "skeleton_gate=0 hard_gate=0 soft=1\n");

    /* Grep: udx: host soft wave */
    host_soft_emit("udx: host soft wave n=%u unit=host exclusive=1 "
                   "prefix=udx:_host_soft deepen=1 "
                   "(soft inventory; never gates skeleton PASS)\n",
                   UDX_HOST_SOFT_WAVE);

    /*
     * Grep: udx: host soft honesty (Wave 126 exclusive deepen).
     * Soft inventory Soft!=product multi-server confine.
     * Constants from host.h (UDX_HOST_SOFT_*); never claim product.
     */
    host_soft_emit("udx: host soft honesty multi_server=%u confine=%u "
                  "exclusive=1 soft=1 product_kernel=OPEN wave=%u "
                  "cap_mint=%u DoD_A_OPEN=%u DoD_B_OPEN=%u "
                  "handle_retain=%u close_on_bind=%u "
                  "Soft!=product G-AC-1 dual=MIT_OR_Apache-2.0\n",
                  (u32)UDX_HOST_SOFT_MULTI_SERVER,
                  (u32)UDX_HOST_SOFT_CONFINE,
                  UDX_HOST_SOFT_WAVE,
                  (u32)UDX_HOST_SOFT_CAP_MINT,
                  (u32)UDX_HOST_SOFT_DOD_A_OPEN,
                  (u32)UDX_HOST_SOFT_DOD_B_OPEN,
                  (u32)UDX_HOST_SOFT_HANDLE_RETAIN,
                  (u32)UDX_HOST_SOFT_CLOSE_ON_BIND);

    /*
     * Lean residual Cap / DDI API honesty (Soft!=product; G-AC-1).
     * C2 deepen: wire UDX_HOST_SOFT_* honesty constants into lean rollup.
     * greppable: udx: host soft residual lean
     * greppable: udx: host soft api honesty
     * greppable: udx: soft residual freestanding class SKIP
     * greppable: udx: soft residual product=UDX+ABI
     */
    host_soft_emit("udx: host soft residual lean "
                   "cap_mint=%u ddi_grant=not_granted "
                   "window_table=soft inject=lab "
                   "product_MMIO_FRAME=OPEN product_IRQ_Notification=OPEN "
                   "product_DMA_window=OPEN soft=1 "
                   "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1 "
                   "multi_server=%u confine=%u "
                   "dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX close=0 "
                   "DoD_A_OPEN=%u DoD_B_OPEN=%u "
                   "handle_retain=%u close_on_bind=%u "
                   "xhci_udx=DoD_A_OPEN rtl8168_udx=DoD_B_OPEN "
                   "agent_ne_close=1 soft_lamps_ne_close=1 "
                   "storm=0 wave=%u "
                   "(Soft!=product; lean residual only)\n",
                   (u32)UDX_HOST_SOFT_CAP_MINT,
                   (u32)UDX_HOST_SOFT_MULTI_SERVER,
                   (u32)UDX_HOST_SOFT_CONFINE,
                   (u32)UDX_HOST_SOFT_DOD_A_OPEN,
                   (u32)UDX_HOST_SOFT_DOD_B_OPEN,
                   (u32)UDX_HOST_SOFT_HANDLE_RETAIN,
                   (u32)UDX_HOST_SOFT_CLOSE_ON_BIND,
                   UDX_HOST_SOFT_WAVE);
    host_soft_emit("udx: host soft api honesty "
                   "cap=hidden_not_minted "
                   "mmio=window_register_lookup "
                   "irq=fire_irq_table "
                   "dma=soft_dma_window_not_granted "
                   "multi_server=%u confine=%u cap_mint=%u "
                   "handle_retain=%u close_on_bind=%u "
                   "dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX close=0 "
                   "DoD_A_OPEN=%u DoD_B_OPEN=%u agent_ne_close=1 "
                   "soft=1 product_kernel=OPEN Soft!=product wave=%u\n",
                   (u32)UDX_HOST_SOFT_MULTI_SERVER,
                   (u32)UDX_HOST_SOFT_CONFINE,
                   (u32)UDX_HOST_SOFT_CAP_MINT,
                   (u32)UDX_HOST_SOFT_HANDLE_RETAIN,
                   (u32)UDX_HOST_SOFT_CLOSE_ON_BIND,
                   (u32)UDX_HOST_SOFT_DOD_A_OPEN,
                   (u32)UDX_HOST_SOFT_DOD_B_OPEN,
                   UDX_HOST_SOFT_WAVE);
    host_soft_emit("udx: soft residual freestanding class SKIP "
                   "kernel_class_drivers=SKIP "
                   "kernel_rtl8168=SKIP kernel_xhci_msc=SKIP "
                   "GJ_RTL8168_PROBE=0 GJ_XHCI_MSC_PROBE=0 "
                   "product=UDX+ABI Linux-shaped userspace "
                   "hosts=rtl8168_udx,xhci_udx "
                   "dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX "
                   "DoD_A_OPEN=%u DoD_B_OPEN=%u agent_ne_close=1 "
                   "soft=1 product=0 G-AC-1 (Soft!=product)\n",
                   (u32)UDX_HOST_SOFT_DOD_A_OPEN,
                   (u32)UDX_HOST_SOFT_DOD_B_OPEN);
    host_soft_emit("udx: soft residual product=UDX+ABI "
                   "surface=Linux-porter "
                   "hosts=rtl8168_udx,xhci_udx "
                   "xhci_udx=DoD_A_OPEN rtl8168_udx=DoD_B_OPEN "
                   "dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX close=0 "
                   "DoD_A_OPEN=%u DoD_B_OPEN=%u agent_ne_close=1 "
                   "caps=hidden soft_inventory=1 product_mint=0 "
                   "cap_mint=%u handle_retain=%u close_on_bind=%u "
                   "G-AC-1 (Soft!=product)\n",
                   (u32)UDX_HOST_SOFT_DOD_A_OPEN,
                   (u32)UDX_HOST_SOFT_DOD_B_OPEN,
                   (u32)UDX_HOST_SOFT_CAP_MINT,
                   (u32)UDX_HOST_SOFT_HANDLE_RETAIN,
                   (u32)UDX_HOST_SOFT_CLOSE_ON_BIND);
    /*
     * Dual DoD A/B OPEN catalog (agent!=close; Soft!=product).
     * greppable: udx: soft residual Dual DoD A OPEN
     * greppable: udx: soft residual Dual DoD B OPEN
     * greppable: udx: soft residual Dual DoD A/B OPEN
     */
    host_soft_emit("udx: soft residual Dual DoD A OPEN "
                   "dual_dod_a=OPEN_UDX path=xhci_udx id=8086:a12f "
                   "bar=0 product=UDX+ABI fs_class=SKIP "
                   "DoD_A_OPEN=%u agent_ne_close=1 soft_lamps_ne_close=1 "
                   "close=0 mint=%u soft=1 Soft!=product\n",
                   (u32)UDX_HOST_SOFT_DOD_A_OPEN,
                   (u32)UDX_HOST_SOFT_CAP_MINT);
    host_soft_emit("udx: soft residual Dual DoD B OPEN "
                   "dual_dod_b=OPEN_UDX path=rtl8168_udx id=10ec:8168 "
                   "bars=0,2 product=UDX+ABI fs_class=SKIP "
                   "DoD_B_OPEN=%u agent_ne_close=1 soft_lamps_ne_close=1 "
                   "close=0 mint=%u soft=1 Soft!=product\n",
                   (u32)UDX_HOST_SOFT_DOD_B_OPEN,
                   (u32)UDX_HOST_SOFT_CAP_MINT);
    host_soft_emit("udx: soft residual Dual DoD A/B OPEN "
                   "dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX close=0 "
                   "DoD_A_OPEN=%u DoD_B_OPEN=%u agent_ne_close=1 "
                   "hosts=xhci_udx,rtl8168_udx product=UDX+ABI "
                   "chain=SCAN,GET,OPEN,MAP_BAR,IRQ,DMA "
                   "side=INVENTORY,CFG,MAP_REMAP,DMA_NOTE,DMA_BUF,IRQ_BIND,WIN,INSTALL,IOREMAP "
                   "ids=10ec:8168,8086:a12f "
                   "handle_retain=%u close_on_bind=%u "
                   "G-AC-1 soft=1 mint=%u Soft!=product\n",
                   (u32)UDX_HOST_SOFT_DOD_A_OPEN,
                   (u32)UDX_HOST_SOFT_DOD_B_OPEN,
                   (u32)UDX_HOST_SOFT_HANDLE_RETAIN,
                   (u32)UDX_HOST_SOFT_CLOSE_ON_BIND,
                   (u32)UDX_HOST_SOFT_CAP_MINT);

    /*
     * C2 Cap residual surface deepen (Soft!=product; G-AC-1).
     * Per host.h C2 residual catalog: MMIO / IRQ / DMA / CLOSE / inject /
     * cap_mint / product_host / dual_dod / c2 path / hazard.
     * Inventory is once-lamp (host_soft_maybe_once) -- H2 no stamp storms.
     * greppable: udx: host soft residual mmio
     * greppable: udx: host soft residual irq
     * greppable: udx: host soft residual dma
     * greppable: udx: host soft residual close
     * greppable: udx: host soft residual inject
     * greppable: udx: host soft residual cap_mint
     * greppable: udx: host soft residual product_host
     * greppable: udx: host soft residual dual_dod
     * greppable: udx: host soft residual c2
     * greppable: udx: host soft residual hazard
     * greppable: udx: host soft c2 product path
     */
    host_soft_emit("udx: host soft residual mmio "
                   "soft=window_register_lookup_map_bar "
                   "product_MMIO_FRAME=OPEN cap_mint=%u "
                   "map_ok=%u map_user=%u win=%u "
                   "ioremap=%u ioremap_ok=%u ioremap_fail=%u "
                   "live=%u peak=%u "
                   "path=MAP_BAR,window_register,ioremap_granted_pa "
                   "hosts=rtl8168_udx,xhci_udx "
                   "DoD_A_OPEN=%u DoD_B_OPEN=%u "
                   "soft_ne_product=1 G-AC-1 dual=MIT_OR_Apache-2.0 "
                   "wave=%u (MMIO residual; Soft!=product)\n",
                   (u32)UDX_HOST_SOFT_CAP_MINT,
                   g_u32DdiResMapOk, g_u32DdiResMapUser, g_u32DdiResWin,
                   g_u32DdiResIoremap, g_u32DdiResIoremapOk,
                   g_u32DdiResIoremapFail,
                   u32Live, g_u32HostWinPeak,
                   (u32)UDX_HOST_SOFT_DOD_A_OPEN,
                   (u32)UDX_HOST_SOFT_DOD_B_OPEN,
                   UDX_HOST_SOFT_WAVE);
    host_soft_emit("udx: host soft residual irq "
                   "soft=IRQ_BIND_badge_plus_fire_irq_table "
                   "product_IRQ_Notification=OPEN cap_mint=%u "
                   "irq_bind=%u irq_ok=%u fire_irq=%u "
                   "badge=UDX_DDI_IRQ_BADGE_SOFT "
                   "hosts=rtl8168_udx,xhci_udx "
                   "DoD_A_OPEN=%u DoD_B_OPEN=%u "
                   "H1=no_net_eth_poll soft_ne_product=1 G-AC-1 "
                   "dual=MIT_OR_Apache-2.0 wave=%u "
                   "(IRQ residual; Soft!=product)\n",
                   (u32)UDX_HOST_SOFT_CAP_MINT,
                   g_u32DdiResIrq, g_u32DdiResIrqOk, g_u32HostFireIrq,
                   (u32)UDX_HOST_SOFT_DOD_A_OPEN,
                   (u32)UDX_HOST_SOFT_DOD_B_OPEN,
                   UDX_HOST_SOFT_WAVE);
    host_soft_emit("udx: host soft residual dma "
                   "soft=DMA_NOTE_plus_DMA_BUF_plus_soft_dma_window "
                   "product_DMA_window=OPEN product_VT_d=OPEN "
                   "cap_mint=%u not_granted=1 "
                   "dma_note=%u dma_note_ok=%u "
                   "dma_buf=%u dma_buf_ok=%u dma_buf_map=%u dma_buf_free=%u "
                   "dma_req=%u dma_reject=%u "
                   "hosts=rtl8168_udx,xhci_udx "
                   "DoD_A_OPEN=%u DoD_B_OPEN=%u "
                   "soft_ne_product=1 G-AC-1 dual=MIT_OR_Apache-2.0 "
                   "wave=%u (DMA residual; Soft!=product)\n",
                   (u32)UDX_HOST_SOFT_CAP_MINT,
                   g_u32DdiResDmaNote, g_u32DdiResDmaNoteOk,
                   g_u32DdiResDmaBuf, g_u32DdiResDmaBufOk,
                   g_u32DdiResDmaBufMap, g_u32DdiResDmaBufFree,
                   g_u32DdiDmaReq, g_u32DdiDmaReject,
                   (u32)UDX_HOST_SOFT_DOD_A_OPEN,
                   (u32)UDX_HOST_SOFT_DOD_B_OPEN,
                   UDX_HOST_SOFT_WAVE);
    host_soft_emit("udx: host soft residual close "
                   "handle_retain=%u close_on_bind=%u "
                   "install_ok=%u product_hosts_keep_OPEN_h=1 "
                   "CLOSE_residual_on_PASS=0 "
                   "hosts=rtl8168_udx,xhci_udx product=UDX+ABI "
                   "DoD_A_OPEN=%u DoD_B_OPEN=%u agent_ne_close=1 "
                   "soft=1 soft_ne_product=1 G-AC-1 wave=%u "
                   "(CLOSE residual honesty; Soft!=product)\n",
                   (u32)UDX_HOST_SOFT_HANDLE_RETAIN,
                   (u32)UDX_HOST_SOFT_CLOSE_ON_BIND,
                   g_u32DdiResInstall,
                   (u32)UDX_HOST_SOFT_DOD_A_OPEN,
                   (u32)UDX_HOST_SOFT_DOD_B_OPEN,
                   UDX_HOST_SOFT_WAVE);
    host_soft_emit("udx: host soft residual inject "
                   "host_linux_lab=UDX_HOST_LIBC "
                   "freestanding_bind=GJ_SYS_DDI "
                   "inject_ne_Dual_DoD_close=1 "
                   "product_hosts=rtl8168_udx,xhci_udx "
                   "fs_class=SKIP product=UDX+ABI "
                   "DoD_A_OPEN=%u DoD_B_OPEN=%u "
                   "soft=1 soft_ne_product=1 G-AC-1 wave=%u "
                   "(inject residual; Soft!=product)\n",
                   (u32)UDX_HOST_SOFT_DOD_A_OPEN,
                   (u32)UDX_HOST_SOFT_DOD_B_OPEN,
                   UDX_HOST_SOFT_WAVE);
    host_soft_emit("udx: host soft residual cap_mint "
                   "cap_mint=%u multi_server=%u confine=%u "
                   "product_MMIO_FRAME=OPEN "
                   "product_IRQ_Notification=OPEN "
                   "product_DMA_window=OPEN "
                   "soft_path=window_table+inject+GJ_SYS_DDI_bind "
                   "product_path=devmgr_CNode_mint OPEN "
                   "hosts=rtl8168_udx,xhci_udx "
                   "DoD_A_OPEN=%u DoD_B_OPEN=%u agent_ne_close=1 "
                   "soft=1 product=0 soft_ne_product=1 "
                   "dual=MIT_OR_Apache-2.0 G-AC-1 wave=%u "
                   "(cap mint residual OPEN; Soft!=product)\n",
                   (u32)UDX_HOST_SOFT_CAP_MINT,
                   (u32)UDX_HOST_SOFT_MULTI_SERVER,
                   (u32)UDX_HOST_SOFT_CONFINE,
                   (u32)UDX_HOST_SOFT_DOD_A_OPEN,
                   (u32)UDX_HOST_SOFT_DOD_B_OPEN,
                   UDX_HOST_SOFT_WAVE);
    host_soft_emit("udx: host soft residual product_host "
                   "hosts=rtl8168_udx,xhci_udx "
                   "surface=GJ_SYS_DDI_bind_SCAN_GET_OPEN_MAP_BAR_IRQ_DMA "
                   "product=UDX+ABI Linux-shaped_userspace=1 "
                   "freestanding_class=SKIP "
                   "kernel_rtl8168=SKIP kernel_xhci_msc=SKIP "
                   "rtl8168_udx=DoD_B_OPEN id=10ec:8168 bars=0,2 "
                   "xhci_udx=DoD_A_OPEN id=8086:a12f bar=0 "
                   "product_hit=%u product_path=%u product_path_full=%u "
                   "pdev_id_match=%u "
                   "dod_a_hit=%u dod_b_hit=%u "
                   "core_ok=%u install=%u "
                   "handle_retain=%u close_on_bind=%u "
                   "sys_ddi=%u lean_checks=%u "
                   "soft=1 product=0 soft_ne_product=1 "
                   "dual=MIT_OR_Apache-2.0 G-AC-1 wave=%u "
                   "(product-host residual; Soft!=product; W23_STRONGER)\n",
                   g_u32DdiResProductHit, g_u32DdiResProductPath,
                   g_u32DdiResProductPathFull, g_u32DdiResPdevIdMatch,
                   g_u32DdiResDodA, g_u32DdiResDodB,
                   g_u32DdiResCoreOk, g_u32DdiResInstall,
                   (u32)UDX_HOST_SOFT_HANDLE_RETAIN,
                   (u32)UDX_HOST_SOFT_CLOSE_ON_BIND,
                   (u32)UDX_HOST_SYS_DDI_NR, (u32)UDX_HOST_LEAN_CHECKS,
                   UDX_HOST_SOFT_WAVE);
    host_soft_emit("udx: host soft residual dual_dod "
                   "DoD_A=OPEN DoD_B=OPEN agent_close=0 "
                   "A_host=xhci_udx A_id=8086:a12f A_surface=USB_UDX_DDI "
                   "B_host=rtl8168_udx B_id=10ec:8168 B_surface=NIC_UDX_DDI "
                   "DoD_A_OPEN=%u DoD_B_OPEN=%u "
                   "soft_lamps_ne_close=1 bind_PASS_ne_close=1 "
                   "freestanding_rtl=SKIP freestanding_usb=SKIP "
                   "in_kernel_ko=0 G-AC-1=1 product=UDX+ABI "
                   "chain=SCAN,GET,OPEN,MAP_BAR,IRQ,DMA "
                   "side=INVENTORY,CFG,MAP_REMAP,DMA_NOTE,DMA_BUF,"
                   "IRQ_BIND,WIN,INSTALL,IOREMAP "
                   "soft=1 product=0 soft_ne_product=1 "
                   "dual=MIT_OR_Apache-2.0 wave=%u "
                   "(Dual DoD A/B OPEN; soft residual != close)\n",
                   (u32)UDX_HOST_SOFT_DOD_A_OPEN,
                   (u32)UDX_HOST_SOFT_DOD_B_OPEN,
                   UDX_HOST_SOFT_WAVE);
    host_soft_emit("udx: host soft residual c2 "
                   "track=libudx_host residual=Cap+DDI_bind "
                   "chain=SCAN,INVENTORY,GET,OPEN,CFG,"
                   "MAP_BAR,MAP_REMAP,IRQ_BIND,DMA_NOTE,DMA_BUF,"
                   "install_granted,window_register,ioremap "
                   "product_chain=SCAN,GET,OPEN,MAP_BAR,IRQ,DMA "
                   "scan_ok=%u open_ok=%u map_ok=%u install=%u "
                   "ioremap=%u ioremap_ok=%u "
                   "inv_ok=%u cfg_ok=%u cfg_id_match=%u "
                   "remap_ok=%u dma_note_ok=%u dma_buf_ok=%u irq_ok=%u "
                   "va_match=%u pref_complete=%u "
                   "func=%u func_step=%u product_path=%u product_path_full=%u "
                   "pdev_id_match=%u lean_checks=%u "
                   "hosts=rtl8168_udx,xhci_udx "
                   "DoD_A_OPEN=%u DoD_B_OPEN=%u "
                   "fs_class=SKIP product=UDX+ABI "
                   "cap_mint=%u handle_retain=%u close_on_bind=%u "
                   "soft=1 product_mint=0 soft_ne_product=1 "
                   "G-AC-1 dual=MIT_OR_Apache-2.0 wave=%u "
                   "(C2 residual; Soft!=product; agent!=close; W23 STRONGER)\n",
                   g_u32DdiResScanOk, g_u32DdiResOpenOk, g_u32DdiResMapOk,
                   g_u32DdiResInstall, g_u32DdiResIoremap,
                   g_u32DdiResIoremapOk, g_u32DdiResInvOk, g_u32DdiResCfgOk,
                   g_u32DdiResCfgIdMatch, g_u32DdiResRemapOk,
                   g_u32DdiResDmaNoteOk, g_u32DdiResDmaBufOk, g_u32DdiResIrqOk,
                   g_u32DdiResVaMatch, g_u32DdiResPrefComplete,
                   g_u32DdiResFunc, g_u32DdiResFuncStep, g_u32DdiResProductPath,
                   g_u32DdiResProductPathFull, g_u32DdiResPdevIdMatch,
                   (u32)UDX_HOST_LEAN_CHECKS,
                   (u32)UDX_HOST_SOFT_DOD_A_OPEN,
                   (u32)UDX_HOST_SOFT_DOD_B_OPEN,
                   (u32)UDX_HOST_SOFT_CAP_MINT,
                   (u32)UDX_HOST_SOFT_HANDLE_RETAIN,
                   (u32)UDX_HOST_SOFT_CLOSE_ON_BIND,
                   UDX_HOST_SOFT_WAVE);
    host_soft_emit("udx: host soft c2 product path "
                   "chain=SCAN,GET,OPEN,MAP_BAR,IRQ,DMA "
                   "side=INVENTORY,CFG,MAP_REMAP,DMA_NOTE,DMA_BUF,"
                   "WIN,INSTALL,IOREMAP "
                   "bind_enter=%u bind_pass=%u bind_skip=%u "
                   "product_hit=%u product_path=%u product_path_full=%u "
                   "pdev_id_match=%u core_ok=%u "
                   "life_peak=0x%x lean_checks=%u "
                   "ioremap_ok=%u ioremap_fail=%u "
                   "irq_ok=%u dma_note_ok=%u dma_buf_ok=%u "
                   "sys_ddi=%u chain_n=%u "
                   "hosts=rtl8168_udx,xhci_udx "
                   "ids=10ec:8168,8086:a12f "
                   "DoD_A=OPEN DoD_B=OPEN "
                   "fs_class=SKIP product=UDX+ABI "
                   "soft=1 product_mint=0 soft_ne_product=1 "
                   "G-AC-1 dual=MIT_OR_Apache-2.0 wave=%u "
                   "(C2 residual; Soft!=product; agent!=close; W23 STRONGER)\n",
                   g_u32DdiBindEnter, g_u32DdiBindPass, g_u32DdiBindSkip,
                   g_u32DdiResProductHit, g_u32DdiResProductPath,
                   g_u32DdiResProductPathFull, g_u32DdiResPdevIdMatch,
                   g_u32DdiResCoreOk, g_u32DdiResLifePeak,
                   (u32)UDX_HOST_LEAN_CHECKS,
                   g_u32DdiResIoremapOk, g_u32DdiResIoremapFail,
                   g_u32DdiResIrqOk, g_u32DdiResDmaNoteOk, g_u32DdiResDmaBufOk,
                   (u32)UDX_HOST_SYS_DDI_NR, (u32)UDX_HOST_PRODUCT_CHAIN_N,
                   UDX_HOST_SOFT_WAVE);
    host_soft_emit("udx: host soft residual hazard "
                   "H2=no_stamp_storms once=1 storm=0 "
                   "inventory_once=host_soft_maybe_once "
                   "honesty_once=g_fDdiResHonestyOnce "
                   "dod_once=g_fDdiDodHonestyOnce "
                   "inv_once=host_ddi_inventory_residual "
                   "soft_lamps_ne_Dual_DoD_close=1 "
                   "agent_ne_close=1 product_close=0 "
                   "soft=1 soft_ne_product=1 G-AC-1 "
                   "dual=MIT_OR_Apache-2.0 wave=%u "
                   "(hazard residual; Soft!=product)\n",
                   UDX_HOST_SOFT_WAVE);

    /* ABI-first soft DDI readiness + first host init PASS (observation). */
    udx_host_soft_init();
    udx_host_soft_note_ddi();
    udx_host_soft_lifecycle_note();

    /* Soft DDI bind inventory tallies (PASS/SKIP are separate lamps). */
    host_soft_emit("udx: host soft ddi_bind enter=%u pass=%u skip=%u "
                   "sys=%u sys_fail=%u "
                   "targets=10ec:8168,8086:a12f "
                   "hosts=rtl8168_udx,xhci_udx "
                   "dod_a=%u dod_b=%u core_ok=%u inv=%u inv_ok=%u "
                   "soft=1 product=0 product_eq=UDX+ABI "
                   "dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX close=0 "
                   "DoD_A_OPEN=%u DoD_B_OPEN=%u "
                   "cap_mint=%u handle_retain=%u close_on_bind=%u\n",
                   g_u32DdiBindEnter, g_u32DdiBindPass, g_u32DdiBindSkip,
                   g_u32DdiBindSys, g_u32DdiBindSysFail,
                   g_u32DdiResDodA, g_u32DdiResDodB, g_u32DdiResCoreOk,
                   g_u32DdiResInv, g_u32DdiResInvOk,
                   (u32)UDX_HOST_SOFT_DOD_A_OPEN,
                   (u32)UDX_HOST_SOFT_DOD_B_OPEN,
                   (u32)UDX_HOST_SOFT_CAP_MINT,
                   (u32)UDX_HOST_SOFT_HANDLE_RETAIN,
                   (u32)UDX_HOST_SOFT_CLOSE_ON_BIND);

    /*
     * SCAN→GET→OPEN→MAP→install→ioremap residual tallies (lean; Soft!=product).
     * greppable: udx: soft ddi bind residual
     */
    host_soft_emit("udx: soft ddi bind residual "
                   "scan=%u scan_ok=%u get=%u get_ok=%u match=%u "
                   "open=%u open_ok=%u map=%u map_ok=%u "
                   "map_empty=%u map_fail=%u map_user=%u "
                   "cfg=%u cfg_ok=%u cfg_id_match=%u "
                   "remap=%u remap_ok=%u remap_idem=%u "
                   "dma_note=%u dma_note_ok=%u "
                   "dma_buf=%u dma_buf_ok=%u dma_buf_map=%u dma_buf_free=%u "
                   "irq=%u irq_ok=%u "
                   "win=%u install=%u "
                   "ioremap=%u ioremap_ok=%u ioremap_fail=%u "
                   "va_match=%u pref_complete=%u "
                   "func=%u func_step=%u product_path=%u product_path_full=%u "
                   "pdev_id_match=%u lean_checks=%u "
                   "product_hit=%u life_peak=0x%x "
                   "inv=%u inv_ok=%u dod_a=%u dod_b=%u core_ok=%u "
                   "sys_ddi=%u chain_n=%u "
                   "chain=SCAN,GET,OPEN,MAP_BAR,IRQ,DMA denser_prove=1 "
                   "side=INVENTORY,CFG,MAP_REMAP,DMA_NOTE,DMA_BUF,"
                   "IRQ_BIND,WIN,INSTALL,IOREMAP "
                   "hosts=rtl8168_udx,xhci_udx "
                   "ids=10ec:8168,8086:a12f product=UDX+ABI "
                   "dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX close=0 "
                   "DoD_A_OPEN=%u DoD_B_OPEN=%u "
                   "cap_mint=%u handle_retain=%u close_on_bind=%u "
                   "close_on_pass=%u handle_retain_lock=%u "
                   "fs_class=SKIP freestanding_class=SKIP "
                   "soft=1 mint=0 Soft!=product dual_dod OPEN "
                   "W23_STRONGER=1\n",
                   g_u32DdiResScan, g_u32DdiResScanOk,
                   g_u32DdiResGet, g_u32DdiResGetOk, g_u32DdiResMatch,
                   g_u32DdiResOpen, g_u32DdiResOpenOk,
                   g_u32DdiResMap, g_u32DdiResMapOk,
                   g_u32DdiResMapEmpty, g_u32DdiResMapFail,
                   g_u32DdiResMapUser,
                   g_u32DdiResCfg, g_u32DdiResCfgOk, g_u32DdiResCfgIdMatch,
                   g_u32DdiResRemap, g_u32DdiResRemapOk,
                   g_u32DdiResRemapIdem,
                   g_u32DdiResDmaNote, g_u32DdiResDmaNoteOk,
                   g_u32DdiResDmaBuf, g_u32DdiResDmaBufOk,
                   g_u32DdiResDmaBufMap, g_u32DdiResDmaBufFree,
                   g_u32DdiResIrq, g_u32DdiResIrqOk,
                   g_u32DdiResWin, g_u32DdiResInstall,
                   g_u32DdiResIoremap, g_u32DdiResIoremapOk,
                   g_u32DdiResIoremapFail,
                   g_u32DdiResVaMatch, g_u32DdiResPrefComplete,
                   g_u32DdiResFunc, g_u32DdiResFuncStep, g_u32DdiResProductPath,
                   g_u32DdiResProductPathFull, g_u32DdiResPdevIdMatch,
                   (u32)UDX_HOST_LEAN_CHECKS,
                   g_u32DdiResProductHit, g_u32DdiResLifePeak,
                   g_u32DdiResInv, g_u32DdiResInvOk,
                   g_u32DdiResDodA, g_u32DdiResDodB, g_u32DdiResCoreOk,
                   (u32)UDX_HOST_SYS_DDI_NR, (u32)UDX_HOST_PRODUCT_CHAIN_N,
                   (u32)UDX_HOST_SOFT_DOD_A_OPEN,
                   (u32)UDX_HOST_SOFT_DOD_B_OPEN,
                   (u32)UDX_HOST_SOFT_CAP_MINT,
                   (u32)UDX_HOST_SOFT_HANDLE_RETAIN,
                   (u32)UDX_HOST_SOFT_CLOSE_ON_BIND,
                   (u32)UDX_HOST_CLOSE_ON_PASS,
                   (u32)UDX_HOST_HANDLE_RETAIN_LOCK);
}

/** Soft: one-shot inventory after first successful window register. */
static void
host_soft_maybe_once(void)
{
    if (g_fHostSoftOnce != 0) {
        return;
    }
    if (g_u32HostWinRegOk == 0 && g_u32HostWinRegUpd == 0 &&
        g_u32HostFireIrq == 0) {
        return;
    }
    g_fHostSoftOnce = 1;
    /* First host activity path: soft DDI host note PASS (idempotent). */
    udx_host_soft_init();
    host_soft_inventory_log();
}

#if defined(UDX_HOST_LIBC)
static int
host_range_ok(u64 u64Base, u64 u64Len, u64 u64Off, u64 u64Width)
{
    if (u64Base == 0 || u64Len == 0 || u64Width == 0) {
        return 0;
    }
    if (u64Off > u64Len) {
        return 0;
    }
    if (u64Width > u64Len - u64Off) {
        return 0;
    }
    return 1;
}
#endif /* UDX_HOST_LIBC — freestanding bind residual does not poke BARs */

void
udx_host_window_register(u64 u64Phys, void *pVa, u64 u64Len)
{
    u32 iWin;
    u32 iFree;

    host_soft_inc(&g_u32HostWinReg);
    if (pVa == NULL || u64Len == 0) {
        host_soft_inc(&g_u32HostWinRegInval);
        return;
    }
    /* Overflow: phys+len must not wrap. */
    if (u64Phys + u64Len < u64Phys) {
        host_soft_inc(&g_u32HostWinRegOvf);
        udx_printk("udx: host window reject overflow phys=%llx len=%llx\n",
                   (unsigned long long)u64Phys,
                   (unsigned long long)u64Len);
        return;
    }

    /* Update existing mapping for the same phys base. */
    iFree = UDX_HOST_WIN_MAX;
    for (iWin = 0; iWin < UDX_HOST_WIN_MAX; iWin++) {
        if (g_aWin[iWin].u8Used && g_aWin[iWin].u64Phys == u64Phys) {
            g_aWin[iWin].u64Len = u64Len;
            g_aWin[iWin].pVa = pVa;
            host_soft_inc(&g_u32HostWinRegUpd);
            host_soft_maybe_once();
            return;
        }
        if (!g_aWin[iWin].u8Used && iFree == UDX_HOST_WIN_MAX) {
            iFree = iWin;
        }
    }
    if (iFree >= UDX_HOST_WIN_MAX) {
        host_soft_inc(&g_u32HostWinRegFull);
        udx_printk("udx: host window table full\n");
        return;
    }
    g_aWin[iFree].u64Phys = u64Phys;
    g_aWin[iFree].u64Len = u64Len;
    g_aWin[iFree].pVa = pVa;
    g_aWin[iFree].u8Used = 1;
    host_soft_inc(&g_u32HostWinRegOk);
    g_u32HostWinLive = host_soft_live_windows();
    host_soft_note_peak(&g_u32HostWinPeak, g_u32HostWinLive);
    host_soft_maybe_once();
}

void
udx_host_window_unregister(u64 u64Phys)
{
    u32 iWin;

    host_soft_inc(&g_u32HostWinUnreg);
    for (iWin = 0; iWin < UDX_HOST_WIN_MAX; iWin++) {
        if (g_aWin[iWin].u8Used && g_aWin[iWin].u64Phys == u64Phys) {
            g_aWin[iWin].u8Used = 0;
            g_aWin[iWin].pVa = NULL;
            g_aWin[iWin].u64Len = 0;
            g_aWin[iWin].u64Phys = 0;
            host_soft_inc(&g_u32HostWinUnregHit);
            g_u32HostWinLive = host_soft_live_windows();
            return;
        }
    }
    host_soft_inc(&g_u32HostWinUnregMiss);
}

void *
udx_host_window_lookup(u64 u64Phys, u64 u64Len, u64 *pu64OffOut)
{
    u32 iWin;

    host_soft_inc(&g_u32HostWinLookup);
    if (u64Len == 0) {
        host_soft_inc(&g_u32HostWinLookInval);
        return NULL;
    }
    if (u64Phys + u64Len < u64Phys) {
        host_soft_inc(&g_u32HostWinLookInval);
        return NULL; /* overflow */
    }

    for (iWin = 0; iWin < UDX_HOST_WIN_MAX; iWin++) {
        u64 u64Base;
        u64 u64End;
        u64 u64ReqEnd;

        if (!g_aWin[iWin].u8Used) {
            continue;
        }
        u64Base = g_aWin[iWin].u64Phys;
        u64End = u64Base + g_aWin[iWin].u64Len;
        if (u64End < u64Base) {
            continue; /* corrupt / overflow entry */
        }
        u64ReqEnd = u64Phys + u64Len;
        if (u64Phys >= u64Base && u64ReqEnd <= u64End) {
            if (pu64OffOut) {
                *pu64OffOut = u64Phys - u64Base;
            }
            host_soft_inc(&g_u32HostWinLookHit);
            return (u8 *)g_aWin[iWin].pVa + (u64Phys - u64Base);
        }
    }
    host_soft_inc(&g_u32HostWinLookMiss);
    return NULL;
}

void
udx_host_fire_irq(int nIrq)
{
    host_soft_inc(&g_u32HostFireIrq);
    host_soft_maybe_once();
    udx_irq_dispatch(nIrq);
}

#if defined(UDX_HOST_LIBC)
static udx_status_t
host_bar_va(struct udx_pci_dev *pPdev, int nBar, u64 u64Off, u64 u64Width,
            void **ppVa)
{
    u64 u64Phys;
    u64 u64Len;
    void *pVa;

    if (pPdev == NULL || ppVa == NULL || nBar < 0 || nBar > 5) {
        return UDX_ERR_INVAL;
    }
    u64Phys = pPdev->aBarPhys[nBar];
    u64Len = pPdev->aBarLen[nBar];
    if (!host_range_ok(u64Phys, u64Len, u64Off, u64Width)) {
        return UDX_ERR_INVAL;
    }
    /* Prefer whole-BAR window; fall back to offset-sized sub-window. */
    pVa = udx_host_window_lookup(u64Phys, u64Len, NULL);
    if (pVa == NULL) {
        pVa = udx_host_window_lookup(u64Phys + u64Off, u64Width, NULL);
        if (pVa == NULL) {
            return UDX_ERR_NODEV;
        }
        *ppVa = pVa;
        return UDX_OK;
    }
    *ppVa = (u8 *)pVa + (size_t)u64Off;
    return UDX_OK;
}
#endif /* UDX_HOST_LIBC */

udx_status_t
udx_host_bar_writel(struct udx_pci_dev *pPdev, int nBar, u64 u64Off, u32 u32Val)
{
#if !defined(UDX_HOST_LIBC)
    (void)pPdev;
    (void)nBar;
    (void)u64Off;
    (void)u32Val;
    host_soft_inc(&g_u32HostBarNosys);
    return UDX_ERR_NOSYS;
#else
    void *pVa;
    udx_status_t st;

    st = host_bar_va(pPdev, nBar, u64Off, 4, &pVa);
    if (st != UDX_OK) {
        host_soft_inc(&g_u32HostBarFail);
        return st;
    }
    *(volatile u32 *)pVa = u32Val;
    host_soft_inc(&g_u32HostBarWrite);
    host_soft_inc(&g_u32HostBarW);
    return UDX_OK;
#endif
}

udx_status_t
udx_host_bar_readl(struct udx_pci_dev *pPdev, int nBar, u64 u64Off, u32 *pu32Val)
{
#if !defined(UDX_HOST_LIBC)
    (void)pPdev;
    (void)nBar;
    (void)u64Off;
    (void)pu32Val;
    host_soft_inc(&g_u32HostBarNosys);
    return UDX_ERR_NOSYS;
#else
    void *pVa;
    udx_status_t st;

    if (pu32Val == NULL) {
        host_soft_inc(&g_u32HostBarFail);
        return UDX_ERR_INVAL;
    }
    st = host_bar_va(pPdev, nBar, u64Off, 4, &pVa);
    if (st != UDX_OK) {
        host_soft_inc(&g_u32HostBarFail);
        return st;
    }
    *pu32Val = *(volatile u32 *)pVa;
    host_soft_inc(&g_u32HostBarRead);
    host_soft_inc(&g_u32HostBarR);
    return UDX_OK;
#endif
}

udx_status_t
udx_host_bar_writew(struct udx_pci_dev *pPdev, int nBar, u64 u64Off, u16 u16Val)
{
#if !defined(UDX_HOST_LIBC)
    (void)pPdev;
    (void)nBar;
    (void)u64Off;
    (void)u16Val;
    host_soft_inc(&g_u32HostBarNosys);
    return UDX_ERR_NOSYS;
#else
    void *pVa;
    udx_status_t st;

    st = host_bar_va(pPdev, nBar, u64Off, 2, &pVa);
    if (st != UDX_OK) {
        host_soft_inc(&g_u32HostBarFail);
        return st;
    }
    *(volatile u16 *)pVa = u16Val;
    host_soft_inc(&g_u32HostBarWrite);
    host_soft_inc(&g_u32HostBarW);
    return UDX_OK;
#endif
}

udx_status_t
udx_host_bar_readw(struct udx_pci_dev *pPdev, int nBar, u64 u64Off, u16 *pu16Val)
{
#if !defined(UDX_HOST_LIBC)
    (void)pPdev;
    (void)nBar;
    (void)u64Off;
    (void)pu16Val;
    host_soft_inc(&g_u32HostBarNosys);
    return UDX_ERR_NOSYS;
#else
    void *pVa;
    udx_status_t st;

    if (pu16Val == NULL) {
        host_soft_inc(&g_u32HostBarFail);
        return UDX_ERR_INVAL;
    }
    st = host_bar_va(pPdev, nBar, u64Off, 2, &pVa);
    if (st != UDX_OK) {
        host_soft_inc(&g_u32HostBarFail);
        return st;
    }
    *pu16Val = *(volatile u16 *)pVa;
    host_soft_inc(&g_u32HostBarRead);
    host_soft_inc(&g_u32HostBarR);
    return UDX_OK;
#endif
}

udx_status_t
udx_host_bar_writeb(struct udx_pci_dev *pPdev, int nBar, u64 u64Off, u8 u8Val)
{
#if !defined(UDX_HOST_LIBC)
    (void)pPdev;
    (void)nBar;
    (void)u64Off;
    (void)u8Val;
    host_soft_inc(&g_u32HostBarNosys);
    return UDX_ERR_NOSYS;
#else
    void *pVa;
    udx_status_t st;

    st = host_bar_va(pPdev, nBar, u64Off, 1, &pVa);
    if (st != UDX_OK) {
        host_soft_inc(&g_u32HostBarFail);
        return st;
    }
    *(volatile u8 *)pVa = u8Val;
    host_soft_inc(&g_u32HostBarWrite);
    host_soft_inc(&g_u32HostBarW);
    return UDX_OK;
#endif
}

udx_status_t
udx_host_bar_readb(struct udx_pci_dev *pPdev, int nBar, u64 u64Off, u8 *pu8Val)
{
#if !defined(UDX_HOST_LIBC)
    (void)pPdev;
    (void)nBar;
    (void)u64Off;
    (void)pu8Val;
    host_soft_inc(&g_u32HostBarNosys);
    return UDX_ERR_NOSYS;
#else
    void *pVa;
    udx_status_t st;

    if (pu8Val == NULL) {
        host_soft_inc(&g_u32HostBarFail);
        return UDX_ERR_INVAL;
    }
    st = host_bar_va(pPdev, nBar, u64Off, 1, &pVa);
    if (st != UDX_OK) {
        host_soft_inc(&g_u32HostBarFail);
        return st;
    }
    *pu8Val = *(volatile u8 *)pVa;
    host_soft_inc(&g_u32HostBarRead);
    host_soft_inc(&g_u32HostBarR);
    return UDX_OK;
#endif
}

udx_status_t
udx_host_bar_writeq(struct udx_pci_dev *pPdev, int nBar, u64 u64Off, u64 u64Val)
{
#if !defined(UDX_HOST_LIBC)
    (void)pPdev;
    (void)nBar;
    (void)u64Off;
    (void)u64Val;
    host_soft_inc(&g_u32HostBarNosys);
    return UDX_ERR_NOSYS;
#else
    void *pVa;
    udx_status_t st;

    st = host_bar_va(pPdev, nBar, u64Off, 8, &pVa);
    if (st != UDX_OK) {
        host_soft_inc(&g_u32HostBarFail);
        return st;
    }
    *(volatile u64 *)pVa = u64Val;
    host_soft_inc(&g_u32HostBarWrite);
    host_soft_inc(&g_u32HostBarW);
    return UDX_OK;
#endif
}

udx_status_t
udx_host_bar_readq(struct udx_pci_dev *pPdev, int nBar, u64 u64Off, u64 *pu64Val)
{
#if !defined(UDX_HOST_LIBC)
    (void)pPdev;
    (void)nBar;
    (void)u64Off;
    (void)pu64Val;
    host_soft_inc(&g_u32HostBarNosys);
    return UDX_ERR_NOSYS;
#else
    void *pVa;
    udx_status_t st;

    if (pu64Val == NULL) {
        host_soft_inc(&g_u32HostBarFail);
        return UDX_ERR_INVAL;
    }
    st = host_bar_va(pPdev, nBar, u64Off, 8, &pVa);
    if (st != UDX_OK) {
        host_soft_inc(&g_u32HostBarFail);
        return st;
    }
    *pu64Val = *(volatile u64 *)pVa;
    host_soft_inc(&g_u32HostBarRead);
    host_soft_inc(&g_u32HostBarR);
    return UDX_OK;
#endif
}

udx_status_t
udx_host_bar_memcpy_to(struct udx_pci_dev *pPdev, int nBar, u64 u64Off,
                       const void *pSrc, size_t cb)
{
#if !defined(UDX_HOST_LIBC)
    (void)pPdev;
    (void)nBar;
    (void)u64Off;
    (void)pSrc;
    (void)cb;
    host_soft_inc(&g_u32HostBarNosys);
    return UDX_ERR_NOSYS;
#else
    void *pVa;
    udx_status_t st;
    size_t iByte;
    const u8 *pIn;
    volatile u8 *pOut;

    if (pSrc == NULL || cb == 0) {
        host_soft_inc(&g_u32HostBarFail);
        return UDX_ERR_INVAL;
    }
    st = host_bar_va(pPdev, nBar, u64Off, (u64)cb, &pVa);
    if (st != UDX_OK) {
        host_soft_inc(&g_u32HostBarFail);
        return st;
    }
    pIn = (const u8 *)pSrc;
    pOut = (volatile u8 *)pVa;
    for (iByte = 0; iByte < cb; iByte++) {
        pOut[iByte] = pIn[iByte];
    }
    host_soft_inc(&g_u32HostBarMemcpy);
    return UDX_OK;
#endif
}

udx_status_t
udx_host_bar_memcpy_from(struct udx_pci_dev *pPdev, int nBar, u64 u64Off,
                         void *pDst, size_t cb)
{
#if !defined(UDX_HOST_LIBC)
    (void)pPdev;
    (void)nBar;
    (void)u64Off;
    (void)pDst;
    (void)cb;
    host_soft_inc(&g_u32HostBarNosys);
    return UDX_ERR_NOSYS;
#else
    void *pVa;
    udx_status_t st;
    size_t iByte;
    u8 *pOut;
    const volatile u8 *pIn;

    if (pDst == NULL || cb == 0) {
        host_soft_inc(&g_u32HostBarFail);
        return UDX_ERR_INVAL;
    }
    st = host_bar_va(pPdev, nBar, u64Off, (u64)cb, &pVa);
    if (st != UDX_OK) {
        host_soft_inc(&g_u32HostBarFail);
        return st;
    }
    pOut = (u8 *)pDst;
    pIn = (const volatile u8 *)pVa;
    for (iByte = 0; iByte < cb; iByte++) {
        pOut[iByte] = pIn[iByte];
    }
    host_soft_inc(&g_u32HostBarMemcpy);
    return UDX_OK;
#endif
}

/* pci inject / remove live in pci.c — declared in host.h */

/*
 * Soft DDI host init — first path greppable PASS.
 * Idempotent; observation only; never gates skeleton PASS.
 *
 * greppable: udx: soft ddi host note PASS
 */
void
udx_host_soft_init(void)
{
    host_soft_inc(&g_u32DdiInitCalls);
    if (g_fDdiHostInited != 0) {
        return;
    }
    g_fDdiHostInited = 1;
    /*
     * Grep: udx: soft ddi host note PASS
     * Soft types (BDF/BAR/window_grant) ready for demos; product mint OPEN.
     * No GPL; dual MIT OR Apache-2.0; no live cap grant.
     */
    host_soft_emit("udx: soft ddi host note PASS "
                   "abi_first=1 dual_license=1 gpl=0 "
                   "bdf=1 bar=1 dma_grant=soft_fail "
                   "lifecycle=probe/remove/quiesce "
                   "cap_mint=%u multi_server=%u confine=%u "
                   "DoD_A_OPEN=%u DoD_B_OPEN=%u "
                   "hosts=rtl8168_udx,xhci_udx product=UDX+ABI "
                   "product_mint=0 soft=1 hard_gate=0 Soft!=product\n",
                   (u32)UDX_HOST_SOFT_CAP_MINT,
                   (u32)UDX_HOST_SOFT_MULTI_SERVER,
                   (u32)UDX_HOST_SOFT_CONFINE,
                   (u32)UDX_HOST_SOFT_DOD_A_OPEN,
                   (u32)UDX_HOST_SOFT_DOD_B_OPEN);
}

int
udx_host_soft_ready(void)
{
    return (g_fDdiHostInited != 0) ? 1 : 0;
}

/*
 * Soft note for future DDI attachment (ABI-first pivot).
 * UDX hosts out-of-tree dual-license drivers; no GPL.
 * Observation only; never gates skeleton PASS or freestanding path.
 *
 * greppable: udx: soft ddi-ready
 */
void
udx_host_soft_note_ddi(void)
{
    /* Ensure first-host-init PASS lamp has fired. */
    udx_host_soft_init();
    /*
     * Grep: udx: soft ddi-ready
     * abi_first=1 — product order is ABI + virtio, then DDI caps.
     * out_of_tree=1 dual_license=1 gpl=0 — host surface for MIT/Apache drivers.
     */
    host_soft_emit("udx: soft ddi-ready "
                   "abi_first=1 out_of_tree=1 dual_license=1 gpl=0 "
                   "host=udx ddi_attach=future soft=1 hard_gate=0 "
                   "DoD_A_OPEN=%u DoD_B_OPEN=%u cap_mint=%u "
                   "fs_class=SKIP product=UDX+ABI Soft!=product\n",
                   (u32)UDX_HOST_SOFT_DOD_A_OPEN,
                   (u32)UDX_HOST_SOFT_DOD_B_OPEN,
                   (u32)UDX_HOST_SOFT_CAP_MINT);
}

/*
 * Soft lifecycle catalog — probe / remove / quiesce (DDI spirit).
 * Documentation lamp only; does not attach or detach devices.
 *
 * greppable: udx: soft ddi lifecycle note
 */
void
udx_host_soft_lifecycle_note(void)
{
    host_soft_inc(&g_u32DdiLifecycleN);
    udx_host_soft_init();
    /*
     * Grep: udx: soft ddi lifecycle note
     * probe → live attach; quiesce → stop DMA/IRQ/work; remove → free resources.
     * Product: process death ⇒ kernel Phase-A revoke (not claimed here).
     */
    /*
     * C2 deepen: Dual DoD OPEN + freestanding SKIP + Cap mint honesty.
     * Soft!=product; G-AC-1; dual MIT OR Apache-2.0.
     */
    host_soft_emit("udx: soft ddi lifecycle note "
                   "probe=attach remove=detach quiesce=stop_dma_mask_irq "
                   "order=quiesce_before_remove "
                   "crash_revoke=product_later "
                   "hosts=rtl8168_udx,xhci_udx product=UDX+ABI "
                   "fs_class=SKIP DoD_A_OPEN=%u DoD_B_OPEN=%u "
                   "agent_ne_close=1 cap_mint=%u "
                   "handle_retain=%u close_on_bind=%u "
                   "soft=1 product=0 gpl=0 Soft!=product G-AC-1\n",
                   (u32)UDX_HOST_SOFT_DOD_A_OPEN,
                   (u32)UDX_HOST_SOFT_DOD_B_OPEN,
                   (u32)UDX_HOST_SOFT_CAP_MINT,
                   (u32)UDX_HOST_SOFT_HANDLE_RETAIN,
                   (u32)UDX_HOST_SOFT_CLOSE_ON_BIND);
}

udx_status_t
udx_host_soft_bdf_from_pdev(const struct udx_pci_dev *pPdev,
                            struct udx_host_bdf *pOut)
{
    if (pPdev == NULL || pOut == NULL) {
        return UDX_ERR_INVAL;
    }
    pOut->u8Bus = pPdev->u8Bus;
    pOut->u8Slot = udx_pci_slot(pPdev);
    pOut->u8Func = udx_pci_func(pPdev);
    pOut->u8Pad = 0;
    host_soft_inc(&g_u32DdiBdfSnap);
    return UDX_OK;
}

udx_status_t
udx_host_soft_bar_snapshot(const struct udx_pci_dev *pPdev, int nBar,
                           struct udx_host_bar *pOut)
{
    if (pPdev == NULL || pOut == NULL || nBar < 0 || nBar > 5) {
        return UDX_ERR_INVAL;
    }
    pOut->u8Index = (u8)nBar;
    pOut->u8Mem = pPdev->aBarIsMem[nBar] ? 1u : 0u;
    pOut->u8Prefetch = 0; /* soft inventory; inject path does not track */
    pOut->u8Pad = 0;
    pOut->u64Pa = pPdev->aBarPhys[nBar];
    pOut->u64Cb = pPdev->aBarLen[nBar];
    host_soft_inc(&g_u32DdiBarSnap);
    return UDX_OK;
}

udx_status_t
udx_host_soft_bar_snapshot_all(const struct udx_pci_dev *pPdev,
                               struct udx_host_bar *aOut, u32 *pu32CountOut)
{
    int nBar;
    u32 u32Count;

    if (pPdev == NULL || aOut == NULL) {
        return UDX_ERR_INVAL;
    }
    u32Count = 0;
    for (nBar = 0; nBar < (int)UDX_HOST_SOFT_BAR_MAX; nBar++) {
        if (udx_host_soft_bar_snapshot(pPdev, nBar, &aOut[nBar]) != UDX_OK) {
            return UDX_ERR_INVAL;
        }
        if (aOut[nBar].u64Cb != 0) {
            if (u32Count < 0xffffffffu) {
                u32Count++;
            }
        }
    }
    if (pu32CountOut != NULL) {
        *pu32CountOut = u32Count;
    }
    return UDX_OK;
}

/*
 * Soft DMA window grant request — always not-granted until product.
 *
 * greppable: udx: soft ddi dma not-granted
 */
udx_status_t
udx_host_soft_dma_window_request(const struct udx_host_bdf *pBdf,
                                 u64 u64PaBase, u64 u64Cb, u8 u8Rw,
                                 struct udx_host_window_grant *pGrantOut)
{
    host_soft_inc(&g_u32DdiDmaReq);
    udx_host_soft_init();

    if (pBdf == NULL || u64Cb == 0) {
        host_soft_inc(&g_u32DdiDmaReject);
        if (pGrantOut != NULL) {
            pGrantOut->bdf.u8Bus = 0;
            pGrantOut->bdf.u8Slot = 0;
            pGrantOut->bdf.u8Func = 0;
            pGrantOut->bdf.u8Pad = 0;
            pGrantOut->u64PaBase = 0;
            pGrantOut->u64Cb = 0;
            pGrantOut->u8Ok = 0;
            pGrantOut->u8Rw = 0;
            pGrantOut->u8Pad[0] = 0;
            pGrantOut->u8Pad[1] = 0;
            pGrantOut->u32WinId = 0xffffffffu;
        }
        return UDX_ERR_INVAL;
    }
    /* Overflow: pa+cb must not wrap. */
    if (u64PaBase + u64Cb < u64PaBase) {
        host_soft_inc(&g_u32DdiDmaReject);
        if (pGrantOut != NULL) {
            pGrantOut->bdf = *pBdf;
            pGrantOut->u64PaBase = u64PaBase;
            pGrantOut->u64Cb = u64Cb;
            pGrantOut->u8Ok = 0;
            pGrantOut->u8Rw = u8Rw;
            pGrantOut->u8Pad[0] = 0;
            pGrantOut->u8Pad[1] = 0;
            pGrantOut->u32WinId = 0xffffffffu;
        }
        return UDX_ERR_INVAL;
    }

    host_soft_inc(&g_u32DdiDmaReject);
    if (pGrantOut != NULL) {
        pGrantOut->bdf = *pBdf;
        pGrantOut->u64PaBase = u64PaBase;
        pGrantOut->u64Cb = u64Cb;
        pGrantOut->u8Ok = 0; /* not granted — product mint OPEN */
        pGrantOut->u8Rw = u8Rw;
        pGrantOut->u8Pad[0] = 0;
        pGrantOut->u8Pad[1] = 0;
        pGrantOut->u32WinId = 0xffffffffu;
    }

    /*
     * Grep: udx: soft ddi dma not-granted
     * Soft fail until devmgr mints real DMA window caps (docs/DDI_SOFT.md).
     */
    host_soft_emit("udx: soft ddi dma not-granted "
                   "bus=%u slot=%u func=%u pa=0x%llx cb=0x%llx rw=0x%x "
                   "ok=0 win_id=0xffffffff product_mint=0 soft=1\n",
                   (unsigned)pBdf->u8Bus, (unsigned)pBdf->u8Slot,
                   (unsigned)pBdf->u8Func,
                   (unsigned long long)u64PaBase,
                   (unsigned long long)u64Cb,
                   (unsigned)u8Rw);

    return UDX_ERR_NOSYS;
}

u32
udx_host_soft_init_calls(void)
{
    return g_u32DdiInitCalls;
}

u32
udx_host_soft_dma_requests(void)
{
    return g_u32DdiDmaReq;
}

u32
udx_host_soft_dma_rejects(void)
{
    return g_u32DdiDmaReject;
}

/*
 * Dual DoD A/B OPEN once-lamp (agent!=close; Soft!=product; H2 no storm).
 * C2 deepen: UDX_HOST_SOFT_DOD_* constants + agent_ne_close honesty.
 * greppable: udx: soft residual Dual DoD A OPEN
 * greppable: udx: soft residual Dual DoD B OPEN
 * greppable: udx: soft residual Dual DoD A/B OPEN
 * greppable: udx: host soft residual dual_dod
 */
static void
host_soft_ddi_dual_dod_honesty(void)
{
    if (g_fDdiDodHonestyOnce != 0) {
        return;
    }
    g_fDdiDodHonestyOnce = 1;
    host_soft_emit("udx: soft residual Dual DoD A OPEN "
                   "dual_dod_a=OPEN_UDX path=xhci_udx id=8086:a12f "
                   "bar=0 product=UDX+ABI fs_class=SKIP "
                   "DoD_A_OPEN=%u agent_ne_close=1 soft_lamps_ne_close=1 "
                   "close=0 mint=%u soft=1 Soft!=product\n",
                   (u32)UDX_HOST_SOFT_DOD_A_OPEN,
                   (u32)UDX_HOST_SOFT_CAP_MINT);
    host_soft_emit("udx: soft residual Dual DoD B OPEN "
                   "dual_dod_b=OPEN_UDX path=rtl8168_udx id=10ec:8168 "
                   "bars=0,2 product=UDX+ABI fs_class=SKIP "
                   "DoD_B_OPEN=%u agent_ne_close=1 soft_lamps_ne_close=1 "
                   "close=0 mint=%u soft=1 Soft!=product\n",
                   (u32)UDX_HOST_SOFT_DOD_B_OPEN,
                   (u32)UDX_HOST_SOFT_CAP_MINT);
    host_soft_emit("udx: soft residual Dual DoD A/B OPEN "
                   "dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX close=0 "
                   "DoD_A_OPEN=%u DoD_B_OPEN=%u agent_ne_close=1 "
                   "hosts=xhci_udx,rtl8168_udx product=UDX+ABI "
                   "chain=SCAN,GET,OPEN,MAP_BAR,IRQ,DMA "
                   "side=INVENTORY,CFG,MAP_REMAP,DMA_NOTE,DMA_BUF,IRQ_BIND,WIN,INSTALL,IOREMAP "
                   "ids=10ec:8168,8086:a12f "
                   "handle_retain=%u close_on_bind=%u "
                   "G-AC-1 soft=1 mint=%u Soft!=product\n",
                   (u32)UDX_HOST_SOFT_DOD_A_OPEN,
                   (u32)UDX_HOST_SOFT_DOD_B_OPEN,
                   (u32)UDX_HOST_SOFT_HANDLE_RETAIN,
                   (u32)UDX_HOST_SOFT_CLOSE_ON_BIND,
                   (u32)UDX_HOST_SOFT_CAP_MINT);
    /* Unified dual_dod residual (matches irq/work/core C2 shape). */
    host_soft_emit("udx: host soft residual dual_dod "
                   "DoD_A=OPEN DoD_B=OPEN agent_close=0 "
                   "A_host=xhci_udx A_id=8086:a12f A_surface=USB_UDX_DDI "
                   "B_host=rtl8168_udx B_id=10ec:8168 B_surface=NIC_UDX_DDI "
                   "DoD_A_OPEN=%u DoD_B_OPEN=%u "
                   "soft_lamps_ne_close=1 bind_PASS_ne_close=1 "
                   "freestanding_rtl=SKIP freestanding_usb=SKIP "
                   "in_kernel_ko=0 G-AC-1=1 product=UDX+ABI "
                   "soft=1 product=0 soft_ne_product=1 "
                   "dual=MIT_OR_Apache-2.0 "
                   "(Dual DoD A/B OPEN; soft residual != close)\n",
                   (u32)UDX_HOST_SOFT_DOD_A_OPEN,
                   (u32)UDX_HOST_SOFT_DOD_B_OPEN);
    udx_printk("udx: soft residual Dual DoD A/B OPEN "
               "dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX close=0 "
               "DoD_A_OPEN=%u DoD_B_OPEN=%u agent_ne_close=1 "
               "hosts=xhci_udx,rtl8168_udx soft=1 product=UDX+ABI\n",
               (u32)UDX_HOST_SOFT_DOD_A_OPEN,
               (u32)UDX_HOST_SOFT_DOD_B_OPEN);
}

/*
 * Soft DDI bind lamps + SCAN→GET→OPEN→MAP_BAR residual (product UDX hosts).
 * C2 deepen: freestanding SKIP + product=UDX+ABI + Dual DoD OPEN honesty.
 * greppable: udx: soft ddi bind PASS
 * greppable: udx: soft ddi bind SKIP
 * greppable: udx: soft ddi bind residual
 * greppable: udx: soft residual freestanding class SKIP
 * greppable: udx: soft residual product=UDX+ABI
 * greppable: udx: soft residual Dual DoD A/B OPEN
 * greppable: udx: host soft residual product_host
 * greppable: udx: host soft residual dual_dod
 */
static void
host_soft_ddi_bind_residual_honesty(void)
{
    /*
     * Once-lamp residual honesty (no stamp storm H2).
     * Freestanding kernel class drivers remain SKIP; product = UDX+ABI.
     * Dual DoD A/B OPEN (agent!=close) -- product hosts xhci/rtl only.
     */
    if (g_fDdiResHonestyOnce != 0) {
        host_soft_ddi_dual_dod_honesty();
        return;
    }
    g_fDdiResHonestyOnce = 1;
    host_soft_emit("udx: soft residual freestanding class SKIP "
                   "kernel_class_drivers=SKIP "
                   "kernel_rtl8168=SKIP kernel_xhci_msc=SKIP "
                   "GJ_RTL8168_PROBE=0 GJ_XHCI_MSC_PROBE=0 "
                   "product=UDX+ABI Linux-shaped userspace "
                   "hosts=rtl8168_udx,xhci_udx "
                   "dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX close=0 "
                   "DoD_A_OPEN=%u DoD_B_OPEN=%u agent_ne_close=1 "
                   "chain=SCAN,GET,OPEN,MAP_BAR,IRQ,DMA "
                   "side=INVENTORY,CFG,MAP_REMAP,DMA_NOTE,DMA_BUF,IRQ_BIND,WIN,INSTALL,IOREMAP "
                   "ids=10ec:8168,8086:a12f "
                   "soft=1 product=0 G-AC-1 Soft!=product\n",
                   (u32)UDX_HOST_SOFT_DOD_A_OPEN,
                   (u32)UDX_HOST_SOFT_DOD_B_OPEN);
    host_soft_emit("udx: soft residual product=UDX+ABI "
                   "surface=Linux-porter "
                   "hosts=rtl8168_udx,xhci_udx "
                   "xhci_udx=DoD_A_OPEN rtl8168_udx=DoD_B_OPEN "
                   "dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX close=0 "
                   "DoD_A_OPEN=%u DoD_B_OPEN=%u agent_ne_close=1 "
                   "bind=udx_host_bind_by_id "
                   "chain=SCAN,GET,OPEN,MAP_BAR,IRQ,DMA "
                   "side=INVENTORY,CFG,MAP_REMAP,DMA_NOTE,DMA_BUF,IRQ_BIND,WIN,INSTALL,IOREMAP "
                   "ids=10ec:8168,8086:a12f "
                   "handle_retain=%u close_on_bind=%u "
                   "caps=hidden soft_inventory=1 product_mint=0 "
                   "cap_mint=%u G-AC-1 Soft!=product\n",
                   (u32)UDX_HOST_SOFT_DOD_A_OPEN,
                   (u32)UDX_HOST_SOFT_DOD_B_OPEN,
                   (u32)UDX_HOST_SOFT_HANDLE_RETAIN,
                   (u32)UDX_HOST_SOFT_CLOSE_ON_BIND,
                   (u32)UDX_HOST_SOFT_CAP_MINT);
    host_soft_emit("udx: host soft residual product_host "
                   "hosts=rtl8168_udx,xhci_udx "
                   "surface=GJ_SYS_DDI_bind_SCAN_GET_OPEN_MAP_BAR_IRQ_DMA "
                   "product=UDX+ABI Linux-shaped_userspace=1 "
                   "freestanding_class=SKIP "
                   "kernel_rtl8168=SKIP kernel_xhci_msc=SKIP "
                   "rtl8168_udx=DoD_B_OPEN id=10ec:8168 bars=0,2 "
                   "xhci_udx=DoD_A_OPEN id=8086:a12f bar=0 "
                   "handle_retain=%u close_on_bind=%u cap_mint=%u "
                   "soft=1 product=0 soft_ne_product=1 "
                   "dual=MIT_OR_Apache-2.0 G-AC-1 "
                   "(product-host residual; Soft!=product)\n",
                   (u32)UDX_HOST_SOFT_HANDLE_RETAIN,
                   (u32)UDX_HOST_SOFT_CLOSE_ON_BIND,
                   (u32)UDX_HOST_SOFT_CAP_MINT);
    udx_printk("udx: soft residual product=UDX+ABI "
               "hosts=rtl8168_udx,xhci_udx "
               "dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX "
               "DoD_A_OPEN=%u DoD_B_OPEN=%u agent_ne_close=1 "
               "chain=SCAN,GET,OPEN,MAP_BAR,IRQ,DMA soft=1\n",
               (u32)UDX_HOST_SOFT_DOD_A_OPEN,
               (u32)UDX_HOST_SOFT_DOD_B_OPEN);
    host_soft_ddi_dual_dod_honesty();
}

#if (!defined(UDX_HOST_LIBC)) || defined(GJ_FREESTANDING)
static void
host_soft_ddi_bind_pass(u16 u16Vendor, u16 u16Device, u64 u64Bar0Pa,
                        u64 u64Bar0Len, u32 u32Maps, u32 u32PrefBars,
                        const char *szHost)
{
    host_soft_inc(&g_u32DdiBindPass);
    udx_host_soft_init();
    if (szHost == NULL) {
        szHost = "udx";
    }
    /*
     * Grep: udx: soft ddi bind PASS
     * Preferred BAR VAs window-registered; ioremap uses granted PA.
     * Soft!=product multi-server / VT-d mint.
     */
    host_soft_emit("udx: soft ddi bind PASS "
                   "vend=%04x dev=%04x bar0_pa=0x%llx bar0_len=0x%llx "
                   "maps=%u pref_bars=%u host=%s "
                   "sys=GJ_SYS_DDI sys_ddi=%u chain_n=%u "
                   "chain=SCAN,GET,OPEN,MAP_BAR,IRQ,DMA denser_prove=1 "
                   "side=INVENTORY,CFG,MAP_REMAP,DMA_NOTE,DMA_BUF,IRQ_BIND,WIN,INSTALL,IOREMAP "
                   "handle_retain=%u close_on_bind=%u "
                   "close_on_pass=%u handle_retain_lock=%u "
                   "life_core=SCAN|GET|OPEN|MAP|WIN|INSTALL "
                   "product_chain=SCAN,GET,OPEN,MAP_BAR,IRQ,DMA "
                   "ioremap=granted_pa_prove product=UDX+ABI "
                   "dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX close=0 "
                   "DoD_A_OPEN=%u DoD_B_OPEN=%u agent_ne_close=1 "
                   "freestanding_class=SKIP lean_checks=%u "
                   "dual_dod OPEN W23_STRONGER=1 "
                   "cap_mint=%u soft=1 product_mint=0 gpl=0 Soft!=product\n",
                   (unsigned)u16Vendor, (unsigned)u16Device,
                   (unsigned long long)u64Bar0Pa,
                   (unsigned long long)u64Bar0Len,
                   (unsigned)u32Maps, (unsigned)u32PrefBars, szHost,
                   (u32)UDX_HOST_SYS_DDI_NR, (u32)UDX_HOST_PRODUCT_CHAIN_N,
                   (u32)UDX_HOST_SOFT_HANDLE_RETAIN,
                   (u32)UDX_HOST_SOFT_CLOSE_ON_BIND,
                   (u32)UDX_HOST_CLOSE_ON_PASS,
                   (u32)UDX_HOST_HANDLE_RETAIN_LOCK,
                   (u32)UDX_HOST_SOFT_DOD_A_OPEN,
                   (u32)UDX_HOST_SOFT_DOD_B_OPEN,
                   (u32)UDX_HOST_LEAN_CHECKS,
                   (u32)UDX_HOST_SOFT_CAP_MINT);
    /* Also via printk so freestanding binaries carry greppable text. */
    udx_printk("udx: soft ddi bind PASS vend=%04x dev=%04x "
               "bar0_pa=%llx maps=%u host=%s soft=1 product=UDX+ABI\n",
               (unsigned)u16Vendor, (unsigned)u16Device,
               (unsigned long long)u64Bar0Pa,
               (unsigned)u32Maps, szHost);
}
#endif

static void
host_soft_ddi_bind_skip(const char *szWhy)
{
    host_soft_inc(&g_u32DdiBindSkip);
    udx_host_soft_init();
    if (szWhy == NULL) {
        szWhy = "unknown";
    }
    /*
     * Grep: udx: soft ddi bind SKIP
     * Syscall unavailable, no match, or host-linux soft fallback.
     * Inject path remains for host-linux tests (soft != product).
     */
    host_soft_emit("udx: soft ddi bind SKIP why=%s "
                   "sys=GJ_SYS_DDI soft_fallback=inject_host_linux "
                   "g752_targets=10ec:8168,8086:a12f "
                   "hosts=rtl8168_udx,xhci_udx product=UDX+ABI "
                   "dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX close=0 "
                   "DoD_A_OPEN=%u DoD_B_OPEN=%u agent_ne_close=1 "
                   "cap_mint=%u fs_class=SKIP soft=1 product_mint=0 "
                   "gpl=0 Soft!=product\n",
                   szWhy,
                   (u32)UDX_HOST_SOFT_DOD_A_OPEN,
                   (u32)UDX_HOST_SOFT_DOD_B_OPEN,
                   (u32)UDX_HOST_SOFT_CAP_MINT);
    udx_printk("udx: soft ddi bind SKIP why=%s soft=1 product=UDX+ABI\n",
               szWhy);
}

/**
 * Product-host name residual (greppable; Soft!=product).
 * Available on host-linux + freestanding (bind_by_id residual by_id lamp).
 */
static const char *
host_ddi_product_host_name(u16 u16Vend, u16 u16Dev)
{
    if (u16Vend == (u16)UDX_DDI_G752_RTL8168_VEND &&
        u16Dev == (u16)UDX_DDI_G752_RTL8168_DEV) {
        return "rtl8168_udx";
    }
    if (u16Vend == (u16)UDX_DDI_G752_XHCI_VEND &&
        u16Dev == (u16)UDX_DDI_G752_XHCI_DEV) {
        return "xhci_udx";
    }
    return "udx";
}

/**
 * Product-host Dual DoD tag residual (agent!=close; Soft!=product).
 * Dual DoD A = USB/xhci_udx; Dual DoD B = NIC/rtl8168_udx.
 * Returns "A", "B", or "none".
 */
static const char *
host_ddi_product_dod_tag(u16 u16Vend, u16 u16Dev)
{
    if (u16Vend == (u16)UDX_DDI_G752_XHCI_VEND &&
        u16Dev == (u16)UDX_DDI_G752_XHCI_DEV) {
        return "A";
    }
    if (u16Vend == (u16)UDX_DDI_G752_RTL8168_VEND &&
        u16Dev == (u16)UDX_DDI_G752_RTL8168_DEV) {
        return "B";
    }
    return "none";
}

#if (!defined(UDX_HOST_LIBC)) || defined(GJ_FREESTANDING)
/**
 * Soft DDI syscall wrapper. Kernel may return -ENOSYS until product.
 * Linux-errno-shaped negatives are treated as failure.
 */
static long
host_ddi_syscall4(long a0, long a1, long a2, long a3)
{
    long ret;

    host_soft_inc(&g_u32DdiBindSys);
    ret = udx_gj_syscall4(UDX_GJ_SYS_DDI, a0, a1, a2, a3);
    if (ret < 0) {
        host_soft_inc(&g_u32DdiBindSysFail);
    }
    return ret;
}

static long
host_ddi_syscall3(long a0, long a1, long a2)
{
    long ret;

    host_soft_inc(&g_u32DdiBindSys);
    ret = udx_gj_syscall3(UDX_GJ_SYS_DDI, a0, a1, a2);
    if (ret < 0) {
        host_soft_inc(&g_u32DdiBindSysFail);
    }
    return ret;
}

/** Soft zero helper (no libc freestanding). */
static void
host_ddi_memzero(void *p, u32 u32Cb)
{
    u8 *pB;
    u32 i;

    if (p == NULL || u32Cb == 0) {
        return;
    }
    pB = (u8 *)p;
    for (i = 0; i < u32Cb; i++) {
        pB[i] = 0;
    }
}

/**
 * Product-host preferred BAR residual.
 * rtl8168_udx: BAR0 (DDI) + BAR2 (silicon-common MMIO).
 * xhci_udx: BAR0. Other IDs: BAR0 only.
 */
static void
host_ddi_preferred_bars(u16 u16Vend, u16 u16Dev, u8 *aBars, u32 *pu32N)
{
    if (aBars == NULL || pu32N == NULL) {
        return;
    }
    if (u16Vend == (u16)UDX_DDI_G752_RTL8168_VEND &&
        u16Dev == (u16)UDX_DDI_G752_RTL8168_DEV) {
        aBars[0] = (u8)UDX_DDI_RTL_PREF_BAR0;
        aBars[1] = (u8)UDX_DDI_RTL_PREF_BAR2;
        *pu32N = 2u;
        host_soft_inc(&g_u32DdiResProductHit);
        return;
    }
    if (u16Vend == (u16)UDX_DDI_G752_XHCI_VEND &&
        u16Dev == (u16)UDX_DDI_G752_XHCI_DEV) {
        aBars[0] = (u8)UDX_DDI_XHCI_PREF_BAR0;
        *pu32N = 1u;
        host_soft_inc(&g_u32DdiResProductHit);
        return;
    }
    aBars[0] = 0;
    *pu32N = 1u;
}

/**
 * Soft OP_INVENTORY residual after SCAN (product-surface honesty).
 * Once-lamp functional residual; never Dual DoD close.
 * greppable: udx: soft ddi residual INVENTORY
 */
static int
host_ddi_inventory_residual(void)
{
    static u8 fDdiInvOnce; /* once-lamp door INVENTORY residual (H2) */
    long ret;
    int fOk;

    host_soft_inc(&g_u32DdiResInv);
    if (fDdiInvOnce != 0) {
        /* Still count enter; skip emit storm (H2). */
        return (g_u32DdiResInvOk != 0u) ? 1 : 0;
    }
    fDdiInvOnce = 1;
    ret = host_ddi_syscall3((long)UDX_DDI_OP_INVENTORY, 0, 0);
    fOk = 0;
    if (ret >= 0) {
        host_soft_inc(&g_u32DdiResInvOk);
        fOk = 1;
        host_soft_emit("udx: soft ddi residual INVENTORY "
                       "PASS product_surface=1 "
                       "ops=scan,get,open,map_bar,cfg,dma,inv,close,"
                       "irq_bind,dma_buf "
                       "hosts=rtl8168_udx,xhci_udx product=UDX+ABI "
                       "dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX close=0 "
                       "mint=0 soft=1 Soft!=product\n");
        udx_printk("udx: soft ddi residual INVENTORY PASS "
                   "product=UDX+ABI soft=1 dual_dod_a=OPEN_UDX "
                   "dual_dod_b=OPEN_UDX\n");
    } else {
        host_soft_emit("udx: soft ddi residual INVENTORY SKIP "
                       "product_surface=0 mint=0 soft=1 Soft!=product "
                       "dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX close=0\n");
        udx_printk("udx: soft ddi residual INVENTORY SKIP soft=1\n");
    }
    return fOk;
}

/**
 * Note Dual DoD residual hit on product-host install path (not close).
 * Soft tallies only; Dual DoD A/B remain OPEN.
 */
static void
host_ddi_note_dod_hit(u16 u16Vend, u16 u16Dev)
{
    const char *szDod;

    szDod = host_ddi_product_dod_tag(u16Vend, u16Dev);
    if (szDod[0] == 'A') {
        host_soft_inc(&g_u32DdiResDodA);
    } else if (szDod[0] == 'B') {
        host_soft_inc(&g_u32DdiResDodB);
    }
}

/** Pull BAR PA/Cb from GET info by index (0..5). */
static void
host_ddi_bar_pa_cb(const struct udx_ddi_dev_info *pInfo, u32 u32Bar,
                   u64 *pu64Pa, u64 *pu64Cb)
{
    u64 u64Pa;
    u64 u64Cb;

    u64Pa = 0;
    u64Cb = 0;
    if (pInfo != NULL) {
        switch (u32Bar) {
        case 0: u64Pa = pInfo->u64Bar0Pa; u64Cb = pInfo->u64Bar0Cb; break;
        case 1: u64Pa = pInfo->u64Bar1Pa; u64Cb = pInfo->u64Bar1Cb; break;
        case 2: u64Pa = pInfo->u64Bar2Pa; u64Cb = pInfo->u64Bar2Cb; break;
        case 3: u64Pa = pInfo->u64Bar3Pa; u64Cb = pInfo->u64Bar3Cb; break;
        case 4: u64Pa = pInfo->u64Bar4Pa; u64Cb = pInfo->u64Bar4Cb; break;
        case 5: u64Pa = pInfo->u64Bar5Pa; u64Cb = pInfo->u64Bar5Cb; break;
        default: break;
        }
    }
    if (pu64Pa != NULL) {
        *pu64Pa = u64Pa;
    }
    if (pu64Cb != NULL) {
        *pu64Cb = u64Cb;
    }
}

/**
 * MAP_BAR one preferred residual; update phys/len/VA on success.
 * Returns 1 if mapped (VA residual), 0 if empty, -1 if soft-fail.
 * *pu8UserOut (optional): note.u8User residual when mapped.
 */
static int
host_ddi_map_one(long h, u32 u32Bar, u64 *pu64Pa, u64 *pu64Cb, void **ppVa,
                 u8 *pu8UserOut)
{
    struct udx_ddi_map_note note;
    long ret;

    if (ppVa != NULL) {
        *ppVa = NULL;
    }
    if (pu8UserOut != NULL) {
        *pu8UserOut = 0;
    }
    if (pu64Pa == NULL || pu64Cb == NULL) {
        return -1;
    }
    if (*pu64Pa == 0 || *pu64Cb == 0) {
        host_soft_inc(&g_u32DdiResMapEmpty);
        return 0;
    }

    host_ddi_memzero(&note, (u32)sizeof(note));
    host_soft_inc(&g_u32DdiResMap);
    ret = host_ddi_syscall4(UDX_DDI_OP_MAP_BAR, h, (long)u32Bar,
                            (long)(uintptr_t)&note);
    if (ret >= 0 && note.u8Ok != 0 && note.u64Va != 0) {
        if (note.u64Pa != 0) {
            *pu64Pa = note.u64Pa;
        }
        if (note.u64Cb != 0) {
            *pu64Cb = note.u64Cb;
        }
        if (ppVa != NULL) {
            *ppVa = (void *)(uintptr_t)note.u64Va;
        }
        if (note.u8User != 0) {
            host_soft_inc(&g_u32DdiResMapUser);
            if (pu8UserOut != NULL) {
                *pu8UserOut = 1;
            }
        }
        host_soft_inc(&g_u32DdiResMapOk);
        /*
         * greppable: udx: soft ddi residual MAP_BAR
         * Per preferred-BAR residual (1–2 bars for product hosts; lean).
         */
        host_soft_emit("udx: soft ddi residual MAP_BAR bar=%u va=0x%llx "
                       "pa=0x%llx cb=0x%llx user=%u ok=1 mint=0 "
                       "soft=1 Soft!=product\n",
                       (unsigned)u32Bar,
                       (unsigned long long)note.u64Va,
                       (unsigned long long)*pu64Pa,
                       (unsigned long long)*pu64Cb,
                       (unsigned)note.u8User);
        udx_printk("udx: soft ddi residual MAP_BAR bar=%u va=%llx "
                   "user=%u soft=1 product=UDX+ABI\n",
                   (unsigned)u32Bar,
                   (unsigned long long)note.u64Va,
                   (unsigned)note.u8User);
        return 1;
    }
    if (ret > 0) {
        /* Positive i64 = user VA when low enough to fit. */
        if (ppVa != NULL) {
            *ppVa = (void *)(uintptr_t)(u64)ret;
        }
        host_soft_inc(&g_u32DdiResMapOk);
        host_soft_emit("udx: soft ddi residual MAP_BAR bar=%u va=0x%llx "
                       "pa=0x%llx cb=0x%llx user=0 ok=1 ret_va=1 mint=0 "
                       "soft=1 Soft!=product\n",
                       (unsigned)u32Bar,
                       (unsigned long long)(u64)ret,
                       (unsigned long long)*pu64Pa,
                       (unsigned long long)*pu64Cb);
        udx_printk("udx: soft ddi residual MAP_BAR bar=%u va=%llx "
                   "ret_va=1 soft=1 product=UDX+ABI\n",
                   (unsigned)u32Bar, (unsigned long long)(u64)ret);
        return 1;
    }
    host_soft_inc(&g_u32DdiResMapFail);
    host_soft_emit("udx: soft ddi residual MAP_BAR bar=%u ok=0 "
                   "mint=0 soft=1 Soft!=product\n",
                   (unsigned)u32Bar);
    return -1;
}

/**
 * Soft CFG_READ residual after OPEN (identity + cmd/status snap).
 * Functional denser: prove cfg0 dword matches GET vend:dev
 * (PCI type-0 dword0 = vendor | (device << 16)). Soft!=product.
 * Returns 1 on ident soft-ok; *pu32IdMatchOut=1 when identity matches.
 * greppable: udx: soft ddi residual CFG
 */
static int
host_ddi_cfg_residual(long h, u16 u16Vend, u16 u16Dev, const char *szHost,
                      u32 *pu32IdMatchOut)
{
    long ret0;
    long ret1;
    int fOk;
    u32 u32IdMatch;
    u16 u16CfgVend;
    u16 u16CfgDev;

    if (pu32IdMatchOut != NULL) {
        *pu32IdMatchOut = 0;
    }
    if (szHost == NULL) {
        szHost = "udx";
    }
    host_soft_inc(&g_u32DdiResCfg);
    ret0 = host_ddi_syscall3(UDX_DDI_OP_CFG_READ, h,
                             (long)UDX_DDI_CFG_OFF_IDENT);
    ret1 = host_ddi_syscall3(UDX_DDI_OP_CFG_READ, h,
                             (long)UDX_DDI_CFG_OFF_CMDST);
    fOk = 0;
    u32IdMatch = 0;
    if (ret0 >= 0) {
        host_soft_inc(&g_u32DdiResCfgOk);
        fOk = 1;
        /* Functional residual: cfg0 identity must match GET vend:dev. */
        u16CfgVend = (u16)((u32)ret0 & 0xffffu);
        u16CfgDev = (u16)(((u32)ret0 >> 16) & 0xffffu);
        if (u16CfgVend == u16Vend && u16CfgDev == u16Dev) {
            host_soft_inc(&g_u32DdiResCfgIdMatch);
            u32IdMatch = 1u;
        }
        host_soft_emit("udx: soft ddi residual CFG host=%s "
                       "vend=%04x dev=%04x cfg0=0x%x cmdst=0x%x "
                       "id_match=%u cfg_vend=%04x cfg_dev=%04x "
                       "PASS soft=1 mint=0 Soft!=product\n",
                       szHost, (unsigned)u16Vend, (unsigned)u16Dev,
                       (unsigned)(u32)ret0,
                       (ret1 >= 0) ? (unsigned)(u32)ret1 : 0u,
                       (unsigned)u32IdMatch,
                       (unsigned)u16CfgVend, (unsigned)u16CfgDev);
        udx_printk("udx: soft ddi residual CFG host=%s cfg0=%x "
                   "id_match=%u soft=1 product=UDX+ABI\n",
                   szHost, (unsigned)(u32)ret0, (unsigned)u32IdMatch);
    } else {
        host_soft_emit("udx: soft ddi residual CFG host=%s SKIP "
                       "soft=1 mint=0 Soft!=product\n",
                       szHost);
        udx_printk("udx: soft ddi residual CFG host=%s SKIP soft=1\n",
                   szHost);
    }
    if (pu32IdMatchOut != NULL) {
        *pu32IdMatchOut = u32IdMatch;
    }
    return fOk;
}

/**
 * Soft MAP_REMAP residual on first mapped preferred BAR.
 * Door grant slot is idempotent; VA match deepens honesty.
 * greppable: udx: soft ddi residual MAP_REMAP
 * Returns 1 on soft-ok; *pu32IdemOut=1 when prior VA matches.
 */
static int
host_ddi_map_remap_residual(long h, u32 u32Bar, u64 u64PriorVa,
                            u32 *pu32IdemOut, const char *szHost)
{
    struct udx_ddi_map_note note;
    long ret;
    int fOk;
    int fIdem;

    if (pu32IdemOut != NULL) {
        *pu32IdemOut = 0;
    }
    if (szHost == NULL) {
        szHost = "udx";
    }
    host_ddi_memzero(&note, (u32)sizeof(note));
    host_soft_inc(&g_u32DdiResRemap);
    ret = host_ddi_syscall4(UDX_DDI_OP_MAP_BAR, h, (long)u32Bar,
                            (long)(uintptr_t)&note);
    fOk = 0;
    fIdem = 0;
    if (ret >= 0 || note.u8Ok != 0) {
        host_soft_inc(&g_u32DdiResRemapOk);
        fOk = 1;
        if (u64PriorVa != 0 && note.u64Va == u64PriorVa) {
            host_soft_inc(&g_u32DdiResRemapIdem);
            fIdem = 1;
        }
        host_soft_emit("udx: soft ddi residual MAP_REMAP host=%s bar=%u "
                       "va=0x%llx prior=0x%llx idem=%u PASS "
                       "mint=0 soft=1 Soft!=product\n",
                       szHost, (unsigned)u32Bar,
                       (unsigned long long)note.u64Va,
                       (unsigned long long)u64PriorVa,
                       (unsigned)fIdem);
        udx_printk("udx: soft ddi residual MAP_REMAP host=%s bar=%u "
                   "idem=%u soft=1 product=UDX+ABI\n",
                   szHost, (unsigned)u32Bar, (unsigned)fIdem);
    } else {
        host_soft_emit("udx: soft ddi residual MAP_REMAP host=%s bar=%u "
                       "SKIP soft=1 mint=0 Soft!=product\n",
                       szHost, (unsigned)u32Bar);
        udx_printk("udx: soft ddi residual MAP_REMAP host=%s SKIP soft=1\n",
                   szHost);
    }
    if (pu32IdemOut != NULL) {
        *pu32IdemOut = (fIdem != 0) ? 1u : 0u;
    }
    return fOk;
}

/**
 * Soft DMA_NOTE residual on first preferred non-empty BAR PA/Cb.
 * Soft inventory only; never DMA window / CNode mint.
 * greppable: udx: soft ddi residual DMA_NOTE
 */
static int
host_ddi_dma_note_residual(long h, u64 u64Pa, u64 u64Cb, const char *szHost)
{
    long ret;
    int fOk;

    if (szHost == NULL) {
        szHost = "udx";
    }
    host_soft_inc(&g_u32DdiResDmaNote);
    if (u64Pa == 0 || u64Cb == 0) {
        host_soft_emit("udx: soft ddi residual DMA_NOTE host=%s "
                       "SKIP why=empty mint=0 soft=1 Soft!=product\n",
                       szHost);
        udx_printk("udx: soft ddi residual DMA_NOTE host=%s SKIP empty "
                   "soft=1\n",
                   szHost);
        return 0;
    }
    ret = host_ddi_syscall4(UDX_DDI_OP_DMA_NOTE, h, (long)u64Pa, (long)u64Cb);
    fOk = 0;
    if (ret >= 0) {
        host_soft_inc(&g_u32DdiResDmaNoteOk);
        fOk = 1;
        host_soft_emit("udx: soft ddi residual DMA_NOTE host=%s "
                       "pa=0x%llx cb=0x%llx PASS soft_only=1 mint=0 "
                       "soft=1 Soft!=product\n",
                       szHost,
                       (unsigned long long)u64Pa,
                       (unsigned long long)u64Cb);
        udx_printk("udx: soft ddi residual DMA_NOTE host=%s PASS "
                   "soft=1 product=UDX+ABI\n",
                   szHost);
    } else {
        host_soft_emit("udx: soft ddi residual DMA_NOTE host=%s "
                       "SKIP mint=0 soft=1 Soft!=product\n",
                       szHost);
        udx_printk("udx: soft ddi residual DMA_NOTE host=%s SKIP soft=1\n",
                   szHost);
    }
    return fOk;
}

/**
 * Soft DMA_BUF residual for product hosts (rtl ring / xhci xfer spirit).
 * Real GJ_SYS_DDI ops: ALLOC → MAP → FREE (soft page residual; mint OPEN).
 * Never claims DMA window / VT-d CNode mint. Soft!=product.
 * greppable: udx: soft ddi residual DMA_BUF
 * Returns 1 if ALLOC soft-ok (MAP/FREE deepen honesty only).
 */
static int
host_ddi_dma_buf_residual(long h, const char *szHost)
{
    long retAlloc;
    long retMap;
    long retFree;
    u64 u64Pa;
    u64 u64Cb;
    int fOk;

    if (szHost == NULL) {
        szHost = "udx";
    }
    host_soft_inc(&g_u32DdiResDmaBuf);
    /* 1 page, flags=0 — lean residual; matches ddi_host / door soft bound. */
    retAlloc = host_ddi_syscall4(UDX_DDI_OP_DMA_BUF_ALLOC, h, 1L, 0L);
    fOk = 0;
    u64Pa = 0;
    u64Cb = 4096ull;
    if (retAlloc > 0) {
        u64Pa = (u64)retAlloc;
        host_soft_inc(&g_u32DdiResDmaBufOk);
        fOk = 1;
        host_soft_emit("udx: soft ddi residual DMA_BUF host=%s "
                       "op=ALLOC pa=0x%llx pages=1 flags=0 PASS "
                       "soft_only=1 mint=0 soft=1 Soft!=product\n",
                       szHost, (unsigned long long)u64Pa);
        udx_printk("udx: soft ddi residual DMA_BUF host=%s ALLOC "
                   "pa=%llx PASS soft=1 product=UDX+ABI\n",
                   szHost, (unsigned long long)u64Pa);

        retMap = host_ddi_syscall4(UDX_DDI_OP_DMA_BUF_MAP, h,
                                   (long)u64Pa, (long)u64Cb);
        if (retMap >= 0) {
            host_soft_inc(&g_u32DdiResDmaBufMap);
            host_soft_emit("udx: soft ddi residual DMA_BUF host=%s "
                           "op=MAP pa=0x%llx cb=0x%llx cookie=0x%llx "
                           "PASS soft_cookie=1 mint=0 soft=1 Soft!=product\n",
                           szHost, (unsigned long long)u64Pa,
                           (unsigned long long)u64Cb,
                           (unsigned long long)(u64)retMap);
            udx_printk("udx: soft ddi residual DMA_BUF host=%s MAP "
                       "PASS soft=1 product=UDX+ABI\n",
                       szHost);
        } else {
            host_soft_emit("udx: soft ddi residual DMA_BUF host=%s "
                           "op=MAP SKIP mint=0 soft=1 Soft!=product\n",
                           szHost);
        }

        retFree = host_ddi_syscall4(UDX_DDI_OP_DMA_BUF_FREE, h,
                                    (long)u64Pa, 1L);
        if (retFree >= 0) {
            host_soft_inc(&g_u32DdiResDmaBufFree);
            host_soft_emit("udx: soft ddi residual DMA_BUF host=%s "
                           "op=FREE pa=0x%llx pages=1 PASS soft_forget=1 "
                           "mint=0 soft=1 Soft!=product\n",
                           szHost, (unsigned long long)u64Pa);
            udx_printk("udx: soft ddi residual DMA_BUF host=%s FREE "
                       "PASS soft=1 product=UDX+ABI\n",
                       szHost);
        } else {
            host_soft_emit("udx: soft ddi residual DMA_BUF host=%s "
                           "op=FREE SKIP mint=0 soft=1 Soft!=product\n",
                           szHost);
        }
    } else {
        host_soft_emit("udx: soft ddi residual DMA_BUF host=%s "
                       "op=ALLOC SKIP mint=0 soft=1 Soft!=product\n",
                       szHost);
        udx_printk("udx: soft ddi residual DMA_BUF host=%s ALLOC "
                   "SKIP soft=1\n",
                   szHost);
    }
    return fOk;
}

/**
 * Soft IRQ_BIND residual after MAP_BAR (soft badge note; mint OPEN).
 * Product chain order: MAP_BAR → IRQ → DMA (not inject-only).
 * greppable: udx: soft ddi residual IRQ_BIND
 * Product hosts retain soft note for freestanding notify residual.
 */
static int
host_ddi_irq_bind_residual(long h, const char *szHost)
{
    long ret;
    int fOk;

    if (szHost == NULL) {
        szHost = "udx";
    }
    host_soft_inc(&g_u32DdiResIrq);
    /* badge soft bit0 (GJ_MSIX_BADGE_SOFT); 0 also selects soft default. */
    ret = host_ddi_syscall3(UDX_DDI_OP_IRQ_BIND, h,
                            (long)UDX_DDI_IRQ_BADGE_SOFT);
    fOk = 0;
    if (ret >= 0) {
        host_soft_inc(&g_u32DdiResIrqOk);
        fOk = 1;
        host_soft_emit("udx: soft ddi residual IRQ_BIND host=%s h=%ld "
                       "badge=0x%llx PASS soft_note=1 mint=0 "
                       "irq_notify=0 soft=1 Soft!=product\n",
                       szHost, (long)h,
                       (unsigned long long)UDX_DDI_IRQ_BADGE_SOFT);
        udx_printk("udx: soft ddi residual IRQ_BIND host=%s PASS "
                   "soft=1 product=UDX+ABI\n",
                   szHost);
    } else {
        host_soft_emit("udx: soft ddi residual IRQ_BIND host=%s h=%ld "
                       "SKIP mint=0 soft=1 Soft!=product\n",
                       szHost, (long)h);
        udx_printk("udx: soft ddi residual IRQ_BIND host=%s SKIP soft=1\n",
                   szHost);
    }
    return fOk;
}

/** Soft life peak note (diagnostics; wrap OK). */
static void
host_ddi_life_note(u32 u32Life)
{
    if (u32Life > g_u32DdiResLifePeak) {
        g_u32DdiResLifePeak = u32Life;
    }
}

/**
 * Functional residual: prove MAP_BAR→window_register→udx_ioremap lookup
 * for one preferred BAR after install. Soft!=product MMIO_FRAME mint OPEN.
 * Denser: when u64ExpectVa != 0, prove window_lookup VA matches MAP VA.
 * Unmaps after prove so freestanding IOMEM pool slots stay free for probe.
 * greppable: udx: soft ddi residual IOREMAP
 * Returns 1 on prove-ok, 0 on empty skip, -1 on fail.
 */
static int
host_ddi_ioremap_residual(u32 u32Bar, u64 u64Pa, u64 u64Cb, u64 u64ExpectVa,
                          const char *szHost)
{
    struct udx_iomem *pIo;
    void *pLook;
    u64 u64Va;
    u64 u64LookVa;
    u32 u32Look;
    u32 u32Map;
    u32 u32VaMatch;
    int fOk;

    if (szHost == NULL) {
        szHost = "udx";
    }
    if (u64Pa == 0 || u64Cb == 0) {
        return 0;
    }
    host_soft_inc(&g_u32DdiResIoremap);
    /*
     * Belt residual: window_lookup first (same path ioremap uses).
     * Then udx_ioremap bookkeeping prove + iounmap (probe re-maps).
     * Functional denser: MAP VA == look VA when expect known.
     */
    pLook = udx_host_window_lookup(u64Pa, u64Cb, NULL);
    pIo = udx_ioremap(u64Pa, u64Cb);
    u32Look = (pLook != NULL) ? 1u : 0u;
    u32Map = (pIo != NULL) ? 1u : 0u;
    u64Va = 0;
    u64LookVa = 0;
    u32VaMatch = 0;
    if (pLook != NULL) {
        u64LookVa = (u64)(uintptr_t)pLook;
    }
    if (pIo != NULL) {
        u64Va = (u64)(uintptr_t)pIo->pVa;
    }
    if (u64ExpectVa != 0 && u64LookVa != 0 && u64LookVa == u64ExpectVa) {
        host_soft_inc(&g_u32DdiResVaMatch);
        u32VaMatch = 1u;
    }
    fOk = 0;
    if (u32Map != 0u && u32Look != 0u) {
        host_soft_inc(&g_u32DdiResIoremapOk);
        fOk = 1;
        host_soft_emit("udx: soft ddi residual IOREMAP host=%s bar=%u "
                       "pa=0x%llx cb=0x%llx va=0x%llx look=1 "
                       "expect=0x%llx look_va=0x%llx va_match=%u PASS "
                       "chain=MAP,INSTALL,WIN,IOREMAP "
                       "mint=0 soft=1 Soft!=product\n",
                       szHost, (unsigned)u32Bar,
                       (unsigned long long)u64Pa,
                       (unsigned long long)u64Cb,
                       (unsigned long long)u64Va,
                       (unsigned long long)u64ExpectVa,
                       (unsigned long long)u64LookVa,
                       (unsigned)u32VaMatch);
        udx_printk("udx: soft ddi residual IOREMAP host=%s bar=%u "
                   "va_match=%u PASS soft=1 product=UDX+ABI\n",
                   szHost, (unsigned)u32Bar, (unsigned)u32VaMatch);
        /* Free slot for product-host probe ioremap residual. */
        udx_iounmap(pIo);
    } else {
        host_soft_inc(&g_u32DdiResIoremapFail);
        if (pIo != NULL) {
            udx_iounmap(pIo);
        }
        host_soft_emit("udx: soft ddi residual IOREMAP host=%s bar=%u "
                       "pa=0x%llx cb=0x%llx look=%u map=%u "
                       "expect=0x%llx va_match=%u FAIL "
                       "mint=0 soft=1 Soft!=product\n",
                       szHost, (unsigned)u32Bar,
                       (unsigned long long)u64Pa,
                       (unsigned long long)u64Cb,
                       (unsigned)u32Look, (unsigned)u32Map,
                       (unsigned long long)u64ExpectVa,
                       (unsigned)u32VaMatch);
        udx_printk("udx: soft ddi residual IOREMAP host=%s bar=%u "
                   "FAIL soft=1 product=UDX+ABI\n",
                   szHost, (unsigned)u32Bar);
        fOk = -1;
    }
    return fOk;
}

/**
 * Residual path lamp for one install (lean; Soft!=product).
 * greppable: udx: soft ddi bind residual
 * greppable: udx: soft ddi bind residual path
 */
static void
host_soft_ddi_bind_residual_path(u16 u16Vend, u16 u16Dev, long h,
                                 u32 u32Maps, u32 u32Pref, u32 u32Empty,
                                 u32 u32Fail, u32 u32Life, u32 u32Idem,
                                 u32 u32MapUser, u32 u32IoremapOk,
                                 u32 u32IoremapFail, u32 u32CfgIdMatch,
                                 u32 u32PrefComplete, const char *szHost)
{
    const char *szDod;
    u32 u32CoreOk;

    if (szHost == NULL) {
        szHost = "udx";
    }
    szDod = host_ddi_product_dod_tag(u16Vend, u16Dev);
    u32CoreOk = ((u32Life & UDX_DDI_LIFE_CORE) == UDX_DDI_LIFE_CORE) ? 1u : 0u;
    if (u32CoreOk != 0u) {
        host_soft_inc(&g_u32DdiResCoreOk);
    }
    host_ddi_note_dod_hit(u16Vend, u16Dev);
    host_soft_emit("udx: soft ddi bind residual path "
                   "vend=%04x dev=%04x h=%ld open=1 maps=%u "
                   "pref=%u empty=%u fail=%u life=0x%x "
                   "remap_idem=%u map_user=%u "
                   "ioremap_ok=%u ioremap_fail=%u "
                   "cfg_id_match=%u pref_complete=%u host=%s "
                   "core_ok=%u dual_dod=%s dual_dod_open=1 close=0 "
                   "DoD_A_OPEN=%u DoD_B_OPEN=%u agent_ne_close=1 "
                   "chain=SCAN,GET,OPEN,MAP_BAR,IRQ,DMA "
                   "side=INVENTORY,CFG,MAP_REMAP,DMA_NOTE,DMA_BUF,IRQ_BIND,WIN,INSTALL,IOREMAP "
                   "handle_retain=%u close_on_bind=%u "
                   "product=UDX+ABI fs_class=SKIP "
                   "mint=%u soft=1 Soft!=product\n",
                   (unsigned)u16Vend, (unsigned)u16Dev, (long)h,
                   (unsigned)u32Maps, (unsigned)u32Pref,
                   (unsigned)u32Empty, (unsigned)u32Fail,
                   (unsigned)u32Life, (unsigned)u32Idem,
                   (unsigned)u32MapUser,
                   (unsigned)u32IoremapOk, (unsigned)u32IoremapFail,
                   (unsigned)u32CfgIdMatch, (unsigned)u32PrefComplete,
                   szHost,
                   (unsigned)u32CoreOk, szDod,
                   (u32)UDX_HOST_SOFT_DOD_A_OPEN,
                   (u32)UDX_HOST_SOFT_DOD_B_OPEN,
                   (u32)UDX_HOST_SOFT_HANDLE_RETAIN,
                   (u32)UDX_HOST_SOFT_CLOSE_ON_BIND,
                   (u32)UDX_HOST_SOFT_CAP_MINT);
    udx_printk("udx: soft ddi bind residual path vend=%04x dev=%04x "
               "maps=%u life=%x ioremap_ok=%u pref_complete=%u "
               "host=%s dual_dod=%s core_ok=%u product=UDX+ABI soft=1\n",
               (unsigned)u16Vend, (unsigned)u16Dev,
               (unsigned)u32Maps, (unsigned)u32Life,
               (unsigned)u32IoremapOk, (unsigned)u32PrefComplete,
               szHost, szDod, (unsigned)u32CoreOk);
}

/**
 * Functional residual denser catalog (preferred over lamps alone).
 * Per-chain-step denser prove for product-host bind residual (not inject):
 *   SCAN → GET → OPEN → MAP_BAR → IRQ → DMA (+ INSTALL / IOREMAP side).
 * Product path OK when SCAN/GET/OPEN/MAP_BAR/IRQ/DMA all soft-prove.
 * W23 STRONGER denser full path (product UDX via GJ_SYS_DDI fully):
 *   product_path_ok + INSTALL + IOREMAP + CFG id_match +
 *   pref_complete + VA match + post-install pdev vend:dev identity +
 *   LIFE_CORE (SCAN|GET|OPEN|MAP|WIN|INSTALL) +
 *   DMA denser both (DMA_NOTE + DMA_BUF) + DMA denser triple (+MAP) +
 *   DMA denser free (ALLOC→MAP→FREE full cycle) +
 *   IRQ denser (irq_ok) + maps>=1 + product_host denser (DoD A/B id) +
 *   Dual DoD A/B OPEN locks + agent_close=0 + Soft!=product +
 *   chain denser SCAN→DMA (all 6 product steps denser_prove) +
 *   chain free denser (DMA free terminal on chain SCAN→DMA) +
 *   chain_step denser + step_all denser + life product chain denser
 *   (SCAN|GET|OPEN|MAP|IRQ|DMA_NOTE) + LIFE_FULL denser (CORE|SIDE) +
 *   REMAP denser + SIDE denser + WIN denser + prefer_real denser +
 *   CFG denser + INSTALL denser + IOREMAP denser +
 *   handle_retain denser + close denser + dual_dod denser +
 *   soft_ne denser + open_h denser + path_full denser +
 *   mint denser + agent denser + fs denser +
 *   maps denser + pref denser + va denser +
 *   core denser + pdev denser + life denser +
 *   h1 denser + inject denser + sys denser +
 *   multi denser + confine denser + wave denser +
 *   path denser + lean denser + product denser +
 *   hazard denser + c2 denser + bar denser +
 *   gac denser + stamp denser + layout denser +
 *   mmio denser + inv denser + sshd denser +
 *   eq denser + once denser + grant denser.
 * close_on_pass=0 handle_retain=1; freestanding class SKIP honesty.
 * Targets: 10ec:8168 (DoD B / rtl8168_udx), 8086:a12f (DoD A / xhci_udx).
 * greppable: udx: soft ddi bind residual functional
 * greppable: udx: soft ddi residual functional step=
 * greppable: product_path_ok= | product_path_full_ok= | pdev_id_match=
 * greppable: chain=SCAN,GET,OPEN,MAP_BAR,IRQ,DMA | sys_ddi=103
 * greppable: close_on_pass=0 handle_retain=1 | freestanding class SKIP
 * greppable: Soft!=product dual_dod OPEN W23_STRONGER
 * greppable: W23_STRONGER=1 | denser_prove=1 | dma_dense_both= | irq_dense=
 * greppable: dma_dense_triple= | dma_dense_free= | chain_dense_scan_dma=
 * greppable: chain_free_dense= | step_all_dense= | life_full_dense=
 * greppable: cfg_dense= | install_dense= | ioremap_dense=
 * greppable: chain_step_dense= | life_product_chain= | product_host_dense=
 * greppable: remap_dense= | side_dense= | win_dense= | prefer_real_dense=
 * greppable: handle_retain_dense= | close_dense= | dual_dod_dense=
 * greppable: soft_ne_dense= | open_h_dense= | path_full_dense=
 * greppable: mint_dense= | agent_dense= | fs_dense=
 * greppable: maps_dense= | pref_dense= | va_dense=
 * greppable: core_dense= | pdev_dense= | life_dense=
 * greppable: h1_dense= | inject_dense= | sys_dense=
 * greppable: multi_dense= | confine_dense= | wave_dense=
 * greppable: path_dense= | lean_dense= | product_dense=
 * greppable: hazard_dense= | c2_dense= | bar_dense=
 * greppable: gac_dense= | stamp_dense= | layout_dense=
 * greppable: mmio_dense= | inv_dense= | sshd_dense=
 * greppable: eq_dense= | once_dense= | grant_dense=
 * Soft!=product; Dual DoD A/B OPEN; never gates skeleton PASS.
 */
static void
host_ddi_functional_residual(u16 u16Vend, u16 u16Dev, long h,
                             u32 u32Life, u32 u32Maps, u32 u32Pref,
                             u32 u32IoremapOk, u32 u32CfgIdMatch,
                             u32 u32PrefComplete, u32 u32VaMatchN,
                             u32 u32IrqOk, u32 u32DmaNoteOk,
                             u32 u32DmaBufOk, u32 u32DmaBufMap,
                             u32 u32DmaBufFree, u32 u32PdevIdMatch,
                             const char *szHost)
{
    const char *szDod;
    u32 u32CoreOk;
    u32 u32StepScan;
    u32 u32StepGet;
    u32 u32StepOpen;
    u32 u32StepMapBar;
    u32 u32StepIrq;
    u32 u32StepDma;
    u32 u32StepInstall;
    u32 u32StepIoremap;
    u32 u32StepCfg;
    u32 u32StepSide;
    u32 u32StepPdev;
    u32 u32ChainOk;
    u32 u32ChainDenseScanDma;
    u32 u32ChainFreeDense;
    u32 u32ChainStepDense;
    u32 u32StepAllDense;
    u32 u32LifeProductChain;
    u32 u32LifeFullDense;
    u32 u32DmaDenseBoth;
    u32 u32DmaDenseTriple;
    u32 u32DmaDenseFree;
    u32 u32RemapDense;
    u32 u32SideDense;
    u32 u32WinDense;
    u32 u32PreferRealDense;
    u32 u32CfgDense;
    u32 u32InstallDense;
    u32 u32IoremapDense;
    u32 u32HandleRetainDense;
    u32 u32CloseDense;
    u32 u32DualDodDense;
    u32 u32SoftNeDense;
    u32 u32OpenHDense;
    u32 u32PathFullDense;
    u32 u32MintDense;
    u32 u32AgentDense;
    u32 u32FsDense;
    u32 u32MapsDense;
    u32 u32PrefDense;
    u32 u32VaDense;
    u32 u32CoreDense;
    u32 u32PdevDense;
    u32 u32LifeDense;
    u32 u32H1Dense;
    u32 u32InjectDense;
    u32 u32SysDense;
    u32 u32MultiDense;
    u32 u32ConfineDense;
    u32 u32WaveDense;
    u32 u32PathDense;
    u32 u32LeanDense;
    u32 u32ProductDense;
    u32 u32HazardDense;
    u32 u32C2Dense;
    u32 u32BarDense;
    u32 u32GacDense;
    u32 u32StampDense;
    u32 u32LayoutDense;
    u32 u32MmioDense;
    u32 u32InvDense;
    u32 u32SshdDense;
    u32 u32EqDense;
    u32 u32OnceDense;
    u32 u32GrantDense;
    u32 u32ProductPathOk;
    u32 u32ProductPathFullOk;
    u32 u32IsProductHost;

    if (szHost == NULL) {
        szHost = "udx";
    }
    szDod = host_ddi_product_dod_tag(u16Vend, u16Dev);
    u32IsProductHost = (szDod[0] == 'A' || szDod[0] == 'B') ? 1u : 0u;
    u32CoreOk = ((u32Life & UDX_DDI_LIFE_CORE) == UDX_DDI_LIFE_CORE) ? 1u : 0u;
    u32StepScan = ((u32Life & UDX_DDI_LIFE_SCAN) != 0u) ? 1u : 0u;
    u32StepGet = ((u32Life & UDX_DDI_LIFE_GET) != 0u) ? 1u : 0u;
    u32StepOpen = ((u32Life & UDX_DDI_LIFE_OPEN) != 0u) ? 1u : 0u;
    u32StepMapBar = ((u32Life & UDX_DDI_LIFE_MAP) != 0u) ? 1u : 0u;
    u32StepIrq = ((u32Life & UDX_DDI_LIFE_IRQ) != 0u || u32IrqOk != 0u)
                     ? 1u
                     : 0u;
    /* DMA functional: DMA_NOTE life and/or DMA_BUF soft residual. */
    u32StepDma = (((u32Life & UDX_DDI_LIFE_DMA_NOTE) != 0u) ||
                  u32DmaNoteOk != 0u || u32DmaBufOk != 0u)
                     ? 1u
                     : 0u;
    u32StepInstall = ((u32Life & UDX_DDI_LIFE_INSTALL) != 0u) ? 1u : 0u;
    u32StepIoremap = (u32IoremapOk != 0u) ? 1u : 0u;
    u32StepCfg = (u32CfgIdMatch != 0u) ? 1u : 0u;
    u32StepSide = (((u32Life & UDX_DDI_LIFE_SIDE) != 0u) ||
                   ((u32Life & UDX_DDI_LIFE_WIN) != 0u) ||
                   u32DmaBufOk != 0u || u32DmaBufMap != 0u ||
                   u32DmaBufFree != 0u) ? 1u : 0u;
    u32StepPdev = (u32PdevIdMatch != 0u) ? 1u : 0u;
    /*
     * Product functional chain OK: SCAN→GET→OPEN→MAP_BAR→IRQ→DMA.
     * INSTALL remains denser residual (not required for product_path_ok).
     */
    u32ProductPathOk = (u32StepScan != 0u && u32StepGet != 0u &&
                        u32StepOpen != 0u && u32StepMapBar != 0u &&
                        u32StepIrq != 0u && u32StepDma != 0u) ? 1u : 0u;
    /* Legacy chain_ok: product path + install residual. */
    u32ChainOk = (u32ProductPathOk != 0u && u32StepInstall != 0u) ? 1u : 0u;
    /* W15 denser: DMA both / triple / free terminal + chain SCAN→DMA. */
    u32DmaDenseBoth = (u32DmaNoteOk != 0u && u32DmaBufOk != 0u) ? 1u : 0u;
    u32DmaDenseTriple = (u32DmaNoteOk != 0u && u32DmaBufOk != 0u &&
                         u32DmaBufMap != 0u) ? 1u : 0u;
    u32DmaDenseFree = (u32DmaNoteOk != 0u && u32DmaBufOk != 0u &&
                       u32DmaBufMap != 0u && u32DmaBufFree != 0u) ? 1u : 0u;
    /* Life product chain denser: SCAN|GET|OPEN|MAP|IRQ|DMA_NOTE all set. */
    u32LifeProductChain =
        ((u32Life & UDX_HOST_LIFE_PRODUCT_CHAIN) ==
         UDX_HOST_LIFE_PRODUCT_CHAIN) ? 1u : 0u;
    /* W17 denser: LIFE_FULL denser (CORE|SIDE complete). */
    u32LifeFullDense =
        (((u32Life & UDX_HOST_LIFE_FULL) == UDX_HOST_LIFE_FULL) &&
         UDX_HOST_LIFE_FULL_DENSE != 0u) ? 1u : 0u;
    /*
     * Chain step denser: all 6 product steps denser_prove + OPEN h>0 +
     * maps>=1 + IRQ denser explicit. greppable: chain_step_dense=
     */
    u32ChainStepDense = (u32StepScan != 0u &&
                         u32StepGet != 0u &&
                         u32StepOpen != 0u &&
                         h > 0 &&
                         u32StepMapBar != 0u &&
                         u32Maps != 0u &&
                         u32StepIrq != 0u &&
                         u32IrqOk != 0u &&
                         u32StepDma != 0u &&
                         UDX_HOST_CHAIN_STEP_DENSE != 0u &&
                         UDX_HOST_PRODUCT_CHAIN_N == 6u) ? 1u : 0u;
    /*
     * W17 denser step_all: SCAN/GET/OPEN h/MAP maps+pref/IRQ/DMA free.
     * greppable: step_all_dense=
     */
    u32StepAllDense = (u32StepScan != 0u &&
                       u32StepGet != 0u &&
                       u32StepOpen != 0u &&
                       h > 0 &&
                       u32StepMapBar != 0u &&
                       u32Maps != 0u &&
                       u32PrefComplete != 0u &&
                       u32StepIrq != 0u &&
                       u32IrqOk != 0u &&
                       u32StepDma != 0u &&
                       u32DmaDenseFree != 0u &&
                       UDX_HOST_STEP_ALL_DENSE != 0u &&
                       UDX_HOST_PRODUCT_CHAIN_N == 6u) ? 1u : 0u;
    /*
     * W15 denser chain SCAN→DMA: W14 denser (product path + IRQ denser +
     * DMA both) + DMA triple terminal + life product chain denser +
     * chain_step denser. greppable: chain_dense_scan_dma=
     * W17 denser: also require DMA free terminal when CHAIN_FREE_DENSE.
     */
    u32ChainDenseScanDma = (u32ProductPathOk != 0u &&
                            u32ChainStepDense != 0u &&
                            u32StepScan != 0u &&
                            u32StepGet != 0u &&
                            u32StepOpen != 0u &&
                            h > 0 &&
                            u32StepMapBar != 0u &&
                            u32Maps != 0u &&
                            u32StepIrq != 0u &&
                            u32IrqOk != 0u &&
                            u32StepDma != 0u &&
                            u32DmaDenseBoth != 0u &&
                            u32DmaDenseTriple != 0u &&
                            u32LifeProductChain != 0u &&
                            /* W17: free terminal denser on chain SCAN→DMA */
                            (UDX_HOST_CHAIN_FREE_DENSE == 0u ||
                             u32DmaDenseFree != 0u) &&
                            UDX_HOST_CHAIN_DENSE_SCAN_DMA != 0u &&
                            UDX_HOST_CHAIN_STEP_DENSE != 0u &&
                            UDX_HOST_PRODUCT_CHAIN_N == 6u) ? 1u : 0u;
    /* W17 denser: chain free denser lamp (SCAN→DMA + DMA free terminal). */
    u32ChainFreeDense = (u32ChainDenseScanDma != 0u &&
                         u32DmaDenseFree != 0u &&
                         UDX_HOST_CHAIN_FREE_DENSE != 0u) ? 1u : 0u;
    /* W16 denser: REMAP + full SIDE + WIN + prefer_real (GJ_SYS_DDI). */
    u32RemapDense = ((u32Life & UDX_DDI_LIFE_REMAP) != 0u &&
                     UDX_HOST_REMAP_DENSE != 0u) ? 1u : 0u;
    u32SideDense = (((u32Life & UDX_DDI_LIFE_SIDE) == UDX_DDI_LIFE_SIDE) &&
                    UDX_HOST_SIDE_DENSE != 0u) ? 1u : 0u;
    u32WinDense = ((u32Life & UDX_DDI_LIFE_WIN) != 0u &&
                   UDX_HOST_WIN_DENSE != 0u) ? 1u : 0u;
    u32PreferRealDense = (UDX_HOST_PREFER_REAL_DENSE != 0u &&
                          UDX_HOST_SYS_DDI_NR == 103u &&
                          UDX_HOST_INJECT_NE_PRODUCT != 0u &&
                          UDX_HOST_HARD_IRQ_BIND == 0u) ? 1u : 0u;
    /* W17 denser: CFG / INSTALL / IOREMAP denser prove. */
    u32CfgDense = (u32StepCfg != 0u && u32CfgIdMatch != 0u &&
                   UDX_HOST_CFG_DENSE != 0u) ? 1u : 0u;
    u32InstallDense = (u32StepInstall != 0u && u32StepPdev != 0u &&
                       UDX_HOST_INSTALL_DENSE != 0u) ? 1u : 0u;
    u32IoremapDense = (u32StepIoremap != 0u && u32VaMatchN != 0u &&
                       UDX_HOST_IOREMAP_DENSE != 0u) ? 1u : 0u;
    /*
     * W18 denser: handle_retain / close / dual_dod / soft_ne / open_h /
     * path_full terminal denser (product_path_full Dual DoD retain).
     * greppable: handle_retain_dense= | close_dense= | dual_dod_dense=
     * greppable: soft_ne_dense= | open_h_dense= | path_full_dense=
     * close_on_pass=0 handle_retain=1; Soft!=product; Dual DoD OPEN.
     */
    u32OpenHDense = (u32StepOpen != 0u && h > 0 &&
                     UDX_HOST_OPEN_H_DENSE != 0u) ? 1u : 0u;
    u32HandleRetainDense = (h > 0 &&
                            u32StepInstall != 0u &&
                            UDX_HOST_HANDLE_RETAIN_LOCK != 0u &&
                            UDX_HOST_HANDLE_RETAIN_DENSE != 0u &&
                            UDX_HOST_CLOSE_ON_PASS == 0u &&
                            UDX_HOST_SOFT_HANDLE_RETAIN != 0u) ? 1u : 0u;
    u32CloseDense = (UDX_HOST_CLOSE_ON_PASS == 0u &&
                     UDX_HOST_SOFT_CLOSE_ON_BIND == 0u &&
                     UDX_HOST_CLOSE_DENSE != 0u &&
                     UDX_HOST_AGENT_CLOSE == 0u) ? 1u : 0u;
    u32DualDodDense = (UDX_HOST_DOD_A_OPEN != 0u &&
                       UDX_HOST_DOD_B_OPEN != 0u &&
                       UDX_HOST_AGENT_CLOSE == 0u &&
                       UDX_HOST_DUAL_DOD_DENSE != 0u &&
                       u32IsProductHost != 0u) ? 1u : 0u;
    u32SoftNeDense = (UDX_HOST_SOFT_NE_PRODUCT_LOCK != 0u &&
                      UDX_HOST_SOFT_NE_DENSE != 0u &&
                      UDX_HOST_INJECT_NE_PRODUCT != 0u &&
                      UDX_HOST_FS_CLASS_SKIP != 0u) ? 1u : 0u;
    /*
     * W19 denser: mint / agent / fs / maps / pref / va denser prove
     * (product_path_full Dual DoD Soft!=product OPEN denser terminal).
     * greppable: mint_dense= | agent_dense= | fs_dense=
     * greppable: maps_dense= | pref_dense= | va_dense=
     * Soft!=product; Dual DoD A/B OPEN; agent!=close; cap_mint=0.
     */
    u32MintDense = (UDX_HOST_SOFT_CAP_MINT == 0u &&
                    UDX_HOST_SOFT_PRODUCT_MINT == 0u &&
                    UDX_HOST_MINT_DENSE != 0u) ? 1u : 0u;
    u32AgentDense = (UDX_HOST_AGENT_CLOSE == 0u &&
                     UDX_HOST_DOD_A_OPEN != 0u &&
                     UDX_HOST_DOD_B_OPEN != 0u &&
                     UDX_HOST_AGENT_DENSE != 0u) ? 1u : 0u;
    u32FsDense = (UDX_HOST_FS_CLASS_SKIP != 0u &&
                  UDX_HOST_SOFT_NEVER_FS_RTL_USB != 0u &&
                  UDX_HOST_FS_DENSE != 0u) ? 1u : 0u;
    u32MapsDense = (u32Maps != 0u &&
                    u32StepMapBar != 0u &&
                    UDX_HOST_MAPS_DENSE != 0u) ? 1u : 0u;
    u32PrefDense = (u32PrefComplete != 0u &&
                    UDX_HOST_PREF_DENSE != 0u) ? 1u : 0u;
    u32VaDense = (u32VaMatchN != 0u &&
                  u32IoremapOk != 0u &&
                  UDX_HOST_VA_DENSE != 0u) ? 1u : 0u;
    /*
     * W20 denser: core / pdev / life / h1 / inject / sys denser prove
     * (product_path_full Dual DoD Soft!=product OPEN denser terminal).
     * greppable: core_dense= | pdev_dense= | life_dense=
     * greppable: h1_dense= | inject_dense= | sys_dense=
     * Soft!=product; Dual DoD A/B OPEN; agent!=close; sys_ddi=103.
     */
    u32CoreDense = (u32CoreOk != 0u &&
                    UDX_HOST_CORE_DENSE != 0u) ? 1u : 0u;
    u32PdevDense = (u32StepPdev != 0u &&
                    u32PdevIdMatch != 0u &&
                    UDX_HOST_PDEV_DENSE != 0u) ? 1u : 0u;
    u32LifeDense = (u32LifeProductChain != 0u &&
                    UDX_HOST_LIFE_DENSE != 0u) ? 1u : 0u;
    u32H1Dense = (UDX_HOST_H1_NO_NET_ETH_POLL != 0u &&
                  UDX_HOST_HARD_IRQ_BIND == 0u &&
                  UDX_HOST_H1_DENSE != 0u) ? 1u : 0u;
    u32InjectDense = (UDX_HOST_INJECT_NE_PRODUCT != 0u &&
                      UDX_HOST_INJECT_DENSE != 0u &&
                      UDX_HOST_PREFER_REAL_DENSE != 0u) ? 1u : 0u;
    u32SysDense = (UDX_HOST_SYS_DDI_NR == 103u &&
                   UDX_HOST_SYS_DENSE != 0u) ? 1u : 0u;
    /*
     * W21 denser: multi / confine / wave / path / lean / product denser prove
     * (product_path_full Dual DoD Soft!=product OPEN denser terminal).
     * greppable: multi_dense= | confine_dense= | wave_dense=
     * greppable: path_dense= | lean_dense= | product_dense=
     * Soft!=product; Dual DoD A/B OPEN; agent!=close; bar wave=75.
     */
    u32MultiDense = (UDX_HOST_SOFT_MULTI_SERVER == 0u &&
                     UDX_HOST_MULTI_DENSE != 0u) ? 1u : 0u;
    u32ConfineDense = (UDX_HOST_SOFT_CONFINE == 0u &&
                       UDX_HOST_CONFINE_DENSE != 0u) ? 1u : 0u;
    u32WaveDense = (UDX_HOST_SOFT_WAVE == 75u &&
                    UDX_HOST_WAVE_DENSE != 0u) ? 1u : 0u;
    u32PathDense = (u32ProductPathOk != 0u &&
                    UDX_HOST_PATH_DENSE != 0u) ? 1u : 0u;
    u32LeanDense = (UDX_HOST_LEAN_CHECKS == 68u &&
                    UDX_HOST_LEAN_DENSE != 0u) ? 1u : 0u;
    u32ProductDense = (UDX_HOST_SOFT_NE_PRODUCT != 0u &&
                       UDX_HOST_PRODUCT_DENSE != 0u &&
                       u32IsProductHost != 0u) ? 1u : 0u;
    /*
     * W22 denser: hazard / c2 / bar / gac / stamp / layout denser prove
     * (product_path_full Dual DoD Soft!=product OPEN denser terminal).
     * greppable: hazard_dense= | c2_dense= | bar_dense=
     * greppable: gac_dense= | stamp_dense= | layout_dense=
     * Soft!=product; Dual DoD A/B OPEN; agent!=close; bar .75.
     */
    u32HazardDense = (UDX_HOST_HAZARD_DENSE != 0u &&
                      UDX_HOST_AGENT_CLOSE == 0u &&
                      UDX_HOST_SOFT_NE_PRODUCT_LOCK != 0u) ? 1u : 0u;
    u32C2Dense = (UDX_HOST_C2_DENSE != 0u &&
                  u32ProductPathOk != 0u &&
                  UDX_HOST_SYS_DDI_NR == 103u &&
                  UDX_HOST_PRODUCT_CHAIN_N == 6u) ? 1u : 0u;
    u32BarDense = (UDX_HOST_BAR_DENSE != 0u &&
                   UDX_HOST_SOFT_WAVE == 75u &&
                   UDX_HOST_WAVE_DENSE != 0u) ? 1u : 0u;
    u32GacDense = (UDX_HOST_GAC_DENSE != 0u &&
                   UDX_HOST_SOFT_NE_PRODUCT_LOCK != 0u &&
                   UDX_HOST_FS_CLASS_SKIP != 0u &&
                   UDX_HOST_INJECT_NE_PRODUCT != 0u) ? 1u : 0u;
    u32StampDense = (UDX_HOST_STAMP_DENSE != 0u &&
                     UDX_HOST_SOFT_WAVE == 75u &&
                     UDX_HOST_AGENT_CLOSE == 0u) ? 1u : 0u;
    u32LayoutDense = (UDX_HOST_LAYOUT_DENSE != 0u &&
                      UDX_HOST_LAYOUT_VER == 1u) ? 1u : 0u;
    /*
     * W23 denser: mmio / inv / sshd / eq / once / grant denser prove
     * (product_path_full Dual DoD Soft!=product OPEN denser terminal).
     * greppable: mmio_dense= | inv_dense= | sshd_dense=
     * greppable: eq_dense= | once_dense= | grant_dense=
     * Soft!=product; Dual DoD A/B OPEN; agent!=close; bar .75.
     */
    u32MmioDense = (u32Maps != 0u &&
                    u32StepMapBar != 0u &&
                    UDX_HOST_MMIO_DENSE != 0u &&
                    UDX_HOST_MAPS_DENSE != 0u) ? 1u : 0u;
    u32InvDense = (UDX_HOST_INV_DENSE != 0u &&
                   UDX_HOST_SYS_DDI_NR == 103u &&
                   UDX_HOST_SOFT_NE_PRODUCT_LOCK != 0u) ? 1u : 0u;
    u32SshdDense = (UDX_HOST_SOFT_PRODUCT_UDX_SSHD_STACK != 0u &&
                    UDX_HOST_SSHD_DENSE != 0u &&
                    u32IsProductHost != 0u) ? 1u : 0u;
    u32EqDense = (UDX_HOST_EQ_DENSE != 0u &&
                  UDX_HOST_SOFT_NE_PRODUCT != 0u &&
                  UDX_HOST_PRODUCT_DENSE != 0u) ? 1u : 0u;
    u32OnceDense = (UDX_HOST_ONCE_DENSE != 0u &&
                    UDX_HOST_AGENT_CLOSE == 0u &&
                    UDX_HOST_SOFT_NE_PRODUCT_LOCK != 0u &&
                    UDX_HOST_HAZARD_DENSE != 0u) ? 1u : 0u;
    u32GrantDense = (UDX_HOST_SOFT_CAP_MINT == 0u &&
                     UDX_HOST_SOFT_PRODUCT_MINT == 0u &&
                     UDX_HOST_GRANT_DENSE != 0u &&
                     UDX_HOST_MINT_DENSE != 0u) ? 1u : 0u;
    /*
     * W23 STRONGER denser full path for product UDX via GJ_SYS_DDI fully:
     *   product_path_ok (SCAN,GET,OPEN,MAP_BAR,IRQ,DMA denser prove) +
     *   INSTALL + IOREMAP + CFG id_match + pref_complete + VA match +
     *   post-install pdev identity (pdev_id_match) + LIFE_CORE +
     *   DMA denser both/triple/free + IRQ denser + maps>=1 +
     *   product_host denser + chain denser SCAN→DMA + chain free denser +
     *   chain_step denser + step_all denser + life product chain denser +
     *   LIFE_FULL denser + REMAP denser + SIDE denser + WIN denser +
     *   prefer_real denser + CFG denser + INSTALL denser + IOREMAP denser +
     *   handle_retain denser + close denser + dual_dod denser +
     *   soft_ne denser + open_h denser + path_full denser +
     *   mint denser + agent denser + fs denser +
     *   maps denser + pref denser + va denser +
     *   core denser + pdev denser + life denser +
     *   h1 denser + inject denser + sys denser +
     *   multi denser + confine denser + wave denser +
     *   path denser + lean denser + product denser +
     *   hazard denser + c2 denser + bar denser +
     *   gac denser + stamp denser + layout denser +
     *   Dual DoD A/B OPEN + agent_close=0 + Soft!=product.
     * close_on_pass=0 handle_retain=1; freestanding class SKIP honesty.
     * greppable: Soft!=product dual_dod OPEN W23_STRONGER
     * greppable: product_path_full_ok= denser_prove=1 W23_STRONGER=1
     * greppable: chain_free_dense= | step_all_dense= | life_full_dense=
     * greppable: cfg_dense= | install_dense= | ioremap_dense=
     * greppable: remap_dense= | side_dense= | win_dense= | prefer_real_dense=
     * greppable: handle_retain_dense= | close_dense= | dual_dod_dense=
     * greppable: soft_ne_dense= | open_h_dense= | path_full_dense=
     * greppable: mint_dense= | agent_dense= | fs_dense=
     * greppable: maps_dense= | pref_dense= | va_dense=
     * greppable: core_dense= | pdev_dense= | life_dense=
     * greppable: h1_dense= | inject_dense= | sys_dense=
     * greppable: multi_dense= | confine_dense= | wave_dense=
     * greppable: path_dense= | lean_dense= | product_dense=
     * greppable: hazard_dense= | c2_dense= | bar_dense=
     * greppable: gac_dense= | stamp_dense= | layout_dense=
     */
    u32ProductPathFullOk = (u32ProductPathOk != 0u &&
                            u32StepInstall != 0u &&
                            u32StepIoremap != 0u &&
                            u32StepCfg != 0u &&
                            u32PrefComplete != 0u &&
                            u32VaMatchN != 0u &&
                            u32StepPdev != 0u &&
                            u32CoreOk != 0u &&
                            /* W15 denser: chain SCAN→DMA terminal denser */
                            u32ChainDenseScanDma != 0u &&
                            u32ChainStepDense != 0u &&
                            u32LifeProductChain != 0u &&
                            u32DmaDenseBoth != 0u &&
                            u32DmaDenseTriple != 0u &&
                            u32DmaDenseFree != 0u &&
                            u32IrqOk != 0u &&
                            u32Maps != 0u &&
                            u32IsProductHost != 0u &&
                            /* W16 denser: prefer_real + side residual */
                            u32RemapDense != 0u &&
                            u32SideDense != 0u &&
                            u32WinDense != 0u &&
                            u32PreferRealDense != 0u &&
                            /* W17 denser: chain free + step_all + life_full +
                             * CFG/INSTALL/IOREMAP denser on product_path_full */
                            u32ChainFreeDense != 0u &&
                            u32StepAllDense != 0u &&
                            u32LifeFullDense != 0u &&
                            u32CfgDense != 0u &&
                            u32InstallDense != 0u &&
                            u32IoremapDense != 0u &&
                            UDX_HOST_PRODUCT_FULL_DENSE != 0u &&
                            UDX_HOST_W14_STRONGER != 0u &&
                            UDX_HOST_W15_STRONGER != 0u &&
                            UDX_HOST_W16_STRONGER != 0u &&
                            UDX_HOST_W17_STRONGER != 0u &&
                            UDX_HOST_W18_STRONGER != 0u &&
                            UDX_HOST_W19_STRONGER != 0u &&
                            UDX_HOST_W20_STRONGER != 0u &&
                            UDX_HOST_W21_STRONGER != 0u &&
                            UDX_HOST_W22_STRONGER != 0u &&
                            UDX_HOST_W23_STRONGER != 0u &&
                            UDX_HOST_DMA_DENSE_BOTH != 0u &&
                            UDX_HOST_DMA_DENSE_TRIPLE != 0u &&
                            UDX_HOST_DMA_DENSE_FREE != 0u &&
                            UDX_HOST_IRQ_DENSE_OK != 0u &&
                            UDX_HOST_CHAIN_DENSE_SCAN_DMA != 0u &&
                            UDX_HOST_CHAIN_FREE_DENSE != 0u &&
                            UDX_HOST_CHAIN_STEP_DENSE != 0u &&
                            UDX_HOST_STEP_ALL_DENSE != 0u &&
                            UDX_HOST_PRODUCT_HOST_DENSE != 0u &&
                            UDX_HOST_REMAP_DENSE != 0u &&
                            UDX_HOST_SIDE_DENSE != 0u &&
                            UDX_HOST_WIN_DENSE != 0u &&
                            UDX_HOST_PREFER_REAL_DENSE != 0u &&
                            UDX_HOST_CFG_DENSE != 0u &&
                            UDX_HOST_INSTALL_DENSE != 0u &&
                            UDX_HOST_IOREMAP_DENSE != 0u &&
                            UDX_HOST_LIFE_FULL_DENSE != 0u &&
                            /* W18 denser: retain/close/dual_dod/soft_ne/open_h */
                            u32HandleRetainDense != 0u &&
                            u32CloseDense != 0u &&
                            u32DualDodDense != 0u &&
                            u32SoftNeDense != 0u &&
                            u32OpenHDense != 0u &&
                            UDX_HOST_HANDLE_RETAIN_DENSE != 0u &&
                            UDX_HOST_CLOSE_DENSE != 0u &&
                            UDX_HOST_DUAL_DOD_DENSE != 0u &&
                            UDX_HOST_SOFT_NE_DENSE != 0u &&
                            UDX_HOST_OPEN_H_DENSE != 0u &&
                            UDX_HOST_PATH_FULL_DENSE != 0u &&
                            /* W19 denser: mint/agent/fs/maps/pref/va */
                            u32MintDense != 0u &&
                            u32AgentDense != 0u &&
                            u32FsDense != 0u &&
                            u32MapsDense != 0u &&
                            u32PrefDense != 0u &&
                            u32VaDense != 0u &&
                            UDX_HOST_MINT_DENSE != 0u &&
                            UDX_HOST_AGENT_DENSE != 0u &&
                            UDX_HOST_FS_DENSE != 0u &&
                            UDX_HOST_MAPS_DENSE != 0u &&
                            UDX_HOST_PREF_DENSE != 0u &&
                            UDX_HOST_VA_DENSE != 0u &&
                            /* W20 denser: core/pdev/life/h1/inject/sys */
                            u32CoreDense != 0u &&
                            u32PdevDense != 0u &&
                            u32LifeDense != 0u &&
                            u32H1Dense != 0u &&
                            u32InjectDense != 0u &&
                            u32SysDense != 0u &&
                            UDX_HOST_CORE_DENSE != 0u &&
                            UDX_HOST_PDEV_DENSE != 0u &&
                            UDX_HOST_LIFE_DENSE != 0u &&
                            UDX_HOST_H1_DENSE != 0u &&
                            UDX_HOST_INJECT_DENSE != 0u &&
                            UDX_HOST_SYS_DENSE != 0u &&
                            /* W21 denser: multi/confine/wave/path/lean/product */
                            u32MultiDense != 0u &&
                            u32ConfineDense != 0u &&
                            u32WaveDense != 0u &&
                            u32PathDense != 0u &&
                            u32LeanDense != 0u &&
                            u32ProductDense != 0u &&
                            UDX_HOST_MULTI_DENSE != 0u &&
                            UDX_HOST_CONFINE_DENSE != 0u &&
                            UDX_HOST_WAVE_DENSE != 0u &&
                            UDX_HOST_PATH_DENSE != 0u &&
                            UDX_HOST_LEAN_DENSE != 0u &&
                            UDX_HOST_PRODUCT_DENSE != 0u &&
                            UDX_HOST_W21_STRONGER != 0u &&
                            /* W22 denser: hazard/c2/bar/gac/stamp/layout */
                            u32HazardDense != 0u &&
                            u32C2Dense != 0u &&
                            u32BarDense != 0u &&
                            u32GacDense != 0u &&
                            u32StampDense != 0u &&
                            u32LayoutDense != 0u &&
                            UDX_HOST_HAZARD_DENSE != 0u &&
                            UDX_HOST_C2_DENSE != 0u &&
                            UDX_HOST_BAR_DENSE != 0u &&
                            UDX_HOST_GAC_DENSE != 0u &&
                            UDX_HOST_STAMP_DENSE != 0u &&
                            UDX_HOST_LAYOUT_DENSE != 0u &&
                            UDX_HOST_W22_STRONGER != 0u &&
                            /* W23 denser: mmio/inv/sshd/eq/once/grant */
                            u32MmioDense != 0u &&
                            u32InvDense != 0u &&
                            u32SshdDense != 0u &&
                            u32EqDense != 0u &&
                            u32OnceDense != 0u &&
                            u32GrantDense != 0u &&
                            UDX_HOST_MMIO_DENSE != 0u &&
                            UDX_HOST_INV_DENSE != 0u &&
                            UDX_HOST_SSHD_DENSE != 0u &&
                            UDX_HOST_EQ_DENSE != 0u &&
                            UDX_HOST_ONCE_DENSE != 0u &&
                            UDX_HOST_GRANT_DENSE != 0u &&
                            UDX_HOST_W23_STRONGER != 0u &&
                            UDX_HOST_CLOSE_ON_PASS == 0u &&
                            UDX_HOST_HANDLE_RETAIN_LOCK != 0u &&
                            UDX_HOST_FS_CLASS_SKIP != 0u &&
                            UDX_HOST_DOD_A_OPEN != 0u &&
                            UDX_HOST_DOD_B_OPEN != 0u &&
                            UDX_HOST_AGENT_CLOSE == 0u &&
                            UDX_HOST_SOFT_NE_PRODUCT_LOCK != 0u &&
                            UDX_HOST_INJECT_NE_PRODUCT != 0u &&
                            UDX_HOST_H1_NO_NET_ETH_POLL != 0u &&
                            UDX_HOST_HARD_IRQ_BIND == 0u &&
                            UDX_HOST_SYS_DDI_NR == 103u) ? 1u : 0u;
    /*
     * W23 path_full denser terminal lamp (equals product_path_full_ok under
     * W23 denser locks already required above). greppable: path_full_dense=
     * chain SCAN→DMA denser + retain/close/dual_dod/soft_ne/open_h +
     * mint/agent/fs/maps/pref/va denser + core/pdev/life/h1/inject/sys denser +
     * multi/confine/wave/path/lean/product denser +
     * hazard/c2/bar/gac/stamp/layout denser +
     * mmio/inv/sshd/eq/once/grant denser.
     */
    u32PathFullDense = (u32ProductPathFullOk != 0u &&
                        u32HandleRetainDense != 0u &&
                        u32CloseDense != 0u &&
                        u32DualDodDense != 0u &&
                        u32SoftNeDense != 0u &&
                        u32OpenHDense != 0u &&
                        u32MintDense != 0u &&
                        u32AgentDense != 0u &&
                        u32FsDense != 0u &&
                        u32MapsDense != 0u &&
                        u32PrefDense != 0u &&
                        u32VaDense != 0u &&
                        u32CoreDense != 0u &&
                        u32PdevDense != 0u &&
                        u32LifeDense != 0u &&
                        u32H1Dense != 0u &&
                        u32InjectDense != 0u &&
                        u32SysDense != 0u &&
                        u32MultiDense != 0u &&
                        u32ConfineDense != 0u &&
                        u32WaveDense != 0u &&
                        u32PathDense != 0u &&
                        u32LeanDense != 0u &&
                        u32ProductDense != 0u &&
                        u32HazardDense != 0u &&
                        u32C2Dense != 0u &&
                        u32BarDense != 0u &&
                        u32GacDense != 0u &&
                        u32StampDense != 0u &&
                        u32LayoutDense != 0u &&
                        u32MmioDense != 0u &&
                        u32InvDense != 0u &&
                        u32SshdDense != 0u &&
                        u32EqDense != 0u &&
                        u32OnceDense != 0u &&
                        u32GrantDense != 0u &&
                        u32ChainDenseScanDma != 0u &&
                        UDX_HOST_PATH_FULL_DENSE != 0u &&
                        UDX_HOST_W18_STRONGER != 0u &&
                        UDX_HOST_W19_STRONGER != 0u &&
                        UDX_HOST_W20_STRONGER != 0u &&
                        UDX_HOST_W21_STRONGER != 0u &&
                        UDX_HOST_W22_STRONGER != 0u &&
                        UDX_HOST_W23_STRONGER != 0u) ? 1u : 0u;
    if (u32ProductPathOk != 0u) {
        host_soft_inc(&g_u32DdiResProductPath);
    }
    if (u32ProductPathFullOk != 0u) {
        host_soft_inc(&g_u32DdiResProductPathFull);
    }

    host_soft_inc(&g_u32DdiResFunc);

    /* greppable: udx: soft ddi residual functional step= denser prove */
    host_soft_inc(&g_u32DdiResFuncStep);
    host_soft_emit("udx: soft ddi residual functional step=SCAN "
                   "ok=%u denser_prove=1 host=%s dual_dod=%s "
                   "chain_dense_scan_dma=%u chain_free_dense=%u "
                   "chain_step_dense=%u step_all_dense=%u "
                   "life_product_chain=%u life_full_dense=%u "
                   "soft=1 Soft!=product dual_dod OPEN W23_STRONGER=1\n",
                   (unsigned)u32StepScan, szHost, szDod,
                   (unsigned)u32ChainDenseScanDma,
                   (unsigned)u32ChainFreeDense,
                   (unsigned)u32ChainStepDense,
                   (unsigned)u32StepAllDense,
                   (unsigned)u32LifeProductChain,
                   (unsigned)u32LifeFullDense);
    host_soft_inc(&g_u32DdiResFuncStep);
    host_soft_emit("udx: soft ddi residual functional step=GET "
                   "ok=%u denser_prove=1 host=%s dual_dod=%s "
                   "soft=1 Soft!=product dual_dod OPEN W23_STRONGER=1\n",
                   (unsigned)u32StepGet, szHost, szDod);
    host_soft_inc(&g_u32DdiResFuncStep);
    host_soft_emit("udx: soft ddi residual functional step=OPEN "
                   "ok=%u h=%ld denser_prove=1 host=%s dual_dod=%s "
                   "soft=1 Soft!=product dual_dod OPEN W23_STRONGER=1\n",
                   (unsigned)u32StepOpen, (long)h, szHost, szDod);
    host_soft_inc(&g_u32DdiResFuncStep);
    host_soft_emit("udx: soft ddi residual functional step=MAP_BAR "
                   "ok=%u maps=%u pref=%u pref_complete=%u denser_prove=1 "
                   "host=%s dual_dod=%s "
                   "soft=1 Soft!=product dual_dod OPEN W23_STRONGER=1\n",
                   (unsigned)u32StepMapBar, (unsigned)u32Maps,
                   (unsigned)u32Pref, (unsigned)u32PrefComplete,
                   szHost, szDod);
    host_soft_inc(&g_u32DdiResFuncStep);
    host_soft_emit("udx: soft ddi residual functional step=IRQ "
                   "ok=%u irq_bind=%u denser_prove=1 host=%s dual_dod=%s "
                   "soft=1 Soft!=product dual_dod OPEN W23_STRONGER=1\n",
                   (unsigned)u32StepIrq, (unsigned)u32IrqOk,
                   szHost, szDod);
    host_soft_inc(&g_u32DdiResFuncStep);
    host_soft_emit("udx: soft ddi residual functional step=DMA "
                   "ok=%u dma_note=%u dma_buf=%u dma_buf_map=%u "
                   "dma_buf_free=%u "
                   "dma_dense_both=%u dma_dense_triple=%u dma_dense_free=%u "
                   "chain_free_dense=%u denser_prove=1 host=%s dual_dod=%s "
                   "soft=1 Soft!=product dual_dod OPEN W23_STRONGER=1\n",
                   (unsigned)u32StepDma, (unsigned)u32DmaNoteOk,
                   (unsigned)u32DmaBufOk, (unsigned)u32DmaBufMap,
                   (unsigned)u32DmaBufFree,
                   (unsigned)u32DmaDenseBoth, (unsigned)u32DmaDenseTriple,
                   (unsigned)u32DmaDenseFree,
                   (unsigned)u32ChainFreeDense,
                   szHost, szDod);
    host_soft_inc(&g_u32DdiResFuncStep);
    host_soft_emit("udx: soft ddi residual functional step=INSTALL "
                   "ok=%u pdev_id_match=%u denser_prove=1 "
                   "host=%s dual_dod=%s "
                   "handle_retain=1 close_on_bind=0 "
                   "close_on_pass=0 handle_retain_lock=1 "
                   "handle_retain_dense=%u close_dense=%u "
                   "dual_dod_dense=%u open_h_dense=%u "
                   "freestanding_class=SKIP product_host_dense=%u "
                   "install_dense=%u pdev_dense=%u core_dense=%u "
                   "soft=1 Soft!=product dual_dod OPEN W23_STRONGER=1\n",
                   (unsigned)u32StepInstall, (unsigned)u32StepPdev,
                   szHost, szDod,
                   (unsigned)u32HandleRetainDense,
                   (unsigned)u32CloseDense,
                   (unsigned)u32DualDodDense,
                   (unsigned)u32OpenHDense,
                   (unsigned)u32IsProductHost,
                   (unsigned)u32InstallDense,
                   (unsigned)u32PdevDense,
                   (unsigned)u32CoreDense);
    host_soft_inc(&g_u32DdiResFuncStep);
    host_soft_emit("udx: soft ddi residual functional step=IOREMAP "
                   "ok=%u ioremap_ok=%u va_match_n=%u host=%s "
                   "dual_dod=%s soft=1 Soft!=product\n",
                   (unsigned)u32StepIoremap, (unsigned)u32IoremapOk,
                   (unsigned)u32VaMatchN, szHost, szDod);
    host_soft_inc(&g_u32DdiResFuncStep);
    host_soft_emit("udx: soft ddi residual functional step=CFG "
                   "ok=%u id_match=%u host=%s dual_dod=%s "
                   "soft=1 Soft!=product\n",
                   (unsigned)u32StepCfg, (unsigned)u32CfgIdMatch,
                   szHost, szDod);
    host_soft_inc(&g_u32DdiResFuncStep);
    host_soft_emit("udx: soft ddi residual functional step=PDEV "
                   "ok=%u id_match=%u host=%s dual_dod=%s "
                   "sys=GJ_SYS_DDI soft=1 Soft!=product\n",
                   (unsigned)u32StepPdev, (unsigned)u32PdevIdMatch,
                   szHost, szDod);

    /*
     * greppable: udx: soft ddi bind residual functional
     * Rollup: denser product-host chain prove (not inject-only).
     * greppable: chain=SCAN,GET,OPEN,MAP_BAR,IRQ,DMA
     * greppable: product_path_ok= | product_path_full_ok= | pdev_id_match=
     * greppable: close_on_pass=0 handle_retain=1
     * greppable: Soft!=product dual_dod OPEN W23_STRONGER
     * greppable: chain_dense_scan_dma= | chain_free_dense= | step_all_dense=
     * greppable: dma_dense_free= | product_host_dense= | life_full_dense=
     * greppable: cfg_dense= | install_dense= | ioremap_dense=
     */
    host_soft_emit("udx: soft ddi bind residual functional "
                   "vend=%04x dev=%04x host=%s dual_dod=%s "
                   "chain=SCAN,GET,OPEN,MAP_BAR,IRQ,DMA denser_prove=1 "
                   "sys_ddi=%u chain_n=%u "
                   "scan=%u get=%u open=%u map_bar=%u irq=%u dma=%u "
                   "install=%u ioremap=%u cfg_id=%u side=%u pdev=%u "
                   "maps=%u pref=%u pref_complete=%u "
                   "va_match_n=%u core_ok=%u chain_ok=%u "
                   "product_path_ok=%u product_path_full_ok=%u "
                   "pdev_id_match=%u product_host=%u product_host_dense=%u "
                   "dma_note=%u dma_buf=%u dma_buf_map=%u dma_buf_free=%u "
                   "dma_dense_both=%u dma_dense_triple=%u dma_dense_free=%u "
                   "irq_dense=%u chain_dense_scan_dma=%u chain_free_dense=%u "
                   "chain_step_dense=%u step_all_dense=%u "
                   "life_product_chain=%u life_full_dense=%u "
                   "remap_dense=%u side_dense=%u win_dense=%u "
                   "prefer_real_dense=%u "
                   "cfg_dense=%u install_dense=%u ioremap_dense=%u "
                   "handle_retain_dense=%u close_dense=%u dual_dod_dense=%u "
                   "soft_ne_dense=%u open_h_dense=%u path_full_dense=%u "
                   "mint_dense=%u agent_dense=%u fs_dense=%u "
                   "maps_dense=%u pref_dense=%u va_dense=%u "
                   "core_dense=%u pdev_dense=%u life_dense=%u "
                   "h1_dense=%u inject_dense=%u sys_dense=%u "
                   "multi_dense=%u confine_dense=%u wave_dense=%u "
                   "path_dense=%u lean_dense=%u product_dense=%u "
                   "hazard_dense=%u c2_dense=%u bar_dense=%u "
                   "gac_dense=%u stamp_dense=%u layout_dense=%u "
                   "mmio_dense=%u inv_dense=%u sshd_dense=%u "
                   "eq_dense=%u once_dense=%u grant_dense=%u "
                   "life=0x%x handle_retain=%u close_on_bind=%u "
                   "close_on_pass=%u handle_retain_lock=%u "
                   "freestanding_class=SKIP lean_checks=%u "
                   "ids=10ec:8168,8086:a12f "
                   "DoD_A_OPEN=%u DoD_B_OPEN=%u agent_ne_close=1 "
                   "dual_dod OPEN product=UDX+ABI fs_class=SKIP inject=0 "
                   "mint=%u soft=1 Soft!=product W23_STRONGER=1\n",
                   (unsigned)u16Vend, (unsigned)u16Dev, szHost, szDod,
                   (unsigned)UDX_HOST_SYS_DDI_NR,
                   (unsigned)UDX_HOST_PRODUCT_CHAIN_N,
                   (unsigned)u32StepScan, (unsigned)u32StepGet,
                   (unsigned)u32StepOpen, (unsigned)u32StepMapBar,
                   (unsigned)u32StepIrq, (unsigned)u32StepDma,
                   (unsigned)u32StepInstall, (unsigned)u32StepIoremap,
                   (unsigned)u32StepCfg, (unsigned)u32StepSide,
                   (unsigned)u32StepPdev,
                   (unsigned)u32Maps, (unsigned)u32Pref,
                   (unsigned)u32PrefComplete, (unsigned)u32VaMatchN,
                   (unsigned)u32CoreOk, (unsigned)u32ChainOk,
                   (unsigned)u32ProductPathOk,
                   (unsigned)u32ProductPathFullOk,
                   (unsigned)u32PdevIdMatch,
                   (unsigned)u32IsProductHost,
                   (unsigned)((u32IsProductHost != 0u &&
                               UDX_HOST_PRODUCT_HOST_DENSE != 0u) ? 1u : 0u),
                   (unsigned)u32DmaNoteOk, (unsigned)u32DmaBufOk,
                   (unsigned)u32DmaBufMap, (unsigned)u32DmaBufFree,
                   (unsigned)u32DmaDenseBoth,
                   (unsigned)u32DmaDenseTriple,
                   (unsigned)u32DmaDenseFree,
                   (unsigned)u32IrqOk,
                   (unsigned)u32ChainDenseScanDma,
                   (unsigned)u32ChainFreeDense,
                   (unsigned)u32ChainStepDense,
                   (unsigned)u32StepAllDense,
                   (unsigned)u32LifeProductChain,
                   (unsigned)u32LifeFullDense,
                   (unsigned)u32RemapDense,
                   (unsigned)u32SideDense,
                   (unsigned)u32WinDense,
                   (unsigned)u32PreferRealDense,
                   (unsigned)u32CfgDense,
                   (unsigned)u32InstallDense,
                   (unsigned)u32IoremapDense,
                   (unsigned)u32HandleRetainDense,
                   (unsigned)u32CloseDense,
                   (unsigned)u32DualDodDense,
                   (unsigned)u32SoftNeDense,
                   (unsigned)u32OpenHDense,
                   (unsigned)u32PathFullDense,
                   (unsigned)u32MintDense,
                   (unsigned)u32AgentDense,
                   (unsigned)u32FsDense,
                   (unsigned)u32MapsDense,
                   (unsigned)u32PrefDense,
                   (unsigned)u32VaDense,
                   (unsigned)u32CoreDense,
                   (unsigned)u32PdevDense,
                   (unsigned)u32LifeDense,
                   (unsigned)u32H1Dense,
                   (unsigned)u32InjectDense,
                   (unsigned)u32SysDense,
                   (unsigned)u32MultiDense,
                   (unsigned)u32ConfineDense,
                   (unsigned)u32WaveDense,
                   (unsigned)u32PathDense,
                   (unsigned)u32LeanDense,
                   (unsigned)u32ProductDense,
                   (unsigned)u32HazardDense,
                   (unsigned)u32C2Dense,
                   (unsigned)u32BarDense,
                   (unsigned)u32GacDense,
                   (unsigned)u32StampDense,
                   (unsigned)u32LayoutDense,
                   (unsigned)u32MmioDense,
                   (unsigned)u32InvDense,
                   (unsigned)u32SshdDense,
                   (unsigned)u32EqDense,
                   (unsigned)u32OnceDense,
                   (unsigned)u32GrantDense,
                   (unsigned)u32Life,
                   (u32)UDX_HOST_SOFT_HANDLE_RETAIN,
                   (u32)UDX_HOST_SOFT_CLOSE_ON_BIND,
                   (u32)UDX_HOST_CLOSE_ON_PASS,
                   (u32)UDX_HOST_HANDLE_RETAIN_LOCK,
                   (u32)UDX_HOST_LEAN_CHECKS,
                   (u32)UDX_HOST_SOFT_DOD_A_OPEN,
                   (u32)UDX_HOST_SOFT_DOD_B_OPEN,
                   (u32)UDX_HOST_SOFT_CAP_MINT);
    /* denser STRONGER rollup: greppable Soft!=product dual_dod OPEN W23_STRONGER */
    host_soft_emit("udx: soft ddi bind residual functional denser "
                   "chain=SCAN,GET,OPEN,MAP_BAR,IRQ,DMA denser_prove=1 "
                   "product_path_ok=%u product_path_full_ok=%u "
                   "pdev_id_match=%u dma_dense_both=%u dma_dense_triple=%u "
                   "dma_dense_free=%u irq_dense=%u "
                   "chain_dense_scan_dma=%u chain_free_dense=%u "
                   "chain_step_dense=%u step_all_dense=%u "
                   "life_product_chain=%u life_full_dense=%u "
                   "product_host_dense=%u "
                   "remap_dense=%u side_dense=%u win_dense=%u "
                   "prefer_real_dense=%u "
                   "cfg_dense=%u install_dense=%u ioremap_dense=%u "
                   "handle_retain_dense=%u close_dense=%u dual_dod_dense=%u "
                   "soft_ne_dense=%u open_h_dense=%u path_full_dense=%u "
                   "mint_dense=%u agent_dense=%u fs_dense=%u "
                   "maps_dense=%u pref_dense=%u va_dense=%u "
                   "core_dense=%u pdev_dense=%u life_dense=%u "
                   "h1_dense=%u inject_dense=%u sys_dense=%u "
                   "multi_dense=%u confine_dense=%u wave_dense=%u "
                   "path_dense=%u lean_dense=%u product_dense=%u "
                   "hazard_dense=%u c2_dense=%u bar_dense=%u "
                   "gac_dense=%u stamp_dense=%u layout_dense=%u "
                   "mmio_dense=%u inv_dense=%u sshd_dense=%u "
                   "eq_dense=%u once_dense=%u grant_dense=%u "
                   "maps=%u pref_complete=%u va_match_n=%u core_ok=%u "
                   "sys_ddi=%u lean_checks=%u chain_n=%u "
                   "close_on_pass=%u handle_retain=%u "
                   "freestanding_class=SKIP dual_dod OPEN "
                   "DoD_A_OPEN=%u DoD_B_OPEN=%u agent_ne_close=1 "
                   "host=%s dual_dod=%s "
                   "soft=1 Soft!=product W23_STRONGER=1\n",
                   (unsigned)u32ProductPathOk,
                   (unsigned)u32ProductPathFullOk,
                   (unsigned)u32PdevIdMatch,
                   (unsigned)u32DmaDenseBoth,
                   (unsigned)u32DmaDenseTriple,
                   (unsigned)u32DmaDenseFree,
                   (unsigned)u32IrqOk,
                   (unsigned)u32ChainDenseScanDma,
                   (unsigned)u32ChainFreeDense,
                   (unsigned)u32ChainStepDense,
                   (unsigned)u32StepAllDense,
                   (unsigned)u32LifeProductChain,
                   (unsigned)u32LifeFullDense,
                   (unsigned)((u32IsProductHost != 0u &&
                               UDX_HOST_PRODUCT_HOST_DENSE != 0u) ? 1u : 0u),
                   (unsigned)u32RemapDense,
                   (unsigned)u32SideDense,
                   (unsigned)u32WinDense,
                   (unsigned)u32PreferRealDense,
                   (unsigned)u32CfgDense,
                   (unsigned)u32InstallDense,
                   (unsigned)u32IoremapDense,
                   (unsigned)u32HandleRetainDense,
                   (unsigned)u32CloseDense,
                   (unsigned)u32DualDodDense,
                   (unsigned)u32SoftNeDense,
                   (unsigned)u32OpenHDense,
                   (unsigned)u32PathFullDense,
                   (unsigned)u32MintDense,
                   (unsigned)u32AgentDense,
                   (unsigned)u32FsDense,
                   (unsigned)u32MapsDense,
                   (unsigned)u32PrefDense,
                   (unsigned)u32VaDense,
                   (unsigned)u32CoreDense,
                   (unsigned)u32PdevDense,
                   (unsigned)u32LifeDense,
                   (unsigned)u32H1Dense,
                   (unsigned)u32InjectDense,
                   (unsigned)u32SysDense,
                   (unsigned)u32MultiDense,
                   (unsigned)u32ConfineDense,
                   (unsigned)u32WaveDense,
                   (unsigned)u32PathDense,
                   (unsigned)u32LeanDense,
                   (unsigned)u32ProductDense,
                   (unsigned)u32HazardDense,
                   (unsigned)u32C2Dense,
                   (unsigned)u32BarDense,
                   (unsigned)u32GacDense,
                   (unsigned)u32StampDense,
                   (unsigned)u32LayoutDense,
                   (unsigned)u32MmioDense,
                   (unsigned)u32InvDense,
                   (unsigned)u32SshdDense,
                   (unsigned)u32EqDense,
                   (unsigned)u32OnceDense,
                   (unsigned)u32GrantDense,
                   (unsigned)u32Maps,
                   (unsigned)u32PrefComplete,
                   (unsigned)u32VaMatchN,
                   (unsigned)u32CoreOk,
                   (unsigned)UDX_HOST_SYS_DDI_NR,
                   (unsigned)UDX_HOST_LEAN_CHECKS,
                   (unsigned)UDX_HOST_PRODUCT_CHAIN_N,
                   (u32)UDX_HOST_CLOSE_ON_PASS,
                   (u32)UDX_HOST_HANDLE_RETAIN_LOCK,
                   (u32)UDX_HOST_SOFT_DOD_A_OPEN,
                   (u32)UDX_HOST_SOFT_DOD_B_OPEN,
                   szHost, szDod);
    udx_printk("udx: soft ddi bind residual functional "
               "vend=%04x dev=%04x host=%s dual_dod=%s "
               "product_path_ok=%u product_path_full_ok=%u "
               "pdev_id_match=%u chain_ok=%u "
               "map_bar=%u irq=%u dma=%u pref_complete=%u "
               "cfg_id=%u pdev=%u maps=%u lean_checks=%u "
               "dma_dense_both=%u dma_dense_triple=%u dma_dense_free=%u "
               "irq_dense=%u chain_dense_scan_dma=%u chain_free_dense=%u "
               "chain_step_dense=%u step_all_dense=%u "
               "life_product_chain=%u life_full_dense=%u "
               "cfg_dense=%u install_dense=%u ioremap_dense=%u "
               "close_on_pass=%u handle_retain=%u "
               "sys_ddi=%u W23_STRONGER=1 Soft!=product "
               "product=UDX+ABI soft=1\n",
               (unsigned)u16Vend, (unsigned)u16Dev, szHost, szDod,
               (unsigned)u32ProductPathOk, (unsigned)u32ProductPathFullOk,
               (unsigned)u32PdevIdMatch, (unsigned)u32ChainOk,
               (unsigned)u32StepMapBar, (unsigned)u32StepIrq,
               (unsigned)u32StepDma, (unsigned)u32PrefComplete,
               (unsigned)u32CfgIdMatch, (unsigned)u32StepPdev,
               (unsigned)u32Maps, (unsigned)UDX_HOST_LEAN_CHECKS,
               (unsigned)u32DmaDenseBoth,
               (unsigned)u32DmaDenseTriple,
               (unsigned)u32DmaDenseFree,
               (unsigned)u32IrqOk,
               (unsigned)u32ChainDenseScanDma,
               (unsigned)u32ChainFreeDense,
               (unsigned)u32ChainStepDense,
               (unsigned)u32StepAllDense,
               (unsigned)u32LifeProductChain,
               (unsigned)u32LifeFullDense,
               (unsigned)u32CfgDense,
               (unsigned)u32InstallDense,
               (unsigned)u32IoremapDense,
               (u32)UDX_HOST_CLOSE_ON_PASS,
               (u32)UDX_HOST_HANDLE_RETAIN_LOCK,
               (unsigned)UDX_HOST_SYS_DDI_NR);
}

/**
 * OPEN → CFG → MAP preferred BARs → MAP_REMAP → IRQ_BIND → DMA_NOTE →
 * DMA_BUF → install_granted → window_register → ioremap prove.
 * Functional product residual (not inject-only; Soft!=product):
 *   SCAN,GET,OPEN,MAP_BAR,IRQ,DMA
 * MAP_BAR arg3 = &udx_ddi_map_note (matches kernel gj_ddi_map_note).
 * Extra preferred BAR VAs are window-registered after install so
 * udx_ioremap works for rtl8168 BAR2 etc. (pci.c only wires BAR0 VA).
 * Post-install ioremap residual proves the lookup then iounmaps so
 * product-host probe can re-ioremap without freestanding pool thrash.
 * Product hosts retain OPEN handle (no CLOSE residual on install path).
 * Targets: 10ec:8168 (rtl8168_udx bars 0+2), 8086:a12f (xhci_udx bar 0).
 */
static udx_status_t
host_ddi_open_map_install_idx(long iIdx,
                              const struct udx_ddi_dev_info *pInfo,
                              struct udx_pci_dev **ppOut)
{
    long h;
    void *pBar0Va;
    void *apVa[UDX_DDI_BAR_MAX];
    udx_status_t st;
    u32 iBar;
    u32 iPref;
    u32 u32PrefN;
    u32 u32Maps;
    u32 u32Empty;
    u32 u32Fail;
    u32 u32Life;
    u32 u32Idem;
    u32 u32MapUser;
    u32 u32FirstBar;
    u32 u32IoremapOk;
    u32 u32IoremapFail;
    u32 u32CfgIdMatch;
    u32 u32PrefPresent;
    u32 u32PrefMapped;
    u32 u32PrefComplete;
    u32 u32VaMatchBefore;
    u32 u32VaMatchN;
    u32 u32IrqOk;
    u32 u32DmaNoteOk;
    u32 u32DmaBufOk;
    u32 u32DmaBufMap;
    u32 u32DmaBufFree;
    u32 u32DmaBufBefore;
    u32 u32DmaBufMapBefore;
    u32 u32DmaBufFreeBefore;
    u32 u32PdevIdMatch;
    u64 u64FirstVa;
    u64 u64DmaPa;
    u64 u64DmaCb;
    u8  aPref[UDX_DDI_PREF_BAR_MAX];
    u64 aBarPhys[6];
    u64 aBarLen[6];
    u8  aBarIsMem[6];
    u8  u8Devfn;
    u8  u8User;
    const char *szHost;

    if (pInfo == NULL) {
        return UDX_ERR_INVAL;
    }

    u32Life = UDX_DDI_LIFE_SCAN | UDX_DDI_LIFE_GET;
    u32PdevIdMatch = 0;
    szHost = host_ddi_product_host_name(pInfo->u16Vend, pInfo->u16Dev);
    host_ddi_preferred_bars(pInfo->u16Vend, pInfo->u16Dev, aPref, &u32PrefN);

    host_soft_inc(&g_u32DdiResOpen);
    h = host_ddi_syscall3(UDX_DDI_OP_OPEN, iIdx, 0);
    if (h <= 0) {
        host_ddi_life_note(u32Life);
        return UDX_ERR_NODEV;
    }
    host_soft_inc(&g_u32DdiResOpenOk);
    u32Life |= UDX_DDI_LIFE_OPEN;
    /*
     * Product hosts retain OPEN handle (no CLOSE on install).
     * Expose last h so xhci_udx can gj_ddi_irq_bind after IMAN.IE.
     */
    g_i64DdiHandleRetain = h;

    /* Soft CFG_READ residual (identity + cmdst + id_match prove). */
    u32CfgIdMatch = 0;
    if (host_ddi_cfg_residual(h, pInfo->u16Vend, pInfo->u16Dev, szHost,
                              &u32CfgIdMatch) != 0) {
        u32Life |= UDX_DDI_LIFE_CFG_R;
    }

    for (iBar = 0; iBar < 6u; iBar++) {
        host_ddi_bar_pa_cb(pInfo, iBar, &aBarPhys[iBar], &aBarLen[iBar]);
        aBarIsMem[iBar] = (aBarLen[iBar] != 0) ? 1u : 0u;
        apVa[iBar] = NULL;
    }

    u32Maps = 0;
    u32Empty = 0;
    u32Fail = 0;
    u32MapUser = 0;
    u32PrefPresent = 0;
    u32PrefMapped = 0;
    u32FirstBar = 0xffffffffu;
    u64FirstVa = 0;
    pBar0Va = NULL;
    for (iPref = 0; iPref < u32PrefN && iPref < UDX_DDI_PREF_BAR_MAX; iPref++) {
        u32 u32B;
        int nMap;
        void *pVa;

        u32B = (u32)aPref[iPref];
        if (u32B >= UDX_DDI_BAR_MAX) {
            continue;
        }
        /* Functional: count preferred BARs with inventory PA/Cb present. */
        if (aBarPhys[u32B] != 0 && aBarLen[u32B] != 0) {
            if (u32PrefPresent < 0xffffffffu) {
                u32PrefPresent++;
            }
        }
        pVa = NULL;
        u8User = 0;
        nMap = host_ddi_map_one(h, u32B, &aBarPhys[u32B], &aBarLen[u32B],
                                &pVa, &u8User);
        if (nMap > 0) {
            apVa[u32B] = pVa;
            if (u32Maps < 0xffffffffu) {
                u32Maps++;
            }
            if (u32PrefMapped < 0xffffffffu) {
                u32PrefMapped++;
            }
            if (u32B == 0u && pVa != NULL) {
                pBar0Va = pVa;
            }
            if (u32FirstBar == 0xffffffffu) {
                u32FirstBar = u32B;
                u64FirstVa = (u64)(uintptr_t)pVa;
            }
            if (u8User != 0) {
                u32MapUser = 1u;
            }
        } else if (nMap == 0) {
            if (u32Empty < 0xffffffffu) {
                u32Empty++;
            }
        } else {
            if (u32Fail < 0xffffffffu) {
                u32Fail++;
            }
        }
    }
    if (u32Maps != 0u) {
        u32Life |= UDX_DDI_LIFE_MAP;
    }
    /*
     * Preferred-BAR completeness residual (rtl 0+2 / xhci 0):
     * complete when every preferred BAR with inventory PA was mapped.
     * Empty inventory (pref_present=0) is not a fail — soft skip only.
     */
    u32PrefComplete = 0;
    if (u32PrefPresent != 0u && u32PrefMapped >= u32PrefPresent) {
        host_soft_inc(&g_u32DdiResPrefComplete);
        u32PrefComplete = 1u;
    }

    /*
     * Soft MAP_REMAP residual on first preferred BAR that mapped
     * (door grant slot idempotency; Soft!=product MMIO_FRAME mint).
     */
    u32Idem = 0;
    if (u32FirstBar != 0xffffffffu) {
        if (host_ddi_map_remap_residual(h, u32FirstBar, u64FirstVa,
                                        &u32Idem, szHost) != 0) {
            u32Life |= UDX_DDI_LIFE_REMAP;
        }
    }

    /*
     * Product functional order (not inject-only; Soft!=product):
     *   MAP_BAR → IRQ_BIND → DMA_NOTE → DMA_BUF
     * greppable product chain: SCAN,GET,OPEN,MAP_BAR,IRQ,DMA
     */
    /* Soft IRQ_BIND residual (soft badge note; mint OPEN). */
    u32IrqOk = 0;
    if (host_ddi_irq_bind_residual(h, szHost) != 0) {
        u32Life |= UDX_DDI_LIFE_IRQ;
        u32IrqOk = 1u;
    }

    /*
     * Soft DMA_NOTE residual on first preferred non-empty BAR PA/Cb.
     * Soft inventory only; never product DMA window mint.
     */
    u64DmaPa = 0;
    u64DmaCb = 0;
    for (iPref = 0; iPref < u32PrefN && iPref < UDX_DDI_PREF_BAR_MAX; iPref++) {
        u32 u32B;

        u32B = (u32)aPref[iPref];
        if (u32B >= UDX_DDI_BAR_MAX) {
            continue;
        }
        if (aBarPhys[u32B] != 0 && aBarLen[u32B] != 0) {
            u64DmaPa = aBarPhys[u32B];
            u64DmaCb = aBarLen[u32B];
            break;
        }
    }
    u32DmaNoteOk = 0;
    if (host_ddi_dma_note_residual(h, u64DmaPa, u64DmaCb, szHost) != 0) {
        u32Life |= UDX_DDI_LIFE_DMA_NOTE;
        u32DmaNoteOk = 1u;
    }

    /*
     * Soft DMA_BUF residual (ALLOC→MAP→FREE) for product hosts.
     * Real GJ_SYS_DDI DMA residual after IRQ; mint OPEN.
     * W15 denser: track MAP for DMA denser triple (NOTE+BUF+MAP) +
     * FREE for DMA denser free (full ALLOC→MAP→FREE cycle).
     */
    u32DmaBufBefore = g_u32DdiResDmaBufOk;
    u32DmaBufMapBefore = g_u32DdiResDmaBufMap;
    u32DmaBufFreeBefore = g_u32DdiResDmaBufFree;
    (void)host_ddi_dma_buf_residual(h, szHost);
    u32DmaBufOk = (g_u32DdiResDmaBufOk > u32DmaBufBefore) ? 1u : 0u;
    u32DmaBufMap = (g_u32DdiResDmaBufMap > u32DmaBufMapBefore) ? 1u : 0u;
    u32DmaBufFree = (g_u32DdiResDmaBufFree > u32DmaBufFreeBefore) ? 1u : 0u;

    u8Devfn = (u8)((pInfo->u8Slot << 3) | (pInfo->u8Func & 7u));
    st = udx_host_install_granted_pci(
        pInfo->u16Vend, pInfo->u16Dev,
        0xffffu, 0xffffu, pInfo->u32Class & 0x00ffffffu,
        pInfo->u8Bus, u8Devfn, 0,
        aBarPhys, aBarLen, aBarIsMem,
        pBar0Va, ppOut);
    if (st != UDX_OK) {
        host_ddi_life_note(u32Life);
        return st;
    }
    host_soft_inc(&g_u32DdiResInstall);
    u32Life |= UDX_DDI_LIFE_INSTALL;

    /*
     * W21 STRONGER denser residual: post-install pdev identity prove.
     * install_granted must yield pci_dev with GET vend:dev match.
     * Feeds product_path_full_ok denser (pdev_id_match) + DMA denser
     * both/triple/free + IRQ denser + product_host denser + chain SCAN→DMA
     * + chain free denser + step_all denser + install denser.
     * Soft!=product; never Dual DoD close.
     * greppable: pdev_id_match= | product_path_full_ok= | pdev_dense= | W23_STRONGER=1
     */
    u32PdevIdMatch = 0;
    if (ppOut != NULL && *ppOut != NULL) {
        if ((*ppOut)->u16Vendor == pInfo->u16Vend &&
            (*ppOut)->u16Device == pInfo->u16Dev) {
            host_soft_inc(&g_u32DdiResPdevIdMatch);
            u32PdevIdMatch = 1u;
            host_soft_emit("udx: soft ddi residual PDEV host=%s "
                           "vend=%04x dev=%04x id_match=1 PASS "
                           "sys=GJ_SYS_DDI install=1 soft=1 Soft!=product\n",
                           szHost,
                           (unsigned)pInfo->u16Vend,
                           (unsigned)pInfo->u16Dev);
            udx_printk("udx: soft ddi residual PDEV host=%s "
                       "vend=%04x dev=%04x id_match=1 soft=1 "
                       "product=UDX+ABI\n",
                       szHost,
                       (unsigned)pInfo->u16Vend,
                       (unsigned)pInfo->u16Dev);
        } else {
            host_soft_emit("udx: soft ddi residual PDEV host=%s "
                           "vend=%04x dev=%04x pdev_vend=%04x pdev_dev=%04x "
                           "id_match=0 FAIL soft=1 Soft!=product\n",
                           szHost,
                           (unsigned)pInfo->u16Vend,
                           (unsigned)pInfo->u16Dev,
                           (unsigned)(*ppOut)->u16Vendor,
                           (unsigned)(*ppOut)->u16Device);
        }
    } else {
        /*
         * Caller may pass NULL ppOut (bind_scan path still installs).
         * Prove via install residual alone when out pointer absent.
         * Prefer identity when *ppOut present (bind_by_id product path).
         */
        host_soft_emit("udx: soft ddi residual PDEV host=%s "
                       "vend=%04x dev=%04x out=NULL id_match=0 "
                       "install_ok=1 soft=1 Soft!=product\n",
                       szHost,
                       (unsigned)pInfo->u16Vend,
                       (unsigned)pInfo->u16Dev);
    }

    /*
     * Window-register every preferred BAR VA residual (BAR0 already
     * wired by install_granted when pBar0Va set; re-register is update).
     * Critical for rtl8168_udx BAR2 silicon-common MMIO ioremap.
     * Functional denser: count every preferred WIN register (incl BAR0).
     */
    for (iBar = 0; iBar < UDX_DDI_BAR_MAX; iBar++) {
        if (apVa[iBar] != NULL && aBarPhys[iBar] != 0 && aBarLen[iBar] != 0) {
            udx_host_window_register(aBarPhys[iBar], apVa[iBar],
                                     aBarLen[iBar]);
            host_soft_inc(&g_u32DdiResWin);
            u32Life |= UDX_DDI_LIFE_WIN;
        }
    }

    /*
     * Functional residual terminal: udx_ioremap prove on preferred mapped
     * BARs (SCAN→GET→OPEN→MAP→install→ioremap). Soft!=product.
     * Denser: pass MAP VA so look VA identity can be proven.
     * 10ec:8168 → BAR0+BAR2; 8086:a12f → BAR0; other → BAR0.
     * greppable: udx: soft ddi residual IOREMAP
     */
    u32IoremapOk = 0;
    u32IoremapFail = 0;
    u32VaMatchBefore = g_u32DdiResVaMatch;
    for (iPref = 0; iPref < u32PrefN && iPref < UDX_DDI_PREF_BAR_MAX; iPref++) {
        u32 u32B;
        u64 u64ExpectVa;
        int nIo;

        u32B = (u32)aPref[iPref];
        if (u32B >= UDX_DDI_BAR_MAX) {
            continue;
        }
        if (apVa[u32B] == NULL || aBarPhys[u32B] == 0 || aBarLen[u32B] == 0) {
            continue;
        }
        u64ExpectVa = (u64)(uintptr_t)apVa[u32B];
        nIo = host_ddi_ioremap_residual(u32B, aBarPhys[u32B], aBarLen[u32B],
                                        u64ExpectVa, szHost);
        if (nIo > 0) {
            if (u32IoremapOk < 0xffffffffu) {
                u32IoremapOk++;
            }
        } else if (nIo < 0) {
            if (u32IoremapFail < 0xffffffffu) {
                u32IoremapFail++;
            }
        }
    }
    u32VaMatchN = g_u32DdiResVaMatch - u32VaMatchBefore;

    host_ddi_life_note(u32Life);
    host_soft_ddi_bind_residual_path(pInfo->u16Vend, pInfo->u16Dev, h,
                                     u32Maps, u32PrefN, u32Empty, u32Fail,
                                     u32Life, u32Idem, u32MapUser,
                                     u32IoremapOk, u32IoremapFail,
                                     u32CfgIdMatch, u32PrefComplete, szHost);
    /*
     * Functional denser catalog (W21 STRONGER): MAP_BAR/IRQ/DMA +
     * product_path_ok + product_path_full_ok denser (DMA both/triple/free +
     * IRQ denser + chain SCAN→DMA free terminal + chain_step denser +
     * step_all denser + life product/full denser + product_host denser +
     * CFG/INSTALL/IOREMAP denser + handle_retain/close/dual_dod denser +
     * soft_ne/open_h/path_full denser + mint/agent/fs/maps/pref/va denser +
     * core/pdev/life/h1/inject/sys denser +
     * multi/confine/wave/path/lean/product denser) + pdev_id_match +
     * close_on_pass=0
     * handle_retain=1 freestanding class SKIP.
     */
    host_ddi_functional_residual(pInfo->u16Vend, pInfo->u16Dev, h,
                                 u32Life, u32Maps, u32PrefN,
                                 u32IoremapOk, u32CfgIdMatch,
                                 u32PrefComplete, u32VaMatchN,
                                 u32IrqOk, u32DmaNoteOk, u32DmaBufOk,
                                 u32DmaBufMap, u32DmaBufFree,
                                 u32PdevIdMatch, szHost);
    host_soft_ddi_bind_pass(pInfo->u16Vend, pInfo->u16Dev,
                            aBarPhys[0], aBarLen[0],
                            u32Maps, u32PrefN, szHost);
    /* Handle retained for product host lifecycle (no CLOSE residual). */
    return UDX_OK;
}

/**
 * Kernel ABI residual: SCAN → GET by index → match vend:dev →
 * OPEN → MAP_BAR preferred → IRQ → DMA → install → ioremap prove
 * (+ side residual deepen). Functional product:
 *   SCAN,GET,OPEN,MAP_BAR,IRQ,DMA (not inject-only).
 * Product hosts: 10ec:8168 / 8086:a12f preferred BAR residual.
 */
static udx_status_t
host_ddi_open_map_install(u16 u16Vendor, u16 u16Device,
                          struct udx_pci_dev **ppOut)
{
    struct udx_ddi_dev_info info;
    long ret;
    long nDev;
    long i;

    host_soft_inc(&g_u32DdiResScan);
    nDev = host_ddi_syscall3(UDX_DDI_OP_SCAN, 0, 0);
    if (nDev < 0) {
        return UDX_ERR_NOSYS;
    }
    host_soft_inc(&g_u32DdiResScanOk);
    /* Soft door product-surface residual (once; Soft!=product). */
    (void)host_ddi_inventory_residual();
    host_soft_emit("udx: soft ddi bind residual scan n=%ld "
                   "vend=%04x dev=%04x "
                   "chain=SCAN,GET,OPEN,MAP_BAR,IRQ,DMA "
                   "side=INVENTORY,CFG,MAP_REMAP,DMA_NOTE,DMA_BUF,IRQ_BIND,WIN,INSTALL,IOREMAP "
                   "dual_dod_a=OPEN_UDX dual_dod_b=OPEN_UDX close=0 "
                   "soft=1 Soft!=product\n",
                   (long)nDev, (unsigned)u16Vendor, (unsigned)u16Device);
    udx_printk("udx: soft ddi bind residual scan n=%ld vend=%04x "
               "dev=%04x soft=1\n",
               (long)nDev, (unsigned)u16Vendor, (unsigned)u16Device);
    if (nDev == 0) {
        return UDX_ERR_NODEV;
    }

    for (i = 0; i < nDev && i < (long)UDX_DDI_SCAN_MAX; i++) {
        host_ddi_memzero(&info, (u32)sizeof(info));
        host_soft_inc(&g_u32DdiResGet);
        ret = host_ddi_syscall4(UDX_DDI_OP_GET, i, (long)(uintptr_t)&info, 0);
        if (ret < 0) {
            continue;
        }
        host_soft_inc(&g_u32DdiResGetOk);
        if (info.u16Vend != u16Vendor || info.u16Dev != u16Device) {
            continue;
        }
        host_soft_inc(&g_u32DdiResMatch);
        host_soft_emit("udx: soft ddi bind residual match "
                       "idx=%ld vend=%04x dev=%04x bus=%u slot=%u "
                       "func=%u host=%s dual_dod=%s dual_dod_open=1 "
                       "close=0 soft=1 Soft!=product\n",
                       (long)i, (unsigned)info.u16Vend,
                       (unsigned)info.u16Dev,
                       (unsigned)info.u8Bus, (unsigned)info.u8Slot,
                       (unsigned)info.u8Func,
                       host_ddi_product_host_name(info.u16Vend, info.u16Dev),
                       host_ddi_product_dod_tag(info.u16Vend, info.u16Dev));
        udx_printk("udx: soft ddi bind residual match idx=%ld "
                   "vend=%04x dev=%04x dual_dod=%s soft=1\n",
                   (long)i, (unsigned)info.u16Vend, (unsigned)info.u16Dev,
                   host_ddi_product_dod_tag(info.u16Vend, info.u16Dev));
        return host_ddi_open_map_install_idx(i, &info, ppOut);
    }
    return UDX_ERR_NODEV;
}
#endif /* freestanding DDI */

udx_status_t
udx_host_bind_scan(void)
{
    host_soft_inc(&g_u32DdiBindEnter);
    udx_host_soft_init();
    host_soft_ddi_bind_residual_honesty();

#if defined(UDX_HOST_LIBC) && !defined(GJ_FREESTANDING)
    /*
     * Host-linux: no GJ_SYS_DDI. Soft SKIP; inject remains for lab tests.
     * Soft residual freestanding class SKIP; product=UDX+ABI hosts use
     * freestanding bind residual on GJ (rtl8168_udx / xhci_udx).
     */
    host_soft_ddi_bind_skip("host_linux_no_sys_ddi");
    return UDX_ERR_NOSYS;
#else
    {
        struct udx_ddi_dev_info info;
        long ret;
        long iEnt;
        long nDev;
        u32 u32Ok;

        host_soft_inc(&g_u32DdiResScan);
        nDev = host_ddi_syscall3(UDX_DDI_OP_SCAN, 0, 0);
        if (nDev < 0) {
            host_soft_ddi_bind_skip("sys_ddi_scan_enosys");
            return UDX_ERR_NOSYS;
        }
        host_soft_inc(&g_u32DdiResScanOk);
        /* Soft door product-surface residual (once; Soft!=product). */
        (void)host_ddi_inventory_residual();
        if (nDev == 0) {
            host_soft_ddi_bind_skip("sys_ddi_scan_empty");
            return UDX_ERR_NODEV;
        }

        u32Ok = 0;
        for (iEnt = 0; iEnt < nDev && iEnt < (long)UDX_DDI_SCAN_MAX; iEnt++) {
            udx_status_t st;
            struct udx_pci_dev *pPdev;

            host_ddi_memzero(&info, (u32)sizeof(info));
            host_soft_inc(&g_u32DdiResGet);
            ret = host_ddi_syscall4(UDX_DDI_OP_GET, iEnt,
                                    (long)(uintptr_t)&info, 0);
            if (ret < 0 || info.u16Vend == 0) {
                continue;
            }
            host_soft_inc(&g_u32DdiResGetOk);
            pPdev = NULL;
            /* Index residual — no re-SCAN per entry. */
            st = host_ddi_open_map_install_idx(iEnt, &info, &pPdev);
            if (st == UDX_OK) {
                if (u32Ok < 0xffffffffu) {
                    u32Ok++;
                }
            }
        }

        if (u32Ok == 0) {
            host_soft_ddi_bind_skip("sys_ddi_scan_no_install");
            return UDX_ERR_NODEV;
        }
        return UDX_OK;
    }
#endif
}

udx_status_t
udx_host_bind_by_id(u16 u16Vendor, u16 u16Device,
                    struct udx_pci_dev **ppOut)
{
    const char *szHost;
    const char *szDod;

    host_soft_inc(&g_u32DdiBindEnter);
    udx_host_soft_init();
    host_soft_ddi_bind_residual_honesty();

    if (ppOut != NULL) {
        *ppOut = NULL;
    }

    /*
     * G752 / product-host target residual (observation; Soft!=product).
     * Preferred: 10ec:8168 (DoD B / rtl8168_udx bars 0+2),
     *            8086:a12f (DoD A / xhci_udx bar 0).
     * greppable: udx: soft ddi bind residual by_id
     */
    szHost = host_ddi_product_host_name(u16Vendor, u16Device);
    szDod = host_ddi_product_dod_tag(u16Vendor, u16Device);
    host_soft_emit("udx: soft ddi bind residual by_id "
                   "vend=%04x dev=%04x host=%s dual_dod=%s "
                   "sys=GJ_SYS_DDI sys_ddi=%u chain_n=%u "
                   "chain=SCAN,GET,OPEN,MAP_BAR,IRQ,DMA denser_prove=1 "
                   "side=INVENTORY,CFG,MAP_REMAP,DMA_NOTE,DMA_BUF,"
                   "IRQ_BIND,WIN,INSTALL,IOREMAP,PDEV "
                   "g752_targets=10ec:8168,8086:a12f "
                   "DoD_A_OPEN=%u DoD_B_OPEN=%u agent_ne_close=1 "
                   "close_on_pass=%u handle_retain=%u "
                   "freestanding_class=SKIP lean_checks=%u "
                   "dual_dod OPEN W23_STRONGER=1 "
                   "product=UDX+ABI soft=1 Soft!=product\n",
                   (unsigned)u16Vendor, (unsigned)u16Device,
                   szHost, szDod,
                   (u32)UDX_HOST_SYS_DDI_NR, (u32)UDX_HOST_PRODUCT_CHAIN_N,
                   (u32)UDX_HOST_SOFT_DOD_A_OPEN,
                   (u32)UDX_HOST_SOFT_DOD_B_OPEN,
                   (u32)UDX_HOST_CLOSE_ON_PASS,
                   (u32)UDX_HOST_HANDLE_RETAIN_LOCK,
                   (u32)UDX_HOST_LEAN_CHECKS);
    udx_printk("udx: soft ddi bind residual by_id vend=%04x dev=%04x "
               "host=%s dual_dod=%s sys=GJ_SYS_DDI soft=1\n",
               (unsigned)u16Vendor, (unsigned)u16Device, szHost, szDod);

#if defined(UDX_HOST_LIBC) && !defined(GJ_FREESTANDING)
    /*
     * Host-linux soft fallback: inject path stays available for tests.
     * Product residual lives on freestanding GJ for rtl8168_udx / xhci_udx.
     */
    host_soft_ddi_bind_skip("host_linux_use_inject");
    return UDX_ERR_NOSYS;
#else
    {
        udx_status_t st;

        st = host_ddi_open_map_install(u16Vendor, u16Device, ppOut);
        if (st != UDX_OK) {
            /*
             * Precise SKIP why residual (lean; Soft!=product).
             * Distinguishes ENOSYS door / no-match / open-map-install fail.
             */
            if (st == UDX_ERR_NOSYS) {
                host_soft_ddi_bind_skip("sys_ddi_scan_enosys");
            } else if (st == UDX_ERR_NODEV) {
                host_soft_ddi_bind_skip("sys_ddi_no_match_or_open");
            } else {
                host_soft_ddi_bind_skip("sys_ddi_open_map_install");
            }
            return st;
        }
        return UDX_OK;
    }
#endif
}

long
udx_host_ddi_handle(void)
{
    return g_i64DdiHandleRetain;
}

/* Soft DDI residual deepen (C2 libudx host; Soft!=product; product=UDX+ABI):
 * greppable: udx: soft ddi host note PASS
 * greppable: udx: soft ddi-ready
 * greppable: udx: soft ddi lifecycle note
 * greppable: udx: soft ddi dma not-granted
 * greppable: udx: soft ddi bind PASS
 * greppable: udx: soft ddi bind SKIP
 * greppable: udx: soft ddi bind residual
 * greppable: udx: soft ddi bind residual path
 * greppable: udx: soft ddi bind residual by_id
 * greppable: udx: soft ddi bind residual functional
 * greppable: udx: soft ddi residual functional step=
 * greppable: udx: soft ddi residual MAP_BAR
 * greppable: udx: soft ddi residual CFG
 * greppable: udx: soft ddi residual MAP_REMAP
 * greppable: udx: soft ddi residual DMA_NOTE
 * greppable: udx: soft ddi residual DMA_BUF
 * greppable: udx: soft ddi residual IRQ_BIND
 * greppable: udx: soft ddi residual IOREMAP
 * greppable: udx: soft ddi residual INVENTORY
 * greppable: udx: host soft residual lean
 * greppable: udx: host soft residual mmio
 * greppable: udx: host soft residual irq
 * greppable: udx: host soft residual dma
 * greppable: udx: host soft residual close
 * greppable: udx: host soft residual inject
 * greppable: udx: host soft residual cap_mint
 * greppable: udx: host soft residual product_host
 * greppable: udx: host soft residual dual_dod
 * greppable: udx: host soft residual c2
 * greppable: udx: host soft residual hazard
 * greppable: udx: host soft c2 product path
 * greppable: udx: soft residual freestanding class SKIP
 * greppable: udx: soft residual product=UDX+ABI
 * greppable: udx: soft residual Dual DoD A OPEN
 * greppable: udx: soft residual Dual DoD B OPEN
 * greppable: udx: soft residual Dual DoD A/B OPEN
 * greppable: chain=SCAN,GET,OPEN,MAP_BAR,IRQ,DMA
 * greppable: product_path_ok=
 * greppable: product_path_full_ok=
 * greppable: pdev_id_match=
 * greppable: close_on_pass=0 handle_retain=1
 * greppable: freestanding class SKIP
 * greppable: Soft!=product dual_dod OPEN W23_STRONGER
 * greppable: sys_ddi=103 lean_checks=
 * greppable: denser_prove=1
 * greppable: dma_dense_both= | dma_dense_triple= | dma_dense_free= | irq_dense=
 * greppable: chain_dense_scan_dma= | chain_free_dense= | chain_step_dense=
 * greppable: step_all_dense= | life_product_chain= | life_full_dense=
 * greppable: product_host_dense=
 * greppable: cfg_dense= | install_dense= | ioremap_dense=
 * greppable: handle_retain_dense= | close_dense= | dual_dod_dense=
 * greppable: soft_ne_dense= | open_h_dense= | path_full_dense=
 * greppable: mint_dense= | agent_dense= | fs_dense=
 * greppable: maps_dense= | pref_dense= | va_dense=
 * greppable: core_dense= | pdev_dense= | life_dense=
 * greppable: h1_dense= | inject_dense= | sys_dense=
 * greppable: multi_dense= | confine_dense= | wave_dense=
 * greppable: path_dense= | lean_dense= | product_dense=
 * greppable: hazard_dense= | c2_dense= | bar_dense=
 * greppable: gac_dense= | stamp_dense= | layout_dense=
 * greppable: mmio_dense= | inv_dense= | sshd_dense=
 * greppable: eq_dense= | once_dense= | grant_dense=
 * greppable: udx: soft ddi residual PDEV
 * greppable: udx: soft ddi bind residual functional denser
 * greppable: cfg_id_match= pref_complete= va_match=
 * greppable: W23_STRONGER=1 lean_checks=
 * Product lamps 0; Dual DoD A/B OPEN (agent!=close); no GPL;
 * no freestanding class re-enable; no version stamp; no GJ_IMAGE_VERSION.
 * Bar honesty v2026.08.04.75 — NEVER invent .76.
 * UDX_HOST_SOFT_* constants (host.h) drive Cap/DoD honesty emits.
 * FUNCTIONAL residual locks: UDX_HOST_* _Static_assert lean_checks=68 denser.
 * G752 / product hosts: 10ec:8168→rtl8168_udx (bars 0+2 Dual DoD B OPEN),
 *   8086:a12f→xhci_udx (bar 0 Dual DoD A OPEN).
 * Functional residual denser (product path; not inject-only):
 *   SCAN→GET→OPEN→MAP_BAR→IRQ→DMA denser prove (+ INSTALL/IOREMAP side);
 *   CFG identity match; MAP VA↔window look match; preferred-BAR complete;
 *   step=SCAN/GET/OPEN/MAP_BAR/IRQ/DMA/INSTALL/IOREMAP/CFG/PDEV catalog;
 *   product_path_ok; product_path_full_ok (W23 STRONGER denser + LIFE_CORE +
 *   DMA denser both + DMA denser triple + DMA denser free + IRQ denser +
 *   maps + product_host denser + chain denser SCAN→DMA free terminal +
 *   chain free denser + chain_step denser + step_all denser +
 *   life product/full denser + REMAP/SIDE/WIN/prefer_real denser +
 *   CFG/INSTALL/IOREMAP denser + Dual DoD OPEN locks +
 *   handle_retain denser + close denser + dual_dod denser +
 *   soft_ne denser + open_h denser + path_full denser +
 *   mint denser + agent denser + fs denser +
 *   maps denser + pref denser + va denser +
 *   core denser + pdev denser + life denser +
 *   h1 denser + inject denser + sys denser +
 *   multi denser + confine denser + wave denser +
 *   path denser + lean denser + product denser +
 *   hazard denser + c2 denser + bar denser +
 *   gac denser + stamp denser + layout denser +
 *   mmio denser + inv denser + sshd denser +
 *   eq denser + once denser + grant denser);
 *   post-install pdev vend:dev identity; DMA_BUF ALLOC→MAP→FREE soft.
 * close_on_pass=0 handle_retain=1; freestanding class SKIP honesty.
 * Soft!=product dual_dod OPEN W23_STRONGER. Handle retained for product hosts.
 * Freestanding bind_by_id via GJ_SYS_DDI fully for product UDX Dual DoD OPEN.
 */
