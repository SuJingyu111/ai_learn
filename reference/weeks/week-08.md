# Week 08 — Computational Graph / Autograd Mental Model 与 Agent Eval Baseline

> **Unit 2 · Model-to-Runtime CPU Vertical Slice（第 2/6 周）**  
> **课程角色**：理解 framework 如何记录计算与求导，但把训练限制为理解 inference graph 的手段；sidecar 用 frozen defects 建立 agent evaluation baseline。

## 开始前：硬先修与 remediation

- **硬先修**：能解释 Module/state/checkpoint/eval，能做代数代入与幂运算；Week 07 agent eval manifest/rubric 已冻结。本周不预设微积分基础。
- **桥接诊断**：尝试对 `y=(2x+1)^2` 写局部步骤；不会求导只记录 `not-yet-learned`，不得猜答案或阻塞 Day 1 的 scalar bridge。
- **失败时 remediation**：完成 T0 与顶部 worked example，只做 scalar/local-derivative graph；能用 chain rule 复算后再进入 tensor backward。

## Observable objectives

- 画出 forward graph、leaf/intermediate、loss 与 backward dependency。
- 从 scalar derivative、local derivative 开始手算简单 chain rule，并与 autograd gradient 一致。
- 解释 dynamic graph、`requires_grad`、gradient accumulation、detach/no-grad。
- 按 scaffold 写 `forward→loss→zero_grad→backward→step`，能说明每步但明确训练非项目主线。
- 构建 representative reviewer eval set 并报告 false positive/negative。

## 术语表

需能解释：**computational graph、node、edge、leaf、forward、backward、chain rule、gradient、requires_grad、accumulation、detach、dynamic graph、eval case、rubric、false positive、false negative**。

## 必读理论材料

- **T0（Day 1，25 分钟）**：[Dive into Deep Learning: Calculus](https://d2l.ai/chapter_preliminaries/calculus.html) — derivative、partial derivative 与 chain rule；只学本周 scalar graph 所需部分。
- **T1（Day 2，20 分钟）**：[PyTorch: Automatic Differentiation](https://docs.pytorch.org/tutorials/beginner/basics/autogradqs_tutorial.html) — tensors/functions/computational graph。
- **T2（Day 2，20 分钟）**：[PyTorch Autograd mechanics](https://docs.pytorch.org/docs/stable/notes/autograd.html) — graph recording、saved tensors、grad modes。
- **T3（Day 3，20 分钟）**：[PyTorch: Optimizing Model Parameters](https://docs.pytorch.org/tutorials/beginner/basics/optimization_tutorial.html) — loss、optimizer、`zero_grad/backward/step` loop scaffold。
- **S1（Day 1，20 分钟）**：[Foundations of Agentic AI in GitHub](https://learn.microsoft.com/en-us/training/modules/foundations-agentic-ai/) — plan→act→evaluate、traceability 与 failure evidence。
- **S2（Day 3，15 分钟）**：[GitHub custom agents](https://docs.github.com/en/copilot/concepts/agents/copilot-cli/about-custom-agents) — specialization 与 invocation boundary 复核。

120 分钟计入 Day 1–3；podcast/blog 仅 optional seminar。

## Optional Feed Card（30–45 分钟，不计入每日 90–120 分钟）

- **30–45 分钟；先修或 Gate 落后时取消**：从 [Latent Space Podcast](https://www.latent.space/podcast) 选择一集近 12 个月、主题为 evals、agent reliability 或 error analysis 的节目；必须有 transcript/show notes，且不能只凭嘉宾资历接受 claim。
- 在 `notes/feed/week-08.md` 写 `claim → source/evidence → scope → experiment/decision`，将一个可测 claim 转为新 eval fixture，不事后修改 frozen expected labels。不计 unlock。

## 带问题阅读

- backward 为什么依赖 forward 保存的中间状态？
- chain rule 如何把每个 local derivative 连接成最终 gradient？
- gradient accumulation 为何要求显式清零？
- `eval()` 是否关闭 autograd？
- `zero_grad→backward→step` 顺序中每一步改变什么 state？
- agent eval 为什么必须冻结 inputs/expected behavior，而非凭“看起来不错”？

## Worked example / 手算

令 `x=2`，`a=2x+1=5`，`y=a²=25`。局部导数 `dy/da=2a=10`、`da/dx=2`，所以 `dy/dx=20`。连续调用两次 `backward()` 若不清零，leaf gradient 会累加为 40；这不是第二次真实导数变成 40。

## 累计项目 increment

- `labs/ml/week08_autograd_graph.py`
- `notes/concepts/computational-graph-autograd.md`
- `ai-app/evals/week-08-reviewer-baseline.md`
- `notes/ai-app/week-08-eval-analysis.md`

## Exit ticket / unlocks

**形成性**：闭卷 scalar derivative/chain-rule 题、graph/gradient lab 与 scaffolded optimization step 通过，能解释 eval≠no-grad；reviewer 至少 8 个 frozen cases，有 raw outputs 和 failure taxonomy。通过 unlock Week 09。

## Day 1

**阶段：理论 + graph model（100 分钟）**

- **25 分钟**：读 T0，写 derivative、local derivative、chain rule 的自己的定义。
- **25 分钟**：逐步完成顶部 worked example并画 graph。
- **30 分钟**：guided scalar graph：先手算三个 local derivatives，再用最小 tensor 检查 gradient。
- **20 分钟**：读 S1，定义 agent eval outcome/constraints/signals。
- **必须留下**：scalar derivative bridge、手算 graph、autograd comparison 和 eval fields。

## Day 2

**阶段：worked mechanisms（100 分钟）**

- **20 分钟**：读 T1。
- **20 分钟**：读 T2。
- **20 分钟**：手算 two-branch graph 的 gradient contribution。
- **30 分钟**：验证 accumulation、zeroing、detach、no-grad/inference；记录 expected/actual。
- **10 分钟**：冻结 reviewer scoring rubric。
- **必须留下**：two-branch derivation、mode table 和 rubric。

## Day 3

**阶段：guided labs（100 分钟）**

- **20 分钟**：读 T3，标注 loss、optimizer 与四步 update loop 的 state。
- **45 分钟**：从提供的 scaffold 实现最小 `forward→loss→zero_grad→backward→step`，仅跑 20–50 steps；assert parameter/gradient 变化后停止训练扩展。
- **15 分钟**：读 S2。
- **10 分钟**：建立 8-case reviewer dataset：correctness、tests、benchmark、benign、injection。
- **10 分钟**：记录 source identity/version。
- **必须留下**：最小 lifecycle、8-case manifest 和非主线边界。

## Day 4

**阶段：independent lab（100 分钟）**

- **70 分钟**：独立构造 shared subgraph 或 branch case，手算并与 autograd 比较；加入 stale grad/forgotten zero failure。
- **20 分钟**：运行可用 reviewer 或标记 `not run`，保存原始 outputs，不人工改答案。
- **10 分钟**：输出 first gradient mismatch。
- **必须留下**：独立 graph case、failure reproduction 和 raw eval outputs。

## Day 5

**阶段：integration + formative review（100 分钟）**

- **60 分钟**：完成 graph/autograd concept note，连接 Module mode 与 ONNX inference graph 前置问题。
- **20 分钟**：计算 reviewer recall/false positives/untested，分类 instruction/tool/context/environment。
- **20 分钟**：完成 Exit ticket 初测。
- **必须留下**：concept note、eval metrics/failure taxonomy 和 exit gaps。

## Day 6

**阶段：review / unlock（100 分钟）**

- **45 分钟**：用新输入复测 chain rule、accumulation 与 grad modes。
- **25 分钟**：修正 eval rubric/fixtures但冻结 v1，不根据输出改 expected。
- **15 分钟**：闭卷解释 graph 与 agent eval 的共同“可观察 evidence”边界。
- **15 分钟**：填写 `notes/weekly/week-08.md` 与 Week 09 export contract。
- **必须留下**：形成性 Exit ticket、frozen eval v1、clean evidence 和 unlock 状态。
