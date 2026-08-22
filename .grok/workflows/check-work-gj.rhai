let meta = #{
    name: "check-work-gj",
    description: "Disjoint GreenJade review (kernel-net, udx, abi-sshd, pack-proof, docs) plus adversarial verify.",
    when_to_use: "When asked to check work, verify a pack, or review the Dual DoD / ABI / UDX tree without overlapping writers.",
    phases: [
        #{ title: "Review", detail: "one read-only shard per disjoint prefix list" },
        #{ title: "Verify", detail: "skeptics fail-closed on each finding" },
        #{ title: "Report", detail: "only findings that survived verification" },
    ],
};

let findings_schema = #{
    "type": "object", "required": ["findings"],
    "properties": #{
        "findings": #{ "type": "array", "maxItems": 8, "items": #{
            "type": "object", "required": ["file", "issue"],
            "properties": #{
                "file": #{ "type": "string" },
                "issue": #{ "type": "string" },
            },
        }},
    },
};
let verdict_schema = #{
    "type": "object", "required": ["real", "reason", "evidence"],
    "properties": #{
        "real": #{ "type": "boolean" },
        "reason": #{ "type": "string" },
        "evidence": #{ "type": "string" },
    },
};

fn trimmed(s) {
    if type_of(s) == "string" { s.trim(); s } else { "" }
}

fn output_map(r) {
    if r == () { return (); }
    if r.success != true { return (); }
    if r.output == () { return (); }
    if type_of(r.output) != "map" { return (); }
    r.output
}

fn str_has(hay, needle) {
    if type_of(hay) != "string" { return false; }
    if type_of(needle) != "string" { return false; }
    let n = needle.len();
    let h = hay.len();
    if n == 0 { return true; }
    if h < n { return false; }
    let last = h - n + 1;
    for i in 0..last {
        if hay.sub_string(i, n) == needle { return true; }
    }
    false
}

fn in_prefixes(path, prefixes) {
    if type_of(path) != "string" { return false; }
    for pre in prefixes {
        if str_has(path, pre) { return true; }
    }
    false
}

const MAX_FINDINGS = 8;

let target = "HEAD";
if args != () && args.target != () {
    let t = trimmed(args.target);
    if t != "" { target = t; }
}

let shards = [
    #{
        id: "kernel-net",
        mode: "read-only",
        kind: "honesty-auditor",
        prefixes: ["kernel/net/", "kernel/drv/net_l2.c", "kernel/drv/fb_console.c", "kernel/include/gj/net_", "kernel/include/gj/fb_console.h"],
    },
    #{
        id: "udx",
        mode: "read-only",
        kind: "honesty-auditor",
        prefixes: ["user/drivers/rtl8168_udx/", "user/drivers/xhci_udx/", "user/udx/"],
    },
    #{
        id: "abi-sshd",
        mode: "read-only",
        kind: "honesty-auditor",
        prefixes: ["kernel/syscall/", "user/sshd/", "user/dash/", "kernel/proc/elf_load.c", "kernel/include/gj/linux_"],
    },
    #{
        id: "pack-proof",
        mode: "execute",
        kind: "hwtest-packer",
        prefixes: ["scripts/make-hwtest-img.sh", "scripts/stage-esp.sh", "scripts/stage-rootfs.sh", "scripts/install-hwtest-usb.sh", "scripts/gj-assurance-check.sh", "build/greenjade-hwtest.img"],
    },
    #{
        id: "docs-honesty",
        mode: "read-only",
        kind: "honesty-auditor",
        prefixes: ["README.md", "docs/HWTEST_TOMORROW.md", "docs/TODO.md", "docs/ASSURANCE_LITE.md", "AGENTS.md"],
    },
];

phase("Review");
let jobs = [];
for sh in shards {
    let pfx_txt = "";
    for pre in sh.prefixes {
        if pfx_txt != "" { pfx_txt += ", "; }
        pfx_txt += pre;
    }
    let pr = "";
    pr += "You are GreenJade ";
    pr += sh.kind;
    pr += " under fail-closed and no-densify. Use tools; do not answer from memory.\n";
    pr += "Scope target: ";
    pr += target;
    pr += "\nYou may inspect ONLY these prefixes: ";
    pr += pfx_txt;
    pr += "\nIf a path is outside that list, ignore it.\n";
    pr += "Read AGENTS.md. Then git diff ";
    pr += target;
    pr += " -- <those prefixes> and read the files. ";
    if sh.id == "pack-proof" {
        pr += "If build/greenjade-hwtest.img exists, extract ESP at offset 1048576 and hash KERNEL vs build/greenjade.elf. ";
        pr += "build/esp is not evidence. Do not flash. Run make assurance-check if cheap; do not rebuild the 2GiB image unless it is missing.\n";
    }
    pr += "Report at most 8 concrete {file, issue} bugs, claim errors, or pack-proof gaps. ";
    pr += "Empty findings is valid only after you inspected the assigned prefixes. ";
    pr += "Do not edit. Do not densify. Do not close Dual DoD.\n";
    jobs.push(#{
        prompt: pr,
        label: "review:" + sh.id,
        agent_type: sh.kind,
        capability_mode: sh.mode,
        output_schema: findings_schema,
    });
}
let results = parallel(jobs);

let findings = [];
let dropped = 0;
let si = 0;
for r in results {
    let o = output_map(r);
    let sh = shards[si];
    if o != () && o.findings != () {
        for f in o.findings {
            let fp = "";
            if f.file != () { fp = trimmed(f.file); }
            if in_prefixes(fp, sh.prefixes) {
                if findings.len() < MAX_FINDINGS {
                    findings.push(#{ file: fp, issue: trimmed(f.issue), shard: sh.id });
                } else {
                    dropped += 1;
                }
            } else {
                dropped += 1;
            }
        }
    }
    si += 1;
}
if dropped > 0 { log("dropped out-of-scope or capped findings: " + dropped.to_string()); }

if findings.len() == 0 {
    let empty = "# check-work-gj\n\nNo in-scope findings after disjoint review of " + target + ".\n";
    let path = write_scratch_file("report.md", empty);
    complete(#{ path: path, summary: "No in-scope findings.", confirmed: [], target: target });
}

phase("Verify");
let vjobs = [];
for f in findings {
    let vp = "";
    vp += "Adversarially verify this GreenJade review finding. Fail-closed. ";
    vp += "Use read-only tools. Read the file yourself. Do not accept the reviewer's word.\n";
    vp += "file: " + json_encode(f.file) + "\n";
    vp += "issue: " + json_encode(f.issue) + "\n";
    vp += "shard: " + f.shard + "\n";
    vp += "Set real=true only with evidence you independently inspected (file:line or command output). ";
    vp += "Otherwise real=false. Soft lamps are not Dual DoD close. build/esp is not pack proof.\n";
    vjobs.push(#{
        prompt: vp,
        label: "verify:" + f.shard,
        agent_type: "honesty-auditor",
        capability_mode: "read-only",
        output_schema: verdict_schema,
    });
}
let verdicts = parallel(vjobs);

let confirmed = [];
let i = 0;
for v in verdicts {
    let o = output_map(v);
    let accepted = false;
    if o != () && o.real == true && o.evidence != () {
        let ev = trimmed(o.evidence);
        if ev != "" { accepted = true; }
    }
    if accepted { confirmed.push(findings[i]); }
    i += 1;
}
log(confirmed.len().to_string() + "/" + findings.len().to_string() + " findings survived verification");

phase("Report");
let report = "";
report += "# check-work-gj\n\n";
report += "- target: " + target + "\n";
report += "- reviewed: " + findings.len().to_string() + "\n";
report += "- confirmed: " + confirmed.len().to_string() + "\n";
report += "- Dual DoD A/B remain OPEN unless host USB path (A) or interactive SSH login (B). Banner / PK_OK / SUCCESS != close\n\n";
if confirmed.len() == 0 {
    report += "No findings survived adversarial verify.\n";
} else {
    report += "## Confirmed\n";
    for f in confirmed {
        report += "- `" + f.file + "` (" + f.shard + "): " + f.issue + "\n";
    }
}
let path = write_scratch_file("report.md", report);
complete(#{
    path: path,
    target: target,
    reviewed: findings.len(),
    confirmed_n: confirmed.len(),
    confirmed: confirmed,
    summary: report,
});
