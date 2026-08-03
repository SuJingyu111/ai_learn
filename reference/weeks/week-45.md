# Unit 8 · Week 45 — Failure Injection、Reproduction 与 GH-600 Domains 1–3

> **课程角色**：对 release candidate 做 clean reproduction、safe failure injection、
> RAII/lifetime 与 security audit；同时用真实 artifacts 复盘 GH-600 的 Architecture、
> Tool/environment、Memory/state 三域，并推进 MCP capstone 的 failure behavior。
>
> 课程导航：[总纲](../CURRICULUM.md) · [资料索引](../docs/resources.md) ·
> [Mastery Gates](../docs/mastery-gates.md)

## 硬先修与 remediation

- **硬先修**：W44 frozen release candidate、standard CI、Orin gate、production runbook；
  Runtime Lab Assistant read-only functional/security baseline。
- TensorRT/Jetson reproduction 必须使用实际 version/engine/environment pins；cache 或旧
  engine 不得掩盖 hidden setup。
- **Remediation**：clean build 失败时优先修第一条 blocker；failure injection 只使用
  controlled flags/fixtures，不制造真实 OOM、设备损坏、远端状态变更或 secrets exposure。

## Observable objectives（可观察目标）

完成后能够：

1. 从 clean directory 重建、部署、运行、验证 release candidate。
2. 对 invalid shape/metadata/path、timeout、early shutdown 等安全注入并验证 cleanup。
3. 从 error/log/trace/artifact 将 failure 定位到 root cause。
4. 为 GH-600 Domains 1–3 建立 artifact map、scenario evidence 与 remediation。

## 术语表

| 术语 | 本周可操作定义 |
|---|---|
| Failure injection | 用受控 fixture 触发预定义错误路径 |
| Fail fast | 在 side effect/partial output 前拒绝 invalid state |
| Sticky state | 一次失败污染后续正常运行的残留状态 |
| RAII audit | 检查所有 exits 的 owner/destruction/async completion |
| Evidence gap | 有知识但无 artifact，或 artifact 无法支撑解释 |
| Root cause | 由 evidence 支持的最底层可修复原因，不是表面 symptom |

## 必修理论材料（分钟计入对应 Day）

| ID | 精确 URL 与章节/页面 | 分钟 | Day |
|---|---|---:|---:|
| T1 | [Security hardening for GitHub Actions](https://docs.github.com/en/actions/security-for-github-actions/security-guides/security-hardening-for-github-actions) — untrusted input、least privilege、third-party actions | 15 | 1 |
| T2 | [MCP Security Best Practices](https://modelcontextprotocol.io/docs/tutorials/security/security_best_practices) — scope minimization、confused deputy、local server security | 20 | 3 |
| T3 | [GH-600 Study Guide](https://learn.microsoft.com/en-us/credentials/certifications/resources/study-guides/gh-600) — **仅用于** Domains 1–3 blueprint/domain/skill mapping；guardrail 机制以 T1/T2 为准 | 25 | 4 |

## 带问题阅读

- clean reproduction 如何证明不是本机 cache/shell history 在“帮忙”？
- failure 返回 error 但泄漏 resource/留下 partial output，能否算正确处理？
- GH-600 场景题中哪个 constraint 决定 tool、state 或 approval 的最佳选择？

## Worked example

```text
fixture: dynamic input H=800 > profile max=640
expected: validate -> reject -> no allocation/enqueue -> structured error -> audit
regression: next valid H=512 succeeds with clean state
```

同一原则用于 MCP traversal/timeout：无 arbitrary path、无 partial trusted result、可恢复。

## 累计项目 increment

- `artifacts/week-45/clean-reproduction.md`
- `artifacts/week-45/failure-matrix.md`
- `artifacts/week-45/lifetime-audit.md`
- `artifacts/week-45/gh600-domains-1-3.md`
- `ai-app/threat-model.md`
- `docs/gh600-checklist.md`

## 本周必须产出

- clean transcript、failure matrix、RAII/lifetime audit 与 regression evidence。
- MCP capstone failure/security tests 与 updated threat model。
- Domains 1–3 artifact/gap map、至少 12 个自写 scenarios 与 top remediation。

## Day 1 — 理论与 clean reproduction（105 分钟）

- **15 分钟**：阅读 T1。
- **20 分钟**：创建新的 build/artifact dirs，冻结 source/model/engine/environment IDs。
- **60 分钟**：按 README 执行 configure→build→test→deploy→run→validate；记录 hidden
  dependencies、permissions、cache、personal paths。
- **10 分钟**：修文档或写 first blocker。

**必须留下**：`clean-reproduction.md`、support matrix 和 exact first blocker。

## Day 2 — Guided lab：Runtime failure injection（110 分钟）

- **20 分钟**：预注册至少五项：missing model、wrong shape/dtype、bad metadata/path、
  timeout/early shutdown、simulated allocation failure。
- **70 分钟**：运行 fixtures；检查 fail-fast、diagnostic、no partial output、cleanup、
  next-valid-run；不在真实设备施加危险资源压力。
- **10 分钟**：跑 normal regression。
- **10 分钟**：按 root-cause taxonomy 标注。

**必须留下**：`failure-matrix.md`、diagnostics、cleanup 与 regression。

## Day 3 — Independent lab：RAII 与 MCP security（105 分钟）

- **20 分钟**：阅读 T2。
- **50 分钟**：审计 host/device buffers、streams/events、runtime/engine/context、
  early returns；修一个 highest-risk lifetime issue并重跑。
- **25 分钟**：运行 MCP invalid schema/path/timeout/prompt-injection fixtures，更新
  `ai-app/threat-model.md`。
- **10 分钟**：记录 residual risks。

**必须留下**：`lifetime-audit.md`、修复 evidence 和 MCP security regression。

## Day 4 — Integration：GH-600 Domains 1–3 evidence lab（110 分钟）

- **25 分钟**：阅读 T3，重新核对当前 domain 名称/权重/skills；记录访问日期。
- **55 分钟**：每域写 4 个原创 scenarios（共 12），每题含 constraint、best action、
  distractor reason 与 repo evidence；不得复制/回忆真实 exam items。
- **20 分钟**：把 gaps 分为 knowledge/no-artifact/weak-explanation。
- **10 分钟**：更新 `docs/gh600-checklist.md`。

**必须留下**：`gh600-domains-1-3.md`、12 题解析和 top-3 gaps。

## Day 5 — Assessment：Mixed failure defense（105 分钟）

- **55 分钟**：随机重跑三条 runtime 与三条 MCP failure cases，口述 detect→contain→
  recover→verify→audit。
- **35 分钟**：闭卷做 9 个新的 Domains 1–3 scenarios，统计 error taxonomy/confidence。
- **15 分钟**：关闭一个 release blocker 或 domain evidence gap。

**必须留下**：sampled regression、9 题 error log 和 remediation evidence。

## Day 6 — Synthesis 与 Exit ticket（100 分钟）

- **40 分钟**：clean success→failure→success sequence 与 CI subset。
- **20 分钟**：抽三个 artifacts 做 2 分钟 mechanism/tradeoff/evidence/failure explanation。
- **25 分钟**：写周总结与 must-fix/known limitation。
- **15 分钟**：准备 W46 case-study evidence index 与 Domains 4–6 map。

**必须留下**：Week 45 evidence index、blocker list 和 W46 inputs。

## Exit ticket 与 unlocks

形成性要求：闭卷 5 题首次 ≥4/5、90 秒口述、1 个 unseen input、随机 3 个术语；
未过项在下一周 Day 1 前用新题订正。

- clean reproduction 不依赖隐藏 cache/history。
- 每项 injected failure 有 expected diagnostic、cleanup 与 next-valid-run。
- Domains 1–3 的每个“会”都链接 artifact，而非主观勾选。
- **Unlock W46**：release blockers 有 owner/status，Domains 1–3 gap map 完整。

## Optional Feed Card（30–45 分钟，不计核心时长）

- **来源/选择规则**：从 [GitHub Blog · Security](https://github.blog/security/)
  只选 1 篇含 failure chain、reproduction、software supply chain 或 agent/tool security
  的文章；必须能提出一个安全、本地、可回滚的 failure fixture。
- **Artifact**：写 `notes/feed/week-45.md`，固定四栏
  `claim → evidence → scope → decision`，并记录拟议 fixture 是否采纳及原因。
- **取消规则**：release blocker 或 required failure matrix 落后时直接取消。
