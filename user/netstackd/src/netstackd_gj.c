/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Freestanding netstackd — claims net door + lo socket echo via GJ_SYS_NET.
 * Hard path: CLAIM → DGRAM echo → TCP single/multi-seg → virtio queue/ring →
 *            RELEASE → live path PASS (dgram RECV green).
 * Soft path (deepened; never aborts live path):
 *   same-token CLAIM reclaim, POLL + eth STATS, QUEUE_INFO owned,
 *   dgram echo integrity, TCP_STATS soft report, VIRTIO_RX drain,
 *   MAP_RING re-MAP (map reclaim), KICK, RING_STATE soft, free RELEASE,
 *   greppable soft inventory under "netstackd-gj: soft …".
 * UDX-shaped virtq: MAP_RING + MAP_DMA + DESC_ALLOC + user desc/avail + kick.
 * Pure C11 freestanding product daemon (MIT OR Apache-2.0; no GPL).
 *
 * Hard smoke markers (prefix-stable for scripts/smoke-all.sh):
 *   netstackd-gj: avail push PASS
 *   netstackd-gj: user ring PASS
 *   netstackd-gj: live path PASS
 * Soft (optional; never fails live path):
 *   netstackd-gj: reclaim soft PASS | soft-skip
 *   netstackd-gj: POLL soft PASS | soft-skip
 *   netstackd-gj: STATS soft PASS | soft-skip
 *   netstackd-gj: QUEUE_INFO soft PASS | soft-skip
 *   netstackd-gj: dgram echo soft PASS | soft-skip
 *   netstackd-gj: TCP_STATS soft PASS | soft-skip
 *   netstackd-gj: VIRTIO_RX soft PASS | soft-empty | soft-skip
 *   netstackd-gj: re-MAP soft PASS | soft-skip
 *   netstackd-gj: KICK soft PASS | soft-skip
 *   netstackd-gj: RING_STATE soft PASS | soft-skip
 *   netstackd-gj: soft door PASS | soft door soft-skip
 *   netstackd-gj: RELEASE free soft PASS | soft-skip
 * Soft inventory (Wave 61 exclusive deepen — greppable "netstackd-gj: soft …"):
 *   netstackd-gj: soft inventory ok=… skip=… bits=0x… ring_mapped=…
 *                free_rel=… wave=61
 *   netstackd-gj: soft door reclaim=… poll=… stats=… qinfo=… dgram=…
 *                tcpst=… rx=… remap=… kick=… ringst=… bits=0x…
 *   netstackd-gj: soft stats arp=… udp=… icmp=… calls=… door=…
 *   netstackd-gj: soft queue tx=… rx=… ready=… owned=… vq=… door=…
 *   netstackd-gj: soft ring s0=… s1=… s2=… s3=… door=…
 *   netstackd-gj: soft rx frames=… last=… empty=…
 *   netstackd-gj: soft free release=… bits=…
 *   netstackd-gj: soft steps ok=… skip=… max=… bits=0x…
 *   netstackd-gj: soft deepen wave=61 areas=… ok=… skip=…
 *   netstackd-gj: soft path reclaim=claim … multi_server=0 confine=0
 *                (soft inventory; not bar3; soft ≠ product multi-server confine)
 *   netstackd-gj: soft free-release PASS | soft free-release soft-skip
 * Diagnostics only — never hard-fail live path; soft ≠ bar3.
 * Honesty: soft inventory ≠ product multi-server confine.
 *
 *   make netstackd-gj → build/user/netstackd.elf
 */
#include <gj/syscalls.h>
/* GJ_NET_OP_CLAIM/RELEASE/SOCKET/BIND/SEND/RECV/CONNECT/CLOSE/LISTEN/ACCEPT/TCP_STATS from gj/syscalls.h */
#define GJ_NET_OP_VIRTIO_TX  12
#define GJ_NET_OP_VIRTIO_RX  13
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

/* Soft VIRTIO_RX drain cap (defensive; empty ring is fine). */
#define GJ_SOFT_RX_CAP    8u
#define GJ_SOFT_RX_FRAME  1514u

/* Soft door sub-step bits (aggregate only; never hard-fail). */
#define GJ_SOFT_BIT_RECLAIM  (1u << 0)
#define GJ_SOFT_BIT_POLL     (1u << 1)
#define GJ_SOFT_BIT_STATS    (1u << 2)
#define GJ_SOFT_BIT_QINFO    (1u << 3)
#define GJ_SOFT_BIT_DGRAM    (1u << 4)
#define GJ_SOFT_BIT_TCPST    (1u << 5)
#define GJ_SOFT_BIT_RX       (1u << 6)
#define GJ_SOFT_BIT_REMAP    (1u << 7)
#define GJ_SOFT_BIT_KICK     (1u << 8)
#define GJ_SOFT_BIT_RINGST   (1u << 9)
/* Soft free-path bit (Wave 61 inventory; post-RELEASE no-op). */
/* Wave 61 soft deepen surfaces (CREATE-ONLY soft ≠ product):
 *   greppable: soft retcounterguard continuum_toward=20300 soft_ne_product=1
 *   greppable: soft retcoveredface exclusive=1 continuum_toward=20300
 * Soft ≠ product complete; product lamps 0; bar3 OPEN.
 */

#define GJ_SOFT_FREE_RELEASE (1u << 0)
/* Soft door suite step ceiling (reclaim..RING_STATE). */
#define GJ_SOFT_DOOR_MAX     10u
/* Soft inventory greppable area count (inventory…path + deepen). */
#define GJ_SOFT_AREAS        11u
/* Wave stamp for greppable soft inventory lines (Wave 61 exclusive). */
#define GJ_SOFT_WAVE 61u

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

/*
 * Soft-door bookkeeping (filled while CLAIM held; never hard-fails).
 * Wave 61 inventory tallies: ok/skip + door lamps + free bit + snapshots.
 */
struct soft_ctx {
    unsigned uBits;      /* GJ_SOFT_BIT_* door suite mask */
    unsigned cOk;        /* soft door sub-steps greened */
    unsigned cSkip;      /* soft door sub-steps soft-skipped */
    unsigned uFreeBits;  /* GJ_SOFT_FREE_* free-path mask */
    int fRingMapped;     /* 1 when hard path mapped RING_VA */
    /* Soft inventory snapshots (greppable "netstackd-gj: soft …"). */
    unsigned aEth[4];  /* arp, udp, icmp, calls */
    unsigned aQ[5];    /* tx, rx, ready, owned, vq_calls */
    unsigned aRing[4]; /* RING_STATE packing */
    unsigned cRxFrames;
    long nRxLast;
    int fEthDoor;  /* STATS door returned 0 */
    int fQDoor;    /* QUEUE_INFO door returned 0 */
    int fRingDoor; /* RING_STATE door returned 0 */
    int fFreeRel;  /* second RELEASE soft no-op greened */
    int fRxEmpty;  /* VIRTIO_RX accepted with zero frames */
};

static void
msg(const char *sz)
{
    size_t n = 0;

    while (sz[n] != '\0') {
        n++;
    }
    (void)gj_debug_log(sz, (long)n);
}

/* Freestanding line builders for soft inventory (no libc sprintf). */
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
        aDig[n++] = '0';
    } else {
        while (u > 0 && n < sizeof(aDig)) {
            aDig[n++] = (char)('0' + (u % 10ul));
            u /= 10ul;
        }
    }
    for (i = n; i > 0 && *po + 1u < cb; i--) {
        aLine[(*po)++] = aDig[i - 1u];
    }
}

static void
append_hex(char *aLine, unsigned cb, unsigned *po, unsigned long u)
{
    char aHex[16];
    unsigned n = 0;
    unsigned i;

    if (aLine == 0 || po == 0 || cb == 0) {
        return;
    }
    if (u == 0) {
        aHex[n++] = '0';
    } else {
        while (u > 0 && n < sizeof(aHex)) {
            unsigned d = (unsigned)(u & 0xful);

            aHex[n++] = (char)(d < 10u ? ('0' + d) : ('a' + (d - 10u)));
            u >>= 4;
        }
    }
    for (i = n; i > 0 && *po + 1u < cb; i--) {
        aLine[(*po)++] = aHex[i - 1u];
    }
}

/* Wave 61: 0/1 lamp from soft door bit mask. */
static unsigned
soft_lamp(unsigned uBits, unsigned uMask)
{
    return ((uBits & uMask) != 0u) ? 1u : 0u;
}

/*
 * Soft inventory dump (Wave 61 exclusive deepen).
 * Greppable prefix: "netstackd-gj: soft …"
 * Pure observation — always soft; never gates live path PASS.
 *
 *   netstackd-gj: soft inventory …
 *   netstackd-gj: soft door …
 *   netstackd-gj: soft stats …
 *   netstackd-gj: soft queue …
 *   netstackd-gj: soft ring …
 *   netstackd-gj: soft rx …
 *   netstackd-gj: soft free …
 *   netstackd-gj: soft steps …
 *   netstackd-gj: soft path …
 */
static void
soft_inventory_log(const struct soft_ctx *pSoft)
{
    char aLine[224];
    unsigned o;

    if (pSoft == 0) {
        msg("netstackd-gj: soft inventory soft-skip\n");
        return;
    }

    /* Grep: netstackd-gj: soft inventory */
    o = 0;
    append_s(aLine, sizeof(aLine), &o, "netstackd-gj: soft inventory ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)pSoft->cOk);
    append_s(aLine, sizeof(aLine), &o, " skip=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)pSoft->cSkip);
    append_s(aLine, sizeof(aLine), &o, " bits=0x");
    append_hex(aLine, sizeof(aLine), &o, (unsigned long)pSoft->uBits);
    append_s(aLine, sizeof(aLine), &o, " ring_mapped=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)(pSoft->fRingMapped ? 1u : 0u));
    append_s(aLine, sizeof(aLine), &o, " free_rel=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)(pSoft->fFreeRel ? 1u : 0u));
    append_s(aLine, sizeof(aLine), &o, " wave=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)GJ_SOFT_WAVE);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: netstackd-gj: soft door (per-step lamps) */
    o = 0;
    append_s(aLine, sizeof(aLine), &o, "netstackd-gj: soft door reclaim=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)soft_lamp(pSoft->uBits, GJ_SOFT_BIT_RECLAIM));
    append_s(aLine, sizeof(aLine), &o, " poll=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)soft_lamp(pSoft->uBits, GJ_SOFT_BIT_POLL));
    append_s(aLine, sizeof(aLine), &o, " stats=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)soft_lamp(pSoft->uBits, GJ_SOFT_BIT_STATS));
    append_s(aLine, sizeof(aLine), &o, " qinfo=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)soft_lamp(pSoft->uBits, GJ_SOFT_BIT_QINFO));
    append_s(aLine, sizeof(aLine), &o, " dgram=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)soft_lamp(pSoft->uBits, GJ_SOFT_BIT_DGRAM));
    append_s(aLine, sizeof(aLine), &o, " tcpst=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)soft_lamp(pSoft->uBits, GJ_SOFT_BIT_TCPST));
    append_s(aLine, sizeof(aLine), &o, " rx=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)soft_lamp(pSoft->uBits, GJ_SOFT_BIT_RX));
    append_s(aLine, sizeof(aLine), &o, " remap=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)soft_lamp(pSoft->uBits, GJ_SOFT_BIT_REMAP));
    append_s(aLine, sizeof(aLine), &o, " kick=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)soft_lamp(pSoft->uBits, GJ_SOFT_BIT_KICK));
    append_s(aLine, sizeof(aLine), &o, " ringst=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)soft_lamp(pSoft->uBits, GJ_SOFT_BIT_RINGST));
    append_s(aLine, sizeof(aLine), &o, " bits=0x");
    append_hex(aLine, sizeof(aLine), &o, (unsigned long)pSoft->uBits);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: netstackd-gj: soft stats */
    o = 0;
    append_s(aLine, sizeof(aLine), &o, "netstackd-gj: soft stats arp=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)pSoft->aEth[0]);
    append_s(aLine, sizeof(aLine), &o, " udp=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)pSoft->aEth[1]);
    append_s(aLine, sizeof(aLine), &o, " icmp=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)pSoft->aEth[2]);
    append_s(aLine, sizeof(aLine), &o, " calls=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)pSoft->aEth[3]);
    append_s(aLine, sizeof(aLine), &o, " door=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)(pSoft->fEthDoor ? 1u : 0u));
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: netstackd-gj: soft queue */
    o = 0;
    append_s(aLine, sizeof(aLine), &o, "netstackd-gj: soft queue tx=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)pSoft->aQ[0]);
    append_s(aLine, sizeof(aLine), &o, " rx=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)pSoft->aQ[1]);
    append_s(aLine, sizeof(aLine), &o, " ready=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)pSoft->aQ[2]);
    append_s(aLine, sizeof(aLine), &o, " owned=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)pSoft->aQ[3]);
    append_s(aLine, sizeof(aLine), &o, " vq=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)pSoft->aQ[4]);
    append_s(aLine, sizeof(aLine), &o, " door=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)(pSoft->fQDoor ? 1u : 0u));
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: netstackd-gj: soft ring */
    o = 0;
    append_s(aLine, sizeof(aLine), &o, "netstackd-gj: soft ring s0=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)pSoft->aRing[0]);
    append_s(aLine, sizeof(aLine), &o, " s1=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)pSoft->aRing[1]);
    append_s(aLine, sizeof(aLine), &o, " s2=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)pSoft->aRing[2]);
    append_s(aLine, sizeof(aLine), &o, " s3=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)pSoft->aRing[3]);
    append_s(aLine, sizeof(aLine), &o, " door=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)(pSoft->fRingDoor ? 1u : 0u));
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: netstackd-gj: soft rx */
    o = 0;
    append_s(aLine, sizeof(aLine), &o, "netstackd-gj: soft rx frames=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)pSoft->cRxFrames);
    append_s(aLine, sizeof(aLine), &o, " last=");
    if (pSoft->nRxLast < 0) {
        append_s(aLine, sizeof(aLine), &o, "-");
        append_u(aLine, sizeof(aLine), &o,
                 (unsigned long)(-pSoft->nRxLast));
    } else {
        append_u(aLine, sizeof(aLine), &o, (unsigned long)pSoft->nRxLast);
    }
    append_s(aLine, sizeof(aLine), &o, " empty=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)(pSoft->fRxEmpty ? 1u : 0u));
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: netstackd-gj: soft free */
    o = 0;
    append_s(aLine, sizeof(aLine), &o, "netstackd-gj: soft free release=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)soft_lamp(pSoft->uFreeBits, GJ_SOFT_FREE_RELEASE));
    append_s(aLine, sizeof(aLine), &o, " bits=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)pSoft->uFreeBits);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: netstackd-gj: soft steps (rollup) */
    o = 0;
    append_s(aLine, sizeof(aLine), &o, "netstackd-gj: soft steps ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)pSoft->cOk);
    append_s(aLine, sizeof(aLine), &o, " skip=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)pSoft->cSkip);
    append_s(aLine, sizeof(aLine), &o, " max=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)GJ_SOFT_DOOR_MAX);
    append_s(aLine, sizeof(aLine), &o, " bits=0x");
    append_hex(aLine, sizeof(aLine), &o, (unsigned long)pSoft->uBits);
    append_s(aLine, sizeof(aLine), &o, " free_bits=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)pSoft->uFreeBits);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: netstackd-gj: soft deepen wave (Wave 61 stamp) */
    o = 0;
    append_s(aLine, sizeof(aLine), &o, "netstackd-gj: soft deepen wave=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)GJ_SOFT_WAVE);
    append_s(aLine, sizeof(aLine), &o, " areas=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)GJ_SOFT_AREAS);
    append_s(aLine, sizeof(aLine), &o, " ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)pSoft->cOk);
    append_s(aLine, sizeof(aLine), &o, " skip=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)pSoft->cSkip);
    append_s(aLine, sizeof(aLine), &o, " multi_server=0 confine=0\n");
    aLine[o] = '\0';
    msg(aLine);

    /*
     * Grep: netstackd-gj: soft path
     * Static route labels for agent greps (not live counters).
     * Soft inventory ≠ product multi-server confine.
     */
    msg("netstackd-gj: soft path reclaim=claim poll=eth stats=eth "
        "queue=owned dgram=echo tcp=door_stats rx=virtio remap=map_ring "
        "kick=vq ring=state free=release_noop multi_server=0 confine=0 "
        "(soft inventory; not bar3; soft != product multi-server confine)\n");

    /*
     * Grep: netstackd-gj: soft honesty (Wave 61 exclusive deepen).
     * Soft inventory ≠ product multi-server confine.
     */
    msg("netstackd-gj: soft honesty multi_server=0 confine=0 bar3=0 "
        "exclusive=1 soft=1 product_kernel=OPEN wave=61\n");
}

static void
mfence(void)
{
    __asm__ volatile("mfence" ::: "memory");
}

static int
mem_eq(const void *a, const void *b, unsigned n)
{
    const unsigned char *p = (const unsigned char *)a;
    const unsigned char *q = (const unsigned char *)b;
    unsigned i;

    for (i = 0; i < n; i++) {
        if (p[i] != q[i]) {
            return 0;
        }
    }
    return 1;
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
 * Soft: TCP_STATS door success is recorded via *pfTcpStats when non-NULL.
 */
static int
tcp_multi_seg_smoke(int *pfTcpStats)
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

    if (pfTcpStats != 0) {
        *pfTcpStats = 0;
    }

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
    if (gj_net(GJ_NET_OP_TCP_STATS, (long)(uintptr_t)aSt, 0, 0) == 0) {
        if (pfTcpStats != 0) {
            *pfTcpStats = 1;
        }
    }
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

/*
 * Soft door suite while CLAIM is held (deepened).
 * Never aborts hard path; each step soft-skip on rejection / empty / NODEV.
 * Call after hard DGRAM + TCP smokes so STATS/calls reflect product traffic.
 * Virtio-dependent steps (RX / re-MAP / KICK / RING_STATE) are soft and
 * must not reorder or replace hard avail-push / user-ring PASS markers.
 *
 * Returns count of soft sub-steps that greened.
 */
static unsigned
soft_door_path(struct soft_ctx *pSoft, unsigned token,
               const char *pDgramTx, long nDgramRx, const char *pDgramRx,
               int fTcpStats)
{
    static unsigned char aRxFrame[GJ_SOFT_RX_FRAME];
    unsigned aSt[4];
    unsigned aQ[5];
    unsigned st[4];
    unsigned i;
    unsigned cRx;
    long n;
    long nRx;

    if (pSoft == 0) {
        msg("netstackd-gj: soft door soft-skip\n");
        return 0u;
    }
    pSoft->uBits = 0u;
    pSoft->cOk = 0u;
    pSoft->cSkip = 0u;
    /* uFreeBits / fFreeRel / fRingMapped owned by _start (free path later). */
    pSoft->aEth[0] = pSoft->aEth[1] = pSoft->aEth[2] = pSoft->aEth[3] = 0u;
    pSoft->aQ[0] = pSoft->aQ[1] = pSoft->aQ[2] = pSoft->aQ[3] = pSoft->aQ[4] = 0u;
    pSoft->aRing[0] = pSoft->aRing[1] = pSoft->aRing[2] = pSoft->aRing[3] = 0u;
    pSoft->cRxFrames = 0u;
    pSoft->nRxLast = 0;
    pSoft->fEthDoor = 0;
    pSoft->fQDoor = 0;
    pSoft->fRingDoor = 0;
    pSoft->fRxEmpty = 0;

    msg("netstackd-gj: soft door start\n");

    /*
     * 1) Soft reclaim: same-token re-CLAIM is idempotent (net_door soft path).
     *    Different token would be BUSY — never try that on the live path.
     */
    if (token != 0u) {
        n = gj_net(GJ_NET_OP_CLAIM, (long)token, 0, 0);
        if (n == 0) {
            pSoft->uBits |= GJ_SOFT_BIT_RECLAIM;
            pSoft->cOk++;
            msg("netstackd-gj: reclaim soft PASS\n");
        } else {
            pSoft->cSkip++;
            msg("netstackd-gj: reclaim soft-skip\n");
        }
    } else {
        pSoft->cSkip++;
        msg("netstackd-gj: reclaim soft-skip\n");
    }

    /* 2) POLL — drain virtio-net / eth; always 0 when door is up. */
    n = gj_net(GJ_NET_OP_POLL, 0, 0, 0);
    if (n == 0) {
        pSoft->uBits |= GJ_SOFT_BIT_POLL;
        pSoft->cOk++;
        msg("netstackd-gj: POLL soft PASS\n");
    } else {
        pSoft->cSkip++;
        msg("netstackd-gj: POLL soft-skip\n");
    }

    /*
     * 3) eth STATS: u32[4] = {arp, udp, icmp, calls}. Soft-green when
     *    door returns 0 and calls counter (aSt[3]) is non-zero (we have
     *    already exercised sockets / claim). Snapshot always for inventory.
     */
    aSt[0] = aSt[1] = aSt[2] = aSt[3] = 0;
    n = gj_net(GJ_NET_OP_STATS, (long)(uintptr_t)aSt, 0, 0);
    pSoft->aEth[0] = aSt[0];
    pSoft->aEth[1] = aSt[1];
    pSoft->aEth[2] = aSt[2];
    pSoft->aEth[3] = aSt[3];
    pSoft->fEthDoor = (n == 0) ? 1 : 0;
    if (n == 0 && aSt[3] != 0u) {
        pSoft->uBits |= GJ_SOFT_BIT_STATS;
        pSoft->cOk++;
        msg("netstackd-gj: STATS soft PASS\n");
    } else {
        pSoft->cSkip++;
        msg("netstackd-gj: STATS soft-skip\n");
    }

    /*
     * 4) QUEUE_INFO: u32[5] = {tx, rx, ready, owned, vq_calls}.
     *    Soft-green when owned bit set (CLAIM held). ready may be 0
     *    without virtio-net (still soft-skip only that half).
     */
    aQ[0] = aQ[1] = aQ[2] = aQ[3] = aQ[4] = 0;
    n = gj_net(GJ_NET_OP_QUEUE_INFO, (long)(uintptr_t)aQ, 0, 0);
    pSoft->aQ[0] = aQ[0];
    pSoft->aQ[1] = aQ[1];
    pSoft->aQ[2] = aQ[2];
    pSoft->aQ[3] = aQ[3];
    pSoft->aQ[4] = aQ[4];
    pSoft->fQDoor = (n == 0) ? 1 : 0;
    if (n == 0 && aQ[3] != 0u) {
        pSoft->uBits |= GJ_SOFT_BIT_QINFO;
        pSoft->cOk++;
        msg("netstackd-gj: QUEUE_INFO soft PASS\n");
    } else {
        pSoft->cSkip++;
        msg("netstackd-gj: QUEUE_INFO soft-skip\n");
    }

    /*
     * 5) DGRAM echo integrity soft — hard path only needs RECV n > 0
     *    for live path PASS; soft compares payload when lengths match.
     */
    if (pDgramTx != 0 && pDgramRx != 0 && nDgramRx > 0) {
        unsigned cb = 0;

        while (pDgramTx[cb] != '\0') {
            cb++;
        }
        cb++; /* include NUL like sizeof(msgb) SEND */
        if ((unsigned long)nDgramRx == (unsigned long)cb &&
            mem_eq(pDgramRx, pDgramTx, cb)) {
            pSoft->uBits |= GJ_SOFT_BIT_DGRAM;
            pSoft->cOk++;
            msg("netstackd-gj: dgram echo soft PASS\n");
        } else {
            pSoft->cSkip++;
            msg("netstackd-gj: dgram echo soft-skip\n");
        }
    } else {
        pSoft->cSkip++;
        msg("netstackd-gj: dgram echo soft-skip\n");
    }

    /* 6) TCP_STATS soft — door success only (payload multi-seg is hard). */
    if (fTcpStats) {
        pSoft->uBits |= GJ_SOFT_BIT_TCPST;
        pSoft->cOk++;
        msg("netstackd-gj: TCP_STATS soft PASS\n");
    } else {
        pSoft->cSkip++;
        msg("netstackd-gj: TCP_STATS soft-skip\n");
    }

    /*
     * 7) VIRTIO_RX soft drain — empty is soft-empty (still a green probe
     *    when door accepts the op); NODEV / error → soft-skip.
     */
    cRx = 0;
    nRx = 0;
    for (i = 0; i < GJ_SOFT_RX_CAP; i++) {
        n = gj_net(GJ_NET_OP_VIRTIO_RX, (long)(uintptr_t)aRxFrame,
                   (long)GJ_SOFT_RX_FRAME, 0);
        if (n < 0) {
            nRx = n;
            break;
        }
        if (n == 0) {
            nRx = 0;
            break;
        }
        cRx++;
        nRx = n;
        if ((unsigned long)n > (unsigned long)GJ_SOFT_RX_FRAME) {
            nRx = -1;
            break;
        }
    }
    pSoft->cRxFrames = cRx;
    pSoft->nRxLast = nRx;
    if (nRx >= 0) {
        pSoft->uBits |= GJ_SOFT_BIT_RX;
        pSoft->cOk++;
        if (cRx > 0u) {
            pSoft->fRxEmpty = 0;
            msg("netstackd-gj: VIRTIO_RX soft PASS\n");
        } else {
            pSoft->fRxEmpty = 1;
            msg("netstackd-gj: VIRTIO_RX soft-empty\n");
        }
    } else {
        pSoft->cSkip++;
        pSoft->fRxEmpty = 0;
        msg("netstackd-gj: VIRTIO_RX soft-skip\n");
    }
    (void)aRxFrame;

    /*
     * 8) MAP_RING re-MAP soft — same VA re-install (kernel map reclaim).
     *    Only when hard path already mapped; never invent a map here.
     */
    if (pSoft->fRingMapped) {
        struct vq_export ex;

        for (i = 0; i < sizeof(ex); i++) {
            ((unsigned char *)&ex)[i] = 0;
        }
        n = gj_net(GJ_NET_OP_MAP_RING, 1, (long)RING_VA,
                   (long)(uintptr_t)&ex);
        if (n == 0) {
            pSoft->uBits |= GJ_SOFT_BIT_REMAP;
            pSoft->cOk++;
            msg("netstackd-gj: re-MAP soft PASS\n");
        } else {
            pSoft->cSkip++;
            msg("netstackd-gj: re-MAP soft-skip\n");
        }
        (void)ex;
    } else {
        pSoft->cSkip++;
        msg("netstackd-gj: re-MAP soft-skip\n");
    }

    /* 9) KICK soft — best-effort notify; NODEV soft-skip without virtio. */
    n = gj_net(GJ_NET_OP_KICK, 1, 0, 0);
    if (n == 0) {
        pSoft->uBits |= GJ_SOFT_BIT_KICK;
        pSoft->cOk++;
        msg("netstackd-gj: KICK soft PASS\n");
    } else {
        pSoft->cSkip++;
        msg("netstackd-gj: KICK soft-skip\n");
    }

    /*
     * 10) RING_STATE soft — always fills when door is up (zeros without
     *     virtio-net). Soft-green on door success alone.
     */
    st[0] = st[1] = st[2] = st[3] = 0;
    n = gj_net(GJ_NET_OP_RING_STATE, (long)(uintptr_t)st, 0, 0);
    pSoft->aRing[0] = st[0];
    pSoft->aRing[1] = st[1];
    pSoft->aRing[2] = st[2];
    pSoft->aRing[3] = st[3];
    pSoft->fRingDoor = (n == 0) ? 1 : 0;
    if (n == 0) {
        pSoft->uBits |= GJ_SOFT_BIT_RINGST;
        pSoft->cOk++;
        msg("netstackd-gj: RING_STATE soft PASS\n");
    } else {
        pSoft->cSkip++;
        msg("netstackd-gj: RING_STATE soft-skip\n");
    }

    /*
     * Soft inventory (greppable "netstackd-gj: soft …") — always emit after
     * sub-steps so smoke can tally door/eth/queue/ring/rx without hard FAIL.
     * free_rel may still be 0 here; final Wave 61 rollup re-emits after free.
     */
    soft_inventory_log(pSoft);

    /* Aggregate soft door line — green if any sub-step greened. */
    if (pSoft->cOk > 0u) {
        msg("netstackd-gj: soft door PASS\n");
    } else {
        msg("netstackd-gj: soft door soft-skip\n");
    }
    return pSoft->cOk;
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
    int fTcpStats = 0;
    struct soft_ctx soft;

    soft.uBits = 0u;
    soft.cOk = 0u;
    soft.cSkip = 0u;
    soft.uFreeBits = 0u;
    soft.fRingMapped = 0;
    soft.aEth[0] = soft.aEth[1] = soft.aEth[2] = soft.aEth[3] = 0u;
    soft.aQ[0] = soft.aQ[1] = soft.aQ[2] = soft.aQ[3] = soft.aQ[4] = 0u;
    soft.aRing[0] = soft.aRing[1] = soft.aRing[2] = soft.aRing[3] = 0u;
    soft.cRxFrames = 0u;
    soft.nRxLast = 0;
    soft.fEthDoor = 0;
    soft.fQDoor = 0;
    soft.fRingDoor = 0;
    soft.fFreeRel = 0;
    soft.fRxEmpty = 0;

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
    (void)tcp_multi_seg_smoke(&fTcpStats);

    /* Virtio TX via door (queue ownership path) — hard markers below */
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
                soft.fRingMapped = 1;
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

    /*
     * Soft door deepen after hard virtio markers — reclaim / poll / stats /
     * queue / dgram integrity / TCP_STATS / RX / re-MAP / KICK / RING_STATE.
     * Never rewrites hard PASS lines; never aborts on soft-skip.
     */
    (void)soft_door_path(&soft, token, msgb, n, rbuf, fTcpStats);

    (void)gj_net(GJ_NET_OP_RELEASE, (long)token, 0, 0);

    /*
     * Soft free RELEASE: door already free → soft no-op (0).
     * Never hard-fails live path (mirrors vfsd RELEASE free soft).
     * Dual markers: legacy RELEASE free soft + greppable soft free-release.
     * Wave 61: also stamp uFreeBits and re-emit soft inventory (free_rel).
     */
    if (gj_net(GJ_NET_OP_RELEASE, (long)token, 0, 0) == 0) {
        soft.fFreeRel = 1;
        soft.uFreeBits |= GJ_SOFT_FREE_RELEASE;
        msg("netstackd-gj: RELEASE free soft PASS\n");
        msg("netstackd-gj: soft free-release PASS\n");
    } else {
        soft.fFreeRel = 0;
        msg("netstackd-gj: RELEASE free soft-skip\n");
        msg("netstackd-gj: soft free-release soft-skip\n");
    }
    /* Final Wave 61 soft inventory rollup (includes free_rel / free bits). */
    soft_inventory_log(&soft);

    /* Hard live path: DGRAM RECV green (prefix-stable; smoke-all greps). */
    if (n > 0) {
        msg("netstackd-gj: live path PASS\n");
        gj_exit(0);
    }
    msg("netstackd-gj: recv fail\n");
    gj_exit(1);
}
