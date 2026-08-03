# Week 01 — 入学诊断、Array/Tensor 与 Git/GitHub Flow

> **Unit 1 · ML Systems Foundations（第 1/6 周）**  
> **课程角色**：诊断周 + 基础数据模型。它建立后续 stride、operator、runtime 的共同语言；sidecar 从可审计的 Git/GitHub Flow 起步。总课程定位见 [CURRICULUM](../CURRICULUM.md)。

## 开始前：硬先修与 remediation

- **硬先修**：能在终端运行 `python3 --version`，理解变量、循环、函数和整数索引；能执行 `git --version`。
- **10 分钟诊断**：在 `notes/diagnostics/entry.md` 记录 Python/NumPy/Git 是否可用，不隐瞒 import error。
- **失败时 remediation**：Python 不可用时先完成纸面 shape/index 题，并按 [NumPy 安装说明](https://numpy.org/install/) 修复；Git 不可用时只做纸面 flow，不创建远端仓库。环境修复最多占 Day 1 的 35 分钟，仍失败则保留 exact command/error。PyTorch 本周可暂缺，但必须按 [getting started](../docs/getting-started.md#4-第一次-ml-baseline) 在 Week 2 Exit Ticket 前通过四包 import/version smoke test。

## Observable objectives

- 从具体 array 说出 shape、rank、axis、dtype、element count 和 bytes。
- 用固定输入验证二维 indexing 与 flatten offset。
- 区分 NumPy array、PyTorch tensor 和逻辑 tensor contract。
- 运行一个带 assertions 的最小 Python lab，并保存可复现命令。
- 用自己的话解释 GitHub Flow 中 branch、commit、pull request、review 的责任边界。

## 术语表

本周结束时需能不用原文解释：**array、tensor、scalar、rank、shape、axis、dtype、element、repository、branch、commit、pull request**。

## 必读理论材料

- **T1（Day 1，20 分钟）**：[NumPy: The absolute basics for beginners](https://numpy.org/doc/stable/user/absolute_beginners.html) — “How to create a basic array” 至 shape/size/ndim。
- **T2（Day 2，20 分钟）**：[PyTorch: Tensors](https://docs.pytorch.org/tutorials/beginner/basics/tensorqs_tutorial.html) — Initialization、Attributes、Indexing/Slicing。
- **S1（Day 1，20 分钟）**：[GitHub Flow](https://docs.github.com/en/get-started/using-github/github-flow) — 全页，关注 branch→PR→review→merge。
- **S2（Day 2，15 分钟）**：[Pro Git: Recording Changes](https://git-scm.com/book/en/v2/Git-Basics-Recording-Changes-to-the-Repository) — status、staging、commit 段落。

以上 75 分钟已计入 Day 1–2；[resources](../docs/resources.md) 是索引而非额外作业。Podcast/blog 仅为可选 seminar，不替代教材、不计解锁。

## Optional Feed Card（30–45 分钟，不计入每日 90–120 分钟）

- **30–45 分钟；先修或 Gate 落后时直接取消**：从 [MLOps Community Podcast](https://mlops.community/podcast) 选择一集与 reproducibility、ML systems 入门或工程学习有关的节目；优先选择近 12 个月且有 show notes 的一集。
- 可在散步时听；在 `notes/feed/week-01.md` 只写 `claim → source/evidence → scope → experiment/decision`，没有可验证 evidence 时写“仅观点”。不影响本周 unlock。

## 带问题阅读

- `shape=(2,3)` 究竟描述逻辑结构还是物理内存？
- dtype 改成 FP16 后，哪些量改变、哪些语义不应改变？
- GitHub Flow 中谁产生 evidence，谁有合并决定权？
- commit、PR 与 review 为什么不是同一种 artifact？

## Worked example / 手算

对 FP32 array `[[0,1,2],[3,4,5]]`：rank=`2`，shape=`(2,3)`，elements=`6`，bytes=`6×4=24`；row-major 下 `(1,2)` 的 flat offset=`1×3+2=5`，value=`5`。本周代码必须复现这五个答案。

## 累计项目 increment

- `notes/diagnostics/entry.md`
- `labs/ml/week01_array_tensor.py`
- `notes/concepts/array-tensor-basics.md`
- `notes/ai-app/week-01-github-flow.md`
- `artifacts/environment/host-learning-freeze.txt`（Week 1 可先留 blocked log，Week 2 必须完成）

## Exit ticket / unlocks

**形成性，不计硬门**：闭卷回答 5 个术语、手算一个 `(row,col)` offset、运行全部 assertions，并画 GitHub Flow。全部完成即 unlock Week 02；失败项在 Week 02 Day 1 前 30 分钟 remediation。评分与后续硬门口径见 [mastery gates](../docs/mastery-gates.md)。

## Day 1

**阶段：理论 + 入学诊断（100 分钟）**

- **20 分钟**：阅读 T1，回答 shape/size/ndim 的区别。
- **25 分钟**：完成 Python/NumPy/terminal 诊断与 8 题基线小测，结果原样写入 entry note。
- **35 分钟**：guided lab：创建 `week01_array_tensor.py`，构造 scalar、1D、2D、`1×3×4×5` arrays，打印 metadata。
- **20 分钟**：阅读 S1，画 GitHub Flow actor/artifact 草图；不创建远端资源。
- **必须留下**：诊断表、脚本首版、运行命令与 flow 草图。

## Day 2

**阶段：worked example（100 分钟）**

- **20 分钟**：阅读 T2，只记录 tensor attributes/indexing 与 NumPy 的异同。
- **25 分钟**：独立重做顶部 worked example，再做 `3×4` INT32 array 的 bytes 与 3 个 offsets。
- **40 分钟**：把手算变成 assertions；加入 wrong-rank 与 out-of-range 的受控失败。
- **15 分钟**：阅读 S2，用 `working tree→staging→commit` 解释本地 evidence lifecycle。
- **必须留下**：两组手算、assertions、受控错误和 Git artifact 对照表。

## Day 3

**阶段：guided lab（100 分钟）**

- **15 分钟**：闭卷回忆术语并标记不确定项。
- **55 分钟**：扩展 lab：固定 seed，比较 NumPy/PyTorch 可用时的 shape/dtype/device/value；无 PyTorch 时保留 import error，不换在线 notebook。
- **20 分钟**：在 `week-01-github-flow.md` 为“修复一个 assertion”写 issue→branch→commit→PR→review 的本地模拟。
- **10 分钟**：记录 exact commands、版本和首个错误假设。
- **必须留下**：跨库对照或 blocked reproduction、完整 flow fixture。

## Day 4

**阶段：independent lab（100 分钟）**

- **10 分钟**：先写 input/output contract。
- **65 分钟**：独立实现 `describe_tensor(x)`，返回 rank/shape/dtype/elements/bytes；覆盖 scalar、empty、odd shape 和 wrong input。
- **15 分钟**：给 flow fixture 加“CI fail”和“review 要求修改”两条回路，明确人类保留 merge 权。
- **10 分钟**：clean process 重跑并保存 output。
- **必须留下**：独立函数、boundary tests、两条 recovery path。

## Day 5

**阶段：integration + formative review（100 分钟）**

- **60 分钟**：整合脚本与概念笔记；随机抽 5 个坐标，先手算再由程序验证，报告首个 mismatch。
- **20 分钟**：写 3 个 Git/GitHub Flow 场景题：未测试 commit、自批自合、CI fail 后 merge，并解释正确边界。
- **20 分钟**：完成 Exit ticket 初测；错项写“原答案→证据→修正”。
- **必须留下**：整合运行记录、3 个 sidecar 场景题、Exit ticket 初测。

## Day 6

**阶段：review / unlock（100 分钟）**

- **30 分钟**：修复 Exit ticket 错项并 clean-run 全部 assertions。
- **20 分钟**：完成 `array-tensor-basics.md`：Mechanism→Worked example→Evidence→Limitation。
- **15 分钟**：闭卷重画 GitHub Flow，与 Day 1 图做 diff。
- **25 分钟**：按 [getting started](../docs/getting-started.md#4-第一次-ml-baseline)
  创建 host learning venv，开始四包安装并做 import/version smoke；下载超时或兼容失败时
  保存 exact log，作为 Week 2 的第一项 remediation。
- **10 分钟**：填写 `notes/weekly/week-01.md`，写 Week 02 第一项 remediation 或 stride 问题。
- **必须留下**：形成性 Exit ticket 终版、clean command、图的 diff、environment
  smoke/failure log 和 unlock 状态。
