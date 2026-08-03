# GH-600 实践与 Evidence 清单

本清单按 2026-05-13 更新的官方 GH-600 Study Guide 整理。考试前必须重新访问
[最新 Study Guide](https://learn.microsoft.com/en-us/credentials/certifications/resources/study-guides/gh-600)；
下面的权重和条目不能替代最新版考纲。

本文件只用于**累计 evidence 和检查 readiness**，不是教材，也不定义学习顺序。
实际动手在 [`../STEPS.md`](../STEPS.md) 的 **Phase B**——那条序列建的是一个真实的
只读 MCP server，六域的 evidence 是它的副产物，不是另外做的作业。

**认证与工程能力是两个独立结论。** 引擎跑通、跑对、跑快，和你考不考这张证没有
关系；反过来也一样。这份清单只回答「六域各有没有真东西可指」。

GH-600 当前面向在 production-grade SDLC 中运行、集成、监督和治理 AI agents 的
工程师，并明确要求 GitHub Copilot、MCP servers、custom instructions、custom
agents、tools 和 Copilot setup 的实践经验。

正式学习材料是 Microsoft Learn 的 agent modules 与 GitHub/MCP 官方文档，
Study Guide 只定义考纲、不承担教学。顺序见
[`../reference/AI_APP_TRACK.md`](../reference/AI_APP_TRACK.md)。

**什么时候开始**：Phase 09 结束之后。在那之前引擎没有真实的 benchmark 产物，
MCP server 无米可炊，六域的 evidence 也只能是玩具。

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

## 定期自查

没有固定周期——**每完成一段 Phase B 就回来一次**：

- [ ] 为这段覆盖到的域链接至少一个真实 artifact（代码、trace、test、audit log）。
- [ ] 写 5 个场景题并解释错误选项**错在哪**。
- [ ] 标出 Study Guide 里仍然没有 hands-on evidence 的条目。
- [ ] 检查产品与考纲是否更新——这一条最容易过期。

## Exam Readiness

- [ ] 六个 domains 均有项目 evidence。
- [ ] 两次独立、自编且明确标注“非官方”的 timed mock 达到稳定阈值。
- [ ] 错题按 root cause 分类并完成复测。
- [ ] 能在没有术语提示时解释 MCP、state、eval、multi-agent 和 guardrails。
- [ ] 考前一周重新核对官方 study guide、course 和 exam availability。

官方 exam sandbox 只用于熟悉考试界面，不能当 practice assessment 或 readiness 分数。

自编 mock 必须标注「自编、非官方」。不使用 exam dump、题库，或任何声称能预测
分数的东西——那既违反考试伦理，也会让你误判自己的真实水平。
