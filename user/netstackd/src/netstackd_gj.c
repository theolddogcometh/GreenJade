/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Freestanding netstackd — claims net door + lo socket echo via GJ_SYS_NET.
 * TCP: single-seg smoke + multi-segment bulk (>MSS) integrity for sshd/Top50.
 * UDX-shaped virtq: MAP_RING + MAP_DMA + DESC_ALLOC + user desc/avail + kick.
 * Pure C11 freestanding product daemon (MIT OR Apache-2.0; no GPL).
 *   make netstackd-gj → build/user/netstackd.elf
 */
#include <gj/syscalls.h>
/* GJ_NET_OP_CLAIM/RELEASE/SOCKET/BIND/SEND/RECV/CONNECT/CLOSE/LISTEN/ACCEPT/TCP_STATS from gj/syscalls.h */
#define GJ_NET_OP_VIRTIO_TX  12
#define GJ_NET_OP_QUEUE_INFO 14
#define GJ_NET_OP_EXPORT_RING 15
#define GJ_NET_OP_MAP_RING    16
#define GJ_NET_OP_KICK        17
#define GJ_NET_OP_AVAIL_PUSH 18
#define GJ_NET_OP_USED_REAP  19
#define GJ_NET_OP_RING_STATE 20
#define GJ_NET_OP_MAP_DMA     21
#define GJ_NET_OP_DESC_ALLOC  22
#define GJ_NET_OP_USER_AVAIL  23
#define GJ_NET_OP_BOUNCE_FILL 24

#define RING_VA  0x30000000ul
#define DMA_VA   0x31000000ul
#define PAGE_SZ  4096u

/* Socket domain/type (net door: family=2 INET, type 1=STREAM 2=DGRAM) */
#define GJ_AF_INET      2
#define GJ_SOCK_STREAM  1
#define GJ_SOCK_DGRAM   2

/*
 * Multi-seg TCP bounds (userspace mirrors of kernel contracts).
 *
 *   GJ_TCP_MSS       ↔ kernel net_tcp TCP_MSS (chunk size on TX)
 *   GJ_NET_XFER_MAX  ↔ kernel net_door NET_XFER_MAX (SEND/RECV bounce)
 *   GJ_MULTI_CB      — bulk length: must be > MSS (multi-seg) and
 *                      ≤ XFER_MAX (single door SEND fits one bounce).
 *   ceil(GJ_MULTI_CB / GJ_TCP_MSS) ≥ 3 payload segments (3000/1024).
 *
 * Door message layouts (vq_export / vq_dma / TCP_STATS packing) are
 * kernel-owned; this file only mirrors them. Do not rearrange fields.
 */
#define GJ_TCP_MSS        1024u
#define GJ_NET_XFER_MAX   4096u
#define GJ_MULTI_CB       3000u
#define GJ_MULTI_MAX_RECV 64u
#define GJ_TCP_TEST_PORT  7777

_Static_assert(GJ_MULTI_CB > GJ_TCP_MSS,
               "GJ_MULTI_CB must exceed MSS for multi-segment TX");
_Static_assert(GJ_MULTI_CB <= GJ_NET_XFER_MAX,
               "GJ_MULTI_CB must fit one net door SEND bounce");
_Static_assert((GJ_MULTI_CB + GJ_TCP_MSS - 1u) / GJ_TCP_MSS >= 3u,
               "GJ_MULTI_CB must yield ≥3 payload segments");

/*
 * Mirrors kernel virtq export layout (packed). Field order is ABI —
 * do not reorder/resize; kernel MAP_RING / EXPORT_RING fill this.
 */
struct vq_export {
    unsigned short which;
    unsigned short size;
    unsigned short queue_idx;
    unsigned short notify_off;
    unsigned long pa_desc;
    unsigned long pa_avail;
    unsigned long pa_used;
    unsigned notify_mult;
    unsigned ready;
    unsigned off_desc;
    unsigned off_avail;
    unsigned off_used;
    unsigned short free_head;
    unsigned short num_free;
};

/* Mirrors kernel DMA export layout — field order is ABI. */
struct vq_dma {
    unsigned n_slots;
    unsigned slot_sz;
    unsigned long va_base;
    unsigned long pa[8];
    unsigned ready;
};

/* Virtio split-vq descriptor (packed ABI). */
struct vq_desc {
    unsigned long addr;
    unsigned len;
    unsigned short flags;
    unsigned short next;
} __attribute__((packed));

/* Virtio split-vq avail ring (packed ABI; ring[256] covers max qsz). */
struct vq_avail {
    unsigned short flags;
    unsigned short idx;
    unsigned short ring[256];
} __attribute__((packed));

static void
msg(const char *sz)
{
    size_t n = 0;

    while (sz[n] != '\0') {
        n++;
    }
    (void)gj_debug_log(sz, (long)n);
}

static void
mfence(void)
{
    __asm__ volatile("mfence" ::: "memory");
}

/*
 * Loopback TCP smoke + multi-segment bulk messaging (client path).
 *
 * Roles:
 *   hSrv  — LISTEN on GJ_TCP_TEST_PORT
 *   hCli  — client: CONNECT + SEND (single-seg then multi-seg bulk)
 *   hAcc  — accepted peer: RECV drain of client's payload
 *
 * Multi-seg client contract:
 *   1) One SEND of GJ_MULTI_CB bytes through the door bounce (≤ XFER_MAX).
 *   2) Kernel net_tcp_send chunks by MSS into ≥3 payload segments.
 *   3) Peer RECV may short-read; drain until full length or error.
 *   4) Byte-compare aTx vs aRx; segs in TCP_STATS low16 is soft.
 *
 * Defensive bounds (userspace only — kernel still clamps):
 *   - SEND length is compile-time ≤ GJ_NET_XFER_MAX
 *   - RECV cap = remaining aRx capacity (never past buffer end)
 *   - RECV return > remaining → treat as fail (stop drain)
 *   - Drain iteration ceiling GJ_MULTI_MAX_RECV (no unbounded loop)
 *
 * Returns 1 if multi-seg integrity holds; 0 otherwise.
 */
static int
tcp_multi_seg_smoke(void)
{
    long hSrv;
    long hCli;
    long hAcc = -1;
    static char aSmall[] = "gj-tcp-ip";
    static unsigned char aTx[GJ_MULTI_CB];
    static unsigned char aRx[GJ_MULTI_CB];
    static char aSmallRx[32];
    long nSent;
    long nGot;
    long n;
    long nR;
    long cbRemain;
    unsigned aSt[4];
    unsigned i;
    unsigned cRecv;
    int fPath = 0;
    int fMulti = 0;

    for (i = 0; i < GJ_MULTI_CB; i++) {
        aTx[i] = (unsigned char)(i & 0xff);
        aRx[i] = 0;
    }

    hSrv = gj_net(GJ_NET_OP_SOCKET, GJ_AF_INET, GJ_SOCK_STREAM, 0);
    hCli = gj_net(GJ_NET_OP_SOCKET, GJ_AF_INET, GJ_SOCK_STREAM, 0);
    if (hSrv < 0 || hCli < 0) {
        return 0;
    }

    (void)gj_net(GJ_NET_OP_BIND, hSrv, GJ_TCP_TEST_PORT, 0);
    (void)gj_net(GJ_NET_OP_LISTEN, hSrv, 2, 0);
    if (gj_net(GJ_NET_OP_CONNECT, hCli, GJ_TCP_TEST_PORT, 0) != 0) {
        goto out;
    }
    hAcc = gj_net(GJ_NET_OP_ACCEPT, hSrv, 0, 0);
    if (hAcc < 0) {
        goto out;
    }

    /*
     * Single-segment client path: small SEND then one RECV.
     * Cap RECV to aSmallRx size (defensive; door also clamps to XFER_MAX).
     */
    n = gj_net(GJ_NET_OP_SEND, hCli, (long)(uintptr_t)aSmall,
               (long)sizeof(aSmall));
    if (n > 0 && (unsigned long)n <= sizeof(aSmallRx)) {
        n = gj_net(GJ_NET_OP_RECV, hAcc, (long)(uintptr_t)aSmallRx,
                   (long)sizeof(aSmallRx));
        if (n > 0 && (unsigned long)n <= sizeof(aSmallRx)) {
            fPath = 1;
            msg("netstackd-gj: TCP path PASS\n");
        }
    }

    /*
     * Multi-segment client SEND: one door call of GJ_MULTI_CB (> MSS,
     * ≤ XFER_MAX). Kernel emits ceil(GJ_MULTI_CB / GJ_TCP_MSS) segs.
     * Length is a compile-time constant already checked by _Static_assert.
     */
    nSent = gj_net(GJ_NET_OP_SEND, hCli, (long)(uintptr_t)aTx,
                   (long)GJ_MULTI_CB);
    /*
     * Reject short/failed SEND before drain: multi-seg integrity needs
     * the full bulk on the wire (partial SEND is not a multi-seg PASS).
     */
    if (nSent != (long)GJ_MULTI_CB || nSent <= (long)GJ_TCP_MSS ||
        nSent > (long)GJ_NET_XFER_MAX) {
        nGot = 0;
        goto multi_done;
    }

    /* Peer RECV drain — short reads OK; never write past aRx[GJ_MULTI_CB]. */
    nGot = 0;
    cRecv = 0;
    while (nGot < (long)GJ_MULTI_CB && cRecv < GJ_MULTI_MAX_RECV) {
        cbRemain = (long)GJ_MULTI_CB - nGot;
        if (cbRemain <= 0) {
            break;
        }
        /* Extra belt: cap request to door bounce even if buffer were larger. */
        if (cbRemain > (long)GJ_NET_XFER_MAX) {
            cbRemain = (long)GJ_NET_XFER_MAX;
        }
        nR = gj_net(GJ_NET_OP_RECV, hAcc,
                    (long)(uintptr_t)(aRx + nGot), cbRemain);
        if (nR <= 0) {
            break;
        }
        /*
         * Defensive: door should never return more than requested. If it
         * does, stop — writing past aRx would corrupt adjacent statics.
         */
        if (nR > cbRemain) {
            nGot = 0;
            break;
        }
        nGot += nR;
        cRecv++;
    }

multi_done:
    if (nSent == (long)GJ_MULTI_CB && nGot == nSent &&
        nSent > (long)GJ_TCP_MSS && nSent <= (long)GJ_NET_XFER_MAX) {
        fMulti = 1;
        for (i = 0; i < (unsigned)nGot; i++) {
            if (aRx[i] != aTx[i]) {
                fMulti = 0;
                break;
            }
        }
    }

    /*
     * TCP_STATS packing (kernel net_door — layout ABI, do not invent fields):
     *   aSt[0]=accepts  aSt[1]=(rtx<<16)|segs  aSt[2]=rxB  aSt[3]=txB
     * Multi-seg PASS is payload integrity of one SEND > MSS; segs low16 is soft.
     * Buffer is exactly 4 u32; door copy_to_user expects that size.
     */
    aSt[0] = aSt[1] = aSt[2] = aSt[3] = 0;
    (void)gj_net(GJ_NET_OP_TCP_STATS, (long)(uintptr_t)aSt, 0, 0);
    if (fMulti) {
        msg("netstackd-gj: TCP multi-segment PASS\n");
    } else if (fPath) {
        msg("netstackd-gj: TCP multi-segment FAIL\n");
    }

    (void)gj_net(GJ_NET_OP_CLOSE, hAcc, 0, 0);
    hAcc = -1;
out:
    if (hAcc >= 0) {
        (void)gj_net(GJ_NET_OP_CLOSE, hAcc, 0, 0);
    }
    (void)gj_net(GJ_NET_OP_CLOSE, hCli, 0, 0);
    (void)gj_net(GJ_NET_OP_CLOSE, hSrv, 0, 0);
    return fMulti;
}

void
_start(void)
{
    unsigned token = 0x11e70002u;
    long srv;
    long cli;
    static char msgb[] = "gj-live-net";
    static char rbuf[32];
    long n;

    msg("netstackd-gj: start\n");
    if (gj_net(GJ_NET_OP_CLAIM, (long)token, 0, 0) != 0) {
        msg("netstackd-gj: CLAIM fail\n");
        gj_exit(1);
    }
    /* UDP-shaped lo path (DGRAM) */
    srv = gj_net(GJ_NET_OP_SOCKET, GJ_AF_INET, GJ_SOCK_DGRAM, 0);
    cli = gj_net(GJ_NET_OP_SOCKET, GJ_AF_INET, GJ_SOCK_DGRAM, 0);
    if (srv < 0 || cli < 0) {
        msg("netstackd-gj: socket fail\n");
        (void)gj_net(GJ_NET_OP_RELEASE, (long)token, 0, 0);
        gj_exit(1);
    }
    (void)gj_net(GJ_NET_OP_BIND, srv, 17, 0);
    (void)gj_net(GJ_NET_OP_LISTEN, srv, 1, 0);
    (void)gj_net(GJ_NET_OP_CONNECT, cli, 17, 0);
    n = gj_net(GJ_NET_OP_SEND, cli, (long)(uintptr_t)msgb, (long)sizeof(msgb));
    n = gj_net(GJ_NET_OP_RECV, srv, (long)(uintptr_t)rbuf, (long)sizeof(rbuf));
    (void)gj_net(GJ_NET_OP_CLOSE, srv, 0, 0);
    (void)gj_net(GJ_NET_OP_CLOSE, cli, 0, 0);

    /* TCP/IPv4 + multi-segment messaging (product stack for sshd / Top50) */
    (void)tcp_multi_seg_smoke();

    /* Virtio TX via door (queue ownership path) */
    {
        static unsigned char frame[64];
        unsigned i;
        long tx;
        unsigned qinfo[5];

        for (i = 0; i < 64; i++) {
            frame[i] = (unsigned char)(0x55 + i);
        }
        frame[0] = frame[1] = frame[2] = frame[3] = frame[4] = frame[5] = 0xff;
        tx = gj_net(GJ_NET_OP_VIRTIO_TX, (long)(uintptr_t)frame, 64, 0);
        (void)gj_net(GJ_NET_OP_QUEUE_INFO, (long)(uintptr_t)qinfo, 0, 0);
        if (tx > 0) {
            msg("netstackd-gj: virtio queue PASS\n");
        }
        /* Ring-level AVAIL push (kernel bounce helper path) */
        {
            long ap;
            unsigned st[4];

            ap = gj_net(GJ_NET_OP_AVAIL_PUSH, 1, (long)(uintptr_t)frame, 64);
            (void)gj_net(GJ_NET_OP_USED_REAP, 1, 8, 0);
            (void)gj_net(GJ_NET_OP_RING_STATE, (long)(uintptr_t)st, 0, 0);
            if (ap > 0 && (st[2] & 0xffffu) >= 1) {
                msg("netstackd-gj: avail push PASS\n");
            }
        }
        /* UDX userspace ring programming (desc + avail on mapped VA) */
        {
            struct vq_export ex;
            struct vq_dma dma;
            long head;
            long filled;
            unsigned st[4];
            struct vq_desc *pDesc;
            struct vq_avail *pAvail;
            unsigned short slot;
            unsigned short qsz;
            unsigned user_pushes;

            for (i = 0; i < sizeof(ex); i++) {
                ((unsigned char *)&ex)[i] = 0;
            }
            for (i = 0; i < sizeof(dma); i++) {
                ((unsigned char *)&dma)[i] = 0;
            }
            (void)gj_net(GJ_NET_OP_EXPORT_RING, 1, (long)(uintptr_t)&ex, 0);
            if (ex.ready) {
                (void)gj_net(GJ_NET_OP_MAP_RING, 1, (long)RING_VA,
                             (long)(uintptr_t)&ex);
                (void)gj_net(GJ_NET_OP_MAP_DMA, (long)DMA_VA,
                             (long)(uintptr_t)&dma, 0);
                filled = gj_net(GJ_NET_OP_BOUNCE_FILL, 1, (long)(uintptr_t)frame,
                                64);
                head = gj_net(GJ_NET_OP_DESC_ALLOC, 1, 0, 0);
                if (head >= 0 && filled > 0 && dma.ready) {
                    qsz = ex.size ? ex.size : 64;
                    pDesc = (struct vq_desc *)(uintptr_t)(
                        RING_VA + (ex.off_desc ? ex.off_desc : 0));
                    pAvail = (struct vq_avail *)(uintptr_t)(
                        RING_VA +
                        (ex.off_avail ? ex.off_avail : PAGE_SZ));
                    /* Userspace programs descriptor fields */
                    pDesc[head].addr = dma.pa[1];
                    pDesc[head].len = (unsigned)filled;
                    pDesc[head].flags = 0;
                    pDesc[head].next = 0;
                    /* Userspace programs avail ring */
                    mfence();
                    slot = (unsigned short)(pAvail->idx % qsz);
                    pAvail->ring[slot] = (unsigned short)head;
                    mfence();
                    pAvail->idx = (unsigned short)(pAvail->idx + 1);
                    mfence();
                    /* Commit: kick only (user already wrote avail) */
                    (void)gj_net(GJ_NET_OP_USER_AVAIL, 1, head, 1);
                    (void)gj_net(GJ_NET_OP_USED_REAP, 1, 8, 0);
                    (void)gj_net(GJ_NET_OP_RING_STATE, (long)(uintptr_t)st, 0,
                                 0);
                    user_pushes = st[2] >> 16;
                    if (user_pushes >= 1) {
                        msg("netstackd-gj: user ring PASS\n");
                    }
                }
            }
        }
        (void)qinfo;
    }
    (void)gj_net(GJ_NET_OP_RELEASE, (long)token, 0, 0);
    if (n > 0) {
        msg("netstackd-gj: live path PASS\n");
        gj_exit(0);
    }
    msg("netstackd-gj: recv fail\n");
    gj_exit(1);
}
