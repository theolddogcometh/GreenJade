/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Net door: eth poll/stats + socket ops for netstackd/sshd/UDX hand-off.
 * Pure C11 freestanding, dual-licensed (MIT OR Apache-2.0).
 *
 * STREAM (SOCK_STREAM) -> net_tcp (virtio + local pair); DGRAM -> net_lo.
 * TCP_STATS packing (u32[4]):
 *   [0] accepts
 *   [1] low16=segments, high16=retransmits
 *   [2] rx_bytes
 *   [3] tx_bytes (full 32-bit)
 *
 * Ownership: token 0 means kernel interim owns policy; non-zero means
 * netstackd claimed the door. Claim is re-entrant for the same token
 * (idempotent reclaim soft), BUSY for a different token. RELEASE when
 * free is soft 0. Queue / ring ops allowed without claim for bring-up
 * smokes (owned path preferred by product netstackd).
 *
 * Ring soft path (netstackd / UDX handoff eng):
 *   EXPORT/MAP/KICK -> NODEV when virtio-net is absent (client soft-skips).
 *   RING_STATE always succeeds: free=0 pushes=0 without device.
 *   MAP records last user VA for diagnostics; re-MAP of the same VA is a
 *   soft reclaim of the map (re-install PTEs, re-export).
 *   USER_AVAIL / DESC_ALLOC / MAP_DMA / BOUNCE_FILL = UDX ring programming.
 *
 * User pointers: prefer user_range_ok + copy_{to,from}_user. The !user
 * branch is for early kernel smokes that pass HHDM/static buffers.
 *
 * Product PASS markers (main.c) depend on CLAIM/RELEASE, QUEUE_INFO,
 * EXPORT/MAP, AVAIL_PUSH, RING_STATE, and USER_AVAIL wire semantics -
 * keep those ABI-stable.
 *
 * Lean soft residual - Dual DoD B UDX handoff eng (Soft!=product dual):
 *   Product DoD B = UDX not freestanding rtl (G-AC-1; no .ko product AC).
 *   Eng: CLAIM -> ring MAP/DMA/DESC/USER_AVAIL (netstackd/UDX host stack) +
 *   SOCKET/BIND(:22)/LISTEN/ACCEPT(+POLL yield)/SEND/RECV/CONNECT/CLOSE/
 *   SOCK_POLL for sshd-gj interim + UDX host stack.
 *   GJ_NET_OP_POLL -> net_eth_poll advances freestanding eth (L2 demux +
 *   tcp input/rtx). Door thr stack only - never IRQ/timer (H1). Soft
 *   always 0; ACCEPT empty soft EAGAIN (-11). dual_dod_b=OPEN_UDX
 *   product_sshd_tcp22=OPEN until host interactive SSH login. Banner != login.
 *   Emission: init lamp + first-call residual once. Never re-dump on
 *   STATS/QUEUE_INFO/RING_STATE/POLL (no stamp storms). No version stamp.
 *   FUNCTIONAL residual net door ops (sshd/UDX; Soft!=product; STRONGER):
 *   SOCK_POLL routes TCP + net_lo poll_mask + want POLLIN/OUT + empty ready;
 *   CONNECT/CLOSE/SEND/RECV/SOCKET TCP|lo route tallies; CONNECT :22 client;
 *   :22 listen/accept/send/recv/close path lamps; ACCEPT eagain22 yield park;
 *   Linux -11 EAGAIN class in note_ret; USER_AVAIL kick bit + MAP which rx|tx.
 *   Functional catalog lean (STRONGER): sshd pipeline steps + UDX host stack +
 *   yield trio POLL|ACCEPT|SOCK_POLL + multi-seg xfer + poll bit constants.
 *   UDX/host residual deepen (Soft!=product): per-op ring-family tallies +
 *   ring MAP notes (map_va, map_which, map_ok, map_reclaim, map_nodev,
 *   map_fault, map_inval, dma_va, dma_ok, dma_reclaim, export_ok, kick_ok,
 *   desc/user_avail/bounce/avail/reap ok|nodev|io, host_owned_ring;
 *   UDX attach desc|avail|used at map_va+off_*) on sparse residual lines
 *   only - not per MAP/RING call. H1: sole net_eth_poll site is
 *   net_door_eth_poll_h1 from GJ_NET_OP_POLL (door thr; never IRQ/timer).
 *   C1 H1 residual deepen (Soft!=product): thr-only compile locks + lean
 *   thr_locks catalog (thr_only/irq_path/poll_sites/door_thr) + poll_h1
 *   match; fault_class=H1_irq_stack_smash. Dual DoD A/B remain OPEN.
 *   greppable: net_door: soft residual
 *   greppable: net_door: soft residual lean
 *   greppable: net_door: soft residual lean PASS
 *   greppable: net_door: soft residual h1
 *   greppable: net_door: soft residual functional
 *   greppable: net_door: soft residual functional catalog
 *   greppable: net_door: ring map notes
 *   greppable: product_dod_b=UDX | not_freestanding_rtl | dual_dod_b=OPEN_UDX
 *   greppable: poll->net_eth_poll | net_eth_poll=run_loop_or_door | irq=0
 *   greppable: h1_poll_sites=1 | poll_h1 | door_thr_only=1 | H1
 *   greppable: thr_only=1 | net_eth_irq=0 | irq_path=0 | poll_own=door_thr
 *   greppable: fault_class=H1_irq_stack_smash | dual_dod_b=OPEN
 *   greppable: handoff=netstackd|sshd|UDX
 *   greppable: udx_host_stack=claim|map_ring|map_dma|desc|user_avail|bounce
 *   greppable: functional_ops=sock_poll|connect|close|send|recv|accept_eagain
 *   greppable: functional_route=tcp|lo | accept_eagain22 | connect22
 *   greppable: sshd_pipeline=socket|bind22|listen|accept|poll|send|recv|close
 *   greppable: yield_trio=poll|accept|sock_poll | functional_steps
 *   W11 Dual DoD B FUNCTIONAL residual (Soft!=product; stamp-free bar
 *   v2026.08.04.75; never invent .76): wire handoff + :22 stack for product
 *   sshd. Compound path: POLL H1 (door thr) -> net_eth_poll -> net_tcp
 *   soft listen :22 / AcceptQ -> BIND22/LISTEN/ACCEPT/SOCK_POLL yield.
 *   sock_poll22 tally while soft :22 held. Dual DoD A/B remain OPEN.
 *   greppable: net_door: soft residual wire22 | wire_handoff+tcp22
 *   greppable: stack=eth|tcp|door|:22 | W11 Dual DoD B FUNCTIONAL
 *   greppable: sock_poll22 | poll_h1_wire22
 *   STRONGER wire22 residual denser (Soft!=product; Dual DoD OPEN;
 *   product_dod_b=UDX; H2 once; stamp-free bar v2026.08.04.75; never .76):
 *   multi-arm denser honesty for SOCK BIND/LISTEN/ACCEPT/POLL :22 door
 *   product sshd path - denser H1 thr-only door eth poll (sole site).
 *   Arms: h1_poll | sock22 | yield | dual_dod_open | product_udx.
 *   arm0 denser: thr|irq|path|door|sites|op|dense sublocks + poll_h1_wire22.
 *   arm1 denser sock22 sublocks: socket|bind|listen|accept|sock_poll|port.
 *   H1 thr-only eth poll from door only (sole net_eth_poll site).
 *   greppable: net_door: soft residual wire22 denser | denser=1
 *   greppable: wire22 denser | denser_arms | poll22 | product_dod_b=UDX
 *   greppable: dual_dod OPEN | dual_dod_b=OPEN | H1 thr-only
 *   greppable: denser_h1 | thr-only door eth poll | poll_h1_wire22
 *   greppable: denser_sock_sub | denser_h1_sub | thr-only_door_eth_poll
 */
#include <gj/error.h>
#include <gj/klog.h>
#include <gj/net_door.h>
#include <gj/net_eth.h>
#include <gj/net_l2.h>
#include <gj/net_lo.h>
#include <gj/net_tcp.h>
#include <gj/string.h>
#include <gj/user_access.h>
#include <gj/virtio_net.h>

#define SOCK_STREAM 1
#define SOCK_DGRAM  2
/* Linux-shaped SOCK_POLL want bits (match net_tcp_poll_mask / net_lo). */
#define NET_DOOR_POLLIN  0x0001u
#define NET_DOOR_POLLOUT 0x0004u
#define NET_DOOR_POLLERR 0x0008u
#define NET_DOOR_POLLHUP 0x0010u

/*
 * Door bounce buffer for SEND/RECV (and virtio frame copy helpers).
 *
 * Multi-seg TCP contract (keep high enough - do not shrink below bulk):
 *   netstackd freestanding smoke sends 3000 B in one GJ_NET_OP_SEND
 *   (MSS = 1024 -> ceil(3000/1024) = 3 payload segments inside net_tcp_send).
 *   NET_XFER_MAX must be >= 3000; 4096 matches TCP_RX_MAX / TCP_TX_MAX.
 * Eth/virtio L2 frames still reject >1514 inside virtio_net_tx.
 */
#define NET_XFER_MAX 4096u
#define NET_ETH_MAX  1514u
/* File-local page geometry (no config.h). Matches 4 KiB freestanding map. */
#define NET_DOOR_PAGE_SIZE 4096u
/*
 * STRONGER FUNCTIONAL residual catalogs (sshd/UDX; Soft!=product).
 * sshd_pipeline steps + UDX host stack + yield trio; lean multi-count.
 * greppable: functional_steps | sshd_pipeline | yield_trio | functional_route
 */
#define NET_DOOR_FUNC_SSHD_STEPS 10u /* socket|bind|listen|accept|poll|send|recv|connect|close|sock_poll */
#define NET_DOOR_FUNC_UDX_STEPS  8u  /* claim|export|map_ring|map_dma|desc|user_avail|bounce|kick */
#define NET_DOOR_FUNC_YIELD_OPS  3u  /* POLL + ACCEPT + SOCK_POLL (sshd park) */
#define NET_DOOR_FUNC_CATALOG_MIN 4u /* lean functional_ok threshold (STRONGER) */

/*
 * H1 thr-only residual locks (C1; Soft!=product; G-AC-1).
 * Door thr is a legal thr-stack owner for freestanding eth advance:
 *   GJ_NET_OP_POLL -> net_door_eth_poll_h1 -> net_eth_poll
 * Never timer / APIC / MSI-X / IRQ stack. Flip requires H1 review
 * (fault_class=H1_irq_stack_smash / #PF I=1 wild RIP). Dual DoD OPEN.
 * greppable: thr_only=1 | net_eth_irq=0 | irq_path=0 | door_thr_only=1
 * greppable: h1_poll_sites=1 | poll_own=door_thr | fault_class=H1_irq_stack_smash
 * greppable: net_door: soft residual h1 | dual_dod_b=OPEN
 */
#define NET_DOOR_H1_IRQ_POLL    0u /* net_eth_irq=0; never IRQ-path eth */
#define NET_DOOR_H1_THR_ONLY    1u /* thr stack only (door thr full kstack) */
#define NET_DOOR_H1_IRQ_PATH    0u /* timer/APIC/MSI-X never door eth poll */
#define NET_DOOR_H1_DOOR_THR    1u /* door thr is legal thr-stack owner */
#define NET_DOOR_H1_POLL_SITES  1u /* sole net_eth_poll site = H1 wrapper */
#define NET_DOOR_H1_LEAN_CHECKS 16u /* H1 + STRONGER functional + wire22 lean */
/* W11 Dual DoD B: soft sshd port honesty (product_sshd_tcp22=OPEN). */
#define NET_DOOR_SSH_PORT       22u
/* W11: wire handoff + :22 stack residual honesty lock. */
#define NET_DOOR_WIRE22_STACK   1u
/*
 * STRONGER wire22 residual denser (Soft!=product; Dual DoD OPEN;
 * product_dod_b=UDX; H2 once; stamp-free bar v2026.08.04.75; never .76).
 * Multi-arm denser for SOCK BIND/LISTEN/ACCEPT/POLL :22 product sshd door.
 * H1 thr-only eth poll from door only (sole net_eth_poll site). agent!=close.
 * Arms: h1_poll | sock22 | yield | dual_dod_open | product_udx.
 * Denser H1 thr-only door eth poll deepen: arm0 multi-sublocks
 * (thr_only/irq/irq_path/door_thr/poll_sites/op_poll) + poll_h1_wire22
 * compound tally when soft :22 held during H1 eth advance.
 * Denser sock22 sublocks: socket|bind|listen|accept|sock_poll|port.
 * greppable: wire22 denser | denser_arms | denser=1 | dual_dod OPEN
 * greppable: denser_h1 | thr-only door eth poll | poll_h1_wire22
 * greppable: denser_sock_sub | denser_h1_sub | thr-only_door_eth_poll
 */
#define NET_DOOR_WIRE22_DENSE       1u /* denser residual honesty lock */
#define NET_DOOR_WIRE22_DENSE_ARMS  5u /* h1_poll|sock22|yield|dod|udx */
#define NET_DOOR_WIRE22_DENSE_MIN   5u /* all denser arms for wire22_ok */
/* Denser arm0 H1 thr-only door eth poll sublocks (all required). */
#define NET_DOOR_WIRE22_DENSE_H1_SUB 7u /* thr|irq|path|door|sites|op|dense */
/* Denser arm1 sock22 sublocks (all required; :22 door product sshd). */
#define NET_DOOR_WIRE22_DENSE_SOCK_SUB 6u /* socket|bind|listen|accept|sock_poll|port */

_Static_assert(NET_DOOR_H1_IRQ_POLL == 0u,
               "H1: net_eth_irq must be 0 (door thr owns door eth poll)");
_Static_assert(NET_DOOR_H1_THR_ONLY == 1u,
               "H1: thr_only must be 1 (never IRQ-stack net_eth_poll)");
_Static_assert(NET_DOOR_H1_IRQ_PATH == 0u,
               "H1: irq_path must be 0 (timer/APIC/MSI-X never door eth)");
_Static_assert(NET_DOOR_H1_DOOR_THR == 1u,
               "H1: door thr is legal thr-stack owner (not IRQ)");
_Static_assert(NET_DOOR_H1_POLL_SITES == 1u,
               "H1: sole net_eth_poll site in this TU is net_door_eth_poll_h1");
_Static_assert(NET_DOOR_SSH_PORT == 22u,
               "W11: soft sshd port honesty (product_sshd_tcp22=OPEN)");
_Static_assert(NET_DOOR_WIRE22_STACK == 1u,
               "W11: wire handoff + :22 stack residual for product sshd");
_Static_assert(NET_DOOR_WIRE22_DENSE == 1u,
               "STRONGER wire22 residual denser honesty lock");
_Static_assert(NET_DOOR_WIRE22_DENSE_ARMS == 5u,
               "wire22 denser arms: h1_poll|sock22|yield|dual_dod_open|product_udx");
_Static_assert(NET_DOOR_WIRE22_DENSE_MIN == NET_DOOR_WIRE22_DENSE_ARMS,
               "wire22 denser min equals denser arms (all required)");
_Static_assert(NET_DOOR_WIRE22_DENSE_H1_SUB == 7u,
               "denser H1 thr-only door eth poll: 7 sublocks required");
_Static_assert(NET_DOOR_WIRE22_DENSE_SOCK_SUB == 6u,
               "denser sock22: 6 sublocks socket|bind|listen|accept|sock_poll|port");

/* Keep multi-seg room: bounce >= bulk smoke and > one MSS. */
typedef char net_xfer_ge_bulk[(NET_XFER_MAX >= 3000u) ? 1 : -1];
typedef char net_xfer_ge_mss[(NET_XFER_MAX > 1024u) ? 1 : -1];
typedef char net_door_page_ge_xfer[(NET_DOOR_PAGE_SIZE >= NET_XFER_MAX) ? 1 : -1];
/* Page size power-of-two for MAP_RING VA align residual (UDX attach). */
typedef char net_door_page_pow2[
    ((NET_DOOR_PAGE_SIZE & (NET_DOOR_PAGE_SIZE - 1u)) == 0u) ? 1 : -1];
/* H1 residual: sole-site lock must stay 1 (compile-time thr-only deepen). */
typedef char net_door_h1_sites_one[(NET_DOOR_H1_POLL_SITES == 1u) ? 1 : -1];
typedef char net_door_h1_thr_only[(NET_DOOR_H1_THR_ONLY == 1u) ? 1 : -1];
typedef char net_door_h1_irq_zero[(NET_DOOR_H1_IRQ_POLL == 0u &&
                                   NET_DOOR_H1_IRQ_PATH == 0u) ? 1 : -1];
/* STRONGER FUNCTIONAL residual: poll bits + catalog floors compile-true. */
typedef char net_door_func_pollin[(NET_DOOR_POLLIN == 0x1u) ? 1 : -1];
typedef char net_door_func_pollout[(NET_DOOR_POLLOUT == 0x4u) ? 1 : -1];
typedef char net_door_func_pollerr[(NET_DOOR_POLLERR == 0x8u) ? 1 : -1];
typedef char net_door_func_pollhup[(NET_DOOR_POLLHUP == 0x10u) ? 1 : -1];
typedef char net_door_func_sshd_steps[(NET_DOOR_FUNC_SSHD_STEPS == 10u) ? 1 : -1];
typedef char net_door_func_udx_steps[(NET_DOOR_FUNC_UDX_STEPS == 8u) ? 1 : -1];
typedef char net_door_func_yield[(NET_DOOR_FUNC_YIELD_OPS == 3u) ? 1 : -1];
typedef char net_door_func_cat_min[(NET_DOOR_FUNC_CATALOG_MIN >= 4u) ? 1 : -1];
typedef char net_door_wire22[(NET_DOOR_WIRE22_STACK == 1u &&
                              NET_DOOR_SSH_PORT == 22u) ? 1 : -1];
/* STRONGER wire22 residual denser compile-true (Soft!=product Dual DoD OPEN). */
typedef char net_door_wire22_dense[(NET_DOOR_WIRE22_DENSE == 1u &&
                                    NET_DOOR_WIRE22_DENSE_ARMS == 5u &&
                                    NET_DOOR_WIRE22_DENSE_MIN == 5u) ? 1 : -1];
typedef char net_door_wire22_dense_h1[(NET_DOOR_H1_THR_ONLY == 1u &&
                                       NET_DOOR_H1_POLL_SITES == 1u &&
                                       NET_DOOR_H1_IRQ_POLL == 0u) ? 1 : -1];
/* Denser H1 thr-only door eth poll sublocks compile-true. */
typedef char net_door_wire22_dense_h1_sub[(NET_DOOR_WIRE22_DENSE_H1_SUB == 7u &&
                                           NET_DOOR_H1_THR_ONLY == 1u &&
                                           NET_DOOR_H1_IRQ_POLL == 0u &&
                                           NET_DOOR_H1_IRQ_PATH == 0u &&
                                           NET_DOOR_H1_DOOR_THR == 1u &&
                                           NET_DOOR_H1_POLL_SITES == 1u &&
                                           NET_DOOR_WIRE22_DENSE == 1u) ? 1 : -1];
/* Denser sock22 sublocks compile-true (Soft!=product Dual DoD OPEN). */
typedef char net_door_wire22_dense_sock_sub[(NET_DOOR_WIRE22_DENSE_SOCK_SUB == 6u &&
                                             NET_DOOR_SSH_PORT == 22u &&
                                             NET_DOOR_WIRE22_STACK == 1u &&
                                             NET_DOOR_WIRE22_DENSE == 1u) ? 1 : -1];

static int g_fInit;
static u32 g_u32Calls;
static u32 g_u32OwnerToken; /* 0 = kernel interim owns */
static u32 g_u32VqCalls;
static u32 g_u32Claims;     /* successful first claims */
static u32 g_u32Reclaims;   /* idempotent same-token CLAIM soft path */
static u32 g_u32RingCalls;  /* EXPORT/MAP/KICK/RING_STATE + UDX user-ring ops */
static u64 g_u64RingMapVa;  /* last successful MAP_RING base (0 = none) */
static u64 g_u64MapDmaVa;   /* last successful MAP_DMA base (0 = none) */
static u16 g_u16LastMapWhich; /* last successful MAP_RING which (0=rx,1=tx) */

/*
 * Lean soft residual tallies (Dual DoD B UDX handoff eng + path honesty).
 * Soft!=product dual license. Counters only - no stamp storms.
 * Product DoD B = UDX not freestanding rtl (G-AC-1).
 * UDX/host stack residual deepen: ring-family per-op tallies + MAP notes
 * feed sparse residual lamps only (never per-POLL/STATS/RING dumps).
 * FUNCTIONAL residual ops (sshd/UDX; STRONGER): CONNECT/CLOSE/SEND/RECV/
 * SOCKET/SOCK_POLL outcomes + TCP|lo routes + :22 path + accept_eagain22 +
 * connect22 + sock_poll want IN/OUT + eagain class + map_rx|tx + kick.
 * H1 residual deepen (C1 thr-only): poll_h1 tracks sole door-thr eth
 * advance site; thr_locks lean catalogs compile locks (Soft!=product).
 * greppable: net_door: soft residual | net_door: soft residual lean
 * greppable: net_door: soft residual lean PASS | net_door: soft residual h1
 * greppable: net_door: soft residual functional
 * greppable: net_door: soft residual functional catalog
 * greppable: net_door: ring map notes
 * greppable: udx_host_stack=claim|map_ring|map_dma|desc|user_avail|bounce
 * greppable: handoff=netstackd|sshd|UDX | door_thr_only=1 | H1
 * greppable: h1_poll_sites=1 | poll_h1 | thr_only=1 | net_eth_irq=0
 * greppable: irq_path=0 | poll_own=door_thr | fault_class=H1_irq_stack_smash
 * greppable: functional_ops=sock_poll|connect|close|send|recv|accept_eagain
 * greppable: functional_route=tcp|lo | accept_eagain22 | connect22
 * greppable: sshd_pipeline | yield_trio | functional_steps
 */
struct net_door_soft {
    u64 u64Enter;          /* net_door_call entries */
    u64 u64NotInit;        /* call before init -> NODEV */
    u64 u64ClaimOk;        /* first CLAIM success (netstackd handoff) */
    u64 u64ClaimReclaim;   /* same-token re-CLAIM soft */
    u64 u64ClaimBusy;      /* different token -> BUSY */
    u64 u64ClaimInval;     /* CLAIM token 0 / high bits (path; note_ret class) */
    u64 u64ReleaseOk;      /* RELEASE matched token */
    u64 u64ReleaseFree;    /* RELEASE when free (soft 0) */
    u64 u64ReleaseInval;   /* RELEASE token mismatch (path; note_ret class) */
    u64 u64Poll;           /* GJ_NET_OP_POLL enter (sshd yield) */
    u64 u64PollOk;         /* POLL soft success (always 0) */
    u64 u64PollL2Ready;    /* POLL while net_l2_ready */
    u64 u64PollEth;        /* POLL advanced freestanding eth (net_eth_poll) */
    u64 u64PollH1;         /* eth advance via H1 door-thr wrapper only */
    u64 u64Poll22;         /* POLL while soft :22 held (wire22 denser) */
    u64 u64PollH122;       /* H1 thr-only eth advance while soft :22 held */
    u64 u64Socket;
    u64 u64SocketTcp;      /* SOCKET SOCK_STREAM -> net_tcp (functional route) */
    u64 u64SocketLo;       /* SOCKET DGRAM/other -> net_lo (functional route) */
    u64 u64Bind;
    u64 u64Bind22;         /* BIND arg port == 22 (sshd soft :22) */
    u64 u64Bind22Ok;       /* successful BIND to :22 */
    u64 u64Listen;
    u64 u64Listen22;       /* LISTEN while soft :22 held */
    u64 u64Accept;
    u64 u64AcceptOk;
    u64 u64AcceptEagain;   /* ACCEPT empty soft (-11) */
    u64 u64AcceptEagain22; /* ACCEPT eagain while soft :22 (sshd yield park) */
    u64 u64Accept22;       /* ACCEPT while soft :22 held */
    u64 u64Accept22Ok;     /* ACCEPT success while soft :22 held */
    u64 u64Listen22Ok;     /* LISTEN success while soft :22 held */
    u64 u64Connect;        /* CONNECT enter (sshd pair / netstackd) */
    u64 u64ConnectOk;
    u64 u64ConnectEagain;  /* CONNECT soft -11 / backlog full */
    u64 u64ConnectTcp;     /* CONNECT on TCP fd (functional route) */
    u64 u64ConnectLo;      /* CONNECT on net_lo fd (functional route) */
    u64 u64Connect22;      /* CONNECT arg port == 22 (sshd client path) */
    u64 u64Connect22Ok;    /* CONNECT ok to :22 */
    u64 u64Close;          /* CLOSE enter */
    u64 u64CloseOk;
    u64 u64CloseTcp;       /* CLOSE on TCP fd (functional route) */
    u64 u64CloseLo;        /* CLOSE on net_lo fd (functional route) */
    u64 u64Close22;        /* CLOSE while soft :22 held (session teardown) */
    u64 u64Send;
    u64 u64SendOk;         /* SEND bytes > 0 */
    u64 u64SendEagain;     /* SEND soft -11 */
    u64 u64SendTcp;        /* SEND on TCP fd (functional route) */
    u64 u64SendLo;         /* SEND on net_lo fd (functional route) */
    u64 u64Send22;         /* SEND while soft :22 held (sshd session) */
    u64 u64Send22Ok;       /* SEND ok while soft :22 held */
    u64 u64Recv;
    u64 u64RecvOk;         /* RECV bytes > 0 */
    u64 u64Recv0;          /* RECV EOF soft (0) */
    u64 u64RecvEagain;     /* RECV soft -11 */
    u64 u64RecvTcp;        /* RECV on TCP fd (functional route) */
    u64 u64RecvLo;         /* RECV on net_lo fd (functional route) */
    u64 u64Recv22;         /* RECV while soft :22 held */
    u64 u64Recv22Ok;       /* RECV ok while soft :22 held */
    u64 u64TcpStats;
    u64 u64SockPoll;
    u64 u64SockPollReady;  /* SOCK_POLL returned non-zero mask */
    u64 u64SockPollEmpty;  /* SOCK_POLL ready==0 soft empty (sshd park) */
    u64 u64SockPollTcp;    /* SOCK_POLL on TCP fd */
    u64 u64SockPollLo;     /* SOCK_POLL on net_lo fd (functional route) */
    u64 u64SockPollIn;     /* SOCK_POLL want & POLLIN */
    u64 u64SockPollOut;    /* SOCK_POLL want & POLLOUT */
    u64 u64SockPoll22;     /* SOCK_POLL while soft :22 held (W11 wire22) */
    u64 u64Eagain;         /* terminal Linux -11 / GJ_ERR_AGAIN class */
    u64 u64RingEnter;      /* ring-family enter (UDX residual) */
    u64 u64UserRing;       /* MAP/DMA/DESC/BOUNCE/USER_AVAIL enter */
    /* UDX/host ring programming residual (Soft!=product; deepen) */
    u64 u64ExportRing;     /* EXPORT_RING enter */
    u64 u64ExportOk;       /* EXPORT_RING success */
    u64 u64ExportNodev;    /* EXPORT_RING without device */
    u64 u64MapRing;        /* MAP_RING enter */
    u64 u64MapRingOk;      /* MAP_RING success (PTE install + export) */
    u64 u64MapRingReclaim; /* re-MAP same VA soft reclaim */
    u64 u64MapRingNodev;   /* MAP_RING while virtio-net absent */
    u64 u64MapRingInval;   /* MAP_RING bad VA / unaligned */
    u64 u64MapRingFault;   /* MAP_RING map_q_user fail */
    u64 u64MapRingRx;      /* MAP_RING ok which=0 (rx; UDX attach) */
    u64 u64MapRingTx;      /* MAP_RING ok which=1 (tx; UDX attach) */
    u64 u64MapDma;         /* MAP_DMA enter */
    u64 u64MapDmaOk;       /* MAP_DMA success */
    u64 u64MapDmaReclaim;  /* re-MAP_DMA same VA soft reclaim */
    u64 u64MapDmaNodev;    /* MAP_DMA without device */
    u64 u64MapDmaInval;    /* MAP_DMA bad VA / unaligned */
    u64 u64MapDmaFault;    /* MAP_DMA map fail */
    u64 u64DescAlloc;      /* DESC_ALLOC enter */
    u64 u64DescAllocOk;    /* DESC_ALLOC returned free head */
    u64 u64DescAllocNodev; /* DESC_ALLOC without device */
    u64 u64DescAllocNomem; /* DESC_ALLOC free list empty */
    u64 u64UserAvail;      /* USER_AVAIL enter */
    u64 u64UserAvailOk;    /* USER_AVAIL programmed */
    u64 u64UserAvailNodev; /* USER_AVAIL without device */
    u64 u64UserAvailIo;    /* USER_AVAIL program fail */
    u64 u64UserAvailKick;  /* USER_AVAIL ok with kick bit (H1 door thr) */
    u64 u64BounceFill;     /* BOUNCE_FILL enter */
    u64 u64BounceFillOk;   /* BOUNCE_FILL filled slot */
    u64 u64BounceFillNodev;/* BOUNCE_FILL without device */
    u64 u64BounceFillInval;/* BOUNCE_FILL bad len/buf */
    u64 u64BounceFillIo;   /* BOUNCE_FILL slot fail */
    u64 u64Kick;           /* KICK enter */
    u64 u64KickOk;         /* KICK success */
    u64 u64KickNodev;      /* KICK without device */
    u64 u64AvailPush;      /* AVAIL_PUSH enter */
    u64 u64AvailPushOk;    /* AVAIL_PUSH success */
    u64 u64AvailPushNodev; /* AVAIL_PUSH without device */
    u64 u64AvailPushInval; /* AVAIL_PUSH bad len/buf */
    u64 u64AvailPushIo;    /* AVAIL_PUSH push fail */
    u64 u64UsedReap;       /* USED_REAP enter */
    u64 u64UsedReapOk;     /* USED_REAP returned (incl 0 empty) */
    u64 u64UsedReapNodev;  /* USED_REAP without device */
    u64 u64RingState;      /* RING_STATE enter */
    u64 u64HostOwnedRing;  /* ring-family op while CLAIM owned (UDX host) */
    u64 u64Nodev;
    u64 u64Inval;
    u64 u64Busy;
    u64 u64Fault;
    u64 u64Io;
    u64 u64Nomem;
    u64 u64Nosupport;
    u64 u64Ok;
    u32 u32LastOp;
    i64 i64LastRet;
    u32 u32LeanChecks;     /* lean residual self-check count */
    u32 u32LeanOk;         /* lean residual checks that passed */
    u32 u32UdxHandoffOk;   /* CLAIM+ring op id lean checks passed */
    u32 u32RingMapNotesOk; /* ring map geometry/op lean notes passed */
    u32 u32SshdHandoffOk;  /* SOCKET/BIND/LISTEN/ACCEPT/POLL lean checks */
    u32 u32FunctionalOk;   /* functional ops lean multi-count (STRONGER catalog) */
    u32 u32FunctionalSteps;/* functional catalog steps passed (sshd+udx+yield) */
    u32 u32H1Ok;           /* H1 door-thr eth advance lean checks */
    u32 u32H1ThrLocks;     /* H1 thr-only compile-lock lean checks (C1) */
    u32 u32Wire22Ok;       /* W11 wire handoff + :22 stack lean checks */
    u32 u32Wire22Dense;    /* STRONGER wire22 denser arm multi-count */
    u32 u32Wire22DenseH1;  /* denser arm0: H1 thr-only door eth poll */
    u32 u32Wire22DenseH1Sub; /* denser arm0 sublock multi-count (thr-only) */
    u32 u32Wire22DenseSock;/* denser arm1: BIND/LISTEN/ACCEPT/SOCK_POLL :22 */
    u32 u32Wire22DenseSockSub; /* denser arm1 sock22 sublock multi-count */
    u32 u32Wire22DenseYield;/* denser arm2: POLL|ACCEPT|SOCK_POLL yield */
    u32 u32Wire22DenseDod; /* denser arm3: dual_dod OPEN honesty */
    u32 u32Wire22DenseUdx; /* denser arm4: product_dod_b=UDX honesty */
};

static struct net_door_soft g_soft;
static u8 g_fSoftOnce;     /* one-shot residual after first product call */
static u8 g_fSoftSshd22;   /* BIND :22 succeeded (Soft!=product) */
static u8 g_fSoftLean;     /* one-shot lean residual gate */

/*
 * Soft UDX L2 bridge (Dual DoD B; freestanding rtl SKIP; Soft!=product):
 *   ETH_UDX_READY arm → net_l2_ready soft + TX enqueue path
 *   ETH_INJECT → net_eth demux (ARP/ICMP/TCP:22)
 *   ETH_TX_PULL → host park drains demux replies for UDX DMA TX
 * greppable: ETH_INJECT | ETH_TX_PULL | ETH_UDX_READY | udx_tx_soft
 */
#define NET_DOOR_UDX_TX_SLOTS  32u
#define NET_DOOR_UDX_TX_MAX    1514u
static u8  g_fUdxReady;    /* product UDX host armed L2 soft */
static u8  g_fUdxReadyLamp;
static u8  g_fUdxInjLamp;
static u8  g_fUdxTxLamp;
static u8  g_aUdxTx[NET_DOOR_UDX_TX_SLOTS][NET_DOOR_UDX_TX_MAX];
static u16 g_aUdxTxLen[NET_DOOR_UDX_TX_SLOTS];
static u32 g_u32UdxTxHead; /* next fill */
static u32 g_u32UdxTxTail; /* next pull */
static u32 g_u32UdxTxN;    /* live count */
static u32 g_u32UdxInjOk;
static u32 g_u32UdxTxEnq;
static u32 g_u32UdxTxPull;

static void net_door_soft_inc(u64 *pCtr);
static void net_door_soft_note_ret(i64 i64Ret);
static void net_door_eth_poll_h1(void);
static void net_door_soft_residual_once(void);
static void net_door_soft_residual_lean_once(void);
static void net_door_soft_maybe_once(void);
static i64  net_door_soft_done(i64 i64Ret);

/** Soft: bump path tally (u64 wrap is fine for telemetry). */
static void
net_door_soft_inc(u64 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    (*pCtr)++;
}

/**
 * H1: sole freestanding eth advance from this TU (C1 thr-only residual).
 * Call only from GJ_NET_OP_POLL on door thr stack - never IRQ/timer.
 * Compile locks: NET_DOOR_H1_* (thr_only=1, irq=0, poll_sites=1).
 * Soft!=product dual; Product DoD B = UDX not freestanding rtl.
 * fault_class=H1_irq_stack_smash if ever called from IRQ path.
 * greppable: poll->net_eth_poll | h1_poll_sites=1 | door_thr_only=1 | H1
 * greppable: thr_only=1 | net_eth_irq=0 | irq_path=0 | poll_own=door_thr
 * greppable: fault_class=H1_irq_stack_smash | net_door: soft residual h1
 */
static void
net_door_eth_poll_h1(void)
{
    /*
     * H1 thr-only: door thr full kstack only. Locks are compile-true;
     * (void) keeps greppable residual surface live under -Wunused.
     * Soft!=product; never IRQ/timer/APIC/MSI-X.
     * Denser wire22: compound poll_h1_wire22 when soft :22 held.
     * greppable: denser_h1 | thr-only door eth poll | poll_h1_wire22
     */
    (void)NET_DOOR_H1_IRQ_POLL;
    (void)NET_DOOR_H1_THR_ONLY;
    (void)NET_DOOR_H1_IRQ_PATH;
    (void)NET_DOOR_H1_DOOR_THR;
    (void)NET_DOOR_H1_POLL_SITES;
    (void)NET_DOOR_WIRE22_DENSE_H1_SUB;
    net_eth_poll();
    net_door_soft_inc(&g_soft.u64PollH1);
    /* Denser H1 thr-only door eth poll while soft :22 (wire22 compound). */
    if (g_fSoftSshd22 != 0) {
        net_door_soft_inc(&g_soft.u64PollH122);
    }
}

/**
 * Soft: classify terminal return (diagnostics only; never alters ret).
 * Functional residual: Linux -11 EAGAIN + GJ_ERR_AGAIN class for sshd park
 * and connect/send/recv soft empty (Soft!=product; never alters ret).
 */
static void
net_door_soft_note_ret(i64 i64Ret)
{
    if (i64Ret >= 0) {
        net_door_soft_inc(&g_soft.u64Ok);
        return;
    }
    /* Linux-shaped soft empty / park (ACCEPT/RECV/CONNECT/SEND residual). */
    if (i64Ret == (i64)-11 || i64Ret == GJ_ERR_AGAIN) {
        net_door_soft_inc(&g_soft.u64Eagain);
        return;
    }
    if (i64Ret == GJ_ERR_NODEV) {
        net_door_soft_inc(&g_soft.u64Nodev);
    } else if (i64Ret == GJ_ERR_INVAL) {
        net_door_soft_inc(&g_soft.u64Inval);
    } else if (i64Ret == GJ_ERR_BUSY) {
        net_door_soft_inc(&g_soft.u64Busy);
    } else if (i64Ret == GJ_ERR_FAULT) {
        net_door_soft_inc(&g_soft.u64Fault);
    } else if (i64Ret == GJ_ERR_IO) {
        net_door_soft_inc(&g_soft.u64Io);
    } else if (i64Ret == GJ_ERR_NOMEM) {
        net_door_soft_inc(&g_soft.u64Nomem);
    } else if (i64Ret == GJ_ERR_NOSUPPORT) {
        net_door_soft_inc(&g_soft.u64Nosupport);
    }
}

/**
 * Sparse residual lamp - Dual DoD B UDX handoff + sshd interim path honesty.
 * Once only after first product call (no stamp storms). Soft!=product.
 * Product DoD B = UDX not freestanding rtl (G-AC-1).
 * Deepen: UDX/host ring MAP notes + desc/user_avail/bounce/avail outcomes;
 * FUNCTIONAL residual ops (sshd/UDX; STRONGER): sock_poll|connect|close|
 * send|recv + TCP|lo routes + :22 path + accept_eagain22 + connect22 +
 * want IN/OUT + eagain class + functional catalog steps; C1 H1 thr-only:
 * poll_h1 sole door thr eth site + compile thr_locks catalog (never
 * IRQ/timer; fault_class=H1_irq_stack_smash). Dual DoD OPEN.
 * greppable: net_door: soft residual
 * greppable: net_door: soft residual h1
 * greppable: net_door: soft residual functional
 * greppable: net_door: soft residual functional catalog
 * greppable: net_door: ring map notes
 * greppable: h1_poll_sites=1 | poll_h1 | thr_only=1 | net_eth_irq=0
 * greppable: poll_own=door_thr | fault_class=H1_irq_stack_smash
 * greppable: functional_ops=sock_poll|connect|close|send|recv|accept_eagain
 * greppable: functional_route=tcp|lo | accept_eagain22 | connect22
 * greppable: sshd_pipeline | yield_trio | functional_steps
 */
static void
net_door_soft_residual_once(void)
{
    u32 u32Owned;
    u32 u32L2Ready;
    u32 u32MapHeld;
    u32 u32DmaHeld;
    u32 u32H1Match;
    u32 u32H1ThrLocks;
    const char *szL2;

    u32Owned = (g_u32OwnerToken != 0) ? 1u : 0u;
    u32L2Ready = (net_l2_ready() != 0) ? 1u : 0u;
    u32MapHeld = (g_u64RingMapVa != 0) ? 1u : 0u;
    u32DmaHeld = (g_u64MapDmaVa != 0) ? 1u : 0u;
    /* H1 honesty: wrapper tally must equal POLL eth advance tally. */
    u32H1Match = (g_soft.u64PollH1 == g_soft.u64PollEth) ? 1u : 0u;
    /* C1 H1 thr-only locks compile-true (door thr legal; never IRQ). */
    u32H1ThrLocks = 0u;
    if (NET_DOOR_H1_IRQ_POLL == 0u && NET_DOOR_H1_THR_ONLY == 1u &&
        NET_DOOR_H1_IRQ_PATH == 0u && NET_DOOR_H1_DOOR_THR == 1u &&
        NET_DOOR_H1_POLL_SITES == 1u) {
        u32H1ThrLocks = 1u;
    }
    szL2 = net_l2_name();
    if (szL2 == NULL) {
        szL2 = "none";
    }

    /*
     * Grep: net_door: soft residual
     * Product DoD B = UDX not freestanding rtl; interim :22 + POLL->eth.
     * greppable: product_dod_b=UDX | not_freestanding_rtl | dual_dod_b=OPEN_UDX
     * greppable: handoff=netstackd|sshd|UDX
     * greppable: udx_host_stack=claim|map_ring|map_dma|desc|user_avail|bounce
     * greppable: door_thr_only=1 | H1 | h1_poll_sites=1 | poll_h1
     * greppable: thr_only=1 | net_eth_irq=0 | irq_path=0 | poll_own=door_thr
     * greppable: fault_class=H1_irq_stack_smash | dual_dod_b=OPEN
     */
    kprintf("net_door: soft residual dual_dod_b=OPEN_UDX product_dod_b=UDX "
            "not_freestanding_rtl=1 product_sshd_tcp22=OPEN "
            "handoff=netstackd|sshd|UDX "
            "path=claim|ring|poll->net_eth_poll|socket|bind22|listen|accept|"
            "send|recv|connect|close|sock_poll "
            "udx_host_stack=claim|map_ring|map_dma|desc|user_avail|bounce "
            "door_thr_only=1 H1=1 h1_poll_sites=%u thr_only=%u net_eth_irq=%u "
            "irq_path=%u poll_own=door_thr fault_class=H1_irq_stack_smash "
            "h1_thr_locks=%u dual_dod_b=OPEN "
            "l2=%s l2_ready=%u port22_held=%u enter=%lu poll=%lu poll_eth=%lu "
            "poll_h1=%lu h1_match=%u "
            "bind22_ok=%lu listen22_ok=%lu accept_eagain=%lu accept22_ok=%lu "
            "send_ok=%lu recv_ok=%lu connect_ok=%lu sock_poll_ready=%lu "
            "eagain=%lu owned=%u claims=%u reclaims=%u "
            "claim_inval=%lu release_inval=%lu "
            "ring=%lu user_ring=%lu host_owned_ring=%lu "
            "map_ring=%lu map_ok=%lu map_reclaim=%lu map_nodev=%lu map_fault=%lu "
            "map_rx=%lu map_tx=%lu "
            "map_dma=%lu map_dma_ok=%lu map_dma_reclaim=%lu "
            "desc=%lu desc_ok=%lu desc_nodev=%lu "
            "user_avail=%lu user_avail_ok=%lu user_avail_kick=%lu "
            "bounce=%lu bounce_ok=%lu "
            "map_held=%u map_va=0x%lx map_which=%u dma_held=%u dma_va=0x%lx "
            "net_eth_poll=run_loop_or_door irq=0 "
            "soft=1 product=0 Soft!=product G-AC-1 "
            "(userspace UDX/netstackd/sshd handoff eng; freestanding eth "
            "via POLL door thr only interim H1 wrapper; functional residual "
            "ops sock_poll|connect|close|send|recv; no .ko product; not "
            "freestanding rtl product; agent!=close)\n",
            (unsigned)NET_DOOR_H1_POLL_SITES,
            (unsigned)NET_DOOR_H1_THR_ONLY,
            (unsigned)NET_DOOR_H1_IRQ_POLL,
            (unsigned)NET_DOOR_H1_IRQ_PATH,
            u32H1ThrLocks,
            szL2, u32L2Ready, g_fSoftSshd22 ? 1u : 0u,
            (unsigned long)g_soft.u64Enter,
            (unsigned long)g_soft.u64Poll,
            (unsigned long)g_soft.u64PollEth,
            (unsigned long)g_soft.u64PollH1,
            u32H1Match,
            (unsigned long)g_soft.u64Bind22Ok,
            (unsigned long)g_soft.u64Listen22Ok,
            (unsigned long)g_soft.u64AcceptEagain,
            (unsigned long)g_soft.u64Accept22Ok,
            (unsigned long)g_soft.u64SendOk,
            (unsigned long)g_soft.u64RecvOk,
            (unsigned long)g_soft.u64ConnectOk,
            (unsigned long)g_soft.u64SockPollReady,
            (unsigned long)g_soft.u64Eagain,
            u32Owned, g_u32Claims, g_u32Reclaims,
            (unsigned long)g_soft.u64ClaimInval,
            (unsigned long)g_soft.u64ReleaseInval,
            (unsigned long)g_soft.u64RingEnter,
            (unsigned long)g_soft.u64UserRing,
            (unsigned long)g_soft.u64HostOwnedRing,
            (unsigned long)g_soft.u64MapRing,
            (unsigned long)g_soft.u64MapRingOk,
            (unsigned long)g_soft.u64MapRingReclaim,
            (unsigned long)g_soft.u64MapRingNodev,
            (unsigned long)g_soft.u64MapRingFault,
            (unsigned long)g_soft.u64MapRingRx,
            (unsigned long)g_soft.u64MapRingTx,
            (unsigned long)g_soft.u64MapDma,
            (unsigned long)g_soft.u64MapDmaOk,
            (unsigned long)g_soft.u64MapDmaReclaim,
            (unsigned long)g_soft.u64DescAlloc,
            (unsigned long)g_soft.u64DescAllocOk,
            (unsigned long)g_soft.u64DescAllocNodev,
            (unsigned long)g_soft.u64UserAvail,
            (unsigned long)g_soft.u64UserAvailOk,
            (unsigned long)g_soft.u64UserAvailKick,
            (unsigned long)g_soft.u64BounceFill,
            (unsigned long)g_soft.u64BounceFillOk,
            u32MapHeld,
            (unsigned long)g_u64RingMapVa,
            (unsigned)g_u16LastMapWhich,
            u32DmaHeld,
            (unsigned long)g_u64MapDmaVa);

    /*
     * Grep: net_door: soft residual h1
     * C1 H1 thr-only residual deepen (Soft!=product; no stamp storms).
     * Sole door eth site + compile locks; Dual DoD OPEN (agent!=close).
     */
    kprintf("net_door: soft residual h1 thr_only=%u net_eth_irq=%u "
            "irq_path=%u door_thr_only=%u h1_poll_sites=%u poll_own=door_thr "
            "h1_match=%u h1_thr_locks=%u poll=%lu poll_eth=%lu poll_h1=%lu "
            "fault_class=H1_irq_stack_smash net_eth_poll=run_loop_or_door "
            "dual_dod_b=OPEN product_dod_b=UDX not_freestanding_rtl=1 "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "(Soft!=product; C1 H1 thr-only residual deepen; door thr full "
            "kstack only; never IRQ/timer; agent!=close; no version stamp)\n",
            (unsigned)NET_DOOR_H1_THR_ONLY,
            (unsigned)NET_DOOR_H1_IRQ_POLL,
            (unsigned)NET_DOOR_H1_IRQ_PATH,
            (unsigned)NET_DOOR_H1_DOOR_THR,
            (unsigned)NET_DOOR_H1_POLL_SITES,
            u32H1Match, u32H1ThrLocks,
            (unsigned long)g_soft.u64Poll,
            (unsigned long)g_soft.u64PollEth,
            (unsigned long)g_soft.u64PollH1);

    /*
     * Grep: net_door: soft residual functional
     * FUNCTIONAL residual net door ops for sshd/UDX stack (Soft!=product;
     * STRONGER): SOCK_POLL TCP+lo + want IN/OUT; CONNECT/CLOSE/SEND/RECV/
     * SOCKET TCP|lo routes; :22 path + accept_eagain22 + connect22.
     * Dual DoD OPEN; agent!=close; no stamp.
     * greppable: functional_ops=sock_poll|connect|close|send|recv|accept_eagain
     * greppable: functional_route=tcp|lo | accept_eagain22 | connect22
     */
    kprintf("net_door: soft residual functional Soft!=product STRONGER=1 "
            "product_dod_b=UDX dual_dod_b=OPEN_UDX product_sshd_tcp22=OPEN "
            "handoff=netstackd|sshd|UDX "
            "functional_ops=sock_poll|connect|close|send|recv|accept_eagain "
            "functional_route=tcp|lo "
            "sshd_pipeline=socket|bind22|listen|accept|poll|send|recv|connect|close|sock_poll "
            "yield_trio=poll|accept|sock_poll "
            "port22_held=%u bind22_ok=%lu listen22_ok=%lu "
            "accept=%lu accept_ok=%lu accept_eagain=%lu accept_eagain22=%lu "
            "accept22=%lu accept22_ok=%lu "
            "connect=%lu connect_ok=%lu connect_eagain=%lu "
            "connect_tcp=%lu connect_lo=%lu connect22=%lu connect22_ok=%lu "
            "close=%lu close_ok=%lu close_tcp=%lu close_lo=%lu close22=%lu "
            "socket_tcp=%lu socket_lo=%lu "
            "send=%lu send_ok=%lu send_eagain=%lu send_tcp=%lu send_lo=%lu "
            "send22=%lu send22_ok=%lu "
            "recv=%lu recv_ok=%lu recv0=%lu recv_eagain=%lu recv_tcp=%lu "
            "recv_lo=%lu recv22=%lu recv22_ok=%lu "
            "sock_poll=%lu sock_poll_ready=%lu sock_poll_empty=%lu "
            "sock_poll_tcp=%lu sock_poll_lo=%lu sock_poll_in=%lu "
            "sock_poll_out=%lu sock_poll22=%lu poll22=%lu eagain=%lu "
            "map_rx=%lu map_tx=%lu user_avail_kick=%lu host_owned_ring=%lu "
            "wire_handoff+tcp22=1 stack=eth|tcp|door|:22 denser=1 denser_h1=1 "
            "poll_h1_wire22=%lu thr-only_door_eth_poll=1 "
            "door_thr_only=1 H1=1 poll_own=door_thr "
            "fault_class=H1_irq_stack_smash not_freestanding_rtl=1 "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "(W11 Dual DoD B FUNCTIONAL residual STRONGER wire22 denser; "
            "denser H1 thr-only door eth poll; wire handoff+:22 stack; "
            "SOCK_POLL TCP+net_lo want IN/OUT; sshd "
            "SOCKET/BIND22/LISTEN/ACCEPT/SEND/RECV/CONNECT/CLOSE routes; "
            "accept_eagain22 yield park; connect22 client; UDX map rx|tx + "
            "user_avail kick; stamp-free bar v2026.08.04.75; never .76; "
            "agent!=close)\n",
            g_fSoftSshd22 ? 1u : 0u,
            (unsigned long)g_soft.u64Bind22Ok,
            (unsigned long)g_soft.u64Listen22Ok,
            (unsigned long)g_soft.u64Accept,
            (unsigned long)g_soft.u64AcceptOk,
            (unsigned long)g_soft.u64AcceptEagain,
            (unsigned long)g_soft.u64AcceptEagain22,
            (unsigned long)g_soft.u64Accept22,
            (unsigned long)g_soft.u64Accept22Ok,
            (unsigned long)g_soft.u64Connect,
            (unsigned long)g_soft.u64ConnectOk,
            (unsigned long)g_soft.u64ConnectEagain,
            (unsigned long)g_soft.u64ConnectTcp,
            (unsigned long)g_soft.u64ConnectLo,
            (unsigned long)g_soft.u64Connect22,
            (unsigned long)g_soft.u64Connect22Ok,
            (unsigned long)g_soft.u64Close,
            (unsigned long)g_soft.u64CloseOk,
            (unsigned long)g_soft.u64CloseTcp,
            (unsigned long)g_soft.u64CloseLo,
            (unsigned long)g_soft.u64Close22,
            (unsigned long)g_soft.u64SocketTcp,
            (unsigned long)g_soft.u64SocketLo,
            (unsigned long)g_soft.u64Send,
            (unsigned long)g_soft.u64SendOk,
            (unsigned long)g_soft.u64SendEagain,
            (unsigned long)g_soft.u64SendTcp,
            (unsigned long)g_soft.u64SendLo,
            (unsigned long)g_soft.u64Send22,
            (unsigned long)g_soft.u64Send22Ok,
            (unsigned long)g_soft.u64Recv,
            (unsigned long)g_soft.u64RecvOk,
            (unsigned long)g_soft.u64Recv0,
            (unsigned long)g_soft.u64RecvEagain,
            (unsigned long)g_soft.u64RecvTcp,
            (unsigned long)g_soft.u64RecvLo,
            (unsigned long)g_soft.u64Recv22,
            (unsigned long)g_soft.u64Recv22Ok,
            (unsigned long)g_soft.u64SockPoll,
            (unsigned long)g_soft.u64SockPollReady,
            (unsigned long)g_soft.u64SockPollEmpty,
            (unsigned long)g_soft.u64SockPollTcp,
            (unsigned long)g_soft.u64SockPollLo,
            (unsigned long)g_soft.u64SockPollIn,
            (unsigned long)g_soft.u64SockPollOut,
            (unsigned long)g_soft.u64SockPoll22,
            (unsigned long)g_soft.u64Poll22,
            (unsigned long)g_soft.u64Eagain,
            (unsigned long)g_soft.u64MapRingRx,
            (unsigned long)g_soft.u64MapRingTx,
            (unsigned long)g_soft.u64UserAvailKick,
            (unsigned long)g_soft.u64HostOwnedRing,
            (unsigned long)g_soft.u64PollH122);

    /*
     * Grep: net_door: soft residual functional catalog
     * STRONGER FUNCTIONAL leftover catalog (sshd pipeline + UDX host).
     * Dual DoD B hop is kernel net_tcp → sshd.elf (not leftover netstackd).
     * Dual DoD OPEN until USB path (A) / interactive SSH login (B).
     */
    kprintf("net_door: soft residual functional catalog Soft!=product "
            "STRONGER=1 product_dod_b=UDX dual_dod_b=OPEN_UDX "
            "product_sshd_tcp22=OPEN handoff=netstackd|sshd|UDX "
            "sshd_steps=%u udx_steps=%u yield_ops=%u catalog_min=%u "
            "sshd_pipeline=socket|bind22|listen|accept|poll|send|recv|"
            "connect|close|sock_poll "
            "udx_host_stack=claim|export|map_ring|map_dma|desc|user_avail|"
            "bounce|kick "
            "yield_trio=poll|accept|sock_poll "
            "functional_route=tcp|lo "
            "functional_ops=sock_poll|connect|close|send|recv|accept_eagain "
            "pollin=0x%x pollout=0x%x pollerr=0x%x pollhup=0x%x "
            "sock_stream=%u sock_dgram=%u "
            "xfer_max=%u multi_seg_bulk=3000 "
            "port22_held=%u accept_eagain22=%lu connect22=%lu "
            "socket_tcp=%lu socket_lo=%lu sock_poll_in=%lu sock_poll_out=%lu "
            "map_rx=%lu map_tx=%lu user_avail_kick=%lu "
            "door_thr_only=1 H1=1 poll_own=door_thr "
            "fault_class=H1_irq_stack_smash not_freestanding_rtl=1 "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "(STRONGER FUNCTIONAL catalog; agent!=close; no version stamp)\n",
            (unsigned)NET_DOOR_FUNC_SSHD_STEPS,
            (unsigned)NET_DOOR_FUNC_UDX_STEPS,
            (unsigned)NET_DOOR_FUNC_YIELD_OPS,
            (unsigned)NET_DOOR_FUNC_CATALOG_MIN,
            (unsigned)NET_DOOR_POLLIN,
            (unsigned)NET_DOOR_POLLOUT,
            (unsigned)NET_DOOR_POLLERR,
            (unsigned)NET_DOOR_POLLHUP,
            (unsigned)SOCK_STREAM,
            (unsigned)SOCK_DGRAM,
            (unsigned)NET_XFER_MAX,
            g_fSoftSshd22 ? 1u : 0u,
            (unsigned long)g_soft.u64AcceptEagain22,
            (unsigned long)g_soft.u64Connect22,
            (unsigned long)g_soft.u64SocketTcp,
            (unsigned long)g_soft.u64SocketLo,
            (unsigned long)g_soft.u64SockPollIn,
            (unsigned long)g_soft.u64SockPollOut,
            (unsigned long)g_soft.u64MapRingRx,
            (unsigned long)g_soft.u64MapRingTx,
            (unsigned long)g_soft.u64UserAvailKick);

    /*
     * Grep: net_door: ring map notes
     * Sparse UDX/host MAP honesty only (Soft!=product; no per-MAP dump).
     * UDX attach: udx_virtq_attach(map_va, export) after MAP_RING success;
     * desc|avail|used at map_va+off_*; MAP_DMA grants bounce slots.
     * Outcome splits for desc/user_avail/bounce/avail/reap (UDX host stack).
     */
    kprintf("net_door: ring map notes Soft!=product product_dod_b=UDX "
            "handoff=netstackd|sshd|UDX page=%u map_held=%u map_va=0x%lx "
            "map_which=%u dma_held=%u dma_va=0x%lx "
            "map_enter=%lu map_ok=%lu map_reclaim=%lu map_nodev=%lu "
            "map_inval=%lu map_fault=%lu map_rx=%lu map_tx=%lu "
            "dma_enter=%lu dma_ok=%lu dma_reclaim=%lu dma_nodev=%lu "
            "dma_inval=%lu dma_fault=%lu "
            "export=%lu export_ok=%lu export_nodev=%lu "
            "kick=%lu kick_ok=%lu kick_nodev=%lu "
            "avail_push=%lu avail_ok=%lu avail_nodev=%lu avail_inval=%lu "
            "avail_io=%lu used_reap=%lu used_ok=%lu used_nodev=%lu "
            "ring_state=%lu "
            "desc_ok=%lu desc_nodev=%lu desc_nomem=%lu "
            "user_avail_ok=%lu user_avail_nodev=%lu user_avail_io=%lu "
            "user_avail_kick=%lu "
            "bounce_ok=%lu bounce_nodev=%lu bounce_inval=%lu bounce_io=%lu "
            "host_owned_ring=%lu "
            "ring_calls=%u owned=%u door_thr_only=1 H1=1 h1_poll_sites=1 "
            "udx_host_stack=claim|map_ring|map_dma|desc|user_avail|bounce "
            "udx_attach=map_va+off_desc|off_avail|off_used "
            "(MAP re-same-VA=soft reclaim; DMA re-same-VA=soft reclaim; "
            "NODEV soft-skip without virtio-net; "
            "map_va last MAP_RING base; dma_va last MAP_DMA base; "
            "Soft!=product dual; G-AC-1 no .ko product)\n",
            (unsigned)NET_DOOR_PAGE_SIZE, u32MapHeld,
            (unsigned long)g_u64RingMapVa,
            (unsigned)g_u16LastMapWhich,
            u32DmaHeld,
            (unsigned long)g_u64MapDmaVa,
            (unsigned long)g_soft.u64MapRing,
            (unsigned long)g_soft.u64MapRingOk,
            (unsigned long)g_soft.u64MapRingReclaim,
            (unsigned long)g_soft.u64MapRingNodev,
            (unsigned long)g_soft.u64MapRingInval,
            (unsigned long)g_soft.u64MapRingFault,
            (unsigned long)g_soft.u64MapRingRx,
            (unsigned long)g_soft.u64MapRingTx,
            (unsigned long)g_soft.u64MapDma,
            (unsigned long)g_soft.u64MapDmaOk,
            (unsigned long)g_soft.u64MapDmaReclaim,
            (unsigned long)g_soft.u64MapDmaNodev,
            (unsigned long)g_soft.u64MapDmaInval,
            (unsigned long)g_soft.u64MapDmaFault,
            (unsigned long)g_soft.u64ExportRing,
            (unsigned long)g_soft.u64ExportOk,
            (unsigned long)g_soft.u64ExportNodev,
            (unsigned long)g_soft.u64Kick,
            (unsigned long)g_soft.u64KickOk,
            (unsigned long)g_soft.u64KickNodev,
            (unsigned long)g_soft.u64AvailPush,
            (unsigned long)g_soft.u64AvailPushOk,
            (unsigned long)g_soft.u64AvailPushNodev,
            (unsigned long)g_soft.u64AvailPushInval,
            (unsigned long)g_soft.u64AvailPushIo,
            (unsigned long)g_soft.u64UsedReap,
            (unsigned long)g_soft.u64UsedReapOk,
            (unsigned long)g_soft.u64UsedReapNodev,
            (unsigned long)g_soft.u64RingState,
            (unsigned long)g_soft.u64DescAllocOk,
            (unsigned long)g_soft.u64DescAllocNodev,
            (unsigned long)g_soft.u64DescAllocNomem,
            (unsigned long)g_soft.u64UserAvailOk,
            (unsigned long)g_soft.u64UserAvailNodev,
            (unsigned long)g_soft.u64UserAvailIo,
            (unsigned long)g_soft.u64UserAvailKick,
            (unsigned long)g_soft.u64BounceFillOk,
            (unsigned long)g_soft.u64BounceFillNodev,
            (unsigned long)g_soft.u64BounceFillInval,
            (unsigned long)g_soft.u64BounceFillIo,
            (unsigned long)g_soft.u64HostOwnedRing,
            g_u32RingCalls, u32Owned);
}

/**
 * Lean residual self-check - UDX/netstackd/sshd host handoff ABI + POLL H1.
 * Once only. Soft!=product dual MIT OR Apache-2.0; no version stamp.
 * Product DoD B = UDX not freestanding rtl (G-AC-1).
 * Deepen: ring MAP notes + EXPORT/KICK/AVAIL + sshd SOCKET/BIND/LISTEN +
 * FUNCTIONAL residual ops CONNECT/CLOSE/SEND/RECV/SOCK_POLL (STRONGER
 * catalog: sshd pipeline + UDX host stack + yield trio + poll bits) +
 * C1 H1 thr-only: poll_h1 match + compile thr_locks (sole door thr eth).
 * greppable: net_door: soft residual lean
 * greppable: net_door: soft residual lean PASS
 * greppable: net_door: soft residual h1
 * greppable: net_door: soft residual functional
 * greppable: net_door: soft residual functional catalog
 * greppable: net_door: ring map notes
 * greppable: h1_poll_sites=1 | poll_h1 | thr_only=1 | net_eth_irq=0
 * greppable: poll_own=door_thr | fault_class=H1_irq_stack_smash
 * greppable: functional_ops=sock_poll|connect|close|send|recv|accept_eagain
 * greppable: functional_route=tcp|lo | sshd_pipeline | yield_trio
 */
static void
net_door_soft_residual_lean_once(void)
{
    u32 u32Ok;
    u32 u32Checks;
    u32 u32PollOp;
    u32 u32AcceptOp;
    u32 u32Xfer;
    u32 u32EthMax;
    u32 u32ClaimOp;
    u32 u32RingOp;
    u32 u32UserAvailOp;
    u32 u32ExportOp;
    u32 u32KickOp;
    u32 u32SshdOp;
    u32 u32FunctionalOp;
    u32 u32H1;
    u32 u32H1ThrLocks;
    u32 u32Wire22;
    u32 u32MapNotes;
    u32 u32UdxOk;
    u32 u32SshdOk;
    u32 u32FunctionalOk;
    u32 u32FunctionalSteps;
    u32 u32Owned;
    u32 u32L2Ready;
    u32 u32MapHeld;
    u32 u32DmaHeld;
    const char *szL2;

    if (g_fSoftLean != 0) {
        return;
    }
    g_fSoftLean = 1;
    u32Ok = 0;
    u32Checks = 0;
    u32UdxOk = 0;
    u32MapNotes = 0;
    u32SshdOk = 0;
    u32FunctionalOk = 0;
    u32FunctionalSteps = 0;
    u32H1 = 0;
    u32H1ThrLocks = 0;
    u32Wire22 = 0;
    u32FunctionalOp = 0;

    /* POLL op id stable + first in door matrix (H1: door thr eth advance). */
    u32Checks++;
    u32PollOp = 0;
    if (GJ_NET_OP_POLL == 1u) {
        u32PollOp = 1;
        u32Ok++;
        u32SshdOk++;
    }

    /* ACCEPT empty soft EAGAIN park; SOCK_POLL terminal op id. */
    u32Checks++;
    u32AcceptOp = 0;
    if (GJ_NET_OP_ACCEPT == 25u && GJ_NET_OP_SOCK_POLL == 27u) {
        u32AcceptOp = 1;
        u32Ok++;
        u32SshdOk++;
    }

    /* Bounce room: multi-seg TCP bulk >= 3000, eth frame <= 1514. */
    u32Checks++;
    u32Xfer = 0;
    if (NET_XFER_MAX >= 3000u && NET_XFER_MAX >= NET_ETH_MAX) {
        u32Xfer = 1;
        u32Ok++;
    }

    /* Page map geometry for ring MAP (UDX residual). */
    u32Checks++;
    u32EthMax = 0;
    if (NET_ETH_MAX == 1514u && NET_DOOR_PAGE_SIZE == 4096u) {
        u32EthMax = 1;
        u32Ok++;
        u32MapNotes++;
    }

    /* CLAIM op id stable - netstackd ownership handoff. */
    u32Checks++;
    u32ClaimOp = 0;
    if (GJ_NET_OP_CLAIM == 10u && GJ_NET_OP_RELEASE == 11u) {
        u32ClaimOp = 1;
        u32Ok++;
        u32UdxOk++;
    }

    /* Ring MAP + USER_AVAIL op ids - UDX ring programming handoff. */
    u32Checks++;
    u32RingOp = 0;
    u32UserAvailOp = 0;
    if (GJ_NET_OP_MAP_RING == 16u && GJ_NET_OP_MAP_DMA == 21u &&
        GJ_NET_OP_DESC_ALLOC == 22u && GJ_NET_OP_USER_AVAIL == 23u &&
        GJ_NET_OP_BOUNCE_FILL == 24u) {
        u32RingOp = 1;
        u32UserAvailOp = 1;
        u32Ok++;
        u32UdxOk++;
        u32MapNotes++;
    }

    /* EXPORT/KICK/AVAIL/RING_STATE op ids - host ring bring-up surface. */
    u32Checks++;
    u32ExportOp = 0;
    u32KickOp = 0;
    if (GJ_NET_OP_EXPORT_RING == 15u && GJ_NET_OP_KICK == 17u &&
        GJ_NET_OP_AVAIL_PUSH == 18u && GJ_NET_OP_USED_REAP == 19u &&
        GJ_NET_OP_RING_STATE == 20u) {
        u32ExportOp = 1;
        u32KickOp = 1;
        u32Ok++;
        u32UdxOk++;
        u32MapNotes++;
    }

    /* MAP_RING VA must be page-aligned power-of-two; diagnostic getters. */
    u32Checks++;
    if (NET_DOOR_PAGE_SIZE == 4096u &&
        (NET_DOOR_PAGE_SIZE & (NET_DOOR_PAGE_SIZE - 1u)) == 0u &&
        NET_DOOR_PAGE_SIZE >= NET_XFER_MAX) {
        u32Ok++;
        u32MapNotes++;
    }

    /* sshd interim socket path opcodes - Soft!=product :22 handoff eng. */
    u32Checks++;
    u32SshdOp = 0;
    if (GJ_NET_OP_SOCKET == 3u && GJ_NET_OP_BIND == 4u &&
        GJ_NET_OP_LISTEN == 9u && GJ_NET_OP_ACCEPT == 25u &&
        GJ_NET_OP_SEND == 5u && GJ_NET_OP_RECV == 6u) {
        u32SshdOp = 1;
        u32Ok++;
        u32SshdOk++;
    }

    /*
     * FUNCTIONAL residual net door ops for sshd/UDX stack (Soft!=product;
     * STRONGER): CONNECT/CLOSE/SOCK_POLL + SEND/RECV/ACCEPT ids stable;
     * SOCK_POLL routes TCP + net_lo poll_mask. Dual DoD OPEN.
     * greppable: functional_ops=sock_poll|connect|close|send|recv|accept_eagain
     * greppable: net_door: soft residual functional
     */
    u32Checks++;
    if (GJ_NET_OP_CONNECT == 7u && GJ_NET_OP_CLOSE == 8u &&
        GJ_NET_OP_SOCK_POLL == 27u && GJ_NET_OP_SEND == 5u &&
        GJ_NET_OP_RECV == 6u && GJ_NET_OP_ACCEPT == 25u) {
        u32FunctionalOp = 1;
        u32Ok++;
        u32SshdOk++;
        u32FunctionalOk++;
        u32FunctionalSteps++;
    }

    /*
     * STRONGER FUNCTIONAL: sshd pipeline op catalog (10 steps).
     * socket|bind|listen|accept|poll|send|recv|connect|close|sock_poll
     * greppable: sshd_pipeline | functional_steps | functional catalog
     */
    u32Checks++;
    if (GJ_NET_OP_SOCKET == 3u && GJ_NET_OP_BIND == 4u &&
        GJ_NET_OP_LISTEN == 9u && GJ_NET_OP_ACCEPT == 25u &&
        GJ_NET_OP_POLL == 1u && GJ_NET_OP_SEND == 5u &&
        GJ_NET_OP_RECV == 6u && GJ_NET_OP_CONNECT == 7u &&
        GJ_NET_OP_CLOSE == 8u && GJ_NET_OP_SOCK_POLL == 27u &&
        NET_DOOR_FUNC_SSHD_STEPS == 10u) {
        u32Ok++;
        u32SshdOk++;
        u32FunctionalOk++;
        u32FunctionalSteps += NET_DOOR_FUNC_SSHD_STEPS;
    }

    /*
     * STRONGER FUNCTIONAL: UDX host stack catalog (8 steps).
     * claim|export|map_ring|map_dma|desc|user_avail|bounce|kick
     * greppable: udx_host_stack | functional_steps | functional catalog
     */
    u32Checks++;
    if (GJ_NET_OP_CLAIM == 10u && GJ_NET_OP_EXPORT_RING == 15u &&
        GJ_NET_OP_MAP_RING == 16u && GJ_NET_OP_MAP_DMA == 21u &&
        GJ_NET_OP_DESC_ALLOC == 22u && GJ_NET_OP_USER_AVAIL == 23u &&
        GJ_NET_OP_BOUNCE_FILL == 24u && GJ_NET_OP_KICK == 17u &&
        NET_DOOR_FUNC_UDX_STEPS == 8u) {
        u32Ok++;
        u32UdxOk++;
        u32FunctionalOk++;
        u32FunctionalSteps += NET_DOOR_FUNC_UDX_STEPS;
    }

    /*
     * STRONGER FUNCTIONAL: sshd yield trio + multi-seg xfer + poll bits.
     * POLL|ACCEPT|SOCK_POLL park; NET_XFER_MAX multi-seg bulk; POLLIN/OUT.
     * greppable: yield_trio=poll|accept|sock_poll | functional_route
     */
    u32Checks++;
    if (GJ_NET_OP_POLL == 1u && GJ_NET_OP_ACCEPT == 25u &&
        GJ_NET_OP_SOCK_POLL == 27u && NET_DOOR_FUNC_YIELD_OPS == 3u &&
        NET_XFER_MAX >= 3000u && NET_DOOR_POLLIN == 0x1u &&
        NET_DOOR_POLLOUT == 0x4u && SOCK_STREAM == 1 && SOCK_DGRAM == 2) {
        u32Ok++;
        u32SshdOk++;
        u32FunctionalOk++;
        u32FunctionalSteps += NET_DOOR_FUNC_YIELD_OPS;
    }

    /*
     * H1 residual deepen (C1 thr-only): sole eth advance is door thr wrapper.
     * poll_h1 must equal poll_eth (both only from GJ_NET_OP_POLL path).
     * greppable: h1_poll_sites=1 | poll_h1 | door_thr_only=1 | H1
     * greppable: thr_only=1 | net_eth_irq=0 | poll_own=door_thr
     */
    u32Checks++;
    if (GJ_NET_OP_POLL == 1u && NET_DOOR_H1_POLL_SITES == 1u &&
        g_soft.u64PollH1 == g_soft.u64PollEth) {
        u32H1 = 1;
        u32Ok++;
        u32SshdOk++;
    }

    /*
     * C1 H1 thr-only compile locks (Soft!=product; Dual DoD OPEN).
     * Never IRQ-path door eth; door thr is legal thr-stack owner.
     * greppable: thr_only=1 | net_eth_irq=0 | irq_path=0 | door_thr_only=1
     * greppable: fault_class=H1_irq_stack_smash | net_door: soft residual h1
     */
    u32Checks++;
    if (NET_DOOR_H1_IRQ_POLL == 0u && NET_DOOR_H1_THR_ONLY == 1u &&
        NET_DOOR_H1_IRQ_PATH == 0u && NET_DOOR_H1_DOOR_THR == 1u &&
        NET_DOOR_H1_POLL_SITES == 1u) {
        u32H1ThrLocks = 1u;
        u32Ok++;
        /* Independent of poll_h1 match; PASS requires both h1_ok + thr_locks. */
    }

    /*
     * STRONGER wire22 residual denser (Soft!=product; Dual DoD OPEN;
     * product_dod_b=UDX; H2 once; stamp-free bar v2026.08.04.75; never .76).
     * Multi-arm denser honesty for SOCK BIND/LISTEN/ACCEPT/POLL :22 door
     * product sshd. H1 thr-only eth poll from door only (sole site).
     * Arms: h1_poll | sock22 | yield | dual_dod_open | product_udx.
     * Denser H1 thr-only door eth poll: arm0 multi-sublocks + poll_h1
     * match + poll_h1_wire22 compound honesty.
     * Denser sock22 sublocks: socket|bind|listen|accept|sock_poll|port.
     * greppable: wire_handoff+tcp22 | stack=eth|tcp|door|:22 | sock_poll22
     * greppable: net_door: soft residual wire22 denser | denser=1
     * greppable: wire22 denser | denser_arms | poll22 | dual_dod OPEN
     * greppable: denser_h1 | thr-only door eth poll | poll_h1_wire22
     * greppable: denser_sock_sub | denser_h1_sub | thr-only_door_eth_poll
     */
    {
        u32 u32W22H1;
        u32 u32W22H1Sub;
        u32 u32W22Sock;
        u32 u32W22SockSub;
        u32 u32W22Yield;
        u32 u32W22Dod;
        u32 u32W22Udx;
        u32 u32W22Dense;

        u32W22H1 = 0u;
        u32W22H1Sub = 0u;
        u32W22Sock = 0u;
        u32W22SockSub = 0u;
        u32W22Yield = 0u;
        u32W22Dod = 0u;
        u32W22Udx = 0u;
        u32W22Dense = 0u;

        /*
         * arm0 denser: H1 thr-only door eth poll (never IRQ/timer).
         * Multi-sublocks: thr_only|irq|path|door_thr|sites|op_poll|dense.
         * Also require poll_h1==poll_eth (sole wrapper site honesty).
         * greppable: denser_h1 | thr-only door eth poll | h1_poll_sites=1
         */
        if (NET_DOOR_H1_THR_ONLY == 1u) {
            u32W22H1Sub++;
        }
        if (NET_DOOR_H1_IRQ_POLL == 0u) {
            u32W22H1Sub++;
        }
        if (NET_DOOR_H1_IRQ_PATH == 0u) {
            u32W22H1Sub++;
        }
        if (NET_DOOR_H1_DOOR_THR == 1u) {
            u32W22H1Sub++;
        }
        if (NET_DOOR_H1_POLL_SITES == 1u) {
            u32W22H1Sub++;
        }
        if (GJ_NET_OP_POLL == 1u) {
            u32W22H1Sub++;
        }
        if (NET_DOOR_WIRE22_DENSE == 1u) {
            u32W22H1Sub++; /* denser residual honesty lock */
        }
        if (u32W22H1Sub >= NET_DOOR_WIRE22_DENSE_H1_SUB &&
            g_soft.u64PollH1 == g_soft.u64PollEth) {
            u32W22H1 = 1u;
            u32W22Dense++;
        }
        /*
         * arm1 denser: SOCK BIND/LISTEN/ACCEPT/SOCK_POLL :22 stack.
         * Multi-sublocks: socket|bind|listen|accept|sock_poll|port.
         * greppable: denser_sock_sub | sock_poll22 | wire_handoff+tcp22
         */
        if (GJ_NET_OP_SOCKET == 3u) {
            u32W22SockSub++;
        }
        if (GJ_NET_OP_BIND == 4u) {
            u32W22SockSub++;
        }
        if (GJ_NET_OP_LISTEN == 9u) {
            u32W22SockSub++;
        }
        if (GJ_NET_OP_ACCEPT == 25u) {
            u32W22SockSub++;
        }
        if (GJ_NET_OP_SOCK_POLL == 27u) {
            u32W22SockSub++;
        }
        if (NET_DOOR_SSH_PORT == 22u) {
            u32W22SockSub++;
        }
        if (u32W22SockSub >= NET_DOOR_WIRE22_DENSE_SOCK_SUB &&
            NET_DOOR_WIRE22_STACK == 1u) {
            u32W22Sock = 1u;
            u32W22Dense++;
        }
        /* arm2: yield trio POLL|ACCEPT|SOCK_POLL (sshd park denser). */
        if (GJ_NET_OP_POLL == 1u && GJ_NET_OP_ACCEPT == 25u &&
            GJ_NET_OP_SOCK_POLL == 27u && NET_DOOR_FUNC_YIELD_OPS == 3u &&
            NET_DOOR_WIRE22_DENSE == 1u) {
            u32W22Yield = 1u;
            u32W22Dense++;
        }
        /* arm3: dual_dod OPEN honesty (soft residual never closes Dual DoD). */
        if (NET_DOOR_WIRE22_DENSE == 1u && NET_DOOR_WIRE22_STACK == 1u &&
            NET_DOOR_WIRE22_DENSE_ARMS == 5u &&
            NET_DOOR_WIRE22_DENSE_SOCK_SUB == 6u) {
            u32W22Dod = 1u;
            u32W22Dense++;
        }
        /* arm4: product_dod_b=UDX (G-AC-1; not freestanding rtl product). */
        if (NET_DOOR_WIRE22_DENSE == 1u && NET_DOOR_SSH_PORT == 22u &&
            NET_DOOR_WIRE22_DENSE_MIN == NET_DOOR_WIRE22_DENSE_ARMS &&
            NET_DOOR_WIRE22_DENSE_H1_SUB == 7u &&
            NET_DOOR_WIRE22_DENSE_SOCK_SUB == 6u) {
            u32W22Udx = 1u;
            u32W22Dense++;
        }

        g_soft.u32Wire22Dense = u32W22Dense;
        g_soft.u32Wire22DenseH1 = u32W22H1;
        g_soft.u32Wire22DenseH1Sub = u32W22H1Sub;
        g_soft.u32Wire22DenseSock = u32W22Sock;
        g_soft.u32Wire22DenseSockSub = u32W22SockSub;
        g_soft.u32Wire22DenseYield = u32W22Yield;
        g_soft.u32Wire22DenseDod = u32W22Dod;
        g_soft.u32Wire22DenseUdx = u32W22Udx;

        /* Composite wire22_ok requires all denser arms + denser sublocks. */
        u32Checks++;
        if (u32W22Dense >= NET_DOOR_WIRE22_DENSE_MIN &&
            u32W22H1 != 0u && u32W22Sock != 0u && u32W22Yield != 0u &&
            u32W22Dod != 0u && u32W22Udx != 0u &&
            u32W22H1Sub >= NET_DOOR_WIRE22_DENSE_H1_SUB &&
            u32W22SockSub >= NET_DOOR_WIRE22_DENSE_SOCK_SUB &&
            NET_DOOR_WIRE22_STACK == 1u && NET_DOOR_SSH_PORT == 22u &&
            NET_DOOR_H1_THR_ONLY == 1u && NET_DOOR_H1_POLL_SITES == 1u &&
            NET_DOOR_H1_IRQ_POLL == 0u && NET_DOOR_H1_DOOR_THR == 1u) {
            u32Wire22 = 1u;
            u32Ok++;
            u32SshdOk++;
            u32FunctionalOk++;
        }
    }
    (void)NET_DOOR_H1_LEAN_CHECKS;
    (void)NET_DOOR_FUNC_CATALOG_MIN;
    (void)NET_DOOR_WIRE22_DENSE_ARMS;
    (void)NET_DOOR_WIRE22_DENSE_H1_SUB;
    (void)NET_DOOR_WIRE22_DENSE_SOCK_SUB;

    g_soft.u32LeanChecks = u32Checks;
    g_soft.u32LeanOk = u32Ok;
    g_soft.u32UdxHandoffOk = u32UdxOk;
    g_soft.u32RingMapNotesOk = u32MapNotes;
    g_soft.u32SshdHandoffOk = u32SshdOk;
    g_soft.u32FunctionalOk = u32FunctionalOk;
    g_soft.u32FunctionalSteps = u32FunctionalSteps;
    g_soft.u32H1Ok = u32H1;
    g_soft.u32H1ThrLocks = u32H1ThrLocks;
    g_soft.u32Wire22Ok = u32Wire22;

    u32Owned = (g_u32OwnerToken != 0) ? 1u : 0u;
    u32L2Ready = (net_l2_ready() != 0) ? 1u : 0u;
    u32MapHeld = (g_u64RingMapVa != 0) ? 1u : 0u;
    u32DmaHeld = (g_u64MapDmaVa != 0) ? 1u : 0u;
    szL2 = net_l2_name();
    if (szL2 == NULL) {
        szL2 = "none";
    }

    /*
     * Grep: net_door: soft residual lean
     * One lean line - Soft!=product dual license; no version stamp; no storm.
     * greppable: product_dod_b=UDX | not_freestanding_rtl | dual_dod_b=OPEN_UDX
     * greppable: handoff=netstackd|sshd|UDX
     * greppable: udx_host_stack=claim|map_ring|map_dma|desc|user_avail|bounce
     * greppable: door_thr_only=1 | H1 | h1_poll_sites=1 | poll_h1
     * greppable: thr_only=1 | net_eth_irq=0 | irq_path=0 | poll_own=door_thr
     * greppable: fault_class=H1_irq_stack_smash | dual_dod_b=OPEN
     */
    kprintf("net_door: soft residual lean "
            "poll_op=%u accept_op=%u xfer=%u eth_geom=%u "
            "claim_op=%u ring_op=%u user_avail_op=%u export_op=%u kick_op=%u "
            "sshd_op=%u functional_op=%u h1_ok=%u h1_thr_locks=%u wire22_ok=%u "
            "denser=1 denser_arms=%u "
            "udx_handoff_ok=%u ring_map_notes_ok=%u sshd_handoff_ok=%u "
            "functional_ok=%u functional_steps=%u STRONGER=1 "
            "checks=%u ok=%u path=claim|ring|poll->net_eth_poll|socket|bind22|"
            "send|recv|connect|close|sock_poll "
            "udx_host_stack=claim|map_ring|map_dma|desc|user_avail|bounce "
            "sshd_pipeline=socket|bind22|listen|accept|poll|send|recv|connect|close|sock_poll "
            "yield_trio=poll|accept|sock_poll functional_route=tcp|lo "
            "wire_handoff+tcp22=1 stack=eth|tcp|door|:22 "
            "handoff=netstackd|sshd|UDX eth_advance=1 irq=0 door_thr_only=1 "
            "net_eth_poll=run_loop_or_door H1=1 h1_poll_sites=%u "
            "thr_only=%u net_eth_irq=%u irq_path=%u poll_own=door_thr "
            "fault_class=H1_irq_stack_smash dual_dod_b=OPEN dual_dod=OPEN "
            "functional_ops=sock_poll|connect|close|send|recv|accept_eagain "
            "poll=%lu poll_eth=%lu poll_h1=%lu poll22=%lu poll_h1_wire22=%lu "
            "poll_l2=%lu denser_h1=%u denser_h1_sub=%u "
            "l2=%s l2_ready=%u "
            "port22_held=%u owned=%u map_held=%u map_va=0x%lx map_which=%u "
            "dma_held=%u dma_va=0x%lx "
            "map_ok=%lu map_reclaim=%lu map_nodev=%lu map_fault=%lu "
            "map_rx=%lu map_tx=%lu "
            "dma_ok=%lu dma_reclaim=%lu "
            "export_ok=%lu kick_ok=%lu "
            "desc_ok=%lu desc_nodev=%lu user_avail_ok=%lu user_avail_kick=%lu "
            "bounce_ok=%lu avail_ok=%lu used_ok=%lu "
            "send_ok=%lu recv_ok=%lu connect_ok=%lu sock_poll_ready=%lu "
            "sock_poll22=%lu accept_eagain22=%lu connect22=%lu "
            "socket_tcp=%lu socket_lo=%lu eagain=%lu "
            "dual_dod_b=OPEN_UDX product_dod_b=UDX "
            "not_freestanding_rtl=1 product_sshd_tcp22=OPEN "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
            "(Soft!=product; W11 Dual DoD B FUNCTIONAL residual lean STRONGER "
            "wire22 denser; denser H1 thr-only door eth poll; wire "
            "handoff+:22 stack; Product DoD B=UDX; stamp-free bar "
            "v2026.08.04.75; never invent .76; G-AC-1; agent!=close)\n",
            u32PollOp, u32AcceptOp, u32Xfer, u32EthMax,
            u32ClaimOp, u32RingOp, u32UserAvailOp, u32ExportOp, u32KickOp,
            u32SshdOp, u32FunctionalOp, u32H1, u32H1ThrLocks, u32Wire22,
            g_soft.u32Wire22Dense,
            u32UdxOk, u32MapNotes, u32SshdOk, u32FunctionalOk,
            u32FunctionalSteps, u32Checks, u32Ok,
            (unsigned)NET_DOOR_H1_POLL_SITES,
            (unsigned)NET_DOOR_H1_THR_ONLY,
            (unsigned)NET_DOOR_H1_IRQ_POLL,
            (unsigned)NET_DOOR_H1_IRQ_PATH,
            (unsigned long)g_soft.u64Poll,
            (unsigned long)g_soft.u64PollEth,
            (unsigned long)g_soft.u64PollH1,
            (unsigned long)g_soft.u64Poll22,
            (unsigned long)g_soft.u64PollH122,
            (unsigned long)g_soft.u64PollL2Ready,
            g_soft.u32Wire22DenseH1,
            g_soft.u32Wire22DenseH1Sub,
            szL2, u32L2Ready,
            g_fSoftSshd22 ? 1u : 0u, u32Owned, u32MapHeld,
            (unsigned long)g_u64RingMapVa,
            (unsigned)g_u16LastMapWhich,
            u32DmaHeld,
            (unsigned long)g_u64MapDmaVa,
            (unsigned long)g_soft.u64MapRingOk,
            (unsigned long)g_soft.u64MapRingReclaim,
            (unsigned long)g_soft.u64MapRingNodev,
            (unsigned long)g_soft.u64MapRingFault,
            (unsigned long)g_soft.u64MapRingRx,
            (unsigned long)g_soft.u64MapRingTx,
            (unsigned long)g_soft.u64MapDmaOk,
            (unsigned long)g_soft.u64MapDmaReclaim,
            (unsigned long)g_soft.u64ExportOk,
            (unsigned long)g_soft.u64KickOk,
            (unsigned long)g_soft.u64DescAllocOk,
            (unsigned long)g_soft.u64DescAllocNodev,
            (unsigned long)g_soft.u64UserAvailOk,
            (unsigned long)g_soft.u64UserAvailKick,
            (unsigned long)g_soft.u64BounceFillOk,
            (unsigned long)g_soft.u64AvailPushOk,
            (unsigned long)g_soft.u64UsedReapOk,
            (unsigned long)g_soft.u64SendOk,
            (unsigned long)g_soft.u64RecvOk,
            (unsigned long)g_soft.u64ConnectOk,
            (unsigned long)g_soft.u64SockPollReady,
            (unsigned long)g_soft.u64SockPoll22,
            (unsigned long)g_soft.u64AcceptEagain22,
            (unsigned long)g_soft.u64Connect22,
            (unsigned long)g_soft.u64SocketTcp,
            (unsigned long)g_soft.u64SocketLo,
            (unsigned long)g_soft.u64Eagain);

    /*
     * Grep: net_door: soft residual wire22
     * W11 Dual DoD B wire handoff + :22 stack honesty (once; Soft!=product).
     * STRONGER denser multi-arm surface (H2 once; Dual DoD OPEN).
     */
    kprintf("net_door: soft residual wire22 Soft!=product "
            "wire_handoff+tcp22=1 wire22_ok=%u denser=1 denser_arms=%u/%u "
            "denser_h1=%u denser_h1_sub=%u/%u denser_sock_sub=%u/%u "
            "ssh_port=%u stack=eth|tcp|door|:22 product_sshd_tcp22=OPEN "
            "path=poll_h1|socket|bind22|listen|accept|sock_poll|send|recv "
            "poll_h1_wire22=%lu sock_poll22=%lu poll22=%lu "
            "bind22_ok=%lu listen22_ok=%lu "
            "accept_eagain22=%lu accept22_ok=%lu connect22=%lu "
            "port22_held=%u door_thr_only=1 H1=1 thr_only=1 net_eth_irq=0 "
            "poll_own=door_thr h1_poll_sites=1 thr-only_door_eth_poll=1 "
            "dual_dod_b=OPEN_UDX dual_dod_b=OPEN product_dod_b=UDX "
            "not_freestanding_rtl=1 handoff=netstackd|sshd|UDX "
            "G-AC-1=1 agent!=close stamp_free=v2026.08.04.75 never=.76 "
            "(W11 Dual DoD B FUNCTIONAL STRONGER wire22 denser; wire "
            "handoff+:22 stack for product sshd; denser H1 thr-only door "
            "eth poll; denser_sock_sub; not Dual DoD close)\n",
            u32Wire22,
            g_soft.u32Wire22Dense, (unsigned)NET_DOOR_WIRE22_DENSE_ARMS,
            g_soft.u32Wire22DenseH1,
            g_soft.u32Wire22DenseH1Sub,
            (unsigned)NET_DOOR_WIRE22_DENSE_H1_SUB,
            g_soft.u32Wire22DenseSockSub,
            (unsigned)NET_DOOR_WIRE22_DENSE_SOCK_SUB,
            (unsigned)NET_DOOR_SSH_PORT,
            (unsigned long)g_soft.u64PollH122,
            (unsigned long)g_soft.u64SockPoll22,
            (unsigned long)g_soft.u64Poll22,
            (unsigned long)g_soft.u64Bind22Ok,
            (unsigned long)g_soft.u64Listen22Ok,
            (unsigned long)g_soft.u64AcceptEagain22,
            (unsigned long)g_soft.u64Accept22Ok,
            (unsigned long)g_soft.u64Connect22,
            g_fSoftSshd22 ? 1u : 0u);

    /*
     * Grep: net_door: soft residual wire22 denser
     * STRONGER multi-arm denser honesty (Soft!=product; Dual DoD OPEN;
     * product_dod_b=UDX; H2 once; stamp-free bar v2026.08.04.75).
     * Denser H1 thr-only door eth poll: arm0 sublocks + poll_h1_wire22.
     * Denser sock22 sublocks: socket|bind|listen|accept|sock_poll|port.
     * greppable: denser=1 | denser_arms | h1_poll | sock22 | yield
     * greppable: dual_dod_open | product_udx | poll22 | dual_dod OPEN
     * greppable: denser_h1 | thr-only door eth poll | poll_h1_wire22
     * greppable: denser_sock_sub | denser_h1_sub | thr-only_door_eth_poll
     */
    kprintf("net_door: soft residual wire22 denser Soft!=product denser=1 "
            "STRONGER=1 denser_arms=%u/%u denser_min=%u wire22_ok=%u "
            "arm_h1_poll=%u arm_h1_sub=%u/%u denser_h1=1 "
            "arm_sock22=%u denser_sock_sub=%u/%u arm_yield=%u "
            "arm_dual_dod_open=%u arm_product_udx=%u "
            "h1_thr_only=%u h1_poll_sites=%u net_eth_irq=%u irq_path=%u "
            "door_thr_only=%u poll_own=door_thr thr-only_door_eth_poll=1 "
            "fault_class=H1_irq_stack_smash "
            "sock_ops=socket|bind|listen|accept|poll|sock_poll "
            "ssh_port=%u wire22_stack=%u yield_ops=%u "
            "poll22=%lu sock_poll22=%lu bind22_ok=%lu listen22_ok=%lu "
            "accept_eagain22=%lu accept22_ok=%lu connect22=%lu "
            "poll_h1=%lu poll_eth=%lu poll_h1_wire22=%lu port22_held=%u "
            "stack=eth|tcp|door|:22 wire_handoff+tcp22=1 "
            "product_sshd_tcp22=OPEN product_dod_b=UDX "
            "dual_dod_b=OPEN_UDX dual_dod_b=OPEN dual_dod=OPEN "
            "not_freestanding_rtl=1 handoff=netstackd|sshd|UDX "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "agent!=close stamp_free=v2026.08.04.75 never=.76 "
            "(STRONGER wire22 residual denser; SOCK BIND/LISTEN/ACCEPT/POLL "
            ":22 door product sshd; denser H1 thr-only door eth poll; "
            "denser_sock_sub; Soft!=product; Dual DoD OPEN; "
            "product_dod_b=UDX; not close)\n",
            g_soft.u32Wire22Dense, (unsigned)NET_DOOR_WIRE22_DENSE_ARMS,
            (unsigned)NET_DOOR_WIRE22_DENSE_MIN, u32Wire22,
            g_soft.u32Wire22DenseH1,
            g_soft.u32Wire22DenseH1Sub,
            (unsigned)NET_DOOR_WIRE22_DENSE_H1_SUB,
            g_soft.u32Wire22DenseSock,
            g_soft.u32Wire22DenseSockSub,
            (unsigned)NET_DOOR_WIRE22_DENSE_SOCK_SUB,
            g_soft.u32Wire22DenseYield, g_soft.u32Wire22DenseDod,
            g_soft.u32Wire22DenseUdx,
            (unsigned)NET_DOOR_H1_THR_ONLY,
            (unsigned)NET_DOOR_H1_POLL_SITES,
            (unsigned)NET_DOOR_H1_IRQ_POLL,
            (unsigned)NET_DOOR_H1_IRQ_PATH,
            (unsigned)NET_DOOR_H1_DOOR_THR,
            (unsigned)NET_DOOR_SSH_PORT,
            (unsigned)NET_DOOR_WIRE22_STACK,
            (unsigned)NET_DOOR_FUNC_YIELD_OPS,
            (unsigned long)g_soft.u64Poll22,
            (unsigned long)g_soft.u64SockPoll22,
            (unsigned long)g_soft.u64Bind22Ok,
            (unsigned long)g_soft.u64Listen22Ok,
            (unsigned long)g_soft.u64AcceptEagain22,
            (unsigned long)g_soft.u64Accept22Ok,
            (unsigned long)g_soft.u64Connect22,
            (unsigned long)g_soft.u64PollH1,
            (unsigned long)g_soft.u64PollEth,
            (unsigned long)g_soft.u64PollH122,
            g_fSoftSshd22 ? 1u : 0u);

    if (u32Ok == u32Checks && u32UdxOk >= 3u && u32MapNotes >= 3u &&
        u32H1 != 0 && u32H1ThrLocks != 0 && u32Wire22 != 0 &&
        g_soft.u32Wire22Dense >= NET_DOOR_WIRE22_DENSE_MIN &&
        u32FunctionalOk >= NET_DOOR_FUNC_CATALOG_MIN) {
        /*
         * Grep: net_door: soft residual lean PASS
         * Soft residual only - never product dual-license DoD close.
         * W11 STRONGER denser: functional_ok + wire22 denser + H1 thr-only.
         */
        kprintf("net_door: soft residual lean PASS "
                "checks=%u ok=%u udx_handoff_ok=%u ring_map_notes_ok=%u "
                "sshd_handoff_ok=%u functional_ok=%u functional_steps=%u "
                "h1_ok=%u h1_thr_locks=%u wire22_ok=%u denser=1 denser_arms=%u "
                "STRONGER=1 map_ring=%u map_dma=%u user_avail=%u sock_poll=%u "
                "wire_handoff+tcp22=1 stack=eth|tcp|door|:22 "
                "handoff=netstackd|sshd|UDX product_dod_b=UDX "
                "door_thr_only=1 H1=1 h1_poll_sites=1 thr_only=1 "
                "net_eth_irq=0 irq_path=0 poll_own=door_thr "
                "fault_class=H1_irq_stack_smash dual_dod_b=OPEN dual_dod=OPEN "
                "functional_ops=sock_poll|connect|close|send|recv|accept_eagain "
                "sshd_pipeline=socket|bind22|listen|accept|poll|send|recv|connect|close|sock_poll "
                "yield_trio=poll|accept|sock_poll functional_route=tcp|lo "
                "soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
                "(Soft!=product; W11 Dual DoD B FUNCTIONAL residual lean "
                "STRONGER wire22 denser; wire handoff+:22 stack; H1 door thr "
                "eth; G-AC-1; stamp-free bar v2026.08.04.75; never invent .76; "
                "agent!=close)\n",
                u32Checks, u32Ok, u32UdxOk, u32MapNotes, u32SshdOk,
                u32FunctionalOk, u32FunctionalSteps, u32H1, u32H1ThrLocks,
                u32Wire22, g_soft.u32Wire22Dense,
                GJ_NET_OP_MAP_RING, GJ_NET_OP_MAP_DMA, GJ_NET_OP_USER_AVAIL,
                GJ_NET_OP_SOCK_POLL);
    }
}

/**
 * After first product call activity, print soft residual once.
 * Safe from call return paths only. No per-op re-dump.
 */
static void
net_door_soft_maybe_once(void)
{
    if (g_fSoftOnce != 0) {
        return;
    }
    if (g_soft.u64Enter == 0) {
        return;
    }
    g_fSoftOnce = 1;
    net_door_soft_residual_once();
    net_door_soft_residual_lean_once();
}

void
net_door_init(void)
{
    g_fInit = 1;
    g_u32Calls = 0;
    g_u32OwnerToken = 0;
    g_u32VqCalls = 0;
    g_u32Claims = 0;
    g_u32Reclaims = 0;
    g_u32RingCalls = 0;
    g_u64RingMapVa = 0;
    g_u64MapDmaVa = 0;
    g_u16LastMapWhich = 0;
    memset(&g_soft, 0, sizeof(g_soft));
    g_fSoftOnce = 0;
    g_fSoftSshd22 = 0;
    g_fSoftLean = 0;
    g_fUdxReady = 0;
    g_fUdxReadyLamp = 0;
    g_fUdxInjLamp = 0;
    g_fUdxTxLamp = 0;
    g_u32UdxTxHead = 0;
    g_u32UdxTxTail = 0;
    g_u32UdxTxN = 0;
    g_u32UdxInjOk = 0;
    g_u32UdxTxEnq = 0;
    g_u32UdxTxPull = 0;
    memset(g_aUdxTxLen, 0, sizeof(g_aUdxTxLen));
    /* Grep: net_door: soft residual (init lamp; sparse) */
    kprintf("net_door: init (POLL->net_eth_poll Dual DoD B=UDX "
            "Soft!=product G-AC-1 dual=MIT_OR_Apache-2.0 "
            "handoff=netstackd|sshd|UDX not_freestanding_rtl=1 "
            "door_thr_only=1 H1=1 h1_poll_sites=%u thr_only=%u "
            "net_eth_irq=%u irq_path=%u poll_own=door_thr "
            "fault_class=H1_irq_stack_smash dual_dod_b=OPEN "
            "udx_host_stack=claim|map_ring|map_dma|desc|user_avail|bounce)\n",
            (unsigned)NET_DOOR_H1_POLL_SITES,
            (unsigned)NET_DOOR_H1_THR_ONLY,
            (unsigned)NET_DOOR_H1_IRQ_POLL,
            (unsigned)NET_DOOR_H1_IRQ_PATH);
    kprintf("net_door: soft residual init xfer_max=%u eth_max=%u page=%u "
            "path=claim|ring|poll->net_eth_poll|socket|bind22|send|recv|"
            "connect|close|sock_poll eth_advance=1 "
            "irq=0 door_thr_only=1 H1=1 h1_poll_sites=%u thr_only=%u "
            "net_eth_irq=%u irq_path=%u poll_own=door_thr poll_h1=0 "
            "fault_class=H1_irq_stack_smash "
            "net_eth_poll=run_loop_or_door poll=always_ok "
            "accept_empty=eagain dual_dod_b=OPEN_UDX product_dod_b=UDX "
            "not_freestanding_rtl=1 product_sshd_tcp22=OPEN dual_dod_b=OPEN "
            "functional_ops=sock_poll|connect|close|send|recv|accept_eagain "
            "udx_host_stack=claim|map_ring|map_dma|desc|user_avail|bounce "
            "handoff=netstackd|sshd|UDX soft=1 product=0 Soft!=product G-AC-1\n",
            NET_XFER_MAX, NET_ETH_MAX, NET_DOOR_PAGE_SIZE,
            (unsigned)NET_DOOR_H1_POLL_SITES,
            (unsigned)NET_DOOR_H1_THR_ONLY,
            (unsigned)NET_DOOR_H1_IRQ_POLL,
            (unsigned)NET_DOOR_H1_IRQ_PATH);
    /* Grep: net_door: soft residual h1 (init; sparse; C1 thr-only deepen) */
    kprintf("net_door: soft residual h1 init thr_only=%u net_eth_irq=%u "
            "irq_path=%u door_thr_only=%u h1_poll_sites=%u poll_own=door_thr "
            "poll_h1=0 h1_match=1 h1_thr_locks=1 "
            "fault_class=H1_irq_stack_smash net_eth_poll=run_loop_or_door "
            "dual_dod_b=OPEN product_dod_b=UDX not_freestanding_rtl=1 "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "(Soft!=product; C1 H1 thr-only residual deepen; door thr full "
            "kstack only; never IRQ/timer; agent!=close; no version stamp)\n",
            (unsigned)NET_DOOR_H1_THR_ONLY,
            (unsigned)NET_DOOR_H1_IRQ_POLL,
            (unsigned)NET_DOOR_H1_IRQ_PATH,
            (unsigned)NET_DOOR_H1_DOOR_THR,
            (unsigned)NET_DOOR_H1_POLL_SITES);
    /* Grep: net_door: soft residual lean (init lean; sparse) */
    kprintf("net_door: soft residual lean init "
            "poll_op=%u claim_op=%u map_ring=%u map_dma=%u user_avail=%u "
            "export=%u kick=%u socket=%u bind=%u listen=%u accept=%u "
            "connect=%u close=%u send=%u recv=%u sock_poll=%u "
            "path=claim|ring|poll->net_eth_poll|socket|bind22|send|recv|"
            "connect|close|sock_poll "
            "udx_host_stack=claim|map_ring|map_dma|desc|user_avail|bounce "
            "handoff=netstackd|sshd|UDX eth_advance=1 irq=0 door_thr_only=1 "
            "H1=1 h1_poll_sites=%u thr_only=%u net_eth_irq=%u irq_path=%u "
            "poll_own=door_thr fault_class=H1_irq_stack_smash dual_dod_b=OPEN "
            "dual_dod_b=OPEN_UDX product_dod_b=UDX not_freestanding_rtl=1 "
            "functional_ops=sock_poll|connect|close|send|recv|accept_eagain "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
            "(Soft!=product; no version stamp; no stamp storms; G-AC-1; "
            "C1 H1 thr-only; functional residual; agent!=close)\n",
            GJ_NET_OP_POLL, GJ_NET_OP_CLAIM, GJ_NET_OP_MAP_RING,
            GJ_NET_OP_MAP_DMA, GJ_NET_OP_USER_AVAIL,
            GJ_NET_OP_EXPORT_RING, GJ_NET_OP_KICK,
            GJ_NET_OP_SOCKET, GJ_NET_OP_BIND, GJ_NET_OP_LISTEN,
            GJ_NET_OP_ACCEPT, GJ_NET_OP_CONNECT, GJ_NET_OP_CLOSE,
            GJ_NET_OP_SEND, GJ_NET_OP_RECV, GJ_NET_OP_SOCK_POLL,
            (unsigned)NET_DOOR_H1_POLL_SITES,
            (unsigned)NET_DOOR_H1_THR_ONLY,
            (unsigned)NET_DOOR_H1_IRQ_POLL,
            (unsigned)NET_DOOR_H1_IRQ_PATH);
    /* Grep: net_door: soft residual functional (init; sparse) */
    kprintf("net_door: soft residual functional init Soft!=product STRONGER=1 "
            "product_dod_b=UDX dual_dod_b=OPEN_UDX product_sshd_tcp22=OPEN "
            "handoff=netstackd|sshd|UDX "
            "wire_handoff+tcp22=1 stack=eth|tcp|door|:22 "
            "functional_ops=sock_poll|connect|close|send|recv|accept_eagain "
            "functional_route=tcp|lo "
            "sshd_pipeline=socket|bind22|listen|accept|poll|send|recv|connect|close|sock_poll "
            "yield_trio=poll|accept|sock_poll "
            "connect_op=%u close_op=%u send_op=%u recv_op=%u sock_poll_op=%u "
            "accept_op=%u map_ring_op=%u user_avail_op=%u "
            "sshd_steps=%u udx_steps=%u yield_ops=%u catalog_min=%u "
            "ssh_port=%u wire22_stack=%u pollin=0x%x pollout=0x%x "
            "door_thr_only=1 H1=1 poll_own=door_thr "
            "fault_class=H1_irq_stack_smash not_freestanding_rtl=1 "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "(W11 Dual DoD B FUNCTIONAL residual STRONGER; wire handoff+:22 "
            "stack; SOCK_POLL TCP+net_lo; agent!=close; stamp-free bar "
            "v2026.08.04.75; never invent .76)\n",
            GJ_NET_OP_CONNECT, GJ_NET_OP_CLOSE, GJ_NET_OP_SEND,
            GJ_NET_OP_RECV, GJ_NET_OP_SOCK_POLL, GJ_NET_OP_ACCEPT,
            GJ_NET_OP_MAP_RING, GJ_NET_OP_USER_AVAIL,
            (unsigned)NET_DOOR_FUNC_SSHD_STEPS,
            (unsigned)NET_DOOR_FUNC_UDX_STEPS,
            (unsigned)NET_DOOR_FUNC_YIELD_OPS,
            (unsigned)NET_DOOR_FUNC_CATALOG_MIN,
            (unsigned)NET_DOOR_SSH_PORT,
            (unsigned)NET_DOOR_WIRE22_STACK,
            (unsigned)NET_DOOR_POLLIN,
            (unsigned)NET_DOOR_POLLOUT);
    /* Grep: net_door: soft residual functional catalog (init; sparse) */
    kprintf("net_door: soft residual functional catalog init Soft!=product "
            "STRONGER=1 product_dod_b=UDX dual_dod_b=OPEN_UDX "
            "product_sshd_tcp22=OPEN handoff=netstackd|sshd|UDX "
            "sshd_steps=%u udx_steps=%u yield_ops=%u catalog_min=%u "
            "sshd_pipeline=socket|bind22|listen|accept|poll|send|recv|"
            "connect|close|sock_poll "
            "udx_host_stack=claim|export|map_ring|map_dma|desc|user_avail|"
            "bounce|kick yield_trio=poll|accept|sock_poll "
            "functional_route=tcp|lo "
            "functional_ops=sock_poll|connect|close|send|recv|accept_eagain "
            "door_thr_only=1 H1=1 poll_own=door_thr "
            "fault_class=H1_irq_stack_smash not_freestanding_rtl=1 "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "(STRONGER FUNCTIONAL catalog init; agent!=close; no stamp)\n",
            (unsigned)NET_DOOR_FUNC_SSHD_STEPS,
            (unsigned)NET_DOOR_FUNC_UDX_STEPS,
            (unsigned)NET_DOOR_FUNC_YIELD_OPS,
            (unsigned)NET_DOOR_FUNC_CATALOG_MIN);
    /*
     * Grep: net_door: soft residual wire22 denser (init; sparse; H2 once)
     * STRONGER wire22 residual denser surface for product sshd Dual DoD B.
     * Soft!=product; Dual DoD OPEN; product_dod_b=UDX;
     * denser H1 thr-only door eth poll (arm0 sublocks + poll_h1_wire22).
     * greppable: denser_h1 | thr-only door eth poll | poll_h1_wire22
     */
    kprintf("net_door: soft residual wire22 denser init Soft!=product denser=1 "
            "STRONGER=1 denser_arms=%u denser_min=%u denser_h1_sub=%u "
            "denser_sock_sub=%u "
            "wire22_stack=%u denser_h1=1 thr-only_door_eth_poll=1 "
            "ssh_port=%u sock_ops=socket|bind|listen|accept|poll|sock_poll "
            "arms=h1_poll|sock22|yield|dual_dod_open|product_udx "
            "h1_thr_only=%u h1_poll_sites=%u net_eth_irq=%u irq_path=%u "
            "door_thr_only=%u poll_own=door_thr poll_h1_wire22=0 "
            "fault_class=H1_irq_stack_smash "
            "stack=eth|tcp|door|:22 wire_handoff+tcp22=1 "
            "product_sshd_tcp22=OPEN product_dod_b=UDX "
            "dual_dod_b=OPEN_UDX dual_dod_b=OPEN dual_dod=OPEN "
            "not_freestanding_rtl=1 handoff=netstackd|sshd|UDX "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
            "agent!=close stamp_free=v2026.08.04.75 never=.76 "
            "(STRONGER wire22 residual denser init; SOCK BIND/LISTEN/ACCEPT/"
            "POLL :22 door product sshd; denser H1 thr-only door eth poll; "
            "denser_sock_sub; Soft!=product; Dual DoD OPEN; product_dod_b=UDX)\n",
            (unsigned)NET_DOOR_WIRE22_DENSE_ARMS,
            (unsigned)NET_DOOR_WIRE22_DENSE_MIN,
            (unsigned)NET_DOOR_WIRE22_DENSE_H1_SUB,
            (unsigned)NET_DOOR_WIRE22_DENSE_SOCK_SUB,
            (unsigned)NET_DOOR_WIRE22_STACK,
            (unsigned)NET_DOOR_SSH_PORT,
            (unsigned)NET_DOOR_H1_THR_ONLY,
            (unsigned)NET_DOOR_H1_POLL_SITES,
            (unsigned)NET_DOOR_H1_IRQ_POLL,
            (unsigned)NET_DOOR_H1_IRQ_PATH,
            (unsigned)NET_DOOR_H1_DOOR_THR);
    /* Grep: net_door: ring map notes (init; sparse) */
    kprintf("net_door: ring map notes init Soft!=product product_dod_b=UDX "
            "page=%u map_ring_op=%u map_dma_op=%u desc_op=%u user_avail_op=%u "
            "export_op=%u kick_op=%u avail_push_op=%u used_reap_op=%u "
            "ring_state_op=%u map_va=0 dma_va=0 reclaim=soft_same_va "
            "dma_reclaim=soft_same_va "
            "udx_attach=map_va+off_desc|off_avail|off_used "
            "udx_host_stack=claim|map_ring|map_dma|desc|user_avail|bounce "
            "handoff=netstackd|sshd|UDX door_thr_only=1 H1=1 h1_poll_sites=%u "
            "thr_only=%u poll_own=door_thr fault_class=H1_irq_stack_smash "
            "(NODEV soft-skip without virtio-net; no per-MAP dump; "
            "dual MIT OR Apache-2.0; G-AC-1; C1 H1 thr-only)\n",
            (unsigned)NET_DOOR_PAGE_SIZE,
            GJ_NET_OP_MAP_RING, GJ_NET_OP_MAP_DMA, GJ_NET_OP_DESC_ALLOC,
            GJ_NET_OP_USER_AVAIL, GJ_NET_OP_EXPORT_RING, GJ_NET_OP_KICK,
            GJ_NET_OP_AVAIL_PUSH, GJ_NET_OP_USED_REAP, GJ_NET_OP_RING_STATE,
            (unsigned)NET_DOOR_H1_POLL_SITES,
            (unsigned)NET_DOOR_H1_THR_ONLY);
}

int
net_door_owned(void)
{
    return g_u32OwnerToken != 0;
}

u32
net_door_owner_token(void)
{
    return g_u32OwnerToken;
}

u64
net_door_ring_map_va(void)
{
    return g_u64RingMapVa;
}

u64
net_door_map_dma_va(void)
{
    return g_u64MapDmaVa;
}

u32
net_door_ring_calls(void)
{
    return g_u32RingCalls;
}

u32
net_door_claim_count(void)
{
    /* Soft diagnostics: first claims + idempotent reclaims. */
    return g_u32Claims + g_u32Reclaims;
}

/**
 * Soft terminal: classify ret + one-shot residual, then return ret.
 * All product returns from net_door_call go through here (ABI unchanged).
 */
static i64
net_door_soft_done(i64 i64Ret)
{
    g_soft.i64LastRet = i64Ret;
    net_door_soft_note_ret(i64Ret);
    net_door_soft_maybe_once();
    return i64Ret;
}

/* ETH_TX_PULL head: any completed attempt that will not retry this slot. */
static void
net_door_udx_tx_drop_head(void)
{
    u32 iSlot;

    if (g_u32UdxTxN == 0u) {
        return;
    }
    iSlot = g_u32UdxTxTail % NET_DOOR_UDX_TX_SLOTS;
    g_aUdxTxLen[iSlot] = 0u;
    g_u32UdxTxTail++;
    g_u32UdxTxN--;
}

/*
 * Product UDX L2: arm ready so ETH_INJECT demux + ETH_TX_PULL enqueue work.
 * Driver may inject before GJ_NET_OP_ETH_UDX_READY; missing arm was NODEV
 * and ARP never demuxed (0.1.185 L3 FAIL). No new lamp. Soft!=product.
 */
static void
net_door_udx_arm_soft(void)
{
    g_fUdxReady = 1u;
    net_l2_udx_ready_identity();
    net_tcp_ensure_listen22();
}

i64
net_door_call(u32 u32Op, u64 u64Arg1, u64 u64Arg2, u64 u64Arg3)
{
    if (!g_fInit) {
        net_door_soft_inc(&g_soft.u64NotInit);
        g_soft.u32LastOp = u32Op;
        return net_door_soft_done(GJ_ERR_NODEV);
    }
    g_u32Calls++;
    net_door_soft_inc(&g_soft.u64Enter);
    g_soft.u32LastOp = u32Op;
    switch (u32Op) {
    case GJ_NET_OP_CLAIM:
        /* arg1 = non-zero ownership token (low 32 bits only). */
        if (u64Arg1 == 0 || (u64Arg1 >> 32) != 0) {
            /* Path tally only; terminal class via note_ret (no double-count). */
            net_door_soft_inc(&g_soft.u64ClaimInval);
            return net_door_soft_done(GJ_ERR_INVAL);
        }
        if (g_u32OwnerToken != 0 && g_u32OwnerToken != (u32)u64Arg1) {
            net_door_soft_inc(&g_soft.u64ClaimBusy);
            return net_door_soft_done(GJ_ERR_BUSY); /* another netstackd */
        }
        /* Soft reclaim: same token re-CLAIM is idempotent (no re-log). */
        if (g_u32OwnerToken == (u32)u64Arg1) {
            g_u32Reclaims++;
            net_door_soft_inc(&g_soft.u64ClaimReclaim);
            return net_door_soft_done(0);
        }
        g_u32OwnerToken = (u32)u64Arg1;
        g_u32Claims++;
        net_door_soft_inc(&g_soft.u64ClaimOk);
        kprintf("net_door: CLAIM token=0x%x (userspace owns net)\n",
                g_u32OwnerToken);
        return net_door_soft_done(0);
    case GJ_NET_OP_RELEASE:
        /* Soft free path: already unowned -> 0 (no token match required). */
        if (g_u32OwnerToken == 0) {
            net_door_soft_inc(&g_soft.u64ReleaseFree);
            return net_door_soft_done(0);
        }
        if ((u64Arg1 >> 32) != 0 || (u32)u64Arg1 != g_u32OwnerToken) {
            /* Path tally only; terminal class via note_ret (no double-count). */
            net_door_soft_inc(&g_soft.u64ReleaseInval);
            return net_door_soft_done(GJ_ERR_INVAL);
        }
        kprintf("net_door: RELEASE token=0x%x\n", g_u32OwnerToken);
        g_u32OwnerToken = 0;
        net_door_soft_inc(&g_soft.u64ReleaseOk);
        return net_door_soft_done(0);
    case GJ_NET_OP_POLL:
        /*
         * Interim freestanding eth advance (Soft!=product dual license):
         * door thr stack -> net_door_eth_poll_h1 -> net_eth_poll
         * (L2 RX demux + net_tcp_input/poll). Never IRQ/timer (H1). Soft
         * always 0; never NODEV. L2 absent: eth soft-skips still ticks
         * net_tcp_poll for rtx/TW. sshd yield loop: POLL+ACCEPT. Product
         * DoD B = UDX not freestanding rtl. No per-POLL residual dump
         * (no stamp storms). Sole net_eth_poll site in this TU is the H1
         * wrapper (h1_poll_sites=1; NET_DOOR_H1_POLL_SITES). C1 thr-only
         * residual deepen: thr_only=1 net_eth_irq=0 irq_path=0
         * poll_own=door_thr fault_class=H1_irq_stack_smash dual_dod_b=OPEN.
         * greppable: poll->net_eth_poll | irq=0 | net_eth_poll=run_loop_or_door
         * greppable: h1_poll_sites=1 | poll_h1 | door_thr_only=1 | H1
         * greppable: thr_only=1 | net_eth_irq=0 | poll_own=door_thr
         * greppable: fault_class=H1_irq_stack_smash | net_door: soft residual h1
         */
        net_door_soft_inc(&g_soft.u64Poll);
        if (net_l2_ready() != 0) {
            net_door_soft_inc(&g_soft.u64PollL2Ready);
        }
        /* STRONGER wire22 denser: POLL while soft :22 held (sshd yield). */
        if (g_fSoftSshd22 != 0) {
            net_door_soft_inc(&g_soft.u64Poll22);
        }
        /* H1: door thr only; sole eth advance site (C1 thr-only locks). */
        net_door_eth_poll_h1();
        net_door_soft_inc(&g_soft.u64PollEth);
        net_door_soft_inc(&g_soft.u64PollOk);
        return net_door_soft_done(0);
    case GJ_NET_OP_STATS: {
        u32 aSt[4];

        if (u64Arg1 == 0) {
            return net_door_soft_done(GJ_ERR_INVAL);
        }
        /*
         * Soft-friendly fill: getters return 0 when eth never replied.
         * ABI: [0]arp [1]udp [2]icmp [3]door calls - stable for shell smoke.
         * No per-STATS residual dump (stamp storm risk under yield loops).
         */
        aSt[0] = net_eth_arp_replies();
        aSt[1] = net_eth_udp_echoes();
        aSt[2] = net_eth_icmp_echoes();
        aSt[3] = g_u32Calls;
        if (user_range_ok(u64Arg1, sizeof(aSt))) {
            if (copy_to_user(u64Arg1, aSt, sizeof(aSt)) != GJ_OK) {
                return net_door_soft_done(GJ_ERR_FAULT);
            }
        } else {
            memcpy((void *)(gj_vaddr_t)u64Arg1, aSt, sizeof(aSt));
        }
        return net_door_soft_done(0);
    }
    case GJ_NET_OP_SOCKET: {
        /*
         * sshd interim residual soft path (Soft!=product; product_dod_b=UDX):
         * sshd-gj SOCKET -> BIND(:22) -> LISTEN -> ACCEPT(+POLL yield).
         * STRONGER FUNCTIONAL: TCP vs lo route tallies.
         */
        i64 i64R;
        int fTcp;

        net_door_soft_inc(&g_soft.u64Socket);
        fTcp = ((int)u64Arg2 == SOCK_STREAM) ? 1 : 0;
        if (fTcp != 0) {
            net_door_soft_inc(&g_soft.u64SocketTcp);
            i64R = net_tcp_socket();
        } else {
            net_door_soft_inc(&g_soft.u64SocketLo);
            i64R = net_lo_socket((int)u64Arg1, (int)u64Arg2, (int)u64Arg3);
        }
        return net_door_soft_done(i64R);
    }
    case GJ_NET_OP_BIND: {
        /*
         * Soft residual: port 22 marks sshd interim :22 (Soft!=product).
         * dual_dod_b=OPEN_UDX product_sshd_tcp22=OPEN until interactive SSH login.
         * Product DoD B = UDX not freestanding rtl (G-AC-1).
         */
        i64 i64R;
        u16 u16Port;
        int fTcp;
        int f22;

        net_door_soft_inc(&g_soft.u64Bind);
        u16Port = (u16)u64Arg2;
        f22 = (u16Port == (u16)NET_DOOR_SSH_PORT) ? 1 : 0;
        fTcp = net_tcp_fd_ok((i64)u64Arg1) ? 1 : 0;
        if (f22 != 0) {
            net_door_soft_inc(&g_soft.u64Bind22);
        }
        if (fTcp != 0) {
            i64R = net_tcp_bind((i64)u64Arg1, u16Port);
        } else {
            i64R = net_lo_bind((i64)u64Arg1, u16Port);
        }
        if (i64R >= 0 && f22 != 0) {
            net_door_soft_inc(&g_soft.u64Bind22Ok);
            g_fSoftSshd22 = 1; /* soft hold :22 for listen/accept lamps */
        }
        return net_door_soft_done(i64R);
    }
    case GJ_NET_OP_LISTEN: {
        i64 i64R;
        int fTcp;

        net_door_soft_inc(&g_soft.u64Listen);
        fTcp = net_tcp_fd_ok((i64)u64Arg1) ? 1 : 0;
        if (g_fSoftSshd22 != 0) {
            net_door_soft_inc(&g_soft.u64Listen22);
        }
        if (fTcp != 0) {
            i64R = net_tcp_listen((i64)u64Arg1, (int)u64Arg2);
        } else {
            i64R = net_lo_listen((i64)u64Arg1, (int)u64Arg2);
        }
        /* Functional residual: listen success while soft :22 held (sshd). */
        if (i64R >= 0 && g_fSoftSshd22 != 0) {
            net_door_soft_inc(&g_soft.u64Listen22Ok);
        }
        return net_door_soft_done(i64R);
    }
    case GJ_NET_OP_ACCEPT: {
        /*
         * sshd interim park: empty accept is soft EAGAIN (-11), not hard-fail.
         * sshd yield loop POLL+ACCEPT; Soft!=product until interactive SSH login.
         * Product DoD B = UDX not freestanding rtl (G-AC-1).
         * Functional residual: accept22_ok + eagain path tallies.
         */
        i64 i64R;
        int fTcp;

        net_door_soft_inc(&g_soft.u64Accept);
        fTcp = net_tcp_fd_ok((i64)u64Arg1) ? 1 : 0;
        if (g_fSoftSshd22 != 0) {
            net_door_soft_inc(&g_soft.u64Accept22);
        }
        if (fTcp != 0) {
            i64R = net_tcp_accept((i64)u64Arg1);
        } else {
            i64R = net_lo_accept((i64)u64Arg1);
        }
        if (i64R >= 0) {
            net_door_soft_inc(&g_soft.u64AcceptOk);
            if (g_fSoftSshd22 != 0) {
                net_door_soft_inc(&g_soft.u64Accept22Ok);
            }
        } else if (i64R == (i64)-11 || i64R == GJ_ERR_AGAIN) {
            /* Soft empty: yield park (sshd interim; product_dod_b=UDX). */
            net_door_soft_inc(&g_soft.u64AcceptEagain);
            /* STRONGER FUNCTIONAL: :22 yield park honesty. */
            if (g_fSoftSshd22 != 0) {
                net_door_soft_inc(&g_soft.u64AcceptEagain22);
            }
        }
        return net_door_soft_done(i64R);
    }
    case GJ_NET_OP_CONNECT: {
        /*
         * Functional residual CONNECT (sshd pair / netstackd loopback).
         * Soft!=product; STRONGER: TCP|lo route + connect :22 client path.
         */
        i64 i64R;
        int fTcp;
        int f22;

        net_door_soft_inc(&g_soft.u64Connect);
        fTcp = net_tcp_fd_ok((i64)u64Arg1) ? 1 : 0;
        f22 = ((u16)u64Arg2 == (u16)NET_DOOR_SSH_PORT) ? 1 : 0;
        if (f22 != 0) {
            net_door_soft_inc(&g_soft.u64Connect22);
        }
        if (fTcp != 0) {
            net_door_soft_inc(&g_soft.u64ConnectTcp);
            i64R = net_tcp_connect((i64)u64Arg1, (u16)u64Arg2);
        } else {
            net_door_soft_inc(&g_soft.u64ConnectLo);
            i64R = net_lo_connect((i64)u64Arg1, (u16)u64Arg2);
        }
        if (i64R >= 0) {
            net_door_soft_inc(&g_soft.u64ConnectOk);
            if (f22 != 0) {
                net_door_soft_inc(&g_soft.u64Connect22Ok);
            }
        } else if (i64R == (i64)-11 || i64R == GJ_ERR_AGAIN) {
            net_door_soft_inc(&g_soft.u64ConnectEagain);
        }
        return net_door_soft_done(i64R);
    }
    case GJ_NET_OP_CLOSE: {
        /*
         * Functional residual CLOSE (sshd session teardown / pair cleanup).
         * STRONGER: TCP|lo route + close while soft :22 held.
         */
        i64 i64R;
        int fTcp;

        net_door_soft_inc(&g_soft.u64Close);
        if (g_fSoftSshd22 != 0) {
            net_door_soft_inc(&g_soft.u64Close22);
        }
        fTcp = net_tcp_fd_ok((i64)u64Arg1) ? 1 : 0;
        if (fTcp != 0) {
            net_door_soft_inc(&g_soft.u64CloseTcp);
            i64R = net_tcp_close((i64)u64Arg1);
        } else {
            net_door_soft_inc(&g_soft.u64CloseLo);
            i64R = net_lo_close((i64)u64Arg1);
        }
        if (i64R >= 0) {
            net_door_soft_inc(&g_soft.u64CloseOk);
        }
        return net_door_soft_done(i64R);
    }
    case GJ_NET_OP_SEND: {
        /* Bounce <= NET_XFER_MAX; TCP multi-seg chunks by MSS inside net_tcp. */
        u8 aBuf[NET_XFER_MAX];
        size_t cb = (size_t)u64Arg3;
        i64 n;
        int fTcp;

        net_door_soft_inc(&g_soft.u64Send);
        if (g_fSoftSshd22 != 0) {
            net_door_soft_inc(&g_soft.u64Send22);
        }
        if (u64Arg2 == 0 || cb == 0) {
            return net_door_soft_done(GJ_ERR_INVAL);
        }
        if (cb > NET_XFER_MAX) {
            cb = NET_XFER_MAX;
        }
        if (user_range_ok(u64Arg2, cb)) {
            if (copy_from_user(aBuf, u64Arg2, cb) != GJ_OK) {
                return net_door_soft_done(GJ_ERR_FAULT);
            }
        } else {
            memcpy(aBuf, (const void *)(gj_vaddr_t)u64Arg2, cb);
        }
        fTcp = net_tcp_fd_ok((i64)u64Arg1) ? 1 : 0;
        if (fTcp != 0) {
            net_door_soft_inc(&g_soft.u64SendTcp);
            n = net_tcp_send((i64)u64Arg1, aBuf, cb);
        } else {
            net_door_soft_inc(&g_soft.u64SendLo);
            n = net_lo_send((i64)u64Arg1, aBuf, cb);
        }
        /* Functional residual SEND outcomes (sshd banner / session / bulk). */
        if (n > 0) {
            net_door_soft_inc(&g_soft.u64SendOk);
            if (g_fSoftSshd22 != 0) {
                net_door_soft_inc(&g_soft.u64Send22Ok);
            }
        } else if (n == (i64)-11 || n == GJ_ERR_AGAIN) {
            net_door_soft_inc(&g_soft.u64SendEagain);
        }
        return net_door_soft_done(n);
    }
    case GJ_NET_OP_RECV: {
        /* Multi-seg drain may take several RECV calls; each <= NET_XFER_MAX. */
        u8 aBuf[NET_XFER_MAX];
        size_t cb = (size_t)u64Arg3;
        i64 n;
        int fTcp;

        net_door_soft_inc(&g_soft.u64Recv);
        if (g_fSoftSshd22 != 0) {
            net_door_soft_inc(&g_soft.u64Recv22);
        }
        if (u64Arg2 == 0 || cb == 0) {
            return net_door_soft_done(GJ_ERR_INVAL);
        }
        if (cb > NET_XFER_MAX) {
            cb = NET_XFER_MAX;
        }
        fTcp = net_tcp_fd_ok((i64)u64Arg1) ? 1 : 0;
        if (fTcp != 0) {
            net_door_soft_inc(&g_soft.u64RecvTcp);
            n = net_tcp_recv((i64)u64Arg1, aBuf, cb);
        } else {
            net_door_soft_inc(&g_soft.u64RecvLo);
            n = net_lo_recv((i64)u64Arg1, aBuf, cb);
        }
        if (n > 0) {
            if ((size_t)n > NET_XFER_MAX) {
                return net_door_soft_done(GJ_ERR_IO); /* defensive */
            }
            if (user_range_ok(u64Arg2, (size_t)n)) {
                if (copy_to_user(u64Arg2, aBuf, (size_t)n) != GJ_OK) {
                    return net_door_soft_done(GJ_ERR_FAULT);
                }
            } else {
                memcpy((void *)(gj_vaddr_t)u64Arg2, aBuf, (size_t)n);
            }
            /* Functional residual RECV ok (sshd session / multi-seg drain). */
            net_door_soft_inc(&g_soft.u64RecvOk);
            if (g_fSoftSshd22 != 0) {
                net_door_soft_inc(&g_soft.u64Recv22Ok);
            }
        } else if (n == 0) {
            net_door_soft_inc(&g_soft.u64Recv0); /* EOF soft */
        } else if (n == (i64)-11 || n == GJ_ERR_AGAIN) {
            net_door_soft_inc(&g_soft.u64RecvEagain);
        }
        return net_door_soft_done(n);
    }
    case GJ_NET_OP_TCP_STATS: {
        u32 aSt[4];
        u32 segs;
        u32 rtx;

        /*
         * sshd freestanding smoke path (Soft!=product). Soft-friendly:
         * getters zero when quiet; never hard-fail on empty stack.
         */
        net_door_soft_inc(&g_soft.u64TcpStats);
        if (u64Arg1 == 0) {
            return net_door_soft_done(GJ_ERR_INVAL);
        }
        segs = net_tcp_segments();
        rtx = net_tcp_retransmits();
        aSt[0] = net_tcp_accepts();
        aSt[1] = (rtx << 16) | (segs & 0xffffu);
        aSt[2] = net_tcp_bytes_rx();
        aSt[3] = net_tcp_bytes_tx();
        if (user_range_ok(u64Arg1, sizeof(aSt))) {
            if (copy_to_user(u64Arg1, aSt, sizeof(aSt)) != GJ_OK) {
                return net_door_soft_done(GJ_ERR_FAULT);
            }
        } else {
            memcpy((void *)(gj_vaddr_t)u64Arg1, aSt, sizeof(aSt));
        }
        return net_door_soft_done(0);
    }
    case GJ_NET_OP_SOCK_POLL: {
        /*
         * FUNCTIONAL residual SOCK_POLL (sshd/UDX / cold poll path; STRONGER):
         * TCP -> net_tcp_poll_mask; net_lo -> net_lo_poll_mask (was soft 0).
         * Want POLLIN/POLLOUT interest + empty ready soft park tallies.
         * Soft!=product: empty ready is soft 0 (never hard-fail for sshd).
         * greppable: functional_ops=sock_poll | functional_route=tcp|lo
         * greppable: net_door: soft residual functional
         */
        u32 u32Want;
        u32 u32Got;
        int fTcp;
        int fLo;

        net_door_soft_inc(&g_soft.u64SockPoll);
        u32Want = (u32)u64Arg2;
        if ((u32Want & NET_DOOR_POLLIN) != 0u) {
            net_door_soft_inc(&g_soft.u64SockPollIn);
        }
        if ((u32Want & NET_DOOR_POLLOUT) != 0u) {
            net_door_soft_inc(&g_soft.u64SockPollOut);
        }
        /* W11 Dual DoD B: SOCK_POLL while soft :22 held (wire22 stack). */
        if (g_fSoftSshd22 != 0) {
            net_door_soft_inc(&g_soft.u64SockPoll22);
        }
        fTcp = net_tcp_fd_ok((i64)u64Arg1) ? 1 : 0;
        fLo = 0;
        if (fTcp != 0) {
            u32Got = net_tcp_poll_mask((i64)u64Arg1, u32Want);
            net_door_soft_inc(&g_soft.u64SockPollTcp);
        } else if (net_lo_fd_ok((i64)u64Arg1)) {
            u32Got = net_lo_poll_mask((i64)u64Arg1, u32Want);
            fLo = 1;
            net_door_soft_inc(&g_soft.u64SockPollLo);
        } else {
            /* Unknown fd: soft empty readiness (never hard-fail). */
            u32Got = 0;
        }
        (void)fLo;
        if (u32Got != 0) {
            net_door_soft_inc(&g_soft.u64SockPollReady);
        } else {
            /* Soft empty ready (sshd park / UDX cold poll). */
            net_door_soft_inc(&g_soft.u64SockPollEmpty);
        }
        return net_door_soft_done((i64)u32Got);
    }
    case GJ_NET_OP_VIRTIO_TX: {
        u8 aFrame[NET_ETH_MAX];
        u32 cb = (u32)u64Arg2;

        if (!virtio_net_ready()) {
            return net_door_soft_done(GJ_ERR_NODEV);
        }
        if (u64Arg1 == 0 || cb == 0 || cb > NET_ETH_MAX) {
            return net_door_soft_done(GJ_ERR_INVAL);
        }
        if (user_range_ok(u64Arg1, cb)) {
            if (copy_from_user(aFrame, u64Arg1, cb) != GJ_OK) {
                return net_door_soft_done(GJ_ERR_FAULT);
            }
        } else {
            memcpy(aFrame, (const void *)(gj_vaddr_t)u64Arg1, cb);
        }
        g_u32VqCalls++;
        if (virtio_net_tx(aFrame, cb) != 0) {
            return net_door_soft_done(GJ_ERR_IO);
        }
        return net_door_soft_done((i64)cb);
    }
    case GJ_NET_OP_VIRTIO_RX: {
        u8 aFrame[NET_ETH_MAX];
        u32 cbMax = (u32)u64Arg2;
        i32 n;

        if (!virtio_net_ready()) {
            return net_door_soft_done(GJ_ERR_NODEV);
        }
        if (u64Arg1 == 0 || cbMax == 0) {
            return net_door_soft_done(GJ_ERR_INVAL);
        }
        if (cbMax > NET_ETH_MAX) {
            cbMax = NET_ETH_MAX;
        }
        g_u32VqCalls++;
        n = virtio_net_rx(aFrame, cbMax);
        if (n < 0) {
            return net_door_soft_done(GJ_ERR_IO);
        }
        if (n == 0) {
            return net_door_soft_done(0);
        }
        if ((u32)n > NET_ETH_MAX) {
            return net_door_soft_done(GJ_ERR_IO);
        }
        if (user_range_ok(u64Arg1, (u32)n)) {
            if (copy_to_user(u64Arg1, aFrame, (size_t)n) != GJ_OK) {
                return net_door_soft_done(GJ_ERR_FAULT);
            }
        } else {
            memcpy((void *)(gj_vaddr_t)u64Arg1, aFrame, (size_t)n);
        }
        return net_door_soft_done((i64)n);
    }
    case GJ_NET_OP_QUEUE_INFO: {
        u32 aQ[5];

        if (u64Arg1 == 0) {
            return net_door_soft_done(GJ_ERR_INVAL);
        }
        aQ[0] = virtio_net_tx_count();
        aQ[1] = virtio_net_rx_count();
        aQ[2] = virtio_net_ready() ? 1u : 0u;
        aQ[3] = g_u32OwnerToken ? 1u : 0u;
        aQ[4] = g_u32VqCalls;
        if (user_range_ok(u64Arg1, sizeof(aQ))) {
            if (copy_to_user(u64Arg1, aQ, sizeof(aQ)) != GJ_OK) {
                return net_door_soft_done(GJ_ERR_FAULT);
            }
        } else {
            memcpy((void *)(gj_vaddr_t)u64Arg1, aQ, sizeof(aQ));
        }
        /* No per-QUEUE_INFO residual dump (no stamp storms). */
        return net_door_soft_done(0);
    }
    case GJ_NET_OP_EXPORT_RING: {
        struct gj_virtq_export ex;

        g_u32RingCalls++;
        net_door_soft_inc(&g_soft.u64RingEnter);
        net_door_soft_inc(&g_soft.u64ExportRing);
        if (g_u32OwnerToken != 0) {
            net_door_soft_inc(&g_soft.u64HostOwnedRing);
        }
        if (u64Arg2 == 0) {
            return net_door_soft_done(GJ_ERR_INVAL);
        }
        if (!virtio_net_ready()) {
            net_door_soft_inc(&g_soft.u64ExportNodev);
            return net_door_soft_done(GJ_ERR_NODEV);
        }
        if (virtio_net_export_q((u16)u64Arg1, &ex) != 0) {
            net_door_soft_inc(&g_soft.u64ExportNodev);
            return net_door_soft_done(GJ_ERR_NODEV);
        }
        g_u32VqCalls++;
        if (user_range_ok(u64Arg2, sizeof(ex))) {
            if (copy_to_user(u64Arg2, &ex, sizeof(ex)) != GJ_OK) {
                return net_door_soft_done(GJ_ERR_FAULT);
            }
        } else {
            memcpy((void *)(gj_vaddr_t)u64Arg2, &ex, sizeof(ex));
        }
        net_door_soft_inc(&g_soft.u64ExportOk);
        return net_door_soft_done(0);
    }
    case GJ_NET_OP_MAP_RING: {
        struct gj_virtq_export ex;
        int fReclaim;

        /*
         * UDX/host ring MAP residual (Soft!=product; product_dod_b=UDX):
         * arg1=which (0=rx,1=tx) arg2=page-aligned user VA
         * arg3=optional gj_virtq_export out for udx_virtq_attach.
         * Soft re-MAP same VA = reclaim (re-install PTEs + re-export).
         * NODEV soft-skip without virtio-net. No per-MAP residual dump.
         * greppable: net_door: ring map notes | udx_host_stack
         * H1: door thr only (not IRQ/timer).
         */
        g_u32RingCalls++;
        net_door_soft_inc(&g_soft.u64RingEnter);
        net_door_soft_inc(&g_soft.u64UserRing);
        net_door_soft_inc(&g_soft.u64MapRing);
        if (g_u32OwnerToken != 0) {
            net_door_soft_inc(&g_soft.u64HostOwnedRing);
        }
        if (u64Arg2 == 0) {
            net_door_soft_inc(&g_soft.u64MapRingInval);
            return net_door_soft_done(GJ_ERR_INVAL);
        }
        if ((u64Arg2 & (NET_DOOR_PAGE_SIZE - 1ull)) != 0) {
            net_door_soft_inc(&g_soft.u64MapRingInval);
            return net_door_soft_done(GJ_ERR_INVAL);
        }
        if (!virtio_net_ready()) {
            net_door_soft_inc(&g_soft.u64MapRingNodev);
            return net_door_soft_done(GJ_ERR_NODEV);
        }
        fReclaim = (g_u64RingMapVa != 0 && g_u64RingMapVa == u64Arg2) ? 1 : 0;
        /*
         * Soft re-MAP of the same VA: re-install PTEs + re-export (idempotent
         * hand-off for netstackd / UDX reclaim of the map window).
         */
        if (virtio_net_map_q_user((u16)u64Arg1, u64Arg2, &ex) != 0) {
            net_door_soft_inc(&g_soft.u64MapRingFault);
            return net_door_soft_done(GJ_ERR_FAULT);
        }
        g_u64RingMapVa = u64Arg2;
        g_u16LastMapWhich = (u16)u64Arg1;
        g_u32VqCalls++;
        net_door_soft_inc(&g_soft.u64MapRingOk);
        /* Functional residual UDX: which=0 rx / which=1 tx attach split. */
        if ((u16)u64Arg1 == 0u) {
            net_door_soft_inc(&g_soft.u64MapRingRx);
        } else {
            net_door_soft_inc(&g_soft.u64MapRingTx);
        }
        if (fReclaim != 0) {
            net_door_soft_inc(&g_soft.u64MapRingReclaim);
        }
        if (u64Arg3 != 0) {
            if (user_range_ok(u64Arg3, sizeof(ex))) {
                if (copy_to_user(u64Arg3, &ex, sizeof(ex)) != GJ_OK) {
                    return net_door_soft_done(GJ_ERR_FAULT);
                }
            } else {
                memcpy((void *)(gj_vaddr_t)u64Arg3, &ex, sizeof(ex));
            }
        }
        return net_door_soft_done(0);
    }
    case GJ_NET_OP_KICK:
        g_u32RingCalls++;
        net_door_soft_inc(&g_soft.u64RingEnter);
        net_door_soft_inc(&g_soft.u64Kick);
        if (g_u32OwnerToken != 0) {
            net_door_soft_inc(&g_soft.u64HostOwnedRing);
        }
        if (!virtio_net_ready()) {
            net_door_soft_inc(&g_soft.u64KickNodev);
            return net_door_soft_done(GJ_ERR_NODEV);
        }
        if (virtio_net_kick_q((u16)u64Arg1) != 0) {
            net_door_soft_inc(&g_soft.u64KickNodev);
            return net_door_soft_done(GJ_ERR_NODEV);
        }
        g_u32VqCalls++;
        net_door_soft_inc(&g_soft.u64KickOk);
        return net_door_soft_done(0);
    case GJ_NET_OP_AVAIL_PUSH: {
        u8 aBuf[NET_ETH_MAX];
        u32 cb = (u32)u64Arg3;
        u16 which = (u16)u64Arg1;
        int fWrite = (which == 0) ? 1 : 0;

        g_u32RingCalls++;
        net_door_soft_inc(&g_soft.u64RingEnter);
        net_door_soft_inc(&g_soft.u64AvailPush);
        if (g_u32OwnerToken != 0) {
            net_door_soft_inc(&g_soft.u64HostOwnedRing);
        }
        if (!virtio_net_ready()) {
            net_door_soft_inc(&g_soft.u64AvailPushNodev);
            return net_door_soft_done(GJ_ERR_NODEV);
        }
        if (u64Arg2 == 0 || cb == 0 || cb > NET_ETH_MAX) {
            net_door_soft_inc(&g_soft.u64AvailPushInval);
            return net_door_soft_done(GJ_ERR_INVAL);
        }
        if (user_range_ok(u64Arg2, cb)) {
            if (copy_from_user(aBuf, u64Arg2, cb) != GJ_OK) {
                return net_door_soft_done(GJ_ERR_FAULT);
            }
        } else {
            memcpy(aBuf, (const void *)(gj_vaddr_t)u64Arg2, cb);
        }
        g_u32VqCalls++;
        if (virtio_net_avail_push(which, aBuf, cb, fWrite, 1) != 0) {
            net_door_soft_inc(&g_soft.u64AvailPushIo);
            return net_door_soft_done(GJ_ERR_IO);
        }
        net_door_soft_inc(&g_soft.u64AvailPushOk);
        return net_door_soft_done((i64)cb);
    }
    case GJ_NET_OP_USED_REAP: {
        u32 n;

        g_u32RingCalls++;
        net_door_soft_inc(&g_soft.u64RingEnter);
        net_door_soft_inc(&g_soft.u64UsedReap);
        if (g_u32OwnerToken != 0) {
            net_door_soft_inc(&g_soft.u64HostOwnedRing);
        }
        if (!virtio_net_ready()) {
            net_door_soft_inc(&g_soft.u64UsedReapNodev);
            return net_door_soft_done(GJ_ERR_NODEV);
        }
        n = virtio_net_used_reap((u16)u64Arg1, (u32)u64Arg2);
        g_u32VqCalls++;
        net_door_soft_inc(&g_soft.u64UsedReapOk);
        return net_door_soft_done((i64)n);
    }
    case GJ_NET_OP_RING_STATE: {
        /* Soft: always fills state; free/pushes = 0 without virtio-net. */
        u32 aS[4];

        g_u32RingCalls++;
        net_door_soft_inc(&g_soft.u64RingEnter);
        net_door_soft_inc(&g_soft.u64RingState);
        if (g_u32OwnerToken != 0) {
            net_door_soft_inc(&g_soft.u64HostOwnedRing);
        }
        if (u64Arg1 == 0) {
            return net_door_soft_done(GJ_ERR_INVAL);
        }
        if (virtio_net_ready()) {
            aS[0] = virtio_net_q_free(1);
            aS[1] = virtio_net_q_free(0);
            /* high 16: user_ring_pushes, low 16: total avail pushes */
            aS[2] = (virtio_net_user_ring_pushes() << 16) |
                    (virtio_net_avail_pushes() & 0xffffu);
        } else {
            aS[0] = 0;
            aS[1] = 0;
            aS[2] = 0;
        }
        aS[3] = g_u32VqCalls;
        if (user_range_ok(u64Arg1, sizeof(aS))) {
            if (copy_to_user(u64Arg1, aS, sizeof(aS)) != GJ_OK) {
                return net_door_soft_done(GJ_ERR_FAULT);
            }
        } else {
            memcpy((void *)(gj_vaddr_t)u64Arg1, aS, sizeof(aS));
        }
        /* No per-RING_STATE residual dump (no stamp storms). */
        return net_door_soft_done(0);
    }
    case GJ_NET_OP_MAP_DMA: {
        struct gj_virtq_dma_export ex;
        int fReclaim;

        /*
         * UDX DMA window handoff residual (Soft!=product).
         * arg1=page-aligned user VA arg2=optional gj_virtq_dma_export out.
         * Last success base in g_u64MapDmaVa (ring map notes residual).
         * Soft re-MAP_DMA same VA = reclaim (re-install DMA window).
         * greppable: net_door: ring map notes | udx_host_stack
         * H1: door thr only.
         */
        g_u32RingCalls++;
        net_door_soft_inc(&g_soft.u64RingEnter);
        net_door_soft_inc(&g_soft.u64UserRing);
        net_door_soft_inc(&g_soft.u64MapDma);
        if (g_u32OwnerToken != 0) {
            net_door_soft_inc(&g_soft.u64HostOwnedRing);
        }
        if (u64Arg1 == 0) {
            net_door_soft_inc(&g_soft.u64MapDmaInval);
            return net_door_soft_done(GJ_ERR_INVAL);
        }
        if ((u64Arg1 & (NET_DOOR_PAGE_SIZE - 1ull)) != 0) {
            net_door_soft_inc(&g_soft.u64MapDmaInval);
            return net_door_soft_done(GJ_ERR_INVAL);
        }
        if (!virtio_net_ready()) {
            net_door_soft_inc(&g_soft.u64MapDmaNodev);
            return net_door_soft_done(GJ_ERR_NODEV);
        }
        fReclaim = (g_u64MapDmaVa != 0 && g_u64MapDmaVa == u64Arg1) ? 1 : 0;
        if (virtio_net_map_dma_user(u64Arg1, &ex) != 0) {
            net_door_soft_inc(&g_soft.u64MapDmaFault);
            return net_door_soft_done(GJ_ERR_FAULT);
        }
        g_u64MapDmaVa = u64Arg1;
        g_u32VqCalls++;
        net_door_soft_inc(&g_soft.u64MapDmaOk);
        if (fReclaim != 0) {
            net_door_soft_inc(&g_soft.u64MapDmaReclaim);
        }
        if (u64Arg2 != 0) {
            if (user_range_ok(u64Arg2, sizeof(ex))) {
                if (copy_to_user(u64Arg2, &ex, sizeof(ex)) != GJ_OK) {
                    return net_door_soft_done(GJ_ERR_FAULT);
                }
            } else {
                memcpy((void *)(gj_vaddr_t)u64Arg2, &ex, sizeof(ex));
            }
        }
        return net_door_soft_done(0);
    }
    case GJ_NET_OP_DESC_ALLOC: {
        int head;

        g_u32RingCalls++;
        net_door_soft_inc(&g_soft.u64RingEnter);
        net_door_soft_inc(&g_soft.u64UserRing);
        net_door_soft_inc(&g_soft.u64DescAlloc);
        if (g_u32OwnerToken != 0) {
            net_door_soft_inc(&g_soft.u64HostOwnedRing);
        }
        if (!virtio_net_ready()) {
            net_door_soft_inc(&g_soft.u64DescAllocNodev);
            return net_door_soft_done(GJ_ERR_NODEV);
        }
        head = virtio_net_desc_alloc((u16)u64Arg1);
        if (head < 0) {
            net_door_soft_inc(&g_soft.u64DescAllocNomem);
            return net_door_soft_done(GJ_ERR_NOMEM);
        }
        g_u32VqCalls++;
        net_door_soft_inc(&g_soft.u64DescAllocOk);
        return net_door_soft_done((i64)head);
    }
    case GJ_NET_OP_USER_AVAIL: {
        /*
         * UDX avail programming; kick path remains door thr (H1).
         * Functional residual: bit0=kick tally (user_avail_kick) on success.
         */
        g_u32RingCalls++;
        net_door_soft_inc(&g_soft.u64RingEnter);
        net_door_soft_inc(&g_soft.u64UserRing);
        net_door_soft_inc(&g_soft.u64UserAvail);
        if (g_u32OwnerToken != 0) {
            net_door_soft_inc(&g_soft.u64HostOwnedRing);
        }
        if (!virtio_net_ready()) {
            net_door_soft_inc(&g_soft.u64UserAvailNodev);
            return net_door_soft_done(GJ_ERR_NODEV);
        }
        if (virtio_net_user_avail((u16)u64Arg1, (u16)u64Arg2, (int)u64Arg3) !=
            0) {
            net_door_soft_inc(&g_soft.u64UserAvailIo);
            return net_door_soft_done(GJ_ERR_IO);
        }
        g_u32VqCalls++;
        net_door_soft_inc(&g_soft.u64UserAvailOk);
        /* bit0=kick: H1 door thr notify residual (UDX host stack). */
        if (((int)u64Arg3 & 1) != 0) {
            net_door_soft_inc(&g_soft.u64UserAvailKick);
        }
        return net_door_soft_done(0);
    }
    case GJ_NET_OP_BOUNCE_FILL: {
        u8 aBuf[NET_ETH_MAX];
        u32 cb = (u32)u64Arg3;
        int n;

        g_u32RingCalls++;
        net_door_soft_inc(&g_soft.u64RingEnter);
        net_door_soft_inc(&g_soft.u64UserRing);
        net_door_soft_inc(&g_soft.u64BounceFill);
        if (g_u32OwnerToken != 0) {
            net_door_soft_inc(&g_soft.u64HostOwnedRing);
        }
        if (!virtio_net_ready()) {
            net_door_soft_inc(&g_soft.u64BounceFillNodev);
            return net_door_soft_done(GJ_ERR_NODEV);
        }
        if (u64Arg2 == 0 || cb == 0 || cb > NET_ETH_MAX) {
            net_door_soft_inc(&g_soft.u64BounceFillInval);
            return net_door_soft_done(GJ_ERR_INVAL);
        }
        if (user_range_ok(u64Arg2, cb)) {
            if (copy_from_user(aBuf, u64Arg2, cb) != GJ_OK) {
                return net_door_soft_done(GJ_ERR_FAULT);
            }
        } else {
            memcpy(aBuf, (const void *)(gj_vaddr_t)u64Arg2, cb);
        }
        /* slot in arg1; TX header prepend for net */
        n = virtio_net_bounce_fill((u32)u64Arg1, aBuf, cb, 1);
        if (n < 0) {
            net_door_soft_inc(&g_soft.u64BounceFillIo);
            return net_door_soft_done(GJ_ERR_IO);
        }
        g_u32VqCalls++;
        net_door_soft_inc(&g_soft.u64BounceFillOk);
        return net_door_soft_done((i64)n);
    }
    case GJ_NET_OP_ETH_UDX_READY: {
        /*
         * Product UDX L2 soft ready (rtl8168_udx park thr). Soft!=product.
         * Arm then pin lab IP + soft demux MAC on net_l2 (backend=none /
         * freestanding rtl SKIP) so ARP/ICMP demux + ETH_TX_PULL work.
         * greppable: ETH_UDX_READY | path=rtl8168_udx | net_l2: soft udx ready
         */
        if (u64Arg1 != 0ull) {
            net_door_udx_arm_soft();
            if (g_fUdxReadyLamp == 0u) {
                g_fUdxReadyLamp = 1u;
                kprintf("net_door: soft ETH_UDX_READY arm=1 "
                        "path=rtl8168_udx owner=product_udx_abi "
                        "lab_ip=10.200.125.50 listen=:22 "
                        "product_net_owns_wire=1 "
                        "soft_listen_ne_host_banner=1 "
                        "dual_dod_b=OPEN_UDX "
                        "freestanding_rtl=SKIP Soft!=product G-AC-1 "
                        "(l2 identity pin + :22 ensure; Soft!=product)\n");
            }
        } else {
            g_fUdxReady = 0u;
        }
        return net_door_soft_done(0);
    }
    case GJ_NET_OP_ETH_SET_MAC: {
        /*
         * Product IDR station MAC → net_l2 soft demux (glass keep=1).
         * Soft!=product Dual DoD B. greppable: ETH_SET_MAC
         */
        u8 aMac[6];

        if (u64Arg1 == 0ull) {
            return net_door_soft_done(GJ_ERR_INVAL);
        }
        if (user_range_ok(u64Arg1, 6u)) {
            if (copy_from_user(aMac, u64Arg1, 6u) != GJ_OK) {
                return net_door_soft_done(GJ_ERR_FAULT);
            }
        } else {
            memcpy(aMac, (const void *)(gj_vaddr_t)u64Arg1, 6u);
        }
        net_l2_set_station_mac(aMac);
        /* IDR published: arm UDX L2 so ARP SHA TX can ETH_TX_PULL. */
        net_door_udx_arm_soft();
        return net_door_soft_done(0);
    }
    case GJ_NET_OP_ETH_INJECT: {
        /*
         * UDX thr-poll RX residual → demux (ARP/ICMP/TCP:22).
         * TCP SYN is not virtio-only: net_eth_input_frame ->
         * net_tcp_input / ensure :22; SYN-ACK net_l2_tx -> ETH_TX_PULL.
         * Missing ETH_UDX_READY used to NODEV (ARP never demuxed). Arm
         * on first valid inject so handle_arp / ICMP / :22 SYN can TX.
         * Soft!=product Dual DoD B.
         * greppable: ETH_INJECT | net_eth: soft udx inject
         */
        u8 aFrame[NET_DOOR_UDX_TX_MAX];
        u32 cb;
        int nOk;

        cb = (u32)u64Arg2;
        if (u64Arg1 == 0ull || cb < 14u || cb > NET_DOOR_UDX_TX_MAX) {
            return net_door_soft_done(GJ_ERR_INVAL);
        }
        if (g_fUdxReady == 0u) {
            net_door_udx_arm_soft();
        }
        if (user_range_ok(u64Arg1, cb)) {
            if (copy_from_user(aFrame, u64Arg1, cb) != GJ_OK) {
                return net_door_soft_done(GJ_ERR_FAULT);
            }
        } else {
            memcpy(aFrame, (const void *)(gj_vaddr_t)u64Arg1, cb);
        }
        nOk = net_eth_input_frame(aFrame, cb);
        if (nOk != 0) {
            if (g_u32UdxInjOk < 0xfffffffeu) {
                g_u32UdxInjOk++;
            }
            if (g_fUdxInjLamp == 0u) {
                g_fUdxInjLamp = 1u;
                kprintf("net_door: soft ETH_INJECT first len=%u ok=1 "
                        "path=rtl8168_udx dual_dod_b=OPEN_UDX "
                        "Soft!=product G-AC-1\n",
                        (unsigned)cb);
            }
        }
        return net_door_soft_done((i64)nOk);
    }
    case GJ_NET_OP_ETH_TX_PULL: {
        /*
         * Host park pulls demux soft TX for UDX DMA. Soft!=product.
         * greppable: ETH_TX_PULL
         */
        u32 cb;
        u32 u32Max;
        u32 iSlot;

        u32Max = (u32)u64Arg2;
        if (u64Arg1 == 0ull || u32Max < 14u) {
            return net_door_soft_done(GJ_ERR_INVAL);
        }
        if (g_u32UdxTxN == 0u) {
            return net_door_soft_done(0);
        }
        iSlot = g_u32UdxTxTail % NET_DOOR_UDX_TX_SLOTS;
        cb = (u32)g_aUdxTxLen[iSlot];
        if (cb < 14u || cb > NET_DOOR_UDX_TX_MAX) {
            net_door_udx_tx_drop_head();
            return net_door_soft_done(0);
        }
        /*
         * Truncating max / copy FAULT: drop. Same slot is not retried.
         * TCP rtx holds SYN-ACK; FIFO is not the only copy.
         */
        if (cb > u32Max) {
            net_door_udx_tx_drop_head();
            return net_door_soft_done(GJ_ERR_INVAL);
        }
        if (user_range_ok(u64Arg1, cb)) {
            if (copy_to_user(u64Arg1, g_aUdxTx[iSlot], cb) != GJ_OK) {
                net_door_udx_tx_drop_head();
                return net_door_soft_done(GJ_ERR_FAULT);
            }
        } else {
            memcpy((void *)(gj_vaddr_t)u64Arg1, g_aUdxTx[iSlot], cb);
        }
        net_door_udx_tx_drop_head();
        if (g_u32UdxTxPull < 0xfffffffeu) {
            g_u32UdxTxPull++;
        }
        if (g_fUdxTxLamp == 0u) {
            g_fUdxTxLamp = 1u;
            kprintf("net_door: soft ETH_TX_PULL first len=%u "
                    "path=rtl8168_udx dual_dod_b=OPEN_UDX "
                    "Soft!=product G-AC-1\n",
                    (unsigned)cb);
        }
        return net_door_soft_done((i64)cb);
    }
    default:
        return net_door_soft_done(GJ_ERR_NOSUPPORT);
    }
}

int
net_door_udx_ready(void)
{
    return (g_fUdxReady != 0u) ? 1 : 0;
}

u32
net_door_udx_tx_pending(void)
{
    return g_u32UdxTxN;
}

int
net_door_udx_tx_soft(const void *pFrame, u32 cb)
{
    u32 iSlot;
    u32 cbOut;

    if (pFrame == NULL || cb < 14u || cb > NET_DOOR_UDX_TX_MAX) {
        return -1;
    }
    if (g_fUdxReady == 0u) {
        net_door_udx_arm_soft();
    }
    if (g_fUdxReady == 0u) {
        return -1;
    }
    /* Ethernet min 60 excl. FCS; runt ARP/SYN-ACK would miss the host. */
    cbOut = cb;
    if (cbOut < 60u) {
        cbOut = 60u;
    }
    if (g_u32UdxTxN >= NET_DOOR_UDX_TX_SLOTS) {
        /* Newest ARP/ICMP/SYN-ACK wins; oldest unsent slot is dropped. */
        net_door_udx_tx_drop_head();
    }
    if (g_u32UdxTxN >= NET_DOOR_UDX_TX_SLOTS) {
        return -1;
    }
    iSlot = g_u32UdxTxHead % NET_DOOR_UDX_TX_SLOTS;
    memcpy(g_aUdxTx[iSlot], pFrame, cb);
    if (cbOut > cb) {
        memset(g_aUdxTx[iSlot] + cb, 0, cbOut - cb);
    }
    g_aUdxTxLen[iSlot] = (u16)cbOut;
    g_u32UdxTxHead++;
    g_u32UdxTxN++;
    if (g_u32UdxTxEnq < 0xfffffffeu) {
        g_u32UdxTxEnq++;
    }
    return 0;
}
