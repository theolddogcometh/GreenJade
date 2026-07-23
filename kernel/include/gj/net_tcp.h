/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Minimal IPv4 TCP over virtio-net + loopback pairs (sshd / netstackd).
 * Pure C, dual-licensed. Clean-room freestanding: SYN handshake, ordered
 * RX, multi-segment TX (MSS chunks), peer window soft limit, last-seg rtx.
 *
 * Sizing contract (see net_tcp.c / net_door NET_XFER_MAX):
 *   MSS 1024, RX/TX caps 4096 — one door SEND of 3000 B → ≥3 payload segs.
 */
#pragma once

#include <gj/types.h>

void net_tcp_init(void);

/** socket(AF_INET, SOCK_STREAM) → fd or -errno */
i64 net_tcp_socket(void);
i64 net_tcp_bind(i64 i64Fd, u16 u16Port);
i64 net_tcp_listen(i64 i64Fd, int nBacklog);
i64 net_tcp_connect(i64 i64Fd, u16 u16Port); /* loopback pair to local listener */
i64 net_tcp_accept(i64 i64Fd);
/**
 * Send: multi-seg when cb > MSS; may short-write on window/RX pressure.
 * Cap per call aligns with door bounce (4 KiB).
 */
i64 net_tcp_send(i64 i64Fd, const void *pBuf, size_t cb);
/** Recv: short reads OK; drain loop for multi-seg bulk. */
i64 net_tcp_recv(i64 i64Fd, void *pBuf, size_t cb);
i64 net_tcp_close(i64 i64Fd);
int net_tcp_fd_ok(i64 i64Fd);

/**
 * Demux IPv4 TCP frame (full eth frame). Called from net_eth_poll.
 * Returns 1 if consumed.
 */
int net_tcp_input(const u8 *pFrame, u32 cb);

/** Idle tick: retransmit last unacked data segment (virtio path). */
void net_tcp_poll(void);

u32 net_tcp_accepts(void);
u32 net_tcp_segments(void);
u32 net_tcp_bytes_rx(void);
u32 net_tcp_bytes_tx(void);
u32 net_tcp_retransmits(void);
