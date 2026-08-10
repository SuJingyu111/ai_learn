# 进度跟踪

完成一个 Step，并确认它的「跑起来」命令产生了预期输出后，把对应的 `[ ]` 改成
`[x]`。这里只列已经有独立任务文件、可以实际执行的 Step；后续 Step 写出来时再追加。

## Phase 00 — Tensor

- [x] [Step 001 — 一个能编译的空引擎与 `tt` 命令行](steps/phase-00-tensor/step-001.md)
- [ ] [Step 002 — `Shape`：rank、维度、元素总数](steps/phase-00-tensor/step-002.md)
- [ ] [Step 003 — `DType`：字节宽度与类型标签](steps/phase-00-tensor/step-003.md)
- [ ] [Step 004 — `Storage`：一块带引用计数的裸内存](steps/phase-00-tensor/step-004.md)
- [ ] [Step 005 — `Tensor` = storage + shape + dtype + offset](steps/phase-00-tensor/step-005.md)
- [ ] [Step 006 — row-major strides 与 flat offset 公式](steps/phase-00-tensor/step-006.md)
- [ ] [Step 007 — `tt dump`：打印元数据和真实地址](steps/phase-00-tensor/step-007.md)
- [ ] [Step 008 — 多维索引 `at(i,j,k)` 与越界检查](steps/phase-00-tensor/step-008.md)
- [ ] [Step 009 — `reshape`：什么时候能不复制](steps/phase-00-tensor/step-009.md)
- [ ] [Step 010 — `slice`：共享 storage 的 view](steps/phase-00-tensor/step-010.md)
- [ ] [Step 011 — `permute` / `transpose`：只改 strides](steps/phase-00-tensor/step-011.md)
- [ ] [Step 012 — `is_contiguous` 与 `contiguous()`](steps/phase-00-tensor/step-012.md)
- [ ] [Step 013 — 从文件读写张量（你自己的 `.tt` 格式）](steps/phase-00-tensor/step-013.md)
- [ ] [Step 014 — 测试骨架：手算 fixture 与容差](steps/phase-00-tensor/step-014.md)

## Phase 01 — 第一批算子

- [ ] [Step 015 — 逐元素算子框架与 `relu`](steps/phase-01-ops/step-015.md)
- [ ] [Step 016 — `add` 与形状检查](steps/phase-01-ops/step-016.md)
- [ ] [Step 017 — 广播规则：先手算，再实现](steps/phase-01-ops/step-017.md)
- [ ] [Step 018 — 广播的两种实现：展开 vs 索引映射](steps/phase-01-ops/step-018.md)
- [ ] [Step 019 — `matmul`：最朴素的三重循环](steps/phase-01-ops/step-019.md)
- [ ] [Step 020 — 手算 fixture 锁死 matmul 语义](steps/phase-01-ops/step-020.md)
- [ ] [Step 021 — 循环顺序 ijk vs ikj](steps/phase-01-ops/step-021.md)
- [ ] [Step 022 — 浮点不结合：为什么 sum 顺序会改变结果](steps/phase-01-ops/step-022.md)
- [ ] [Step 023 — 容差怎么定：把它放宽会掩盖什么](steps/phase-01-ops/step-023.md)
- [ ] [Step 024 — `numpy` 对拍脚本：第一个外部 oracle](steps/phase-01-ops/step-024.md)
- [ ] [Step 025 — `softmax`：数值稳定性不是可选项](steps/phase-01-ops/step-025.md)
- [ ] [Step 026 — 把算子接进统一的注册表](steps/phase-01-ops/step-026.md)
- [ ] [Step 027 — `bench` harness：median/p95 与溯源](steps/phase-01-ops/step-027.md)
