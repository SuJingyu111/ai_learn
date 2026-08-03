# Unit 8 Syllabus — Capstone Studio、Reliability 与 Portfolio

**Weeks 43–48 · M8 · Gate G8**

## Entry prerequisite

G1–G7 全部 PASS。若任何 hard blocker 尚未关闭，先用 buffer week remediation；本单元
不应用新 feature 掩盖旧 correctness、security 或 reproducibility debt。

## Observable outcomes

1. 使用 hypothesis→controlled change→evidence→decision 完成有限优化；
2. 建立 sustained validation、CI、release、artifact provenance 和 rollback；
3. 完成 failure injection、long-run、shutdown、OOM/shape/version/security tests；
4. 将三个 case study 写成可复核的 technical narrative；
5. 用真实 artifact 覆盖 GH-600 六域，并识别仍未达到的 readiness gap；
6. 从 clean checkout 完成 release、demo 和 10 分钟 defense。

## Concept inventory

`controlled experiment`、`baseline`、`regression`、`reproducibility`、`provenance`、
`release artifact`、`rollback`、`failure injection`、`long-run`、`CI permissions`、
`untrusted input`、`threat model`、`eval coverage`、`design defense`、`readiness`。

## Teaching spine

本单元原则上不引入大型新技术。主要复习：

- [`benchmark-methodology.md`](../../docs/benchmark-methodology.md)
- [`mastery-gates.md`](../../docs/mastery-gates.md)
- [GitHub Actions workflow syntax](https://docs.github.com/en/actions/reference/workflows-and-actions/workflow-syntax)
- [Secure use of GitHub Actions](https://docs.github.com/en/actions/reference/security/secure-use)
- [MCP Security Best Practices](https://modelcontextprotocol.io/docs/tutorials/security/security_best_practices)
- [GH-600 Study Guide](https://learn.microsoft.com/en-us/credentials/certifications/resources/study-guides/gh-600)
- [GH-600 course](https://learn.microsoft.com/en-us/training/courses/gh-600t00)

Study Guide 用于 coverage，不是教学正文。GH-600T00 页面在 2026-07-26 仍标注
2026-07-31 才可用；此前由三个已发布 Microsoft Learn modules 与 GitHub/MCP 官方文档
承担 instruction，上线后也要先复核内容。产品状态与考纲在 Week 45 和 Week 48
重新核对。

## Six-week studio sequence

| Week | Studio focus | Review problem | Project increment |
|---:|---|---|---|
| 43 | one-variable controlled optimization | 识别 confounder | candidate release |
| 44 | sustained validation + standard CI | permission/injection review | release workflow |
| 45 | failure injection/reproduction/security | incident walkthrough | hardened RC |
| 46 | three case studies + GH domain gaps | evidence-to-claim audit | portfolio draft |
| 47 | self-authored unofficial mock + demo defense | error taxonomy | final fixes |
| 48 | clean release + external-style defense | unseen follow-ups | M8 / G8 |

GitHub Agentic Workflows、GitHub Models eval、Copilot Memory 等 preview features 都是
elective；standard Actions、本地 eval harness 和通用 state/security concepts 是
mandatory fallback。

## Required studio reviews

- 每个性能 claim 必须链接 raw samples、environment 和 controlled diff。
- 每个 case study 必须包含一个被证据推翻的 hypothesis 或明确 limitation。
- threat model 至少覆盖 prompt injection、tool misuse、data exfiltration、token leakage、
  path traversal、schema abuse、stale evidence 和 unauthorized write。
- CI 必须最小权限、隔离 untrusted input、固定第三方 action revision。
- GH-600 mock 必须标注“自编、非官方”；exam sandbox 只用于熟悉 UI。

## Final deliverables

1. Orin inference pipeline release；
2. versioned benchmark/evidence bundles；
3. Runtime Lab Assistant v1；
4. CUDA preprocessor、async pipeline、runtime integration 三份 case study；
5. architecture、ownership、threat-model、runbook 和 rollback；
6. clean reproduction transcript；
7. Runtime graduation report；
8. 独立的 GH-600 readiness report。

## G8

- cumulative closed-book knowledge exam；
- unseen system/debug/security practical；
- M8 clean checkout、representative sustained run、all mandatory tests；
- 10 分钟 demo/design defense + random follow-ups。

课程毕业要求 G8 PASS。GH-600 是否预约由最近两次 timed self-authored mock、六域 evidence
和最新考纲共同决定，不与 Runtime 分数混算。
