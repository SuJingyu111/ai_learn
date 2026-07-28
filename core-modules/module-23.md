# Core Module 23 — Reliability、CI 与受控 Agent

## 本周目标

把实验代码提升为可交付工程：clean build、failure path、资源清理、CPU CI、Orin
验证流程和最小权限的辅助 Agent。

## 每日计划

### Day 1 — Clean Reproducibility Audit

- **10 分钟**：列 host CPU、Orin CUDA、runtime/capstone 三条 build/run paths。
- **20 分钟**：创建新 build directories，不复用本机 cache。
- **50 分钟**：按 README 执行 configure/build/test/model/engine/run；记录缺失依赖和
  hidden setup。
- **10–30 分钟**：修正文档或 scripts，重跑失败步骤。
- **必须留下**：clean transcript 和 host/Orin support matrix。

### Day 2 — Failure Injection

- **10 分钟**：选择 OOM、invalid shape、bad metadata、early shutdown 中至少三个。
- **15 分钟**：定义 expected error、cleanup 和 process state。
- **55 分钟**：实现 tests/controlled flags，验证错误不会变 silent success、partial
  output 或后续 sticky state。
- **10–30 分钟**：运行正常 path regression。
- **必须留下**：failure matrix、diagnostics 和 cleanup evidence。

### Day 3 — RAII/Lifetime Audit

- **10 分钟**：画 destructor order：work completion→context/engine→stream/events→buffers。
- **20 分钟**：审阅 CUDA/ORT/TensorRT owners、move/copy semantics 和 error exits。
- **50 分钟**：修复高风险 manual cleanup 或集中 RAII；不做无关重构。
- **10–30 分钟**：重复 early-return/failure tests，使用 host sanitizer/tool（可用时）。
- **必须留下**：ownership audit、修复和 residual risks。

### Day 4 — CI 与 Orin Gate

- **10 分钟**：列 hosted CPU CI 能验证与不能验证的内容。
- **20 分钟**：审阅 `.github/workflows/ci.yml` 的 compilers/permissions/timeouts。
- **50 分钟**：补 CPU tests/format/smoke；为 Orin 写明确 manual/self-hosted checklist，
  不在无 GPU job 假标 CUDA pass。
- **10–30 分钟**：本地模拟 CI commands；记录 artifact fields。
- **必须留下**：CI scope table 和 Orin validation checklist。

### Day 5 — Reviewer Agent Evaluation

- **10 分钟**：从 bounds、async free、benchmark sync 中选一个已知缺陷 fixture/diff。
- **15 分钟**：写 expected findings 和禁止事项。
- **45 分钟**：让 read-only CUDA reviewer审查，记录 findings、false positives、
  misses 和 code evidence quality。
- **20–40 分钟**：改进 repository/agent instructions，而不是让 Agent 自批自合。
- **必须留下**：review evaluation report 和权限边界。

### Day 6 — Release/Trust Checklist

- **15 分钟**：检查 secret/path/trace/model license/large artifact 风险。
- **35 分钟**：完成 release checklist、MCP/tool trust boundary、human approval。
- **20 分钟**：运行 final CPU/Orin test summary，列 unavailable gates。
- **20–40 分钟**：weekly review，冻结 Week 24 只做交付，不再增加 feature。
- **必须留下**：release readiness 和明确 residual risks。

## 工程原则

- 外部 issue、logs、model metadata 和 tool output 都是不可信输入。
- CI 没有 GPU 时明确 CPU-only，不将 CUDA 标记为 passed。
- benchmark regression 与 correctness regression 分开。
- release、merge、设备配置修改和 secrets 需要明确权限。

## 交付

- clean build/test record。
- failure-path tests。
- Agent review quality note。
- Orin manual/self-hosted validation checklist。

## 完成标准

- [ ] 正常和主要失败路径不泄漏资源。
- [ ] CI scope 与缺失的 GPU coverage 明确。
- [ ] Agent 没有 merge/secret/设备管理权限。
- [ ] 项目可由文档重新部署。
