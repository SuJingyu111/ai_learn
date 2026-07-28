# Runtime 学习主线说明

正式 Runtime curriculum 已重构为 8 个有硬依赖的 Unit。这里解释学习边界与旧 24 个
Core Module 如何使用；日常任务仍只从 [`weeks/`](weeks/) 进入。

## 正式课程

| Unit | Weeks | Syllabus | Gate |
|---|---:|---|---|
| U1 | 1–6 | [`course/unit-01/syllabus.md`](course/unit-01/syllabus.md) | tensor/math/CPU oracle |
| U2 | 7–12 | [`course/unit-02/syllabus.md`](course/unit-02/syllabus.md) | ONNX/ORT CPU/measurement |
| U3 | 13–18 | [`course/unit-03/syllabus.md`](course/unit-03/syllabus.md) | CUDA foundations |
| U4 | 19–24 | [`course/unit-04/syllabus.md`](course/unit-04/syllabus.md) | profiler/CUDA preprocessor |
| U5 | 25–30 | [`course/unit-05/syllabus.md`](course/unit-05/syllabus.md) | async/lifetime |
| U6 | 31–36 | [`course/unit-06/syllabus.md`](course/unit-06/syllabus.md) | ORT CUDA/TensorRT |
| U7 | 37–42 | [`course/unit-07/syllabus.md`](course/unit-07/syllabus.md) | Orin sustained beta |
| U8 | 43–48 | [`course/unit-08/syllabus.md`](course/unit-08/syllabus.md) | production release |

课程不假设 ML 经历。U1–U2 提供专门 bridge：tensor/storage/stride、运行时所需线性代数、
MatMul/Conv、model lifecycle、graph/ONNX/ORT。它们不是 CUDA 前的“概念速览”，而是
后续 correctness 和 runtime reasoning 的硬先修。

## 能力边界

必须掌握：

- tensor shape/stride/storage/layout/dtype 与地址；
- MatMul/Conv/Resize 等 operator 的 shape/dataflow/bytes；
- model lifecycle、ONNX IR、provider/fallback；
- CUDA execution/memory/sync/error/timing；
- CPU/CUDA oracle、tolerance、boundary/random/stress tests；
- Nsight Systems/Compute 的问题分工；
- streams/events/ownership/lifetime；
- ORT I/O Binding/user stream；
- TensorRT engine/context/FP16/dynamic profile；
- Orin power/clock/thermal/reliability；
- evidence-backed reproduction 和 technical defense。

只建立概念或 elective：

- backprop/optimizer 的细节；
- affine/perspective warp；
- CUDA Graphs；
- INT8 calibration、DLA、custom plugin；
- camera/DeepStream；
- distributed training/serving。

## 正式学习循环

```text
prerequisite retrieval
  → systematic lesson
  → worked derivation
  → guided experiment
  → independent new variant
  → cumulative project
  → Exit Ticket / Major Gate
```

只读文档、只跑 sample 或只复述术语不算掌握。每个核心 claim 必须能链接：

```text
source → derivation/minimal experiment → raw evidence → scoped conclusion
```

## 旧版 24 个 Core Modules

[`core-modules/`](core-modules/) 保留了旧版 intensive 内容，适合：

- 某个 Gate 失败后的 targeted remediation；
- 已通过 Gate 后的专题加深；
- 查找额外实验 idea。

它们不再与 calendar week 一一绑定，也不应覆盖当前 Unit 的 prerequisite 顺序。尤其：

- 旧 affine warp Core 已降为 elective；
- CUDA Graphs 只有在 launch overhead 有证据时 elective；
- TensorRT 已扩成整个 U6，不再只靠两周速成；
- capstone model、runtime input、normalize/layout contract 在 U2 冻结；Resize 的精确
  coordinate/border/rounding contract 在 U4 学完后冻结，不等到 Week 41；
- `src/cpu/resize_cpu.cpp` 与 `src/cuda/resize_cuda.cu` 是 reference，G4 前封存。

## 卡住时

1. 沿 [`docs/concept-map.md`](docs/concept-map.md) 的 DAG 向上找缺失先修；
2. 回到 Unit syllabus 的 primary teaching spine；
3. 用 [`notes/templates/concept-note.md`](notes/templates/concept-note.md) 重建机制；
4. 做一个更小的 worked example 和新 input；
5. 按 [`docs/mastery-gates.md`](docs/mastery-gates.md) 记录 remediation/retest。
