# Steam on hardware-test media (options 2 + 3)

**Bar3 status (product ceiling):** media path + host prep are **wired**.  
**Kernel/surface done does not equal bar3 client run.** Continuum **makefile_max=14900** is soft graph growth only.  
Parallel waves (soft ship only): continuum decades, **io_uring** min rings + mmap/SQE soft smoke, **768G** hierarchical soak, aarch64 M0 scaffold (+ shared C / PSCI smokes), HDA multi-stream kernel, live sshd/scsi_mid — **none** close bar3.  
Kernel large-RAM **768G soak** (`pmm` / hierarchical freelist path) is **shipped PASS** — still not a Steam client claim.  
**Open:** real-DUT UEFI install → launch Steam client → Deck Top 50 matrix  
rows leave `NOT-TRIED` (see `matrix/deck-top50-2026-07-19.md`).

**Policy:** put a **prebuilt Steam tree** on media.  
GreenJade does **not** run `dpkg`, apt, or a Debian userland.

Valve’s Steam client is **proprietary**. Binaries are **not** committed to this repository.

| Option | Meaning | GreenJade support |
|--------|---------|-------------------|
| **1** | Runtime/tarball-only install story | Not the primary path |
| **2** | Prebuilt tree **packed into** hwtest media at image build | **Yes** — `make steam-fetch` / `steam-stage` / `hwtest-img` |
| **3** | **Host-side prep** — fetch/extract on lab host, **copy onto** existing image/USB/mount | **Yes** — `make steam-host-prep` / `scripts/steam-host-prep.sh` |
| `.deb` on DUT | `dpkg -i` inside GreenJade | **No** |

Options **2 and 3 share the same tree** (`build/steam-tree/`).  
2 bakes it in when building the image; 3 pushes it later onto a stick you already wrote.

## Bar3 checklist (what works vs open)

Host soft check (no download, always exit 0):

```sh
./scripts/steam-bar3-check.sh
# → READY | SKELETON | MISSING
```

### Works today (media + kernel surface)

| Item | Status | Evidence / how to verify |
|------|--------|---------------------------|
| Host fetch bootstrap (`.deb` extract only) | **wired** | `make steam-fetch` → `build/steam-tree/` + `READY` |
| Stage tree with `STATUS` | **wired** | `make steam-stage` → `build/steam-stage/steam/STATUS` = `READY` or `SKELETON` |
| Pack into hwtest image (option 2) | **wired** | `make hwtest-img` → p2 `GJ-PERSIST/steam/` |
| Host prep onto stick/image (option 3) | **wired** | `scripts/steam-host-prep.sh --to-label` / `--to-img` / `--to-mount` |
| Offline / skip re-download | **wired** | `GJ_SKIP_FETCH=1` or `make steam-stage` alone → skeleton if no tree |
| Product **sshd** live at boot | **done** | `sshd.elf` embed; TCP **:22**; `sshd: live spawn PASS` |
| **scsi_mid** live embed | **done** | `scsi_mid.elf` boot spawn; `GJ_SYS_SCSI` + virtio-scsi path |
| **HDA multi-stream (kernel)** | **PASS** | SD0 DMA + multi-stream mixer; `hda: multi-stream mixer PASS` — **kernel only** |
| Graph continuum **14900** | **soft** | `makefile_max=14900` CREATE-ONLY soft gates shipped; **not** bar3 client run |
| **io_uring** min rings + mmap/SQE soft | **PASS** | Kernel smokes (`linux: io_uring min rings PASS`, `mmap PASS`, `SQE I/O PASS`); vfs_ram / soft surface — **not** game I/O |
| Kernel large-RAM **768G soak** | **PASS** | Hierarchical freelist + `soak_tib` at 768G; **not** full 1 TiB host path; **not** Steam |
| **aarch64 M0 scaffold** | **smoke PASS** | `make aarch64` / `aarch64-smoke`; shared C + PSCI smokes; separate arch tree — **not** Deck/x86 path |
| GPU/session A1 (virtio-gpu 2D + ICD) | **partial** | Compositor/ICD smokes green on QEMU; not SteamUI |
| libcgj / ld-gj growth | **in progress** | Clean-room glibc-shaped surface expanding; not full Steam Runtime |

### Continuum 14900 (soft — honest scope)

| Claim | Status | Honesty bound |
|-------|--------|---------------|
| Decades through **14900** parent wire / soft gates | **shipped soft** | Graph/product soft stubs + milestone gates only (parallel-wave growth) |
| `makefile_max=14900` on tree | **yes** | CREATE-ONLY continuum batches through `graph_batch14900.c` |
| Implies Steam client / Top-50 | **no** | Soft continuum ≠ runtime ABI ≠ DUT launch |
| Closes bar3 | **no** | Bar3 remains client-run + matrix open |

Continuum growth is **clean-room surface expansion**. Treat 14900 as a graph milestone, not a Steam readiness claim.

### Kernel large-RAM 768G soak (shipped — honest scope)

| Claim | Status | Honesty bound |
|-------|--------|---------------|
| Hierarchical freelist PMM + 768G soak | **PASS** | Large-RAM path exercised; product bar still **≥ 1 TiB** when host allows |
| Full 1 TiB host soak (`-m 1280G` class) | **open** | Soft-SKIP / not claimed until host allows |
| Implies Steam / bar3 client | **no** | Kernel memory soak ≠ client launch ≠ Top-50 |

**Do not** promote “768G soak PASS” to “desktop 1 TiB bar met” or “Steam ready.”

### HDA multi-stream: kernel PASS vs client run (still open)

| Layer | Status | What it means |
|-------|--------|---------------|
| Kernel multi-stream mixer / SD0 DMA | **PASS** | Smoke: `hda: multi-stream mixer PASS` |
| CORB/BDL / HW CORB / `GJ_SYS_HDA_STREAM` | **kernel done** | Open/write/start/tick/stats path exists for freestanding clients |
| Freestanding `hda_client` smoke | **kernel-side exercise** | Not Steam, not PipeWire, not game audio |
| Userspace audio stack to Steam / games | **open** | No Steam-ready PCM pipe / mixer policy for client |
| Audio in Steam client / Deck titles | **open** | Blocked on bar3 client run + full audio path |

**Do not** promote “HDA multi-stream PASS” to “Steam audio works.” Kernel PASS is real; bar3 audio remains open until a client/title actually plays sound on GreenJade.

### Open (bar3 client run — do not claim done)

| Item | Status | Notes |
|------|--------|-------|
| Real-DUT UEFI install + boot to shell/sshd | **open** | Media path exists; operator DUT still required |
| Launch Steam client on GreenJade | **open** | Bootstrap on media ≠ client runs |
| Steam Runtime first-run download | **open** | Needs working net + ABI + FS write |
| GPU present path for SteamUI / games | **open** | Beyond virtio-gpu 2D ICD smoke |
| Audio (HDA) to Steam / games | **open** | Kernel multi-stream **PASS**; userspace → Steam still open |
| Continuum 14900 → product Steam path | **open** | Soft gates only; does not unblock client |
| io_uring soft PASS → game/client I/O | **open** | Kernel vfs_ram SQE smoke ≠ Steam Runtime / title I/O |
| Kernel 768G soak → Steam path | **open** | Memory soak PASS ≠ ABI / GPU / client |
| Full 1 TiB host soak | **open** | Product RAM bar; separate from bar3 client |
| Input (gamepad / desktop) for client | **open** | virtio-input hub exists; Steam mapping not validated |
| **Deck Top 50** title rows | **NOT-TRIED** | Matrix claim level: **targeting only** — zero PASS titles |

**Hard rule:** never mark Top-50 rows `PASS` from host stage/`STATUS=READY`, continuum soft gates, kernel HDA PASS, io_uring soft PASS, or 768G PMM soak alone.  
`READY` means the **bootstrap tree is on media**, not that a game ran.

### Operator path when bar3 unblocks

1. Build/write hwtest USB (`make hwtest-img` + `install-hwtest-usb`).  
2. Confirm `GJ-PERSIST/steam/STATUS` = `READY` (or run option 3 host prep).  
3. Boot DUT UEFI → serial/`sshd` reachability.  
4. Attempt launcher: `/opt/steam/usr/bin/steam` or persist path (when ABI allows).  
5. Only then fill `matrix/deck-top50-*.md` with real PASS/FAIL.

## Layout

| Location | Role |
|----------|------|
| `build/steam-tree/` | Host cache after fetch (**gitignored**) |
| `build/steam-stage/steam/` | Staged tree (`STATUS=READY` or `SKELETON`) |
| `build/rootfs/opt/steam/` | Rootfs copy |
| **GJ-PERSIST** `/steam/` | Primary durable payload on USB |
| `/usr/bin/steam` | Thin launcher → `/opt/steam` or persist |

## Option 2 — pack into image build

```sh
make steam-fetch      # download+extract Valve bootstrap → build/steam-tree
make steam-stage      # STATUS=READY
make hwtest-img       # p2 GJ-PERSIST/steam/ … (image grows if READY)
sudo make install-hwtest-usb DEV=/dev/sdX
```

One-shot rebuild with fetch+stage+image:

```sh
make steam-host-prep-all    # or: ./scripts/steam-host-prep.sh --all-img
```

## Option 3 — host prep onto existing media

Use when the USB/image already exists and you only need to refresh Steam.

```sh
# Always: fetch + stage on lab host
make steam-fetch
make steam-stage

# Then copy onto target (pick one):

# A) Stick already plugged in (label GJ-PERSIST)
sudo make steam-to-persist
# same as: sudo ./scripts/steam-host-prep.sh --to-label

# B) Explicit mount
sudo mount -L GJ-PERSIST /mnt/gj-persist
sudo ./scripts/steam-host-prep.sh --to-mount /mnt/gj-persist
sudo umount /mnt/gj-persist

# C) Partition node
sudo ./scripts/steam-host-prep.sh --to-dev /dev/sdX2

# D) Existing image file (loop-mount p2, no full rebuild)
sudo ./scripts/steam-host-prep.sh --to-img build/greenjade-hwtest.img

# E) Rootfs tree only
./scripts/steam-host-prep.sh --to-rootfs
```

Skip re-download:

```sh
GJ_SKIP_FETCH=1 sudo ./scripts/steam-host-prep.sh --to-label
# or:
./scripts/steam-host-prep.sh --skip-fetch --to-stage
```

## Inspect

```sh
./scripts/steam-bar3-check.sh          # soft: READY | SKELETON | MISSING

sudo mount -L GJ-PERSIST /mnt/gj-persist
cat /mnt/gj-persist/steam/STATUS    # READY or SKELETON
ls /mnt/gj-persist/steam/
cat /mnt/gj-persist/steam/MANIFEST.txt
```

## Without network

```sh
make steam-stage    # SKELETON placeholders if no build/steam-tree
make hwtest-img     # still packs steam/README + STATUS
# If deb already under build/steam-tree/download/:
GJ_SKIP_FETCH=1 make steam-stage
```

## Runtime (bar #3 — still open)

Staging does **not** mean Steam runs on the DUT yet:

1. Linux ABI / `libcgj` + `ld-gj` for launcher + Steam Runtime  
2. GPU / compositor / input / audio / net  
3. First-run runtime download (needs network)

Launch candidates when ready:

```text
/opt/steam/usr/bin/steam
/mnt/gj-persist/steam/usr/bin/steam
/usr/bin/steam
```

## Related

- [HCL.md](HCL.md) — hardware-test USB flow  
- [PROTON_PERSONALITY.md](PROTON_PERSONALITY.md) — Deck Top 50  
- [GLIBC_COMPAT.md](GLIBC_COMPAT.md) — libc / Steam ABI policy  
- [matrix/deck-top50-2026-07-19.md](../matrix/deck-top50-2026-07-19.md)  
