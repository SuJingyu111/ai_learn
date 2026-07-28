# Unit 6 · Week 33 — TensorRT、`trtexec` 与 Object Model

> **课程角色**：Unit 6 的入口课。把上一单元的 ONNX/ORT correctness oracle 转成
> TensorRT FP32 baseline，并建立 builder、serialized engine、runtime、execution
> context 的生命周期模型。Sidecar 聚焦 multi-agent sequential/parallel 模式选择。
>
> 课程导航：[总纲](../CURRICULUM.md) · [资料索引](../docs/resources.md) ·
> [Mastery Gates](../docs/mastery-gates.md)

## 硬先修与 remediation

- **硬先修**：冻结的 ONNX model/input/output contract、同一 input 的 framework/ORT
  reference、可登录的 Jetson Orin、可写 artifact 目录。
- **版本 pin（阻塞项）**：在 Orin 记录 `/etc/nv_tegra_release`、JetPack/L4T、
  `dpkg-query -W 'libnvinfer*'`、`trtexec --version`。从
  [TensorRT Archives](https://docs.nvidia.com/deeplearning/tensorrt/archives/) 选择与
  **设备实际 TensorRT major.minor 完全一致**的文档，并把最终 URL 写入
  `artifacts/week-33/trt-version-pin.md`。禁止直接以 `latest` 文档替代设备版本。
- **Remediation**：缺 reference 时，用 Day 1 的后 45 分钟先生成一个 fixed input/output；
  版本不明时只做 inventory，不 build engine；无 Orin 时保留 blocked reproduction，
  不把 host 结果标成通过。

## Observable objectives（可观察目标）

完成后能够：

1. 从实际版本 manifest 选择正确 TensorRT 文档与命令参数。
2. 用 `trtexec` 分离 parse/build/serialize 与 warm-up/steady inference。
3. 画出 builder→engine→runtime→context→enqueue 的 object/lifetime 图。
4. 为一个任务依据 dependency 与 shared-write risk 选择 sequential 或 parallel agents。

## 术语表

| 术语 | 本周可操作定义 |
|---|---|
| Builder | 解析 network、选择 tactics 并生成 serialized engine 的构建期对象 |
| Engine | 与 TensorRT/硬件/shape/precision 条件绑定的序列化执行计划 |
| Runtime | 反序列化 engine 的运行期 owner，不等同于 builder |
| Execution context | 某个 engine 的一次执行状态；保存 shape、tensor address 等 |
| Tactic | builder 为 layer/shape 选择的候选实现 |
| Steady state | 排除 build、load、first-run 初始化后的重复执行窗口 |

## 必修理论材料（分钟计入对应 Day）

| ID | 精确 URL 与章节/页面 | 分钟 | Day |
|---|---|---:|---:|
| T1 | [JetPack Archive](https://developer.nvidia.com/embedded/jetpack-archive) — 与设备 L4T 对应的 release page，核对 “JetPack Components” | 10 | 1 |
| T2 | [TensorRT Archives](https://docs.nvidia.com/deeplearning/tensorrt/archives/) — 进入已 pin 的 Developer Guide，读 “Command-Line Programs → `trtexec`” | 20 | 1 |
| T3 | [Microsoft Agent Framework — Workflow orchestrations](https://learn.microsoft.com/en-us/agent-framework/workflows/orchestrations/) — sequential、concurrent、handoff 的执行与选择机制 | 10 | 4 |
| T4 | [GH-600 Study Guide](https://learn.microsoft.com/en-us/credentials/certifications/resources/study-guides/gh-600) — **仅用于** blueprint/domain/skill mapping，不作为 orchestration 教材 | 5 | 4 |

若设备恰为 TensorRT 10.3.0，可使用这个
[10.3.0 Developer Guide PDF](https://docs.nvidia.com/deeplearning/tensorrt/archives/tensorrt-1030/pdf/TensorRT-Developer-Guide.pdf)；
否则它只能作为 URL 形态示例，必须换成实际版本 archive。

## 带问题阅读

- `trtexec` 的 build 指标与 inference 指标为何不能直接相加或横向比较？
- engine 为什么不能脱离生成它的版本、硬件与 profile 条件解释？
- 哪些任务有严格 dependency，错误并行会制造什么共享状态冲突？

## Worked example

一个 ONNX 模型的可信记录不是“latency=2 ms”，而是：

```text
parse/build 18.4 s -> serialize engine 6.2 MiB
load 41 ms -> warm-up 20 runs -> steady 100 runs
input=(1,3,224,224), FP32, power mode=P, TensorRT=X.Y
```

build time 不进入 steady latency；两个 agents 可分别检查 version manifest 与 ONNX
metadata，但 engine build 必须等待两者通过，且只能有一个 canonical engine writer。

## 累计项目 increment

- `artifacts/week-33/trt-version-pin.md`
- `artifacts/week-33/fp32-engine-manifest.md`
- `artifacts/week-33/trtexec.log`
- `notes/concepts/tensorrt-build-runtime.md`
- `notes/ai-app/week-33.md`（multi-agent pattern decision record）

## 本周必须产出

- 可复现的 `trtexec` FP32 build/run、engine identity 与 timing boundary。
- TensorRT object/lifetime 图及最小 engine inspector。
- sequential/parallel decision matrix、一个安全 dry-run 和 3–5 个 GH-600 scenarios。

## Day 1 — 理论、版本诊断与 first engine（105 分钟）

- **20 分钟**：采集 Orin/JetPack/L4T/CUDA/TensorRT manifest；不修改 power mode。
- **10 分钟**：阅读 T1，核对 JetPack bundled components。
- **20 分钟**：阅读 T2，只读已 pin 版本的 `trtexec` 页面。
- **45 分钟**：检查 ONNX names/shapes/dtypes/reference，执行最小 FP32 build；失败时保存
  first blocker，不从网络下载不明 engine/wheel。
- **10 分钟**：写 daily log 与 resolved archive URL。

**必须留下**：`trt-version-pin.md`、fixed model contract 和 first build transcript。

## Day 2 — Guided lab：可信 `trtexec` baseline（110 分钟）

- **15 分钟**：从 worked example 标出 build/load/warm-up/steady 边界。
- **70 分钟**：在相同 model/input/power 条件下 build、save engine、load、warm-up、
  steady run；保存完整 command、layer info、engine size、raw output，不只截屏。
- **15 分钟**：画 parse→build→serialize→load→context→enqueue timeline。
- **10 分钟**：列出当前数据可回答与不可回答的问题。

**必须留下**：`fp32-engine-manifest.md`、`trtexec.log` 和 timing-stage table。

## Day 3 — Independent lab：Object inspection（105 分钟）

- **15 分钟**：闭卷写 builder、engine、runtime、context 的 owner 与 destruction order。
- **70 分钟**：创建/扩展 `labs/runtime/week18_trt_runner.cpp`，只做 deserialize 与
  engine inspection；打印 I/O tensor name/mode/shape/dtype/format，并与 ONNX contract
  对照，遇到 unknown/dynamic shape 先显式报告。
- **20 分钟**：clean compile/run，保存 exact build command 与 metadata diff。

**必须留下**：可运行 inspector、I/O metadata table 和 object ownership 草图。

## Day 4 — Integration：Object model × agent pattern（110 分钟）

- **15 分钟**：阅读 T3+T4；从 T3 学机制，用 T4 只定位 blueprint/domain，并写
  pattern selection 的三个约束。
- **60 分钟**：把 inspector 接入 engine manifest；检查 engine/version/hash 不匹配时
  fail closed，完成 `notes/concepts/tensorrt-build-runtime.md`。
- **25 分钟**：为 “version check→model check→build→review” 画 dependency graph，比较
  sequential 与受控 parallel；所有模拟 worker 只读、输出路径隔离。
- **10 分钟**：记录 join condition、owner 与 conflict signal。

**必须留下**：object/lifetime 图和 `notes/ai-app/week-33.md` pattern decision record。

## Day 5 — Assessment：解释、复现与场景题（105 分钟）

- **60 分钟**：从空 build 目录重跑一条 build/load/inspect 路径；逐项核对 source、
  model、engine、environment 与 raw log provenance。
- **30 分钟**：写 3–5 个 GH-600 scenarios，覆盖 strict dependency、parallel read、
  shared writer、reviewer 与 stalled worker；解释错误选项。
- **15 分钟**：将 evidence 链到 `docs/gh600-checklist.md`，做一次 secret/path 检查。

**必须留下**：clean reproduction、3–5 题解析和 checklist evidence link。

## Day 6 — Synthesis 与 Exit ticket（100 分钟）

- **40 分钟**：clean rerun `trtexec` 与 inspector；补齐任何 version/timing gap。
- **20 分钟**：闭卷口述 engine/context 区别及 enqueue 前后状态。
- **25 分钟**：按 “Mechanism→Evidence→Limitation→Next action” 写周总结。
- **15 分钟**：冻结 W34 使用的 engine/input/reference 与 remediation list。

**必须留下**：Week 33 evidence index、口述答案和 W34 frozen inputs。

## Exit ticket 与 unlocks

形成性要求：闭卷 5 题首次 ≥4/5、90 秒口述、1 个 unseen input、随机 3 个术语；
未过项在下一周 Day 1 前用新题订正。

- 能指出一个 build-time metric、一个 run-time metric，以及混淆它们会产生的错误结论。
- 能从 manifest 找到设备实际 TensorRT 文档 pin；没有 pin 则本周不通过。
- 能说明为什么 execution context 不是 engine。
- **Unlock W34**：FP32 engine 可复现、inspector metadata 与 ONNX contract 一致。

## Optional Feed Card（30–45 分钟，不计核心时长）

- **来源/选择规则**：从 [NVIDIA Technical Blog · TensorRT](https://developer.nvidia.com/blog/tag/tensorrt/)
  只选 1 篇直接区分 engine build、load 或 steady inference 的文章；无法映射到实机
  TensorRT pin 的内容不选。
- **Artifact**：写 `notes/feed/week-33.md`，固定四栏
  `claim → evidence → scope → decision`，decision 只能是 adopt/test/reject/defer。
- **取消规则**：任一核心 Day 落后或存在 blocker 时直接取消，不补做、不挤占核心时长。
