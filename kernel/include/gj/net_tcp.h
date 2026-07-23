/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Minimal IPv4 TCP over virtio-net + loopback pairs (sshd / netstackd).
 * Pure C11 freestanding, dual MIT OR Apache-2.0. Clean-room — no GPL stack.
 *
 * Features (bring-up product path):
 *   SYN handshake, ordered RX ring, multi-segment TX (MSS chunks),
 *   advertised peer window soft limit, last-segment retransmit on poll,
 *   soft close states (FIN_WAIT / LAST_ACK / TIME_WAIT), listen backlog.
 *
 * FD layout (avoid vfs_ram and net_lo ranges):
 *   FDs 96..111 → 16 slots (TCP_MAX). net_tcp_fd_ok is the ownership test.
 *
 * Sizing contract (see net_tcp.c / net_door NET_XFER_MAX):
 *   MSS 1024, RX/TX caps 4096 — one door SEND of 3000 B → ≥3 payload segs.
 *   Compile-time guards fail if multi-seg room shrinks below bulk smoke.
 *   Peer window (u16PeerWnd) soft-limits in-flight bytes across chunks.
 *   Loopback path requires full peer RX push per segment (no silent short
 *   fill) so multi-seg integrity holds across the whole door transfer.
 *
 * Poll integration:
 *   net_eth_poll → net_tcp_input (per TCP frame) then net_tcp_poll
 *   (rtx last unacked data seg + TIME_WAIT soft reap after TCP_TW_MS).
 *
 * Stats (accepts, segments, bytes_rx/tx, retransmits, tw_reaps):
 *   segs = TX segments + RX segments seen by net_tcp_input
 *   rtx  = successful last-segment retransmits from net_tcp_poll
 *   Exposed via net_door TCP_STATS and getters below.
 *
 * Greppable: multi-seg bulk / soft TIME_WAIT / listen backlog soft
 */
#pragma once

#include <gj/types.h>

/** Zero TCP table and soft stats. Safe to re-call. */
void net_tcp_init(void);

/**
 * socket(AF_INET, SOCK_STREAM)-shaped → fd or -errno.
 * EMFILE when table full. Domain fixed to IPv4 STREAM soft.
 */
i64 net_tcp_socket(void);

/** bind: local port (host order). EADDRINUSE soft without free slot/port. */
i64 net_tcp_bind(i64 i64Fd, u16 u16Port);

/** listen: store/clamp soft backlog for SYN/accept queue (1..TCP_BACKLOG_MAX). */
i64 net_tcp_listen(i64 i64Fd, int nBacklog);

/**
 * connect: loopback pair to local listener on port (SYN handshake soft).
 * -ECONNREFUSED / -EAGAIN-shaped when no listener or backlog full.
 */
i64 net_tcp_connect(i64 i64Fd, u16 u16Port);

/** accept: connected peer fd or -EAGAIN if none pending. */
i64 net_tcp_accept(i64 i64Fd);

/**
 * Send: multi-seg when cb > MSS; may short-write on window/RX pressure.
 * Cap per call aligns with door bounce (4 KiB). Allowed in CLOSE_WAIT soft
 * (send remaining data after peer FIN).
 */
i64 net_tcp_send(i64 i64Fd, const void *pBuf, size_t cb);

/**
 * Recv: short reads OK; drain loop for multi-seg bulk. 0 after peer FIN
 * once RX ring empty (EOF soft). -EAGAIN if non-ready empty soft.
 */
i64 net_tcp_recv(i64 i64Fd, void *pBuf, size_t cb);

/**
 * Close: soft FIN on ESTABLISHED/CLOSE_WAIT (virtio + loop peer half-close),
 * then free local slot when terminal. Peer TIME_WAIT reaped by net_tcp_poll.
 */
i64 net_tcp_close(i64 i64Fd);

/** Non-zero if fd is a live net_tcp table entry. */
int net_tcp_fd_ok(i64 i64Fd);

/**
 * Demux IPv4 TCP frame (full eth frame from net_eth_poll).
 * Returns 1 if consumed (ours), 0 if ignored / bad args.
 */
int net_tcp_input(const u8 *pFrame, u32 cb);

/**
 * Idle tick: retransmit last unacked data segment (TCP_RTX_MS / max)
 * + TIME_WAIT soft reap (TCP_TW_MS). Called from net_eth_poll.
 */
void net_tcp_poll(void);

/** Lifetime successful accepts (soft product counter). */
u32 net_tcp_accepts(void);
/** Lifetime TX+RX segments observed (soft). */
u32 net_tcp_segments(void);
/** Lifetime payload bytes received into RX rings. */
u32 net_tcp_bytes_rx(void);
/** Lifetime payload bytes accepted on send path. */
u32 net_tcp_bytes_tx(void);
/** Lifetime successful last-segment retransmits. */
u32 net_tcp_retransmits(void);
/** Soft TIME_WAIT reaps performed by net_tcp_poll. */
u32 net_tcp_tw_reaps(void);
