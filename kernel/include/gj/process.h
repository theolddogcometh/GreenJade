/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Process (task): shared CNode, root meta (slot 0), default pager on PCB,
 * wait4 reaper, G-PROC-5 death (CNode wipe + private AS for wait children).
 * docs/CAP_ADDRESSING.md · docs/APPLE_CHANNEL_REMAINING.md · SOLARIS_STYLE §6
 */
#pragma once

#include <gj/cap.h>
#include <gj/error.h>
#include <gj/fault.h>
#include <gj/types.h>

/*
 * Root meta object installed in CNode slot 0 after bootstrap.
 * Kernel-internal links: self process + this process CNode (not the pager).
 * User-facing: KERNEL OPS ONLY — Identify / kernel-mediated queries.
 * Not a factory for transferable PROCESS/CNODE caps (CAP_ADDRESSING K1–K6).
 * Transferable task handle = GJ_CAP_PROCESS minted to parent on spawn.
 */
struct gj_root_meta {
    struct gj_obj_hdr   hdr;
    struct gj_process  *pProc;  /* kernel pointer; not a user cap */
    struct gj_cnode    *pCnode; /* kernel pointer; not a user cap */
};

/*
 * Process / task (address-space owner).
 * All threads of this process share pCnode.
 * refPager = default pager fallback; regions/objects (Apple) own primary path.
 * Exception port separate from pager (Apple §12) — fields added when wired.
 */
struct gj_process {
    struct gj_obj_hdr    hdr;
    struct gj_cnode     *pCnode;
    struct gj_root_meta *pRootMeta; /* NULL until bootstrap fills slot 0 */
    struct gj_cap_ref    refPager;  /* default pager; gen==0 ⇒ none */
    struct gj_obj_hdr   *pPagerEpObj;
    u32                  u32PagerBadge;
    /*
     * Address space fault serialization (one lock per space).
     * When gj_space exists, this lives on the space; mirrored here until then.
     */
    struct gj_space_fault fault;
    /* Linux personality / JIT (docs/LINUX_ABI_HYBRID.md · PROTON) */
    u32                  u32Personality; /* 0 native, 1 linux */
    u32                  u32Jit;         /* CapJit: allow W|X mprotect */
    /*
     * Soft multi-server confine (OpenBSD promises-shaped bitmask).
     * u32Confined=0 ⇒ ambient authority (bring-up). When set, checks
     * gj_process_promise_ok() before ambient-style ops. Grep: confine soft
     */
    u32                  u32Confined;
    u32                  u32Promises;
    u64                  u64Cr3;         /* 0 = use BSP/boot CR3 (G-AS-1) */
    u64                  u64AnonNext;    /* per-AS mmap cursor */
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
    struct gj_process   *pParent;
    u32                  u32ExitCode;
    u32                  u32Alive;
    /* Exception port (Proton A0–A1 SEH/signal-shaped) */
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
 * Slot 0 left INVALID (empty until bootstrap_root_meta).
 * Pager empty (refPager gen 0).
 */
void gj_process_init(struct gj_process *pProc, struct gj_cnode *pCnode,
                     struct gj_cap_slot *pSlots, u64 cSlots);

/**
 * Install root meta into slot 0: process self + CNode self.
 * Requires pRootMeta storage; links pProc <-> pRootMeta.
 * Returns handle for slot 0 in pOutRef (may be NULL).
 */
gj_status_t gj_process_bootstrap_root_meta(struct gj_process *pProc,
                                           struct gj_root_meta *pMeta,
                                           struct gj_cap_ref *pOutRef);

/**
 * Canonical pager registration (PCB). Does not require well-known CNode slot.
 * Endpoint is named by Scheme A handle in *this* process CNode (or null to clear).
 * v1: stores ref; full impl resolves ENDPOINT + GRANT and holds kernel ref.
 */
gj_status_t gj_process_set_pager(struct gj_process *pProc, u64 u64EpSlot,
                                 u32 u32EpGen);

/** Clear pager (empty again). */
void gj_process_clear_pager(struct gj_process *pProc);

/** Non-zero if pager registered (gen != 0). */
int gj_process_has_pager(const struct gj_process *pProc);

/**
 * Page-fault policy entry (CAP_ADDRESSING fault path).
 * If no pager: returns GJ_ERR_FAULT (caller kills thread) — fail closed.
 * If pager set: returns GJ_ERR_AGAIN until IPC Call to pager is implemented;
 *   full path will Call pager with mono timeout.
 *
 * u64FaultVa / fWrite / fExec describe the fault for future pager payload.
 */
gj_status_t gj_process_handle_fault(struct gj_process *pProc, u64 u64FaultVa,
                                    int fWrite, int fExec);

/** CapJit: allow simultaneous WRITE|EXEC on user maps for this process. */
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

/** Drop ambient: set confined + promises mask (monotonic drop later). */
void gj_process_confine(struct gj_process *pProc, u32 u32Promises);
/** 1 if not confined or promise bit present. NULL ⇒ 0. */
int  gj_process_promise_ok(const struct gj_process *pProc, u32 u32Promise);
/**
 * Soft gate: 0 if ambient or promise present; -13 (LINUX_EACCES shape) else.
 * NULL process ⇒ 0 (no confine subject; product ambient smokes).
 */
int  gj_process_promise_require(const struct gj_process *pProc, u32 u32Promise);
int  gj_process_has_jit(const struct gj_process *pProc);

/* ---- Linux wait4 reaper (interim; product: PROCESS caps G-PROC-5) -------- */

/** Register a child process for wait4 (assigns Linux-shaped pid). Returns pid. */
u32  process_wait_register(struct gj_process *pChild, u32 u32Ppid);
/** Mark process exited (zombie) with code. */
void process_wait_note_exit(struct gj_process *pChild, u32 u32Code);
/** Drop wait-table entry for pProc (cap wait / spawn slot recycle). */
void process_wait_forget(struct gj_process *pProc);
/**
 * wait4-shaped: pid -1 = any child; options bit0 = WNOHANG.
 * Returns child pid, 0 (WNOHANG none), or -errno (ECHILD).
 * *pStatus gets exit status (code << 8) when non-NULL.
 */
i64  process_wait4(i64 i64Pid, i32 *pStatus, int nOptions);

/**
 * G-PROC-5 death: mark zombie, clear pager/exception port, drop regions.
 * For wait-registered children: wipe CNode slots + destroy private AS.
 * Never destroys boot/init AS. Idempotent when already fully torn down.
 */
void process_death(struct gj_process *pProc, u32 u32ExitCode);

/**
 * Linux fork/vfork-shaped: create stub child, register wait4 pid, return pid.
 * fExitNow: immediately mark zombie (vfork). Else schedule deferred exit worker
 * so parent wait4 can reap after a few yields (fork bring-up).
 */
i64 process_linux_fork(u32 u32Ppid, int fExitNow);

/** Look up wait-table pid for a process pointer (0 if unregistered). */
u32 process_wait_pid_of(struct gj_process *pProc);

/**
 * wait4 filtered by parent pid (u32Ppid==0 → any parent, legacy).
 */
i64 process_wait4_ppid(u32 u32Ppid, i64 i64Pid, i32 *pStatus, int nOptions);

/** Mark a wait-registered Linux pid as zombie (exit code). Returns 0 or -ESRCH. */
i64 process_linux_exit_pid(u32 u32Pid, u32 u32Code);
