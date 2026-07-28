# GH-600 实践与 Evidence 清单

本清单按 2026-05-13 更新的官方 GH-600 Study Guide 整理。考试前必须重新访问
[最新 Study Guide](https://learn.microsoft.com/en-us/credentials/certifications/resources/study-guides/gh-600)；
下面的权重和条目不能替代最新版考纲。

本文件只用于累计 evidence 和检查 readiness。逐日学习、实验和场景题已经嵌入
[`../weeks/week-01.md`](../weeks/week-01.md) 至
[`../weeks/week-48.md`](../weeks/week-48.md)，执行时不需要从本清单反查当周任务。

GH-600 当前面向在 production-grade SDLC 中运行、集成、监督和治理 AI agents 的
工程师，并明确要求 GitHub Copilot、MCP servers、custom instructions、custom
agents、tools 和 Copilot setup 的实践经验。

正式学习主线是三个 Microsoft Learn modules 与 GitHub/MCP 官方文档，Study Guide
只定义考纲。Week 1–2 先完成 Git/GitHub Flow、Actions、status checks 与 repository
governance；没有这些 prerequisite，不进入 agent architecture。

## 1. Prepare agent architecture and SDLC processes（15–20%）

- [ ] 为 agent task 定义 inputs、outputs、success criteria 和 non-goals。
- [ ] 识别 agent anti-patterns：无界任务、模糊完成条件、自批自合、隐藏失败。
- [ ] 区分 planning、reasoning artifact 和 action。
- [ ] 在执行前验证 structured plan；高风险 action 等待 approval。
- [ ] 让输出以 issue、plan、diff、test、artifact 等形式可检查。
- [ ] 设计 issue → branch → PR → review → merge 的 Agent/人类责任边界。

Evidence：

- `weeks/week-01.md` 至 `week-02.md` 的 GitHub Flow/Actions prerequisites。
- `weeks/week-03.md` 至 `week-08.md` 的 architecture/control artifacts。
- `.github/copilot-instructions.md`
- `.github/agents/cuda-reviewer.agent.md`
- 一个 plan-before-action fixture 和 review 记录。

## 2. Implement tool use and environment interaction（20–25%）

- [ ] 根据任务选择最小工具集合和权限。
- [ ] 为 tools 定义 typed schema、validation 和 structured errors。
- [ ] 配置 local stdio MCP server。
- [ ] 配置 remote Streamable HTTP MCP server。
- [ ] 理解 GitHub remote MCP server、registry 和 allowlist。
- [ ] 限制 repository/branch/environment scope。
- [ ] 配置 Agent 在 CI 中运行且不泄露 secrets。
- [ ] 实现 timeout、retry、idempotency、rollback、escalation 和 audit。

Evidence：

- Runtime Lab Assistant MCP server。
- tool allowlist/blast-radius table。
- invalid input、timeout 和 missing permission tests。
- 一个 read-only CI analysis workflow。

## 3. Manage memory, state, and execution（10–15%）

- [ ] 区分 short-term、long-term 和 external memory。
- [ ] memory 只保留 task-relevant information。
- [ ] 定义 expiration、pruning、reset 和 privacy。
- [ ] 用 durable artifact 保存 progress、decisions 和 source identity。
- [ ] resume 时不重复已完成步骤，不偏离已确认 decision。
- [ ] 检测 stale context 和 context drift。
- [ ] 多工具/环境共享 state 时防止冲突。

Evidence：

- resumable experiment review state。
- memory lifecycle table。
- stale-state/context-drift eval cases。
- handoff artifact。

## 4. Perform evaluation, error analysis, and tuning（15–20%）

- [ ] 定义 outcome、operational constraints、qualitative/quantitative signals。
- [ ] 建立代表性 eval dataset，而不是只测试 happy path。
- [ ] 使用 tests、scans、logs、traces 和 artifacts 生成 evidence。
- [ ] 将失败分类为 reasoning、tool misuse、context、environment 或 policy。
- [ ] 根据 failure 修改 instructions、workflow、memory 或 tool access。
- [ ] before/after evaluation 使用相同 case set。
- [ ] 记录 false positive、false negative 和 residual risk。

Evidence：

- CUDA reviewer known-defect evaluation。
- Runtime Lab Assistant 20+ eval cases。
- 一份 tuning before/after report。

## 5. Orchestrate multi-agent coordination（15–20%）

- [ ] 为任务选择 sequential、parallel、handoff 或 reviewer pattern。
- [ ] parallel agents 使用隔离 scope，避免重叠 edits。
- [ ] 检测 duplicated work、overlapping changes 和 contradictory outputs。
- [ ] 记录 handoff、decisions 和 completion evidence。
- [ ] 识别 failed、partial、stalled 或 degraded execution。
- [ ] 定义 retry、rollback 和 human-in-the-loop recovery。
- [ ] 增加、替换、retire agent 时保留 auditability。

Evidence：

- 一个隔离的 parallel/reviewer fixture。
- conflict simulation 和 recovery record。
- explicit agent lifecycle/runbook。

## 6. Implement guardrails and accountability（10–15%）

- [ ] 按 operational/security/compliance risk 划分 autonomy levels。
- [ ] 明确哪些 actions 需要 human judgment。
- [ ] block 违反 policy 的 actions。
- [ ] permissions 和 execution context 遵循 least privilege。
- [ ] irreversible/compliance-sensitive change 需要 explicit authorization。
- [ ] 不给低风险 read-only 行为增加无意义审批。
- [ ] 对 issue、PR、logs、web、MCP output 做 prompt-injection threat model。
- [ ] remote MCP 验证 token audience，禁止 token passthrough，保护 secrets。

Evidence：

- action risk/approval matrix。
- MCP threat model。
- prompt injection/tool misuse eval。
- audit log with who/what/why/result。

## Monthly Check

每四周：

- [ ] 为本月覆盖域链接至少一个真实 artifact。
- [ ] 写 5 个场景题并解释错误选项。
- [ ] 标出 study guide 中仍没有 hands-on evidence 的条目。
- [ ] 检查产品/考试内容是否更新。

## Exam Readiness

- [ ] 六个 domains 均有项目 evidence。
- [ ] 两次独立、自编且明确标注“非官方”的 timed mock 达到稳定阈值。
- [ ] 错题按 root cause 分类并完成复测。
- [ ] 能在没有术语提示时解释 MCP、state、eval、multi-agent 和 guardrails。
- [ ] 考前一周重新核对官方 study guide、course 和 exam availability。

官方 exam sandbox 只用于熟悉考试界面，不能当 practice assessment 或 readiness
分数。Certification readiness 与 Runtime G1–G8 毕业结论分开。
