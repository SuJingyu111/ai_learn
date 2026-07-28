# Core Module 15 — Streams、Events 与 Pinned Memory

## 本周目标

理解 stream 是有序 command queue，不是 CPU thread；建立 async copy、event
dependency 和 buffer lifetime 的正确模型。

## 每日计划

### Day 1 — 单帧 Stream/Event Timeline

- **10 分钟**：闭卷画 synchronous H2D→kernel→D2H。
- **20 分钟**：阅读 stream ordering、event record/wait/synchronize 的必要段落。
- **50 分钟**：画 default stream 与 explicit stream 两条 timeline，标明 host return、
  output-ready、buffer-reusable 点。
- **10–30 分钟**：为每个 arrow 写具体 CUDA API，不写模糊“sync”。
- **必须留下**：两张 timeline 和五个 lifetime statements。

### Day 2 — Explicit-stream Operator

- **10 分钟**：审阅现有 CUDA resize/warp stream 参数和调用者责任。
- **15 分钟**：定义一个 single-frame runner 的 source/destination ownership。
- **55 分钟**：将 allocation/copies/operator 放入 explicit stream；去掉无必要的
  device-wide sync。
- **10–30 分钟**：在唯一必要的 output-read boundary 同步并比较结果。
- **必须留下**：runner、correctness 和每个同步的理由。

### Day 3 — Events 与分阶段 Timing

- **10 分钟**：预测 event placement 错误会漏测哪些工作。
- **20 分钟**：设计 H2D/preprocess/D2H/end-to-end timing。
- **50 分钟**：加入 events，确保记录在目标 stream，复用 events 或说明创建成本。
- **10–30 分钟**：与 host clock 对照，解释两者差异。
- **必须留下**：stage latency raw samples 和 event dependency 图。

### Day 4 — Pageable vs Pinned

- **10 分钟**：写 pinned memory 的预期收益与系统成本。
- **15 分钟**：冻结 transfer size、direction、iterations 和 allocation boundary。
- **55 分钟**：实现 pageable/pinned 两种 host buffers；分别报告 registration/
  allocation setup 和 steady transfer。
- **10–30 分钟**：检查真正使用 async API 和正确 stream。
- **必须留下**：transfer table、host allocation lifetime 和受限结论。

### Day 5 — Lifetime 故障实验

- **10 分钟**：列提前 free/reuse、wrong event、wrong stream wait 三种风险。
- **15 分钟**：选择一个隔离、可由 sanitizer/timeline 观察的故障。
- **55 分钟**：运行错误版本→收集 diagnostic→修复→压力重复；不把 UB 放进默认 build。
- **10–30 分钟**：记录工具是否可靠发现该类问题。
- **必须留下**：failure scenario、diagnostic、fix 和 residual risk。

### Day 6 — Single-frame Async Contract

- **15 分钟**：闭卷回答本周必答问题。
- **40 分钟**：写 ownership/lifetime/error propagation 文档。
- **20 分钟**：clean run pageable/pinned、stage timing、correctness。
- **15–30 分钟**：weekly review；把单 frame buffer 状态扩展为 Week 16 两个 slots。
- **必须留下**：single-frame contract 和进入 multi-frame 的 invariants。

## 必答问题

- API 返回时哪些 work 已完成？
- event record/wait/synchronize 分别建立什么关系？
- `cudaMemcpyAsync` 为什么不总能实现预期 overlap？
- pinned memory 有什么系统成本？
- default stream 如何与第三方 runtime stream 交互？

## 交付

- explicit-stream API path。
- pageable/pinned transfer 数据。
- single-frame buffer lifetime diagram。

## 完成标准

- [ ] 正常执行不依赖全局同步。
- [ ] event 与 buffer ownership 对应关系可解释。
- [ ] async error 在明确边界被观察。
- [ ] pinned memory 数量和 lifetime 受控。
