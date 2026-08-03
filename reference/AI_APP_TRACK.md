# Agentic AI / MCP / GH-600 副线课程

副线每天约 20–30 分钟、每周 1.5–2 小时，但仍按 prerequisite chain 前进。它围绕同一个
Runtime Lab Assistant 逐步升级；不是每天刷认证题，也不在未学协议/权限前先部署 server。
日常任务已经直接写进 [`weeks/`](weeks/)。

## 知识依赖

```text
Git/GitHub Flow
 → Actions / SDLC / governance
 → agent lifecycle / control plane
 → typed tools / safe execution
 → MCP JSON-RPC / lifecycle / capabilities
 → local read-only server
 → HTTP / authorization / security
 → durable state / eval
 → multi-agent isolation / handoff
 → HITL / audit / red-team
 → GitHub MCP / CI / production
 → GH-600 evidence / defense
```

## 八单元安排

| Unit | Weeks | 正式知识 | 累计产品 |
|---|---:|---|---|
| A1 | 1–6 | Git、GitHub、Actions、agent fundamentals/control plane | architecture dossier + read-only reviewer |
| A2 | 7–12 | custom agents、typed tools、JSON Schema、permissions、failure | failure-aware tool |
| A3 | 13–18 | MCP architecture、JSON-RPC、lifecycle、capabilities/primitives | protocol traces + stdio skeleton |
| A4 | 19–24 | server/debug/Inspector、HTTP、auth、security | secured Runtime Lab Assistant v0 |
| A5 | 25–30 | durable state、provenance、TTL/reset/resume、eval dataset | stateful/evaluable v0.3 |
| A6 | 31–36 | traces/tuning、multi-agent patterns/isolation/handoff/recovery | evidence reviewer fixture |
| A7 | 37–42 | autonomy、HITL、audit、red team、GitHub MCP/registry/CI | guarded evidence service |
| A8 | 43–48 | production hardening、GH-600 six domains、defense | Runtime Lab Assistant v1 |

## Primary courses

- [Introduction to Git](https://learn.microsoft.com/en-us/training/modules/intro-to-git/)
- [Introduction to GitHub](https://learn.microsoft.com/en-us/training/modules/introduction-to-github/)
- [Introduction to GitHub Actions](https://learn.microsoft.com/en-us/training/modules/introduction-to-github-actions/)
- [Foundations of Agentic AI in GitHub](https://learn.microsoft.com/en-us/training/modules/foundations-agentic-ai/)
- [Design Agent Architecture and SDLC Integration](https://learn.microsoft.com/en-us/training/modules/design-agent-architecture-integration/)
- [Agent Tooling, MCP, and Execution Environments](https://learn.microsoft.com/en-us/training/modules/agent-tooling-mcp-execution-environments/)
- [MCP Architecture](https://modelcontextprotocol.io/docs/learn/architecture)
- [MCP specification 2025-11-25](https://modelcontextprotocol.io/specification/2025-11-25)
- [GH-600T00 course](https://learn.microsoft.com/en-us/training/courses/gh-600t00)：
  页面截至 2026-07-26 标注 2026-07-31 才可用；上线前不把它当成已完成的 prerequisite。

完整有序章节见 [`docs/resources.md`](docs/resources.md)。当前教学主线是上面的三个
已发布 Microsoft Learn modules 与 GitHub/MCP 官方文档；GH-600 Study Guide 是考纲，
不是教材；podcast/blog 是 optional seminar。

## Runtime Lab Assistant contract

Mandatory read-only capabilities：

```text
resources/read fixed run manifests
tools/list_runs(filter)
tools/validate_run(run_id)
tools/compare_runs(baseline_id, candidate_id)
prompts/profiling_review
```

强制安全边界：

- fixed artifact root/resource URIs，不接收任意 path；
- 不暴露 shell、device configuration、secret 或 write capability；
- tool/MCP/GitHub output 一律视为 untrusted；
- structured schema 不等于 authorization；
- timeout、body/result limits、audit、source identity；
- high-risk action 保持在 human approval/control plane。

## Sidecar assessment

每个 Unit 随主线 Gate 检查：

- closed-book mechanism/scenario questions；
- mandatory schema/protocol/security negative tests 100%；
- clean regression client/eval dataset；
- oral explanation：保证、非保证、failure、blast radius；
- artifact 链接到 GH-600 domain。

Preview feature（例如某些 Copilot Memory、GitHub Models eval、Agentic Workflows）无法访问
时，用本地、可重复 fixture 评估通用机制。产品体验可 elective，不阻塞课程。

## GH-600 readiness

- 六个 domain 均有实际 artifact；
- 最近两次 timed mock 达到自定稳定阈值；
- mock 明确标注“自编、非官方”；
- exam sandbox 只用于熟悉 UI；
- 考前重新核对最新 [Study Guide](https://learn.microsoft.com/en-us/credentials/certifications/resources/study-guides/gh-600)；
- 与 Runtime graduation 分开出具 readiness report。
