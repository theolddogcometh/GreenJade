/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Tiny L2/L3 on virtio-net (pure C, dual-licensed): ARP reply, ICMP echo,
 * UDP echo (port 7), IPv4 TCP demux → net_tcp_input + multi-seg rtx tick.
 *
 * Soft deepen: multi-frame RX drain per poll, VLAN skip, IHL-aware ICMP,
 * frame/drop/tcp demux counters, greppable soft eth inventory log.
 *
 * Greppable soft inventory (prefix-stable):
 *   net: eth soft …
 *   net_eth: ARP/UDP/ICMP-echo helpers
 */
#include <gj/klog.h>
#include <gj/net_eth.h>
#include <gj/net_tcp.h>
#include <gj/string.h>
#include <gj/virtio_net.h>

/* Guest "address" for QEMU user net (10.0.2.15 is typical DHCP guest) */
static const u8 g_aOurMac[6] = { 0x52, 0x54, 0x00, 0x12, 0x34, 0x56 };
static const u8 g_aOurIp[4] = { 10, 0, 2, 15 };

/* Max frames drained per net_eth_poll (soft batch). */
#define NET_ETH_POLL_MAX 8

/* Protocol soft counters (lifetime; door STATS + inventory). */
static u32 g_u32ArpReplies;
static u32 g_u32UdpEchoes;
static u32 g_u32IcmpEchoes;
static u32 g_u32FramesRx;
static u32 g_u32FramesDrop;
static u32 g_u32TcpDemux;
static u32 g_u32VlanSkip;

/* Soft inventory deepen: poll/batch + demux-ok + link soft state. */
static u32 g_u32FramesOk;      /* handle_frame recognized ethertype path */
static u32 g_u32Polls;         /* net_eth_poll entries */
static u32 g_u32PollsNoDev;    /* polls with virtio-net not ready */
static u32 g_u32PollsDrain;    /* polls that drained ≥1 frame */
static u32 g_u32LastBatch;     /* frames drained on last drain poll */
static u32 g_u32BatchMax;      /* peak frames in one poll drain */
static u32 g_u32LinkReady;     /* soft shadow of virtio_net_ready() */
static u32 g_u32LinkChanges;   /* soft link ready 0↔1 transitions */
static u32 g_u32SoftLogN;      /* inventory log emissions (cap spam) */

static u16
htons(u16 u16V)
{
    return (u16)((u16V << 8) | (u16V >> 8));
}

static u16
ip_checksum(const void *p, u32 cb)
{
    const u16 *pW = (const u16 *)p;
    u32 u32Sum = 0;

    while (cb > 1) {
        u32Sum += *pW++;
        cb -= 2;
    }
    if (cb) {
        u32Sum += *(const u8 *)pW;
    }
    while (u32Sum >> 16) {
        u32Sum = (u32Sum & 0xffffu) + (u32Sum >> 16);
    }
    return (u16)~u32Sum;
}

/**
 * Soft eth inventory log — greppable product markers only (never hard-gates).
 *
 *   net: eth soft PASS|UP|NODEV|PARTIAL frames_rx=… drop=… ok=… vlan=…
 *        tcp=… arp=… udp=… icmp=… poll_max=…
 *   net: eth soft link ready=… polls=… nodev=… drain=… last_batch=…
 *        batch_max=… link_ch=… ip=… mac=…
 */
static void
net_eth_soft_log(void)
{
    const char *szVerdict;
    u32 u32Ready;
    u32 u32Proto;

    u32Ready = virtio_net_ready() ? 1u : 0u;
    u32Proto = g_u32ArpReplies + g_u32UdpEchoes + g_u32IcmpEchoes +
               g_u32TcpDemux;

    /*
     * Verdict (soft product inventory; door paths unchanged):
     *   NODEV   — virtio-net not ready (interim / no device)
     *   UP      — link soft ready, no frames yet
     *   PASS    — ready + demux-ok or protocol soft activity
     *   PARTIAL — ready with only drops / vlan skips (no demux-ok/proto)
     */
    if (u32Ready == 0u) {
        szVerdict = "NODEV";
    } else if (g_u32FramesOk != 0u || u32Proto != 0u) {
        szVerdict = "PASS";
    } else if (g_u32FramesDrop != 0u || g_u32VlanSkip != 0u) {
        szVerdict = "PARTIAL";
    } else {
        szVerdict = "UP";
    }

    if (g_u32SoftLogN < 0xffffffffu) {
        g_u32SoftLogN++;
    }

    /* Grep: net: eth soft */
    kprintf("net: eth soft %s frames_rx=%u drop=%u ok=%u vlan=%u tcp=%u "
            "arp=%u udp=%u icmp=%u poll_max=%u\n",
            szVerdict, g_u32FramesRx, g_u32FramesDrop, g_u32FramesOk,
            g_u32VlanSkip, g_u32TcpDemux, g_u32ArpReplies, g_u32UdpEchoes,
            g_u32IcmpEchoes, (u32)NET_ETH_POLL_MAX);
    kprintf("net: eth soft link ready=%u polls=%u nodev=%u drain=%u "
            "last_batch=%u batch_max=%u link_ch=%u log_n=%u "
            "ip=%u.%u.%u.%u mac=%02x:%02x:%02x:%02x:%02x:%02x\n",
            u32Ready, g_u32Polls, g_u32PollsNoDev, g_u32PollsDrain,
            g_u32LastBatch, g_u32BatchMax, g_u32LinkChanges, g_u32SoftLogN,
            g_aOurIp[0], g_aOurIp[1], g_aOurIp[2], g_aOurIp[3],
            g_aOurMac[0], g_aOurMac[1], g_aOurMac[2], g_aOurMac[3],
            g_aOurMac[4], g_aOurMac[5]);
}

void
net_eth_init(void)
{
    g_u32ArpReplies = 0;
    g_u32UdpEchoes = 0;
    g_u32IcmpEchoes = 0;
    g_u32FramesRx = 0;
    g_u32FramesDrop = 0;
    g_u32TcpDemux = 0;
    g_u32VlanSkip = 0;
    g_u32FramesOk = 0;
    g_u32Polls = 0;
    g_u32PollsNoDev = 0;
    g_u32PollsDrain = 0;
    g_u32LastBatch = 0;
    g_u32BatchMax = 0;
    g_u32LinkReady = 0;
    g_u32LinkChanges = 0;
    g_u32SoftLogN = 0;
    /* Soft shadow of device ready at init (probe may run later). */
    g_u32LinkReady = virtio_net_ready() ? 1u : 0u;
    kprintf("net_eth: ARP/UDP/ICMP-echo helpers (IP %u.%u.%u.%u) poll_max=%u\n",
            g_aOurIp[0], g_aOurIp[1], g_aOurIp[2], g_aOurIp[3],
            NET_ETH_POLL_MAX);
    /* Greppable soft inventory at init (NODEV typical before virtio probe). */
    net_eth_soft_log();
}

static void
handle_icmp(const u8 *pFrame, u32 cb)
{
    /* eth(14) + ip(ihl) + icmp(8+) — IHL-aware soft path */
    const u8 *pIp;
    const u8 *pIcmp;
    u8 aOut[128];
    u16 u16Tot;
    u16 u16Ihl;
    u32 cbIcmp;
    u32 i;

    if (cb < 42) {
        return;
    }
    pIp = pFrame + 14;
    if ((pIp[0] & 0xf0) != 0x40 || pIp[9] != 1) {
        return; /* IPv4 ICMP */
    }
    u16Ihl = (u16)((pIp[0] & 0x0f) * 4);
    if (u16Ihl < 20 || cb < 14u + u16Ihl + 8u) {
        return;
    }
    if (memcmp(pIp + 16, g_aOurIp, 4) != 0) {
        return;
    }
    pIcmp = pIp + u16Ihl;
    if (pIcmp[0] != 8) {
        return; /* echo request */
    }
    u16Tot = (u16)((pIp[2] << 8) | pIp[3]);
    if (u16Tot < (u16)(u16Ihl + 8) || (u32)(14 + u16Tot) > cb || u16Tot > 100) {
        return;
    }
    cbIcmp = (u32)u16Tot - (u32)u16Ihl;
    memset(aOut, 0, sizeof(aOut));
    memcpy(aOut, pFrame + 6, 6);
    memcpy(aOut + 6, g_aOurMac, 6);
    aOut[12] = 0x08;
    aOut[13] = 0x00;
    /* Reply with minimal 20-byte IP header (strip options soft). */
    memcpy(aOut + 14, pIp, 20);
    aOut[14] = 0x45;
    {
        u16 u16NewTot = (u16)(20 + cbIcmp);

        aOut[14 + 2] = (u8)(u16NewTot >> 8);
        aOut[14 + 3] = (u8)u16NewTot;
    }
    /* swap IP src/dst */
    memcpy(aOut + 14 + 12, pIp + 16, 4);
    memcpy(aOut + 14 + 16, pIp + 12, 4);
    aOut[14 + 10] = 0;
    aOut[14 + 11] = 0;
    {
        u16 u16C = ip_checksum(aOut + 14, 20);

        aOut[14 + 10] = (u8)(u16C >> 8);
        aOut[14 + 11] = (u8)(u16C & 0xff);
    }
    /* ICMP echo reply */
    aOut[14 + 20] = 0; /* type echo reply */
    aOut[14 + 21] = 0;
    aOut[14 + 22] = 0;
    aOut[14 + 23] = 0;
    for (i = 4; i < cbIcmp; i++) {
        aOut[14 + 20 + i] = pIcmp[i];
    }
    {
        u16 u16C = ip_checksum(aOut + 14 + 20, cbIcmp);

        aOut[14 + 22] = (u8)(u16C >> 8);
        aOut[14 + 23] = (u8)(u16C & 0xff);
    }
    if (virtio_net_tx(aOut, 14u + 20u + cbIcmp) == 0) {
        g_u32IcmpEchoes++;
    }
}

static void
handle_arp(const u8 *pFrame, u32 cb)
{
    /* eth(14) + arp(28) */
    u8 aOut[64];
    const u8 *pArp;

    if (cb < 42) {
        return;
    }
    pArp = pFrame + 14;
    /* op == request (1), tpa == us */
    if (pArp[6] != 0 || pArp[7] != 1) {
        return;
    }
    if (memcmp(pArp + 24, g_aOurIp, 4) != 0) {
        return;
    }
    memset(aOut, 0, sizeof(aOut));
    /* eth */
    memcpy(aOut, pFrame + 6, 6);       /* dst = sender mac */
    memcpy(aOut + 6, g_aOurMac, 6);
    aOut[12] = 0x08;
    aOut[13] = 0x06;
    /* arp reply */
    aOut[14] = 0;
    aOut[15] = 1; /* htype eth */
    aOut[16] = 0x08;
    aOut[17] = 0x00; /* ptype ip */
    aOut[18] = 6;
    aOut[19] = 4;
    aOut[20] = 0;
    aOut[21] = 2; /* reply */
    memcpy(aOut + 22, g_aOurMac, 6);
    memcpy(aOut + 28, g_aOurIp, 4);
    memcpy(aOut + 32, pArp + 8, 6);  /* tha = sender hw */
    memcpy(aOut + 38, pArp + 14, 4); /* tpa = sender ip */
    if (virtio_net_tx(aOut, 42) == 0) {
        g_u32ArpReplies++;
    }
}

static void
handle_udp(const u8 *pFrame, u32 cb)
{
    const u8 *pIp;
    const u8 *pUdp;
    u16 u16Tot;
    u16 u16Ihl;
    u16 u16Dport;
    u16 u16Sport;
    u16 u16Ulen;
    u32 cbPay;
    u8 aOut[1518];
    u8 *pOip;
    u8 *pOudp;
    u32 cbOut;

    if (cb < 14 + 20 + 8) {
        return;
    }
    pIp = pFrame + 14;
    if ((pIp[0] >> 4) != 4 || pIp[9] != 17) {
        return; /* not IPv4 UDP */
    }
    u16Ihl = (u16)((pIp[0] & 0x0f) * 4);
    if (cb < 14u + u16Ihl + 8u) {
        return;
    }
    pUdp = pIp + u16Ihl;
    u16Dport = (u16)((pUdp[2] << 8) | pUdp[3]);
    u16Sport = (u16)((pUdp[0] << 8) | pUdp[1]);
    u16Ulen = (u16)((pUdp[4] << 8) | pUdp[5]);
    if (u16Dport != 7) {
        return; /* echo */
    }
    if (u16Ulen < 8 || 14u + u16Ihl + u16Ulen > cb) {
        return;
    }
    cbPay = (u32)u16Ulen - 8u;
    /* Build reply */
    memset(aOut, 0, sizeof(aOut));
    memcpy(aOut, pFrame + 6, 6);
    memcpy(aOut + 6, g_aOurMac, 6);
    aOut[12] = 0x08;
    aOut[13] = 0x00;
    pOip = aOut + 14;
    pOip[0] = 0x45;
    u16Tot = (u16)(20 + 8 + cbPay);
    pOip[2] = (u8)(u16Tot >> 8);
    pOip[3] = (u8)u16Tot;
    pOip[8] = 64;
    pOip[9] = 17;
    memcpy(pOip + 12, g_aOurIp, 4);
    memcpy(pOip + 16, pIp + 12, 4);
    {
        u16 u16C = ip_checksum(pOip, 20);

        pOip[10] = (u8)(u16C >> 8);
        pOip[11] = (u8)u16C;
    }
    pOudp = pOip + 20;
    pOudp[0] = (u8)(u16Dport >> 8); /* swap ports */
    pOudp[1] = (u8)u16Dport;
    pOudp[2] = (u8)(u16Sport >> 8);
    pOudp[3] = (u8)u16Sport;
    pOudp[4] = (u8)(u16Ulen >> 8);
    pOudp[5] = (u8)u16Ulen;
    pOudp[6] = 0;
    pOudp[7] = 0;
    if (cbPay) {
        memcpy(pOudp + 8, pUdp + 8, cbPay);
    }
    cbOut = 14u + 20u + 8u + cbPay;
    if (virtio_net_tx(aOut, cbOut) == 0) {
        g_u32UdpEchoes++;
    }
    (void)htons;
}

/**
 * Soft demux one L2 frame: ARP / IPv4(ICMP|UDP|TCP) / VLAN skip.
 * Returns 1 if recognized path touched, 0 if drop/ignored.
 * Does not change door STATS packing — only deepens local soft inventory.
 */
static int
handle_frame(const u8 *pFrame, u32 cb)
{
    u16 u16Etype;

    if (pFrame == NULL || cb < 14) {
        g_u32FramesDrop++;
        return 0;
    }
    g_u32FramesRx++;
    u16Etype = (u16)((pFrame[12] << 8) | pFrame[13]);
    /* Soft VLAN: 802.1Q — count and ignore (no inner demux yet). */
    if (u16Etype == 0x8100u) {
        g_u32VlanSkip++;
        return 0;
    }
    if (u16Etype == 0x0806u) {
        handle_arp(pFrame, cb);
        g_u32FramesOk++;
        return 1;
    }
    if (u16Etype == 0x0800u) {
        const u8 *pIp = pFrame + 14;

        if (cb < 14 + 20) {
            g_u32FramesDrop++;
            return 0;
        }
        if ((pIp[0] & 0xf0) != 0x40) {
            g_u32FramesDrop++;
            return 0;
        }
        if (pIp[9] == 1) {
            handle_icmp(pFrame, cb);
            g_u32FramesOk++;
            return 1;
        }
        if (pIp[9] == 17) {
            handle_udp(pFrame, cb);
            g_u32FramesOk++;
            return 1;
        }
        if (pIp[9] == 6) {
            /* Ordered multi-seg data path for product TCP. */
            if (net_tcp_input(pFrame, cb)) {
                g_u32TcpDemux++;
            }
            g_u32FramesOk++;
            return 1;
        }
        g_u32FramesDrop++;
        return 0;
    }
    g_u32FramesDrop++;
    return 0;
}

/**
 * Soft link-state sample: refresh g_u32LinkReady from virtio_net_ready().
 * On 0↔1 transition, bump link_ch and emit greppable inventory (rate-capped).
 * Returns current ready (0/1). Never fails door / poll control flow.
 */
static u32
net_eth_soft_link_sample(void)
{
    u32 u32Now;

    u32Now = virtio_net_ready() ? 1u : 0u;
    if (u32Now != g_u32LinkReady) {
        g_u32LinkReady = u32Now;
        if (g_u32LinkChanges < 0xffffffffu) {
            g_u32LinkChanges++;
        }
        /* Cap inventory spam: init + first few link flips only. */
        if (g_u32SoftLogN < 8u) {
            net_eth_soft_log();
        }
    }
    return u32Now;
}

void
net_eth_poll(void)
{
    static u8 aRx[1518];
    i32 i32N;
    u32 u32Batch;
    u32 u32Ready;

    if (g_u32Polls < 0xffffffffu) {
        g_u32Polls++;
    }

    u32Ready = net_eth_soft_link_sample();
    if (u32Ready == 0u) {
        /*
         * No device: skip RX. Multi-seg rtx only applies to virtio peers;
         * loopback multi-seg advances SndUna inline in net_tcp_send.
         * Still tick TCP soft TIME_WAIT / rtx for any live sockets.
         */
        if (g_u32PollsNoDev < 0xffffffffu) {
            g_u32PollsNoDev++;
        }
        net_tcp_poll();
        return;
    }
    /* Soft multi-frame drain: up to NET_ETH_POLL_MAX frames per call. */
    for (u32Batch = 0; u32Batch < NET_ETH_POLL_MAX; u32Batch++) {
        i32N = virtio_net_rx(aRx, sizeof(aRx));
        if (i32N < 14 || (u32)i32N > sizeof(aRx)) {
            break;
        }
        (void)handle_frame(aRx, (u32)i32N);
    }
    if (u32Batch != 0u) {
        g_u32LastBatch = u32Batch;
        if (g_u32PollsDrain < 0xffffffffu) {
            g_u32PollsDrain++;
        }
        if (u32Batch > g_u32BatchMax) {
            g_u32BatchMax = u32Batch;
        }
        /*
         * First drain with traffic: one more greppable inventory line so
         * smokes see frames soft counters without needing a soft_log export.
         */
        if (g_u32PollsDrain == 1u && g_u32SoftLogN < 8u) {
            net_eth_soft_log();
        }
    }
    /* Always tick last-seg rtx + TIME_WAIT soft reap after poll attempt. */
    net_tcp_poll();
}

u32
net_eth_arp_replies(void)
{
    return g_u32ArpReplies;
}

u32
net_eth_udp_echoes(void)
{
    return g_u32UdpEchoes;
}

u32
net_eth_icmp_echoes(void)
{
    return g_u32IcmpEchoes;
}

u32
net_eth_frames_rx(void)
{
    return g_u32FramesRx;
}

u32
net_eth_frames_drop(void)
{
    return g_u32FramesDrop;
}

u32
net_eth_tcp_demux(void)
{
    return g_u32TcpDemux;
}

u32
net_eth_vlan_skip(void)
{
    return g_u32VlanSkip;
}
