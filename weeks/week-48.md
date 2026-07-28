# Unit 8 · Week 48 — Final Release、MCP Capstone 与 M8 Hard Gate

> **课程角色**：一年课程结课。完成可由陌生 reviewer clean reproduce、质疑和回滚的
> runtime portfolio；发布 read-only Runtime Lab Assistant v1；用 M8 hard gate 判断
> 课程完成，并独立做 GH-600 预约/暂缓决定；Day 5–6 为两日评估。
>
> 课程导航：[总纲](../CURRICULUM.md) · [资料索引](../docs/resources.md) ·
> [Mastery Gates](../docs/mastery-gates.md)

## 硬先修与 remediation

- **硬先修**：G1–G7 全部 PASS；W47 demo、release-blocker audit 与 six-domain map
  完成；standard CI、Orin gate、three case studies、MCP functional/security evidence
  均可从 source commit 追溯。
- 所有 TensorRT/Jetson claims 保留 actual version/archive/engine/operating-point pins；
  release 前 stack 变化必须重新验证，不能引用 `latest` 代替。
- **Remediation**：Day 1 冻结后只修 blocker/regression/security high finding，不新增
  feature；任一 hard blocker 未过则 M8 fail/remediate，不用总分或 unofficial mock 抵消。

## Observable objectives（可观察目标）

完成后能够：

1. 从 clean state reproduce runtime release 与 representative Orin evidence。
2. 演示 read-only MCP v1 的 resources/list/validate/compare、errors、audit 与 disable。
3. 交付三份 traceable case studies、README、5–10 分钟 talk 与 residual-risk register。
4. 按统一 rubric 完成 M8，并以六域稳定 evidence 做 exam readiness 决策。

## 术语表

| 术语 | 本周可操作定义 |
|---|---|
| Release identity | source/model/engine/environment/config/artifact hashes 的集合 |
| Reproducibility | 陌生 reviewer 仅按文档可得到 declared result 或 exact blocker |
| Residual risk | 已知但未消除、含 severity/mitigation/owner 的风险 |
| Safe-use boundary | MCP v1 被允许/禁止的 data、tools、paths 与 actions |
| Readiness decision | 基于六域证据与重复错误，而非一次 unofficial score |
| M8 hard gate | ≥80 且所有 release/correctness/security blockers pass |

## 必修理论材料（分钟计入对应 Day）

| ID | 精确 URL 与章节/页面 | 分钟 | Day |
|---|---|---:|---:|
| T1 | [GH-600 Study Guide](https://learn.microsoft.com/en-us/credentials/certifications/resources/study-guides/gh-600) — **仅用于** six-domain blueprint/skill mapping，不承担机制教学 | 5 | 1 |
| T2 | [MCP Tools, stable revision 2025-11-25](https://modelcontextprotocol.io/specification/2025-11-25/server/tools) — schemas、results、human control | 15 | 2 |
| T3 | [MCP Security Best Practices](https://modelcontextprotocol.io/docs/tutorials/security/security_best_practices) — scope、token、sandbox、confused deputy | 20 | 3 |
| T4 | [Exam and assessment security policies](https://learn.microsoft.com/en-us/credentials/support/exam-and-assessment-lab-security-policies) — ethical/confidentiality boundary | 10 | 5 |
| T5 | [Microsoft Agent Framework — Workflow orchestrations](https://learn.microsoft.com/en-us/agent-framework/workflows/orchestrations/) — sequential、concurrent、handoff 与 HITL 的执行机制 | 10 | 1 |

## 带问题阅读

- 陌生 reviewer 如何验证 release，而不是相信 README claim？
- MCP v1 的 disable/rollback/audit 与普通 happy-path demo 有何不同？
- 课程 Gate pass 与 GH-600 exam readiness 为什么是两个独立决定？

## Worked example

```text
release manifest:
source=S, model=M, engine=E, JetPack/L4T/TRT=V, power=P, input=I
tests: CPU/Orin correctness + sustained + failure + CI
MCP v1: read-only allowlisted artifacts; no arbitrary path/shell/write
known risk: severity + mitigation + owner + retest date
```

## 累计项目 increment

- `artifacts/week-48/final-release-manifest.md`
- `artifacts/week-48/mcp-v1-report.md`
- `artifacts/week-48/m8-scorecard.md`
- `artifacts/week-48/final-readiness.md`
- `notes/gates/gate-8.md`
- `docs/case-studies/resize.md`
- `docs/case-studies/async-pipeline.md`
- `docs/case-studies/runtime-integration.md`
- `ai-app/deployment/production-runbook.md`

## 本周必须产出

- final release/README/reproduction、three case studies、talk 与 residual risks。
- Runtime Lab Assistant v1 functional/security/deployment evidence。
- M8 scorecard、exam readiness decision 与 90-day transition plan。

## Day 1 — 理论与 final scope freeze（105 分钟）

- **15 分钟**：阅读 T1+T5；T1 只核对六域 blueprint/访问日期，T5 复核
  multi-agent/HITL 的真实执行机制。
- **25 分钟**：冻结 source/model/engine/environment/input/config IDs 与 non-goals。
- **55 分钟**：从 final candidate clean build/test/deploy/run representative case；
  失败只修 blocker。
- **10 分钟**：创建 `final-release-manifest.md`。

**必须留下**：release identity、clean transcript 和 frozen blocker list。

## Day 2 — Guided lab：MCP v1 end-to-end（110 分钟）

- **15 分钟**：阅读 T2。
- **70 分钟**：fresh client 完成 initialize/discovery→read architecture/methodology→
  list allowlisted runs→validate→compare→profiling-review prompt；保存 correlation IDs、
  schemas、typed results、audit。
- **15 分钟**：验证 v1 manifest 只有已测试 read-only capabilities。
- **10 分钟**：补 hidden prerequisites。

**必须留下**：MCP demo trace、v1 capability manifest 和 audit evidence。

## Day 3 — Independent lab：Security/eval release gate（105 分钟）

- **20 分钟**：阅读 T3。
- **60 分钟**：运行 functional、invalid schema、missing permission、traversal、prompt
  injection、timeout、stale/incompatible result、audit completeness tests。
- **15 分钟**：只修最高严重度一项并重跑 regression。
- **10 分钟**：冻结 known-risk register。

**必须留下**：`mcp-v1-report.md`、fix/regression 和 residual risks。

## Day 4 — Integration：Portfolio handoff（110 分钟）

- **60 分钟**：以陌生 reviewer 视角按 README setup/run/validate/profile；每步给
  expected output、hardware/time、fallback、cleanup，blocked 项写 exact reproduction。
- **30 分钟**：审查三份 case studies 的 raw links、units、environment、negative
  result、limitations。
- **20 分钟**：按 production runbook 演练 health→disable→rollback→log preservation。

**必须留下**：handoff transcript、final README/case studies 和 rollback evidence。

## Day 5 — M8 Gate A：Knowledge 与 submission audit（105 分钟）

- **10 分钟**：阅读 T4，确认 assessment 只使用自编题，不使用/分享真实 exam content。
- **50 分钟**：闭卷完成预先冻结、首次作答留痕的 scored Knowledge assessment，
  计 **25 分**；题目跨 runtime、profiling、reliability、CI、MCP 与 security。
- **25 分钟**：执行 submission audit：核对 commit/environment/variant、release manifest、
  case-study raw links、MCP evidence、CI/Orin results 与四项 rubric evidence index。
- **20 分钟**：执行 blocker audit，冻结 Day 6 submission 与 unseen variant；单独记录
  GH-600 exam 预约/暂缓决定，明确它不属于课程 Gate 得分。

**必须留下**：Knowledge 原始答卷与 25 分得分、submission manifest、blocker audit、
Day 6 frozen variant ID 和 `final-readiness.md`。

## Day 6 — M8 Gate B：Practical、project、oral 与 report（110 分钟）

- **45 分钟**：执行考官从冻结池抽取的 unseen practical，计 **30 分**；覆盖 runtime
  或 MCP failure/security path，保存 diagnostic、cleanup、rollback 与 next-valid-run。
- **30 分钟**：从 clean process 复现 cumulative runtime project 与 MCP v1 demo，
  计 **30 分**；验证 case-study claims、standard CI/Orin gate 与 raw evidence。
- **20 分钟**：完成 oral defense，计 **15 分**；用 5–10 分钟讲清
  problem→contracts→correctness→profiling→reliability→MCP，并回答现场追问。
- **15 分钟**：汇总两日 **Knowledge 25 / Unseen practical 30 / Cumulative project 30 /
  Oral defense 15**，签署 pass/remediate，写 retrospective 与最多三条 90-day backlog。

**必须留下**：`m8-scorecard.md`、`notes/gates/gate-8.md`、unseen/clean transcripts、
oral prompts/notes、final handoff package 和 90-day plan。

## M8 blockers、Exit ticket 与 unlocks

**通过条件：总分 ≥80/100，且 blockers 全部通过。**

- [ ] actual JetPack/TensorRT/engine/model/input/power identity 与 clean reproduction 完整。
- [ ] declared runtime cases correctness、sustained、failure/cleanup、CI/Orin gates 通过。
- [ ] 三份 case studies 的 claims 可追到 raw evidence，limitations/negative result 未隐藏。
- [ ] MCP v1 只读、allowlisted、可 disable/rollback/audit，functional/security tests 通过。
- [ ] GH-600 六域各有真实 artifact；所有 mock 明确 self-authored/unofficial，考试伦理合规。

Gate administration：Knowledge 首次 ≥80%、订正后 100%；mandatory correctness/security
tests 100%；report 记录 date、commit、environment、variant ID、四项得分与 remediation。

Exit ticket：10 分钟内完成 portfolio+MCP defense，并指出三个 residual risks。
**Unlock**：M8 pass 后进入求职/贡献/90-day specialization；exam 预约仍服从独立 readiness
decision，不由课程 Gate 自动触发。

## Optional Feed Card（30–45 分钟，不计核心时长）

- **来源/选择规则**：从 [GitHub ReadME Project](https://github.com/readme)
  只选 1 篇包含 maintainer、production practice、incident learning 或 community
  contribution 的访谈；至少有一个可行动且可验证的工程主张。
- **Artifact**：写 `notes/feed/week-48.md`，固定四栏
  `claim → evidence → scope → decision`，decision 进入 90-day backlog 或明确 reject/defer；
  不得作为 M8 补分。
- **取消规则**：Gate submission/blocker audit 落后或未冻结时直接取消，不补做。
