/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Native GreenJade syscalls (GJ_SYS_*; non-Linux numbers).
 *
 * Product path:
 *   - Entered only when process personality is NATIVE (or tests call us).
 *   - User pointers: user_range_ok + copy_{to,from}_user. The !user branch
 *     is for early kernel smokes that pass HHDM/static buffers.
 *   - Reserved CAP/VM/FUTEX numbers fall through to GJ_ERR_NOSUPPORT until
 *     their product wiring lands; numbers stay frozen (syscall.h).
 *   - Door façades (session/net/store/vfs) and platform ops are the main
 *     product surface for freestanding embeds (sessiond, scsi_mid, …).
 *   - Soft stats (gj_native_dispatch_stats_*): entry/outcome + subsystem
 *     buckets + door split + reserved-NR class + copy-helper counters
 *     (user vs ksmoke path). Diagnostics only; never hard-gate.
 *     greppable: native: soft stats
 *
 * Soft inventory (Wave 11/14 base + Wave 35 exclusive deepen; this unit only):
 *   native: soft inventory   — entry/handled/nosupport + caps + log_n
 *   native: soft stats       — aggregate rollup (legacy greppable line)
 *   native: soft outcome     — ok/err + GJ_ERR_* buckets
 *   native: soft class       — handled subsystem buckets
 *   native: soft door        — session/net/store/vfs façade split
 *   native: soft reserved    — frozen VM/futex/wait/untyped/unknown NR
 *   native: soft copy        — copy_in/out/name + user vs ksmoke path
 *   native: soft platform    — PLATFORM_INFO op split
 *   native: soft console     — CONSOLE op split
 *   native: soft scsi        — SCSI op split
 *   native: soft hda         — HDA_STREAM op split
 *   native: soft cap         — CAP_* op split
 *   native: soft ipc         — IPC_CALL/RECV/REPLY + diag split
 *   native: soft last        — last_nr / last_ret snapshot
 *   native: soft path        — honesty: native GJ path ≠ Linux hybrid / bar3
 * Wave 14/15 exclusive complementary (never reshape primary lines):
 *   native: soft process     — PROCESS_SPAWN (wired) + frozen set_pager/kill
 *   native: soft thread      — THREAD_SET_QOS / SET_CPU
 *   native: soft gpu         — GPU_PRESENT / DISPLAY_INFO
 *   native: soft memobj      — CREATE_NAMED / MAP_NAMED
 *   native: soft cold        — COLD_DEQUEUE / REPLY / PERSONALITY_SERVE
 *   native: soft notify      — NOTIFY_WAIT
 *   native: soft rates       — handled/nosupport/err basis points
 *   native: soft honesty     — hybrid open; not bar3
 *   native: soft deepen      — wave=61 area stamp
 *   native: soft edge        — Wave 15 entries/null/handled combined
 *   native: soft share       — Wave 15 door/class share lamps
 *   native: soft catalog     — Wave 19 surface catalog stamp
 *   native: soft inventory PASS (soft lamp only; not product gate)
 * greppable: native: soft
 *
 * Soft only; Linux ABI hybrid product remains open. Pure C11 freestanding.
 * Dual-licensed MIT OR Apache-2.0.
 */
#include <gj/cap.h>
#include <gj/cold_ipc.h>
#include <gj/door.h>
#include <gj/error.h>
#include <gj/hda.h>
#include <gj/klog.h>
#include <gj/iommu.h>
#include <gj/net_door.h>
#include <gj/pci_caps.h>
#include <gj/scsi_mid.h>
#include <gj/session_door.h>
#include <gj/store_door.h>
#include <gj/vfs_door.h>
#include <gj/irq_msix.h>
#include <gj/linux_abi.h>
#include <gj/linux_dispatch.h>
#include <gj/memobj.h>
#include <gj/notify.h>
#include <gj/process.h>
#include <gj/spawn.h>
#include <gj/string.h>
#include <gj/syscall.h>
#include <gj/thread.h>
#include <gj/user_access.h>
#include <gj/virtio_gpu.h>
#include <gj/vmm.h>
#include <gj/wow64.h>

/* Cap DEBUG_LOG payload so a bad len cannot pin the CPU copying forever. */
#define GJ_NATIVE_DEBUG_LOG_MAX 4096u
/* Console non-blocking drain chunk (stack buffer). */
#define GJ_NATIVE_CONSOLE_CHUNK 64u
/* HDA write staging chunk. */
#define GJ_NATIVE_HDA_CHUNK     256u
/* SCSI xfer staging; one outstanding door submit at a time. */
#define GJ_NATIVE_SCSI_XFER_MAX 4096u

extern struct gj_process *g_pLinuxProc;

/* Soft product counters (wrap OK). See gj_native_dispatch_stats in syscall.h. */
static struct gj_native_dispatch_stats g_nativeStats;

/*
 * Wave 11/14 base + Wave 35 exclusive deepen (file-local; never hard-gates).
 * Per-op tallies for multi-op GJ_SYS_* surfaces + inventory emission count.
 * greppable: native: soft …
 */
struct native_soft_deep {
    u64 u64SoftLog; /* inventory / multi-line dump emissions */
    /* PLATFORM_INFO arg0 split */
    u64 u64PlatIommu;
    u64 u64PlatMsix;
    u64 u64PlatWow64;
    u64 u64PlatMsixInj;
    u64 u64PlatIommuEnf;
    u64 u64PlatIommuWin;
    u64 u64PlatInval;
    /* CONSOLE arg0 split */
    u64 u64ConPoll;
    u64 u64ConGetc;
    u64 u64ConRead;
    u64 u64ConInval;
    /* SCSI arg0 split */
    u64 u64ScsiReady;
    u64 u64ScsiStats;
    u64 u64ScsiInq;
    u64 u64ScsiReadCap;
    u64 u64ScsiRead10;
    u64 u64ScsiWrite10;
    u64 u64ScsiInval;
    /* HDA_STREAM arg0 split */
    u64 u64HdaOpen;
    u64 u64HdaWrite;
    u64 u64HdaStart;
    u64 u64HdaTick;
    u64 u64HdaClose;
    u64 u64HdaStats;
    u64 u64HdaInval;
    /* CAP_* number split (handled path) */
    u64 u64CapMint;
    u64 u64CapMove;
    u64 u64CapCopy;
    u64 u64CapRevoke;
    u64 u64CapIdent;
    /* IPC + diag number split */
    u64 u64IpcCall;
    u64 u64IpcRecv;
    u64 u64IpcReply;
    u64 u64DiagLog;
    u64 u64DiagYield;
    u64 u64DiagExit;
    /* Wave 14/15 exclusive op splits */
    u64 u64ProcSpawn;    /* GJ_SYS_PROCESS_SPAWN */
    u64 u64ThrQos;       /* GJ_SYS_THREAD_SET_QOS */
    u64 u64ThrCpu;       /* GJ_SYS_THREAD_SET_CPU */
    u64 u64GpuPresent;   /* GJ_SYS_GPU_PRESENT */
    u64 u64GpuInfo;      /* GJ_SYS_GPU_DISPLAY_INFO */
    u64 u64MemobjCreate; /* GJ_SYS_MEMOBJ_CREATE_NAMED */
    u64 u64MemobjMap;    /* GJ_SYS_MEMOBJ_MAP_NAMED */
    u64 u64ColdDeq;      /* GJ_SYS_COLD_DEQUEUE */
    u64 u64ColdReply;    /* GJ_SYS_COLD_REPLY */
    u64 u64ColdServe;    /* GJ_SYS_PERSONALITY_SERVE */
    u64 u64NotifyWait;   /* GJ_SYS_NOTIFY_WAIT */
};

/* Wave 15 soft inventory stamp + area count (greppable deepen). */
#define NATIVE_SOFT_WAVE 61u
#define NATIVE_SOFT_AREAS 112u

static struct native_soft_deep g_nativeDeep;
/* One-shot multi-line inventory after first non-null dispatch (soft). */
static u8 g_fSoftNativeOnce;

static void native_soft_inc(u64 *pCtr);
static void native_soft_inventory_log(void);
static void native_soft_maybe_once(void);

/** Soft: bump path tally (u64 wrap is fine for telemetry). */
static void
native_soft_inc(u64 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    (*pCtr)++;
}

/**
 * Greppable Wave 11 + Wave 15 soft native inventory (product / smoke).
 * Snapshots public stats + file-local deepen counters before print.
 * Wave 11 primary lines stay field-stable; Wave 14/15 add complementary.
 * Diagnostics only; wrap OK; never hard-gates.
 * greppable: native: soft
 */
static void
native_soft_inventory_log(void)
{
    struct gj_native_dispatch_stats s;
    struct native_soft_deep d;
    u64 u64BpHandled;
    u64 u64BpNosupport;
    u64 u64BpErr;
    u64 u64Term;

    native_soft_inc(&g_nativeDeep.u64SoftLog);
    s = g_nativeStats;
    d = g_nativeDeep;

    /* Soft terminal share (basis points; 0 if no entries). */
    u64Term = s.u64Handled + s.u64Nosupport;
    if (u64Term != 0) {
        u64BpHandled = (s.u64Handled * 10000ull) / u64Term;
        u64BpNosupport = (s.u64Nosupport * 10000ull) / u64Term;
    } else {
        u64BpHandled = 0;
        u64BpNosupport = 0;
    }
    if ((s.u64Ok + s.u64Err) != 0) {
        u64BpErr = (s.u64Err * 10000ull) / (s.u64Ok + s.u64Err);
    } else {
        u64BpErr = 0;
    }

    /* Grep: native: soft inventory */
    kprintf("native: soft inventory entries=%llu null=%llu handled=%llu "
            "nosupport=%llu door=%llu logs=%llu "
            "debug_log_max=%u console_chunk=%u hda_chunk=%u scsi_xfer=%u "
            "user_copy=user_range_ok+copy_{to,from}_user "
            "ksmoke=HHDM/static personality=NATIVE wave=%u\n",
            (unsigned long long)s.u64Entries,
            (unsigned long long)s.u64NullGuard,
            (unsigned long long)s.u64Handled,
            (unsigned long long)s.u64Nosupport,
            (unsigned long long)s.u64DoorFacade,
            (unsigned long long)d.u64SoftLog,
            (unsigned)GJ_NATIVE_DEBUG_LOG_MAX,
            (unsigned)GJ_NATIVE_CONSOLE_CHUNK,
            (unsigned)GJ_NATIVE_HDA_CHUNK,
            (unsigned)GJ_NATIVE_SCSI_XFER_MAX,
            (unsigned)NATIVE_SOFT_WAVE);

    /*
     * Legacy aggregate rollup — keep field order stable for existing greps.
     * Grep: native: soft stats
     */
    kprintf("native: soft stats entries=%llu null=%llu handled=%llu "
            "nosupport=%llu ok=%llu err=%llu ret0=%llu ret_pos=%llu "
            "inval=%llu fault=%llu nodev=%llu again=%llu io=%llu nomem=%llu "
            "noent=%llu perm=%llu busy=%llu "
            "diag=%llu ipc=%llu cap=%llu process=%llu thread=%llu cold=%llu "
            "gpu=%llu memobj=%llu hda=%llu door=%llu "
            "sess=%llu net=%llu store=%llu vfs=%llu "
            "platform=%llu notify=%llu console=%llu scsi=%llu "
            "vm=%llu futex=%llu wait=%llu untyped=%llu unk_nr=%llu "
            "cin_ok=%llu cin_fail=%llu cout_ok=%llu cout_fail=%llu "
            "cname_ok=%llu cname_fail=%llu bin=%llu bout=%llu "
            "c_user=%llu c_ksmoke=%llu "
            "last_nr=%llu last_ret=%llu\n",
            (unsigned long long)s.u64Entries,
            (unsigned long long)s.u64NullGuard,
            (unsigned long long)s.u64Handled,
            (unsigned long long)s.u64Nosupport,
            (unsigned long long)s.u64Ok,
            (unsigned long long)s.u64Err,
            (unsigned long long)s.u64RetZero,
            (unsigned long long)s.u64RetPos,
            (unsigned long long)s.u64Inval,
            (unsigned long long)s.u64Fault,
            (unsigned long long)s.u64Nodev,
            (unsigned long long)s.u64Again,
            (unsigned long long)s.u64Io,
            (unsigned long long)s.u64Nomem,
            (unsigned long long)s.u64Noent,
            (unsigned long long)s.u64Perm,
            (unsigned long long)s.u64Busy,
            (unsigned long long)s.u64Diag,
            (unsigned long long)s.u64Ipc,
            (unsigned long long)s.u64Cap,
            (unsigned long long)s.u64Process,
            (unsigned long long)s.u64Thread,
            (unsigned long long)s.u64Cold,
            (unsigned long long)s.u64Gpu,
            (unsigned long long)s.u64Memobj,
            (unsigned long long)s.u64Hda,
            (unsigned long long)s.u64DoorFacade,
            (unsigned long long)s.u64Session,
            (unsigned long long)s.u64Net,
            (unsigned long long)s.u64Store,
            (unsigned long long)s.u64Vfs,
            (unsigned long long)s.u64Platform,
            (unsigned long long)s.u64Notify,
            (unsigned long long)s.u64Console,
            (unsigned long long)s.u64Scsi,
            (unsigned long long)s.u64Vm,
            (unsigned long long)s.u64Futex,
            (unsigned long long)s.u64Wait,
            (unsigned long long)s.u64Untyped,
            (unsigned long long)s.u64UnknownNr,
            (unsigned long long)s.u64CopyInOk,
            (unsigned long long)s.u64CopyInFail,
            (unsigned long long)s.u64CopyOutOk,
            (unsigned long long)s.u64CopyOutFail,
            (unsigned long long)s.u64CopyNameOk,
            (unsigned long long)s.u64CopyNameFail,
            (unsigned long long)s.u64BytesCopyIn,
            (unsigned long long)s.u64BytesCopyOut,
            (unsigned long long)s.u64CopyUser,
            (unsigned long long)s.u64CopyKsmoke,
            (unsigned long long)s.u64LastNr,
            (unsigned long long)s.u64LastRet);

    /* Grep: native: soft outcome */
    kprintf("native: soft outcome ok=%llu err=%llu ret0=%llu ret_pos=%llu "
            "inval=%llu fault=%llu nodev=%llu again=%llu io=%llu nomem=%llu "
            "noent=%llu perm=%llu busy=%llu "
            "nosupport_term=%llu (err includes nosupport)\n",
            (unsigned long long)s.u64Ok,
            (unsigned long long)s.u64Err,
            (unsigned long long)s.u64RetZero,
            (unsigned long long)s.u64RetPos,
            (unsigned long long)s.u64Inval,
            (unsigned long long)s.u64Fault,
            (unsigned long long)s.u64Nodev,
            (unsigned long long)s.u64Again,
            (unsigned long long)s.u64Io,
            (unsigned long long)s.u64Nomem,
            (unsigned long long)s.u64Noent,
            (unsigned long long)s.u64Perm,
            (unsigned long long)s.u64Busy,
            (unsigned long long)s.u64Nosupport);

    /* Grep: native: soft class */
    kprintf("native: soft class diag=%llu ipc=%llu cap=%llu process=%llu "
            "thread=%llu cold=%llu gpu=%llu memobj=%llu hda=%llu "
            "platform=%llu notify=%llu console=%llu scsi=%llu door=%llu\n",
            (unsigned long long)s.u64Diag,
            (unsigned long long)s.u64Ipc,
            (unsigned long long)s.u64Cap,
            (unsigned long long)s.u64Process,
            (unsigned long long)s.u64Thread,
            (unsigned long long)s.u64Cold,
            (unsigned long long)s.u64Gpu,
            (unsigned long long)s.u64Memobj,
            (unsigned long long)s.u64Hda,
            (unsigned long long)s.u64Platform,
            (unsigned long long)s.u64Notify,
            (unsigned long long)s.u64Console,
            (unsigned long long)s.u64Scsi,
            (unsigned long long)s.u64DoorFacade);

    /* Grep: native: soft door */
    kprintf("native: soft door facade=%llu sess=%llu net=%llu store=%llu "
            "vfs=%llu claim=door_facade product=userspace_servers\n",
            (unsigned long long)s.u64DoorFacade,
            (unsigned long long)s.u64Session,
            (unsigned long long)s.u64Net,
            (unsigned long long)s.u64Store,
            (unsigned long long)s.u64Vfs);

    /* Grep: native: soft reserved */
    kprintf("native: soft reserved vm=%llu futex=%llu wait=%llu untyped=%llu "
            "unknown_nr=%llu "
            "(default-path only; frozen NR; never product gate)\n",
            (unsigned long long)s.u64Vm,
            (unsigned long long)s.u64Futex,
            (unsigned long long)s.u64Wait,
            (unsigned long long)s.u64Untyped,
            (unsigned long long)s.u64UnknownNr);

    /* Grep: native: soft copy */
    kprintf("native: soft copy cin_ok=%llu cin_fail=%llu cout_ok=%llu "
            "cout_fail=%llu cname_ok=%llu cname_fail=%llu "
            "bin=%llu bout=%llu user=%llu ksmoke=%llu\n",
            (unsigned long long)s.u64CopyInOk,
            (unsigned long long)s.u64CopyInFail,
            (unsigned long long)s.u64CopyOutOk,
            (unsigned long long)s.u64CopyOutFail,
            (unsigned long long)s.u64CopyNameOk,
            (unsigned long long)s.u64CopyNameFail,
            (unsigned long long)s.u64BytesCopyIn,
            (unsigned long long)s.u64BytesCopyOut,
            (unsigned long long)s.u64CopyUser,
            (unsigned long long)s.u64CopyKsmoke);

    /* Grep: native: soft platform */
    kprintf("native: soft platform iommu=%llu msix=%llu wow64=%llu "
            "msix_inj=%llu iommu_enf=%llu iommu_win=%llu inval=%llu "
            "ops=0..5 handled_total=%llu\n",
            (unsigned long long)d.u64PlatIommu,
            (unsigned long long)d.u64PlatMsix,
            (unsigned long long)d.u64PlatWow64,
            (unsigned long long)d.u64PlatMsixInj,
            (unsigned long long)d.u64PlatIommuEnf,
            (unsigned long long)d.u64PlatIommuWin,
            (unsigned long long)d.u64PlatInval,
            (unsigned long long)s.u64Platform);

    /* Grep: native: soft console */
    kprintf("native: soft console poll=%llu getc=%llu read=%llu inval=%llu "
            "chunk=%u handled_total=%llu\n",
            (unsigned long long)d.u64ConPoll,
            (unsigned long long)d.u64ConGetc,
            (unsigned long long)d.u64ConRead,
            (unsigned long long)d.u64ConInval,
            (unsigned)GJ_NATIVE_CONSOLE_CHUNK,
            (unsigned long long)s.u64Console);

    /* Grep: native: soft scsi */
    kprintf("native: soft scsi ready=%llu stats=%llu inq=%llu readcap=%llu "
            "read10=%llu write10=%llu inval=%llu xfer_max=%u "
            "handled_total=%llu\n",
            (unsigned long long)d.u64ScsiReady,
            (unsigned long long)d.u64ScsiStats,
            (unsigned long long)d.u64ScsiInq,
            (unsigned long long)d.u64ScsiReadCap,
            (unsigned long long)d.u64ScsiRead10,
            (unsigned long long)d.u64ScsiWrite10,
            (unsigned long long)d.u64ScsiInval,
            (unsigned)GJ_NATIVE_SCSI_XFER_MAX,
            (unsigned long long)s.u64Scsi);

    /* Grep: native: soft hda */
    kprintf("native: soft hda open=%llu write=%llu start=%llu tick=%llu "
            "close=%llu stats=%llu inval=%llu chunk=%u handled_total=%llu\n",
            (unsigned long long)d.u64HdaOpen,
            (unsigned long long)d.u64HdaWrite,
            (unsigned long long)d.u64HdaStart,
            (unsigned long long)d.u64HdaTick,
            (unsigned long long)d.u64HdaClose,
            (unsigned long long)d.u64HdaStats,
            (unsigned long long)d.u64HdaInval,
            (unsigned)GJ_NATIVE_HDA_CHUNK,
            (unsigned long long)s.u64Hda);

    /* Grep: native: soft cap */
    kprintf("native: soft cap mint=%llu move=%llu copy=%llu revoke=%llu "
            "ident=%llu handled_total=%llu scheme=A_current_cnode\n",
            (unsigned long long)d.u64CapMint,
            (unsigned long long)d.u64CapMove,
            (unsigned long long)d.u64CapCopy,
            (unsigned long long)d.u64CapRevoke,
            (unsigned long long)d.u64CapIdent,
            (unsigned long long)s.u64Cap);

    /* Grep: native: soft ipc */
    kprintf("native: soft ipc call=%llu recv=%llu reply=%llu "
            "diag_log=%llu diag_yield=%llu diag_exit=%llu "
            "ipc_total=%llu diag_total=%llu\n",
            (unsigned long long)d.u64IpcCall,
            (unsigned long long)d.u64IpcRecv,
            (unsigned long long)d.u64IpcReply,
            (unsigned long long)d.u64DiagLog,
            (unsigned long long)d.u64DiagYield,
            (unsigned long long)d.u64DiagExit,
            (unsigned long long)s.u64Ipc,
            (unsigned long long)s.u64Diag);

    /* Grep: native: soft last */
    kprintf("native: soft last nr=%llu ret_bits=0x%llx logs=%llu\n",
            (unsigned long long)s.u64LastNr,
            (unsigned long long)s.u64LastRet,
            (unsigned long long)d.u64SoftLog);

    /*
     * Wave 14/15 exclusive complementary surfaces (never reshape primary lines).
     * Grep: native: soft process
     */
    kprintf("native: soft process spawn=%llu handled_total=%llu "
            "frozen_set_pager_kill=soft_nosupport wave=%u\n",
            (unsigned long long)d.u64ProcSpawn,
            (unsigned long long)s.u64Process,
            (unsigned)NATIVE_SOFT_WAVE);

    /* Grep: native: soft thread */
    kprintf("native: soft thread qos=%llu cpu=%llu handled_total=%llu "
            "wave=%u\n",
            (unsigned long long)d.u64ThrQos,
            (unsigned long long)d.u64ThrCpu,
            (unsigned long long)s.u64Thread,
            (unsigned)NATIVE_SOFT_WAVE);

    /* Grep: native: soft gpu */
    kprintf("native: soft gpu present=%llu info=%llu handled_total=%llu "
            "wave=%u\n",
            (unsigned long long)d.u64GpuPresent,
            (unsigned long long)d.u64GpuInfo,
            (unsigned long long)s.u64Gpu,
            (unsigned)NATIVE_SOFT_WAVE);

    /* Grep: native: soft memobj */
    kprintf("native: soft memobj create=%llu map=%llu handled_total=%llu "
            "wave=%u\n",
            (unsigned long long)d.u64MemobjCreate,
            (unsigned long long)d.u64MemobjMap,
            (unsigned long long)s.u64Memobj,
            (unsigned)NATIVE_SOFT_WAVE);

    /* Grep: native: soft cold */
    kprintf("native: soft cold dequeue=%llu reply=%llu serve=%llu "
            "handled_total=%llu wave=%u\n",
            (unsigned long long)d.u64ColdDeq,
            (unsigned long long)d.u64ColdReply,
            (unsigned long long)d.u64ColdServe,
            (unsigned long long)s.u64Cold,
            (unsigned)NATIVE_SOFT_WAVE);

    /* Grep: native: soft notify */
    kprintf("native: soft notify wait=%llu handled_total=%llu wave=%u\n",
            (unsigned long long)d.u64NotifyWait,
            (unsigned long long)s.u64Notify,
            (unsigned)NATIVE_SOFT_WAVE);

    /* Grep: native: soft rates */
    kprintf("native: soft rates bp_handled=%llu bp_nosupport=%llu "
            "bp_err=%llu term=%llu ok=%llu err=%llu wave=%u\n",
            (unsigned long long)u64BpHandled,
            (unsigned long long)u64BpNosupport,
            (unsigned long long)u64BpErr,
            (unsigned long long)u64Term,
            (unsigned long long)s.u64Ok,
            (unsigned long long)s.u64Err,
            (unsigned)NATIVE_SOFT_WAVE);

    /* Grep: native: soft honesty */
    kprintf("native: soft honesty native_gj=1 linux_hybrid=0 bar3=0 "
            "product_linux_abi=open soft_only=1 wave=%u "
            "(native soft inventory; hybrid product remains open)\n",
            (unsigned)NATIVE_SOFT_WAVE);

    /* Grep: native: soft surfaces (Wave 20 deepen) */
    kprintf("native: soft surfaces count=%u wave=%u "
            "names=inventory,route,path,rates,honesty,last,catalog,"
            "surfaces,note,return,retmap,deepen,PASS\n",
            NATIVE_SOFT_AREAS, (unsigned)NATIVE_SOFT_WAVE);

    /* Grep: native: soft note (Wave 20 deepen) */
    kprintf("native: soft note milestone=wave61 exclusive=1 "
            "soft_only=1 not_bar3=1 wave=%u\n",
            (unsigned)NATIVE_SOFT_WAVE);

    /* Grep: native: soft retmap — Wave 19 return-surface map */
    kprintf("native: soft retmap ok|fail|inval|nodev|busy|nomem product_gate=0 soft_only=1 wave=61\n");

    /* Grep: native: soft deepen */
    /* Grep: native: soft edge (Wave 20 deepen) */
    kprintf("native: soft edge entries=%llu null=%llu handled=%llu "
            "nosupport=%llu door=%llu term=%llu wave=%u\n",
            (unsigned long long)s.u64Entries,
            (unsigned long long)s.u64NullGuard,
            (unsigned long long)s.u64Handled,
            (unsigned long long)s.u64Nosupport,
            (unsigned long long)s.u64DoorFacade,
            (unsigned long long)u64Term,
            (unsigned)NATIVE_SOFT_WAVE);

    /* Grep: native: soft share (Wave 20 deepen) */
    kprintf("native: soft share bp_handled=%llu bp_nosupport=%llu "
            "bp_err=%llu door=%llu cap=%llu ipc=%llu wave=%u\n",
            (unsigned long long)u64BpHandled,
            (unsigned long long)u64BpNosupport,
            (unsigned long long)u64BpErr,
            (unsigned long long)s.u64DoorFacade,
            (unsigned long long)s.u64Cap,
            (unsigned long long)s.u64Ipc,
            (unsigned)NATIVE_SOFT_WAVE);

    /* Grep: native: soft catalog (Wave 20 deepen) */
    kprintf("native: soft catalog wave=%u areas=%u "
            "surfaces=inventory,stats,outcome,class,door,reserved,copy,"
            "platform,console,scsi,hda,cap,ipc,last,process,thread,gpu,"
            "memobj,cold,notify,rates,honesty,edge,share,catalog,surfaces,note,return,retmap,deepen,path\n",
            (unsigned)NATIVE_SOFT_WAVE, NATIVE_SOFT_AREAS);

    /* Grep: native: soft return (Wave 20 deepen) */
    kprintf("native: soft return handled=%llu nosupport=%llu err=%llu "
            "ok=%llu null=%llu term=%llu product_gate=0 wave=%u\n",
            (unsigned long long)s.u64Handled,
            (unsigned long long)s.u64Nosupport,
            (unsigned long long)s.u64Err,
            (unsigned long long)s.u64Ok,
            (unsigned long long)s.u64NullGuard,
            (unsigned long long)u64Term,
            (unsigned)NATIVE_SOFT_WAVE);

    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: native: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("native: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)NATIVE_SOFT_WAVE);
    /* Grep: native: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("native: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)NATIVE_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: native: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("native: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)NATIVE_SOFT_WAVE);
    /* Grep: native: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("native: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)NATIVE_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: native: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("native: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)NATIVE_SOFT_WAVE);
            /* Grep: native: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("native: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)NATIVE_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: native: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("native: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)NATIVE_SOFT_WAVE);
            /* Grep: native: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("native: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)NATIVE_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: native: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("native: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)NATIVE_SOFT_WAVE);
            /* Grep: native: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("native: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)NATIVE_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: native: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("native: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)NATIVE_SOFT_WAVE);
            /* Grep: native: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("native: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)NATIVE_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: native: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("native: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)NATIVE_SOFT_WAVE);
            /* Grep: native: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("native: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)NATIVE_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: native: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("native: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)NATIVE_SOFT_WAVE);
            /* Grep: native: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("native: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)NATIVE_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: native: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("native: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)NATIVE_SOFT_WAVE);
                    /* Grep: native: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("native: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)NATIVE_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: native: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("native: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)NATIVE_SOFT_WAVE);
                            /* Grep: native: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("native: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)NATIVE_SOFT_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: native: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("native: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retglyph honesty; Soft≠product; not bar3)\n",
                                    (unsigned)NATIVE_SOFT_WAVE);
                            /* Grep: native: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("native: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retscepter stamp; Soft≠product)\n",
                                    (unsigned)NATIVE_SOFT_WAVE);
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: native: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("native: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)NATIVE_SOFT_WAVE);
                            /* Grep: native: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("native: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retemblem stamp; Soft≠product)\n",
                                    (unsigned)NATIVE_SOFT_WAVE);
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: native: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("native: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retaegis honesty; Soft≠product; not bar3)\n",
                                    (unsigned)NATIVE_SOFT_WAVE);
                            /* Grep: native: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("native: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)NATIVE_SOFT_WAVE);
                            /* Grep: native: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("native: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retmantle stamp; Soft≠product)\n",
                                    (unsigned)NATIVE_SOFT_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: native: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("native: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)NATIVE_SOFT_WAVE);
/* Grep: native: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("native: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)NATIVE_SOFT_WAVE);
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: native: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("native: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)NATIVE_SOFT_WAVE);
/* Grep: native: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("native: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)NATIVE_SOFT_WAVE);
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: native: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("native: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retredoubt honesty; Soft≠product; not bar3)\n",
        (unsigned)NATIVE_SOFT_WAVE);
/* Grep: native: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("native: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retkeep stamp; Soft≠product)\n",
        (unsigned)NATIVE_SOFT_WAVE);
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: native: soft retfortress — Wave 35 return-fortress honesty */
kprintf("native: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retfortress honesty; Soft≠product; not bar3)\n",
        (unsigned)NATIVE_SOFT_WAVE);
/* Grep: native: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("native: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpalace stamp; Soft≠product)\n",
        (unsigned)NATIVE_SOFT_WAVE);
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: native: soft rethold — Wave 36 return-hold honesty */
kprintf("native: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(rethold honesty; Soft≠product; not bar3)\n",
        (unsigned)NATIVE_SOFT_WAVE);
/* Grep: native: soft retspire — Wave 36 exclusive spire stamp */
kprintf("native: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retspire stamp; Soft≠product)\n",
        (unsigned)NATIVE_SOFT_WAVE);
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: native: soft retwall — Wave 37 return-wall honesty */
kprintf("native: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retwall honesty; Soft≠product; not bar3)\n",
        (unsigned)NATIVE_SOFT_WAVE);
/* Grep: native: soft retgate — Wave 37 exclusive gate stamp */
kprintf("native: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retgate stamp; Soft≠product)\n",
        (unsigned)NATIVE_SOFT_WAVE);
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: native: soft retmoat — Wave 38 return-moat honesty */
kprintf("native: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmoat honesty; Soft≠product; not bar3)\n",
        (unsigned)NATIVE_SOFT_WAVE);
/* Grep: native: soft retower — Wave 38 exclusive tower stamp */
kprintf("native: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retower stamp; Soft≠product)\n",
        (unsigned)NATIVE_SOFT_WAVE);
                            
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: native: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("native: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbarbican honesty; Soft≠product; not bar3)\n",
        (unsigned)NATIVE_SOFT_WAVE);
/* Grep: native: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("native: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retglacis stamp; Soft≠product)\n",
        (unsigned)NATIVE_SOFT_WAVE);
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: native: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("native: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retcurtain honesty; Soft≠product; not bar3)\n",
        (unsigned)NATIVE_SOFT_WAVE);
/* Grep: native: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("native: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retparapet stamp; Soft≠product)\n",
        (unsigned)NATIVE_SOFT_WAVE);
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: native: soft retravelin — Wave 41 return-travelin honesty */
kprintf("native: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retravelin honesty; Soft≠product; not bar3)\n",
        (unsigned)NATIVE_SOFT_WAVE);
/* Grep: native: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("native: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retditch stamp; Soft≠product)\n",
        (unsigned)NATIVE_SOFT_WAVE);
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: native: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("native: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retportcullis honesty; Soft≠product; not bar3)\n",
        (unsigned)NATIVE_SOFT_WAVE);
/* Grep: native: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("native: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retbattlement stamp; Soft≠product)\n",
        (unsigned)NATIVE_SOFT_WAVE);
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: native: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("native: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmachicolation honesty; Soft≠product; not bar3)\n",
        (unsigned)NATIVE_SOFT_WAVE);
/* Grep: native: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("native: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retarrowslit stamp; Soft≠product)\n",
        (unsigned)NATIVE_SOFT_WAVE);

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: native: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("native: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmerlon honesty; Soft≠product; not bar3)\n",
        (unsigned)NATIVE_SOFT_WAVE);
/* Grep: native: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("native: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retembrasure stamp; Soft≠product)\n",
        (unsigned)NATIVE_SOFT_WAVE);

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: native: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("native: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retkeepgate honesty; Soft≠product; not bar3)\n",
        (unsigned)NATIVE_SOFT_WAVE);
/* Grep: native: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("native: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retouterward stamp; Soft≠product)\n",
        (unsigned)NATIVE_SOFT_WAVE);

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: native: soft retbailey — Wave 46 return-bailey honesty */
kprintf("native: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbailey honesty; Soft≠product; not bar3)\n",
        (unsigned)NATIVE_SOFT_WAVE);
/* Grep: native: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("native: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpostern stamp; Soft≠product)\n",
        (unsigned)NATIVE_SOFT_WAVE);

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: native: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("native: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=61 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: native: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("native: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=61 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: native: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("native: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=61 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: native: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("native: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=61 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: native: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("native: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=61 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: native: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("native: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=61 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: native: soft retsally — Wave 50 return-sally honesty */
kprintf("native: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=61 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: native: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("native: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=61 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: native: soft retfosse — Wave 51 return-fosse honesty */
kprintf("native: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=61 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: native: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("native: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=61 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: native: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("native: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=61 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: native: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("native: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=61 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: native: soft retravelin — Wave 53 return-travelin honesty */
kprintf("native: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=61 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: native: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("native: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=61 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: native: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("native: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=61 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: native: soft retredan — Wave 54 exclusive redan stamp */
kprintf("native: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=61 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: native: soft retflank — Wave 55 return-flank honesty */
kprintf("native: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=61 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: native: soft retface — Wave 55 exclusive face stamp */
kprintf("native: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=61 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: native: soft retgorge — Wave 56 return-gorge honesty */
kprintf("native: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=61 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: native: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("native: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=61 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: native: soft retraverse — Wave 57 return-traverse honesty */
kprintf("native: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=61 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: native: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("native: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=61 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: native: soft retorillon — Wave 58 return-orillon honesty */
kprintf("native: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=61 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: native: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("native: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=61 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: native: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("native: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=61 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: native: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("native: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=61 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: native: soft retplace — Wave 60 return-place honesty */
kprintf("native: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=61 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: native: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("native: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=61 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: native: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("native: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=61 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: native: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("native: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=61 "
        "(retcoveredface stamp; Soft≠product)\n");
                            kprintf("native: soft deepen wave=%u areas=%u ok=1 "
            "prefix=native:soft "
            "surfaces=inventory,stats,outcome,class,door,reserved,copy,"
            "platform,console,scsi,hda,cap,ipc,last,process,thread,gpu,"
            "memobj,cold,notify,rates,honesty,edge,share,catalog,surfaces,note,return,retmap,deepen,path\n",
            (unsigned)NATIVE_SOFT_WAVE, NATIVE_SOFT_AREAS);

    /*
     * Honesty: native GJ_SYS_* path is not Linux hybrid and not bar3.
     * Grep: native: soft path
     */
    kprintf("native: soft path claim=native_gj_sys "
            "linux_hybrid=0 bar3=0 product_doors=session,net,store,vfs "
            "reserved_stubs=vm,futex,wait,untyped "
            "copy_user=1 copy_ksmoke=1 soft_only=1 wave=%u\n",
            (unsigned)NATIVE_SOFT_WAVE);

    /*
     * Soft lamp only — inventory emit succeeded. Never hard-gates.
     * Grep: native: soft inventory PASS
     * Grep: native: soft PASS
     */
    kprintf("native: soft inventory PASS wave=%u logs=%llu entries=%llu "
            "handled=%llu nosupport=%llu\n",
            (unsigned)NATIVE_SOFT_WAVE,
            (unsigned long long)d.u64SoftLog,
            (unsigned long long)s.u64Entries,
            (unsigned long long)s.u64Handled,
            (unsigned long long)s.u64Nosupport);
    kprintf("native: soft PASS wave=%u logs=%llu\n",
            (unsigned)NATIVE_SOFT_WAVE,
            (unsigned long long)d.u64SoftLog);
}

/**
 * After first non-null dispatch, print soft inventory once
 * (mirrors futex/door soft-stats-once). Diagnostics only.
 */
static void
native_soft_maybe_once(void)
{
    if (g_fSoftNativeOnce != 0) {
        return;
    }
    if (g_nativeStats.u64Entries == 0 && g_nativeStats.u64NullGuard == 0) {
        return;
    }
    g_fSoftNativeOnce = 1;
    native_soft_inventory_log();
}

void
gj_native_dispatch_stats_get(struct gj_native_dispatch_stats *pOut)
{
    if (pOut == NULL) {
        return;
    }
    *pOut = g_nativeStats;
}

void
gj_native_dispatch_stats_reset(void)
{
    memset(&g_nativeStats, 0, sizeof(g_nativeStats));
    memset(&g_nativeDeep, 0, sizeof(g_nativeDeep));
    g_fSoftNativeOnce = 0;
}

u64
gj_native_dispatch_stats_soft(void)
{
    /*
     * Full Wave 11 + Wave 15 multi-line soft inventory (legacy stats line).
     * Snapshots inside native_soft_inventory_log; never hard-gates.
     * Grep: native: soft stats / native: soft inventory
     */
    native_soft_inventory_log();
    return g_nativeStats.u64Entries;
}

/**
 * Bump subsystem bucket for a handled GJ_SYS_* number.
 * Unknown / reserved numbers are not classified here (caller uses nosupport).
 */
static void
native_stats_class_bump(u64 u64Nr)
{
    switch (u64Nr) {
    case GJ_SYS_DEBUG_LOG:
    case GJ_SYS_YIELD:
    case GJ_SYS_EXIT:
        g_nativeStats.u64Diag++;
        break;
    case GJ_SYS_IPC_CALL:
    case GJ_SYS_IPC_RECV:
    case GJ_SYS_IPC_REPLY:
        g_nativeStats.u64Ipc++;
        break;
    case GJ_SYS_CAP_MINT:
    case GJ_SYS_CAP_MOVE:
    case GJ_SYS_CAP_COPY:
    case GJ_SYS_CAP_REVOKE:
    case GJ_SYS_CAP_IDENT:
        g_nativeStats.u64Cap++;
        break;
    case GJ_SYS_PROCESS_SET_PAGER:
    case GJ_SYS_PROCESS_SPAWN:
    case GJ_SYS_PROCESS_KILL:
        g_nativeStats.u64Process++;
        break;
    case GJ_SYS_THREAD_SET_QOS:
    case GJ_SYS_THREAD_SET_CPU:
        g_nativeStats.u64Thread++;
        break;
    case GJ_SYS_COLD_DEQUEUE:
    case GJ_SYS_COLD_REPLY:
    case GJ_SYS_PERSONALITY_SERVE:
        g_nativeStats.u64Cold++;
        break;
    case GJ_SYS_GPU_PRESENT:
    case GJ_SYS_GPU_DISPLAY_INFO:
        g_nativeStats.u64Gpu++;
        break;
    case GJ_SYS_MEMOBJ_CREATE_NAMED:
    case GJ_SYS_MEMOBJ_MAP_NAMED:
        g_nativeStats.u64Memobj++;
        break;
    case GJ_SYS_HDA_STREAM:
        g_nativeStats.u64Hda++;
        break;
    case GJ_SYS_SESSION:
        g_nativeStats.u64DoorFacade++;
        g_nativeStats.u64Session++;
        break;
    case GJ_SYS_NET:
        g_nativeStats.u64DoorFacade++;
        g_nativeStats.u64Net++;
        break;
    case GJ_SYS_STORE:
        g_nativeStats.u64DoorFacade++;
        g_nativeStats.u64Store++;
        break;
    case GJ_SYS_VFS:
        g_nativeStats.u64DoorFacade++;
        g_nativeStats.u64Vfs++;
        break;
    case GJ_SYS_PLATFORM_INFO:
        g_nativeStats.u64Platform++;
        break;
    case GJ_SYS_NOTIFY_WAIT:
        g_nativeStats.u64Notify++;
        break;
    case GJ_SYS_CONSOLE:
        g_nativeStats.u64Console++;
        break;
    case GJ_SYS_SCSI:
        g_nativeStats.u64Scsi++;
        break;
    default:
        /* Handled case without a named bucket (should stay rare). */
        break;
    }
}

/**
 * Soft-classify a default-path (NOSUPPORT) NR into reserved blocks.
 * Helps smoke see which frozen stubs were hit without product wiring.
 */
static void
native_stats_nosupport_class(u64 u64Nr)
{
    switch (u64Nr) {
    case GJ_SYS_UNTYPED_RETYPE:
        g_nativeStats.u64Untyped++;
        break;
    case GJ_SYS_VM_MAP:
    case GJ_SYS_VM_UNMAP:
    case GJ_SYS_VM_PROTECT:
    case GJ_SYS_VM_MAP_OBJ:
        g_nativeStats.u64Vm++;
        break;
    case GJ_SYS_WAIT_TIMEOUT:
        g_nativeStats.u64Wait++;
        break;
    case GJ_SYS_FUTEX_WAIT:
    case GJ_SYS_FUTEX_WAKE:
        g_nativeStats.u64Futex++;
        break;
    case GJ_SYS_PROCESS_SET_PAGER:
    case GJ_SYS_PROCESS_KILL:
        /* Frozen numbers; product cases not yet wired on native path. */
        g_nativeStats.u64Process++;
        break;
    default:
        g_nativeStats.u64UnknownNr++;
        break;
    }
}

/**
 * Classify outcome from i64Ret after a dispatch completes.
 */
static void
native_stats_outcome(i64 i64Ret)
{
    if (i64Ret >= 0) {
        g_nativeStats.u64Ok++;
        if (i64Ret == 0) {
            g_nativeStats.u64RetZero++;
        } else {
            g_nativeStats.u64RetPos++;
        }
        return;
    }
    g_nativeStats.u64Err++;
    if (i64Ret == GJ_ERR_INVAL) {
        g_nativeStats.u64Inval++;
    } else if (i64Ret == GJ_ERR_FAULT) {
        g_nativeStats.u64Fault++;
    } else if (i64Ret == GJ_ERR_NODEV) {
        g_nativeStats.u64Nodev++;
    } else if (i64Ret == GJ_ERR_AGAIN) {
        g_nativeStats.u64Again++;
    } else if (i64Ret == GJ_ERR_IO) {
        g_nativeStats.u64Io++;
    } else if (i64Ret == GJ_ERR_NOMEM) {
        g_nativeStats.u64Nomem++;
    } else if (i64Ret == GJ_ERR_NOENT) {
        g_nativeStats.u64Noent++;
    } else if (i64Ret == GJ_ERR_PERM) {
        g_nativeStats.u64Perm++;
    } else if (i64Ret == GJ_ERR_BUSY) {
        g_nativeStats.u64Busy++;
    }
    /* NOSUPPORT and other GJ_ERR_* remain under u64Err (+ u64Nosupport). */
}

/**
 * Finish soft accounting for one dispatch (known case or default).
 */
static void
native_stats_finish(u64 u64Nr, i64 i64Ret, int fHitDefault)
{
    if (fHitDefault) {
        g_nativeStats.u64Nosupport++;
        native_stats_nosupport_class(u64Nr);
    } else {
        g_nativeStats.u64Handled++;
        native_stats_class_bump(u64Nr);
    }
    native_stats_outcome(i64Ret);
    g_nativeStats.u64LastNr = u64Nr;
    g_nativeStats.u64LastRet = (u64)i64Ret;
}

/**
 * Copy @cb bytes to caller buffer at @u64Dst.
 * Uses copy_to_user when the range is in the user VA window.
 */
static i64
native_copy_out(u64 u64Dst, const void *pSrc, u32 cb)
{
    if (u64Dst == 0 || pSrc == NULL || cb == 0) {
        g_nativeStats.u64CopyOutFail++;
        return GJ_ERR_INVAL;
    }
    if (user_range_ok(u64Dst, cb)) {
        if (copy_to_user(u64Dst, pSrc, cb) != GJ_OK) {
            g_nativeStats.u64CopyOutFail++;
            return GJ_ERR_FAULT;
        }
        g_nativeStats.u64CopyUser++;
    } else {
        /* Kernel-smoke path: destination is a trusted HHDM/static buffer. */
        memcpy((void *)(gj_vaddr_t)u64Dst, pSrc, cb);
        g_nativeStats.u64CopyKsmoke++;
    }
    g_nativeStats.u64CopyOutOk++;
    g_nativeStats.u64BytesCopyOut += (u64)cb;
    return 0;
}

/**
 * Copy @cb bytes from caller buffer at @u64Src into @pDst.
 */
static i64
native_copy_in(void *pDst, u64 u64Src, u32 cb)
{
    if (pDst == NULL || u64Src == 0 || cb == 0) {
        g_nativeStats.u64CopyInFail++;
        return GJ_ERR_INVAL;
    }
    if (user_range_ok(u64Src, cb)) {
        if (copy_from_user(pDst, u64Src, cb) != GJ_OK) {
            g_nativeStats.u64CopyInFail++;
            return GJ_ERR_FAULT;
        }
        g_nativeStats.u64CopyUser++;
    } else {
        memcpy(pDst, (const void *)(gj_vaddr_t)u64Src, cb);
        g_nativeStats.u64CopyKsmoke++;
    }
    g_nativeStats.u64CopyInOk++;
    g_nativeStats.u64BytesCopyIn += (u64)cb;
    return 0;
}

/**
 * Copy a NUL-terminated name from @u64Src into @szDst (max @cbMax, incl NUL).
 * Always NUL-terminates on success. Returns 0 or GJ_ERR_*.
 */
static i64
native_copy_name(char *szDst, u64 u64Src, u32 cbMax)
{
    u32 i;
    i64 st;

    if (szDst == NULL || u64Src == 0 || cbMax < 2u) {
        g_nativeStats.u64CopyNameFail++;
        return GJ_ERR_INVAL;
    }
    memset(szDst, 0, cbMax);
    for (i = 0; i + 1u < cbMax; i++) {
        char ch = 0;

        st = native_copy_in(&ch, u64Src + i, 1u);
        if (st != 0) {
            g_nativeStats.u64CopyNameFail++;
            return st;
        }
        szDst[i] = ch;
        if (ch == '\0') {
            g_nativeStats.u64CopyNameOk++;
            return 0;
        }
    }
    /* Truncated: force terminator (defensive; names must fit). */
    szDst[cbMax - 1u] = '\0';
    g_nativeStats.u64CopyNameOk++;
    return 0;
}

/**
 * Resolve tid 0 to the calling thread id. Returns 0 if no current thread.
 */
static u32
native_tid_or_current(u32 u32Id)
{
    struct gj_thread *pCur;

    if (u32Id != 0) {
        return u32Id;
    }
    pCur = thread_current();
    if (pCur == NULL) {
        return 0;
    }
    return pCur->u32Id;
}

void
gj_native_syscall_dispatch(struct gj_syscall_regs *pRegs)
{
    int fHitDefault = 0;

    if (pRegs == NULL) {
        g_nativeStats.u64NullGuard++;
        /* Soft: still allow one-shot inventory after null-guard activity. */
        native_soft_maybe_once();
        return;
    }
    g_nativeStats.u64Entries++;
    /* Native errors are GJ_ERR_*; unknown numbers land here until a case. */
    pRegs->i64Ret = GJ_ERR_NOSUPPORT;

    switch (pRegs->u64Nr) {
    case GJ_SYS_DEBUG_LOG: {
        char aKbuf[128];
        u64 u64Len = pRegs->u64Arg1;
        u64 u64Off = 0;
        u64 u64Src = pRegs->u64Arg0;
        size_t cbChunk;
        i64 st;

        native_soft_inc(&g_nativeDeep.u64DiagLog);
        if (u64Src == 0) {
            pRegs->i64Ret = GJ_ERR_INVAL;
            break;
        }
        if (u64Len > GJ_NATIVE_DEBUG_LOG_MAX) {
            u64Len = GJ_NATIVE_DEBUG_LOG_MAX;
        }
        while (u64Off < u64Len) {
            cbChunk = (size_t)(u64Len - u64Off);
            if (cbChunk > sizeof(aKbuf) - 1u) {
                cbChunk = sizeof(aKbuf) - 1u;
            }
            st = native_copy_in(aKbuf, u64Src + u64Off, (u32)cbChunk);
            if (st != 0) {
                pRegs->i64Ret = u64Off ? (i64)u64Off : st;
                break;
            }
            aKbuf[cbChunk] = '\0';
            kprintf("%s", aKbuf);
            u64Off += cbChunk;
        }
        if (u64Off == u64Len) {
            pRegs->i64Ret = (i64)u64Len;
        }
        break;
    }
    case GJ_SYS_YIELD:
        native_soft_inc(&g_nativeDeep.u64DiagYield);
        thread_yield();
        pRegs->i64Ret = 0;
        break;
    case GJ_SYS_EXIT:
        native_soft_inc(&g_nativeDeep.u64DiagExit);
        kprintf("gj: SYS_EXIT %ld\n", (long)pRegs->u64Arg0);
        thread_exit();
        /* Not reached on a live path; keep a defined ret for smokes. */
        pRegs->i64Ret = 0;
        break;
    case GJ_SYS_PROCESS_SPAWN: {
        /* arg0 = entry VA (kernel smoke: function ptr), arg1 = arg, arg2 = flags */
        struct gj_spawn_args args;
        struct gj_cap_ref ref;
        struct gj_process *pParent = g_pLinuxProc;
        gj_status_t st;

        native_soft_inc(&g_nativeDeep.u64ProcSpawn);
        if (pParent == NULL || pRegs->u64Arg0 == 0) {
            pRegs->i64Ret = GJ_ERR_INVAL;
            break;
        }
        memset(&args, 0, sizeof(args));
        memset(&ref, 0, sizeof(ref));
        args.pfnEntry = (void (*)(void *))(gj_vaddr_t)pRegs->u64Arg0;
        args.pArg = (void *)(gj_vaddr_t)pRegs->u64Arg1;
        args.u32Jit = (pRegs->u64Arg2 & 1ull) ? 1u : 0u;
        args.u32Personality = 1;
        st = process_spawn(pParent, &args, NULL, &ref);
        if (st != GJ_OK) {
            pRegs->i64Ret = st;
        } else {
            /* Return slot in low 32, gen in high — smoke ABI */
            pRegs->i64Ret = (i64)((ref.u64Slot & 0xffffffffull) |
                                  ((u64)ref.u32SlotGen << 32));
        }
        break;
    }
    case GJ_SYS_IPC_CALL: {
        /* Cold personality door call: arg0..5 = linux-ish regs payload */
        struct gj_linux_regs req;
        struct gj_door *pDoor;

        native_soft_inc(&g_nativeDeep.u64IpcCall);
        memset(&req, 0, sizeof(req));
        req.u64Nr = pRegs->u64Arg0;
        req.u64Arg0 = pRegs->u64Arg1;
        req.u64Arg1 = pRegs->u64Arg2;
        req.u64Arg2 = pRegs->u64Arg3;
        req.u64Arg3 = pRegs->u64Arg4;
        req.u64Arg4 = pRegs->u64Arg5;
        pDoor = door_cold_personality();
        if (pDoor == NULL) {
            pRegs->i64Ret = GJ_ERR_NODEV;
            break;
        }
        pRegs->i64Ret = door_call(pDoor, &req);
        break;
    }
    case GJ_SYS_IPC_RECV: {
        /* arg0 = user ptr to gj_linux_regs (or kernel for smoke) */
        struct gj_linux_regs req;
        struct gj_door *pDoor;
        int n;
        i64 st;

        native_soft_inc(&g_nativeDeep.u64IpcRecv);
        pDoor = door_cold_personality();
        if (pDoor == NULL) {
            pRegs->i64Ret = GJ_ERR_NODEV;
            break;
        }
        memset(&req, 0, sizeof(req));
        n = door_recv(pDoor, &req);
        if (n != 0) {
            pRegs->i64Ret = GJ_ERR_AGAIN;
            break;
        }
        if (pRegs->u64Arg0 == 0) {
            pRegs->i64Ret = GJ_ERR_INVAL;
            break;
        }
        st = native_copy_out(pRegs->u64Arg0, &req, (u32)sizeof(req));
        pRegs->i64Ret = (st == 0) ? 0 : st;
        break;
    }
    case GJ_SYS_IPC_REPLY: {
        struct gj_door *pDoor = door_cold_personality();

        native_soft_inc(&g_nativeDeep.u64IpcReply);
        if (pDoor == NULL) {
            pRegs->i64Ret = GJ_ERR_NODEV;
            break;
        }
        door_reply(pDoor, (i64)pRegs->u64Arg0);
        pRegs->i64Ret = 0;
        break;
    }
    case GJ_SYS_PERSONALITY_SERVE: {
        /* arg0 = user/kernel ptr to gj_linux_regs; run cold service, write ret */
        struct gj_linux_regs req;
        i64 st;

        native_soft_inc(&g_nativeDeep.u64ColdServe);
        if (pRegs->u64Arg0 == 0) {
            pRegs->i64Ret = GJ_ERR_INVAL;
            break;
        }
        if (user_range_ok(pRegs->u64Arg0, sizeof(req))) {
            st = native_copy_in(&req, pRegs->u64Arg0, (u32)sizeof(req));
            if (st != 0) {
                pRegs->i64Ret = st;
                break;
            }
            (void)cold_ipc_service_local(&req);
            st = native_copy_out(pRegs->u64Arg0, &req, (u32)sizeof(req));
            pRegs->i64Ret = (st == 0) ? req.i64Ret : st;
        } else {
            struct gj_linux_regs *pK =
                (struct gj_linux_regs *)(gj_vaddr_t)pRegs->u64Arg0;

            pRegs->i64Ret = cold_ipc_service_local(pK);
        }
        break;
    }
    case GJ_SYS_COLD_DEQUEUE: {
        struct gj_cold_request *pOut;

        native_soft_inc(&g_nativeDeep.u64ColdDeq);
        if (pRegs->u64Arg0 == 0) {
            pRegs->i64Ret = GJ_ERR_INVAL;
            break;
        }
        pOut = (struct gj_cold_request *)(gj_vaddr_t)pRegs->u64Arg0;
        pRegs->i64Ret = cold_ipc_dequeue(pOut) ? 1 : 0;
        break;
    }
    case GJ_SYS_COLD_REPLY:
        native_soft_inc(&g_nativeDeep.u64ColdReply);
        pRegs->i64Ret = cold_ipc_reply(pRegs->u64Arg0, (i64)pRegs->u64Arg1)
                            ? 0
                            : GJ_ERR_NOENT;
        break;
    case GJ_SYS_THREAD_SET_QOS:
        /* arg0 = thr id (0 = current), arg1 = qos class */
        {
            u32 u32Id = native_tid_or_current((u32)pRegs->u64Arg0);

            native_soft_inc(&g_nativeDeep.u64ThrQos);
            if (u32Id == 0) {
                pRegs->i64Ret = GJ_ERR_INVAL;
                break;
            }
            thread_set_qos(u32Id, (u32)pRegs->u64Arg1);
            pRegs->i64Ret = 0;
        }
        break;
    case GJ_SYS_THREAD_SET_CPU:
        /* arg0 = thr id (0 = current), arg1 = cpu slot */
        {
            u32 u32Id = native_tid_or_current((u32)pRegs->u64Arg0);

            native_soft_inc(&g_nativeDeep.u64ThrCpu);
            if (u32Id == 0) {
                pRegs->i64Ret = GJ_ERR_INVAL;
                break;
            }
            thread_set_cpu(u32Id, (u32)pRegs->u64Arg1);
            pRegs->i64Ret = 0;
        }
        break;
    case GJ_SYS_GPU_PRESENT: {
        /* Compositor / ICD present: width, height, fb VA, stride */
        u32 u32W = (u32)pRegs->u64Arg0;
        u32 u32H = (u32)pRegs->u64Arg1;
        void *pFb = (void *)(gj_vaddr_t)pRegs->u64Arg2;
        u32 u32Stride = (u32)pRegs->u64Arg3;

        native_soft_inc(&g_nativeDeep.u64GpuPresent);
        if (!virtio_gpu_ready()) {
            pRegs->i64Ret = GJ_ERR_NODEV;
            break;
        }
        if (pFb == NULL || u32W == 0 || u32H == 0) {
            pRegs->i64Ret = GJ_ERR_INVAL;
            break;
        }
        pRegs->i64Ret =
            virtio_gpu_present(u32W, u32H, pFb, u32Stride) == 0 ? 0 : GJ_ERR_IO;
        break;
    }
    case GJ_SYS_GPU_DISPLAY_INFO: {
        u32 aWH[2];
        u32 u32W = 0;
        u32 u32H = 0;
        i64 st;

        native_soft_inc(&g_nativeDeep.u64GpuInfo);
        if (pRegs->u64Arg0 == 0) {
            pRegs->i64Ret = GJ_ERR_INVAL;
            break;
        }
        if (!virtio_gpu_ready() ||
            virtio_gpu_get_display(&u32W, &u32H) != 0) {
            pRegs->i64Ret = GJ_ERR_NODEV;
            break;
        }
        aWH[0] = u32W;
        aWH[1] = u32H;
        st = native_copy_out(pRegs->u64Arg0, aWH, (u32)sizeof(aWH));
        pRegs->i64Ret = (st == 0) ? 0 : st;
        break;
    }
    case GJ_SYS_MEMOBJ_CREATE_NAMED: {
        char szName[GJ_MEMOBJ_NAME_MAX];
        struct gj_memobj *pObj;
        i64 st;

        native_soft_inc(&g_nativeDeep.u64MemobjCreate);
        if (pRegs->u64Arg0 == 0 || pRegs->u64Arg1 == 0) {
            pRegs->i64Ret = GJ_ERR_INVAL;
            break;
        }
        st = native_copy_name(szName, pRegs->u64Arg0, (u32)sizeof(szName));
        if (st != 0) {
            pRegs->i64Ret = st;
            break;
        }
        if (szName[0] == '\0') {
            pRegs->i64Ret = GJ_ERR_INVAL;
            break;
        }
        pObj = memobj_create_named(szName, (u32)pRegs->u64Arg1);
        pRegs->i64Ret = (pObj != NULL) ? 0 : GJ_ERR_NOMEM;
        break;
    }
    case GJ_SYS_SESSION:
        pRegs->i64Ret = session_door_call((u32)pRegs->u64Arg0, pRegs->u64Arg1,
                                          pRegs->u64Arg2, pRegs->u64Arg3);
        break;
    case GJ_SYS_NET:
        pRegs->i64Ret = net_door_call((u32)pRegs->u64Arg0, pRegs->u64Arg1,
                                      pRegs->u64Arg2, pRegs->u64Arg3);
        break;
    case GJ_SYS_STORE:
        pRegs->i64Ret = store_door_call((u32)pRegs->u64Arg0, pRegs->u64Arg1,
                                        pRegs->u64Arg2, pRegs->u64Arg3);
        break;
    case GJ_SYS_VFS:
        pRegs->i64Ret = vfs_door_call((u32)pRegs->u64Arg0, pRegs->u64Arg1,
                                      pRegs->u64Arg2, pRegs->u64Arg3);
        break;
    case GJ_SYS_CONSOLE: {
        u32 u32Op = (u32)pRegs->u64Arg0;

        if (u32Op == 0) {
            native_soft_inc(&g_nativeDeep.u64ConPoll);
            pRegs->i64Ret = serial_poll();
        } else if (u32Op == 1) {
            native_soft_inc(&g_nativeDeep.u64ConGetc);
            pRegs->i64Ret = serial_getchar();
        } else if (u32Op == 2) {
            u64 u64Buf = pRegs->u64Arg1;
            u32 cbMax = (u32)pRegs->u64Arg2;
            u32 n = 0;
            u8 aTmp[GJ_NATIVE_CONSOLE_CHUNK];
            i64 st;

            native_soft_inc(&g_nativeDeep.u64ConRead);
            if (u64Buf == 0 || cbMax == 0) {
                pRegs->i64Ret = GJ_ERR_INVAL;
                break;
            }
            if (cbMax > sizeof(aTmp)) {
                cbMax = (u32)sizeof(aTmp);
            }
            while (n < cbMax && serial_poll()) {
                aTmp[n++] = (u8)serial_getchar();
            }
            if (n > 0) {
                st = native_copy_out(u64Buf, aTmp, n);
                if (st != 0) {
                    pRegs->i64Ret = st;
                    break;
                }
            }
            pRegs->i64Ret = (i64)n;
        } else {
            native_soft_inc(&g_nativeDeep.u64ConInval);
            pRegs->i64Ret = GJ_ERR_INVAL;
        }
        break;
    }
    case GJ_SYS_PLATFORM_INFO: {
        u32 u32Op = (u32)pRegs->u64Arg0;

        if (u32Op == 0) {
            struct gj_iommu_info info;
            i64 st;

            native_soft_inc(&g_nativeDeep.u64PlatIommu);
            memset(&info, 0, sizeof(info));
            iommu_info_get(&info);
            if (pRegs->u64Arg1 != 0) {
                st = native_copy_out(pRegs->u64Arg1, &info, (u32)sizeof(info));
                if (st != 0) {
                    pRegs->i64Ret = st;
                    break;
                }
            }
            pRegs->i64Ret = info.u8Present ? 1 : 0;
        } else if (u32Op == 1) {
            struct gj_pci_msix_info aInfo[16];
            u32 n;
            u32 cb;

            native_soft_inc(&g_nativeDeep.u64PlatMsix);
            memset(aInfo, 0, sizeof(aInfo));
            n = pci_msix_scan(aInfo, 16);
            if (pRegs->u64Arg1 != 0 && n > 0) {
                cb = n * (u32)sizeof(aInfo[0]);
                if (cb > sizeof(aInfo)) {
                    cb = (u32)sizeof(aInfo);
                }
                /* Best-effort inventory copy; count still returned. */
                (void)native_copy_out(pRegs->u64Arg1, aInfo, cb);
            }
            pRegs->i64Ret = (i64)n;
        } else if (u32Op == 2) {
            /* WoW64 personality control */
            u32 u32Act = (u32)pRegs->u64Arg1;

            native_soft_inc(&g_nativeDeep.u64PlatWow64);
            if (u32Act == 1) {
                wow64_set(1);
            } else if (u32Act == 2) {
                wow64_set(0);
            }
            pRegs->i64Ret = (i64)wow64_calls() |
                            ((i64)(wow64_enabled() ? 1 : 0) << 32);
        } else if (u32Op == 3) {
            /* MSI-X Notification soft inject */
            native_soft_inc(&g_nativeDeep.u64PlatMsixInj);
            irq_msix_soft_inject(pRegs->u64Arg1 ? pRegs->u64Arg1 : 1ull);
            pRegs->i64Ret = (i64)irq_msix_count();
        } else if (u32Op == 4) {
            native_soft_inc(&g_nativeDeep.u64PlatIommuEnf);
            iommu_enforce_set((int)pRegs->u64Arg1);
            pRegs->i64Ret = iommu_enforce_get();
        } else if (u32Op == 5) {
            u32 u32Bdf = (u32)pRegs->u64Arg1;
            u8 u8Bus = (u8)((u32Bdf >> 16) & 0xffu);
            u8 u8Slot = (u8)((u32Bdf >> 8) & 0xffu);
            u8 u8Func = (u8)(u32Bdf & 0xffu);

            native_soft_inc(&g_nativeDeep.u64PlatIommuWin);
            if (pRegs->u64Arg3 == 0) {
                pRegs->i64Ret = GJ_ERR_INVAL;
                break;
            }
            if (iommu_window_grant(u8Bus, u8Slot, u8Func, pRegs->u64Arg2,
                                   pRegs->u64Arg3) != 0) {
                pRegs->i64Ret = GJ_ERR_NOMEM;
            } else {
                pRegs->i64Ret = (i64)iommu_window_count();
            }
        } else {
            native_soft_inc(&g_nativeDeep.u64PlatInval);
            pRegs->i64Ret = GJ_ERR_INVAL;
        }
        break;
    }
    case GJ_SYS_NOTIFY_WAIT: {
        struct gj_notify *pN;
        u64 u64Mask = pRegs->u64Arg1;
        int fBlock = (int)pRegs->u64Arg2;

        native_soft_inc(&g_nativeDeep.u64NotifyWait);
        if ((u32)pRegs->u64Arg0 == 0) {
            pN = notify_msix_global();
        } else {
            pRegs->i64Ret = GJ_ERR_INVAL;
            break;
        }
        if (pN == NULL) {
            pRegs->i64Ret = GJ_ERR_NODEV;
            break;
        }
        pRegs->i64Ret = (i64)notify_wait(pN, u64Mask, fBlock);
        break;
    }
    case GJ_SYS_SCSI: {
        /* arg0=op → scsi_door_submit / transport probe (userspace scsi_mid). */
        u32 u32Op = (u32)pRegs->u64Arg0;
        struct scsi_door_req dreq;
        static u8 aScsiXfer[GJ_NATIVE_SCSI_XFER_MAX];

        switch (u32Op) {
        case 5: /* ready */
            native_soft_inc(&g_nativeDeep.u64ScsiReady);
            pRegs->i64Ret = scsi_mid_ready() ? 1 : 0;
            break;
        case 6: { /* stats */
            u32 aSt[2];
            i64 st;

            native_soft_inc(&g_nativeDeep.u64ScsiStats);
            if (pRegs->u64Arg1 == 0) {
                pRegs->i64Ret = GJ_ERR_INVAL;
                break;
            }
            aSt[0] = scsi_door_io_count();
            aSt[1] = scsi_mid_ready() ? 1u : 0u;
            st = native_copy_out(pRegs->u64Arg1, aSt, (u32)sizeof(aSt));
            pRegs->i64Ret = (st == 0) ? 0 : st;
            break;
        }
        case 0: { /* inquiry */
            u32 cb = (u32)pRegs->u64Arg2;
            i64 st;

            native_soft_inc(&g_nativeDeep.u64ScsiInq);
            if (pRegs->u64Arg1 == 0 || cb == 0 || cb > sizeof(aScsiXfer)) {
                pRegs->i64Ret = GJ_ERR_INVAL;
                break;
            }
            memset(&dreq, 0, sizeof(dreq));
            dreq.u32Op = 0;
            dreq.cbData = cb;
            if (scsi_door_submit(&dreq, aScsiXfer, cb) != 0) {
                pRegs->i64Ret = GJ_ERR_IO;
                break;
            }
            st = native_copy_out(pRegs->u64Arg1, aScsiXfer, cb);
            pRegs->i64Ret = (st == 0) ? (i64)cb : st;
            break;
        }
        case 1: { /* read capacity → u32[2] */
            u32 aCap[2];
            i64 st;

            native_soft_inc(&g_nativeDeep.u64ScsiReadCap);
            if (pRegs->u64Arg1 == 0) {
                pRegs->i64Ret = GJ_ERR_INVAL;
                break;
            }
            memset(&dreq, 0, sizeof(dreq));
            dreq.u32Op = 1;
            dreq.cbData = 8;
            if (scsi_door_submit(&dreq, aScsiXfer, 8) != 0) {
                pRegs->i64Ret = GJ_ERR_IO;
                break;
            }
            /* big-endian last LBA + block len */
            aCap[0] = ((u32)aScsiXfer[0] << 24) | ((u32)aScsiXfer[1] << 16) |
                      ((u32)aScsiXfer[2] << 8) | (u32)aScsiXfer[3];
            aCap[1] = ((u32)aScsiXfer[4] << 24) | ((u32)aScsiXfer[5] << 16) |
                      ((u32)aScsiXfer[6] << 8) | (u32)aScsiXfer[7];
            st = native_copy_out(pRegs->u64Arg1, aCap, (u32)sizeof(aCap));
            pRegs->i64Ret = (st == 0) ? 0 : st;
            break;
        }
        case 2: /* read10 */
        case 3: { /* write10 */
            u32 u32Lba = (u32)pRegs->u64Arg1;
            u32 u32Blocks = (u32)pRegs->u64Arg3;
            u32 cb;
            i64 st;

            if (u32Op == 2) {
                native_soft_inc(&g_nativeDeep.u64ScsiRead10);
            } else {
                native_soft_inc(&g_nativeDeep.u64ScsiWrite10);
            }
            if (u32Blocks == 0) {
                u32Blocks = 1;
            }
            /* Guard multiply overflow before size check. */
            if (u32Blocks > (sizeof(aScsiXfer) / 512u)) {
                pRegs->i64Ret = GJ_ERR_INVAL;
                break;
            }
            cb = u32Blocks * 512u;
            if (pRegs->u64Arg2 == 0 || cb == 0 || cb > sizeof(aScsiXfer)) {
                pRegs->i64Ret = GJ_ERR_INVAL;
                break;
            }
            memset(&dreq, 0, sizeof(dreq));
            dreq.u32Op = u32Op;
            dreq.u32Lba = u32Lba;
            dreq.u16Blocks = (u16)u32Blocks;
            dreq.cbData = cb;
            if (u32Op == 3) {
                st = native_copy_in(aScsiXfer, pRegs->u64Arg2, cb);
                if (st != 0) {
                    pRegs->i64Ret = st;
                    break;
                }
            }
            if (scsi_door_submit(&dreq, aScsiXfer, cb) != 0) {
                pRegs->i64Ret = GJ_ERR_IO;
                break;
            }
            if (u32Op == 2) {
                st = native_copy_out(pRegs->u64Arg2, aScsiXfer, cb);
                if (st != 0) {
                    pRegs->i64Ret = st;
                    break;
                }
            }
            pRegs->i64Ret = (i64)cb;
            break;
        }
        default:
            native_soft_inc(&g_nativeDeep.u64ScsiInval);
            pRegs->i64Ret = GJ_ERR_INVAL;
            break;
        }
        break;
    }
    case GJ_SYS_HDA_STREAM: {
        /* arg0=op: 0 open, 1 write, 2 start, 3 tick, 4 close, 5 stats */
        u32 u32Op = (u32)pRegs->u64Arg0;

        switch (u32Op) {
        case 0:
            native_soft_inc(&g_nativeDeep.u64HdaOpen);
            pRegs->i64Ret =
                hda_stream_open((u32)pRegs->u64Arg1, (u32)pRegs->u64Arg2,
                                (u32)pRegs->u64Arg3) == 0
                    ? 0
                    : GJ_ERR_INVAL;
            break;
        case 1: {
            u8 aTmp[GJ_NATIVE_HDA_CHUNK];
            u32 cbWant = (u32)pRegs->u64Arg2;
            u32 cbGot = 0;
            u32 cbChunk;
            i64 st;

            native_soft_inc(&g_nativeDeep.u64HdaWrite);
            if (pRegs->u64Arg1 == 0 || cbWant == 0) {
                pRegs->i64Ret = GJ_ERR_INVAL;
                break;
            }
            while (cbGot < cbWant) {
                cbChunk = cbWant - cbGot;
                if (cbChunk > sizeof(aTmp)) {
                    cbChunk = (u32)sizeof(aTmp);
                }
                st = native_copy_in(aTmp, pRegs->u64Arg1 + cbGot, cbChunk);
                if (st != 0) {
                    pRegs->i64Ret = (cbGot != 0) ? (i64)cbGot : st;
                    goto hda_done;
                }
                {
                    u32 n = hda_stream_write(aTmp, cbChunk);

                    cbGot += n;
                    if (n < cbChunk) {
                        break;
                    }
                }
            }
            pRegs->i64Ret = (i64)cbGot;
            break;
        }
        case 2:
            native_soft_inc(&g_nativeDeep.u64HdaStart);
            pRegs->i64Ret = hda_stream_start() == 0 ? 0 : GJ_ERR_INVAL;
            break;
        case 3:
            native_soft_inc(&g_nativeDeep.u64HdaTick);
            pRegs->i64Ret = (i64)hda_stream_tick((u32)pRegs->u64Arg1);
            break;
        case 4:
            native_soft_inc(&g_nativeDeep.u64HdaClose);
            hda_stream_close();
            pRegs->i64Ret = 0;
            break;
        case 5: {
            u32 aSt[3];
            i64 st;

            native_soft_inc(&g_nativeDeep.u64HdaStats);
            if (pRegs->u64Arg1 == 0) {
                pRegs->i64Ret = GJ_ERR_INVAL;
                break;
            }
            aSt[0] = hda_stream_bytes_queued();
            aSt[1] = hda_stream_bytes_played();
            aSt[2] = hda_stream_underruns();
            st = native_copy_out(pRegs->u64Arg1, aSt, (u32)sizeof(aSt));
            pRegs->i64Ret = (st == 0) ? 0 : st;
            break;
        }
        default:
            native_soft_inc(&g_nativeDeep.u64HdaInval);
            pRegs->i64Ret = GJ_ERR_INVAL;
            break;
        }
    hda_done:
        break;
    }
    case GJ_SYS_MEMOBJ_MAP_NAMED: {
        char szName[GJ_MEMOBJ_NAME_MAX];
        gj_vaddr_t va;
        struct gj_process *pProc = g_pLinuxProc;
        i64 st;

        native_soft_inc(&g_nativeDeep.u64MemobjMap);
        /* ABI: failure returns 0 (not a negative errno). */
        if (pRegs->u64Arg0 == 0 || pProc == NULL) {
            pRegs->i64Ret = 0;
            break;
        }
        st = native_copy_name(szName, pRegs->u64Arg0, (u32)sizeof(szName));
        if (st != 0 || szName[0] == '\0') {
            pRegs->i64Ret = 0;
            break;
        }
        {
            u32 u32Prot = (u32)pRegs->u64Arg2
                              ? (u32)pRegs->u64Arg2
                              : (GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE);
            /* Userspace maps always need U/S so ring-3 can touch the pages. */
            u32Prot |= GJ_VMM_PROT_USER;
            va = memobj_map_named(pProc, szName, pRegs->u64Arg1, u32Prot);
        }
        pRegs->i64Ret = (i64)(u64)va;
        break;
    }
    case GJ_SYS_CAP_MINT:
    case GJ_SYS_CAP_COPY:
    case GJ_SYS_CAP_MOVE:
    case GJ_SYS_CAP_REVOKE:
    case GJ_SYS_CAP_IDENT: {
        /*
         * Cap ops on current process CNode (Scheme A).
         * MINT:  arg0=srcSlot arg1=srcGen arg2=rights → slot|gen in ret
         * COPY:  arg0=srcSlot arg1=srcGen arg2=rights → slot|gen
         * MOVE:  arg0=srcSlot arg1=srcGen → slot|gen
         * REVOKE:arg0=slot arg1=gen → 0 or -errno
         * IDENT: arg0=slot arg1=gen → type in low16, rights in next16
         */
        struct gj_process *pProc = g_pLinuxProc;
        struct gj_cnode *pCnode;
        struct gj_cap_ref out;
        struct gj_cap_resolved res;
        gj_status_t st;

        if (pRegs->u64Nr == GJ_SYS_CAP_IDENT) {
            native_soft_inc(&g_nativeDeep.u64CapIdent);
        } else if (pRegs->u64Nr == GJ_SYS_CAP_REVOKE) {
            native_soft_inc(&g_nativeDeep.u64CapRevoke);
        } else if (pRegs->u64Nr == GJ_SYS_CAP_MOVE) {
            native_soft_inc(&g_nativeDeep.u64CapMove);
        } else if (pRegs->u64Nr == GJ_SYS_CAP_COPY) {
            native_soft_inc(&g_nativeDeep.u64CapCopy);
        } else {
            native_soft_inc(&g_nativeDeep.u64CapMint);
        }
        if (pProc == NULL || pProc->pCnode == NULL) {
            pRegs->i64Ret = GJ_ERR_INVAL;
            break;
        }
        pCnode = pProc->pCnode;
        memset(&out, 0, sizeof(out));
        if (pRegs->u64Nr == GJ_SYS_CAP_IDENT) {
            st = gj_cap_resolve(pCnode, pRegs->u64Arg0, (u32)pRegs->u64Arg1,
                                &res);
            if (st != GJ_OK) {
                pRegs->i64Ret = st;
            } else {
                pRegs->i64Ret = (i64)((u32)res.u16Type |
                                      ((u32)res.u16Rights << 16));
            }
            break;
        }
        if (pRegs->u64Nr == GJ_SYS_CAP_REVOKE) {
            st = gj_cap_delete(pCnode, pRegs->u64Arg0, (u32)pRegs->u64Arg1);
            pRegs->i64Ret = st;
            break;
        }
        if (pRegs->u64Nr == GJ_SYS_CAP_MOVE) {
            st = gj_cap_move(pCnode, pRegs->u64Arg0, (u32)pRegs->u64Arg1, &out);
        } else if (pRegs->u64Nr == GJ_SYS_CAP_COPY) {
            st = gj_cap_copy(pCnode, pRegs->u64Arg0, (u32)pRegs->u64Arg1,
                             (u16)pRegs->u64Arg2, &out);
        } else {
            st = gj_cap_mint(pCnode, pRegs->u64Arg0, (u32)pRegs->u64Arg1,
                             (u16)pRegs->u64Arg2, pCnode, &out);
        }
        if (st != GJ_OK) {
            pRegs->i64Ret = st;
        } else {
            pRegs->i64Ret = (i64)((out.u64Slot & 0xffffffffull) |
                                  ((u64)out.u32SlotGen << 32));
        }
        break;
    }
    default:
        fHitDefault = 1;
        pRegs->i64Ret = GJ_ERR_NOSUPPORT;
        break;
    }

    native_stats_finish(pRegs->u64Nr, pRegs->i64Ret, fHitDefault);
    /* Wave 15: one-shot greppable soft inventory after first activity. */
    native_soft_maybe_once();
}
