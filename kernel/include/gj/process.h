/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Process (task): shared CNode, root meta (slot 0), default pager on PCB,
 * wait4 reaper, G-PROC-5 death (CNode wipe + private AS for wait children).
 *
 * Rules (docs/DESIGN_SPEC_COMPLETE.md §6 · CAP_ADDRESSING · SOLARIS_STYLE):
 *   G-PROC-1  spawn: PROCESS + AS + CNode + root meta + first thread
 *   G-PROC-2  parent receives PROCESS task cap (not root meta)
 *   G-PROC-4  no ambient full-CNode fork (security core)
 *   G-PROC-5  death: clear pager, wipe CNode (wait children), unmap private
 *             AS, refund quota/CDT; never destroy boot/init AS
 *
 * Soft deepen (this surface / kernel/proc/process.c):
 *   pager   ep ref hold + badge snap + optional slot-1 mirror; LIVE refresh
 *   wait    reparent orphans → init(1); WNOWAIT peek; live/zombie counts
 *   death   quota+CDT-aware CNode clear; exec/auxv scrub; fault-lock force
 *   confine OpenBSD-shaped promise bits (ambient until confined)
 *
 * Grep markers:
 *   process:pager ref | process:pager slot1 | process: set_pager
 *   process: wait register | process: zombie | process: wait reparent
 *   process:death cnode | process: cnode_clear | process: as_destroy
 *   process: death exit= (G-PROC-5)
 *
 * Product spawn / kill / wait-by-cap live in <gj/spawn.h> (G-PROC-*).
 * This header is PCB + bootstrap + pager policy + Linux wait4 interim.
 *
 * docs/CAP_ADDRESSING.md · docs/APPLE_CHANNEL_REMAINING.md ·
 * docs/SOLARIS_STYLE_REMAINING.md §6 · §9 · docs/LINUX_ABI_HYBRID.md
 */
#pragma once

#include <gj/cap.h>
#include <gj/error.h>
#include <gj/fault.h>
#include <gj/types.h>

/*
 * Optional user-visible pager mirror (CAP_ADDRESSING optional slot 1).
 * Canonical pager remains PCB refPager / pPagerEpObj; slot is introspection
 * soft only (READ|IDENTIFY, +GRANT if source had it — not ambient MAP).
 * Cleared with pager; install fails soft if CNode too small.
 */
#ifndef GJ_CAP_SLOT_PAGER
#define GJ_CAP_SLOT_PAGER 1ull
#endif

/* wait4 option bits (Linux-shaped soft; WNOHANG is the product path). */
#define GJ_WAIT_WNOHANG    1         /* return 0 if no zombie ready */
#define GJ_WAIT_WUNTRACED  2         /* catalog; stop not modelled */
#define GJ_WAIT_WCONTINUED 8         /* catalog; continue not modelled */
#define GJ_WAIT_WNOWAIT    0x01000000 /* soft: peek zombie, do not reap */

/*
 * Root meta object installed in CNode slot 0 after bootstrap.
 * Kernel-internal links: self process + this process CNode (not the pager).
 * User-facing: KERNEL OPS ONLY — Identify / kernel-mediated queries.
 * Not a factory for transferable PROCESS/CNODE caps (CAP_ADDRESSING K1–K6).
 * Transferable task handle = GJ_CAP_PROCESS minted to parent on spawn
 * (see process_spawn / GJ_SPAWN_PROCESS_RIGHTS_* in spawn.h).
 */
struct gj_root_meta {
    struct gj_obj_hdr   hdr;    /* GJ_CAP_ROOT_META; first field */
    struct gj_process  *pProc;  /* kernel pointer; not a user cap */
    struct gj_cnode    *pCnode; /* kernel pointer; not a user cap */
};

/*
 * Process / task (address-space owner).
 *
 * Threads of this process share pCnode (Scheme A handles).
 * refPager = default pager fallback; regions/objects (Apple channel) own the
 * primary map path. Exception port is separate from pager (Apple §12) —
 * fields live in excPort (Proton A0–A1 SEH/signal-shaped).
 *
 * u64Cr3 == 0 ⇒ inherit BSP/boot CR3 until process_as_ensure (G-AS-1).
 * Wait-registered children get private AS destroyed on process_death;
 * boot/init long-lived PCBs keep their AS.
 */
struct gj_process {
    struct gj_obj_hdr    hdr;           /* first: PROCESS / task object */
    struct gj_cnode     *pCnode;
    struct gj_root_meta *pRootMeta;     /* NULL until bootstrap fills slot 0 */
    struct gj_cap_ref    refPager;      /* Scheme A handle; gen==0 ⇒ none */
    struct gj_obj_hdr   *pPagerEpObj;   /* soft kernel hold while named */
    u32                  u32PagerBadge; /* door badge snap or explicit */
    /*
     * Address space fault serialization (one lock per space).
     * When gj_space exists, this lives on the space; mirrored here until then.
     * process_death force-clears so death cannot leave AS locked.
     */
    struct gj_space_fault fault;
    /* Linux personality / JIT (docs/LINUX_ABI_HYBRID.md · PROTON) */
    u32                  u32Personality; /* 0 native, 1 linux (init default) */
    u32                  u32Jit;         /* CapJit cache: allow W|X mprotect */
    /*
     * Soft multi-server confine (OpenBSD promises-shaped bitmask).
     * u32Confined=0 ⇒ ambient authority (bring-up). When set, gates call
     * gj_process_promise_ok() before ambient-style ops.
     * Grep: confine soft
     */
    u32                  u32Confined;
    u32                  u32Promises;   /* allowed bits when confined */
    u64                  u64Cr3;         /* 0 = use BSP/boot CR3 (G-AS-1) */
    u64                  u64AnonNext;    /* per-AS mmap cursor (default 1G) */
    /* Last execve image facts (auxv / dynlinker handoff — G-ELF) */
    u64                  u64ExecEntry;   /* main binary entry (post-bias) */
    u64                  u64InterpEntry; /* PT_INTERP entry or 0 */
    u64                  u64LoadBias;    /* main ET_DYN bias */
    u64                  u64ExecStack;   /* SP for post-exec (handoff stack) */
    u64                  u64StartEntry;  /* INTERP-first or main entry */
    u32                  u32StartThr;    /* live ring-3 thr for start entry */
    u32                  u32ExecFlags;   /* GJ_ELF_INFO_* mirror */
    u32                  cNeededLoaded;  /* DT_NEEDED SOs mapped into AS */
    u32                  cAuxv;          /* number of auxv key/value pairs */
#define GJ_PROC_AUXV_MAX 24u
    u64                  aAuxv[GJ_PROC_AUXV_MAX * 2u]; /* key,value,... */
    char                 szExecPath[128]; /* last execve path (AT_EXECFN) */
    /* Regions: views onto memory objects (G-MO) — fixed table for bring-up */
#define GJ_PROC_REGION_MAX 32u
    struct {
        u8               u8Used;
        u8               u8Pad[3];
        u32              u32Prot;
        gj_vaddr_t       vaBase;
        size_t           cbLen;
        u64              u64ObjOff;
        void            *pObj; /* struct gj_memobj * */
    } aRegions[GJ_PROC_REGION_MAX];
    struct gj_process   *pParent;       /* soft link from wait table parent */
    u32                  u32ExitCode;
    u32                  u32Alive;      /* 1 live, 0 after note_exit/death */
    /* Exception port (Proton A0–A1 SEH/signal-shaped; see except.h) */
    struct {
        u8  u8Live;
        u8  u8Pending;
        u8  u8Pad[2];
        u32 u32HandlerThr;
        u32 u32Vec;
        u32 u32Count;
        u64 u64Error;
        u64 u64Rip;
        u64 u64Cr2;
    } excPort;
};

/* ---- Bootstrap / lifecycle ---------------------------------------------- */

/**
 * Initialize process + CNode over caller-provided storage.
 * Zeros pager, regions, exec facts, excPort; sets u32Alive=1,
 * personality=linux (1), promises=ALL ambient, anon cursor 0x40000000.
 * Slot 0 left INVALID until bootstrap_root_meta. Pager empty (gen 0).
 * NULL args: no-op.
 */
void gj_process_init(struct gj_process *pProc, struct gj_cnode *pCnode,
                     struct gj_cap_slot *pSlots, u64 cSlots);

/**
 * Install root meta into slot 0: process self + CNode self.
 * Requires pRootMeta storage; links pProc <-> pRootMeta.
 * Rights on slot 0: READ|IDENTIFY only (ROOT_META, not transferable PROCESS).
 * Returns GJ_OK and optional handle in *pOutRef; GJ_ERR_BUSY if already set;
 * GJ_ERR_INVAL if null args / no CNode; or slot-install status.
 */
gj_status_t gj_process_bootstrap_root_meta(struct gj_process *pProc,
                                           struct gj_root_meta *pMeta,
                                           struct gj_cap_ref *pOutRef);

/**
 * Canonical pager registration (PCB). Does not require a well-known CNode slot.
 * Endpoint is named by Scheme A handle in *this* process CNode
 * (u32EpGen==0 clears). Soft (SOLARIS_STYLE §9):
 *   ENDPOINT type + GRANT rights, LIVE check (fail closed → GJ_ERR_DEAD),
 *   kernel u32Ref hold on ep object, badge snap via door_get_badge(),
 *   optional slot-1 mirror; replace drops prior hold after new hold.
 * Grep: process: set_pager | process:pager ref
 */
gj_status_t gj_process_set_pager(struct gj_process *pProc, u64 u64EpSlot,
                                 u32 u32EpGen);

/**
 * Soft: set pager with explicit badge.
 * u32Badge==0 ⇒ snap door_get_badge() from the resolved endpoint;
 * non-zero stores the caller badge on the PCB (msg.u32Flags stamp later).
 * Same resolve / rights / LIVE rules as gj_process_set_pager.
 */
gj_status_t gj_process_set_pager_badge(struct gj_process *pProc, u64 u64EpSlot,
                                       u32 u32EpGen, u32 u32Badge);

/**
 * Clear pager (empty again). Soft: drop kernel ep ref; clear slot-1 mirror;
 * zero badge. Safe on already-empty.
 */
void gj_process_clear_pager(struct gj_process *pProc);

/**
 * Non-zero if pager registered (gen != 0) and endpoint still LIVE when
 * pPagerEpObj is known. Const path does not clear; use pager_refresh to drop
 * dead endpoints. NULL process ⇒ 0.
 */
int gj_process_has_pager(const struct gj_process *pProc);

/** Soft: PCB pager badge (0 if none / NULL process). */
u32 gj_process_pager_badge(const struct gj_process *pProc);

/**
 * Soft: if registered pager endpoint is not LIVE (DEAD/REVOKING), clear PCB
 * pager (ep-revoke hook surface; full global scan later).
 * Grep: process: pager refresh clear
 */
void gj_process_pager_refresh(struct gj_process *pProc);

/**
 * Page-fault policy entry (CAP_ADDRESSING fault path · SOLARIS_STYLE §7).
 * Serializes on pProc->fault. Soft-refreshes pager first.
 *
 *   no pager  → GJ_ERR_FAULT (caller kills thread) — fail closed
 *   has pager → builds 1-page fault cluster + map cookie, stamps badge into
 *               msg flags, invalidates cookie, returns GJ_ERR_AGAIN until
 *               IPC Call to pager is implemented (full path: mono timeout)
 *   W|X both  → GJ_ERR_PERM at policy layer (W^X)
 *
 * u64FaultVa / fWrite / fExec describe the fault for future pager payload.
 */
gj_status_t gj_process_handle_fault(struct gj_process *pProc, u64 u64FaultVa,
                                    int fWrite, int fExec);

/**
 * CapJit: allow simultaneous WRITE|EXEC on user maps for this process.
 * Caches GJ_RIGHT_JIT authority (G-JIT-4); true source is task-cap right
 * when mint-wired (G-JIT-1).
 */
void gj_process_set_jit(struct gj_process *pProc, int fEnable);

/* Soft confine promise bits (OpenBSD-shaped names; clean-room values). */
#define GJ_PROMISE_STDIO   (1u << 0)
#define GJ_PROMISE_RPATH   (1u << 1)
#define GJ_PROMISE_WPATH   (1u << 2)
#define GJ_PROMISE_CPATH   (1u << 3)
#define GJ_PROMISE_INET    (1u << 4)
#define GJ_PROMISE_PROC    (1u << 5)
#define GJ_PROMISE_EXEC    (1u << 6)
#define GJ_PROMISE_ALL     0xffffffffu

/**
 * Drop ambient: set confined=1 + promises mask.
 * Soft: not yet monotonic (re-confine may widen); product will only drop.
 */
void gj_process_confine(struct gj_process *pProc, u32 u32Promises);

/** 1 if not confined or promise bit present. NULL process ⇒ 0. */
int  gj_process_promise_ok(const struct gj_process *pProc, u32 u32Promise);

/**
 * Soft gate for ambient-style ops: 0 if ambient or promise present;
 * -13 (LINUX_EACCES shape) else. NULL process ⇒ 0 (no confine subject;
 * product ambient smokes treat null as unrestricted).
 */
int  gj_process_promise_require(const struct gj_process *pProc, u32 u32Promise);

/** CapJit cache non-zero? NULL ⇒ 0. */
int  gj_process_has_jit(const struct gj_process *pProc);

/* ---- Linux wait4 reaper (interim; product: PROCESS caps G-PROC-5) -------- */
/*
 * Fixed zombie table (64 slots, pids from 100). Product reaper is PROCESS
 * task-cap wait (spawn.h process_wait). This path services Linux wait4 /
 * fork stubs and gates G-PROC-5 AS/CNode reclaim (wait-registered only).
 */

/**
 * Register a child for wait4. Assigns Linux-shaped pid (≥100).
 * Soft-links pChild->pParent when parent PCB is still in the table.
 * Returns pid, or 0 if table full (caller may continue without wait4).
 * Idempotent: re-register of same child returns existing pid.
 * Grep: process: wait register
 */
u32  process_wait_register(struct gj_process *pChild, u32 u32Ppid);

/**
 * Mark process exited (zombie) with code. Updates PCB u32Alive/exit and
 * wait-slot zombie flag. Soft: re-note updates exit code if already zombie.
 * Grep: process: zombie
 */
void process_wait_note_exit(struct gj_process *pChild, u32 u32Code);

/**
 * Drop wait-table entry for pProc (cap wait / spawn slot recycle).
 * Does not run process_death — only frees the reaper slot.
 */
void process_wait_forget(struct gj_process *pProc);

/**
 * wait4-shaped: pid -1 = any child of the implicit parent filter;
 * options WNOHANG / soft WNOWAIT (peek without reap).
 * Returns child pid, 0 (WNOHANG none), or -errno (ECHILD).
 * *pStatus gets exit status (code << 8) when non-NULL.
 */
i64  process_wait4(i64 i64Pid, i32 *pStatus, int nOptions);

/**
 * G-PROC-5 death: mark zombie, clear pager/exception port, drop region views.
 * Soft deepen:
 *   - orphan reparent unreaped children → init (ppid 1)
 *   - wait-registered only: quota+CDT CNode wipe + revoke walk + exec scrub
 *   - private AS destroy when CR3 is non-kernel (save/restore caller CR3)
 * Never destroys boot/init AS. Idempotent when already fully torn down
 * (second call only re-notes zombie code).
 * Grep: process: death exit= | process: cnode_clear | process: as_destroy
 */
void process_death(struct gj_process *pProc, u32 u32ExitCode);

/**
 * Linux fork/vfork-shaped: create stub child PCB, register wait4 pid.
 * fExitNow: immediately process_death (vfork). Else schedule deferred exit
 * worker (two yields) so parent wait4 can reap after fork returns.
 * Returns child pid or -EAGAIN (-11) if stub table full (16).
 * Not a full AS/CNode clone (G-PROC-4); product path is process_spawn.
 */
i64 process_linux_fork(u32 u32Ppid, int fExitNow);

/** Look up wait-table pid for a process pointer (0 if unregistered). */
u32 process_wait_pid_of(struct gj_process *pProc);

/**
 * wait4 filtered by parent pid (u32Ppid==0 → any parent, legacy).
 * Same return contract as process_wait4.
 */
i64 process_wait4_ppid(u32 u32Ppid, i64 i64Pid, i32 *pStatus, int nOptions);

/**
 * Mark a wait-registered Linux pid as zombie (exit code).
 * Returns 0 or -ESRCH if pid not found.
 */
i64 process_linux_exit_pid(u32 u32Pid, u32 u32Code);

/**
 * Soft wait-table observability (reaper deepen).
 * u32Ppid==0 → any parent. Counts unreaped used slots matching filter.
 * live = not zombie; zombie = unreaped zombies.
 */
u32 process_wait_live_count(u32 u32Ppid);
u32 process_wait_zombie_count(u32 u32Ppid);

/**
 * Soft: reparent unreaped children of u32OldPpid onto u32NewPpid (init=1).
 * Used by process_death (G-PROC-5 orphan path). Clears soft pParent link.
 * Returns count reparented. No-op if old/new zero or equal.
 * Grep: process: wait reparent
 */
u32 process_wait_reparent(u32 u32OldPpid, u32 u32NewPpid);
