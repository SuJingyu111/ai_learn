# Core Module 22 — End-to-end Optimization、Power 与 Thermal

## 本周目标

选择 Week 21 top bottleneck 中最值得优化的一项，评估 latency、throughput、memory
和 power/thermal，而不是只优化最显眼的 kernel。

## 候选方向

- 去除 hidden copy/reformat。
- buffer reuse 或 allocator policy。
- fused preprocessing。
- CUDA Graph replay。
- multi-frame overlap。
- shape/profile 或 FP16 engine。
- CPU thread scheduling/feeding bottleneck。

## 每日计划

### Day 1 — Optimization Experiment Contract

- **10 分钟**：复制 Week 21 baseline identity，不重新选择有利 workload。
- **20 分钟**：写单一 hypothesis、预期 metric、可推翻条件和 correctness invariants。
- **50 分钟**：设计 before/after procedure、sample count、power/thermal stabilization、
  rollback path。
- **10–30 分钟**：做一次 baseline confirmation，超过合理漂移先修 measurement。
- **必须留下**：experiment document 和 confirmed baseline。

### Day 2 — Focused Evidence

- **10 分钟**：只选择能判断 hypothesis 的 profiler/tool。
- **15 分钟**：缩小 capture 到相关 stages/frames。
- **55 分钟**：收集 Systems/Compute/TensorRT layer/engine evidence，定位具体 copy、
  kernel、reformat、allocation、launch 或 host gap。
- **10–30 分钟**：写“证据支持实现什么变化”，避免凭直觉换优化方向。
- **必须留下**：focused metrics、trace references 和 change spec。

### Day 3 — 实现单一变化

- **10 分钟**：记录将修改的 files/symbols 和不修改的 semantics。
- **50 分钟**：实现优化，保留 baseline variant 或可安全切换路径。
- **20 分钟**：运行 unit/operator/end-to-end correctness 和 failure path。
- **10–30 分钟**：出现 correctness regression 时停止 timing，先定位。
- **必须留下**：working change、full correctness 和 rollback method。

### Day 4 — Before/After Measurement

- **10 分钟**：确认 model/input/build/power/clocks/temperature window 相同。
- **50 分钟**：交替或合理顺序采集 before/after raw samples，避免只先冷机跑一版。
- **20 分钟**：计算 stage/kernel/end-to-end median/p95/throughput/memory。
- **10–30 分钟**：重新 profile optimized representative case，验证机制真的变化。
- **必须留下**：raw before/after 和 mechanism evidence。

### Day 5 — Sustained Power/Thermal

- **10 分钟**：定义 sustained duration 和停止条件，不盲目锁最高 clocks。
- **50 分钟**：运行 baseline/optimized sustained workload，采集 latency over time、
  temperature、power telemetry、throttling indications。
- **20 分钟**：比较 cold、warm、steady windows。
- **10–30 分钟**：记录散热/供电/后台负载限制，不能测 energy 时明确 unavailable。
- **必须留下**：sustained data 和结论适用的 operating point。

### Day 6 — Runtime Case Study

- **15 分钟**：整理 raw links、environment、correctness、local/end-to-end delta。
- **55 分钟**：写 `docs/case-studies/runtime-integration.md`，包括无收益/反直觉结果。
- **15 分钟**：验证所有图表可从 raw data重建，删除 unsupported claims。
- **15–30 分钟**：weekly review，录制 5 分钟讲解。
- **必须留下**：第三份作品级 case study 和 sustained Orin result。

## 输出指标

- first-frame 与 steady-state median/p95。
- throughput。
- GPU/CPU memory。
- power mode、温度区间和可用功耗 telemetry。
- max/relative error 或任务输出一致性。

## 完成标准

- [ ] 优化只改变声明的变量。
- [ ] sustained run 没有把冷机数据当稳态。
- [ ] kernel 与 end-to-end delta 均被报告。
- [ ] 若无收益，保留证据并解释瓶颈。
