# Core Module 24 — Portfolio、面试与下一阶段

## 本周目标

把半年学习整理成可复现、可解释的作品，不重写历史掩盖失败。完成三份 case study、
最终 README、10 分钟讲解和下一阶段选择。

## 每日计划

### Day 1 — 最终 Release Candidate

- **10 分钟**：冻结 source identity，不再加入功能。
- **20 分钟**：确认最终 Orin environment/power/model/workload manifests。
- **50 分钟**：clean build/test/correctness/representative benchmark，保存完整 transcript。
- **10–30 分钟**：创建 release candidate checklist；失败只修 blocker/regression。
- **必须留下**：final test matrix、environment 和 release candidate identity。

### Day 2 — Resize Case Study 编辑

- **10 分钟**：从面试者视角写一句问题和一句结论。
- **20 分钟**：检查 raw data、commands、correctness、before/after、limitations links。
- **50 分钟**：精简并完善 `docs/case-studies/resize.md`，加入一张最有解释力的表/图。
- **10–30 分钟**：3 分钟口述；删掉无法当场解释的 metrics。
- **必须留下**：可独立阅读的 resize case study。

### Day 3 — Pipeline/Runtime Case Studies 编辑

- **10 分钟**：确认两份报告分别回答 lifetime/overlap 和 runtime/device-resident。
- **20 分钟**：检查 timeline annotations、power/thermal、provider/engine/version。
- **50 分钟**：完善 async/runtime 两份 case studies，消除重复并交叉链接架构。
- **10–30 分钟**：确保至少包含一次被 profiler 推翻的 hypothesis。
- **必须留下**：三份风格一致、数据可追溯的报告。

### Day 4 — README 与 Reproduction

- **10 分钟**：列新读者需要的 problem/architecture/build/test/run/results/limitations。
- **50 分钟**：更新 README 和 architecture links，只保留验证过的命令。
- **20 分钟**：从 README 逐条 dry-run 或在 clean shell 验证。
- **10–30 分钟**：检查 broken links、个人路径、版本敏感说明和 unsupported claims。
- **必须留下**：作品集首页和可执行 quick start。

### Day 5 — 讲解与面试演练

- **15 分钟**：按建议时间写 10 分钟讲解提纲，不写逐字稿。
- **30 分钟**：完整录音一次，记录超时、含糊和证据不足位置。
- **25 分钟**：回答本周 6 个深入问题，每题限制 2–3 分钟。
- **20–40 分钟**：第二次讲解，确保能从 graph 讲到 Orin hardware evidence。
- **必须留下**：讲解提纲、问题答案和需要诚实承认的限制。

### Day 6 — Retrospective 与 90 天 Backlog

- **15 分钟**：统计实际投入、完成 Gates、未完成 optional，不美化历史。
- **40 分钟**：写 24 周 retrospective：迁移成功的旧经验、错误类比、最大能力缺口。
- **20 分钟**：按目标岗位/JD evidence 选择一个下一阶段方向。
- **15–30 分钟**：建立 90 天 backlog，只保留一个 primary track 和明确退出条件。
- **必须留下**：retrospective、下一阶段决策和项目最终完成标准证据。

## 10 分钟讲解建议

- 1 分钟：背景与转型目标。
- 2 分钟：tensor/operator/runtime execution chain。
- 2 分钟：API、correctness 和 ownership。
- 3 分钟：两个 profiler evidence 与优化取舍。
- 1 分钟：Orin power/end-to-end 结论。
- 1 分钟：限制和下一步。

## 面试自测

- graph node、operator 和 CUDA kernel 为什么不是一一对应？
- layout conversion 和 hidden copy 如何被发现？
- stream、event、execution context 和 buffer lifetime 如何协调？
- FP16/dynamic shape 为什么可能改变 kernel selection？
- kernel 更快为什么不一定降低 end-to-end latency？
- Orin power/thermal 状态为何是 benchmark contract 的一部分？

## 最终完成标准

- [ ] 三份 case study 有 raw data、correctness 和限制。
- [ ] capstone 可从 clean checkout 重现。
- [ ] 所有性能数字注明 Orin/JetPack/power/thermal。
- [ ] 讲解包含一次被证据推翻的 hypothesis。
- [ ] 下一阶段依据能力缺口选择，而非追逐工具名称。

## 下一阶段候选

- 深入 TensorRT plugin、allocator、CUDA Graph/runtime internals。
- 选择 transformer/KV-cache workload 补模型类型。
- 深入 Triton/MLIR/TVM compiler stack。
- 做 DeepStream/camera/multi-sensor edge pipeline。
- 在更大 NVIDIA GPU 上学习 multi-GPU/NCCL/serving。
