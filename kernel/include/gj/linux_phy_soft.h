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
 * init. Complements F2 linux_ksym zero-return stubs: this TU owns real soft
 * bodies that return non-NULL soft objects where needed; linux_phy_soft_init()
 * registers them via linux_ksym_register (replacing earlier zero stubs).
 *
 * Soft ≠ ABI-stable
 * -----------------
 * struct phy_device / mii_bus here are INCOMPLETE soft shapes. Field order and
 * size are NOT guaranteed to match any Linux kernel version. Do not claim
 * binary .ko layout compatibility.
 *
 * Greppable markers (keep stable):
 *   linux_phy_soft: soft init PASS n=
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

/**
 * Soft init: idempotent ready lamp + linux_ksym_register of soft bodies.
 * Prefer: linux_ksym_register("phy_connect_direct", phy_connect_direct) …
 * when F2 linux_ksym is linked (weak-call safe if absent).
 * Grep: linux_phy_soft: soft init PASS n=
 */
void linux_phy_soft_init(void);

/** Non-zero after successful soft init. */
int  linux_phy_soft_ready(void);

/*
 * Soft bodies with Linux C names — ksym / modules resolve these symbols.
 * Arguments are void * / int so F2 stubs and soft callers share signatures
 * without claiming Linux ABI struct layout under the pointer.
 *
 * Pointer returns (non-NULL soft static objects):
 *   mdiobus_get_phy, devm_mdiobus_alloc_size
 * phy_connect_direct: Linux-shaped int success 0; soft phy is via mdiobus_get_phy
 *   / the pPhy argument already held by the caller (r8169 soft-complete).
 * Int / void returns: soft success 0 (ethtool EOPNOTSUPP soft as 0).
 */

/* ---- phy connect / lifecycle ------------------------------------------ */
int   phy_connect_direct(void *pNetdev, void *pPhy, void *pfnHandler, int nIface);
void  phy_disconnect(void *pPhy);
void  phy_start(void *pPhy);
void  phy_stop(void *pPhy);
int   phy_init_hw(void *pPhy);
int   phy_resume(void *pPhy);
void  phy_attached_info(void *pPhy);
void  phy_print_status(void *pPhy);
int   phy_set_max_speed(void *pPhy, int nSpeed);
int   genphy_soft_reset(void *pPhy);

/* ---- MDIO bus --------------------------------------------------------- */
int   mdiobus_read(void *pBus, int nAddr, u32 u32Regnum);
int   mdiobus_write(void *pBus, int nAddr, u32 u32Regnum, u16 u16Val);
int   __mdiobus_write(void *pBus, int nAddr, u32 u32Regnum, u16 u16Val);
void *mdiobus_get_phy(void *pBus, int nAddr);
void *devm_mdiobus_alloc_size(void *pDev, int nSizeofPriv);
int   __devm_mdiobus_register(void *pDev, void *pBus, void *pOwner);

/* ---- phy register / page helpers -------------------------------------- */
int   phy_read_paged(void *pPhy, int nPage, u32 u32Regnum);
int   phy_modify(void *pPhy, u32 u32Regnum, u16 u16Mask, u16 u16Set);
int   phy_modify_paged(void *pPhy, int nPage, u32 u32Regnum, u16 u16Mask,
                       u16 u16Set);
int   __phy_modify(void *pPhy, u32 u32Regnum, u16 u16Mask, u16 u16Set);
int   phy_select_page(void *pPhy, int nPage);
int   phy_restore_page(void *pPhy, int nOldPage, int nRet);

/* ---- pause / eee / speed / ioctl -------------------------------------- */
void  phy_mac_interrupt(void *pPhy);
void  phy_get_pause(void *pPhy, u8 *pu8Tx, u8 *pu8Rx);
void  phy_set_asym_pause(void *pPhy, int nRx, int nTx);
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

/** Soft diagnostics. */
u32   linux_phy_soft_ksym_ok(void);
u32   linux_phy_soft_ksym_skip(void);

#ifdef __cplusplus
}
#endif
