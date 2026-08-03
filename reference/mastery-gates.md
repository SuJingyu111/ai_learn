# Mastery Gate、评分与重修规则

## 为什么 Gate 是课程的一部分

本课程的内容有真实依赖。不会 stride/address 就无法判断 CUDA memory access；没有 CPU
oracle 就无法判断 GPU 优化是否仍正确；没有 stream/ownership 心智模型就无法安全地
做 device-resident runtime integration。因此“完成了 6 周”不等于“可以进入下一单元”。

## 两种评估

### Weekly Exit Ticket

Week 1–5、7–11 等非 Gate 周使用形成性评估：

- 5 道闭卷短题，首次至少 4/5；
- 1 个 90 秒口述题；
- 1 个本周 lab 的新输入；
- 术语表中随机抽 3 个，必须给出自己的定义和一个反例；
- 未通过项在下一周 Day 1 前订正并用新题复测。

Exit Ticket 不单独解锁下个 Unit，但遗漏的 prerequisite 会在 Major Gate 重新出现。

### Major Gate

Week 6、12、18、24、30、36、42、48 使用统一 rubric：

| 部分 | 权重 | 评估方式 |
|---|---:|---|
| Knowledge | 25% | 45–60 分钟闭卷、新题；定义、推导、机制和 failure case |
| Unseen practical | 30% | 新 input/spec/bug，不得打开 reference solution |
| Project milestone | 30% | clean build/run、tests、raw evidence、设计与限制 |
| Oral/design defense | 15% | 5–10 分钟讲解 + 随机追问 |

表中的 25/30/30/15 是**分值，不是分钟**。一个 Major Gate 使用 Week 的 Day 5–6
完成：Knowledge 必须保留连续 45–60 分钟；其余时间用于 unseen practical、clean
project audit、oral defense 和记分。每天仍不超过 120 分钟；较复杂的 G6–G8 明确拆成
两日 scored assessment，不能把四项硬塞进 70 分钟。

通过要求：

1. 加权总分 `>= 80/100`；
2. 下列 hard blockers 全部为 PASS；
3. Knowledge 首次至少 80%；订正后所有题 100%；
4. Mandatory correctness/security tests 100% 通过，不能用高分抵消。

## 通用 hard blockers

- 输出不正确，或 tolerance/semantic contract 未冻结；
- OOB、race、use-after-free、unobserved async error；
- benchmark 没有环境、warm-up、迭代数、raw samples 或 timed-region 定义；
- clean checkout 无法复现；
- MCP/agent 暴露 arbitrary shell/path、secret 或未授权写操作；
- evidence 无法关联 commit、环境、输入和工具版本；
- 直接使用已暴露的 reference solution 完成 summative practical。

## 各 Gate 解锁表

| Gate | 必须证明 | 解锁 | 典型 blocker |
|---|---|---|---|
| G1 / W6 | stride/address、MatMul/Conv、CPU tensor transforms | U2 | 地址/shape/CPU oracle 错 |
| G2 / W12 | PyTorch→ONNX→ORT CPU、contract、measurement | U3 | export/tolerance/clean run 失败 |
| G3 / W18 | independent CUDA transform、errors、sanitizer | U4 | odd size、OOB/race、错误不可见 |
| G4 / W24 | Resize semantics、CPU/CUDA、profiler case study | U5 | border/coordinate 不一致、无 raw evidence |
| G5 / W30 | stream/event/lifetime、500–1000 frame pipeline | U6 | 隐式同步、race、shutdown 不可靠 |
| G6 / W36 | ORT CUDA/TRT、device I/O、FP16/dynamic | U7 | fallback、host round-trip、lifetime 错 |
| G7 / W42 | Orin sustained beta、power/thermal、failure/MCP | U8 | measurement invalid、security regression |
| G8 / W48 | clean release、3 reports、defense | Graduation | 不可复现、evidence 断链 |

## 评分细则

### Knowledge，25 分

- 20–25：能准确解释机制、推导结果、识别适用条件与反例；
- 16–19：核心正确，存在局部术语或边界错误；
- 0–15：依赖记忆结论，无法推导或混淆邻近概念。

### Unseen practical，30 分

- 27–30：独立完成，包含边界 tests、first-error diagnosis 和清晰 evidence；
- 24–26：正确完成，但诊断或证据不完整；
- 18–23：需关键提示，或只覆盖 happy path；
- 0–17：结果错误、不安全、不可复现或使用 reference answer。

### Project milestone，30 分

- Correctness/contract：10；
- engineering/lifetime/error handling：7；
- measurement/raw evidence：7；
- reproducibility/documentation：6。

任何 critical correctness/security blocker 会令整个 milestone 不通过。

### Oral/design defense，15 分

- mechanism/dataflow：5；
- evidence and trade-off：4；
- failure/limitation：3；
- follow-up questions：3。

只复述术语而无法指向代码、trace 或 raw data，不得超过 8 分。

优先请同事、mentor 或社区 reviewer 现场追问。若只能自学：先冻结 submission，再让
AI/题库基于 rubric 生成 3–5 个未预演追问；全程录音或保存 transcript，闭卷作答，
最后逐项链接证据并保守评分。生成追问的对话不得提前给答案，也不能由学习者事后删除
答不出的题。

## 结果与重修

| 结果 | 条件 | 下一步 |
|---|---|---|
| PASS | 总分≥80，所有 blockers PASS | 解锁下一 Unit |
| CONDITIONAL | 总分70–79，且无 critical blocker | 1 个 remediation week；新题/new input 重测 |
| RETAKE | <70 或存在 critical blocker | 重做本 Unit 的最后 2 周，再完整重测 |

重测不能重复原题或只重跑相同输入。每次 Gate 在
`notes/gates/gate-N.md` 记录：

- date、commit、host/Orin environment；
- assessment variant ID；
- 分项得分与 hard-blocker checklist；
- code、tests、raw data、trace、report 的路径；
- `PASS / CONDITIONAL / RETAKE`；
- remediation 内容、重测日期和新 variant。

使用 [`notes/templates/gate-report.md`](../notes/templates/gate-report.md)。

## Assessment integrity

- Formative worked example 可以看提示；summative practical 不可以。
- 自己编写并已看过答案的题只能练习，不能成为最终 Gate 的唯一依据。
- `src/cpu/resize_cpu.cpp`、`src/cuda/resize_cuda.cu` 是现存 reference implementation。
  G4 前的 Resize milestone 应在 `labs/resize_starter/`、个人 namespace 或独立 starter
  branch 完成；Gate 通过后才做 reference comparison。
- 若没有 instructor，可在一周前让 AI 生成**只含输入/输出 contract 和 hidden-like
  tests、不含实现**的新 variant；考试时关闭对话并记录 prompt/version。
- AI 可以用于学习、解释和 formative debugging；Gate report 必须披露使用范围。

## GH-600 readiness 与课程毕业分离

GH-600 当前考试内容和产品状态会变化。副线采用相同 evidence discipline，但：

- Runtime graduation 看 G1–G8；
- Certification readiness 单独看六域 coverage、两次 timed self-authored mock 的稳定性
  以及最新官方 Study Guide；
- 自编 mock 必须标注“非官方”，官方 exam sandbox 只用于熟悉界面；
- preview feature 无法访问时，用本地、可重复的等价 fixture 评估概念，不阻塞主线毕业。
