# GreenJade — ABI-first wave status (soft inventory)

| Field | Value |
|-------|--------|
| **As of** | 2026-08-04 lab panel/serial: `PROBE SOFT` · `NET R0 B132` · hybrid ON · `L2 BR 0` · USB MSC **NOT READY** · xHCI **SKIP BUILTIN**; **parallel goals** (TODO Current track): net FS ping · USB FS MSC · NIC hybrid · USB module · DDI/UDX; Soft ≠ product · **G-AC-1** |
| **Law** | Dual **MIT OR Apache-2.0** only; **no GPL** in tree; **G-AC-1** — **not** Linux `.ko`; **Linux-SHAPED UDX** only |
| **Normative pivot** | [ABI_FIRST_PIVOT.md](ABI_FIRST_PIVOT.md) |
| **Coding backlog** | [TODO.md](TODO.md) § *Current track* |
| **Hybrid design** | [LINUX_ABI_HYBRID.md](LINUX_ABI_HYBRID.md) |
| **Soft DDI** | [DDI_SOFT.md](DDI_SOFT.md) |
| **HCL** | [HCL.md](HCL.md) |
| **Bar3 / Top 50** | [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) **only** (do not restate in boot kprintf) |
| **Laptop UDX host runbook** | [LAPTOP_LINUX_DRIVER_HOST.md](LAPTOP_LINUX_DRIVER_HOST.md) — G752VT wave D host path; **G-AC-1** |
| **Module path** | [LINUX_MODULE_PATH.md](LINUX_MODULE_PATH.md) — soft loader / staged `.ko`; **not** product AC |
| **pci_dev plan** | [PCI_DEV_SOFT_LAYOUT.md](PCI_DEV_SOFT_LAYOUT.md) — hostish **0xb40** Strategy A (real probe soft PASS) |

**Honesty rule:** greppable soft PASS / scaffold / soft bridge ≠ product DoD. Bar3: [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md). T0 product net = **virtio**. Freestanding `xhci_msc` / `rtl8168` = **lab soft only**. Soft `NETDEV SOFT ≥1` ≠ TX/RX. Soft L2 bridge ON ≠ freestanding MMIO handoff. Real probe `ST=0` ≠ product datapath. **G-AC-1:** staged / soft-loaded Linux `.ko` ≠ product claim. **Soft ≠ product.**

---

## Wave note — 2026-08-04 (parallel goals; lab panel/serial)

**Parallel (TODO Current track):** (1) net FS ping **blocking** (2) USB FS MSC (3) NIC hybrid (4) USB module (5) DDI/UDX. Soft ≠ product · **G-AC-1** · dual **MIT OR Apache-2.0**.

### Current lamps

| Lamp | Value |
|------|--------|
| **PROBE** | `PROBE SOFT` |
| **NET** | `R0 B132` (not live RX path) |
| **hybrid** | ON |
| **hold14** | `L2 BR 0` |
| **USB MSC** | **NOT READY** |
| **xHCI** | **SKIP BUILTIN** |

### Prior proven (hybrid 4a eng)

| Lamp | Value |
|------|--------|
| **NETDEV** | `NETDEV SOFT 1` |
| **PROBE** | `PROBE 10EC:8168 REAL` · `REG=1 MATCH=1 ST=0` |
| **ksym** | **N=295** |
| **MOD** | `MOD R8169 INIT=0` |
| **hold14** | **`L2 BR RX=2 TX=0`** (live; RX≡`R2`) |
| **hold15** | **`HYBRID WIRE=FS SOFT=R8169`** |
| **NET** | `10.200.125.50 UP T4/F0/B924/R2` |
| **xHCI** | `MOD XHCI PCI SKIP BUILTIN` |

| Item | Fact |
|------|------|
| **Proven HW** | **Hybrid 4a eng PASS** — Linux soft r8169 + freestanding wire + L2 RX live (`HYBRID WIRE=FS SOFT=R8169` · `L2 BR RX=2`) |
| **Current lab** | SOFT probe · L2 BR 0 · MSC NOT READY; ping blocking |
| **Prior** | REAL+SOFT1 stable (ksym was **N=289**) |
| **Not proven** | Full `.ko` wire (4b); product AC; USB MSC stick; freestanding ping this boot |
| **Next** | Five parallel goals — not single-threaded 4b-only |
| **Claim** | **Soft ≠ product.** Hybrid eng ≠ product / bar3. **G-AC-1.** |

### Prior proven + regression (same week)

| Lab milestone | Note |
|---------------|------|
| **REAL+SOFT1 stable** | ksym N=289; Soft L2 ON |
| **First REAL probe** | `PROBE … REAL` `ST=0` `NETDEV SOFT 1` |
| **Soft EMU** | `REG=1` `MATCH=1` `NETDEV SOFT 1` `PROBE … SOFT` |
| **First soft NETDEV SOFT 1** | id_table 40 B holds |
| **force-EMU netdev-only fix** | `NETDEV SOFT 0` / `MISS` — bound without netdev |
| **id_table stride fix** | `NETDEV SOFT 0` from 32 B vs 40 B `pci_device_id` stride (**fixed**) |

Also: boot/runtime **bar3 honesty spam** stripped from kprintf → [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) only.

---

## 1. Disposition matrix

| Surface | In tree? | Class | Product claim? |
|---------|----------|-------|----------------|
| Option C hot/cold dispatch | Yes | Design + kernel path | Soft surface ≠ Steam/bar3 |
| `linux_cold_net` socket family | Yes | **Soft bridge** | Partial — STREAM→`net_tcp`, else `net_lo` |
| `net_tcp_poll_mask` + vfs net route | Yes | **Soft/partial** readiness | Not full blocking daemon product |
| `memobj_map_file_fd` | Yes | **Soft** file mmap snapshot | No live pager |
| `process_linux_fork` / soft wait | Yes | **Soft** fork-wait | Not full COW/session product |
| `personality_gj` userspace scaffold | Yes | **Soft scaffold** | Not cold-door default |
| `devmgr_soft` + `gj/devmgr.h` | Yes | **Soft inventory + real PCI scan** | Soft table; **no** live CNode cap mint |
| **`GJ_SYS_DDI` (103) / `ddi_door`** | Yes | **Wave D live soft door** | SCAN/GET/OPEN/MAP_BAR soft; product grants **OPEN** |
| `vmm_map_user_device` | Yes | **Soft user MMIO map** | Used by MAP_BAR; not full MMIO_FRAME cap story |
| UDX host bind (SCAN/GET/OPEN/MAP_BAR) | Yes | **Wave D soft bind** | Matches kernel ops; product mint **OPEN** |
| `rtl8168_udx` clean-room skeleton | Yes | **Soft class host** | **No** product TX/RX |
| `xhci_udx` clean-room skeleton | Yes | **Soft class host** | **No** product BOT/MSC |
| virtio-net / virtio-blk T0 | Yes | Product T0 path | CI/smoke gate |
| In-kernel `xhci_msc` | Yes | Lab soft scaffold | **Never** T1 from stage count |
| In-kernel `rtl8168` | Yes | Lab soft only | **Never** T0 product net |
| Linux `.ko` / GPL drivers | **Forbidden** as product AC | **G-AC-1** | **Never** bar3-by-`.ko` — UDX shape for product |
| Soft module path (r8169) | Yes | **Soft eng track** | REAL+SOFT1 stable; waves 1–3 code land; freestanding owns MMIO at gate0; **D7 OPEN** |
| Soft firmware ksym | Yes | **Soft eng** | `-ENOENT` + null fw; **≠** real `.fw` load |
| Hostish `pci_dev` **0xb40** | Soft PASS probe | **REAL+SOFT1 stable** | next = gate1 sole-owner → Option B / phase4 |
| D4 media boot | Yes | **PARTIAL** | ESP+UEFI `source=media`; embed fallback; await lab panel/serial for `source=media` |

---

## 2. Scaffold inventory (paths)

### 2.1 Socket-family cold bridge

| Item | Path |
|------|------|
| Implementation | `/home/jay/Documents/knano/kernel/syscall/linux_cold_net.c` |
| Header | `/home/jay/Documents/knano/kernel/include/gj/linux_cold_net.h` |
| Cold wire | `protonrt_cold_link.c` → `gj_linux_cold_{socket,bind,listen,accept,connect,sendto,recvfrom,shutdown,getsockname}` |
| Makefile | `kernel/syscall/linux_cold_net.c` linked |
| Markers | `linux_cold_net: soft …` · `linux_cold_net: soft inventory …` |

| Op | Soft behavior |
|----|----------------|
| `socket` | AF_INET + SOCK_STREAM → `net_tcp_socket`; else `net_lo_socket` |
| `bind`/`listen`/`accept`/`connect` | Per-fd table (`net_tcp_fd_ok` / `net_lo_fd_ok`) |
| `sendto`/`recvfrom` | Bounce ≤4 KiB; multi-seg TCP path where present |
| `shutdown` | lo real; TCP soft 0 (no half-close API yet) |
| `getsockname` | lo real; TCP soft fabricated sockaddr when no name API |

### 2.2 Poll readiness net route

| Item | Path |
|------|------|
| TCP mask | `kernel/net/net_tcp.c` — `net_tcp_poll_mask` |
| Ram/unified | `kernel/fs/vfs_ram.c` — `vfs_ram_poll_mask` routes non-ram FD to `net_tcp` / `net_lo` |
| Consumers | cold `poll`/`ppoll`/`epoll_*` in `protonrt_cold_link.c`; io_uring min soft; PE32 traps |

**Soft ≠ product:** readiness bits exist; full blocking multi-fd daemon loops with production timeouts remain partial.

### 2.3 File-backed mmap soft path

| Item | Path |
|------|------|
| Core | `kernel/mm/memobj.c` — `memobj_map_file_fd` |
| Header | `kernel/include/gj/memobj.h` (FILE memobj; soft pager note) |
| Hot call | `kernel/syscall/linux_hot.c` — mmap file branch → map_file_fd; else `linux_hot: mmap file soft ENOSYS` |

**Soft:** vfs_ram **regular-file snapshot** into FILE memobj; no live fd pager; non-ramfs / specials fail closed to ENOSYS.

### 2.4 Process spawn / fork-wait soft

| Item | Path |
|------|------|
| Soft catalog | `kernel/proc/process.c` — `process: soft fork` · `process: soft wait` · inventory lamps |
| Fork/clone | `process_linux_fork` / clone flag map (soft AS shell; not full runnable COW child) |
| Wait | `process_wait4` / `process_wait4_ppid` + spawn wait poll (`kernel/proc/spawn.c`) |
| Cold NRs | `clone` / `clone3` / `wait4` via `protonrt_cold_link.c` |
| Smokes | PE32 `vfork_wait` / `fork_wait`; init soft fork/wait4 |

### 2.5 Userspace cold personality scaffold (G-PERS)

| Item | Path |
|------|------|
| Source | `user/personality/personality_gj.c` |
| Story | `user/personality/README.md` |
| Build | Makefile → `build/user/personality.elf` (`make personality-gj` when wired) |
| Soft marker | `personality: soft userspace scaffold PASS` |
| Door loop | Compiled out by default (`PERSONALITY_DOOR_LOOP=0`) — avoids smoke hang |

**Default cold path remains kernel `cold_personality_server` kthread.** Product = this (or equivalent) NATIVE process owns the cold door.

### 2.6 Soft DDI / devmgr / door (**wave D**)

| Item | Path |
|------|------|
| Header (devmgr) | `kernel/include/gj/devmgr.h` |
| Soft impl | `kernel/drv/devmgr_soft.c` — **real type-0 CF8/CFC PCI walk** |
| DDI door | `kernel/drv/ddi_door.c` · `kernel/include/gj/ddi_door.h` |
| Syscall NR | **`GJ_SYS_DDI` = 103** (`kernel/include/gj/syscall.h`; UDX `user/udx/include/udx/ddi.h`) |
| Dispatch | `kernel/syscall/native.c` → `ddi_door_syscall` |
| User MMIO | `kernel/mm/vmm.c` — `vmm_map_user_device` (MAP_BAR preferred path) |
| Doc | [DDI_SOFT.md](DDI_SOFT.md) · [LAPTOP_LINUX_DRIVER_HOST.md](LAPTOP_LINUX_DRIVER_HOST.md) |
| Markers | `devmgr: soft init PASS` · `devmgr: soft pci scan PASS n=N` · `devmgr: soft found 10ec:8168` / `8086:a12f` · `ddi_door: soft product surface PASS` · `ddi_door: soft scan count=` · `main: soft ddi laptop smoke PASS` |

| Door op (arg0) | Soft behavior |
|----------------|---------------|
| **SCAN** (1) | `devmgr_soft_pci_scan` → device count |
| **GET** (2) | Fill `gj_ddi_dev_info` (BDF, vend/dev, BAR0..5 pa/cb) |
| **OPEN** (3) | Soft handle id (no live cap mint) |
| **MAP_BAR** (4) | Prefer `vmm_map_user_device`; else kernel UC `vmm_map_device_uc` |
| **CFG_READ** (5) | Config dword via CF8/CFC |
| **DMA_NOTE** (6) | Soft window note / `iommu_window_grant` if available |
| **INVENTORY** (7) | Once: `ddi_door: soft product surface PASS` |
| **CFG_WRITE** (16) | Careful soft note; reject unsafe identity/BAR writes |

**Soft ≠ product:** soft open/map/cfg/dma **notes** and user-AS UC maps when process exists. **Not** full match graph, **not** MMIO_FRAME / IRQ Notification / DMA window **cap mint** into host CNode product story.

### 2.7 UDX host bind + clean-room class skeletons (**wave D**)

| Item | Path |
|------|------|
| UDX runtime | `user/udx/` — libudx; Linux-shaped probe/remove/ioremap/irq/dma |
| Bind | `user/udx/src/host.c` — `udx_host_bind_scan` / `udx_host_bind_by_id` |
| DDI opcodes (UDX) | `user/udx/include/udx/ddi.h` — **must match** kernel `DDI_OP_*` |
| Bind sequence | **SCAN → GET → OPEN → MAP_BAR** (same as kernel door) |
| Bind lamps | `udx: soft ddi bind PASS` · `udx: soft ddi bind SKIP` |
| xHCI soft BOT progress | `xhci_udx: soft bot stage=1..4` · `soft bot stub PASS` · `soft ports` / `soft params` — **≠ product BOT** |
| rtl8168 skeleton | `user/drivers/rtl8168_udx/` — soft probe/ISR/work; **TX/RX OPEN** |
| xhci skeleton | `user/drivers/xhci_udx/` — soft CAPLENGTH/HCIVERSION; **BOT OPEN** |
| Stage helper | `scripts/stage-udx-drivers.sh` — ESP `drivers/` only; **no `.ko`** |

**G-AC-1:** these are **Linux-SHAPED UDX** userspace hosts (MIT OR Apache-2.0 clean-room). They are **not** Linux kernel modules and must never be treated as product AC via `.ko`.

**Still OPEN on class hosts:** product NIC **TX/RX**, product xHCI **BOT/MSC**, **live IRQ** (kernel notify product). Soft `fire_irq` / inject paths are lab only.

---

## 3. Law and freestanding HW disposition

| Rule | Status |
|------|--------|
| Dual MIT OR Apache-2.0 | Project license |
| No GPL source in GreenJade tree | Enforced by policy |
| No Linux `.ko` product acceptance criterion | **G-AC-1** — forbidden; product = **Linux-SHAPED UDX** |
| T0 net product | **virtio-net** only |
| `xhci_msc` / freestanding USB MSC | Soft/lab scaffold |
| `rtl8168` in-kernel | Soft/lab only — not product NIC |
| `rtl8168_udx` / `xhci_udx` | Clean-room **soft skeletons** — not product TX/RX/BOT |
| Product TX/RX · BOT · live IRQ | **OPEN** |
| Linux inventory media PASS | Operator DUT map only ≠ freestanding T1 |

---

## 4. Recommended next items (honest)

Order matches [ABI_FIRST_PIVOT.md](ABI_FIRST_PIVOT.md). Soft scaffolds **already present** are marked — do not re-scaffold.

| # | Item | Current | Next product step |
|---|------|---------|-------------------|
| 1 | Userspace cold personality default | Soft scaffold ELF | Boot spawn + door ownership; live serve markers |
| 2 | Blocking poll/epoll product | Soft readiness + net route | Daemon-grade timeouts / multi-fd correctness |
| 3 | fork/clone + exec + wait session | Soft fork-wait stubs | Runnable child + execve product + reaper |
| 4 | File-backed mmap product | Soft ramfs snapshot | Live pager / vfs-door file maps |
| 5 | DDI product caps + class I/O | Wave D soft door + user MMIO + skeletons | Mint MMIO/IRQ/DMA **caps**; product TX/RX; BOT; live IRQ |

**Module-path engineering (parallel, not product AC):**

| # | Item | Current | Next step |
|---|------|---------|-----------|
| M1 | Soft netdev bind | **`NETDEV SOFT 1`** | Soft done |
| M2 | Hostish real probe | **PASS** REAL+SOFT1 stable | Soft done; soft ≠ product |
| M3 | **D7** MMIO handoff / datapath | phase0 live; 1–3 stubs gate0; reverse TX soft; freestanding owns MMIO | **Next:** gate1 sole-owner · Option B ndo_open · phase4 wire |
| M4 | Soft firmware | `-ENOENT` null fw | Real `.fw` load |
| M5 | D4 media | **PARTIAL** `source=media` + embed | await lab panel/serial; GJ-PERSIST ext4 still OPEN |
| M6 | xHCI `.ko` | **SKIP builtin** | Optional; not blocking NIC |

**Already soft-landed (deepen, don’t invent):**

- Socket family cold bridge (`linux_cold_net`)  
- `net_tcp_poll_mask` / vfs net poll route  
- `memobj_map_file_fd` soft path  
- process soft fork/wait catalog  
- `devmgr_soft` **real PCI scan** + G752 found lamps  
- **`GJ_SYS_DDI=103` / `ddi_door`** SCAN/GET/OPEN/MAP_BAR  
- **`vmm_map_user_device`** for MAP_BAR user-AS UC  
- UDX **bind** matching kernel ops  
- Clean-room **`rtl8168_udx`** / **`xhci_udx`** skeletons  

---

## 5. Top remaining gaps

### 5.1 Top 5 (ABI product)

1. **G-PERS product default** — cold personality still a **kernel thread**; `personality_gj` is scaffold-only (no live door ownership).  
2. **Blocking I/O multiplexing product** — poll/epoll readiness is soft-routed (incl. TCP); full multi-fd blocking/timeout product for real daemons is incomplete.  
3. **Process model product** — soft fork returns wait-table pids; not a full private-AS runnable child + product `execve` session.  
4. **File mmap product** — snapshot-only from vfs_ram; no live pager; many fds still ENOSYS.  
5. **DDI product grants + class datapath** — wave D soft door/maps exist; still **no** live MMIO_FRAME / IRQ Notification / DMA window **caps** for hosts; **product TX/RX**, xHCI **BOT**, and **live IRQ** remain **OPEN**.

### 5.2 Wave D short DoD vs remaining

```text
WAVE D + SOFT BIND DONE              STILL OPEN
──────────────────────────────       ────────────────────────────────
GJ_SYS_DDI=103 (ddi_door.c)          Product TX/RX (rtl8168_udx / .ko)
Real soft PCI scan (devmgr_soft)     Product xHCI BOT/MSC (xhci_udx)
UDX SCAN/GET/OPEN/MAP_BAR bind       Live IRQ / kernel notify product
vmm_map_user_device (user MMIO)      Full CNode MMIO/IRQ/DMA cap mint
Clean-room rtl8168_udx + xhci_udx    Multi-server devmgr match graph
r8169 NETDEV SOFT 1                  Net/USB datapath (TX/RX · BOT)
PROBE 10ec:8168 REAL ST=0            **D7** net datapath / MMIO handoff
REG=1 MATCH=1 (prior EMU)            Freestanding still owns MMIO
G-AC-1: UDX shape, not .ko product   bar3; G-PERS product default
Soft laptop smoke lamps              Product sshd on laptop NIC
```

*Secondary (explicit non-gaps for thrash): freestanding xHCI stage count and rtl8168 OWN recovery are **not** product bars under this pivot.*

---

## 6. Related docs

| Doc | Role |
|-----|------|
| [ABI_FIRST_PIVOT.md](ABI_FIRST_PIVOT.md) | Strategy + gap snapshot + next five |
| [DDI_SOFT.md](DDI_SOFT.md) | Soft devmgr API honesty |
| [HCL.md](HCL.md) | T0 virtio / T1+ real HW; freestanding soft disposition |
| [LAPTOP_LINUX_DRIVER_HOST.md](LAPTOP_LINUX_DRIVER_HOST.md) | G752VT operator runbook; wave D DoD vs OPEN bars |
| [LINUX_MODULE_PATH.md](LINUX_MODULE_PATH.md) | Collect → stage → soft bind; NETDEV SOFT 1 wave |
| [PCI_DEV_SOFT_LAYOUT.md](PCI_DEV_SOFT_LAYOUT.md) | Hostish `pci_dev` 0xb40 Strategy A plan |
| [LINUX_ABI_HYBRID.md](LINUX_ABI_HYBRID.md) | Option C architecture |
| [PROTON_PERSONALITY.md](PROTON_PERSONALITY.md) | Personality / Proton targeting |
| [STEAM_BAR3_STATUS.md](STEAM_BAR3_STATUS.md) | bar3 OPEN |
| [UDX_LINUX_PORTER.md](UDX_LINUX_PORTER.md) | Porter contract for Linux-shaped UDX |

*Soft confine ≠ product multi-server; soft bridge ≠ full glibc; soft scaffold ≠ G-PERS closed; soft probe ≠ TX/RX/BOT; **G-AC-1** ≠ Linux `.ko` product AC. Dual MIT OR Apache-2.0 tree only.*
