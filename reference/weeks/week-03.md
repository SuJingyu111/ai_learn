# Week 03 — 线性代数、MatMul 与 Agent Fundamentals

> **Unit 1 · ML Systems Foundations（第 3/6 周）**  
> **课程角色**：把 tensor layout 连接到第一个核心算子，并为 M1 增加 broadcast、reduction 与 channel-wise normalize；sidecar 从确定性 workflow 进入 agent 的目标、工具、环境和控制循环。

## 开始前：硬先修与 remediation

- **硬先修**：能解释二维 shape/stride，并熟练使用 Python loop 与 assertions。
- **先修检查**：写出 `(2×3) @ (3×4)` 的 output shape；若不是 `2×4`，先补内维相等与 dot product。
- **失败时 remediation**：用长度 3 的两个 vector 手算 dot product；再用 `1×3 @ 3×1` 验证。30 分钟内未通过则只做二维 MatMul，不做 batch。

## Observable objectives

- 解释 vector、matrix、dimension、dot product、MatMul 与 batch MatMul。
- 手算小矩阵乘法并由三重 loop reference 验证。
- 推导 broadcast 后的 shape、指定 axis 的 reduction shape/value，并识别 incompatible shapes。
- 实现 channel-wise `(x-mean)/std` normalize，拒绝 channel mismatch 与 zero std。
- 计算 MatMul 的乘加次数、tensor bytes 与 arithmetic-intensity 粗略比值。
- 区分 agent、workflow、tool，并为 agent task 写 success criteria/non-goals。

## 术语表

需能解释：**vector、matrix、dimension、dot product、inner dimension、MatMul/GEMM、batch、broadcast、reduction、axis、mean、normalization、FLOP、byte traffic、agent、workflow、tool、environment、success criterion**。

## 必读理论材料

- **T1（Day 1，25 分钟）**：[Dive into Deep Learning: Linear Algebra](https://d2l.ai/chapter_preliminaries/linear-algebra.html) — Scalars through matrix-vector products，并读 reductions/means。
- **T2（Day 2，15 分钟）**：[Dive into Deep Learning: Data Manipulation](https://d2l.ai/chapter_preliminaries/ndarray.html) — Broadcasting。
- **T3（Day 2，15 分钟）**：[NumPy `matmul`](https://numpy.org/doc/stable/reference/generated/numpy.matmul.html) — signature、stack behavior、errors。
- **S1（Day 1，20 分钟）**：[Foundations of Agentic AI in GitHub](https://learn.microsoft.com/en-us/training/modules/foundations-agentic-ai/) — “Define agentic AI in the SDLC” 与 plan→act→evaluate。
- **S2（Day 3，20 分钟）**：[GitHub Copilot custom agents: About custom agents](https://docs.github.com/en/copilot/concepts/agents/copilot-cli/about-custom-agents) — profile、tools、task specialization。

95 分钟已计入 Day 1–3。Podcast/blog 只可作为可选 seminar。

## Optional Feed Card（30–45 分钟，不计入每日 90–120 分钟）

- **30–45 分钟；先修或 Gate 落后时取消**：从 [Latent Space Podcast](https://www.latent.space/podcast) 选择一集近 12 个月、主题含 coding agents、agent architecture 或 tool use 的节目；优先选择附 transcript/show notes 的一集。
- 在 `notes/feed/week-03.md` 写 `claim → source/evidence → scope → experiment/decision`，并明确主持人/嘉宾的观点不能替代 GitHub 产品保证。可散步听，不计 unlock。

## 带问题阅读

- `M×K` 和 `K×N` 中 K 为什么必须一致？
- broadcast 与 reduction 分别改变哪些 axis，为什么不是同一种操作？
- NumPy stack/batch 语义与简单二维公式差在哪里？
- channel-wise normalize 中 mean/std 的 shape 如何与 HWC channels 对齐？
- 什么时候固定 workflow 已足够，何时才需要 agent 自主选择下一步？
- 模糊完成条件为何是 agent anti-pattern？

## Worked example / 手算

`A=[[1,2,3],[4,5,6]]`，`B=[[1,0],[0,1],[1,1]]`。`C=A@B`：`C00=1×1+2×0+3×1=4`，`C01=1×0+2×1+3×1=5`，第二行得到 `[10,11]`，所以 `C=[[4,5],[10,11]]`，shape=`2×2`，共 `2×2×3=12` 次 multiply-add terms。

对一个 HWC pixel `x=(10,20,30)`、channel mean=`(1,2,3)`、std=`(3,6,9)`，broadcast 后逐 channel normalize 为 `((10-1)/3,(20-2)/6,(30-3)/9)=(3,3,3)`；reduction `sum(axis=channel)` 得 `9`。这一步不包含 Resize。

## 累计项目 increment

- `labs/ml/week03_matmul.py`
- `labs/ml/week03_normalize.py`
- `notes/concepts/matmul-data-movement.md`
- `notes/ai-app/week-03-agent-fundamentals.md`
- `notes/ai-app/week-03-agent-cases.md`

## Exit ticket / unlocks

**形成性**：闭卷手算一个 `2×3 @ 3×2`、一个 broadcast/reduction case 和一组 channel normalize；reference/framework 一致、bytes/FLOPs 表完整，并正确分类 3 个 agent/workflow/tool use cases。通过 unlock Week 04。

## Day 1

**阶段：理论 + 概念模型（100 分钟）**

- **25 分钟**：读 T1，回答 scalar/vector/matrix operations 的 shape。
- **25 分钟**：逐步完成顶部 worked example，再独立做一组带负数的矩阵。
- **30 分钟**：用纸面/REPL 诊断 dot product、sum/mean reduction、transpose 与 shape error。
- **20 分钟**：读 S1，写 agent task 的 inputs/outputs/success/non-goals 四格。
- **必须留下**：两组手算、shape 诊断和 agent task contract。

## Day 2

**阶段：worked example → guided implementation（100 分钟）**

- **15 分钟**：读 T2，手算 `2×1 + 1×3` 的 broadcast output。
- **15 分钟**：读 T3，记录二维与 stacked arrays 的差异。
- **25 分钟**：把 MatMul worked example 展开为 i/j/k 三层 loop trace，并标出合法 reduction axis。
- **30 分钟**：guided lab：实现 `matmul_reference(A,B)`，与 NumPy/PyTorch 比较，并 assert row/column reductions 与 incompatible shapes。
- **15 分钟**：把固定 reference run 分类为 workflow/tool，写为何不需要 agent。
- **必须留下**：broadcast/reduction 手算、loop trace、reference、assertions 与分类依据。

## Day 3

**阶段：guided lab（100 分钟）**

- **40 分钟**：扩展到多组 M/K/N；生成 output shape、multiply-add terms、input/output bytes 表，固定小整数验证。
- **20 分钟**：读 S2，标出 model/instructions/tools/environment 与人类 control。
- **30 分钟**：guided 实现 channel-wise normalize 的 loop reference 与 vectorized version；覆盖 wrong-channel 和 zero-std failure。
- **10 分钟**：记录命令、版本和首个错误。
- **必须留下**：case table、normalize reference/tests 和 agent component map。

## Day 4

**阶段：independent lab（100 分钟）**

- **55 分钟**：独立加入 batch MatMul 或明确不支持的 validation；覆盖 `1×1`、odd K、zero/invalid dimension 和 fixed-seed random。
- **15 分钟**：独立完成两个 broadcast/reduction variants，先写 expected shape/value 再运行。
- **20 分钟**：设计 read-only reviewer control plane：输入只允许 diff/test output，禁止 edit/merge/secrets。
- **10 分钟**：输出 first mismatch 与 tolerance（整数应 exact）。
- **必须留下**：independent cases、boundary contract 和 reviewer scope。

## Day 5

**阶段：integration + formative review（100 分钟）**

- **60 分钟**：整合 MatMul、broadcast/reduction 与 normalize correctness 和 bytes/FLOPs worksheet；说明粗略 traffic 为什么不等于 profiler measurement。
- **20 分钟**：写 5 个 agent fundamentals scenarios，解释错误选项。
- **20 分钟**：完成 Exit ticket 初测与错因分类。
- **必须留下**：整合报告、5 个 scenarios 和 exit evidence。

## Day 6

**阶段：review / unlock（100 分钟）**

- **40 分钟**：clean-run MatMul、broadcast/reduction、normalize cases，并闭卷重做一题。
- **30 分钟**：完成 `matmul-data-movement.md`，串起 shape→broadcast/reduction→loop→normalize→FLOPs/bytes。
- **15 分钟**：用 90 秒解释 agent/workflow/tool 边界。
- **15 分钟**：填写 `notes/weekly/week-03.md` 与 Week 04 Conv shape 预习题。
- **必须留下**：形成性 Exit ticket、clean output、90 秒提纲和 unlock 状态。
