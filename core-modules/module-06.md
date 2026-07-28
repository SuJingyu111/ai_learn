# Core Module 06 — Warp、Divergence、SM 与 Occupancy

## 本周目标

建立 SIMT 调度直觉，区分 warp divergence、occupancy、utilization 和实际吞吐。

## 每日计划

### Day 1 — Warp/Block/SM 心智模型

- **10 分钟**：写下 warp、block、SM 的当前定义。
- **25 分钟**：只读 CUDA execution hierarchy 和 Orin compute capability 信息。
- **45 分钟**：画 1000 threads 在 block size 128 下的 grid/block/warp mapping，
  标出最后一个 partial warp 和 bounds check。
- **10–30 分钟**：用 device query/sample 记录 SM count、warp size、limits。
- **必须留下**：硬件表和一张 mapping 图，不从理论 limits 推导性能结论。

### Day 2 — Block-size Sweep

- **10 分钟**：预测 `64/128/256/512` 中哪项更快及可推翻条件。
- **15 分钟**：冻结同一 SAXPY/input/build/power/iterations。
- **55 分钟**：让 block size 可配置，依次 warm-up 和采集 raw latency；每个变体
  运行相同 correctness。
- **10–30 分钟**：保存 occupancy calculator/API 的理论结果与实际 median/p95。
- **必须留下**：block-size CSV、预测和初步结论。

### Day 3 — Divergence 对照

- **10 分钟**：画 uniform、warp-coherent、lane-alternating 三种 lane path。
- **20 分钟**：阅读 SIMT divergence 相关段落。
- **50 分钟**：创建三种 branch variants，确保做相同总工作量并产生相同 output。
- **10–30 分钟**：随机化/固定 input 验证，检查 compiler 没把分支完全优化掉。
- **必须留下**：source variants、work equivalence 说明和 correctness。

### Day 4 — Nsight Compute 验证

- **10 分钟**：选择 2–4 个要回答问题对应的 metrics，不收集整套后再找故事。
- **20 分钟**：学习 `ncu` 最小 launch/section 命令和报告范围。
- **50 分钟**：profile 三种 branches；记录 branch efficiency/warp-related/duration
  等当前工具实际提供的指标。
- **10–30 分钟**：比较预测与证据，注明 profiler overhead 不作为 latency baseline。
- **必须留下**：exact `ncu` command、metric table 和一次预测修正。

### Day 5 — Resource/Occupancy 实验

- **10 分钟**：预测增加每线程 registers 或改变 block size 的影响。
- **20 分钟**：阅读 occupancy 限制因素：threads/registers/shared memory。
- **50 分钟**：选择一个可控变体，记录 registers、theoretical occupancy、duration；
  只改变一个变量。
- **10–30 分钟**：解释 occupancy 与 achieved performance 是否同方向。
- **必须留下**：before/after、confounders 和“不能证明什么”。

### Day 6 — Block/Occupancy 结论

- **15 分钟**：闭卷回答本周必答问题。
- **40 分钟**：写 `notes/concepts/warp-sm-occupancy.md`，加入三种 branch 图。
- **20 分钟**：复跑 block sweep 的两个关键点，确认数据稳定。
- **15–30 分钟**：weekly review；为 Week 7 选择一个明确 memory address 问题。
- **必须留下**：整理后的 CSV、概念笔记和适用范围。

## 控制变量

- 相同输入、输出和总工作量。
- 相同 build flags、power mode 和 warm-up。
- 不把 branch 变体改成不同算法。
- 每个输出在 timing 后验证。

## 必答问题

- warp divergence 何时会序列化路径？
- 边界 bounds check 为什么通常不是主要问题？
- 高 occupancy 为什么不保证更快？
- block 资源如何限制 SM 并发驻留？

## 完成标准

- [ ] 三类 branch 输出一致。
- [ ] block-size 数据包含 median/p95。
- [ ] 至少一个结论绑定 profiler 指标。
- [ ] 能区分 theoretical occupancy 与 achieved performance。
