# 累计项目 Milestones 与依赖

每个 milestone 必须复用上一个 milestone 的 artifact，而不是每 6 周另做一个无关
demo。M2 冻结 model/runtime-input 等早期 contract，M4 冻结完整 Resize semantics；
后续只允许通过 ADR 做有依据的变更。

| Milestone | Week | Runtime increment | Runtime Lab Assistant increment | Entry dependency |
|---|---:|---|---|---|
| M1 | 6 | CPU normalize/HWC→CHW + tiny model vertical slice | GitHub control-plane dossier | Course 0 |
| M2 | 12 | Frozen model/dataset/runtime-input/normalize/layout；PyTorch→ONNX→ORT CPU | typed failure-aware read-only tool | G1 |
| M3 | 18 | CUDA normalize/HWC→CHW primitives + sanitizer evidence | MCP lifecycle/capability traces + stdio skeleton | G2 |
| M4 | 24 | independent CPU/CUDA Resize + profiler case study + fusion | local/HTTP read-only server v0 + auth/security tests | G3 |
| M5 | 30 | frame slots、events、double-buffer、stress/shutdown | durable state + versioned eval dataset | G4 |
| M6 | 36 | ORT CUDA + TRT `RuntimeBackend`、device I/O、FP16/dynamic | evidence reviewer/multi-agent fixture | G5 |
| M7 | 42 | Orin beta、BenchmarkRecorder、sustained/failure evidence | `list/validate/compare runs` + GitHub/CI security | G6 |
| M8 | 48 | production release、3 case studies、reproduction | Runtime Lab Assistant v1、threat model、eval、GH evidence | G7 |

## Contract freeze 与变更

Week 9–12 必须冻结：

- model/version/hash；
- input dataset manifest；
- runtime input tensor 的 shape/dtype/layout/value range；
- normalize/layout semantics；
- fixed/dynamic shapes；
- FP32 reference tolerance；
- runtime output/postprocess；
- benchmark warm-up、iteration、latency metrics。

Week 21–24 学完 Resize 后再冻结 coordinate transform、pixel-center、interpolation、
border、rounding 与 Resize tolerance。M2 只记录计划目标尺寸，不凭未学过的语义提前
签署完整 Resize contract。

之后如需改变，创建 ADR，说明：

1. 原 contract；
2. 新证据和变更原因；
3. correctness/performance 数据是否仍可比较；
4. migration 和 rollback；
5. 哪些旧 milestone 必须重测。

## Milestone Definition of Done

每个 milestone 都必须：

- 从上一个已通过 tag/commit 构建；
- 有代码、tests、raw evidence 和一页设计说明；
- 能在 clean directory 运行代表 case；
- 清楚标记 mandatory、elective 和未完成项；
- 通过对应 [`Mastery Gate`](../../docs/mastery-gates.md)；
- 失败时保留 first error、hypothesis、修复和 regression test。
