/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Minimal IPv4 TCP over virtio-net, UDX L2 (ETH_INJECT / ETH_TX_PULL),
 * and loopback pairs (sshd / netstackd).
 * Pure C11 freestanding, dual MIT OR Apache-2.0. Clean-room - no GPL stack.
 *
 * Features (bring-up product path):
 *   SYN handshake, ordered RX ring, multi-segment TX (MSS chunks),
 *   advertised peer window soft limit, last-segment retransmit on poll,
 *   soft close states (FIN_WAIT / LAST_ACK / TIME_WAIT), listen backlog.
 *
 * FD layout (avoid vfs_ram and net_lo ranges):
 *   FDs 96..111 -> 16 slots (TCP_MAX). net_tcp_fd_ok is the ownership test.
 *
 * Sizing contract (see net_tcp.c / net_door NET_XFER_MAX):
 *   MSS 1024, RX/TX caps 4096 - one door SEND of 3000 B -> >=3 payload segs.
 *   Compile-time guards fail if multi-seg room shrinks below bulk smoke.
 *   Peer window (u16PeerWnd) soft-limits in-flight bytes across chunks.
 *   Loopback path requires full peer RX push per segment (no silent short
 *   fill) so multi-seg integrity holds across the whole door transfer.
 *
 * Poll integration:
 *   net_eth_poll (run-loop / scheduler_run only - NEVER timer IRQ) ->
 *   net_tcp_input (per TCP frame) then net_tcp_poll
 *   (lean rtx SYN/SYN-ACK + last unacked data + TIME_WAIT soft reap).
 *   Cold poll/epoll readiness: net_tcp_poll_mask / net_tcp_fd_ok
 *   (POLLIN/OUT/ERR/HUP from RX, accept queue, write window, closed,
 *   soft half-close SHUT_RD/WR, sticky RST POLLERR).
 *
 * Userspace / ABI socket residual (lean; Soft!=product; G-AC-1):
 *   listen/accept/shutdown/name/rtx for cold Linux personality + sshd over
 *   stack (mirror net_lo shapes). Not freestanding rtl R0 deepen thrash.
 *   Product NIC = UDX+ABI. Soft listen :22 != interactive SSH login.
 *   Functional thrash-strip: lean rtx multi-pass only (no micro/nano/pico).
 *   listen: re-arm after SHUT_RD; soft→product AcceptQ transfer on :22.
 *   accept: ESTABLISHED FIFO; name-ready getsockname/getpeername.
 *   Half-close: SHUT_WR emit FIN + bare-FIN rtx on eth busy; send -> -EPIPE;
 *   SHUT_RD empty recv -> 0; listen SHUT_RD stops accept/SYN demux.
 *   Peer RST sticky POLLERR until close. Soft!=product · dual MIT/Apache.
 *
 * Stats (accepts, segments, bytes_rx/tx, retransmits, tw_reaps):
 *   segs = TX segments + RX segments seen by net_tcp_input
 *   rtx  = successful SYN/data retransmits from net_tcp_poll
 *   Exposed via net_door TCP_STATS and getters below.
 *
 * Greppable freestanding :22 (lab residual) - Soft!=product:
 *   net_tcp: soft listen :22 ...       - ensure listen after net_l2 ready
 *   net_tcp: soft honesty listen_not_banner - Soft!=product honesty lamp
 *   net_tcp: soft bind ... lab_ip=...  - rtl bind uses lab 10.200.125.50
 *   net_tcp: soft eth_syn / eth_estab  - passive SYN / handshake
 *   net_tcp: soft accept ...           - accept() minted peer fd (name-ready)
 * Greppable userspace/ABI socket residual - Soft!=product:
 *   net_tcp: soft shutdown ...         - half-close SHUT_RD/WR/RDWR + listen stop
 *   net_tcp: soft getsockname ...      - local sockaddr_in (L2 IP + port)
 *   net_tcp: soft getpeername ...      - peer sockaddr_in or -ENOTCONN
 *   net_tcp: soft socket path ...      - residual inventory lamp (once)
 *
 * Lean residual HARD:
 *   · no version/wave stamp; no TCP_SOFT_DEEPEN_WAVE reintroduction
 *   · no stamp storms (one-line lamps; event/poll hard caps)
 *   · no net_eth_poll / net_tcp_poll on IRQ (run-loop only)
 *   · dual MIT OR Apache-2.0 · Soft!=product · G-AC-1
 *   · prefer functional userspace/ABI residual; freestanding thrash SKIP
 *   · no micro/nano/pico multi-pass thrash; lean busy multi-pass only
 *   · W11 Dual DoD B FUNCTIONAL: wire handoff + :22 stack for product sshd;
 *     net_tcp_poll thr/door only (H1); Dual DoD OPEN; stamp-free bar
 *     v2026.08.04.75; never invent .76.
 * greppable: net_tcp: soft residual wire22 | wire_handoff+tcp22
 * greppable: stack=eth|tcp|door|:22 | W11 Dual DoD B FUNCTIONAL
 */
#pragma once

#include <gj/types.h>

/** Zero TCP table and soft stats. Safe to re-call. */
void net_tcp_init(void);

/**
 * socket(AF_INET, SOCK_STREAM)-shaped -> fd or -errno.
 * EMFILE when table full. Domain fixed to IPv4 STREAM soft.
 */
i64 net_tcp_socket(void);

/** bind: local port (host order). EADDRINUSE soft without free slot/port. */
i64 net_tcp_bind(i64 i64Fd, u16 u16Port);

/**
 * listen: store/clamp soft backlog for SYN/accept queue (1..TCP_BACKLOG_MAX).
 * Re-listen preserves pending AcceptQ; clears SHUT_RD stop re-arm.
 * Product :22 transfers soft-mint AcceptQ then frees soft. Soft!=product.
 * Soft listen :22 != host banner proof (honesty; G-AC-1).
 */
i64 net_tcp_listen(i64 i64Fd, int nBacklog);

/**
 * Hold/mint :22 listen after L2 ready (virtio, rtl, or UDX ETH_UDX_READY).
 * Product listen supersedes soft mint. Soft!=product. != host banner.
 * Call from ETH_UDX_READY so laptop wire (backend=none) is not virtio-only.
 * greppable: net_tcp: soft listen :22 | product_net_owns_wire
 */
void net_tcp_ensure_listen22(void);

/**
 * connect: loopback pair to local listener on port (SYN handshake soft).
 * -ECONNREFUSED / -EAGAIN-shaped when no listener or backlog full.
 */
i64 net_tcp_connect(i64 i64Fd, u16 u16Port);

/**
 * accept: connected peer fd or -EAGAIN if none pending.
 * ESTABLISHED AcceptQ only (FIFO oldest + eth prefer); SHUT_RD -> -EINVAL.
 * FIFO rehook oldest remaining ESTABLISHED; name-ready for name residual.
 * Soft!=product · sshd over stack.
 */
i64 net_tcp_accept(i64 i64Fd);

/**
 * Send: multi-seg when cb > MSS; may short-write on window/RX pressure.
 * Cap per call aligns with door bounce (4 KiB). Allowed in CLOSE_WAIT soft
 * (send remaining data after peer FIN). SHUT_WR / RST -> -EPIPE.
 */
i64 net_tcp_send(i64 i64Fd, const void *pBuf, size_t cb);

/**
 * Recv: short reads OK; drain loop for multi-seg bulk. 0 after peer FIN
 * once RX ring empty (EOF soft). SHUT_RD empty -> 0. -EAGAIN if empty soft.
 */
i64 net_tcp_recv(i64 i64Fd, void *pBuf, size_t cb);

/**
 * Close: soft FIN on ESTABLISHED/CLOSE_WAIT (virtio + loop peer half-close),
 * then free local slot when terminal. Peer TIME_WAIT reaped by net_tcp_poll.
 */
i64 net_tcp_close(i64 i64Fd);

/**
 * shutdown(how): soft half-close for userspace/ABI socket path.
 * how: 0=SHUT_RD, 1=SHUT_WR, 2=SHUT_RDWR (Linux-shaped).
 * SHUT_WR emits FIN on ESTABLISHED/CLOSE_WAIT when not yet FinSent;
 * eth busy soft-accepts FinSent + arms bare-FIN rtx for net_tcp_poll.
 * Listen SHUT_RD/RDWR: stop accept + SYN demux on this fd.
 * Soft!=product · G-AC-1 · dual MIT OR Apache-2.0.
 * greppable: net_tcp: soft shutdown
 */
i64 net_tcp_shutdown(i64 i64Fd, int nHow);

/**
 * getsockname: fill sockaddr_in (16 B) with AF_INET + local port (net order)
 * + live L2 IPv4 (lab/virtio; rtl forces lab). Loopback -> 127.0.0.1.
 * *pLen in/out; requires >=16. Soft!=product - userspace/ABI residual.
 * greppable: net_tcp: soft getsockname
 */
i64 net_tcp_getsockname(i64 i64Fd, void *pAddr, u32 *pLen);

/**
 * getpeername: fill sockaddr_in with peer port + peer IPv4 when connected;
 * -ENOTCONN (-107) when peer unknown (listen / AcceptQ / pre-connect CLOSED).
 * Loopback uses 127.0.0.1; eth uses aRip. Soft!=product.
 * greppable: net_tcp: soft getpeername
 */
i64 net_tcp_getpeername(i64 i64Fd, void *pAddr, u32 *pLen);

/**
 * Nonzero if fd is a live net_tcp socket.
 * Ownership test for FD range 96..111 (TCP_MAX slots).
 */
int net_tcp_fd_ok(i64 i64Fd);

/**
 * Return Linux-shaped POLLIN/POLLOUT/POLLERR/POLLHUP bits for a TCP fd,
 * or 0 if not a net_tcp fd.
 * Bits: POLLIN=0x1 POLLPRI=0x2 POLLOUT=0x4 POLLERR=0x8 POLLHUP=0x10
 * Readiness from RX ring, accept queue, write window, close states,
 * soft half-close (SHUT_RD EOF / SHUT_WR no OUT / listen SHUT_RD no accept),
 * sticky RST POLLERR. Cold poll/epoll path - no vfs_ram or protonrt.
 */
u32 net_tcp_poll_mask(i64 i64Fd, u32 u32Want);

/** 1 if any ESTABLISHED AcceptQ child is on TCP port 22. Dual DoD B OPEN. */
int net_tcp_acceptq_estab22(void);

/**
 * Demux IPv4 TCP frame (full eth frame from net_eth_poll run-loop).
 * Returns 1 if consumed (ours), 0 if ignored / bad args.
 *
 * Soft demux residual (Dual DoD B; Soft!=product):
 *   dest 10.200.125.50 always accepted (force lab identity) so host
 *   SYN is not dropped when L2 still surfaces QEMU 10.0.2.15 mid-handoff
 *   or after R0->RX return; rtl/UDX/lab-identity force + recheck for
 *   non-lab dest match; backend=none + ETH_UDX_READY is a live wire
 *   (not virtio-only) so ETH_INJECT SYN reaches :22 listen; any :22 to
 *   ours re-ensures soft/product listen after L2 ready before AcceptQ
 *   match (product claims :22 still wins);
 *   SHUT_RD listeners skipped. Soft listen != host banner proof.
 * Hot path: silent tallies only on accept/miss (no kprintf storm;
 * event-capped eth_syn/estab lamps only). Soft!=product · lean residual.
 */
int net_tcp_input(const u8 *pFrame, u32 cb);

/**
 * Idle tick: lean retransmit SYN/SYN-ACK + bare FIN + last unacked data
 * (TCP_RTX_MS steady; busy-armed / :22 never-landed -> immediate each poll;
 * :22 post-success TCP_RTX_MS/POST22_DIV; TCP_RTX_PASSES busy multi-pass with
 * :22 SYN_RCVD then ESTABLISHED preferred + BUSY_SHOTS same-pass retries;
 * freestanding micro/nano/pico thrash SKIP) + TIME_WAIT soft reap.
 * Soft ensure listen :22 after L2 ready (entry + post multi-pass only;
 * thrash-strip mid-pass ensure storm). Lab IP force on rtl. Demux accepts
 * dest lab 10.200.125.50. kprintf: one-shot / hard cap only; hot residual
 * silent. Called from net_eth_poll run-loop only - NEVER timer IRQ.
 * Soft!=product · G-AC-1 · product NIC=UDX+ABI.
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
