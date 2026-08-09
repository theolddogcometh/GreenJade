/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft Linux PHY / MDIO seed (clean-room).
 * Dual MIT OR Apache-2.0. No GPL / no Linux source.
 *
 * Purpose
 * -------
 * Soft bodies so a soft-loaded r8169 (and libphy/mdio helpers) that touch
 * phy_* / mdiobus_* do not crash on NULL ksym stubs and can soft-complete
 * init and post-probe lifecycle. Complements F2 linux_ksym zero-return stubs:
 * this TU owns real soft bodies that return non-NULL soft objects where needed;
 * linux_phy_soft_init() registers them via linux_ksym_register (replacing
 * earlier zero stubs). r8169.ko und phy/mdio/genphy class is complete here.
 *
 * Residual lean (hybrid net eng honesty only):
 * Soft incomplete phy_device / mii_bus front shapes + denser clause-22 /
 * MMD-ish MDIO shadow + genphy_* status/link residual. Soft != product copper.
 * Soft != freestanding thrash: shadow MDIO only; never BAR 0xB0 / CF8 /
 * rtl8168 MDIO helpers. Soft!=product · G-AC-1 (no .ko product AC).
 *
 * Residual lean deepen (layout_ver ≥ 3 · C0 eng only):
 * Soft phy state machine bookkeeping + BMCR/BMSR front mirrors + page +
 * thrash-refuse tallies on soft_phy_device / soft_mii_bus. R0 thrash refuse
 * once-lamp: every soft MDIO op stays shadow-only (no freestanding dual-drive).
 * Dual DoD B residual companion (gate0): freestanding_rtl=SKIP default;
 * product_nic=UDX; soft PHY is control/bookkeeping only (never product link).
 * Soft!=product · G-AC-1 · no stamp storms.
 *
 * Residual lean deepen (layout_ver ≥ 4 · future UDX NIC eng):
 * Soft front stamps u8UdxEng / u8NeverPhyar / u8ProductOpen / u8Gate0 +
 * genphy/thrash tallies so future userspace UDX NIC eng can lean on a stable
 * soft bookkeeping shape. Soft!= freestanding rtl thrash; product copper OPEN.
 * Soft!=product · G-AC-1 · once-lamps only (no stamp storms).
 *
 * Residual lean deepen (layout_ver ≥ 5 · LAW residual):
 * freestanding_rtl=SKIP · product_nic=UDX · never product link claim.
 * Soft front stamps u8FsRtlSkip / u8ProductNicUdx / u8NoProductLink.
 * Soft link lamps are bookkeeping only (Soft!=product copper / UDX wire).
 * Dual DoD B product direction = userspace UDX; freestanding rtl default SKIP.
 * Soft!=product · G-AC-1 · once-lamps only (no stamp storms).
 *
 * Soft != ABI-stable
 * -----------------
 * struct soft_phy_device / soft_mii_bus / opaque phy_device / mii_bus here are
 * INCOMPLETE soft shapes. Field order and size are NOT guaranteed to match any
 * Linux kernel version. Do not claim binary .ko layout compatibility.
 *
 * Soft != product: no real MDIO bus, no copper link, no .ko PHY driver load.
 * Never product link claim: soft BMSR/LSTATUS / link lamps != product copper.
 * Product NIC = userspace UDX. Freestanding rtl default SKIP (not product).
 *
 * Non-destructive to freestanding wire (hybrid 4a / gate0 · residual):
 * Soft MDIO is clause-22 (+ soft MMD) shadow only (hybrid-safe soft-only path)
 * - never programs 10ec:8168 BAR MDIO (0xB0-class), never CF8, never calls
 * freestanding rtl8168 MDIO. Soft start/stop/aneg only update soft BMSR /
 * soft link lamps. Freestanding rtl default SKIP; product NIC = UDX.
 * Grep: linux_phy_soft: soft hybrid zero-touch PASS
 * Grep: linux_phy_soft: soft residual lean PASS
 * Grep: linux_phy_soft: soft residual freestanding_rtl=SKIP
 * Grep: linux_phy_soft: soft residual product_nic=UDX
 * Grep: linux_phy_soft: soft residual lean UDX eng
 * Grep: linux_phy_soft: soft residual R0 thrash refuse
 * Grep: linux_phy_soft: soft dual DoD B residual gate0 PASS
 *
 * Soft libphy eng marker (no real libphy.ko):
 * linux_phy_soft_init registers soft-virtual deps libphy + mdio_devres so
 * r8169 soft load does not spuriously FAIL KSYM need:libphy (eng only).
 * Grep: linux_module: soft dep virtual libphy soft=1 product=0
 * Grep: linux_phy_soft: soft dep virtual register libphy+mdio_devres
 *
 * Greppable markers (keep stable; once-lamps only - no stamp storms):
 *   linux_phy_soft: soft init PASS n=
 *   linux_phy_soft: soft hybrid zero-touch PASS
 *   linux_phy_soft: soft residual lean PASS
 *   linux_phy_soft: soft residual freestanding_rtl=SKIP
 *   linux_phy_soft: soft residual product_nic=UDX
 *   linux_phy_soft: soft residual lean UDX eng
 *   linux_phy_soft: soft residual layout ...
 *   linux_phy_soft: soft residual R0 thrash refuse ...
 *   linux_phy_soft: soft dual DoD B residual gate0 PASS ...
 *   linux_phy_soft: soft dep virtual register libphy+mdio_devres ...
 *   linux_phy_soft: soft connect ...
 *   linux_phy_soft: soft start ...
 *   linux_phy_soft: soft stop ...
 *   linux_phy_soft: soft reset ...
 *   linux_phy_soft: soft mdiobus ...
 *   linux_phy_soft: soft phy_read ...
 *   linux_phy_soft: soft phy_write ...
 *   linux_phy_soft: soft link ...
 *   linux_phy_soft: soft genphy ...
 *
 * See docs/LINUX_MODULE_PATH.md · docs/LAPTOP_LINUX_DRIVER_HOST.md.
 */
#pragma once

#include <gj/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Soft object sizes (opaque blob large enough for soft field writes). */
#define LINUX_PHY_SOFT_PHYDEV_CB   512u
#define LINUX_PHY_SOFT_MIIBUS_CB   512u

/* Soft clause-22 register shadow (post-probe r8169 MDIO soft). */
#define LINUX_PHY_SOFT_REG_MAX     32u

/* Soft MMD-ish shadow dims (clause-45 folded; Soft!=product MMD space). */
#define LINUX_PHY_SOFT_MMD_DEV_MAX 8u
#define LINUX_PHY_SOFT_MMD_REG_MAX 32u

/* Soft default link bookkeeping (never product link; freestanding_rtl=SKIP). */
#define LINUX_PHY_SOFT_SPEED_DEFAULT 1000
#define LINUX_PHY_SOFT_DUPLEX_FULL   1
#define LINUX_PHY_SOFT_DUPLEX_HALF   0

/*
 * Soft layout residual lean (eng only; Soft!=host ABI; not an image stamp).
 * Bump when soft_phy_device / soft_mii_bus front field set grows.
 * Grep: linux_phy_soft: soft residual lean PASS layout_ver=
 * ver3: state + BMCR/BMSR mirrors + page + thrash-refuse tallies.
 * ver4: UDX eng residual stamps (udx_eng/never_phyar/product_open/gate0) +
 *       genphy/thrash tally mirrors for future UDX NIC eng (not rtl thrash).
 * ver5: freestanding_rtl=SKIP + product_nic=UDX + no product link claim
 *       (u8FsRtlSkip / u8ProductNicUdx / u8NoProductLink). LAW residual.
 */
#define LINUX_PHY_SOFT_LAYOUT_VER  5u

/* Soft magic markers ('GJph' / 'GJmb' LE) - debugger / residual honesty. */
#define LINUX_PHY_SOFT_PHYDEV_MAGIC  0x68704a47u /* 'GJph' little-endian */
#define LINUX_PHY_SOFT_MIIBUS_MAGIC  0x626d4a47u /* 'GJmb' little-endian */

/*
 * Soft phy state bookkeeping (Soft!=Linux phy_state enum ABI).
 * Residual lean only - freestanding_rtl=SKIP; product_nic=UDX.
 */
#define LINUX_PHY_SOFT_STATE_DOWN    0
#define LINUX_PHY_SOFT_STATE_READY   1
#define LINUX_PHY_SOFT_STATE_UP      2
#define LINUX_PHY_SOFT_STATE_NOLINK  3
#define LINUX_PHY_SOFT_STATE_HALTED  4

/**
 * Soft-shaped phy_device front (incomplete Soft!=Linux phy_device ABI).
 * Overlay on g_aSoftPhyDev[LINUX_PHY_SOFT_PHYDEV_CB]. Remaining bytes pad.
 * Soft residual lean for UDX/hybrid eng - freestanding_rtl=SKIP default.
 * layout_ver ≥ 3: nState / page / BMCR·BMSR mirrors / thrash refuse tally.
 * layout_ver ≥ 4: UDX eng residual stamps + genphy tally (future UDX NIC eng).
 * layout_ver ≥ 5: freestanding_rtl SKIP + product_nic UDX + no product link.
 */
struct soft_phy_device {
    u32   u32Magic;       /* LINUX_PHY_SOFT_PHYDEV_MAGIC */
    u32   u32LayoutVer;   /* LINUX_PHY_SOFT_LAYOUT_VER */
    void *pAttachedDev;   /* soft netdev cookie (may be NULL) */
    void *pBus;           /* soft mii_bus cookie */
    void *pfnAdjustLink;  /* soft link-change handler cookie */
    int   nAddr;          /* soft PHY address */
    int   nIface;         /* soft phy_interface_t-ish */
    int   nSpeed;         /* Mbps bookkeeping */
    int   nDuplex;        /* 1=full 0=half */
    int   nLink;          /* 1=up soft bookkeeping (never product link) */
    int   nAutoneg;       /* 1=AN enable soft */
    int   nPause;         /* soft pause advertise lamp */
    int   nAsymPause;     /* soft asym pause lamp */
    int   nMaxSpeed;      /* 0 = no cap */
    u16   u16PhyId1;
    u16   u16PhyId2;
    u8    u8Connected;
    u8    u8Started;
    u8    u8SoftOnly;     /* always 1 - Soft!=product */
    u8    u8WireSafe;     /* always 1 - no freestanding thrash */
    /* layout_ver ≥ 3 residual lean front */
    int   nState;         /* LINUX_PHY_SOFT_STATE_* bookkeeping */
    int   nPage;          /* soft clause-22 page select shadow */
    u16   u16Bmcr;        /* soft BMCR mirror (reg 0) */
    u16   u16Bmsr;        /* soft BMSR mirror (reg 1) */
    void *pPriv;          /* soft private cookie */
    u32   u32ShadowOps;   /* shadow MDIO ops tally (thrash refuse) */
    u32   u32LinkChanges; /* soft link-change count mirror */
    /* layout_ver ≥ 4 residual lean for future UDX NIC eng */
    u8    u8UdxEng;       /* always 1 - soft residual lean surface for UDX */
    u8    u8NeverPhyar;   /* always 1 - never PHYAR/BAR 0xB0 thrash */
    u8    u8ProductOpen;  /* always 1 - product copper OPEN (G-AC-1) */
    u8    u8Gate0;        /* always 1 - hybrid gate0 soft-only MDIO */
    u32   u32GenphyOps;   /* genphy_* soft call tally mirror */
    /* layout_ver ≥ 5 residual lean LAW stamps */
    u8    u8FsRtlSkip;    /* always 1 - freestanding_rtl=SKIP residual */
    u8    u8ProductNicUdx;/* always 1 - product_nic=UDX residual */
    u8    u8NoProductLink;/* always 1 - never product link claim */
    u8    u8PadVer5;      /* reserved residual pad */
    /* pad to LINUX_PHY_SOFT_PHYDEV_CB via blob storage */
};

/**
 * Soft-shaped mii_bus front (incomplete Soft!=Linux mii_bus ABI).
 * Overlay on g_aSoftMiiBus[LINUX_PHY_SOFT_MIIBUS_CB]. Soft residual lean.
 * layout_ver ≥ 3: shadow-op tally + soft wire-safe + thrash-refuse lamps.
 * layout_ver ≥ 5: freestanding_rtl SKIP + product_nic UDX + no product link.
 */
struct soft_mii_bus {
    u32   u32Magic;       /* LINUX_PHY_SOFT_MIIBUS_MAGIC */
    u32   u32LayoutVer;   /* LINUX_PHY_SOFT_LAYOUT_VER */
    void *pPriv;          /* soft private cookie */
    void *pParent;        /* soft parent device cookie */
    int   nRegistered;    /* 1 after soft mdiobus_register */
    int   nPhyMask;       /* soft scan mask (eng) */
    char  szId[32];       /* soft bus id string */
    u8    u8SoftOnly;     /* always 1 */
    u8    u8WireSafe;     /* always 1 - shadow MDIO only */
    u8    u8ShadowOnly;   /* always 1 - never BAR 0xB0 / CF8 */
    u8    u8ThrashRefuse; /* always 1 - R0 thrash refuse sticky */
    u32   u32ShadowOps;   /* soft mdiobus read+write shadow tally */
    /* layout_ver ≥ 4 residual lean for future UDX NIC eng */
    u8    u8UdxEng;       /* always 1 - future UDX NIC eng lean surface */
    u8    u8NeverPhyar;   /* always 1 - never PHYAR-class BAR thrash */
    u8    u8ProductOpen;  /* always 1 - product copper OPEN */
    u8    u8Gate0;        /* always 1 - hybrid gate0 soft-only */
    u32   u32ThrashRefuse;/* shadow thrash-refuse tally mirror */
    /* layout_ver ≥ 5 residual lean LAW stamps */
    u8    u8FsRtlSkip;    /* always 1 - freestanding_rtl=SKIP residual */
    u8    u8ProductNicUdx;/* always 1 - product_nic=UDX residual */
    u8    u8NoProductLink;/* always 1 - never product link claim */
    u8    u8PadVer5;      /* reserved residual pad */
    /* pad to LINUX_PHY_SOFT_MIIBUS_CB via blob storage */
};

/**
 * Soft init: idempotent ready lamp + linux_ksym_register of soft bodies.
 * Prefer: linux_ksym_register("phy_connect_direct", phy_connect_direct) ...
 * when F2 linux_ksym is linked (weak-call safe if absent).
 * Grep: linux_phy_soft: soft init PASS n=
 */
void linux_phy_soft_init(void);

/** Non-zero after successful soft init. */
int  linux_phy_soft_ready(void);

/**
 * Once-lamp soft residual lean honesty (layout / genphy / wire-safe / UDX eng).
 * Soft!=product; freestanding_rtl=SKIP; product_nic=UDX; never product link.
 * Hard-capped (no stamp storms). Returns 1 if lamp fired.
 * Grep: linux_phy_soft: soft residual lean PASS
 * Grep: linux_phy_soft: soft residual freestanding_rtl=SKIP
 * Grep: linux_phy_soft: soft residual product_nic=UDX
 * Grep: linux_phy_soft: soft residual layout
 * Grep: linux_phy_soft: soft residual lean UDX eng
 */
int  linux_phy_soft_residual_lean_lamp_once(void);

/**
 * Once-lamp soft residual R0 thrash refuse honesty (shadow MDIO only).
 * Soft!=product; freestanding_rtl=SKIP; product_nic=UDX. Returns 1 if lamp fired.
 * Grep: linux_phy_soft: soft residual R0 thrash refuse
 */
int  linux_phy_soft_thrash_refuse_lamp_once(void);

/**
 * Once-lamp dual DoD B residual gate0 companion (PHY soft bookkeeping).
 * Soft!=product; product_nic=UDX OPEN; freestanding_rtl=SKIP. Returns 1 if lamp fired.
 * Grep: linux_phy_soft: soft dual DoD B residual gate0 PASS
 */
int  linux_phy_soft_dual_dod_b_lamp_once(void);

/*
 * Soft bodies with Linux C names - ksym / modules resolve these symbols.
 * Arguments are void * / int so F2 stubs and soft callers share signatures
 * without claiming Linux ABI struct layout under the pointer.
 *
 * Pointer returns (non-NULL soft static objects):
 *   phy_connect, mdiobus_get_phy, devm_mdiobus_alloc_size, mdiobus_alloc_size
 *   mdiobus_alloc, devm_mdiobus_alloc
 * phy_connect_direct: Linux-shaped int success 0; soft phy is via mdiobus_get_phy
 *   / the pPhy argument already held by the caller (r8169 soft-complete).
 * Int / void returns: soft success 0 (ethtool EOPNOTSUPP soft as 0) where safe.
 * phy_read / mdiobus_read: return soft register data (0..0xffff), not errno.
 * All MDIO paths: shadow only - R0 thrash refuse (never BAR / CF8 / rtl MDIO).
 */

/* ---- phy connect / lifecycle ------------------------------------------ */
void *phy_connect(void *pNetdev, const char *szBusId, void *pfnHandler,
                  int nIface);
int   phy_connect_direct(void *pNetdev, void *pPhy, void *pfnHandler,
                         int nIface);
void  phy_disconnect(void *pPhy);
void  phy_start(void *pPhy);
void  phy_stop(void *pPhy);
int   phy_start_aneg(void *pPhy);
int   phy_init_hw(void *pPhy);
int   phy_resume(void *pPhy);
int   phy_suspend(void *pPhy);
void  phy_attached_info(void *pPhy);
void  phy_print_status(void *pPhy);
int   phy_set_max_speed(void *pPhy, int nSpeed);
int   phy_aneg_done(void *pPhy);
int   genphy_soft_reset(void *pPhy);

/* ---- genphy residual lean (shadow status; Soft!=product copper) -------- */
int   genphy_read_status(void *pPhy);
int   genphy_update_link(void *pPhy);
int   genphy_restart_aneg(void *pPhy);
int   genphy_check_and_restart_aneg(void *pPhy, int fRestart);
int   genphy_read_lpa(void *pPhy);
int   genphy_read_abilities(void *pPhy);
int   genphy_config_aneg(void *pPhy);
int   genphy_setup_forced(void *pPhy);
int   genphy_resume(void *pPhy);
int   genphy_suspend(void *pPhy);

/* ---- MDIO bus --------------------------------------------------------- */
int   mdiobus_read(void *pBus, int nAddr, u32 u32Regnum);
int   mdiobus_write(void *pBus, int nAddr, u32 u32Regnum, u16 u16Val);
int   __mdiobus_read(void *pBus, int nAddr, u32 u32Regnum);
int   __mdiobus_write(void *pBus, int nAddr, u32 u32Regnum, u16 u16Val);
void *mdiobus_get_phy(void *pBus, int nAddr);
void *mdiobus_alloc(void);
void *mdiobus_alloc_size(int nSizeofPriv);
void *devm_mdiobus_alloc(void *pDev);
void *devm_mdiobus_alloc_size(void *pDev, int nSizeofPriv);
void  mdiobus_free(void *pBus);
int   mdiobus_register(void *pBus);
int   __mdiobus_register(void *pBus, void *pOwner);
int   __devm_mdiobus_register(void *pDev, void *pBus, void *pOwner);
void  mdiobus_unregister(void *pBus);

/* ---- phy register / page helpers -------------------------------------- */
int   phy_read(void *pPhy, u32 u32Regnum);
int   phy_write(void *pPhy, u32 u32Regnum, u16 u16Val);
int   __phy_read(void *pPhy, u32 u32Regnum);
int   __phy_write(void *pPhy, u32 u32Regnum, u16 u16Val);
int   phy_read_paged(void *pPhy, int nPage, u32 u32Regnum);
int   phy_write_paged(void *pPhy, int nPage, u32 u32Regnum, u16 u16Val);
int   phy_read_mmd(void *pPhy, int nDevad, u32 u32Regnum);
int   phy_write_mmd(void *pPhy, int nDevad, u32 u32Regnum, u16 u16Val);
int   phy_modify(void *pPhy, u32 u32Regnum, u16 u16Mask, u16 u16Set);
int   phy_modify_paged(void *pPhy, int nPage, u32 u32Regnum, u16 u16Mask,
                       u16 u16Set);
int   phy_modify_mmd(void *pPhy, int nDevad, u32 u32Regnum, u16 u16Mask,
                     u16 u16Set);
int   __phy_modify(void *pPhy, u32 u32Regnum, u16 u16Mask, u16 u16Set);
int   phy_select_page(void *pPhy, int nPage);
int   phy_restore_page(void *pPhy, int nOldPage, int nRet);
int   phy_save_page(void *pPhy);

/* ---- pause / eee / speed / ioctl -------------------------------------- */
void  phy_mac_interrupt(void *pPhy);
void  phy_get_pause(void *pPhy, u8 *pu8Tx, u8 *pu8Rx);
void  phy_set_asym_pause(void *pPhy, int nRx, int nTx);
void  phy_support_asym_pause(void *pPhy);
void  phy_support_sym_pause(void *pPhy);
void  phy_set_sym_pause(void *pPhy, int nRx, int nTx, int nAutoneg);
void  phy_support_eee(void *pPhy);
void  phy_resolve_aneg_pause(void *pPhy);
int   phy_advertise_eee_all(void *pPhy);
int   phy_speed_down(void *pPhy, int fSync);
int   phy_speed_up(void *pPhy);
int   phy_do_ioctl_running(void *pNetdev, void *pIfr, int nCmd);

/* ---- ethtool (soft success 0; EOPNOTSUPP soft as 0) -------------------- */
int   phy_ethtool_nway_reset(void *pNetdev);
int   phy_ethtool_get_eee(void *pPhy, void *pData);
int   phy_ethtool_set_eee(void *pPhy, void *pData);
int   phy_ethtool_get_link_ksettings(void *pNetdev, void *pCmd);
int   phy_ethtool_set_link_ksettings(void *pNetdev, const void *pCmd);
/* Aliases used by some und tables (same soft bodies). */
int   phy_ethtool_ksettings_get(void *pNetdev, void *pCmd);
int   phy_ethtool_ksettings_set(void *pNetdev, const void *pCmd);

/** Soft diagnostics (eng honesty; Soft!=product metrics). */
u32   linux_phy_soft_ksym_ok(void);
u32   linux_phy_soft_ksym_skip(void);
u32   linux_phy_soft_connect_count(void);
u32   linux_phy_soft_start_count(void);
/**
 * Soft link-up lamp (1=up); bookkeeping only.
 * Never product link claim; freestanding_rtl=SKIP; product_nic=UDX.
 */
int   linux_phy_soft_link_up(void);
/** Soft negotiated speed (Mbps); 0 if soft link down. Soft!=product link. */
int   linux_phy_soft_speed_mbps(void);
/** Soft duplex: 1=full, 0=half (when soft link up). Soft!=product link. */
int   linux_phy_soft_duplex_full(void);
u32   linux_phy_soft_link_change_count(void);
u32   linux_phy_soft_mdio_rd_count(void);
u32   linux_phy_soft_mdio_wr_count(void);
u32   linux_phy_soft_phy_rd_count(void);
u32   linux_phy_soft_phy_wr_count(void);
u32   linux_phy_soft_mmd_rd_count(void);
u32   linux_phy_soft_mmd_wr_count(void);
/** Soft layout residual lean stamp (LINUX_PHY_SOFT_LAYOUT_VER) or 0. */
u32   linux_phy_soft_layout_ver(void);
/** Soft bus-registered lamp (1 after mdiobus_register). */
int   linux_phy_soft_bus_registered(void);
/** Always 1: soft MDIO never thrash freestanding BAR / copper. */
int   linux_phy_soft_wire_safe(void);
/** Soft thrash-refuse tally (shadow MDIO ops that refused BAR touch). */
u32   linux_phy_soft_thrash_refuse_count(void);
/** Soft phy state bookkeeping (LINUX_PHY_SOFT_STATE_*). */
int   linux_phy_soft_state(void);
/** Always 1: residual lean surface for future UDX NIC eng (Soft!=product). */
int   linux_phy_soft_udx_eng(void);
/** Soft genphy_* call tally (eng residual; not product metrics). */
u32   linux_phy_soft_genphy_count(void);
/** Always 1: freestanding_rtl=SKIP residual honesty (Soft!=product). */
int   linux_phy_soft_freestanding_rtl_skip(void);
/** Always 1: product_nic=UDX residual honesty (Soft!=product; G-AC-1). */
int   linux_phy_soft_product_nic_udx(void);
/** Always 1: soft link lamps never claim product copper/link. */
int   linux_phy_soft_no_product_link(void);

#ifdef __cplusplus
}
#endif
