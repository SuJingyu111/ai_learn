# Benchmark Methodology

## 必填环境

每次 run 都要有：

- 日期、Git commit（或明确标记 `unversioned`/`dirty`）、主机和 OS
- **CPU 型号**、RAM
- build type、compiler、**实际生效的 flags**（不是"Release"三个字）

跑在 GPU 上时追加：

- GPU、compute capability、CUDA、TensorRT、cuDNN

跑在 Orin 上时再追加：

- Orin 型号、内存、JetPack/L4T
- power mode、clock policy
- benchmark 前后温度、thermal throttling 和后台负载

`engine/bench/benchmark_resize.cpp` 是照这份清单实现的参照：commit 和 CPU 型号
走环境变量（`TT_COMMIT`/`TT_CPU`，由 `scripts/run_benchmarks.sh` 填），
build type、compiler 和 flags 由 CMake 在编译期注入。

## 必填工作负载

- 输入 / 输出 shape、channel、dtype、layout
- operation、interpolation、border mode
- warm-up 次数、测量次数、同步位置
- 是否包含 allocation、H2D、D2H、engine build

## 输出指标

- median、p95、min；不要只报最佳值
- kernel-only latency
- transfer-only latency
- end-to-end latency
- throughput / effective bandwidth（适用时）
- first-frame 与 sustained steady-state（pipeline 适用时）
- memory high-water mark 和 power/energy telemetry（可用且方法明确时）
- correctness tolerance 和最大误差

## 公平性检查

1. 所有 backend 使用相同输入和输出语义。
2. steady-state latency 不包含一次性 engine build；另行报告 build time。
3. CUDA event 测量 GPU work；host clock 测量 end-to-end。
4. 在计时外完成日志输出与结果验证。
5. 保存原始 CSV/JSON，不只保存图表。
6. 不将冷机短跑数据与达到稳态温度后的 sustained run 直接比较。
7. 更改 JetPack、power mode、clocks、散热或模型版本后重新建立 baseline。

使用 [`notes/templates/experiment.md`](../notes/templates/experiment.md) 记录实验。
