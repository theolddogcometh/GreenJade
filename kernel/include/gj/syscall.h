/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * GreenJade native syscall numbers + hybrid Linux personality entry.
 *
 * Product contract:
 *   - Native mode: rax/x0 is GJ_SYS_* (this table). Numbers are frozen after
 *     first userland; do not renumber (docs/APPLE_CHANNEL §5).
 *   - Linux mode: rax/x0 is a Linux NR; hybrid hot/cold path (Option C).
 *   - Top-level entry is gj_syscall_dispatch(); personality comes from the
 *     current process PCB (default LINUX until a native app sets 0).
 *
 * Pure C11. Dual-licensed MIT OR Apache-2.0 (project root LICENSE).
 */
#pragma once

#include <gj/linux_abi.h>
#include <gj/types.h>

/* ---- Native GJ syscall numbers (distinct from Linux 0..n) --------------- */
/* Keep sparse blocks by subsystem so new ops can land without renumbering. */

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
 */
enum gj_personality {
    GJ_PERSONALITY_NATIVE = 0,
    GJ_PERSONALITY_LINUX  = 1,
};

/**
 * Register frame for native / top-level dispatch.
 * Arch entry (or smoke tests) fill nr + arg0..5; handlers write i64Ret.
 * Never trust arg pointers without user_range_ok + copy_{to,from}_user.
 */
struct gj_syscall_regs {
    u64 u64Nr;
    u64 u64Arg0;
    u64 u64Arg1;
    u64 u64Arg2;
    u64 u64Arg3;
    u64 u64Arg4;
    u64 u64Arg5;
    i64 i64Ret;
};

/** Bring up cold IPC, futex, and Linux hybrid tables. Call once at boot. */
void gj_syscall_init(void);

/** Override default personality used when no current process is bound. */
void gj_syscall_set_default_personality(enum gj_personality e);
enum gj_personality gj_syscall_get_default_personality(void);

/**
 * Top-level entry (from asm or tests).
 * Routes by current task personality (default LINUX until native apps exist).
 * NULL pRegs is a no-op (defensive).
 */
void gj_syscall_dispatch(struct gj_syscall_regs *pRegs);

/** Force Linux hybrid path (Option C). Declared here; implemented in linux_*. */
void gj_linux_syscall_dispatch(struct gj_linux_regs *pRegs);

/**
 * Native GJ path: switch on GJ_SYS_*.
 * Unimplemented / reserved numbers return GJ_ERR_NOSUPPORT.
 */
void gj_native_syscall_dispatch(struct gj_syscall_regs *pRegs);

/* Stats for smoke / debugging (Linux hybrid path). */
struct gj_linux_dispatch_stats {
    u64 u64HotHits;
    u64 u64ColdHits;
    u64 u64Enosys;
};

void gj_linux_dispatch_stats_get(struct gj_linux_dispatch_stats *pOut);
void gj_linux_dispatch_stats_reset(void);
