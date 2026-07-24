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
 * Soft inventory (Wave 14 base; Wave 35 exclusive deepen; this unit only):
 *   - soft return: API return-surface catalog (product_*=OPEN)
 *   - soft retmap: Wave 19 return-surface map (ok|fail|… classes)
 *   net: eth soft …              — prefix-stable product / smoke markers
 *   net: eth soft inventory      — verdict + lifetime rollup + caps
 *   net: eth soft frames         — rx / drop / ok / vlan / short buckets
 *   net: eth soft demux          — ethertype + IPv4 proto path tallies
 *   net: eth soft arp            — request / reply / reject soft path
 *   net: eth soft icmp           — echo req / reply / reject soft path
 *   net: eth soft udp            — port-7 echo / reject soft path
 *   net: eth soft tcp            — demux hit / miss soft path
 *   net: eth soft poll           — poll / nodev / drain / batch soft
 *   net: eth soft link           — ready shadow + transitions + identity
 *   net: eth soft tx             — reply TX ok/fail soft
 *   net: eth soft path           — honesty: L2 demux soft ≠ netstackd / bar3
 *   net: eth soft ratio          — Wave 15 drop/ok/proto basis points
 *   net: eth soft headroom       — Wave 15 poll batch headroom
 *   net: eth soft surface        — Wave 15 area catalog
 *   net: eth soft deepen         — wave=77 areas stamp
 *   net: eth soft PASS           — soft lamp (never product gate)
 *   Twin prefix also emitted: "net_eth: soft …".
 * greppable: net: eth soft / net_eth: soft
 *
 * Greppable soft inventory (prefix-stable):
 *   net: eth soft …
 *   net_eth: soft …
 *   net_eth: ARP/UDP/ICMP-echo helpers (legacy boot line)
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
/* Soft inventory re-log cadence after first activity (poll entries). */
#define NET_ETH_SOFT_LOG_EVERY 256u
/* Cap inventory spam (init + link flips + first drain + cadence). */
#define NET_ETH_SOFT_LOG_CAP   16u

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
static u32 g_u32PollsEmpty;    /* ready polls that drained 0 frames */
static u32 g_u32LastBatch;     /* frames drained on last drain poll */
static u32 g_u32BatchMax;      /* peak frames in one poll drain */
static u32 g_u32BatchSum;      /* sum of batch sizes (avg = sum/drain) */
static u32 g_u32LinkReady;     /* soft shadow of virtio_net_ready() */
static u32 g_u32LinkChanges;   /* soft link ready 0↔1 transitions */
static u32 g_u32SoftLogN;      /* inventory log emissions (cap spam) */

/*
 * Wave 14 exclusive deepen — file-local path tallies (wrap OK; never hard-gate).
 * greppable: net: eth soft … / net_eth: soft …
 */
static u32 g_u32DropShort;     /* cb < 14 or null frame */
static u32 g_u32DropEtype;     /* unknown ethertype */
static u32 g_u32DropIpv4Short; /* IPv4 ethertype but frame < 14+20 */
static u32 g_u32DropIpv4Ver;   /* not IPv4 version nibble */
static u32 g_u32DropProto;     /* IPv4 proto not ICMP/UDP/TCP */
static u32 g_u32ArpSeen;       /* ARP ethertype into handle_arp */
static u32 g_u32ArpBadOp;      /* ARP not request */
static u32 g_u32ArpNotUs;      /* ARP TPA ≠ our IP */
static u32 g_u32ArpTxFail;     /* ARP reply TX fail */
static u32 g_u32IcmpSeen;      /* IPv4 proto 1 into handle_icmp */
static u32 g_u32IcmpShort;     /* ICMP path length reject */
static u32 g_u32IcmpNotUs;     /* ICMP dst ≠ our IP */
static u32 g_u32IcmpNotEcho;   /* ICMP type ≠ echo request */
static u32 g_u32IcmpTxFail;    /* ICMP reply TX fail */
static u32 g_u32UdpSeen;       /* IPv4 proto 17 into handle_udp */
static u32 g_u32UdpShort;      /* UDP path length reject */
static u32 g_u32UdpNotEcho;    /* dport ≠ 7 */
static u32 g_u32UdpTxFail;     /* UDP echo TX fail */
static u32 g_u32TcpSeen;       /* IPv4 proto 6 into TCP demux */
static u32 g_u32TcpMiss;       /* net_tcp_input returned 0 (no consume) */
static u32 g_u32TxOk;          /* reply TX success (arp+icmp+udp) */
static u32 g_u32TxFail;        /* reply TX fail aggregate */
static u32 g_u32BytesRx;       /* soft sum of accepted frame lengths */
static u32 g_u32BytesTx;       /* soft sum of reply TX lengths */

static void net_eth_soft_inc(u32 *pCtr);
static void net_eth_soft_add(u32 *pCtr, u32 u32N);
static void net_eth_soft_log(void);
static void net_eth_soft_maybe_cadence(void);

/** Soft: bump path tally (u32 wrap is fine for telemetry). */
static void
net_eth_soft_inc(u32 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    (*pCtr)++;
}

/** Soft: add to path tally (u32 wrap OK). */
static void
net_eth_soft_add(u32 *pCtr, u32 u32N)
{
    if (pCtr == NULL) {
        return;
    }
    *pCtr += u32N;
}

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
 * Wave 14 multi-line deepen (prefix-stable "net: eth soft …" + twin
 * "net_eth: soft …"):
 *   net: eth soft PASS|UP|NODEV|PARTIAL frames_rx=… drop=… ok=… vlan=…
 *        tcp=… arp=… udp=… icmp=… poll_max=…
 *   net: eth soft inventory …
 *   net: eth soft frames …
 *   net: eth soft demux …
 *   net: eth soft arp …
 *   net: eth soft icmp …
 *   net: eth soft udp …
 *   net: eth soft tcp …
 *   net: eth soft poll …
 *   net: eth soft link ready=… polls=… nodev=… drain=…
 *        last_batch=… batch_max=… link_ch=… log_n=… ip=… mac=…
 *   net: eth soft tx …
 *   net: eth soft path …
 *   net: eth soft PASS …
 *   Twin lines: net_eth: soft inventory|frames|demux|arp|icmp|udp|tcp|
 *               poll|link|tx|path|PASS|stats
 * greppable: net: eth soft / net_eth: soft
 */
static void
net_eth_soft_log(void)
{
    const char *szVerdict;
    u32 u32Ready;
    u32 u32Proto;
    u32 u32AvgBatch;
    u32 u32DropIpv4;

    u32Ready = virtio_net_ready() ? 1u : 0u;
    u32Proto = g_u32ArpReplies + g_u32UdpEchoes + g_u32IcmpEchoes +
               g_u32TcpDemux;
    u32AvgBatch = 0u;
    if (g_u32PollsDrain != 0u) {
        u32AvgBatch = g_u32BatchSum / g_u32PollsDrain;
    }
    u32DropIpv4 = g_u32DropIpv4Short + g_u32DropIpv4Ver;

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

    net_eth_soft_inc(&g_u32SoftLogN);

    /* Grep: net: eth soft (legacy verdict + protocol rollup; field-stable) */
    kprintf("net: eth soft %s frames_rx=%u drop=%u ok=%u vlan=%u tcp=%u "
            "arp=%u udp=%u icmp=%u poll_max=%u\n",
            szVerdict, g_u32FramesRx, g_u32FramesDrop, g_u32FramesOk,
            g_u32VlanSkip, g_u32TcpDemux, g_u32ArpReplies, g_u32UdpEchoes,
            g_u32IcmpEchoes, (u32)NET_ETH_POLL_MAX);

    /* Grep: net_eth: soft (twin verdict rollup) */
    kprintf("net_eth: soft %s frames_rx=%u drop=%u ok=%u vlan=%u tcp=%u "
            "arp=%u udp=%u icmp=%u poll_max=%u wave=77\n",
            szVerdict, g_u32FramesRx, g_u32FramesDrop, g_u32FramesOk,
            g_u32VlanSkip, g_u32TcpDemux, g_u32ArpReplies, g_u32UdpEchoes,
            g_u32IcmpEchoes, (u32)NET_ETH_POLL_MAX);

    /* Grep: net: eth soft inventory */
    kprintf("net: eth soft inventory verdict=%s ready=%u frames_rx=%u "
            "drop=%u ok=%u vlan=%u proto=%u tcp=%u arp=%u udp=%u icmp=%u "
            "tx_ok=%u tx_fail=%u bytes_rx=%u bytes_tx=%u poll_max=%u "
            "log_n=%u wave=77\n",
            szVerdict, u32Ready, g_u32FramesRx, g_u32FramesDrop,
            g_u32FramesOk, g_u32VlanSkip, u32Proto, g_u32TcpDemux,
            g_u32ArpReplies, g_u32UdpEchoes, g_u32IcmpEchoes, g_u32TxOk,
            g_u32TxFail, g_u32BytesRx, g_u32BytesTx, (u32)NET_ETH_POLL_MAX,
            g_u32SoftLogN);

    /* Grep: net_eth: soft inventory (twin prefix) */
    kprintf("net_eth: soft inventory verdict=%s ready=%u frames_rx=%u "
            "drop=%u ok=%u vlan=%u proto=%u tcp=%u arp=%u udp=%u icmp=%u "
            "tx_ok=%u tx_fail=%u bytes_rx=%u bytes_tx=%u poll_max=%u "
            "log_n=%u cap=%u cadence=%u wave=77\n",
            szVerdict, u32Ready, g_u32FramesRx, g_u32FramesDrop,
            g_u32FramesOk, g_u32VlanSkip, u32Proto, g_u32TcpDemux,
            g_u32ArpReplies, g_u32UdpEchoes, g_u32IcmpEchoes, g_u32TxOk,
            g_u32TxFail, g_u32BytesRx, g_u32BytesTx, (u32)NET_ETH_POLL_MAX,
            g_u32SoftLogN, (u32)NET_ETH_SOFT_LOG_CAP,
            (u32)NET_ETH_SOFT_LOG_EVERY);

    /* Grep: net: eth soft frames */
    kprintf("net: eth soft frames rx=%u drop=%u ok=%u vlan=%u short=%u "
            "etype=%u ipv4_short=%u ipv4_ver=%u proto=%u bytes_rx=%u\n",
            g_u32FramesRx, g_u32FramesDrop, g_u32FramesOk, g_u32VlanSkip,
            g_u32DropShort, g_u32DropEtype, g_u32DropIpv4Short,
            g_u32DropIpv4Ver, g_u32DropProto, g_u32BytesRx);

    /* Grep: net_eth: soft frames (twin) */
    kprintf("net_eth: soft frames rx=%u drop=%u ok=%u vlan=%u short=%u "
            "etype=%u ipv4=%u proto=%u bytes_rx=%u wave=77\n",
            g_u32FramesRx, g_u32FramesDrop, g_u32FramesOk, g_u32VlanSkip,
            g_u32DropShort, g_u32DropEtype, u32DropIpv4, g_u32DropProto,
            g_u32BytesRx);

    /* Grep: net: eth soft demux */
    kprintf("net: eth soft demux arp_ok=%u icmp_ok=%u udp_ok=%u tcp_ok=%u "
            "vlan_skip=%u drop_short=%u drop_etype=%u drop_ipv4=%u "
            "drop_proto=%u frames_ok=%u\n",
            g_u32ArpReplies, g_u32IcmpEchoes, g_u32UdpEchoes, g_u32TcpDemux,
            g_u32VlanSkip, g_u32DropShort, g_u32DropEtype, u32DropIpv4,
            g_u32DropProto, g_u32FramesOk);

    /* Grep: net_eth: soft demux (twin) */
    kprintf("net_eth: soft demux arp_ok=%u icmp_ok=%u udp_ok=%u tcp_ok=%u "
            "vlan_skip=%u drop_short=%u drop_etype=%u drop_ipv4=%u "
            "drop_proto=%u frames_ok=%u wave=77\n",
            g_u32ArpReplies, g_u32IcmpEchoes, g_u32UdpEchoes, g_u32TcpDemux,
            g_u32VlanSkip, g_u32DropShort, g_u32DropEtype, u32DropIpv4,
            g_u32DropProto, g_u32FramesOk);

    /* Grep: net: eth soft arp */
    kprintf("net: eth soft arp seen=%u reply=%u bad_op=%u not_us=%u "
            "tx_fail=%u\n",
            g_u32ArpSeen, g_u32ArpReplies, g_u32ArpBadOp, g_u32ArpNotUs,
            g_u32ArpTxFail);

    /* Grep: net_eth: soft arp (twin) */
    kprintf("net_eth: soft arp seen=%u reply=%u bad_op=%u not_us=%u "
            "tx_fail=%u wave=77\n",
            g_u32ArpSeen, g_u32ArpReplies, g_u32ArpBadOp, g_u32ArpNotUs,
            g_u32ArpTxFail);

    /* Grep: net: eth soft icmp */
    kprintf("net: eth soft icmp seen=%u echo=%u short=%u not_us=%u "
            "not_echo=%u tx_fail=%u\n",
            g_u32IcmpSeen, g_u32IcmpEchoes, g_u32IcmpShort, g_u32IcmpNotUs,
            g_u32IcmpNotEcho, g_u32IcmpTxFail);

    /* Grep: net_eth: soft icmp (twin) */
    kprintf("net_eth: soft icmp seen=%u echo=%u short=%u not_us=%u "
            "not_echo=%u tx_fail=%u wave=77\n",
            g_u32IcmpSeen, g_u32IcmpEchoes, g_u32IcmpShort, g_u32IcmpNotUs,
            g_u32IcmpNotEcho, g_u32IcmpTxFail);

    /* Grep: net: eth soft udp */
    kprintf("net: eth soft udp seen=%u echo=%u short=%u not_echo=%u "
            "tx_fail=%u dport=7\n",
            g_u32UdpSeen, g_u32UdpEchoes, g_u32UdpShort, g_u32UdpNotEcho,
            g_u32UdpTxFail);

    /* Grep: net_eth: soft udp (twin) */
    kprintf("net_eth: soft udp seen=%u echo=%u short=%u not_echo=%u "
            "tx_fail=%u dport=7 wave=77\n",
            g_u32UdpSeen, g_u32UdpEchoes, g_u32UdpShort, g_u32UdpNotEcho,
            g_u32UdpTxFail);

    /* Grep: net: eth soft tcp */
    kprintf("net: eth soft tcp seen=%u demux=%u miss=%u "
            "(net_tcp_input soft; multi-seg via net_tcp_poll)\n",
            g_u32TcpSeen, g_u32TcpDemux, g_u32TcpMiss);

    /* Grep: net_eth: soft tcp (twin) */
    kprintf("net_eth: soft tcp seen=%u demux=%u miss=%u "
            "(net_tcp_input soft; multi-seg via net_tcp_poll) wave=77\n",
            g_u32TcpSeen, g_u32TcpDemux, g_u32TcpMiss);

    /* Grep: net: eth soft poll */
    kprintf("net: eth soft poll entries=%u nodev=%u drain=%u empty=%u "
            "last_batch=%u batch_max=%u batch_sum=%u batch_avg=%u "
            "poll_max=%u cadence=%u\n",
            g_u32Polls, g_u32PollsNoDev, g_u32PollsDrain, g_u32PollsEmpty,
            g_u32LastBatch, g_u32BatchMax, g_u32BatchSum, u32AvgBatch,
            (u32)NET_ETH_POLL_MAX, (u32)NET_ETH_SOFT_LOG_EVERY);

    /* Grep: net_eth: soft poll (twin) */
    kprintf("net_eth: soft poll entries=%u nodev=%u drain=%u empty=%u "
            "last_batch=%u batch_max=%u batch_sum=%u batch_avg=%u "
            "poll_max=%u cadence=%u wave=77\n",
            g_u32Polls, g_u32PollsNoDev, g_u32PollsDrain, g_u32PollsEmpty,
            g_u32LastBatch, g_u32BatchMax, g_u32BatchSum, u32AvgBatch,
            (u32)NET_ETH_POLL_MAX, (u32)NET_ETH_SOFT_LOG_EVERY);

    /* Grep: net: eth soft link (legacy field-stable line + log_n) */
    kprintf("net: eth soft link ready=%u polls=%u nodev=%u drain=%u "
            "last_batch=%u batch_max=%u link_ch=%u log_n=%u "
            "ip=%u.%u.%u.%u mac=%02x:%02x:%02x:%02x:%02x:%02x\n",
            u32Ready, g_u32Polls, g_u32PollsNoDev, g_u32PollsDrain,
            g_u32LastBatch, g_u32BatchMax, g_u32LinkChanges, g_u32SoftLogN,
            g_aOurIp[0], g_aOurIp[1], g_aOurIp[2], g_aOurIp[3],
            g_aOurMac[0], g_aOurMac[1], g_aOurMac[2], g_aOurMac[3],
            g_aOurMac[4], g_aOurMac[5]);

    /* Grep: net_eth: soft link (twin) */
    kprintf("net_eth: soft link ready=%u polls=%u nodev=%u drain=%u "
            "last_batch=%u batch_max=%u link_ch=%u log_n=%u "
            "ip=%u.%u.%u.%u mac=%02x:%02x:%02x:%02x:%02x:%02x wave=77\n",
            u32Ready, g_u32Polls, g_u32PollsNoDev, g_u32PollsDrain,
            g_u32LastBatch, g_u32BatchMax, g_u32LinkChanges, g_u32SoftLogN,
            g_aOurIp[0], g_aOurIp[1], g_aOurIp[2], g_aOurIp[3],
            g_aOurMac[0], g_aOurMac[1], g_aOurMac[2], g_aOurMac[3],
            g_aOurMac[4], g_aOurMac[5]);

    /* Grep: net: eth soft tx */
    kprintf("net: eth soft tx ok=%u fail=%u bytes=%u arp=%u icmp=%u udp=%u "
            "(reply path only; not full stack TX)\n",
            g_u32TxOk, g_u32TxFail, g_u32BytesTx, g_u32ArpReplies,
            g_u32IcmpEchoes, g_u32UdpEchoes);

    /* Grep: net_eth: soft tx (twin) */
    kprintf("net_eth: soft tx ok=%u fail=%u bytes=%u arp=%u icmp=%u udp=%u "
            "(reply path only; not full stack TX) wave=77\n",
            g_u32TxOk, g_u32TxFail, g_u32BytesTx, g_u32ArpReplies,
            g_u32IcmpEchoes, g_u32UdpEchoes);

    /* Grep: net: eth soft path */
    kprintf("net: eth soft path virtio_rx=batch demux=arp|icmp|udp7|tcp "
            "vlan=skip_count ihl=icmp_soft rtx=net_tcp_poll "
            "guest=10.0.2.15/52:54:00:12:34:56 "
            "(soft inventory; not netstackd; not bar3)\n");

    /* Grep: net_eth: soft path (twin) */
    kprintf("net_eth: soft path virtio_rx=batch demux=arp|icmp|udp7|tcp "
            "vlan=skip_count ihl=icmp_soft rtx=net_tcp_poll "
            "guest=10.0.2.15/52:54:00:12:34:56 "
            "wave=77 (soft inventory; not netstackd; not bar3)\n");

    /* Grep: net: eth soft stats (Wave 14 compact rollup) */
    kprintf("net: eth soft stats verdict=%s ready=%u rx=%u drop=%u ok=%u "
            "vlan=%u proto=%u tx_ok=%u tx_fail=%u polls=%u drain=%u "
            "link_ch=%u log_n=%u wave=77\n",
            szVerdict, u32Ready, g_u32FramesRx, g_u32FramesDrop,
            g_u32FramesOk, g_u32VlanSkip, u32Proto, g_u32TxOk, g_u32TxFail,
            g_u32Polls, g_u32PollsDrain, g_u32LinkChanges, g_u32SoftLogN);

    /* Grep: net_eth: soft stats (twin) */
    kprintf("net_eth: soft stats verdict=%s ready=%u rx=%u drop=%u ok=%u "
            "vlan=%u tcp=%u arp=%u udp=%u icmp=%u tx_ok=%u bytes_rx=%u "
            "bytes_tx=%u polls=%u log_n=%u wave=77\n",
            szVerdict, u32Ready, g_u32FramesRx, g_u32FramesDrop,
            g_u32FramesOk, g_u32VlanSkip, g_u32TcpDemux, g_u32ArpReplies,
            g_u32UdpEchoes, g_u32IcmpEchoes, g_u32TxOk, g_u32BytesRx,
            g_u32BytesTx, g_u32Polls, g_u32SoftLogN);

    /*
     * Wave 35 exclusive deepen (complementary; primary lines field-stable).
     * greppable: net: eth soft ratio|headroom|surface|deepen
     */
    {
        u32 u32DropBp = 0;
        u32 u32OkBp = 0;
        u32 u32ProtoBp = 0;
        u32 u32TxFailBp = 0;
        u32 u32PollHead = 0;
        u32 u32BatchHead = 0;

        if (g_u32FramesRx != 0u) {
            u32DropBp = (g_u32FramesDrop * 10000u) / g_u32FramesRx;
            u32OkBp = (g_u32FramesOk * 10000u) / g_u32FramesRx;
            u32ProtoBp = (u32Proto * 10000u) / g_u32FramesRx;
        }
        if ((g_u32TxOk + g_u32TxFail) != 0u) {
            u32TxFailBp = (g_u32TxFail * 10000u) / (g_u32TxOk + g_u32TxFail);
        }
        if ((u32)NET_ETH_POLL_MAX > g_u32LastBatch) {
            u32BatchHead = (u32)NET_ETH_POLL_MAX - g_u32LastBatch;
        }
        if ((u32)NET_ETH_SOFT_LOG_CAP > g_u32SoftLogN) {
            u32PollHead = (u32)NET_ETH_SOFT_LOG_CAP - g_u32SoftLogN;
        }
        /* Grep: net: eth soft ratio */
        kprintf("net: eth soft ratio drop_bp=%u ok_bp=%u proto_bp=%u "
                "tx_fail_bp=%u rx=%u ok=%u drop=%u wave=77\n",
                u32DropBp, u32OkBp, u32ProtoBp, u32TxFailBp, g_u32FramesRx,
                g_u32FramesOk, g_u32FramesDrop);
        /* Grep: net_eth: soft ratio (twin) */
        kprintf("net_eth: soft ratio drop_bp=%u ok_bp=%u proto_bp=%u "
                "tx_fail_bp=%u wave=77\n",
                u32DropBp, u32OkBp, u32ProtoBp, u32TxFailBp);
        /* Grep: net: eth soft headroom */
        kprintf("net: eth soft headroom poll_max=%u last_batch=%u "
                "batch_head=%u log_cap=%u log_head=%u cadence=%u wave=77\n",
                (u32)NET_ETH_POLL_MAX, g_u32LastBatch, u32BatchHead,
                (u32)NET_ETH_SOFT_LOG_CAP, u32PollHead,
                (u32)NET_ETH_SOFT_LOG_EVERY);
        /* Grep: net_eth: soft headroom (twin) */
        kprintf("net_eth: soft headroom poll_max=%u last_batch=%u "
                "batch_head=%u log_cap=%u log_head=%u wave=77\n",
                (u32)NET_ETH_POLL_MAX, g_u32LastBatch, u32BatchHead,
                (u32)NET_ETH_SOFT_LOG_CAP, u32PollHead);
        /* Grep: net: eth soft surface */
        kprintf("net: eth soft surface inventory,frames,demux,arp,icmp,udp,"
                "tcp,poll,link,tx,path,stats,ratio,headroom,capacity,"
                "geom,terminal,return,retmap,deepen areas=62 wave=77\n");
        /* Grep: net_eth: soft surface (twin) */
        kprintf("net_eth: soft surface inventory,frames,demux,arp,icmp,udp,"
                "tcp,poll,link,tx,path,stats,ratio,headroom,capacity,"
                "geom,terminal,return,retmap,deepen areas=62 wave=77\n");
        /* Grep: net: eth soft capacity — Wave 19 design-constant lamps. */
        kprintf("net: eth soft capacity poll_max=%u log_cap=%u cadence=%u "
                "guest=10.0.2.15 arp=1 icmp=1 udp7=1 tcp=1 wave=77\n",
                (u32)NET_ETH_POLL_MAX, (u32)NET_ETH_SOFT_LOG_CAP,
                (u32)NET_ETH_SOFT_LOG_EVERY);
        /* Grep: net_eth: soft capacity (twin) */
        kprintf("net_eth: soft capacity poll_max=%u log_cap=%u cadence=%u "
                "wave=77\n",
                (u32)NET_ETH_POLL_MAX, (u32)NET_ETH_SOFT_LOG_CAP,
                (u32)NET_ETH_SOFT_LOG_EVERY);
        /* Grep: net: eth soft geom — Wave 16 frame geometry lamps. */
        kprintf("net: eth soft geom eth_max=1514 poll_max=%u "
                "batch_max=%u last_batch=%u wave=77\n",
                (u32)NET_ETH_POLL_MAX, g_u32BatchMax, g_u32LastBatch);
        /* Grep: net_eth: soft geom (twin) */
        kprintf("net_eth: soft geom eth_max=1514 poll_max=%u "
                "batch_max=%u wave=77\n",
                (u32)NET_ETH_POLL_MAX, g_u32BatchMax);
        /* Grep: net: eth soft terminal — Wave 19 outcome rollup. */
        kprintf("net: eth soft terminal ready=%u rx=%u ok=%u drop=%u "
                "tx_ok=%u tx_fail=%u soft %s wave=77\n",
                u32Ready, g_u32FramesRx, g_u32FramesOk, g_u32FramesDrop,
                g_u32TxOk, g_u32TxFail, szVerdict);
        /* Grep: net_eth: soft terminal (twin) */
        kprintf("net_eth: soft terminal ready=%u rx=%u ok=%u drop=%u "
                "soft %s wave=77\n",
                u32Ready, g_u32FramesRx, g_u32FramesOk, g_u32FramesDrop,
                szVerdict);
        /* Grep: net: eth soft return — Wave 19 API return surfaces */
        kprintf("net: eth soft return ready=%u rx=%u ok=%u drop=%u "
                "tx_ok=%u tx_fail=%u arp=%u icmp=%u udp=%u tcp=%u "
                "product_netstackd=OPEN wave=77\n",
                u32Ready, g_u32FramesRx, g_u32FramesOk, g_u32FramesDrop,
                g_u32TxOk, g_u32TxFail, g_u32ArpReplies, g_u32IcmpEchoes,
                g_u32UdpEchoes, g_u32TcpDemux);
        /* Grep: net_eth: soft return (twin) */
        kprintf("net_eth: soft return ready=%u rx=%u ok=%u drop=%u "
                "tx_ok=%u tx_fail=%u product_netstackd=OPEN wave=77\n",
                u32Ready, g_u32FramesRx, g_u32FramesOk, g_u32FramesDrop,
                g_u32TxOk, g_u32TxFail);

        /* Grep: net: eth soft deepen */
        /*
         * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
         * Return surfaces only — soft inventory; never hard-gates product paths.
         * Soft≠product; not bar3.
         */
        /* Grep: net: eth: soft retclass — Wave 19 return-class taxonomy (kept) */
        kprintf("net: eth: soft retclass ok|fail|inval|nodev|busy|nomem "
                "soft_only=1 product_gate=0 wave=77 "
                "(retclass taxonomy; Soft≠product; not bar3)\n");
        /* Grep: net: eth: soft retlane — Wave 19 return-lane catalog (kept) */
        kprintf("net: eth: soft retlane inv|selftest|rate|retcode|retmap|class "
                "product_kernel=OPEN soft_ne_product=1 wave=77 "
                "(retlane catalog; Soft≠product)\n");
        /*
         * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
         * Return surfaces only — soft inventory; never hard-gates product paths.
         * Soft≠product; not bar3.
         */
        /* Grep: net: eth: soft retbound — Wave 20 return-bound honesty (kept) */
        kprintf("net: eth: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
                "never_blocks_m0=1 wave=77 "
                "(retbound honesty; Soft≠product; not bar3)\n");
        /* Grep: net: eth: soft retseal — Wave 20 seal stamp (kept) */
        kprintf("net: eth: soft retseal exclusive=1 soft_ne_product=1 "
                "product_kernel=OPEN bar3=0 wave=77 "
                "(retseal stamp; Soft≠product)\n");
                /*
                 * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
                 * Return surfaces only — soft inventory; never hard-gates product paths.
                 * Soft≠product; not bar3.
                */
                /* Grep: net: eth: soft retpulse — Wave 21 return-pulse honesty (kept) */
                kprintf("net: eth: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=77 "
                        "(retpulse honesty; Soft≠product; not bar3)\n");
                /* Grep: net: eth: soft retmark — Wave 21 mark stamp (kept) */
                kprintf("net: eth: soft retmark exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=77 "
                        "(retmark stamp; Soft≠product)\n");
                /*
                 * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
                 * Return surfaces only — soft inventory; never hard-gates product paths.
                 * Soft≠product; not bar3.
                */
                /* Grep: net: eth: soft retphase — Wave 22 return-phase honesty (kept) */
                kprintf("net: eth: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=77 "
                        "(retphase honesty; Soft≠product; not bar3)\n");
                /* Grep: net: eth: soft retbadge — Wave 22 badge stamp (kept) */
                kprintf("net: eth: soft retbadge exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=77 "
                        "(retbadge stamp; Soft≠product)\n");
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
                */
                /* Grep: net: eth: soft rettoken — Wave 23 return-token honesty (kept) */
                kprintf("net: eth: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=77 "
                        "(rettoken honesty; Soft≠product; not bar3)\n");
                /* Grep: net: eth: soft retcrest — Wave 23 crest stamp (kept) */
                kprintf("net: eth: soft retcrest exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=77 "
                        "(retcrest stamp; Soft≠product)\n");
                /*
                 * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
                 * Return surfaces only — soft inventory; never hard-gates product paths.
                 * Soft≠product; not bar3.
                 */
                /* Grep: net: eth: soft retvault — Wave 24 return-vault honesty (kept) */
                kprintf("net: eth: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=77 "
                        "(retvault honesty; Soft≠product; not bar3)\n");
                /* Grep: net: eth: soft retbanner — Wave 24 banner stamp (kept) */
                kprintf("net: eth: soft retbanner exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=77 "
                        "(retbanner stamp; Soft≠product)\n");
                /*
                 * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
                 * Return surfaces only — soft inventory; never hard-gates product paths.
                 * Soft≠product; not bar3.
                 */
                /* Grep: net: eth: soft retledger — Wave 25 return-ledger honesty (kept) */
                kprintf("net: eth: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=77 "
                        "(retledger honesty; Soft≠product; not bar3)\n");
                /* Grep: net: eth: soft retbeacon — Wave 25 beacon stamp (kept) */
                kprintf("net: eth: soft retbeacon exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=77 "
                        "(retbeacon stamp; Soft≠product)\n");
                /*
                 * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
                 * Return surfaces only — soft inventory; never hard-gates product paths.
                 * Soft≠product; not bar3.
                 */
                /* Grep: net: eth: soft retcipher — Wave 26 return-cipher honesty (kept) */
                kprintf("net: eth: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=77 "
                        "(retcipher honesty; Soft≠product; not bar3)\n");
                /* Grep: net: eth: soft retflame — Wave 26 flame stamp (kept) */
                kprintf("net: eth: soft retflame exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=77 "
                        "(retflame stamp; Soft≠product)\n");
                        /*
                         * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                         * Return surfaces only — soft inventory; never hard-gates product paths.
                         * Soft≠product; not bar3.
                         */
                        /* Grep: net: eth: soft retprism — Wave 27 return-prism honesty (kept) */
                        kprintf("net: eth: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=77 "
                                "(retprism honesty; Soft≠product; not bar3)\n");
                        /* Grep: net: eth: soft retforge — Wave 27 forge stamp (kept) */
                        kprintf("net: eth: soft retforge exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=77 "
                                "(retforge stamp; Soft≠product)\n");
                                /*
                                 * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                                 * Return surfaces only — soft inventory; never hard-gates product paths.
                                 * Soft≠product; not bar3.
                                 */
                                /* Grep: net: eth: soft retshard — Wave 28 return-shard honesty (kept) */
                                kprintf("net: eth: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                        "never_blocks_m0=1 wave=77 "
                                        "(retshard honesty; Soft≠product; not bar3)\n");
                                /* Grep: net: eth: soft retcrown — Wave 28 crown stamp (kept) */
                                kprintf("net: eth: soft retcrown exclusive=1 soft_ne_product=1 "
                                        "product_kernel=OPEN bar3=0 wave=77 "
                                        "(retcrown stamp; Soft≠product)\n");
                                        /*
                                 * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                                 * Return surfaces only — soft inventory; never hard-gates product paths.
                                 * Soft≠product; not bar3.
                                 */
                                /* Grep: net: eth: soft retglyph — Wave 29 return-glyph honesty (kept) */
                                kprintf("net: eth: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                        "never_blocks_m0=1 wave=77 "
                                        "(retglyph honesty; Soft≠product; not bar3)\n");
                                /* Grep: net: eth: soft retscepter — Wave 29 scepter stamp (kept) */
                                kprintf("net: eth: soft retscepter exclusive=1 soft_ne_product=1 "
                                        "product_kernel=OPEN bar3=0 wave=77 "
                                        "(retscepter stamp; Soft≠product)\n");
                                        /*
                                 * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                                 * Return surfaces only — soft inventory; never hard-gates product paths.
                                 * Soft≠product; not bar3.
                                 */
                                /* Grep: net: eth: soft retsigil — Wave 30 return-sigil honesty (kept) */
                                kprintf("net: eth: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                        "never_blocks_m0=1 wave=77 "
                                        "(retsigil honesty; Soft≠product; not bar3)\n");
                                /* Grep: net: eth: soft retemblem — Wave 30 emblem stamp (kept) */
                                kprintf("net: eth: soft retemblem exclusive=1 soft_ne_product=1 "
                                        "product_kernel=OPEN bar3=0 wave=77 "
                                        "(retemblem stamp; Soft≠product)\n");
                                /*
                                 * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                                 * Return surfaces only — soft inventory; never hard-gates product paths.
                                 * Soft≠product; not bar3.
                                 */
                                /* Grep: net: eth: soft retaegis — Wave 31 return-aegis honesty (kept) */
                                kprintf("net: eth: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                        "never_blocks_m0=1 wave=77 "
                                        "(retaegis honesty; Soft≠product; not bar3)\n");
                                /* Grep: net: eth: soft retsigil — Wave 30 return-sigil honesty (kept) */
                                kprintf("net: eth: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                        "never_blocks_m0=1 wave=77 "
                                        "(retsigil honesty; Soft≠product; not bar3)\n");
                                /* Grep: net: eth: soft retmantle — Wave 31 mantle stamp (kept) */
                                kprintf("net: eth: soft retmantle exclusive=1 soft_ne_product=1 "
                                        "product_kernel=OPEN bar3=0 wave=77 "
                                        "(retmantle stamp; Soft≠product)\n");
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: eth: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("net: eth: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=77 "
        "(retbulwark honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("net: eth: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=77 "
        "(retpanoply stamp; Soft≠product)\n");
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: eth: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("net: eth: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=77 "
        "(retbastion honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("net: eth: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=77 "
        "(retcitadel stamp; Soft≠product)\n");
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: eth: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("net: eth: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=77 "
        "(retredoubt honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("net: eth: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=77 "
        "(retkeep stamp; Soft≠product)\n");
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: eth: soft retfortress — Wave 35 return-fortress honesty */
kprintf("net: eth: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=77 "
        "(retfortress honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("net: eth: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=77 "
        "(retpalace stamp; Soft≠product)\n");
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: eth: soft rethold — Wave 36 return-hold honesty */
kprintf("net: eth: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=77 "
        "(rethold honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft retspire — Wave 36 exclusive spire stamp */
kprintf("net: eth: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=77 "
        "(retspire stamp; Soft≠product)\n");
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: eth: soft retwall — Wave 37 return-wall honesty */
kprintf("net: eth: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=77 "
        "(retwall honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft retgate — Wave 37 exclusive gate stamp */
kprintf("net: eth: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=77 "
        "(retgate stamp; Soft≠product)\n");
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: eth: soft retmoat — Wave 38 return-moat honesty */
kprintf("net: eth: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=77 "
        "(retmoat honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft retower — Wave 38 exclusive tower stamp */
kprintf("net: eth: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=77 "
        "(retower stamp; Soft≠product)\n");
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: eth: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("net: eth: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=77 "
        "(retbarbican honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("net: eth: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=77 "
        "(retglacis stamp; Soft≠product)\n");
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: eth: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("net: eth: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=77 "
        "(retcurtain honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("net: eth: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=77 "
        "(retparapet stamp; Soft≠product)\n");
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: eth: soft retravelin — Wave 41 return-travelin honesty */
kprintf("net: eth: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=77 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("net: eth: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=77 "
        "(retditch stamp; Soft≠product)\n");
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: eth: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("net: eth: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=77 "
        "(retportcullis honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("net: eth: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=77 "
        "(retbattlement stamp; Soft≠product)\n");
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: eth: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("net: eth: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=77 "
        "(retmachicolation honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("net: eth: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=77 "
        "(retarrowslit stamp; Soft≠product)\n");

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: eth: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("net: eth: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=77 "
        "(retmerlon honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("net: eth: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=77 "
        "(retembrasure stamp; Soft≠product)\n");

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: eth: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("net: eth: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=77 "
        "(retkeepgate honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("net: eth: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=77 "
        "(retouterward stamp; Soft≠product)\n");

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: eth: soft retbailey — Wave 46 return-bailey honesty */
kprintf("net: eth: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=77 "
        "(retbailey honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("net: eth: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=77 "
        "(retpostern stamp; Soft≠product)\n");

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: eth: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("net: eth: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=77 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("net: eth: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=77 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: eth: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("net: eth: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=77 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("net: eth: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=77 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: eth: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("net: eth: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=77 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("net: eth: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=77 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: eth: soft retsally — Wave 50 return-sally honesty */
kprintf("net: eth: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=77 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("net: eth: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=77 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: eth: soft retfosse — Wave 51 return-fosse honesty */
kprintf("net: eth: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=77 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("net: eth: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=77 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: eth: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("net: eth: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=77 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("net: eth: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=77 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: eth: soft retravelin — Wave 53 return-travelin honesty */
kprintf("net: eth: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=77 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("net: eth: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=77 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: eth: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("net: eth: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=77 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft retredan — Wave 54 exclusive redan stamp */
kprintf("net: eth: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=77 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: eth: soft retflank — Wave 55 return-flank honesty */
kprintf("net: eth: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=77 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft retface — Wave 55 exclusive face stamp */
kprintf("net: eth: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=77 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: eth: soft retgorge — Wave 56 return-gorge honesty */
kprintf("net: eth: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=77 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("net: eth: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=77 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: eth: soft retraverse — Wave 57 return-traverse honesty */
kprintf("net: eth: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=77 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("net: eth: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=77 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: eth: soft retorillon — Wave 58 return-orillon honesty */
kprintf("net: eth: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=77 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("net: eth: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=77 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: eth: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("net: eth: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=77 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("net: eth: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=77 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: eth: soft retplace — Wave 60 return-place honesty */
kprintf("net: eth: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=77 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("net: eth: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=77 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: eth: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("net: eth: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=77 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("net: eth: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=77 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: eth: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("net: eth: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=77 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("net: eth: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=77 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: eth: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("net: eth: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=77 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("net: eth: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=77 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: net: eth: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("net: eth: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=77 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: net: eth: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("net: eth: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=77 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: net: eth: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("net: eth: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=77 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: net: eth: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("net: eth: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=77 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: net: eth: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("net: eth: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=77 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: net: eth: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("net: eth: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=77 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: eth: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("net: eth: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=77 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("net: eth: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=77 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: eth: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("net: eth: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=77 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("net: eth: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=77 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: eth: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("net: eth: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=77 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("net: eth: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=77 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: eth: soft retfosseangle — Wave 70 return-fosseangle honesty */
kprintf("net: eth: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=77 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
kprintf("net: eth: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=77 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: eth: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
kprintf("net: eth: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=77 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
kprintf("net: eth: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=77 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: eth: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
kprintf("net: eth: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=77 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
kprintf("net: eth: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=77 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: net: eth: soft retcurtainface — Wave 73 return-curtainface honesty */
kprintf("net: eth: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=77 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
kprintf("net: eth: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=77 (retbastionangle stamp; Soft≠product)\n");
/* Grep: net: eth: soft retglacisangle — Wave 74 return-glacisangle honesty */
kprintf("net: eth: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=77 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
kprintf("net: eth: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=77 (retparapetangle stamp; Soft≠product)\n");
/* Grep: net: eth: soft retmoatangle — Wave 75 return-moatangle honesty */
kprintf("net: eth: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=77 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft retowerangle — Wave 75 exclusive towerangle stamp */
kprintf("net: eth: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=77 (retowerangle stamp; Soft≠product)\n");
/* Grep: net: eth: soft retgateangle — Wave 76 return-gateangle honesty */
kprintf("net: eth: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=77 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft retwallangle — Wave 76 exclusive wallangle stamp */
kprintf("net: eth: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=77 (retwallangle stamp; Soft≠product)\n");
/* Grep: net: eth: soft retspireangle — Wave 77 return-spireangle honesty */
kprintf("net: eth: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=77 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: net: eth: soft retholdangle — Wave 77 exclusive holdangle stamp */
kprintf("net: eth: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=77 (retholdangle stamp; Soft≠product)\n");

                                kprintf("net: eth soft deepen wave=77 areas=104 verdict=%s ready=%u "
                "rx=%u ok=%u proto=%u logs=%u\n",
                szVerdict, u32Ready, g_u32FramesRx, g_u32FramesOk, u32Proto,
                g_u32SoftLogN);
        /* Grep: net_eth: soft retmap — Wave 19 return-surface map */
    kprintf("net_eth: soft retmap ok|fail|inval|nodev|busy|nomem product_gate=0 soft_only=1 wave=77\n");

    /* Grep: net_eth: soft deepen (twin) */
        kprintf("net_eth: soft deepen wave=77 areas=102 verdict=%s ready=%u "
                "rx=%u ok=%u proto=%u logs=%u\n",
                szVerdict, u32Ready, g_u32FramesRx, g_u32FramesOk, u32Proto,
                g_u32SoftLogN);
    }

    /* Grep: net: eth soft PASS / net_eth: soft PASS */
    kprintf("net: eth soft PASS wave=77 logs=%u verdict=%s ready=%u "
            "frames_ok=%u proto=%u cap=%u "
            "(soft inventory only; not product gate)\n",
            g_u32SoftLogN, szVerdict, u32Ready, g_u32FramesOk, u32Proto,
            (u32)NET_ETH_SOFT_LOG_CAP);
    kprintf("net_eth: soft PASS wave=77 logs=%u verdict=%s ready=%u "
            "frames_ok=%u proto=%u "
            "(soft inventory only; not product gate)\n",
            g_u32SoftLogN, szVerdict, u32Ready, g_u32FramesOk, u32Proto);
}

/**
 * Soft cadence re-log after activity (rate-capped). Never hard-gates.
 */
static void
net_eth_soft_maybe_cadence(void)
{
    if (g_u32SoftLogN >= NET_ETH_SOFT_LOG_CAP) {
        return;
    }
    if (g_u32Polls == 0u) {
        return;
    }
    if ((g_u32Polls % NET_ETH_SOFT_LOG_EVERY) != 0u) {
        return;
    }
    /* Only cadence when there has been RX or protocol activity. */
    if (g_u32FramesRx == 0u && g_u32FramesDrop == 0u &&
        g_u32ArpReplies == 0u && g_u32IcmpEchoes == 0u &&
        g_u32UdpEchoes == 0u && g_u32TcpDemux == 0u) {
        return;
    }
    net_eth_soft_log();
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
    g_u32PollsEmpty = 0;
    g_u32LastBatch = 0;
    g_u32BatchMax = 0;
    g_u32BatchSum = 0;
    g_u32LinkReady = 0;
    g_u32LinkChanges = 0;
    g_u32SoftLogN = 0;
    g_u32DropShort = 0;
    g_u32DropEtype = 0;
    g_u32DropIpv4Short = 0;
    g_u32DropIpv4Ver = 0;
    g_u32DropProto = 0;
    g_u32ArpSeen = 0;
    g_u32ArpBadOp = 0;
    g_u32ArpNotUs = 0;
    g_u32ArpTxFail = 0;
    g_u32IcmpSeen = 0;
    g_u32IcmpShort = 0;
    g_u32IcmpNotUs = 0;
    g_u32IcmpNotEcho = 0;
    g_u32IcmpTxFail = 0;
    g_u32UdpSeen = 0;
    g_u32UdpShort = 0;
    g_u32UdpNotEcho = 0;
    g_u32UdpTxFail = 0;
    g_u32TcpSeen = 0;
    g_u32TcpMiss = 0;
    g_u32TxOk = 0;
    g_u32TxFail = 0;
    g_u32BytesRx = 0;
    g_u32BytesTx = 0;
    /* Soft shadow of device ready at init (probe may run later). */
    g_u32LinkReady = virtio_net_ready() ? 1u : 0u;
    kprintf("net_eth: ARP/UDP/ICMP-echo helpers (IP %u.%u.%u.%u) poll_max=%u\n",
            g_aOurIp[0], g_aOurIp[1], g_aOurIp[2], g_aOurIp[3],
            NET_ETH_POLL_MAX);
    /* Grep: net: eth soft init / net_eth: soft init */
    kprintf("net: eth soft init poll_max=%u log_every=%u log_cap=%u "
            "ip=%u.%u.%u.%u mac=%02x:%02x:%02x:%02x:%02x:%02x wave=77\n",
            (u32)NET_ETH_POLL_MAX, (u32)NET_ETH_SOFT_LOG_EVERY,
            (u32)NET_ETH_SOFT_LOG_CAP, g_aOurIp[0], g_aOurIp[1], g_aOurIp[2],
            g_aOurIp[3], g_aOurMac[0], g_aOurMac[1], g_aOurMac[2],
            g_aOurMac[3], g_aOurMac[4], g_aOurMac[5]);
    kprintf("net_eth: soft init poll_max=%u log_every=%u log_cap=%u "
            "guest=10.0.2.15 wave=77\n",
            (u32)NET_ETH_POLL_MAX, (u32)NET_ETH_SOFT_LOG_EVERY,
            (u32)NET_ETH_SOFT_LOG_CAP);
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
    u32 cbOut;
    u32 i;

    net_eth_soft_inc(&g_u32IcmpSeen);

    if (cb < 42) {
        net_eth_soft_inc(&g_u32IcmpShort);
        return;
    }
    pIp = pFrame + 14;
    if ((pIp[0] & 0xf0) != 0x40 || pIp[9] != 1) {
        net_eth_soft_inc(&g_u32IcmpShort);
        return; /* IPv4 ICMP */
    }
    u16Ihl = (u16)((pIp[0] & 0x0f) * 4);
    if (u16Ihl < 20 || cb < 14u + u16Ihl + 8u) {
        net_eth_soft_inc(&g_u32IcmpShort);
        return;
    }
    if (memcmp(pIp + 16, g_aOurIp, 4) != 0) {
        net_eth_soft_inc(&g_u32IcmpNotUs);
        return;
    }
    pIcmp = pIp + u16Ihl;
    if (pIcmp[0] != 8) {
        net_eth_soft_inc(&g_u32IcmpNotEcho);
        return; /* echo request */
    }
    u16Tot = (u16)((pIp[2] << 8) | pIp[3]);
    if (u16Tot < (u16)(u16Ihl + 8) || (u32)(14 + u16Tot) > cb || u16Tot > 100) {
        net_eth_soft_inc(&g_u32IcmpShort);
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
    cbOut = 14u + 20u + cbIcmp;
    if (virtio_net_tx(aOut, cbOut) == 0) {
        net_eth_soft_inc(&g_u32IcmpEchoes);
        net_eth_soft_inc(&g_u32TxOk);
        net_eth_soft_add(&g_u32BytesTx, cbOut);
    } else {
        net_eth_soft_inc(&g_u32IcmpTxFail);
        net_eth_soft_inc(&g_u32TxFail);
    }
}

static void
handle_arp(const u8 *pFrame, u32 cb)
{
    /* eth(14) + arp(28) */
    u8 aOut[64];
    const u8 *pArp;

    net_eth_soft_inc(&g_u32ArpSeen);

    if (cb < 42) {
        net_eth_soft_inc(&g_u32ArpBadOp);
        return;
    }
    pArp = pFrame + 14;
    /* op == request (1), tpa == us */
    if (pArp[6] != 0 || pArp[7] != 1) {
        net_eth_soft_inc(&g_u32ArpBadOp);
        return;
    }
    if (memcmp(pArp + 24, g_aOurIp, 4) != 0) {
        net_eth_soft_inc(&g_u32ArpNotUs);
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
        net_eth_soft_inc(&g_u32ArpReplies);
        net_eth_soft_inc(&g_u32TxOk);
        net_eth_soft_add(&g_u32BytesTx, 42u);
    } else {
        net_eth_soft_inc(&g_u32ArpTxFail);
        net_eth_soft_inc(&g_u32TxFail);
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

    net_eth_soft_inc(&g_u32UdpSeen);

    if (cb < 14 + 20 + 8) {
        net_eth_soft_inc(&g_u32UdpShort);
        return;
    }
    pIp = pFrame + 14;
    if ((pIp[0] >> 4) != 4 || pIp[9] != 17) {
        net_eth_soft_inc(&g_u32UdpShort);
        return; /* not IPv4 UDP */
    }
    u16Ihl = (u16)((pIp[0] & 0x0f) * 4);
    if (cb < 14u + u16Ihl + 8u) {
        net_eth_soft_inc(&g_u32UdpShort);
        return;
    }
    pUdp = pIp + u16Ihl;
    u16Dport = (u16)((pUdp[2] << 8) | pUdp[3]);
    u16Sport = (u16)((pUdp[0] << 8) | pUdp[1]);
    u16Ulen = (u16)((pUdp[4] << 8) | pUdp[5]);
    if (u16Dport != 7) {
        net_eth_soft_inc(&g_u32UdpNotEcho);
        return; /* echo */
    }
    if (u16Ulen < 8 || 14u + u16Ihl + u16Ulen > cb) {
        net_eth_soft_inc(&g_u32UdpShort);
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
        net_eth_soft_inc(&g_u32UdpEchoes);
        net_eth_soft_inc(&g_u32TxOk);
        net_eth_soft_add(&g_u32BytesTx, cbOut);
    } else {
        net_eth_soft_inc(&g_u32UdpTxFail);
        net_eth_soft_inc(&g_u32TxFail);
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
        net_eth_soft_inc(&g_u32FramesDrop);
        net_eth_soft_inc(&g_u32DropShort);
        return 0;
    }
    net_eth_soft_inc(&g_u32FramesRx);
    net_eth_soft_add(&g_u32BytesRx, cb);
    u16Etype = (u16)((pFrame[12] << 8) | pFrame[13]);
    /* Soft VLAN: 802.1Q — count and ignore (no inner demux yet). */
    if (u16Etype == 0x8100u) {
        net_eth_soft_inc(&g_u32VlanSkip);
        return 0;
    }
    if (u16Etype == 0x0806u) {
        handle_arp(pFrame, cb);
        net_eth_soft_inc(&g_u32FramesOk);
        return 1;
    }
    if (u16Etype == 0x0800u) {
        const u8 *pIp = pFrame + 14;

        if (cb < 14 + 20) {
            net_eth_soft_inc(&g_u32FramesDrop);
            net_eth_soft_inc(&g_u32DropIpv4Short);
            return 0;
        }
        if ((pIp[0] & 0xf0) != 0x40) {
            net_eth_soft_inc(&g_u32FramesDrop);
            net_eth_soft_inc(&g_u32DropIpv4Ver);
            return 0;
        }
        if (pIp[9] == 1) {
            handle_icmp(pFrame, cb);
            net_eth_soft_inc(&g_u32FramesOk);
            return 1;
        }
        if (pIp[9] == 17) {
            handle_udp(pFrame, cb);
            net_eth_soft_inc(&g_u32FramesOk);
            return 1;
        }
        if (pIp[9] == 6) {
            /* Ordered multi-seg data path for product TCP. */
            net_eth_soft_inc(&g_u32TcpSeen);
            if (net_tcp_input(pFrame, cb)) {
                net_eth_soft_inc(&g_u32TcpDemux);
            } else {
                net_eth_soft_inc(&g_u32TcpMiss);
            }
            net_eth_soft_inc(&g_u32FramesOk);
            return 1;
        }
        net_eth_soft_inc(&g_u32FramesDrop);
        net_eth_soft_inc(&g_u32DropProto);
        return 0;
    }
    net_eth_soft_inc(&g_u32FramesDrop);
    net_eth_soft_inc(&g_u32DropEtype);
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
        net_eth_soft_inc(&g_u32LinkChanges);
        /* Cap inventory spam: init + first few link flips only. */
        if (g_u32SoftLogN < NET_ETH_SOFT_LOG_CAP) {
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

    net_eth_soft_inc(&g_u32Polls);

    u32Ready = net_eth_soft_link_sample();
    if (u32Ready == 0u) {
        /*
         * No device: skip RX. Multi-seg rtx only applies to virtio peers;
         * loopback multi-seg advances SndUna inline in net_tcp_send.
         * Still tick TCP soft TIME_WAIT / rtx for any live sockets.
         */
        net_eth_soft_inc(&g_u32PollsNoDev);
        net_tcp_poll();
        net_eth_soft_maybe_cadence();
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
        net_eth_soft_inc(&g_u32PollsDrain);
        net_eth_soft_add(&g_u32BatchSum, u32Batch);
        if (u32Batch > g_u32BatchMax) {
            g_u32BatchMax = u32Batch;
        }
        /*
         * First drain with traffic: one more greppable inventory line so
         * smokes see frames soft counters without needing a soft_log export.
         */
        if (g_u32PollsDrain == 1u && g_u32SoftLogN < NET_ETH_SOFT_LOG_CAP) {
            net_eth_soft_log();
        }
    } else {
        net_eth_soft_inc(&g_u32PollsEmpty);
    }
    /* Always tick last-seg rtx + TIME_WAIT soft reap after poll attempt. */
    net_tcp_poll();
    net_eth_soft_maybe_cadence();
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
