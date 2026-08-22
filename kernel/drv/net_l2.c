/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * L2 backend mux: virtio-net (QEMU T0 product) when present; otherwise
 * none (product laptop NIC = userspace rtl8168_udx over hot+cold ABI).
 * In-kernel rtl8168 / linux_*_soft are abandoned (G-AC-1). Pure C11.
 * Dual MIT OR Apache-2.0.
 *
 * greppable: net_l2: backend=virtio
 * greppable: net_l2: backend=none (UDX)
 * greppable: net_l2: soft udx ready
 * greppable: net_l2: soft udx tx enqueue
 * greppable: LAB_MAC_UDX=02:00:00:47:4a:50
 */
#include <gj/fb_console.h>
#include <gj/klog.h>
#include <gj/net_door.h>
#include <gj/net_l2.h>
#include <gj/string.h>
#include <gj/types.h>
#include <gj/virtio_net.h>

/* QEMU user-net shaped (SLIRP) */
static const u8 g_aVirtIp[4] = { 10, 0, 2, 15 };
static const u8 g_aVirtMac[6] = { 0x52, 0x54, 0x00, 0x12, 0x34, 0x56 };

/*
 * Lab static IPv4 when product UDX owns wire (G752 on operator LAN).
 * DUT sits on 10.200.125.0/24.
 */
static const u8 g_aRtlIp[4] = { 10, 200, 125, 50 };

/*
 * Soft demux lab MAC residual (backend=none + ETH_UDX_READY).
 * LAB_MAC_UDX=02:00:00:47:4a:50 — same 6 bytes as rtl8168_udx product
 * IDR lab_fallback. Soft!=product Dual DoD B OPEN.
 */
static const u8 g_aLabMacUdx[6] = { 0x02, 0x00, 0x00, 0x47, 0x4a, 0x50 };

static u32 g_u32Backend = GJ_NET_L2_NONE;
static u8 g_aIp[4];
static u8 g_aMac[6];

/* Soft MMIO handoff flags (net_eth fail-closed residual). */
static int g_fSoftHandoffPending;
static int g_fSoftHandoffFault;
static int g_fSoftHandoffFaultLogged;

static const char *net_l2_name_local(void);

static void
net_l2_status_ip_pin(void)
{
    char sz[40];
    char *q;
    u32 i;
    u32 v;

    q = sz;
    *q++ = 'I';
    *q++ = 'P';
    *q++ = ' ';
    for (i = 0; i < 4u; i++) {
        v = g_aIp[i];
        if (v >= 100u) {
            *q++ = (char)('0' + (v / 100u) % 10u);
        }
        if (v >= 10u) {
            *q++ = (char)('0' + (v / 10u) % 10u);
        }
        *q++ = (char)('0' + (v % 10u));
        if (i < 3u) {
            *q++ = '.';
        }
    }
    *q++ = ' ';
    *q++ = ':';
    *q++ = '2';
    *q++ = '2';
    *q = '\0';
    {
        static char s_szLast[40];
        u32 n;

        n = 0u;
        while (sz[n] != '\0' && n < 39u) {
            if (s_szLast[n] != sz[n]) {
                break;
            }
            n++;
        }
        if (sz[n] == '\0' && s_szLast[n] == '\0') {
            return;
        }
        n = 0u;
        while (sz[n] != '\0' && n < 39u) {
            s_szLast[n] = sz[n];
            n++;
        }
        s_szLast[n] = '\0';
    }
    fb_console_hold(7, sz);
}

void
net_l2_init(void)
{
    g_u32Backend = GJ_NET_L2_NONE;
    memset(g_aIp, 0, 4);
    memset(g_aMac, 0, 6);

    if (virtio_net_ready() != 0) {
        g_u32Backend = GJ_NET_L2_VIRTIO;
        memcpy(g_aIp, g_aVirtIp, 4);
        memcpy(g_aMac, g_aVirtMac, 6);
        kprintf("net_l2: backend=virtio ip=%u.%u.%u.%u (QEMU SLIRP)\n",
                g_aIp[0], g_aIp[1], g_aIp[2], g_aIp[3]);
        net_l2_status_ip_pin();
        return;
    }

    /* Lab DUT identity is known before ETH_UDX_READY (0.1.143 hold7 0.0.0.0). */
    memcpy(g_aIp, g_aRtlIp, 4);
    kprintf("net_l2: backend=none (UDX)\n");
    net_l2_status_ip_pin();
}

u32
net_l2_backend(void)
{
    return g_u32Backend;
}

void
net_l2_refresh_mac(void)
{
    /* Abandoned freestanding rtl station refresh. UDX publishes via
     * net_l2_set_station_mac. */
}

void
net_l2_udx_ready_identity(void)
{
    u32 i;
    int fMacZero;

    if (g_u32Backend != GJ_NET_L2_NONE) {
        return;
    }
    if (net_door_udx_ready() == 0) {
        return;
    }
    memcpy(g_aIp, g_aRtlIp, 4);
    fMacZero = 1;
    for (i = 0; i < 6u; i++) {
        if (g_aMac[i] != 0u) {
            fMacZero = 0;
            break;
        }
    }
    if (fMacZero != 0) {
        memcpy(g_aMac, g_aLabMacUdx, 6);
    }
    {
        static u8 s_fUdxIdLamp;

        if (s_fUdxIdLamp == 0u) {
            s_fUdxIdLamp = 1u;
            kprintf("net_l2: soft udx ready backend=none "
                    "owner=product_udx_abi path=rtl8168_udx "
                    "lab_ip=10.200.125.50 "
                    "mac=%02x:%02x:%02x:%02x:%02x:%02x "
                    "ip=%u.%u.%u.%u dual_dod_b=OPEN_UDX "
                    "(identity pin; prefer set_station_mac after IDR keep)\n",
                    g_aMac[0], g_aMac[1], g_aMac[2], g_aMac[3], g_aMac[4],
                    g_aMac[5], g_aIp[0], g_aIp[1], g_aIp[2], g_aIp[3]);
        }
    }
    net_l2_status_ip_pin();
}

int
net_l2_ready(void)
{
    if (g_fSoftHandoffPending != 0 || g_fSoftHandoffFault != 0) {
        return 0;
    }
    if (g_u32Backend == GJ_NET_L2_VIRTIO) {
        return virtio_net_ready();
    }
    if (g_u32Backend == GJ_NET_L2_NONE && net_door_udx_ready() != 0) {
        /* Identity is pinned at ETH_UDX_READY / set_station_mac — not
         * every ready() sample (hold7 IP was flashing on GOP). */
        return 1;
    }
    return 0;
}

int
net_l2_tx(const void *pFrame, u32 cbLen)
{
    if (g_fSoftHandoffPending != 0 || g_fSoftHandoffFault != 0) {
        return -1;
    }
    if (pFrame == NULL || cbLen < 14u) {
        return -1;
    }

    if (g_u32Backend == GJ_NET_L2_NONE && net_door_udx_ready() != 0) {
        if (net_door_udx_tx_soft(pFrame, cbLen) == 0) {
            static u8 s_fUdxTxLamp;

            if (s_fUdxTxLamp == 0u) {
                s_fUdxTxLamp = 1u;
                kprintf("net_l2: soft udx tx enqueue first len=%u "
                        "path=rtl8168_udx dual_dod_b=OPEN_UDX\n",
                        (unsigned)cbLen);
            }
            return 0;
        }
        return -1;
    }

    if (g_u32Backend == GJ_NET_L2_VIRTIO) {
        return virtio_net_tx(pFrame, cbLen);
    }

    return -1;
}

i32
net_l2_rx(void *pOut, u32 cbMax)
{
    if (g_fSoftHandoffPending != 0 || g_fSoftHandoffFault != 0) {
        return -1;
    }
    if (pOut == NULL || cbMax < 14u) {
        return -1;
    }

    if (g_u32Backend == GJ_NET_L2_VIRTIO) {
        return virtio_net_rx(pOut, cbMax);
    }

    return -1;
}

void
net_l2_mac(u8 *pMac)
{
    u32 i;

    if (pMac == NULL) {
        return;
    }
    for (i = 0; i < 6u; i++) {
        pMac[i] = g_aMac[i];
    }
}

void
net_l2_set_station_mac(const u8 *pMac6)
{
    u32 i;
    u32 fZero;

    if (pMac6 == NULL) {
        return;
    }
    fZero = 1u;
    for (i = 0; i < 6u; i++) {
        if (pMac6[i] != 0u) {
            fZero = 0u;
            break;
        }
    }
    if (fZero != 0u) {
        return;
    }
    for (i = 0; i < 6u; i++) {
        g_aMac[i] = pMac6[i];
    }
    if (g_u32Backend == GJ_NET_L2_NONE) {
        memcpy(g_aIp, g_aRtlIp, 4);
    }
    {
        static u8 s_fStationMacLamp;

        if (s_fStationMacLamp == 0u) {
            s_fStationMacLamp = 1u;
            kprintf("net_l2: soft station mac "
                    "mac=%02x:%02x:%02x:%02x:%02x:%02x "
                    "lab_ip=%u.%u.%u.%u backend=%u "
                    "path=rtl8168_udx dual_dod_b=OPEN "
                    "(product IDR station published for ARP SHA)\n",
                    g_aMac[0], g_aMac[1], g_aMac[2], g_aMac[3], g_aMac[4],
                    g_aMac[5], g_aIp[0], g_aIp[1], g_aIp[2], g_aIp[3],
                    (unsigned)g_u32Backend);
        }
    }
    net_l2_status_ip_pin();
}

void
net_l2_ip(u8 *pIp)
{
    u32 i;

    if (pIp == NULL) {
        return;
    }
    for (i = 0; i < 4u; i++) {
        pIp[i] = g_aIp[i];
    }
}

static const char *
net_l2_name_local(void)
{
    if (g_u32Backend == GJ_NET_L2_VIRTIO) {
        return "virtio";
    }
    if (g_u32Backend == GJ_NET_L2_RTL8168) {
        return "rtl8168";
    }
    if (net_door_udx_ready() != 0) {
        return "udx";
    }
    return "none";
}

const char *
net_l2_name(void)
{
    return net_l2_name_local();
}

void
net_l2_soft_linux_note(void)
{
    /* Abandoned linux_netdev_soft coexistence lamp. Product = UDX. */
}

void
net_l2_soft_handoff_mark_pending(void)
{
    if (g_fSoftHandoffFault != 0) {
        return;
    }
    g_fSoftHandoffPending = 1;
    kprintf("net_l2: soft mmio handoff pending backend was=%s "
            "tx=fail-closed\n",
            net_l2_name());
}

int
net_l2_soft_handoff_pending(void)
{
    return g_fSoftHandoffPending != 0 ? 1 : 0;
}

int
net_l2_soft_handoff_fault(void)
{
    return g_fSoftHandoffFault != 0 ? 1 : 0;
}

void
net_l2_soft_handoff_set_fault(const char *szWhy)
{
    g_fSoftHandoffFault = 1;
    g_fSoftHandoffPending = 1;
    if (g_fSoftHandoffFaultLogged == 0) {
        g_fSoftHandoffFaultLogged = 1;
        kprintf("net_l2: soft mmio handoff FAULT why=%s (fail closed)\n",
                (szWhy != NULL) ? szWhy : "?");
    }
}
