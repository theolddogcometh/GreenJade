/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room Linux x86_64 syscall surface (public man pages / ABI docs).
 * Not Linux source. Hybrid Option C: hot paths in kernel, cold → personality.
 * Pure C11 freestanding — dual MIT OR Apache-2.0.
 *
 * -------------------------------------------------------------------------
 * Scope (header-only ABI catalog)
 * -------------------------------------------------------------------------
 * Stable numeric surface consumed by hybrid dispatch, PE32/WoW64 soft paths,
 * freestanding libgj, and cold personality (libprotonrt / doors):
 *   LINUX_E*           errno values returned as -errno in rax / i64Ret
 *   LINUX_NR_*         x86_64 SYSCALL numbers (public syscall_64.tbl ABI)
 *   LINUX_PROT_*, LINUX_MAP_*, CLOCK_*, ARCH_*, open/at/seek/wait/clone
 *   struct gj_linux_regs, linux_timespec, enum gj_linux_path
 *
 * This header is macros + small POD types + optional static inlines only.
 * No .c, no runtime state, no GPL paste. Values match public Linux ABI
 * numbers; code that implements them lives in linux_hot, linux_dispatch,
 * and cold_ipc.
 *
 * -------------------------------------------------------------------------
 * Contract
 * -------------------------------------------------------------------------
 *   - NR and errno numeric values are ABI-stable once present: never renumber
 *     existing LINUX_NR_* / LINUX_E* / flag macros. Append new names only.
 *   - Return convention: success >= 0 in rax; failure = -(LINUX_E code).
 *   - Native kernel status is gj_status_t / GJ_ERR_* (gj/error.h), not
 *     interchangeable with LINUX_E*. Map at the personality / door edge.
 *   - Sparse dispatch table span is owned by linux_dispatch.h
 *     (GJ_LINUX_NR_TABLE, currently 512). Catalog here may list NRs above
 *     any soft product maximum; unknown or unregistered -> PATH_NONE / ENOSYS.
 *   - Option C (docs/LINUX_ABI_HYBRID.md): HOT in-kernel, COLD doors to
 *     personality, NONE -> ENOSYS. Classification is not encoded in NR
 *     values; see enum gj_linux_path and gj_linux_classify().
 *
 * -------------------------------------------------------------------------
 * Clean-room / product notes
 * -------------------------------------------------------------------------
 * Derived from published man-pages and architecture ABI tables only.
 * Companion: docs/LINUX_ABI_HYBRID.md, docs/PROTON_PERSONALITY.md,
 *            docs/GLIBC_COMPAT.md, docs/SECURITY_CORE_DESIGN.md
 * PE32 int80 / i386 NR map is wow64.h (maps into these x86_64 NRs).
 *
 * greppable: LINUX_NR_ LINUX_E LINUX_PROT_ LINUX_MAP_ LINUX_CLOCK_
 * greppable: GJ_LINUX_PATH LINUX_ABI_HYBRID Option_C gj_linux_regs
 * greppable: LINUX_O_ LINUX_AT_ LINUX_SEEK_ LINUX_CLONE_ LINUX_WNOHANG
 */
#pragma once

#include <gj/types.h>

/* ---- Linux errno (subset; return as -errno in rax) ---------------------- */
/*
 * Public Linux/x86 errno numbers (asm-generic/errno*.h shape). Incomplete by
 * design — product hot/cold paths only need the classes they actually return.
 * Append new codes with their true public values; never renumber.
 *
 * Soft mapping tip (native → Linux edge): GJ_ERR_NOENT→ENOENT, PERM→EACCES
 * or EPERM, NOMEM→ENOMEM, FAULT→EFAULT, TIMEOUT→ETIMEDOUT, NOSUPPORT→ENOSYS,
 * BUSY→EBUSY, AGAIN→EAGAIN, PEER_DEAD often →EIO at doors (product choice).
 */
#define LINUX_EPERM        1
#define LINUX_ENOENT       2
#define LINUX_ESRCH        3
#define LINUX_EINTR        4
#define LINUX_EIO          5
#define LINUX_ENXIO        6
#define LINUX_E2BIG        7
#define LINUX_ENOEXEC      8
#define LINUX_EBADF        9
#define LINUX_ECHILD      10
#define LINUX_EAGAIN      11
/** Same numeric value as EAGAIN (public Linux); alias for nonblock APIs. */
#define LINUX_EWOULDBLOCK LINUX_EAGAIN
#define LINUX_ENOMEM      12
#define LINUX_EACCES      13
#define LINUX_EFAULT      14
#define LINUX_EBUSY       16
#define LINUX_EEXIST      17
#define LINUX_EXDEV       18   /* deepen: cross-device link/rename */
#define LINUX_ENODEV      19
#define LINUX_ENOTDIR     20
#define LINUX_EISDIR      21   /* deepen: directory where file expected */
#define LINUX_EINVAL      22
#define LINUX_ENFILE      23
#define LINUX_EMFILE      24
#define LINUX_ENOTTY      25   /* deepen: inappropriate ioctl */
#define LINUX_EFBIG       27   /* deepen: file too large */
#define LINUX_ENOSPC      28
#define LINUX_ESPIPE      29   /* deepen: illegal seek on pipe/socket */
#define LINUX_EROFS       30
#define LINUX_EMLINK      31   /* deepen: too many links */
#define LINUX_EPIPE       32
#define LINUX_ERANGE      34
#define LINUX_EDEADLK     35   /* deepen: resource deadlock would occur */
#define LINUX_ENAMETOOLONG 36  /* deepen: path/name too long */
#define LINUX_ENOLCK      37
#define LINUX_ENOSYS      38
#define LINUX_ENOTEMPTY   39
#define LINUX_ELOOP       40   /* deepen: too many symlink levels */
#define LINUX_ENOMSG      42
#define LINUX_EIDRM       43
#define LINUX_ENODATA     61
#define LINUX_ETIME       62
#define LINUX_ENOSR       63
#define LINUX_ENOLINK     67
#define LINUX_EPROTO      71
#define LINUX_EMULTIHOP   72
#define LINUX_EBADMSG     74
#define LINUX_EOVERFLOW   75
#define LINUX_EILSEQ      84
#define LINUX_ENOTSOCK    88
#define LINUX_EDESTADDRREQ 89
#define LINUX_EMSGSIZE    90
#define LINUX_EPROTOTYPE  91
#define LINUX_ENOPROTOOPT 92
#define LINUX_EPROTONOSUPPORT 93
#define LINUX_ESOCKTNOSUPPORT 94
#define LINUX_EOPNOTSUPP  95
#define LINUX_EPFNOSUPPORT 96
#define LINUX_EAFNOSUPPORT 97
#define LINUX_EADDRINUSE  98
#define LINUX_EADDRNOTAVAIL 99
#define LINUX_ENETDOWN   100
#define LINUX_ENETUNREACH 101
#define LINUX_ENETRESET  102
#define LINUX_ECONNABORTED 103
#define LINUX_ECONNRESET 104
#define LINUX_ENOBUFS    105
#define LINUX_EISCONN    106
#define LINUX_ENOTCONN   107
#define LINUX_ESHUTDOWN  108
#define LINUX_ETOOMANYREFS 109
#define LINUX_ETIMEDOUT    110
#define LINUX_ECONNREFUSED 111
#define LINUX_EHOSTDOWN  112
#define LINUX_EHOSTUNREACH 113
#define LINUX_EALREADY   114
#define LINUX_EINPROGRESS 115
#define LINUX_ESTALE     116
#define LINUX_EDQUOT     122
#define LINUX_ECANCELED  125
#define LINUX_EOWNERDEAD 130
#define LINUX_ENOTRECOVERABLE 131
#define LINUX_ERFKILL    132
#define LINUX_EHWPOISON  133

/* ---- x86_64 Linux syscall numbers (public ABI) -------------------------- */
/*
 * Numbers are the public x86_64 Linux SYSCALL ABI (syscall_64.tbl). Order in
 * this file is historical / grouped for greppability — not NR order. Values
 * alone are the ABI contract.
 *
 * Soft product use:
 *   - HOT set registered in linux_dispatch (getpid, write, mmap, futex, …)
 *   - COLD → doors / protonrt_cold_linux for FS/net/poll/ioctl-heavy NRs
 *   - Unregistered / PATH_NONE → -ENOSYS (fail closed, not silent success)
 *
 * Deepen policy: fill historical gaps used by glibc/musl/Steam/Proton soft
 * paths; keep every pre-existing LINUX_NR_* value unchanged.
 */

/* I/O + classic path */
#define LINUX_NR_read              0
#define LINUX_NR_write             1
#define LINUX_NR_open              2
#define LINUX_NR_close             3
#define LINUX_NR_stat              4
#define LINUX_NR_fstat             5
#define LINUX_NR_lstat             6
#define LINUX_NR_poll              7
#define LINUX_NR_lseek             8
#define LINUX_NR_mmap              9
#define LINUX_NR_mprotect         10
#define LINUX_NR_munmap           11
#define LINUX_NR_brk              12
#define LINUX_NR_rt_sigaction     13
#define LINUX_NR_rt_sigprocmask   14
#define LINUX_NR_ioctl            16
#define LINUX_NR_pread64          17
#define LINUX_NR_pwrite64         18
#define LINUX_NR_readv            19
#define LINUX_NR_writev           20
#define LINUX_NR_preadv          295
#define LINUX_NR_pwritev         296
#define LINUX_NR_preadv2         327
#define LINUX_NR_pwritev2        328
#define LINUX_NR_access           21
#define LINUX_NR_pipe             22
#define LINUX_NR_select           23
#define LINUX_NR_sched_yield      24
#define LINUX_NR_mremap           25
#define LINUX_NR_msync            26
#define LINUX_NR_mincore          27
#define LINUX_NR_madvise         28
/* SysV IPC (deepen catalog; typically COLD / ENOSYS until product wires) */
#define LINUX_NR_shmget           29
#define LINUX_NR_shmat            30
#define LINUX_NR_shmctl           31
#define LINUX_NR_dup              32
#define LINUX_NR_dup2             33
#define LINUX_NR_dup3            292
#define LINUX_NR_recvmmsg        299
#define LINUX_NR_sendmmsg        307
#define LINUX_NR_nanosleep        35
#define LINUX_NR_getitimer        36
#define LINUX_NR_setitimer        38
#define LINUX_NR_getpid           39
#define LINUX_NR_socket           41
#define LINUX_NR_connect          42
#define LINUX_NR_accept           43
#define LINUX_NR_sendto           44
#define LINUX_NR_recvfrom         45
#define LINUX_NR_sendmsg          46
#define LINUX_NR_recvmsg          47
#define LINUX_NR_shutdown         48
#define LINUX_NR_socketpair       53
#define LINUX_NR_bind             49
#define LINUX_NR_listen           50
#define LINUX_NR_getsockname      51
#define LINUX_NR_getpeername      52
#define LINUX_NR_setsockopt       54
#define LINUX_NR_getsockopt       55
#define LINUX_NR_clone            56
#define LINUX_NR_fork             57
#define LINUX_NR_vfork            58
#define LINUX_NR_execve           59
#define LINUX_NR_execveat        322
#define LINUX_NR_exit             60
#define LINUX_NR_wait4            61
#define LINUX_NR_kill             62
#define LINUX_NR_tkill           200
#define LINUX_NR_tgkill          234
#define LINUX_NR_uname            63
#define LINUX_NR_semget           64
#define LINUX_NR_semop            65
#define LINUX_NR_semctl           66
#define LINUX_NR_shmdt            67
#define LINUX_NR_msgget           68
#define LINUX_NR_msgsnd           69
#define LINUX_NR_msgrcv           70
#define LINUX_NR_msgctl           71
#define LINUX_NR_rt_sigreturn     15
#define LINUX_NR_rt_sigpending    127
#define LINUX_NR_rt_sigtimedwait  128
#define LINUX_NR_rt_sigqueueinfo  129
#define LINUX_NR_rt_sigsuspend    130
#define LINUX_NR_sigaltstack      131
#define LINUX_NR_sched_setparam   142
#define LINUX_NR_sched_getparam   143
#define LINUX_NR_sched_setscheduler 144
#define LINUX_NR_sched_getscheduler 145
#define LINUX_NR_sched_get_priority_max 146
#define LINUX_NR_sched_get_priority_min 147
#define LINUX_NR_sched_rr_get_interval 148
#define LINUX_NR_unshare          272
#define LINUX_NR_setns            308
#define LINUX_NR_chroot            161
#define LINUX_NR_mount            165
#define LINUX_NR_umount2          166
#define LINUX_NR_pivot_root       155
#define LINUX_NR_swapon           167
#define LINUX_NR_swapoff          168
#define LINUX_NR_reboot           169
#define LINUX_NR_sethostname      170
#define LINUX_NR_setdomainname    171
#define LINUX_NR_readahead        187
#define LINUX_NR_sync_file_range  277
#define LINUX_NR_vmsplice         278
#define LINUX_NR_getdents          78
#define LINUX_NR_syslog           103
#define LINUX_NR_getsid           124
/*
 * Slot 134 public name is uselib; historical tree used bdflush for this
 * number — value kept, alias added (do not renumber).
 */
#define LINUX_NR_bdflush          134
#define LINUX_NR_uselib           134
#define LINUX_NR_personality      135
#define LINUX_NR_ustat            136
#define LINUX_NR_sysfs            139
#define LINUX_NR_fcntl            72
#define LINUX_NR_fsync            74
#define LINUX_NR_fdatasync        75
#define LINUX_NR_truncate          76
#define LINUX_NR_sync             162
#define LINUX_NR_syncfs           306
#define LINUX_NR_ftruncate        77
#define LINUX_NR_getcwd           79
#define LINUX_NR_chdir            80
#define LINUX_NR_fchdir           81
#define LINUX_NR_rename           82
#define LINUX_NR_mkdir            83
#define LINUX_NR_rmdir            84
#define LINUX_NR_creat            85
#define LINUX_NR_link             86
#define LINUX_NR_unlink           87
#define LINUX_NR_symlink          88
#define LINUX_NR_readlink         89
#define LINUX_NR_readlinkat      267
#define LINUX_NR_chmod            90
#define LINUX_NR_fchmodat        268
#define LINUX_NR_chown            92
#define LINUX_NR_lchown           94
#define LINUX_NR_umask            95
#define LINUX_NR_getxattr        191
#define LINUX_NR_lgetxattr       192
#define LINUX_NR_fgetxattr       193
#define LINUX_NR_setxattr        188
#define LINUX_NR_lsetxattr       189
#define LINUX_NR_fsetxattr       190
#define LINUX_NR_listxattr       194
#define LINUX_NR_llistxattr      195
#define LINUX_NR_flistxattr      196
#define LINUX_NR_removexattr     197
#define LINUX_NR_lremovexattr    198
#define LINUX_NR_fremovexattr    199
#define LINUX_NR_utime            132
#define LINUX_NR_mknod            133
#define LINUX_NR_utimensat        280
#define LINUX_NR_ptrace           101
#define LINUX_NR_getuid          102
#define LINUX_NR_getgid          104
#define LINUX_NR_setuid          105
#define LINUX_NR_setgid          106
#define LINUX_NR_geteuid         107
#define LINUX_NR_getegid         108
#define LINUX_NR_setpgid         109
#define LINUX_NR_getppid         110
#define LINUX_NR_getpgrp         111
#define LINUX_NR_setsid          112
#define LINUX_NR_setreuid        113
#define LINUX_NR_setregid        114
#define LINUX_NR_getgroups       115
#define LINUX_NR_setgroups       116
#define LINUX_NR_setresuid       117
#define LINUX_NR_getresuid       118
#define LINUX_NR_setresgid       119
#define LINUX_NR_getresgid       120
#define LINUX_NR_getpgid         121
#define LINUX_NR_setfsuid        122
#define LINUX_NR_setfsgid        123
#define LINUX_NR_mlock           149
#define LINUX_NR_munlock         150
#define LINUX_NR_mlockall        151
#define LINUX_NR_munlockall      152
#define LINUX_NR_vhangup         153
#define LINUX_NR_modify_ldt      154
#define LINUX_NR_sysctl          156  /* historical _sysctl */
#define LINUX_NR_mlock2          325
#define LINUX_NR_pkey_mprotect   329
#define LINUX_NR_pkey_alloc      330
#define LINUX_NR_pkey_free       331
#define LINUX_NR_gettimeofday     96
#define LINUX_NR_getrlimit        97
#define LINUX_NR_setrlimit       160
#define LINUX_NR_getrusage        98
#define LINUX_NR_sysinfo          99
#define LINUX_NR_times           100
#define LINUX_NR_time            201
#define LINUX_NR_adjtimex        159
#define LINUX_NR_settimeofday    164
#define LINUX_NR_acct            163
#define LINUX_NR_iopl            172
#define LINUX_NR_ioperm          173
#define LINUX_NR_getpriority     140
#define LINUX_NR_setpriority     141
#define LINUX_NR_capget          125
#define LINUX_NR_capset          126
#define LINUX_NR_sched_setaffinity 203
#define LINUX_NR_sched_getaffinity 204
#define LINUX_NR_set_thread_area 205  /* PE32 TLS soft / glibc i386 bridge */
#define LINUX_NR_get_thread_area 211
#define LINUX_NR_getcpu          309
#define LINUX_NR_alarm           37
#define LINUX_NR_pause           34
#define LINUX_NR_flock           73
#define LINUX_NR_fchmod          91
#define LINUX_NR_fchown          93
#define LINUX_NR_arch_prctl      158
#define LINUX_NR_gettid          186
#define LINUX_NR_futex           202
#define LINUX_NR_set_tid_address 218
#define LINUX_NR_clock_gettime   228
#define LINUX_NR_clock_getres    229
#define LINUX_NR_clock_nanosleep 230
#define LINUX_NR_exit_group      231
#define LINUX_NR_epoll_wait      232
#define LINUX_NR_epoll_ctl       233
#define LINUX_NR_sendfile         40
#define LINUX_NR_prctl           157
#define LINUX_NR_getdents64      217
#define LINUX_NR_waitid          247
#define LINUX_NR_openat          257
#define LINUX_NR_mkdirat         258
#define LINUX_NR_mknodat         259
#define LINUX_NR_fchownat        260
#define LINUX_NR_futimesat       261
#define LINUX_NR_newfstatat      262
#define LINUX_NR_unlinkat        263
#define LINUX_NR_faccessat       269
#define LINUX_NR_pselect6        270
#define LINUX_NR_ppoll           271
#define LINUX_NR_set_robust_list 273
#define LINUX_NR_get_robust_list 274
#define LINUX_NR_splice          275
#define LINUX_NR_tee             276
#define LINUX_NR_inotify_init1   294
#define LINUX_NR_inotify_add_watch 254
#define LINUX_NR_inotify_rm_watch  255
#define LINUX_NR_renameat        264
#define LINUX_NR_linkat          265
#define LINUX_NR_symlinkat       266
#define LINUX_NR_renameat2       316
#define LINUX_NR_copy_file_range 326
#define LINUX_NR_clock_settime   227
#define LINUX_NR_clock_adjtime   305
#define LINUX_NR_io_pgetevents   333
#define LINUX_NR_userfaultfd     323
#define LINUX_NR_seccomp         317
#define LINUX_NR_bpf             321
#define LINUX_NR_keyctl          250
#define LINUX_NR_add_key         248
#define LINUX_NR_request_key     249
#define LINUX_NR_name_to_handle_at 303
#define LINUX_NR_open_by_handle_at 304
#define LINUX_NR_epoll_pwait     281
#define LINUX_NR_signalfd4       282
#define LINUX_NR_timerfd_create  283
#define LINUX_NR_eventfd         284
#define LINUX_NR_fallocate       285
#define LINUX_NR_timerfd_settime 286
#define LINUX_NR_timerfd_gettime 287
#define LINUX_NR_accept4         288
#define LINUX_NR_signalfd        289
#define LINUX_NR_eventfd2        290
#define LINUX_NR_io_setup        206
#define LINUX_NR_io_destroy      207
#define LINUX_NR_io_getevents    208
#define LINUX_NR_io_submit       209
#define LINUX_NR_io_cancel       210
#define LINUX_NR_epoll_create    213
#define LINUX_NR_epoll_create1   291
#define LINUX_NR_pipe2           293
#define LINUX_NR_prlimit64       302
#define LINUX_NR_getrandom       318
#define LINUX_NR_memfd_create    319
#define LINUX_NR_process_vm_readv  310
#define LINUX_NR_process_vm_writev 311
#define LINUX_NR_membarrier      324
#define LINUX_NR_statfs          137
#define LINUX_NR_fstatfs         138
#define LINUX_NR_statx           332
#define LINUX_NR_rseq            334
#define LINUX_NR_openat2         437
#define LINUX_NR_faccessat2      439
#define LINUX_NR_pidfd_send_signal 424
#define LINUX_NR_pidfd_open      434
#define LINUX_NR_clone3          435
#define LINUX_NR_close_range     436
#define LINUX_NR_landlock_create_ruleset 444
#define LINUX_NR_landlock_add_rule       445
#define LINUX_NR_landlock_restrict_self  446
#define LINUX_NR_memfd_secret    447
#define LINUX_NR_process_mrelease 448
#define LINUX_NR_futex_waitv     449
#define LINUX_NR_set_mempolicy_home_node 450
#define LINUX_NR_cachestat       451
#define LINUX_NR_fchmodat2       452
#define LINUX_NR_map_shadow_stack 453
#define LINUX_NR_futex_wake      454
#define LINUX_NR_futex_wait      455
#define LINUX_NR_futex_requeue   456
#define LINUX_NR_io_uring_setup  425
#define LINUX_NR_io_uring_enter  426
#define LINUX_NR_io_uring_register 427
#define LINUX_NR_open_tree       428
#define LINUX_NR_move_mount      429
#define LINUX_NR_fsopen          430
#define LINUX_NR_fsconfig        431
#define LINUX_NR_fsmount         432
#define LINUX_NR_fspick          433
#define LINUX_NR_mount_setattr   442
#define LINUX_NR_quotactl_fd     443
#define LINUX_NR_pidfd_getfd     438
#define LINUX_NR_process_madvise 440
#define LINUX_NR_epoll_pwait2    441
#define LINUX_NR_fanotify_init   300
#define LINUX_NR_fanotify_mark   301
#define LINUX_NR_kcmp            312
#define LINUX_NR_finit_module    313
#define LINUX_NR_sched_setattr   314
#define LINUX_NR_sched_getattr   315
#define LINUX_NR_kexec_file_load 320
#define LINUX_NR_quotactl        179
#define LINUX_NR_nfsservctl      180
#define LINUX_NR_getpmsg         181
#define LINUX_NR_putpmsg         182
#define LINUX_NR_afs_syscall     183
#define LINUX_NR_tuxcall         184
#define LINUX_NR_security        185
#define LINUX_NR_lookup_dcookie  212
#define LINUX_NR_epoll_ctl_old   214
#define LINUX_NR_epoll_wait_old  215
#define LINUX_NR_remap_file_pages 216
#define LINUX_NR_restart_syscall 219
#define LINUX_NR_semtimedop      220
#define LINUX_NR_fadvise64       221
#define LINUX_NR_utimes          235
#define LINUX_NR_vserver         236  /* obsolete placeholder NR */
#define LINUX_NR_mbind           237
#define LINUX_NR_set_mempolicy   238
#define LINUX_NR_get_mempolicy   239
#define LINUX_NR_migrate_pages   256
#define LINUX_NR_move_pages      279
#define LINUX_NR_perf_event_open 298
#define LINUX_NR_rt_tgsigqueueinfo 297
#define LINUX_NR_timer_create    222
#define LINUX_NR_timer_settime   223
#define LINUX_NR_timer_gettime   224
#define LINUX_NR_timer_getoverrun 225
#define LINUX_NR_timer_delete    226
#define LINUX_NR_mq_open         240
#define LINUX_NR_mq_unlink       241
#define LINUX_NR_mq_timedsend    242
#define LINUX_NR_mq_timedreceive 243
#define LINUX_NR_mq_notify       244
#define LINUX_NR_mq_getsetattr   245
#define LINUX_NR_kexec_load      246
#define LINUX_NR_ioprio_set      251
#define LINUX_NR_ioprio_get      252
#define LINUX_NR_inotify_init    253
/* Module / legacy create (deepen; product usually ENOSYS) */
#define LINUX_NR_create_module   174
#define LINUX_NR_init_module     175
#define LINUX_NR_delete_module   176
#define LINUX_NR_get_kernel_syms 177
#define LINUX_NR_query_module    178

/*
 * Highest NR value currently catalogued above (inclusive). Soft inventory /
 * greps only — dispatch table size remains GJ_LINUX_NR_TABLE in
 * linux_dispatch.h (must be > this for set_hot/set_cold of max NR).
 * greppable: LINUX_NR_MAX_CATALOG
 */
#define LINUX_NR_MAX_CATALOG     456

/* clock_gettime clock ids (public clockid_t subset) */
#define LINUX_CLOCK_REALTIME           0
#define LINUX_CLOCK_MONOTONIC          1
#define LINUX_CLOCK_PROCESS_CPUTIME_ID 2
#define LINUX_CLOCK_THREAD_CPUTIME_ID  3
#define LINUX_CLOCK_MONOTONIC_RAW      4
#define LINUX_CLOCK_REALTIME_COARSE    5
#define LINUX_CLOCK_MONOTONIC_COARSE   6
#define LINUX_CLOCK_BOOTTIME           7
#define LINUX_CLOCK_REALTIME_ALARM     8
#define LINUX_CLOCK_BOOTTIME_ALARM     9
#define LINUX_CLOCK_TAI               11

/* mmap prot / flags (subset; x86_64 public values) */
#define LINUX_PROT_READ   0x1
#define LINUX_PROT_WRITE  0x2
#define LINUX_PROT_EXEC   0x4
#define LINUX_PROT_NONE   0x0
#define LINUX_PROT_GROWSDOWN 0x01000000
#define LINUX_PROT_GROWSUP   0x02000000

#define LINUX_MAP_SHARED    0x01
#define LINUX_MAP_PRIVATE   0x02
#define LINUX_MAP_FIXED     0x10
#define LINUX_MAP_ANONYMOUS 0x20
#define LINUX_MAP_GROWSDOWN 0x0100
#define LINUX_MAP_DENYWRITE 0x0800
#define LINUX_MAP_EXECUTABLE 0x1000
#define LINUX_MAP_LOCKED    0x2000
#define LINUX_MAP_NORESERVE 0x4000
#define LINUX_MAP_POPULATE  0x8000
#define LINUX_MAP_NONBLOCK  0x10000
#define LINUX_MAP_STACK     0x20000
#define LINUX_MAP_HUGETLB   0x40000
#define LINUX_MAP_SYNC      0x80000
#define LINUX_MAP_FIXED_NOREPLACE 0x100000
#define LINUX_MREMAP_MAYMOVE 1
#define LINUX_MREMAP_FIXED   2
#define LINUX_MREMAP_DONTUNMAP 4

/* open(2) / openat flags (asm-generic/fcntl.h shape; deepen for cold FS) */
#define LINUX_O_ACCMODE      0x3
#define LINUX_O_RDONLY       0x0
#define LINUX_O_WRONLY       0x1
#define LINUX_O_RDWR         0x2
#define LINUX_O_CREAT        0x40
#define LINUX_O_EXCL         0x80
#define LINUX_O_NOCTTY       0x100
#define LINUX_O_TRUNC        0x200
#define LINUX_O_APPEND       0x400
#define LINUX_O_NONBLOCK     0x800
#define LINUX_O_DSYNC        0x1000
#define LINUX_O_ASYNC        0x2000
#define LINUX_O_DIRECT       0x4000
#define LINUX_O_LARGEFILE    0x8000
#define LINUX_O_DIRECTORY    0x10000
#define LINUX_O_NOFOLLOW     0x20000
#define LINUX_O_NOATIME      0x40000
#define LINUX_O_CLOEXEC      0x80000
#define LINUX_O_SYNC         0x101000
#define LINUX_O_PATH         0x200000
#define LINUX_O_TMPFILE      0x410000

/* *at(2) dirfd / flag bits */
#define LINUX_AT_FDCWD            (-100)
#define LINUX_AT_SYMLINK_NOFOLLOW 0x100
#define LINUX_AT_EACCESS          0x200
#define LINUX_AT_REMOVEDIR        0x200
#define LINUX_AT_SYMLINK_FOLLOW   0x400
#define LINUX_AT_NO_AUTOMOUNT     0x800
#define LINUX_AT_EMPTY_PATH       0x1000
#define LINUX_AT_STATX_SYNC_TYPE  0x6000
#define LINUX_AT_STATX_SYNC_AS_STAT 0x0000
#define LINUX_AT_STATX_FORCE_SYNC 0x2000
#define LINUX_AT_STATX_DONT_SYNC  0x4000
#define LINUX_AT_RECURSIVE        0x8000

/* lseek whence */
#define LINUX_SEEK_SET  0
#define LINUX_SEEK_CUR  1
#define LINUX_SEEK_END  2
#define LINUX_SEEK_DATA 3
#define LINUX_SEEK_HOLE 4

/* wait4 / waitid option bits (subset) */
#define LINUX_WNOHANG    0x00000001
#define LINUX_WUNTRACED  0x00000002
#define LINUX_WSTOPPED   LINUX_WUNTRACED
#define LINUX_WEXITED    0x00000004
#define LINUX_WCONTINUED 0x00000008
#define LINUX_WNOWAIT    0x01000000
#define LINUX_WNOTHREAD  0x20000000
#define LINUX_WALL       0x40000000
#define LINUX_WCLONE     0x80000000

/* clone(2) flag subset (public sched.h values; soft product / PE32) */
#define LINUX_CSIGNAL              0x000000ff
#define LINUX_CLONE_VM             0x00000100
#define LINUX_CLONE_FS             0x00000200
#define LINUX_CLONE_FILES          0x00000400
#define LINUX_CLONE_SIGHAND        0x00000800
#define LINUX_CLONE_PIDFD          0x00001000
#define LINUX_CLONE_PTRACE         0x00002000
#define LINUX_CLONE_VFORK          0x00004000
#define LINUX_CLONE_PARENT         0x00008000
#define LINUX_CLONE_THREAD         0x00010000
#define LINUX_CLONE_NEWNS          0x00020000
#define LINUX_CLONE_SYSVSEM        0x00040000
#define LINUX_CLONE_SETTLS         0x00080000
#define LINUX_CLONE_PARENT_SETTID  0x00100000
#define LINUX_CLONE_CHILD_CLEARTID 0x00200000
#define LINUX_CLONE_DETACHED       0x00400000
#define LINUX_CLONE_UNTRACED       0x00800000
#define LINUX_CLONE_CHILD_SETTID   0x01000000
#define LINUX_CLONE_NEWCGROUP      0x02000000
#define LINUX_CLONE_NEWUTS         0x04000000
#define LINUX_CLONE_NEWIPC         0x08000000
#define LINUX_CLONE_NEWUSER        0x10000000
#define LINUX_CLONE_NEWPID         0x20000000
#define LINUX_CLONE_NEWNET         0x40000000
#define LINUX_CLONE_IO             0x80000000u

/* arch_prctl codes (x86_64 public) */
#define LINUX_ARCH_SET_GS 0x1001
#define LINUX_ARCH_SET_FS 0x1002
#define LINUX_ARCH_GET_FS 0x1003
#define LINUX_ARCH_GET_GS 0x1004

/* poll(2) event bits (subset; cold poll/ppoll) */
#define LINUX_POLLIN     0x0001
#define LINUX_POLLPRI    0x0002
#define LINUX_POLLOUT    0x0004
#define LINUX_POLLERR    0x0008
#define LINUX_POLLHUP    0x0010
#define LINUX_POLLNVAL   0x0020
#define LINUX_POLLRDNORM 0x0040
#define LINUX_POLLRDBAND 0x0080
#define LINUX_POLLWRNORM 0x0100
#define LINUX_POLLWRBAND 0x0200

/* epoll_ctl ops + event bits (subset) */
#define LINUX_EPOLL_CTL_ADD 1
#define LINUX_EPOLL_CTL_DEL 2
#define LINUX_EPOLL_CTL_MOD 3
#define LINUX_EPOLLIN      0x00000001u
#define LINUX_EPOLLPRI     0x00000002u
#define LINUX_EPOLLOUT     0x00000004u
#define LINUX_EPOLLERR     0x00000008u
#define LINUX_EPOLLHUP     0x00000010u
#define LINUX_EPOLLRDHUP   0x00002000u
#define LINUX_EPOLLET      0x80000000u
#define LINUX_EPOLLONESHOT 0x40000000u

/* common ioctl request numbers used by soft TTY/cold paths */
#define LINUX_TIOCGWINSZ  0x5413
#define LINUX_TIOCSWINSZ  0x5414
#define LINUX_FIONREAD    0x541B
#define LINUX_FIONBIO     0x5421
#define LINUX_FIONCLEX    0x5450
#define LINUX_FIOCLEX     0x5451

/*
 * Linux x86_64 calling convention at syscall entry:
 *   nr  = rax
 *   a0  = rdi, a1 = rsi, a2 = rdx, a3 = r10 (not rcx), a4 = r8, a5 = r9
 *   ret = rax  (negative = -errno)
 *
 * SYSCALL clobbers rcx/r11; arg3 is deliberately r10. Hybrid dispatcher
 * fills this POD from the trap frame / soft smoke without assuming C ABI
 * argument layout for the six slots.
 *
 * greppable: gj_linux_regs
 */
struct gj_linux_regs {
    u64 u64Nr;   /* LINUX_NR_* */
    u64 u64Arg0; /* rdi */
    u64 u64Arg1; /* rsi */
    u64 u64Arg2; /* rdx */
    u64 u64Arg3; /* r10 */
    u64 u64Arg4; /* r8 */
    u64 u64Arg5; /* r9 */
    i64 i64Ret;  /* set by dispatcher: ≥0 success or -LINUX_E* */
};

/**
 * Linux timespec (x86_64 LP64 public layout): seconds + nanoseconds.
 * Used by clock_*, nanosleep, futex timeout soft shapes, utimensat.
 */
struct linux_timespec {
    i64 i64Sec;
    i64 i64Nsec;
};

/**
 * timeval for gettimeofday / settimeofday soft edges (public layout).
 */
struct linux_timeval {
    i64 i64Sec;
    i64 i64Usec;
};

/* Path classification for hybrid Option C (docs/LINUX_ABI_HYBRID.md) */
enum gj_linux_path {
    GJ_LINUX_PATH_HOT  = 0, /* handled entirely in kernel (linux_hot) */
    GJ_LINUX_PATH_COLD = 1, /* upcall / personality (doors / protonrt) */
    GJ_LINUX_PATH_NONE = 2, /* unknown / unregistered → -ENOSYS */
};

/* ---- Header-only soft helpers (no .c) ----------------------------------- */

/**
 * Encode a positive LINUX_E* as the negative i64 returned in rax / i64Ret.
 * nErrno ≤ 0 is returned unchanged (already negative or zero).
 * greppable: gj_linux_neg_errno
 */
static inline i64
gj_linux_neg_errno(int nErrno)
{
    if (nErrno <= 0) {
        return (i64)nErrno;
    }
    return -(i64)nErrno;
}

/**
 * Non-zero if u64Nr is within the usual sparse product table span (512).
 * Matches GJ_LINUX_NR_TABLE in linux_dispatch.h without including it.
 * greppable: gj_linux_nr_in_table_span
 */
static inline int
gj_linux_nr_in_table_span(u64 u64Nr)
{
    return (u64Nr < 512ull) ? 1 : 0;
}

/**
 * Non-zero if u64Nr is at most LINUX_NR_MAX_CATALOG (this header's catalog).
 * Does not imply the NR is registered HOT/COLD — only that it is named here
 * or falls under the catalog high-water mark.
 */
static inline int
gj_linux_nr_in_catalog(u64 u64Nr)
{
    return (u64Nr <= (u64)LINUX_NR_MAX_CATALOG) ? 1 : 0;
}

/**
 * Soft: zero a regs block and set NR + six args (ret left 0).
 * NULL pRegs → no-op. Used by soft smokes / PE32 int80 staging.
 */
static inline void
gj_linux_regs_set(struct gj_linux_regs *pRegs, u64 u64Nr,
                  u64 a0, u64 a1, u64 a2, u64 a3, u64 a4, u64 a5)
{
    if (pRegs == 0) {
        return;
    }
    pRegs->u64Nr   = u64Nr;
    pRegs->u64Arg0 = a0;
    pRegs->u64Arg1 = a1;
    pRegs->u64Arg2 = a2;
    pRegs->u64Arg3 = a3;
    pRegs->u64Arg4 = a4;
    pRegs->u64Arg5 = a5;
    pRegs->i64Ret  = 0;
}

/**
 * Soft: true if i64Ret is a Linux-style -errno failure (negative).
 */
static inline int
gj_linux_ret_is_err(i64 i64Ret)
{
    return (i64Ret < 0) ? 1 : 0;
}

/**
 * Soft: extract positive errno from a negative i64Ret; 0 if non-negative.
 */
static inline int
gj_linux_ret_errno(i64 i64Ret)
{
    if (i64Ret >= 0) {
        return 0;
    }
    return (int)(-i64Ret);
}
