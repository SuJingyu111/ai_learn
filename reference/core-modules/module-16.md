# Core Module 16 — Multi-frame Pipeline 与 CUDA Graphs

## 本周目标

实现同步、single-stream async、multi-stream/double-buffer 对照，并用 timeline
证明实际 overlap。CUDA Graphs 仅用于稳定、重复 workload。

## 每日计划

### Day 1 — Frame Slot 状态机

- **10 分钟**：从 Week 15 单帧 timeline 列出每个 buffer 的 owner。
- **20 分钟**：定义 `free/filling/H2D/GPU/D2H/consuming/error` 状态和 transition。
- **50 分钟**：画两个 slots、三个 frames 的理想 timeline；为每个 transition 指定
  event/host action。
- **10–30 分钟**：模拟 shutdown、one-frame、odd frame count。
- **必须留下**：状态机、invariants 和 illegal transitions。

### Day 2 — Double-buffer Baseline

- **10 分钟**：冻结 synchronous runner 作为 oracle/baseline。
- **20 分钟**：设计两个完整 buffer sets 和 streams，不共享仍在使用的 storage。
- **50 分钟**：实现 double-buffer pipeline，先处理固定少量 frames。
- **10–30 分钟**：逐帧与 synchronous outputs 比较，验证 ordering。
- **必须留下**：可运行 pipeline、frame IDs 和 per-frame correctness。

### Day 3 — Stress、Shutdown 与 Error Path

- **10 分钟**：定义 1/2/3/100/数百 frames 和 early stop cases。
- **15 分钟**：加入 deterministic input/frame checksum，避免错序不被发现。
- **55 分钟**：压力运行；注入一个 operator/copy failure，验证停止提交、等待/清理
  已提交 work。
- **10–30 分钟**：运行 sanitizer 或日志验证无复用/析构 race。
- **必须留下**：stress summary、shutdown sequence 和 failure behavior。

### Day 4 — Nsight Systems Overlap

- **10 分钟**：预测 H2D/kernel/D2H 哪些能/不能 overlap。
- **15 分钟**：用 NVTX 标 frame/slot/stage，限制 capture range。
- **55 分钟**：profile synchronous、single-stream、double-buffer；寻找隐式 sync 和
  copy engine/resource限制。
- **10–30 分钟**：用 timestamps 量化 overlap，不只看颜色重叠截图。
- **必须留下**：三条 timeline 证据和“没有 overlap”时的解释。

### Day 5 — CUDA Graphs 对照

- **10 分钟**：判断 workload 是否 fixed shape/address/control flow，是否适合 capture。
- **20 分钟**：阅读 capture restrictions 和 replay lifecycle。
- **50 分钟**：对单 slot 或稳定 sequence capture/replay；处理 setup 与 error path。
- **10–30 分钟**：比较 launch/API overhead、GPU duration、end-to-end。
- **必须留下**：graph/non-graph raw data、capture contract 和限制。

### Day 6 — Pipeline Case Study/Gate

- **15 分钟**：运行四种 variants 的公平 benchmark 和 full correctness。
- **50 分钟**：写 `docs/case-studies/async-pipeline.md`，加入状态机、timeline、
  latency/throughput 和 lifetime。
- **15 分钟**：逐项检查 setup cost、power/thermal、frame count、raw data。
- **10–30 分钟**：完成 Week 16 Gate、weekly review 和 3 分钟口述总结。
- **必须留下**：第二份作品级 case study。

## 公平对照

- 相同 frames、input、operator 和 output validation。
- 报告 first-frame latency、steady-state latency、throughput。
- allocation/registration/setup 是否计入必须一致或单独报告。
- 记录 Orin power/clock/thermal 和 memory pressure。

## 交付

- buffer state diagram。
- synchronous/single-stream/multi-stream/graph raw data。
- Nsight Systems timeline case study。

## Week 16 Gate

- [ ] 数百帧无已知 lifetime/race。
- [ ] overlap 由 timeline 证明或否定。
- [ ] throughput 与单帧 latency 分开。
- [ ] graph capture 限制与收益条件明确。
