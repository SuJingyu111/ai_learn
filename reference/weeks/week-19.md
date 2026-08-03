# Week 19 — Unit 4：Profiler Mental Model 与 NVTX

## Unit / 课程角色

- **Unit 4 · Image Operator Case Study，第一周。** 先学习“问题→工具→证据”的 profiler mental model，再进入 transpose/reduction 与 resize。
- 课程导航：[总纲](../CURRICULUM.md) · [官方资料索引](../docs/resources.md) · [Mastery Gates](../docs/mastery-gates.md)。
- **Sidecar 依赖位置**：给 local read-only MCP server 增加固定 Resources；Week 20 再加入 Prompts。

## 硬先修与 Remediation

- **硬先修**：M3 `PASS`；能解释 wall/event timing、async error boundary、sanitizer limitation；有一条 clean CUDA experiment command。
- **开周诊断（15 分钟）**：对“latency 高”分别写 Systems、Compute、event、wall clock 能回答/不能回答的问题。
- **未通过时**：回 Week 17 timing table 与 Week 18 reproducibility blocker；先完成一份 hypothesis→metric→expected observation 卡片，不得无问题地打开 profiler。

## Observable Objectives

1. 根据 CPU/GPU/API/kernel 问题选择 wall clock、CUDA event、Nsight Systems 或 Nsight Compute。
2. 用 NVTX 给 setup、H2D、kernel、D2H、validation 建互不混淆的语义边界。
3. 限制 capture range，生成可复现 Systems report，并用 timestamps 回答预先写好的问题。
4. 解释 profiler overhead、DVFS/thermal、warm-up、background load 等 confounders。
5. 用 MCP `resources/list/read` 暴露固定 architecture/methodology 文档，保留 provenance，不接受任意 path。

## 术语表

| 术语 | 本周可操作定义 |
|---|---|
| hypothesis | 可以被观测证据推翻的机制预测 |
| NVTX range | host code 中带语义名称的时间区间 |
| capture range | profiler 实际收集 trace 的有限窗口 |
| timeline | 按 timestamp 表示 CPU/API/copy/kernel/dependency 的记录 |
| profiler overhead | tracing/metric replay 对被测执行的扰动 |
| confounder | 可改变结果、却不是待测变量的环境因素 |
| MCP resource | application-controlled、由 URI 标识的只读上下文 |

## 必读理论材料（时间已计入对应 Day）

| Day | 材料与精确章节 | 分钟 |
|---:|---|---:|
| 1 | [Nsight Systems User Guide](https://docs.nvidia.com/nsight-systems/UserGuide/index.html)，读 “Profiling from the CLI”“Controlling Data Collection” | 25 |
| 1 | [NVTX Documentation](https://nvidia.github.io/NVTX/doxygen/index.html)，读 Core API concepts 与 ranges/domains | 15 |
| 2 | [Nsight Compute Profiling Guide](https://docs.nvidia.com/nsight-compute/ProfilingGuide/index.html)，读 “Introduction”“Profiling Overview” | 20 |
| 2 | [MCP 2025-11-25 — Resources](https://modelcontextprotocol.io/specification/2025-11-25/server/resources)，读 Capabilities、Listing、Reading、Security | 20 |

## 带问题阅读

- API time、kernel time、end-to-end latency 分别需要什么边界？
- 什么情况下一张 timeline 只能提出下一步问题，不能解释根因？
- NVTX 命名怎样避免把 validation/logging 错算成 GPU stage？
- resource URI、MIME type、source hash、last-modified 各支持什么判断？
- 为什么固定 resource allowlist 比接收 client path 更容易审计？

## Worked Example / 手算

给 host 序列 `alloc 2 ms → H2D 1 ms → kernel 3 ms → D2H 1 ms → validate 4 ms`：

- 手算 kernel-only、GPU-stage、end-to-end 三个区间。
- 假设 host 在 kernel 后做 2 ms CPU work，画可能 overlap 与必须同步的位置。
- 写三个 profiler 问题：“copy 是否串行”“哪次 API 阻塞”“validation 是否在 timed region”。
- 设计 `runtime://methodology/benchmark` resource descriptor：固定 URI、MIME、source hash、无 path 参数。

## 累计项目 Increment

- `labs/profiling/profiler_baseline/`：environment manifest、NVTX-instrumented target、Systems/Compute commands 与 raw reports。
- `ai-app/server/resources/`：固定 architecture 与 benchmark methodology resources。
- `notes/curriculum/week-19.md`：question→tool→evidence decision log。

## 本周必须产出

- profiler decision table、NVTX naming map、environment manifest 与第一份 focused Systems trace。
- 两个固定 MCP resources、client assertions、unknown/path-escape negative tests。
- profiler evidence memo 与 Exit ticket。

## Day 1 — Theory：问题、工具与时间边界（105 分钟）

- **25 分钟｜必修理论**：阅读 Nsight Systems 指定章节；记录 CLI capture、range、output identity。
- **15 分钟｜必修理论**：阅读 NVTX ranges/domains；写命名与 nesting 规则。
- **30 分钟｜mental model**：建立 wall/event/Systems/Compute decision table，每格写适用问题、同步要求、局限。
- **25 分钟｜worked example**：完成 2/1/3/1/4 ms timeline 手算与三个问题。
- **10 分钟｜retrieval**：闭卷解释 capture range、overhead、confounder。
- **必须留下**：decision table、手算 timeline、NVTX naming draft。

## Day 2 — Guided Practice：Environment、NVTX、Resources（105 分钟）

- **20 分钟｜必修理论**：阅读 Nsight Compute Profiling Overview，区分 timeline 与 per-kernel metric。
- **20 分钟｜必修理论**：阅读 MCP Resources stable spec 指定章节。
- **40 分钟｜guided lab**：采集 GPU/driver/CUDA/build flags/power/clocks/temperature manifest；给 transform target 加 setup/H2D/kernel/D2H/validate NVTX ranges。
- **15 分钟｜MCP guided**：定义 `runtime://architecture`、`runtime://methodology/benchmark` descriptor 与 source identity。
- **10 分钟｜验证**：正常 run 确认 NVTX 未改变 correctness，resource 无 caller path。
- **必须留下**：manifest、instrumented code、resource schema/source allowlist。

## Day 3 — Guided → Independent：第一份 Focused Trace（105 分钟）

- **55 分钟｜Core lab**：先写 3 个问题，再用有限 capture range 运行 `nsys profile`；从 report/summary 提取 API、copies、kernel、sync 的 timestamps 与 duration。
- **35 分钟｜Sidecar build**：实现 `resources/list/read`，只读固定文件/嵌入内容；返回 URI、MIME、source hash/last-modified。
- **15 分钟｜验证**：client 断言 list/read deterministic；unknown URI 返回稳定错误，进程继续健康。
- **必须留下**：exact profiler command/report、3 个证据答案、resource transcript。

## Day 4 — Independent Lab：Trace Variation 与 Resource Abuse（105 分钟）

- **60 分钟｜Core independent**：改变一个变量（workload size 或显式 sync 位置），先预测再 profile；用 timestamp diff 判断 hypothesis，不以截图颜色下结论。
- **30 分钟｜Sidecar independent**：测试 `file://`、`../`、oversized URI、malicious resource content；固定 allowlist 阻断前三项，第四项按 untrusted data 返回且不执行。
- **15 分钟｜证据审阅**：记录 profiler overhead、thermal/power 与 capture variation。
- **必须留下**：before/after trace table、四条 resource negative results、confounder note。

## Day 5 — Integration：Question → Evidence Memo（105 分钟）

- **55 分钟｜Core integration**：写一页 profiling memo：Question→Prediction→Command→Timeline Evidence→Conclusion→Limitation→Next metric；每个数字指向 raw report。
- **35 分钟｜Sidecar integration**：让 methodology resource 返回本周 measurement contract；client 核对 source hash 后才展示，MCP output 不成为 instruction。
- **15 分钟｜交叉审查**：用 resource 中 methodology 检查 memo，但任何冲突需人类显式解决，不自动改实验。
- **必须留下**：profiling memo、resource provenance check 与冲突处理记录。

## Day 6 — Assessment / Exit Ticket（105 分钟）

- **45 分钟｜Core assessment**：给一条陌生 timeline，选择下一工具/metric，指出至少一个 implicit sync 与一个证据不足点；再从 clean command 重现本周 trace。
- **35 分钟｜Sidecar assessment**：演示 resource list/read、source identity、unknown/path escape、malicious content；解释 resource 与 tool 的 control model。
- **25 分钟｜Exit ticket**：回答“timeline 何时只是相关性，何时足以否定 hypothesis？”并列 Week 20 需要 profile 的 transpose/reduction 问题。
- **必须留下**：assessment answers、clean trace、MCP demo 与 Exit ticket。

## Optional Feed Card（30–45 分钟，不计入每日 90–120 分钟）

- **准入 / 取消**：仅在 M3 为 `PASS` 且本周 trace/resource artifacts 无 backlog 时做；若 Gate 落后、clean trace不可复现或 profiler remediation 未清，直接取消。
- **来源与选择规则**：读 NVIDIA Technical Blog [“Nsight Systems Exposes New GPU Optimization Opportunities”](https://developer.nvidia.com/blog/nsight-systems-exposes-gpu-optimization/)；只选一个 GPU starvation、CPU↔GPU correlation 或 NVTX claim，且必须能用当前 Nsight Systems User Guide 和自己的 timeline限定。
- **30–45 分钟流程**：5 分钟选 claim → 15–20 分钟读文章并定位本地 trace → 10–15 分钟写 `claim → evidence → scope → decision`；文章截图仅作线索，不作本地证据。
- **artifact**：`notes/feed/week-19.md`，含 trace timestamp/report link、tool/device版本、一个 next-metric 决策。

## Exit Ticket / Unlocks

- **通过条件**：tool selection 有理由；NVTX/timed regions 无污染；trace 可重现；resource 固定、read-only、有 provenance 且 path escape 被拒。
- **解锁**：Week 20 memory-access/shared-memory/occupancy lab。
