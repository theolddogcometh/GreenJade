/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * L2 backend mux: virtio-net (QEMU) preferred, else rtl8168 (G752 Realtek).
 * Pure C11 freestanding. Dual MIT OR Apache-2.0.
 *
 * Soft Linux netdev coexistence (lamp only; Soft≠product):
 *   When linux_netdev_soft_count()≥1 and primary exists, net_l2 may note
 *   that a soft Linux-shaped netdev is present. TX/RX stay on freestanding
 *   backend (virtio|rtl8168) until a real soft datapath probe is ready.
 *   Greppable: net_l2: soft linux netdev note n=N (Soft≠product)
 */
#include <gj/klog.h>
#include <gj/linux_netdev_soft.h>
#include <gj/net_l2.h>
#include <gj/rtl8168.h>
#include <gj/string.h>
#include <gj/types.h>
#include <gj/virtio_net.h>

/* QEMU user-net shaped (SLIRP) */
static const u8 g_aVirtIp[4] = { 10, 0, 2, 15 };
static const u8 g_aVirtMac[6] = { 0x52, 0x54, 0x00, 0x12, 0x34, 0x56 };

/*
 * Lab static IPv4 when Realtek is up (G752 on operator LAN).
 * DUT sits on 10.200.125.0/24 — pick a free host; not a DHCP client (honest).
 * Override by editing this constant and rebuilding if .50 is taken.
 */
static const u8 g_aRtlIp[4] = { 10, 200, 125, 50 };

static u32 g_u32Backend = GJ_NET_L2_NONE;
static u8 g_aIp[4];
static u8 g_aMac[6];
/** Once-ish soft coexistence lamp (does not switch TX/RX). */
static int g_fSoftLinuxNoted;

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
        return;
    }

    (void)rtl8168_probe();
    if (rtl8168_ready() != 0) {
        g_u32Backend = GJ_NET_L2_RTL8168;
        memcpy(g_aIp, g_aRtlIp, 4);
        rtl8168_mac(g_aMac);
        kprintf("net_l2: backend=rtl8168 ip=%u.%u.%u.%u mac=%02x:%02x:%02x:"
                "%02x:%02x:%02x link=%u\n",
                g_aIp[0], g_aIp[1], g_aIp[2], g_aIp[3], g_aMac[0], g_aMac[1],
                g_aMac[2], g_aMac[3], g_aMac[4], g_aMac[5],
                rtl8168_link_up() ? 1u : 0u);
        kprintf("net_l2: lab static IP — set host route/ARP; ssh root@%u.%u."
                "%u.%u (when link+sshd up)\n",
                g_aIp[0], g_aIp[1], g_aIp[2], g_aIp[3]);
        return;
    }

    kprintf("net_l2: backend=none (no virtio-net, no rtl8168)\n");
}

u32
net_l2_backend(void)
{
    return g_u32Backend;
}

int
net_l2_ready(void)
{
    if (g_u32Backend == GJ_NET_L2_VIRTIO) {
        return virtio_net_ready();
    }
    if (g_u32Backend == GJ_NET_L2_RTL8168) {
        return rtl8168_ready();
    }
    return 0;
}

int
net_l2_tx(const void *pFrame, u32 cbLen)
{
    if (g_u32Backend == GJ_NET_L2_VIRTIO) {
        return virtio_net_tx(pFrame, cbLen);
    }
    if (g_u32Backend == GJ_NET_L2_RTL8168) {
        return rtl8168_tx(pFrame, cbLen);
    }
    return -1;
}

i32
net_l2_rx(void *pOut, u32 cbMax)
{
    if (g_u32Backend == GJ_NET_L2_VIRTIO) {
        return virtio_net_rx(pOut, cbMax);
    }
    if (g_u32Backend == GJ_NET_L2_RTL8168) {
        return rtl8168_rx(pOut, cbMax);
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

const char *
net_l2_name(void)
{
    if (g_u32Backend == GJ_NET_L2_VIRTIO) {
        return "virtio";
    }
    if (g_u32Backend == GJ_NET_L2_RTL8168) {
        return "rtl8168";
    }
    return "none";
}

/*
 * Soft Linux netdev note — coexistence lamp only.
 * Call after r8169 / linux_netdev_soft register path (main soft module wave).
 * Does not change g_u32Backend or net_l2_tx/rx routing (freestanding stays).
 * Grep: net_l2: soft linux netdev note n=N (Soft≠product)
 */
void
net_l2_soft_linux_note(void)
{
    int nSoft;
    void *pPrimary;

    if (g_fSoftLinuxNoted != 0) {
        return;
    }

    /* Soft surface may not be init yet — treat as absent (no lamp). */
    if (linux_netdev_soft_ready() == 0) {
        return;
    }

    nSoft = linux_netdev_soft_count();
    pPrimary = linux_netdev_soft_primary();
    if (nSoft < 1 || pPrimary == NULL) {
        return;
    }

    g_fSoftLinuxNoted = 1;
    /* Grep: net_l2: soft linux netdev note n=N (Soft≠product) */
    kprintf("net_l2: soft linux netdev note n=%d (Soft≠product)\n", nSoft);
    kprintf("net_l2: soft linux present backend=%s tx=freestanding "
            "(Soft≠product; no TX switch yet)\n",
            net_l2_name());
}
