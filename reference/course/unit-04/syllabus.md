# Unit 4 Syllabus — GPU Performance 与 CUDA Preprocessor

**Weeks 19–24 · M4 · Gate G4**

## Entry prerequisite

G3 PASS；能够实现边界安全 kernel、解释 warp memory access、正确 timing 并使用
Compute Sanitizer。

## Observable outcomes

1. 区分 Nsight Systems 与 Nsight Compute 的问题范围；
2. 用 bandwidth、transactions、occupancy、warp stalls、operational intensity 形成假设；
3. 冻结 Resize coordinate/pixel-center/border/rounding/tolerance contract；
4. 先建立 independent CPU oracle，再实现 naive/optimized CUDA；
5. 用 controlled A/B 和 raw samples 支持或推翻至少一个性能假设；
6. 将 resize、normalize、layout conversion 合并并解释收益适用范围。

## Concept inventory

`NVTX`、`timeline`、`kernel metric`、`effective bandwidth`、`coalescing`、`shared memory`、
`bank conflict`、`occupancy`、`latency hiding`、`utilization`、`operational intensity`、
`roofline`、`inverse mapping`、`pixel center`、`half_pixel`、`align_corners`、`border`、
`interpolation`、`first mismatch`。

## Primary teaching spine

- [CUDA Best Practices](https://docs.nvidia.com/cuda/cuda-c-best-practices-guide/index.html)：
  Timing、Bandwidth、Coalesced Access、Shared Memory、Occupancy。
- [Nsight Systems User Guide](https://docs.nvidia.com/nsight-systems/UserGuide/index.html)：
  CUDA trace、CLI profile/stats、timeline。
- [Nsight Compute Profiling Guide](https://docs.nvidia.com/nsight-compute/ProfilingGuide/index.html)：
  LaunchStats、Occupancy、WarpStateStats、MemoryWorkloadAnalysis；Roofline 为补充。
- [ONNX Resize specification](https://onnx.ai/onnx/operators/onnx__Resize.html)：
  coordinate transformation、nearest/rounding、antialias。
- [OpenCV geometric transforms](https://docs.opencv.org/master/da/d54/group__imgproc__transform.html)：
  只用于对照 API 和独立 oracle，不替代已冻结 contract。

## Six-week lesson sequence

| Week | Theory | Worked problem | Lab | Project increment |
|---:|---|---|---|---|
| 19 | profiler mental model/NVTX | timeline→candidate kernel | Systems→Compute workflow | evidence harness |
| 20 | transpose/reduction/shared/occupancy | bytes/transactions/occupancy | naive vs tiled experiments | performance primitives |
| 21 | sampling/Resize semantics | output→source coordinate table | semantic fixtures | contract freeze |
| 22 | CPU oracle | border/rounding cases | independent CPU Resize | M4 oracle |
| 23 | CUDA Resize + Nsight | access/branch hypothesis | naive CUDA + profiler | M4 CUDA |
| 24 | controlled optimization/case study | A/B interpretation | optimized/fused variant | M4 / G4 |

## Required problem set

- 从一个 timeline 选择正确 profiler，不先打开几十个 metric。
- 计算 copy/transpose/resize 的 useful bytes 与 effective bandwidth。
- 对 `half_pixel`、`asymmetric`、`align_corners` 各手算 4 个 source coordinates。
- 设计 single-pixel、odd、padded-stride、border、randomized test matrix。
- 给出“occupancy 提高但 kernel 变慢”和“fusion 不值得”的反例。

## Reference-solution quarantine

现有 `src/cpu/resize_cpu.cpp` 与 `src/cuda/resize_cuda.cu` 包含参考实现。G4 前必须在
`labs/resize_starter/`、个人 namespace 或独立 starter branch 实现，不能打开参考文件。
Gate 后才能做 differential review，并明确哪些设计是独立得出。

## Sidecar prerequisite chain

基于 U3 的 stdio skeleton，依次实现 schema/errors/tests、resources/prompts、Inspector，
再切换 Streamable HTTP，学习 authorization 和 security。协议统一记录 stable
`2025-11-25` revision。最终是 read-only Runtime Lab Assistant v0；不接受 arbitrary
path/shell。

## G4

- closed-book profiler/Resize/performance exam；
- unseen coordinate/border variant 或 CUDA bug diagnosis；
- single-pixel/odd/padded/random/border CPU↔CUDA 全通过；
- raw benchmark + one-variable A/B + Nsight evidence + case-study defense。

语义不一致、只看截图、缺少 raw samples 或 reference-solution leakage 均阻塞 U5。
