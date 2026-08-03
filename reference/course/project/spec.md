# 累计项目最终规格

## 项目问题

在 Jetson Orin 上构建一个 correctness-first、device-resident、可测量和可复现的
inference pipeline，并提供一个只读 Runtime Lab Assistant 来验证、比较和解释
benchmark evidence。

## Runtime 产品

```text
versioned input
  → CUDA resize / normalize / layout conversion
  → device tensor
  → RuntimeBackend
      ├─ ONNX Runtime CUDA
      └─ TensorRT
  → postprocess / comparator
  → structured benchmark bundle
```

最低接口 contract：

- input shape、dtype、layout、stride、value range；
- Resize coordinate transformation、border、rounding、tolerance；
- buffer ownership、lifetime、stream/event dependency；
- backend、precision、dynamic profile；
- error propagation 和 shutdown；
- environment、power/thermal 与 measurement boundary。

## AI Application 产品

Runtime Lab Assistant 读取固定 schema 的 artifact store：

```text
resources:
  runtime://runs/{run_id}/manifest
  runtime://runs/{run_id}/metrics
  runtime://runs/{run_id}/correctness

read-only tools:
  list_runs(filter)
  validate_run(run_id)
  compare_runs(baseline_id, candidate_id)
```

它不得：

- 接收任意 filesystem path 或 shell command；
- 修改 Orin power mode、部署或 benchmark data；
- 暴露 token、secret、个人目录或无关 repository；
- 将模型输出当作授权；
- 在缺少 raw evidence 时宣称 optimization 成功。

## 结构化 evidence bundle

每次代表性 run 至少包含：

- `manifest.json`：commit、host/device、JetPack/CUDA/TensorRT/ORT、model hash；
- `workload.json`：input set、shape、precision、warm-up、iterations；
- `correctness.json`：oracle、tolerance、max/mean/task-level error；
- `latency.csv`：raw samples；
- `system.csv`：power mode、clocks、temperature、GR3D/EMC/memory/power；
- profiler command、report summary 与原始 artifact 路径；
- conclusion、limitation、rollback decision。

## 最终验收

- clean checkout 能生成/获取 model、build、deploy、run；
- CPU/PyTorch/ORT CPU/ORT CUDA/TensorRT correctness chain 可追溯；
- normal、boundary、odd/dynamic、stress 和 failure cases；
- 无未解释 fallback、copy 或 synchronization；
- 固定 power mode 下 sustained measurement；
- 三份 case study：CUDA preprocessor、async pipeline、runtime integration；
- Runtime Lab Assistant schema/security/eval/audit tests；
- 10 分钟演示和随机追问。

## Non-goals

- 训练大型模型或追求 SOTA accuracy；
- 同时支持很多 image operators、runtime 或模型；
- 在 Core Gate 前做 DLA、INT8 calibration、custom plugin、camera/DeepStream；
- 让 agent 直接控制设备或执行任意代码；
- 用 desktop GPU 结果代替 Orin 结论。

