# GreenJade — Hybrid Linux ABI (Option C)

| Field | Value |
|-------|--------|
| **Document** | Linux ABI hybrid **v1.4** |
| **Status** | **Accepted** — per-CPU swapgs, scheduler sleep, doors cold path |
| **Production freezes** | [DESIGN_SPEC_COMPLETE.md](DESIGN_SPEC_COMPLETE.md) §§1–4 |
| **Model** | **Option C:** kernel hot paths + **doors** cold personality thread |
| **License** | Clean-room; **no GPL source** |
| **Companion** | [PROTON_PERSONALITY.md](PROTON_PERSONALITY.md) · [CAP_ADDRESSING.md](CAP_ADDRESSING.md) |

---

## 1. Architecture

```text
  ring-3 Linux task
       │ SYSCALL
       ▼
  swapgs → per-CPU kernel stack (struct gj_cpu @ GS)
       │
       ▼
  gj_linux_syscall_dispatch
       ├─ HOT  → linux_hot (+ user_copy, CapJit, PA futex)
       ├─ COLD → door_call(cold_personality)
       │              │
       │              ▼
       │         personality kernel thread
       │              door_recv → protonrt_cold_linux → door_reply
       └─ NONE → -ENOSYS
```

### Per-CPU / multi-process

| Piece | Status |
|-------|--------|
| `struct gj_cpu` + GS / KERNEL_GS | **Done** (`cpu_init_bsp`) |
| SYSCALL `swapgs` + `u64KernelRsp` | **Done** (`syscall_entry.S`) |
| `process.u64Cr3` + `cpu_load_cr3` on schedule | **Done** (shared boot CR3 until per-AS clone) |
| Multi-CPU AP online | Later (MADT) |

### Scheduler

| Piece | Status |
|-------|--------|
| Kernel threads, `switch_context` | **Done** |
| `thread_block` / `thread_wake` / `schedule` | **Done** |
| Futex sleep via block (not spin) | **Done** when `thread_current()` exists |
| Cold doors sleep | **Done** (`door_call` / `door_recv`) |
| Preemption / timer tick | Later |

### Cold path = doors process (kernel thread stand-in)

Until userland personality process exists, **`cold_personality_server`** is a **kernel thread** using the same doors API a userspace server will later use over IPC.

---

## 2. Hot vs cold (summary)

Unchanged hot set (write, mmap/vmm, mprotect+CapJit, futex PA-share, …).  
Cold: openat/read/… via **door_call** → personality thread → libprotonrt.

---

## 3. Code map (new)

| Path | Role |
|------|------|
| `kernel/cpu/cpu.c` | per-CPU, GS, CR3 |
| `kernel/sched/thread.c` | threads + schedule |
| `kernel/arch/x86_64/switch.S` | context switch |
| `kernel/ipc/door.c` | doors rendezvous |
| `kernel/syscall/cold_ipc.c` | submit → door_call |

---

## 4. Production requirements (from design complete)

| Item | Spec |
|------|------|
| Untrusted ELF | **After** IDT + supervisor-only kernel maps |
| AS isolation | Per-process CR3 clone (kernel half shared) |
| Personality | Migrate kernel thread → **userspace** door server |
| Cold API | **Doors only** (dequeue syscalls deprecated) |
| CapJit | Cap/right, not permanent boolean authority |
| Pointers | `copy_*_user` only; no kernel bypass in product |
| Idle | **HLT** when idle; timer for preemption + futex timeout |

Implementation remaining: code those freezes — design is closed.

---

*v1.4 — hybrid + production freezes linked.*
