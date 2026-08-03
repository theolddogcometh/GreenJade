/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Freestanding userspace cold personality door server scaffold (G-PERS).
 *
 * Product shape (Option C / DESIGN_SPEC_COMPLETE §3):
 *   ring-3 NATIVE process owns the cold personality door:
 *     gj_ipc_recv → cold serve (libprotonrt) → gj_ipc_reply
 *   replacing kernel kthread cold_personality_server as the default.
 *
 * Scaffold status (this TU only):
 *   - Links as freestanding ELF via user/init/user.ld + libgj
 *   - Soft phases only by default (PERSONALITY_DOOR_LOOP=0) so smoke cannot
 *     hang on IPC_RECV with no door binding
 *   - Soft once markers via native GJ_SYS_DEBUG_LOG (gj_debug_log)
 *
 * Soft markers (prefix-stable — do not rename once smoke greps depend):
 *   personality: soft userspace scaffold PASS
 *   personality-gj: soft serve ready
 *
 * Soft phases (greppable comments; runtime markers above):
 *   phase1 scaffold PASS   — _start→main, DEBUG_LOG works
 *   phase2 soft serve ready — soft once; scaffold complete (≠ product door)
 *
 * When PERSONALITY_DOOR_LOOP=1 (product attach only):
 *   personality-gj: door server up
 *   personality-gj: soft recv-miss / soft serve-miss / soft reply-miss
 *   personality-gj: soft serve ready   (first full recv→serve→reply)
 *   personality-gj: soft would_recv cap (optional smoke cap exit)
 *
 * Planned hard markers (not claimed by this scaffold):
 *   personality-gj: live path PASS
 *
 *   make personality-gj → build/user/personality.elf  (coordinator Makefile)
 *
 * Pure C11 freestanding. Dual-licensed MIT OR Apache-2.0 (no GPL).
 */
#include <gj/syscalls.h>

/*
 * gj_linux_regs wire size is 64 bytes (8 × u64; i64Ret at offset 56).
 * Keep a 128-byte frame for alignment/headroom (matches protonrt-server).
 * Local layout only — full struct lives with the kernel cold path.
 */
#define PERS_REGS_WIRE   64u
#define PERS_REGS_FRAME  128u

/*
 * Soft scaffold default: door loop compiled out so freestanding smoke cannot
 * hang on IPC_RECV with no client/door binding.
 *
 * Enable product park only after cold-door attach:
 *   -DPERSONALITY_DOOR_LOOP=1
 * (see README "Door attach contract").
 */
#ifndef PERSONALITY_DOOR_LOOP
#define PERSONALITY_DOOR_LOOP  0
#endif

/*
 * When PERSONALITY_DOOR_LOOP=1: soft-count consecutive soft-miss recv
 * yields ("would_recv" iterations). 0 = product infinite park (never
 * return). N > 0 = after N soft recv-misses, log once and return so a
 * mis-attached smoke cannot hang forever.
 *
 * Enable with e.g. -DPERSONALITY_WOULD_RECV_CAP=64 alongside DOOR_LOOP=1.
 */
#ifndef PERSONALITY_WOULD_RECV_CAP
#define PERSONALITY_WOULD_RECV_CAP  0
#endif

static void
msg(const char *sz)
{
    size_t n = 0;

    if (sz == 0) {
        return;
    }
    while (sz[n] != '\0') {
        n++;
    }
    /* Native kprint path (GJ_SYS_DEBUG_LOG) — NATIVE personality server. */
    (void)gj_debug_log(sz, (long)n);
}

static void
regs_wire_zero(unsigned char *pFrame)
{
    unsigned i;

    if (pFrame == 0) {
        return;
    }
    for (i = 0; i < PERS_REGS_WIRE; i++) {
        pFrame[i] = 0;
    }
}

/*
 * Soft door_recv loop (product path).
 *
 * When PERSONALITY_DOOR_LOOP is 1 and boot has attached this task as the
 * cold personality door owner, this matches cold_personality_server /
 * protonrt-server: recv → serve → reply; soft miss → yield; never EXIT
 * unless PERSONALITY_WOULD_RECV_CAP > 0 and soft recv-misses hit the cap
 * (smoke-safe escape only — product leaves CAP at 0).
 *
 * Until then the body is compiled out so freestanding smoke cannot hang
 * on IPC_RECV with no client/door binding.
 */
#if PERSONALITY_DOOR_LOOP
static void
personality_door_loop(void)
{
    static unsigned char aFrame[PERS_REGS_FRAME];
    unsigned fSoftReady = 0;
    unsigned fSoftRecv = 0;
    unsigned fSoftReply = 0;
    unsigned fSoftServe = 0;
#if PERSONALITY_WOULD_RECV_CAP > 0
    unsigned fSoftCap = 0;
    unsigned cWouldRecv = 0;
#endif

    msg("personality-gj: door server up\n");

    for (;;) {
        long nRecv;
        long nServe;
        long nReply;

        regs_wire_zero(aFrame);

        /*
         * Soft would_recv: each soft-miss recv iteration counts toward
         * PERSONALITY_WOULD_RECV_CAP (when > 0). Product leaves CAP at 0.
         */
        nRecv = gj_ipc_recv(aFrame); /* door_recv */
        if (nRecv < 0) {
            if (fSoftRecv == 0u) {
                fSoftRecv = 1u;
                msg("personality-gj: soft recv-miss\n");
            }
#if PERSONALITY_WOULD_RECV_CAP > 0
            cWouldRecv++;
            if (cWouldRecv >= (unsigned)PERSONALITY_WOULD_RECV_CAP) {
                if (fSoftCap == 0u) {
                    fSoftCap = 1u;
                    msg("personality-gj: soft would_recv cap\n");
                }
                return;
            }
#endif
            gj_yield();
            continue;
        }

#if PERSONALITY_WOULD_RECV_CAP > 0
        /* Successful recv resets soft-miss would_recv tally. */
        cWouldRecv = 0;
#endif

        /*
         * Interim: kernel cold policy via PERSONALITY_SERVE.
         * Product: call libprotonrt cold path in userspace instead.
         */
        nServe = gj_personality_serve(aFrame);
        if (nServe < 0 && fSoftServe == 0u) {
            fSoftServe = 1u;
            msg("personality-gj: soft serve-miss\n");
        }

        nReply = gj_ipc_reply(nServe); /* door_reply */
        if (nReply < 0) {
            if (fSoftReply == 0u) {
                fSoftReply = 1u;
                msg("personality-gj: soft reply-miss\n");
            }
            gj_yield();
            continue;
        }

        if (fSoftReady == 0u) {
            fSoftReady = 1u;
            msg("personality-gj: soft serve ready\n");
        }
    }
}
#else
/*
 * Scaffold: door loop disabled (PERSONALITY_DOOR_LOOP=0 default).
 *
 * API already in gj/syscalls.h:
 *   gj_ipc_recv / gj_ipc_reply / gj_personality_serve / gj_yield
 *
 * Enable product park after cold-door attach (README "Door attach contract"):
 *   $(CC) … -DPERSONALITY_DOOR_LOOP=1
 * Optional smoke-safe cap (soft-miss would_recv iterations then return):
 *   -DPERSONALITY_DOOR_LOOP=1 -DPERSONALITY_WOULD_RECV_CAP=64
 */
static void
personality_door_loop(void)
{
    /* Soft once: scaffold "serve ready" — not product door serve. */
    msg("personality-gj: soft serve ready\n");

    /* Keep symbols referenced so the TU stays freestanding-link clean. */
    (void)regs_wire_zero;
    (void)PERS_REGS_FRAME;
    (void)PERSONALITY_WOULD_RECV_CAP;
}
#endif

/**
 * Scaffold entry body — greppable soft phases for bring-up honesty.
 * Freestanding link uses _start (user.ld ENTRY); _start calls main.
 *
 * Soft phases (default DOOR_LOOP=0 — never blocks):
 *   1. personality: soft userspace scaffold PASS
 *   2. personality-gj: soft serve ready   (soft once via door_loop stub)
 */
int
main(void)
{
    /* Soft phase 1 — ELF + DEBUG_LOG path alive (prefix-stable). */
    msg("personality: soft userspace scaffold PASS\n");

    /*
     * Soft phase 2 / product path:
     *   DOOR_LOOP=0 → soft once "soft serve ready", then return 0
     *   DOOR_LOOP=1 → park on door (never returns unless WOULD_RECV_CAP>0)
     */
    personality_door_loop();

    return 0;
}

void
_start(void)
{
    int n;

    n = main();
    gj_exit(n);
}
