/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Host-oracle offsets for RHEL-class Linux struct net_device / net_device_ops.
 * Used only for soft module-path experiments (r8169.ko REAL probe + future
 * Option B ndo_open layout readiness). Soft ≠ ABI-stable product.
 * Soft ≠ product AC (G-AC-1). Do not claim layout matches any other kver /
 * CONFIG / RH_KABI rebuild.
 *
 * Measured the same way as linux_pci_hostish_off.h (throwaway OOT module +
 * offsetof/sizeof on a lab host with kernel-devel). Numbers only — no Linux
 * GPL source is in this header.
 *
 * Measured: kver=5.14.0-687.15.1.el9_8.x86_64
 * Date (UTC): 2026-08-04
 * Companion: docs/R8169_KO_NDO_OPEN.md · docs/PCI_DEV_SOFT_LAYOUT.md Strategy A
 *
 * Soft vs host (critical):
 *   Soft net_device front  = 2048 B (LINUX_NETDEV_SOFT_ND_BYTES); soft ops @+0x18
 *   Host sizeof(net_device)= 0xaf0; host netdev_ops @+0x08; ndo_open @ ops+0x10
 *   Soft priv base         = +0x800 (after soft front)
 *   Host netdev_priv base  = after aligned sizeof(net_device) ≈ +0xaf0
 *   → Soft priv ≠ host priv. Dual object preferred: soft inventory stays on
 *     soft struct; hostish 0xaf0 blob captures .ko host-offset stores for
 *     Option B readiness (still no ndo_open call by default).
 */
#pragma once

/* ---- sizeof ----------------------------------------------------------- */
#define LINUX_NETDEV_HOSTISH_SIZE_NET_DEVICE   0x0af0u /* 2800 */
#define LINUX_NETDEV_HOSTISH_SIZE_NETDEV_OPS   0x03c0u /* 960 */

/* ---- struct net_device field offsets (RHEL 5.14 host oracle) ---------- */
#define LINUX_NETDEV_HOSTISH_OFF_NETDEV_OPS    0x0008u
#define LINUX_NETDEV_HOSTISH_OFF_MTU           0x0038u
#define LINUX_NETDEV_HOSTISH_OFF_FLAGS         0x00b0u
#define LINUX_NETDEV_HOSTISH_OFF_NAME          0x0120u
#define LINUX_NETDEV_HOSTISH_OFF_DEV_ADDR      0x03e0u
#define LINUX_NETDEV_HOSTISH_OFF_ML_PRIV       0x0550u
#define LINUX_NETDEV_HOSTISH_OFF_DEV           0x0578u /* embedded struct device */

/* ---- struct net_device_ops relative offsets --------------------------- */
#define LINUX_NETDEV_HOSTISH_OPS_OFF_NDO_INIT        0x0000u
#define LINUX_NETDEV_HOSTISH_OPS_OFF_NDO_UNINIT      0x0008u
#define LINUX_NETDEV_HOSTISH_OPS_OFF_NDO_OPEN        0x0010u
#define LINUX_NETDEV_HOSTISH_OPS_OFF_NDO_STOP        0x0018u
#define LINUX_NETDEV_HOSTISH_OPS_OFF_NDO_START_XMIT  0x0020u

/* Soft pool / blob sizing (mirror pci hostish: pad ≥ host sizeof). */
#define LINUX_NETDEV_HOSTISH_POOL           1u
#define LINUX_NETDEV_HOSTISH_BLOB_BYTES     0x0c00u /* 3072 ≥ 0xaf0; pad to 0xc00 */

/*
 * Soft bookkeeping front (clean-room) vs host — not interchangeable.
 * Soft netdev_ops field offset inside soft struct net_device:
 */
#define LINUX_NETDEV_SOFT_OFF_NETDEV_OPS    0x18u /* soft field; ≠ host 0x08 */

/*
 * Priv base conflict (document only — fill path uses dual object):
 *   Soft:  (u8 *)nd + LINUX_NETDEV_SOFT_ND_BYTES   (= +0x800)
 *   Host:  (u8 *)nd + aligned_sizeof(net_device)    (≈ +0xaf0)
 * Hostish blob path does NOT move soft priv; Option B must not assume soft
 * priv is host netdev_priv until a matching layout exists.
 */
#define LINUX_NETDEV_SOFT_PRIV_OFF          0x0800u
#define LINUX_NETDEV_HOSTISH_PRIV_OFF       LINUX_NETDEV_HOSTISH_SIZE_NET_DEVICE

/*
 * Usage (freestanding hostish fill — dual object after register_netdev):
 *
 *   u8 *pSoft = (u8 *)soft_primary;          // same VA .ko mutated
 *   u8 *pHost = hostish_blob;                // separate 0xc00 pool slot
 *   memset(pHost, 0, LINUX_NETDEV_HOSTISH_BLOB_BYTES);
 *   memcpy(pHost, pSoft, LINUX_NETDEV_HOSTISH_SIZE_NET_DEVICE);
 *   // .ko wrote netdev_ops at host +0x08 into soft slab → recovered here:
 *   void *ops = *(void **)(pHost + LINUX_NETDEV_HOSTISH_OFF_NETDEV_OPS);
 *   void *open = ops
 *       ? *(void **)((u8 *)ops + LINUX_NETDEV_HOSTISH_OPS_OFF_NDO_OPEN)
 *       : NULL;
 *
 * Gate real .ko ndo_open behind GJ_SOFT_R8169_KO_NDO_OPEN (default 0).
 * Soft≠product. Never call ndo_open from fill/diagnostic path.
 */
