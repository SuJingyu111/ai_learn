# Unit 8 · Week 44 — Sustained Validation、Standard Actions/CI 与 Production Runbook

> **课程角色**：验证 W43 decision 在 sustained Orin 条件下是否仍成立，并用标准
> GitHub Actions 固化 host build/test/artifacts；GPU/Orin 仍走明确 manual/self-hosted
> gate。Sidecar 建立 production retry/rollback/escalation runbook。
>
> **Agentic Workflows：Preview/选修**，不属于本周必修、不得替代标准 Actions/CI，
> 不作为 Gate 或 GH-600 readiness evidence。GitHub Models eval 与 Copilot Memory
> 若另行探索也均标为 **Preview/选修**；本周不使用。
>
> 课程导航：[总纲](../CURRICULUM.md) · [资料索引](../docs/resources.md) ·
> [Mastery Gates](../docs/mastery-gates.md)

## 硬先修与 remediation

- **硬先修**：W43 keep/revert decision、rollback path、frozen beta/candidate identities。
- TensorRT/Jetson sustained evidence继续使用实际 version pins 与相同 operating point。
- **Remediation**：candidate correctness 未通过时直接 rollback；standard CI 不能运行
  GPU 时写 CPU-only scope，不下载伪 GPU emulator、不把 unavailable 标 passed。

## Observable objectives（可观察目标）

完成后能够：

1. 在 sustained 条件比较 beta/candidate 并判断 thermal/variance/reliability。
2. 用 standard Actions 实现 least-privilege host build/test/artifact workflow。
3. 明确 hosted CI 与 Orin manual/self-hosted gate 的 coverage gap。
4. 用 trigger→validate→execute→verify→rollback→escalate 运行 production tabletop。

## 术语表

| 术语 | 本周可操作定义 |
|---|---|
| Sustained validation | 在稳定 operating window 重复验证 correctness/performance |
| Standard Actions | YAML workflow/jobs/steps/permissions/artifacts 的确定性 CI |
| Artifact retention | workflow outputs 的保存期限、provenance 与访问边界 |
| Concurrency | 避免重复/过时 runs 争用或覆盖 artifacts 的策略 |
| Runbook | 有 trigger、owner、commands、verification、rollback、escalation 的操作文档 |

## 必修理论材料（分钟计入对应 Day）

| ID | 精确 URL 与章节/页面 | 分钟 | Day |
|---|---|---:|---:|
| T1 | [GitHub Actions workflow syntax](https://docs.github.com/en/actions/reference/workflows-and-actions/workflow-syntax) — `permissions`、`concurrency`、`timeout-minutes` | 20 | 1 |
| T2 | [Store and share workflow artifacts](https://docs.github.com/en/actions/tutorials/store-and-share-data) — upload/download artifacts | 15 | 2 |
| T3 | [Security hardening for GitHub Actions](https://docs.github.com/en/actions/security-for-github-actions/security-guides/security-hardening-for-github-actions) — least privilege、untrusted input、pinning | 20 | 4 |

## 带问题阅读

- 为什么 hosted CPU CI 不能将 CUDA/Orin gate 标为 pass？
- artifact 应保存什么才能复现，同时避免 secrets/oversized raw data？
- retry 何时安全，何时会放大非幂等 side effects？

## Worked example

```text
pull_request:
  permissions: contents: read
  jobs: format + CPU build + unit/reference tests + manifest artifact
Orin gate:
  explicit device owner + pinned environment + signed artifact/result
```

runbook 在 timeout 后先判定是否已有 partial side effect；只有幂等/read-only step 可直接 retry。

## 累计项目 increment

- `.github/workflows/ci.yml`
- `artifacts/week-44/sustained-validation.csv`
- `artifacts/week-44/ci-scope-table.md`
- `ai-app/deployment/production-runbook.md`
- `docs/case-studies/runtime-integration.md`

## 本周必须产出

- beta/candidate sustained comparison 与 final keep/revert。
- standard Actions CPU CI、artifact schema、Orin gate checklist。
- production runbook、一次 tabletop 与 3–5 个 GH scenarios。

## Day 1 — 理论与 CI contract（105 分钟）

- **20 分钟**：阅读 T1。
- **25 分钟**：定义 host CI triggers、permissions、timeouts、concurrency、pass/fail。
- **50 分钟**：审计现有 `.github/workflows/ci.yml`，列 CPU-covered 与 unavailable GPU
  coverage；修改只限 deterministic standard Actions。
- **10 分钟**：写 CI threat/scope table。

**必须留下**：CI contract、permissions diff 和 `ci-scope-table.md`。

## Day 2 — Guided lab：Host CI 与 artifacts（110 分钟）

- **15 分钟**：阅读 T2。
- **70 分钟**：本地模拟 configure/build/test/format commands；设置 manifest/test
  artifact，含 source identity、toolchain、result，不含 secrets/大模型。
- **15 分钟**：设计 explicit Orin manual/self-hosted checklist 与 artifact fields。
- **10 分钟**：记录 unavailable coverage。

**必须留下**：standard CI workflow、artifact example 和 Orin checklist。

## Day 3 — Independent lab：Sustained candidate validation（105 分钟）

- **15 分钟**：确认 beta/candidate 的 model/input/power/thermal/sampling 相同。
- **70 分钟**：交替运行 sustained windows，采 correctness、latency over time、
  throughput、memory、temperature/power/throttling。
- **20 分钟**：按 cold/warm/steady 比较，执行 final keep/revert。

**必须留下**：`sustained-validation.csv`、operating conditions 和 decision。

## Day 4 — Integration：Production runbook（110 分钟）

- **20 分钟**：阅读 T3。
- **60 分钟**：编写 `production-runbook.md`：trigger→validate→execute→observe→verify→
  retry/rollback→escalate→close；写 owner、timeout、idempotency、audit、disable。
- **20 分钟**：tabletop timeout、partial artifact、duplicate CI run。
- **10 分钟**：记录 rollback/recovery gaps。

**必须留下**：runbook v1、三条 tabletop traces 和 gap list。

## Day 5 — Assessment：CI/security/reliability drill（105 分钟）

- **50 分钟**：从 clean state跑 host CI commands、artifact validation 与一个 Orin gate
  dry-run；检查 untrusted PR/issue/log 不进入 shell。
- **35 分钟**：写 3–5 个 scenarios，覆盖 permissions、artifact trust、retry、
  rollback、GPU unavailable。
- **20 分钟**：更新 GH checklist 与 `runtime-integration.md` sustained section。

**必须留下**：CI validation、3–5 题解析和 case-study increment。

## Day 6 — Synthesis 与 Exit ticket（100 分钟）

- **35 分钟**：clean rerun representative sustained/CI subset。
- **25 分钟**：tabletop disable→rollback→evidence preservation→escalation。
- **25 分钟**：写周总结与 production limitations。
- **15 分钟**：准备 W45 failure/reproduction/security matrix。

**必须留下**：Week 44 evidence index、runbook drill 和 W45 risk backlog。

## Exit ticket 与 unlocks

形成性要求：闭卷 5 题首次 ≥4/5、90 秒口述、1 个 unseen input、随机 3 个术语；
未过项在下一周 Day 1 前用新题订正。

- standard Actions 明确 least privilege、timeout、concurrency、artifacts。
- hosted CI 不谎报 GPU coverage；Orin gate 有 owner/commands/evidence。
- sustained decision 符合 W43 preregistration，runbook rollback 可执行。
- **Unlock W45**：release candidate identity 与 top failure/security risks 已冻结。

## Optional Feed Card（30–45 分钟，不计核心时长）

- **来源/选择规则**：从 [GitHub Changelog · Actions](https://github.blog/changelog/label/actions/)
  只选 1 条与 permissions、artifacts、concurrency、runner 或 supply-chain security
  相关的 GA/retirement 更新；Agentic Workflows 仍为 Preview/选修。
- **Artifact**：写 `notes/feed/week-44.md`，固定四栏
  `claim → evidence → scope → decision`，并注明是否需要 standard Actions regression。
- **取消规则**：standard CI、Orin gate 或 runbook 任一落后时直接取消。
