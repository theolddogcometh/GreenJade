/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * GreenJade native syscall numbers + hybrid Linux personality entry.
 * Pure C11 freestanding; dual-licensed MIT OR Apache-2.0 (project root LICENSE).
 *
 * -------------------------------------------------------------------------
 * Role
 * -------------------------------------------------------------------------
 * Top-level SYSCALL surface for ring-3 → kernel. Two personalities share one
 * LSTAR/SVC entry; routing is decided from the bound process PCB (or the
 * kernel default personality when no process is bound):
 *
 *   Native  — rax/x0 is GJ_SYS_* (this table); gj_native_syscall_dispatch
 *   Linux   — rax/x0 is a Linux NR; Option C hot/cold hybrid path
 *             (gj_linux_syscall_dispatch → linux_hot / cold_ipc)
 *
 * Product contract
 * ----------------
 *   - Native mode: GJ_SYS_* numbers are frozen after first userland.
 *     Do not renumber; append in sparse subsystem blocks only
 *     (docs/APPLE_CHANNEL_REMAINING.md §5 number stability).
 *   - Linux mode: numbers are the public Linux ABI (gj/linux_abi.h).
 *   - Top-level entry is gj_syscall_dispatch(); personality comes from the
 *     current process PCB (default LINUX until a native app sets 0).
 *   - Arg pointers are never trusted without user_range_ok + copy_{to,from}_user.
 *   - Soft counters wrap OK and never hard-gate product paths.
 *
 * Code map (implementation elsewhere; this header is the contract)
 * ----------------------------------------------------------------
 *   kernel/syscall/entry_bridge.c  — LSTAR bridge soft note
 *   kernel/syscall/dispatch.c      — personality route + entry stats
 *   kernel/syscall/native.c        — GJ_SYS_* switch + native stats
 *   kernel/syscall/linux_*.c       — hybrid Option C (see linux_dispatch.h)
 *   kernel/arch/x86_64/syscall_entry.S — LSTAR → C bridge
 *
 * Soft product surfaces (greppable)
 * ---------------------------------
 *   greppable: SYSCALL_ENTRY_SOFT_STATS
 *   greppable: "syscall: soft stats"
 *   greppable: "native: soft stats"
 *   greppable: GJ_SYS_
 *
 * Related headers
 * ---------------
 *   gj/linux_abi.h, gj/linux_dispatch.h, gj/cold_ipc.h, gj/process.h,
 *   gj/error.h (native returns gj_status_t-shaped i64Ret on many ops)
 * docs/LINUX_ABI_HYBRID.md · docs/PROTON_PERSONALITY.md
 */
#pragma once

#include <gj/linux_abi.h>
#include <gj/types.h>

/* ---- Native GJ syscall numbers (distinct from Linux 0..n) --------------- */
/*
 * Sparse blocks by subsystem so new ops land without renumbering.
 * Convention: arg0..arg5 map to u64Arg0..u64Arg5; result in i64Ret.
 * Negative i64Ret is usually GJ_ERR_* (native) or -LINUX_E* (Linux path).
 * Reserved / stub numbers must return GJ_ERR_NOSUPPORT, not silent success.
 */

/* Diagnostics / scheduling */
#define GJ_SYS_DEBUG_LOG   0  /* arg0=user buf, arg1=len → bytes written */
#define GJ_SYS_YIELD       1  /* voluntary reschedule */
#define GJ_SYS_EXIT        2  /* terminate current thread/process */

/* Door IPC (cold personality / server side) */
#define GJ_SYS_IPC_CALL   10  /* door_call cold personality with arg payload */
#define GJ_SYS_IPC_RECV   11  /* arg0=user ptr to gj_linux_regs */
#define GJ_SYS_IPC_REPLY  12  /* arg0=i64 reply value */

/* Capability table (stubs reserved; numbers frozen) */
#define GJ_SYS_CAP_MINT   20
#define GJ_SYS_CAP_MOVE   21
#define GJ_SYS_CAP_COPY   22
#define GJ_SYS_CAP_REVOKE 23
#define GJ_SYS_CAP_IDENT  24

/* Untyped / VM (stubs reserved) */
#define GJ_SYS_UNTYPED_RETYPE 30
#define GJ_SYS_VM_MAP     40
#define GJ_SYS_VM_UNMAP   41
#define GJ_SYS_VM_PROTECT 42
#define GJ_SYS_VM_MAP_OBJ 43

/* Process control */
#define GJ_SYS_PROCESS_SET_PAGER 50
#define GJ_SYS_PROCESS_SPAWN     51  /* arg0=entry arg1=arg arg2=flags → slot|gen */
#define GJ_SYS_PROCESS_KILL      52

/* Wait / futex (stubs reserved where not yet wired on native path) */
#define GJ_SYS_WAIT_TIMEOUT      60
#define GJ_SYS_FUTEX_WAIT        70
#define GJ_SYS_FUTEX_WAKE        71
#define GJ_SYS_THREAD_SET_QOS    72  /* arg0=tid(0=cur) arg1=qos class */
#define GJ_SYS_THREAD_SET_CPU    73  /* arg0=tid(0=cur) arg1=cpu slot */

/* Cold IPC scaffolds (G-COLD-2: deprecate after userspace door server) */
#define GJ_SYS_COLD_DEQUEUE      80  /* arg0=kernel ptr to gj_cold_request */
#define GJ_SYS_COLD_REPLY        81  /* arg0=token arg1=i64 ret */
/** arg0 = user ptr to gj_linux_regs: fill i64Ret via cold service (G-PERS). */
#define GJ_SYS_PERSONALITY_SERVE 82

/* GPU / compositor present (virtio-gpu) */
/** arg0=width arg1=height arg2=fb_va arg3=stride (0=w*4). Present via virtio-gpu. */
#define GJ_SYS_GPU_PRESENT       90
/** arg0=user ptr to u32[2] for width,height of scanout 0. */
#define GJ_SYS_GPU_DISPLAY_INFO  91

/* Named memory objects (shared scanout / IPC buffers) */
/** Named memobj: arg0=name ptr, arg1=pages. Returns 0 or -errno. */
#define GJ_SYS_MEMOBJ_CREATE_NAMED 92
/** Map named: arg0=name, arg1=hint, arg2=prot. Returns VA or 0. */
#define GJ_SYS_MEMOBJ_MAP_NAMED  93

/**
 * HDA software stream:
 *   arg0 = op (0=open 1=write 2=start 3=tick 4=close 5=stats)
 *   open:  arg1=ch arg2=rate arg3=bits
 *   write: arg1=user ptr arg2=bytes → ret bytes
 *   tick:  arg1=frames → ret bytes consumed
 *   stats: arg1=user ptr to u32[3] {queued,played,underruns}
 */
#define GJ_SYS_HDA_STREAM        94

/**
 * Session door (sessiond hand-off):
 *   arg0 = GJ_SESS_OP_*  present / display_info / input_poll / input_pop / stats
 *   arg1.. = op-specific (see gj/session_door.h)
 */
#define GJ_SYS_SESSION           95
/** Net door: arg0=GJ_NET_OP_* (poll/stats). See gj/net_door.h */
#define GJ_SYS_NET               96
/** Storage door: arg0=GJ_STORE_OP_* (stats). See gj/store_door.h */
#define GJ_SYS_STORE             97

/**
 * Platform info:
 *   arg0=0 → IOMMU: arg1=user ptr to gj_iommu_info; ret=present
 *   arg0=1 → MSI-X inventory: ret=count, optional arg1=info array
 *   arg0=2 → WoW64: arg1=0 query, arg1=1 enable, arg1=2 disable → calls|enabled
 *   arg0=3 → MSI-X IRQ notify: soft inject badge=arg1; ret=irq count
 *   arg0=4 → IOMMU enforce: arg1=0/1 set; ret=enforce
 *   arg0=5 → IOMMU window grant: arg1=BDF (bus<<16|slot<<8|func) arg2=pa arg3=cb
 */
#define GJ_SYS_PLATFORM_INFO     98

/**
 * Notification wait (native):
 *   arg0=which (0=MSI-X global) arg1=mask arg2=block(0/1)
 *   → pending badges cleared and returned
 */
#define GJ_SYS_NOTIFY_WAIT       99

/**
 * VFS door (multi-client mini-FS):
 *   arg0 = GJ_VFS_OP_*  (see gj/vfs_door.h)
 *   arg1.. = op-specific
 */
#define GJ_SYS_VFS               100

/**
 * Console (serial COM1 bring-up):
 *   arg0=0 poll → 0/1 available
 *   arg0=1 getc → byte 0..255 (blocks)
 *   arg0=2 read → arg1=user buf arg2=max; non-blocking drain, ret=bytes
 */
#define GJ_SYS_CONSOLE           101

/**
 * SCSI door (userspace scsi_mid → kernel virtio-scsi / HBA):
 *   arg0 = op: 0=inquiry 1=read_cap 2=read10 3=write10 5=ready 6=stats
 *   inquiry: arg1=user buf arg2=cb
 *   read_cap: arg1=user u32[2] {last_lba, block_len}
 *   read10/write10: arg1=lba arg2=user buf arg3=blocks (512B)
 *   ready: ret 1 if transport ready
 *   stats: arg1=user u32[2] {door_io, ready}
 */
#define GJ_SYS_SCSI              102

/*
 * Personality mode for a task (PCB field when tasks exist).
 * Linux mode: rax is Linux NR; hybrid hot/cold dispatch.
 * Native mode: rax is GJ_SYS_*.
 *
 * Stored on struct gj_process::u32Personality (0/1). Default for unbound
 * dispatch is LINUX so glibc/Proton bring-up works before native apps land.
 */
enum gj_personality {
    GJ_PERSONALITY_NATIVE = 0,
    GJ_PERSONALITY_LINUX  = 1,
};

/**
 * Register frame for native / top-level dispatch.
 *
 * Arch entry (or smoke tests) fill u64Nr + u64Arg0..5; handlers write i64Ret.
 * On x86_64 LSTAR this is rebuilt from the user register save area; on aarch64
 * SVC the same shape is filled from x0..x5 / x8-style NR conventions in the
 * arch bridge (see arch SVC path).
 *
 * Security: never trust arg pointers without user_range_ok + copy_{to,from}_user.
 * Soft: NULL frames are counted and rejected at each entry edge.
 */
struct gj_syscall_regs {
    u64 u64Nr;    /* GJ_SYS_* or Linux NR (personality decides interpretation) */
    u64 u64Arg0;
    u64 u64Arg1;
    u64 u64Arg2;
    u64 u64Arg3;
    u64 u64Arg4;
    u64 u64Arg5;
    i64 i64Ret;   /* out: success ≥0 or negative status / -errno */
};

/**
 * Bring up cold IPC, futex, and Linux hybrid tables.
 * Call once at boot after memory and early CPU are live. Idempotence is
 * implementation-defined; product path calls once from kmain.
 */
void gj_syscall_init(void);

/**
 * Override default personality used when no current process is bound.
 * Soft counters: u64SetPersOk / u64SetPersReject on the entry stats surface.
 */
void gj_syscall_set_default_personality(enum gj_personality e);
enum gj_personality gj_syscall_get_default_personality(void);

/**
 * Top-level entry (from asm or tests).
 * Routes by current task personality (default LINUX until native apps exist).
 * NULL pRegs is a no-op (defensive). Soft counters: SYSCALL_ENTRY_SOFT_STATS.
 *
 * Order: soft-count enter → resolve PCB personality → native or linux path →
 * outcome soft (ret sign buckets + last nr/ret snapshot).
 */
void gj_syscall_dispatch(struct gj_syscall_regs *pRegs);

/**
 * Asm LSTAR edge (syscall_entry.S → C). Soft-notes bridge then dispatches.
 * NULL pRegs is a soft-counted no-op (defensive).
 * Smoke tests that call gj_syscall_dispatch directly skip bridge counters.
 */
void gj_syscall_entry_asm_bridge(struct gj_syscall_regs *pRegs);

/**
 * Soft edge note for LSTAR bridge only.
 * Bumps bridge enter / null; does not route. Smoke tests that call
 * gj_syscall_dispatch directly skip this path (bridge counters stay 0).
 */
void gj_syscall_entry_soft_note_bridge(struct gj_syscall_regs *pRegs);

/**
 * Force Linux hybrid path (Option C). Declared here; implemented in linux_*.
 * See gj/linux_dispatch.h for classify / hot / cold contracts.
 */
void gj_linux_syscall_dispatch(struct gj_linux_regs *pRegs);

/**
 * Native GJ path: switch on GJ_SYS_*.
 * Unimplemented / reserved numbers return GJ_ERR_NOSUPPORT.
 * Soft: gj_native_dispatch_stats_* for smoke (never hard-gate).
 */
void gj_native_syscall_dispatch(struct gj_syscall_regs *pRegs);

/**
 * Soft counters for native GJ_SYS_* dispatch (wrap OK; diagnostics only).
 * Never hard-gate product paths. greppable: native: soft stats
 *
 * Entries / outcomes:
 *   u64Entries     — non-NULL dispatch calls
 *   u64NullGuard   — NULL pRegs early return
 *   u64Handled     — switch hit a known case (not default)
 *   u64Nosupport   — default / reserved NR → GJ_ERR_NOSUPPORT
 *   u64Ok          — i64Ret >= 0 after handler (ret_zero + ret_pos)
 *   u64Err         — i64Ret < 0 after handler
 *   u64RetZero     — i64Ret == 0
 *   u64RetPos      — i64Ret > 0
 *   u64Inval/Fault/Nodev/Again/Io/Nomem/Noent/Perm/Busy — GJ_ERR_* buckets
 *
 * Subsystem buckets (sparse GJ_SYS_* blocks; only counted when handled):
 *   u64Diag        — DEBUG_LOG / YIELD / EXIT
 *   u64Ipc         — IPC_CALL / RECV / REPLY
 *   u64Cap         — CAP_MINT / MOVE / COPY / REVOKE / IDENT
 *   u64Process     — PROCESS_SPAWN (handled); SET_PAGER/KILL also when default
 *   u64Thread      — THREAD_SET_QOS / CPU
 *   u64Cold        — COLD_DEQUEUE / COLD_REPLY / PERSONALITY_SERVE
 *   u64Gpu         — GPU_PRESENT / GPU_DISPLAY_INFO
 *   u64Memobj      — MEMOBJ_CREATE_NAMED / MAP_NAMED
 *   u64Hda         — HDA_STREAM
 *   u64DoorFacade  — SESSION / NET / STORE / VFS (sum of door soft split)
 *   u64Session/Net/Store/Vfs — per-door façade soft split
 *   u64Platform    — PLATFORM_INFO
 *   u64Notify      — NOTIFY_WAIT
 *   u64Console     — CONSOLE
 *   u64Scsi        — SCSI
 *
 * Reserved / unwired NR soft class (default path only; never product gate):
 *   u64Vm          — VM_MAP / UNMAP / PROTECT / MAP_OBJ
 *   u64Futex       — FUTEX_WAIT / FUTEX_WAKE
 *   u64Wait        — WAIT_TIMEOUT
 *   u64Untyped     — UNTYPED_RETYPE
 *   u64UnknownNr   — default NR outside known reserved blocks
 *
 * Copy helpers used by the native path (soft deepen):
 *   u64CopyInOk/Fail, u64CopyOutOk/Fail, u64CopyNameOk/Fail
 *   u64BytesCopyIn / u64BytesCopyOut
 *   u64CopyUser    — copy_{in,out} via user_range_ok path
 *   u64CopyKsmoke  — copy via trusted HHDM/static kernel-smoke path
 *
 * Snapshot: u64LastNr / u64LastRet (bit pattern of last i64Ret).
 */
struct gj_native_dispatch_stats {
    u64 u64Entries;
    u64 u64NullGuard;
    u64 u64Handled;
    u64 u64Nosupport;
    u64 u64Ok;
    u64 u64Err;
    u64 u64RetZero;
    u64 u64RetPos;
    u64 u64Inval;
    u64 u64Fault;
    u64 u64Nodev;
    u64 u64Again;
    u64 u64Io;
    u64 u64Nomem;
    u64 u64Noent;
    u64 u64Perm;
    u64 u64Busy;
    u64 u64Diag;
    u64 u64Ipc;
    u64 u64Cap;
    u64 u64Process;
    u64 u64Thread;
    u64 u64Cold;
    u64 u64Gpu;
    u64 u64Memobj;
    u64 u64Hda;
    u64 u64DoorFacade;
    u64 u64Session;
    u64 u64Net;
    u64 u64Store;
    u64 u64Vfs;
    u64 u64Platform;
    u64 u64Notify;
    u64 u64Console;
    u64 u64Scsi;
    u64 u64Vm;
    u64 u64Futex;
    u64 u64Wait;
    u64 u64Untyped;
    u64 u64UnknownNr;
    u64 u64CopyInOk;
    u64 u64CopyInFail;
    u64 u64CopyOutOk;
    u64 u64CopyOutFail;
    u64 u64CopyNameOk;
    u64 u64CopyNameFail;
    u64 u64BytesCopyIn;
    u64 u64BytesCopyOut;
    u64 u64CopyUser;
    u64 u64CopyKsmoke;
    u64 u64LastNr;
    u64 u64LastRet;
};

/** Snapshot soft counters into *pOut (NULL → no-op). Never hard-gates. */
void gj_native_dispatch_stats_get(struct gj_native_dispatch_stats *pOut);
/** Clear soft counters (lifetime restarts). Safe to call anytime. */
void gj_native_dispatch_stats_reset(void);
/**
 * Greppable soft line: "native: soft stats ...".
 * Diagnostics only; wrap OK; never hard-gates product paths.
 * Snapshots counters before printing (soft race hygiene).
 * Returns u64Entries (handy for smoke assert chains).
 */
u64 gj_native_dispatch_stats_soft(void);

/**
 * Coarse Linux hybrid hit counters (hot vs cold vs ENOSYS).
 * Deeper NR-table inventory lives in gj_linux_nr_class_stats (linux_dispatch.h).
 * Kept here for historical smoke that includes syscall.h only.
 */
struct gj_linux_dispatch_stats {
    u64 u64HotHits;
    u64 u64ColdHits;
    u64 u64Enosys;
};

void gj_linux_dispatch_stats_get(struct gj_linux_dispatch_stats *pOut);
void gj_linux_dispatch_stats_reset(void);

/*
 * Soft deepen: top-level SYSCALL entry / personality-route counters.
 * Distinct from gj_native_dispatch_stats / gj_linux_dispatch_stats (those
 * cover handler interiors). This surface is LSTAR bridge + route only.
 * Wrap OK; diagnostics / smoke — never hard-gate product paths.
 * greppable: SYSCALL_ENTRY_SOFT_STATS
 * greppable: "syscall: soft stats"
 *
 * Bridge (entry_bridge.c / LSTAR):
 *   u64BridgeEnter / u64BridgeNull
 * Dispatch route (dispatch.c; asm + smoke):
 *   u64DispatchEnter / u64DispatchNull
 *   u64Native / u64Linux
 *   u64PcbNative / u64PcbLinux / u64DefaultPers
 *   u64Bound / u64Unbound
 * Lifecycle:
 *   u64Init / u64SetPersOk / u64SetPersReject
 * Outcome soft (after route writes i64Ret):
 *   u64Complete / u64RetNeg / u64RetZero / u64RetPos
 *   u64LastNr / u64LastRetBits
 * Live mirrors (0/1, not cumulative):
 *   u64DefaultIsLinux / u64DefaultIsNative
 */
struct gj_syscall_entry_stats {
    u64 u64BridgeEnter;
    u64 u64BridgeNull;
    u64 u64DispatchEnter;
    u64 u64DispatchNull;
    u64 u64Native;
    u64 u64Linux;
    u64 u64PcbNative;
    u64 u64PcbLinux;
    u64 u64DefaultPers;
    u64 u64Bound;
    u64 u64Unbound;
    u64 u64Init;
    u64 u64SetPersOk;
    u64 u64SetPersReject;
    u64 u64Complete;
    u64 u64RetNeg;
    u64 u64RetZero;
    u64 u64RetPos;
    u64 u64LastNr;
    u64 u64LastRetBits;
    u64 u64DefaultIsLinux;
    u64 u64DefaultIsNative;
};

/** Snapshot entry soft counters into *pOut. NULL pOut is a no-op. */
void gj_syscall_entry_stats_get(struct gj_syscall_entry_stats *pOut);

/** Zero cumulative entry soft counters; refresh default-personality mirrors. */
void gj_syscall_entry_stats_reset(void);

/**
 * Greppable soft stats line: "syscall: soft stats ..."
 * Returns u64DispatchEnter (handy for smoke without parsing).
 */
u64 gj_syscall_entry_stats_soft(void);
