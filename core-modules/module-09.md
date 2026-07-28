# Core Module 09 — Nsight 与 Benchmark Baseline

## 本周目标

把性能工作变成可重复的 measure-first 流程，为后续 resize case study 建立
环境采集、raw result、NVTX 和 profiler 模板。

## 每日计划

### Day 1 — Environment/Workload Manifest

- **10 分钟**：列出当前 benchmark 还缺哪些环境字段。
- **20 分钟**：对照 `docs/benchmark-methodology.md`，只保留能稳定采集的字段。
- **50 分钟**：设计并生成一次 environment manifest，包含 Orin/L4T/JetPack/CUDA、
  power/clocks/compiler/build flags/温度；格式选 Markdown、JSON 或 key-value。
- **10–30 分钟**：在两次运行间 diff，标记会导致 baseline invalid 的字段。
- **必须留下**：manifest 样例、采集命令和 unknown fields，不伪造 unavailable 数据。

### Day 2 — NVTX 与阶段边界

- **10 分钟**：画 allocation/H2D/kernel/D2H/validation 的 host timeline。
- **20 分钟**：阅读 Nsight Systems 的 focused profiling/NVTX ranges。
- **50 分钟**：为独立 lab 或 `resize_demo` 加 NVTX ranges，确保 validation/logging
  不进入 kernel range；命名 stream/thread。
- **10–30 分钟**：build/run，确认没有改变 correctness 和正常 timing。
- **必须留下**：range naming scheme、代码位置和运行命令。

### Day 3 — 第一份 Nsight Systems Timeline

- **10 分钟**：写 3 个 timeline 要回答的问题。
- **15 分钟**：确定 capture range、trace APIs 和 output path，避免无限 trace。
- **55 分钟**：运行 `nsys profile`，查看/导出 summary；定位 API calls、copies、
  kernel 和 synchronizations。
- **10–30 分钟**：截图可辅助，但用文字记录 timestamps/durations/dependencies。
- **必须留下**：exact command、report path、3 个问题的证据答案。

### Day 4 — 两类 Kernel 的 Nsight Compute

- **10 分钟**：从 Week 7/8 各选一个 memory-oriented 和 sync/compute-oriented kernel。
- **20 分钟**：为每个 kernel 只选与 hypothesis 对应的 sections/metrics。
- **50 分钟**：分别 profile，记录 launch config、duration、memory、occupancy/stall
  等实际可用指标。
- **10–30 分钟**：写为何同一个 metric 对两个 kernels 的意义不同。
- **必须留下**：两张 compact metric tables 和 profiler commands。

### Day 5 — 第一份 Profiling Report

- **10 分钟**：选择一个可以被证据推翻的问题。
- **20 分钟**：复制 `notes/templates/profiling-report.md`。
- **50 分钟**：填写 hypothesis、reproduction、evidence、conclusion；若未改代码，
  Change/Result 明确写“尚未执行”。
- **10–30 分钟**：让所有数字指向 raw data/report，删除无证据形容词。
- **必须留下**：完整报告草稿和至少一个 limitation。

### Day 6 — 冻结 Resize Measurement Procedure

- **15 分钟**：列出 DVFS、thermal、warm-up、profiler overhead、background load 等
  confounders。
- **35 分钟**：写 resize benchmark procedure：cases、warm-up、iterations、
  synchronization、validation、output fields。
- **20 分钟**：dry-run 一次现有 CPU/GPU demo，标出代码缺口。
- **20–40 分钟**：weekly review；将 procedure 作为 Week 10–12 不随意更改的 contract。
- **必须留下**：baseline procedure 和进入 Week 10 的缺口清单。

## 指标边界

- host wall clock：端到端或 host-observed latency。
- CUDA event：同一 device/stream 的 GPU work。
- Nsight Systems：跨 CPU/GPU/API timeline。
- Nsight Compute：kernel-level metrics。

四者回答的问题不同，不能互相替代。

## 交付

- environment snapshot。
- 一份 `.nsys-rep` 或其受控摘要与 raw stats。
- 一份 Nsight Compute 指标摘要。
- 更新后的 experiment/profiling note。

## 完成标准

- [ ] profiler capture 可由一条文档化命令复现。
- [ ] 日志和 correctness 位于 timed region 外。
- [ ] median/p95/min 与原始 samples 被保存。
- [ ] 至少识别一个测量混杂因素。
