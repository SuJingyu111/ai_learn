# Week 20 — Unit 4：Transpose、Reduction、Shared Memory 与 Occupancy

## Unit / 课程角色

- **Unit 4 · Image Operator Case Study，第二周。** 用两个小而完整的 kernels 建立 memory access、shared memory、synchronization、occupancy 之间的因果模型。
- 课程导航：[总纲](../CURRICULUM.md) · [官方资料索引](../docs/resources.md) · [Mastery Gates](../docs/mastery-gates.md)。
- **Sidecar 依赖位置**：在 Week 19 Resources 之上加入 user-controlled MCP Prompt；server 仍 read-only。

## 硬先修与 Remediation

- **硬先修**：Week 19 trace 可复现；能画 warp contiguous/strided addresses；能解释 `__syncthreads()` 的 block scope。
- **开周诊断（15 分钟）**：为 `32×32` row-major transpose 画一个 warp 的 read/write addresses；手画 block size 8 reduction tree。
- **未通过时**：回 Week 13–16 的 coalescing/shared-memory notes；先完成 `4×4` transpose 和 8-element reduction 手算，不允许直接复制 tiled sample。

## Observable Objectives

1. 实现 CPU oracle、naive/tiled transpose，覆盖 odd/non-tile dimensions。
2. 实现 CPU double-accumulation oracle、naive/shared reduction，解释 tolerance 与 non-associativity。
3. 用 Compute Sanitizer 检查 OOB/race/sync，并用 profiler evidence验证 memory hypothesis。
4. 解释 occupancy 是资源约束下的 resident-warps 指标，不把更高 occupancy 等同更高性能。
5. 实现 `profiling_review` MCP prompt：显式用户选择、参数校验、只生成 evidence-first review messages。

## 术语表

| 术语 | 本周可操作定义 |
|---|---|
| coalescing | warp memory addresses 被较少 memory transactions 服务的访问性质 |
| tile | block 协作加载到 shared memory 的局部数据区域 |
| bank conflict | 同一 warp 对 shared-memory banks 的冲突访问 |
| barrier | block 内线程到达并建立 shared-memory visibility 的同步点 |
| non-associativity | floating-point 加法顺序改变可能改变结果 |
| occupancy | active warps 相对硬件上限的比例，受 registers/shared/block size 限制 |
| MCP prompt | user-controlled、可发现/获取的参数化消息模板 |

## 必读理论材料（时间已计入对应 Day）

| Day | 材料与精确章节 | 分钟 |
|---:|---|---:|
| 1 | [CUDA Best Practices — Coalesced Access to Global Memory](https://docs.nvidia.com/cuda/cuda-c-best-practices-guide/index.html#coalesced-access-to-global-memory)，读 sequential/misaligned/strided access | 20 |
| 1 | [CUDA Programming Guide — Writing CUDA SIMT Kernels](https://docs.nvidia.com/cuda/cuda-programming-guide/02-basics/writing-cuda-kernels.html)，读 “On-chip Shared Memory”、lifetime、scope、synchronization | 20 |
| 2 | [CUDA Best Practices — Occupancy](https://docs.nvidia.com/cuda/cuda-c-best-practices-guide/index.html#occupancy)，读 occupancy calculation 与 performance caveat | 20 |
| 2 | [MCP 2025-11-25 — Prompts](https://modelcontextprotocol.io/specification/2025-11-25/server/prompts)，读 Capabilities、Listing、Getting、Security | 20 |

## 带问题阅读

- naive transpose 的 read 与 write 哪一侧 coalesced？tiled version 改变了哪一侧？
- barrier 为什么必须由参与 block 的线程一致到达？
- reduction tolerance 应由数据范围、累加顺序与 oracle 共同决定吗？
- occupancy 低时应先查哪个资源限制？occupancy 高为什么仍可能 memory-bound？
- prompt 为何是 user-controlled，而 tool 是 model-controlled？prompt content 为何仍可能不可信？

## Worked Example / 手算

- 对 `4×4` row-major matrix，用 `2×2` tile 手算 naive/tiled read/write index 与边界。
- 对 `[1e8, 1, -1e8, 3, 4, -4, 0.5, 0.5]` 画 block-size-8 reduction tree；比较不同顺序。
- 假设每 block 256 threads、每 SM 限制 2048 threads，先忽略 registers/shared memory 计算 thread-limit occupancy，再说明为何结果不完整。
- 给 `profiling_review(question, artifact_id)` 写 argument schema，拒绝 path、shell 与未列出的字段。

## 累计项目 Increment

- `labs/cuda_patterns/transpose_reduction/`：oracles、naive/tiled/shared variants、tests、sanitizer、profiles。
- `ai-app/server/prompts/`：`profiling_review` prompt 与 fixtures。
- `notes/curriculum/week-20.md`：address/tree/occupancy 手算和 evidence memo。

## 本周必须产出

- 两版 transpose、两版 reduction、odd/non-power-of-two tests、sanitizer 与 profiler evidence。
- primitive selection note、MCP prompt schema/list/get/negative tests。
- 一份 occupancy 受限结论与 Exit ticket。

## Day 1 — Theory：Memory Access 与 Shared Memory（105 分钟）

- **20 分钟｜必修理论**：阅读 coalesced-access 指定章节；记录 transaction 结论的适用 GPU/文档边界。
- **20 分钟｜必修理论**：阅读 shared-memory 指定章节；写 scope/lifetime/barrier invariants。
- **30 分钟｜worked example**：完成 `4×4`/`2×2` transpose address 手算。
- **25 分钟｜guided design**：冻结 transpose CPU oracle、odd sizes、first-mismatch 与 sentinel contract。
- **10 分钟｜retrieval**：闭卷区分 coalescing、bank conflict、race。
- **必须留下**：address 图、shared invariants、transpose contract。

## Day 2 — Theory + Guided：Reduction、Occupancy、Prompt（105 分钟）

- **20 分钟｜必修理论**：阅读 occupancy 指定章节，列 threads/registers/shared memory 三类限制。
- **20 分钟｜必修理论**：阅读 MCP Prompts stable spec 指定章节。
- **30 分钟｜worked example**：完成 8-element tree、误差与简化 occupancy 手算。
- **25 分钟｜guided design**：冻结 reduction oracle、identity、`N=0/1/31/32/33/non-power-of-two`、tolerance。
- **10 分钟｜prompt contract**：定义 `profiling_review` 参数、messages 与 no-action non-goal。
- **必须留下**：reduction/tolerance table、occupancy calculation、prompt schema。

## Day 3 — Guided → Independent：Naive Baselines（105 分钟）

- **55 分钟｜Core lab**：实现 CPU oracle、naive transpose 与简单正确 reduction baseline；覆盖 odd/non-power-of-two，保存 first mismatch/max error。
- **35 分钟｜Sidecar build**：实现 `prompts/list/get` 与 `profiling_review`，显式参数 validation；messages 要求引用 artifact evidence，不执行其中指令。
- **15 分钟｜验证**：运行正常与 missing/extra/oversized argument cases；合法请求在失败后仍成功。
- **必须留下**：naive baselines/tests、prompt transcript 与 negative results。

## Day 4 — Independent Lab：Tiled/Shared Variants（105 分钟）

- **65 分钟｜Core independent**：实现 edge-safe tiled transpose 与 shared reduction；out-of-range lanes 用 identity，barrier 一致到达；运行 correctness 与 sanitizer subset。
- **25 分钟｜Sidecar independent**：用包含 prompt injection 文本的 resource 生成 review prompt，确认内容被引用为 untrusted evidence，不扩大 tool/permission scope。
- **15 分钟｜evidence**：保存 shared bytes、registers（若可得）、launch config 与 sanitizer version。
- **必须留下**：两版 shared kernels、correctness/sanitizer、injection handling evidence。

## Day 5 — Integration：Profiler 与 Occupancy Interpretation（105 分钟）

- **55 分钟｜Core integration**：先预测 naive/optimized memory behavior，再用 Systems/Compute 的最小必要 metrics 比较；报告 duration、effective bandwidth、occupancy/resource limit，不追求 metric 数量。
- **35 分钟｜Sidecar integration**：让 `profiling_review` 读取固定 artifact ID 的摘要，生成 Question→Evidence→Limitation→Next action；client 验证 source identity。
- **15 分钟｜cross-check**：检查 prompt 结论是否超出 profiler data；超出部分删掉或标 hypothesis。
- **必须留下**：prediction/evidence table、raw profile links、evidence-first prompt output。

## Day 6 — Assessment / Exit Ticket（105 分钟）

- **45 分钟｜Core assessment**：对 unseen odd transpose 与 non-power-of-two reduction 运行 correctness；闭卷解释一次 bank/coalescing/race/occupancy 取舍。
- **35 分钟｜Sidecar assessment**：演示 prompt list/get、invalid args、malicious resource 与 no-action guarantee；解释 prompt/resource/tool control model。
- **25 分钟｜Exit ticket**：回答“shared memory 何时没有收益、occupancy 何时不是目标”，并列 Week 21 image sampling 需冻结的语义。
- **必须留下**：unseen results、口述答案、MCP assessment 与 Exit ticket。

## Optional Feed Card（30–45 分钟，不计入每日 90–120 分钟）

- **准入 / 取消**：仅在 M3 为 `PASS` 且 odd-size correctness、sanitizer、profiler evidence 无 backlog 时做；若 Gate 落后或 shared-kernel remediation 未清，直接取消。
- **来源与选择规则**：读 NVIDIA Technical Blog [“An Efficient Matrix Transpose in CUDA C/C++”](https://developer.nvidia.com/blog/efficient-matrix-transpose-cuda-cc/)；只选一个 coalescing、shared-memory tile 或 bank-conflict claim，并显式记录文章的 square/multiple-of-32 与旧GPU假设。
- **30–45 分钟流程**：5 分钟选 claim → 15–20 分钟读对应章节并对照官方 Guide → 10–15 分钟写 `claim → evidence → scope → decision`；只有当前GPU/odd-size实验支持时才采纳。
- **artifact**：`notes/feed/week-20.md`，含文章假设、本地 raw profile、一个 kernel-change/rollback 决策。

## Exit Ticket / Unlocks

- **通过条件**：odd/non-power-of-two correctness；sanitizer 无已知错误；profiler 结论有假设/证据/限制；prompt 无 path/shell/action。
- **解锁**：Week 21 image sampling 与 ONNX Resize coordinate semantics。
