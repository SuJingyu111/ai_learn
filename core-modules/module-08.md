# Core Module 08 — Shared Memory、Reduction 与 Compute Sanitizer

## 本周目标

理解 shared memory 的容量、bank、显式装载和 block 内同步语义，并建立 CUDA
memory/race 检查习惯。

## 每日计划

### Day 1 — Reduction Oracle 与数值策略

- **10 分钟**：写 floating-point reduction 不能假设 bit-exact 的原因。
- **20 分钟**：阅读 numerical accuracy/non-associativity 相关内容。
- **50 分钟**：创建 CPU double-accumulation oracle；生成正负、不同 magnitude、
  `N=0,1,31,32,33,1000,非2次幂` cases。
- **10–30 分钟**：定义 absolute/relative tolerance，并用错误实现验证 test 会失败。
- **必须留下**：oracle、seed、case matrix 和 tolerance rationale。

### Day 2 — Naive GPU Reduction

- **10 分钟**：画每个 thread/block 负责的输入范围和 partial results。
- **20 分钟**：选择简单正确的 baseline，不追求单 kernel 最优。
- **50 分钟**：实现 naive/global-memory 或 two-stage baseline；处理任意 N。
- **10–30 分钟**：与 CPU oracle 比较，输出 GPU sum、error 和 first failing case。
- **必须留下**：baseline、launch/temporary buffer contract 和 correctness。

### Day 3 — Shared-memory Tree Reduction

- **10 分钟**：手画 block size 8 的 shared reduction steps。
- **20 分钟**：阅读 shared memory 和 barrier 语义。
- **50 分钟**：实现 shared-memory version；确保 out-of-range lanes 使用 identity，
  所有参与线程一致到达 barrier。
- **10–30 分钟**：测试非 2 次幂 N、多个 blocks，并与 baseline timing 对照。
- **必须留下**：shared version、同步图和 raw latency。

### Day 4 — Barrier/Race 故障实验

- **10 分钟**：预测 divergent barrier、missing barrier、out-of-bounds 的症状。
- **15 分钟**：将故障样例与正常代码完全隔离。
- **55 分钟**：创建一个小型错误 kernel；分别运行 Compute Sanitizer 的 memcheck、
  racecheck/synccheck（以设备工具支持为准）。
- **10–30 分钟**：修复后重跑，记录工具找到和未找到的内容。
- **必须留下**：故障、tool command、diagnostic、修复 diff 的文字摘要。

### Day 5 — 第二个 Shared-memory Pattern

- **10 分钟**：在 tiled transpose 与小 matmul 中选择一个，优先补 Week 7 transpose。
- **20 分钟**：预测 tile、padding、bank/access 影响。
- **50 分钟**：完成 edge-safe tiled implementation，覆盖 non-tile dimensions。
- **10–30 分钟**：运行 correctness、sanitizer 和最小 benchmark。
- **必须留下**：第二个 pattern 及为何 shared memory 有/无收益。

### Day 6 — Week 8 Gate

- **15 分钟**：闭卷解释 shared memory、barrier、bank conflict、race。
- **35 分钟**：写 `notes/concepts/shared-memory-reduction.md`。
- **25 分钟**：clean rebuild/run reduction、transpose 和 sanitizer 正常路径。
- **15–30 分钟**：逐项完成 Week 8 Gate；失败项明确阻塞 Week 9 哪一步。
- **必须留下**：Gate evidence、numerical table、sanitizer result。

## Correctness

- 输入包含正负、极小/极大值和非 2 次幂长度。
- 明确 floating-point non-associativity。
- tolerance 由累加顺序和数值范围解释。
- sanitizer 结果保存命令和工具版本。

## 必答问题

- shared memory 为什么不是自动 cache？
- 为什么 `__syncthreads()` 必须由参与 block 的线程一致到达？
- bank conflict、coalescing 和 race 是三类什么问题？
- shared memory 版本为什么可能没有更快？

## Week 8 Gate

- [ ] reduction 支持任意 N。
- [ ] CPU/GPU 结果在有依据的 tolerance 内。
- [ ] sanitizer 无已知错误，或记录平台限制。
- [ ] 能解释一次资源/同步/数值取舍。
