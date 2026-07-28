# Unit 7 Syllabus — Orin End-to-end Systems Engineering

**Weeks 37–42 · M7 · Gate G7**

## Entry prerequisite

G6 device-resident ORT CUDA/TensorRT vertical slice PASS；实际 Orin 可用。

## Observable outcomes

1. 冻结并验证 end-to-end beta contract，不在此时首次选择 model/input；
2. 追踪 allocator、copy、layout conversion、sync 和 runtime phases；
3. 解释 FP16/dynamic results，并掌握量化 scale/zero-point/QDQ 的运行时语义；
4. 在明确 stream/dependency 下连接 preprocessor 与 backend；
5. 在固定 power mode、clocks 和 thermal 条件下做 sustained benchmark；
6. 完成 failure injection、rollback 和 Runtime Lab Assistant evidence validation。

## Concept inventory

`device-resident`、`allocator`、`hidden copy`、`implicit sync`、`precision gate`、
`scale`、`zero-point`、`saturation`、`per-tensor/per-channel`、`accumulator`、
`calibration`、`Q/DQ`、`DVFS`、`nvpmodel`、`jetson_clocks`、`thermal throttling`、
`tegrastats`、`sustained benchmark`、`failure injection`、`rollback`。

## Primary teaching spine

- [TensorRT Quantized Types](https://docs.nvidia.com/deeplearning/tensorrt/10.x.x/inference-library/work-quantized-types.html)
- [CUDA for Tegra](https://docs.nvidia.com/cuda/cuda-for-tegra-appnote/)
- [JetPack downloads and release notes](https://developer.nvidia.com/embedded/jetpack/downloads)
- [Jetson Linux documentation archives](https://docs.nvidia.com/jetson/archives/)
- [Tegrastats Utility](https://docs.nvidia.com/jetson/archives/r39.2/DeveloperGuide/AT/JetsonLinuxDevelopmentTools/TegrastatsUtility.html)
- [Orin Power and Performance](https://docs.nvidia.com/jetson/archives/r39.2/DeveloperGuide/SD/PlatformPowerAndPerformance/JetsonOrinNanoSeriesJetsonOrinNxSeriesAndJetsonAgxOrinSeries.html)

上面 r39.2 页面是当前示例；设备不是 r39.2 时必须换成匹配 archive。MAXN 不保证最高
持续性能；`jetson_clocks` 仍受当前 power mode 约束。

## Six-week lesson sequence

| Week | Theory | Worked problem | Lab | Project increment |
|---:|---|---|---|---|
| 37 | beta contract/end-to-end phases | copy/sync inventory | clean deploy baseline | M7 alpha |
| 38 | precision/dynamic/quantization | scale/zero-point/QDQ | accuracy stress | precision report |
| 39 | device-resident preprocessing | ownership/allocator | direct buffer path | hidden-copy removal |
| 40 | stream integration/timeline/security | dependency trace | end-to-end Nsight | integrated beta |
| 41 | power/thermal/reliability | sustained dataset interpretation | tegrastats/failures | hardened beta |
| 42 | sustained release/evidence service | run validation | compare/validate runs | M7 / G7 |

完整 INT8 calibration、DLA 和 custom plugin 为 elective；必须在 Core 全部通过后进行。

## Required problem set

- 从 timeline 标出 allocator、copy、sync、preprocess、runtime、postprocess。
- 手算 affine quantization/dequantization，并识别 saturation 和 accumulator risk。
- 比较两组不同 temperature/power mode 数据，判断为何不能直接声称加速。
- 设计 invalid shape、OOM、engine mismatch、thermal throttle、interrupted run 的 rollback。
- 为 benchmark bundle 设计 schema/path/injection negative cases。

## Sidecar prerequisite chain

Week 37–40：autonomy risk、HITL、authorization、audit、prompt injection/tool misuse/data
exfiltration red team。Week 41–42：GitHub remote MCP read-only integration、registry/allowlist
局限、standard Actions CI。Registry/allowlist 不是可靠安全边界；policy/entitlement 限制
必须在报告中明确。

## G7

- closed-book Orin/quantization/power/reliability/security exam；
- unseen failure 或 invalid-measurement diagnosis；
- clean host→Orin deploy、normal/boundary/stress correctness、固定条件 sustained data；
- no unexplained copy/sync、failure/rollback、MCP schema/path/injection tests。

缺少 power/thermal 元数据、measurement invalid 或 security regression 均阻塞 U8。

