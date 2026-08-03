# Unit 6 · Week 35 — FP16、Dynamic Profiles 与 Quantization Preview

> **课程角色**：在 W34 runner 上引入一个变量一次的 precision/shape flexibility，
> 建立 FP32/FP16 correctness contract 与 min/opt/max profile。Quantization 只做概念和
> Q/DQ 数值预览，INT8 engine 是选修、不是 gate blocker。Sidecar 聚焦 explicit handoff。
>
> 课程导航：[总纲](../CURRICULUM.md) · [资料索引](../docs/resources.md) ·
> [Mastery Gates](../docs/mastery-gates.md)

## 硬先修与 remediation

- **硬先修**：W34 C++ runner、FP32 reference/raw samples、engine/TensorRT pin、已知
  fixed shape correctness。
- 继续使用实际 JetPack/TensorRT archive；若 `libnvinfer` 或 JetPack 改变，先使
  W33–34 baseline invalid，再重建。禁止从非匹配版本复制 FP16/profile flags。
- **Remediation**：FP32 尚未通过时，用 Day 1 的后 50 分钟修 reference/runner；
  无真实 dynamic-shape 需求时选一个有业务意义的 spatial 或 batch range，不能把所有
  dimensions 设 dynamic；INT8 不成功不阻塞。

## Observable objectives（可观察目标）

完成后能够：

1. 用 representative set 和有理由的 thresholds 判断 FP16 correctness。
2. 构建并检查 FP16 engine，报告 reformat/fallback 与性能边界。
3. 定义 min/opt/max profile，正确处理 in-range 与 out-of-range shapes。
4. 生成带 artifact hashes、scope、open questions 的 builder→reviewer handoff。

## 术语表

| 术语 | 本周可操作定义 |
|---|---|
| FP16 | exponent/range 更窄的 16-bit floating-point；速度收益不能假定 |
| Accuracy contract | 输入集、metrics、thresholds 与 downstream task checks 的组合 |
| Optimization profile | dynamic inputs 的 min/opt/max build-time 约束 |
| Runtime shape | context 在 enqueue 前设置的本次实际 dimensions |
| Q/DQ | 显式 quantize/dequantize 节点表达的 scale/zero-point 边界 |
| Handoff | versioned、可验证、最小充分的 agent state transfer artifact |

## 必修理论材料（分钟计入对应 Day）

| ID | 精确 URL 与章节/页面 | 分钟 | Day |
|---|---|---:|---:|
| T1 | [TensorRT Archives](https://docs.nvidia.com/deeplearning/tensorrt/archives/) — 已 pin guide 的 “Accuracy Considerations / Reduced Precision” | 20 | 1 |
| T2 | [TensorRT Archives](https://docs.nvidia.com/deeplearning/tensorrt/archives/) — 已 pin guide 的 “Working with Dynamic Shapes / Optimization Profiles” | 25 | 3 |
| T3 | [Microsoft Agent Framework — Handoff orchestration](https://learn.microsoft.com/en-us/agent-framework/workflows/orchestrations/handoff) — context synchronization、termination、checkpoint 与 approval handoff | 10 | 4 |
| T4 | [GH-600 Study Guide](https://learn.microsoft.com/en-us/credentials/certifications/resources/study-guides/gh-600) — **仅用于** blueprint/domain/skill mapping，不作为 handoff/state 教材 | 5 | 4 |
| T5 | [TensorRT 10.x Accuracy Considerations](https://docs.nvidia.com/deeplearning/tensorrt/10.x.x/inference-library/accuracy-considerations.html) — “Quantization Errors”；仅在设备为 TRT 10.x 时作补充 | 15 | 5 |

T4 不是版本 pin 的替代；非 10.x 设备改读实际 archive 的同名章节并记录 resolved URL。

## 带问题阅读

- FP16 的 max error 为何不足以代表 downstream correctness？
- opt shape 的“opt”是什么意思，为什么不保证所有 in-range shapes 同样快？
- handoff 中哪些是事实、推断、建议；consumer 如何验证？

## Worked example

```text
profile: N=1, C=3, H/W min=224, opt=384, max=640
request 512x512 -> in range; calculate bytes for 512, then set shape, then enqueue
request 800x800 -> reject before allocation/enqueue
FP16 acceptance -> max_abs + p99_abs + task-level top-k agreement
```

量化预览：若 `x=1.2, scale=0.1, zero_point=0`，`q=12`，反量化为 `1.2`；若超范围则
clamp，误差来源必须区分 rounding 与 clipping。

## 累计项目 increment

- `artifacts/week-35/fp16-engine-manifest.md`
- `artifacts/week-35/precision-results.csv`
- `artifacts/week-35/dynamic-profile.json`
- `artifacts/week-35/multi-shape-results.csv`
- `notes/concepts/precision-and-quantization.md`
- `notes/ai-app/week-35.md`（handoff schema/fixtures）

## 本周必须产出

- FP32/FP16 correctness/performance table 与 inspected engine manifest。
- 一个 dynamic profile、多 shape buffer policy 和 out-of-range evidence。
- explicit handoff validator、valid/stale/incomplete fixtures 与 3–5 个 GH-600 scenarios。

## Day 1 — 理论与 precision contract（105 分钟）

- **20 分钟**：阅读 T1，记录 FP16 range/rounding/overflow 风险。
- **25 分钟**：冻结 representative inputs、FP32 full outputs、max/relative/p99/task checks。
- **50 分钟**：运行 FP32 reference 并用人工 perturbation 验证 thresholds 不过宽。
- **10 分钟**：记录 acceptance rationale 与 unknowns。

**必须留下**：precision contract、reference set 和 threshold perturbation result。

## Day 2 — Guided lab：FP16 build 与公平比较（110 分钟）

- **15 分钟**：从实际版本 `trtexec --help` 核对 FP16/typing flags。
- **60 分钟**：build FP16 engine，保存 layer/tensor precision/format/warnings；先过
  correctness，失败时定位而不放宽 threshold。
- **25 分钟**：在相同 warm-up/iterations/power 下采 FP32/FP16 latency、throughput、
  memory、engine size 与 error samples。
- **10 分钟**：记录 reformat/fallback 与 thermal condition。

**必须留下**：FP16 manifest、`precision-results.csv` 和受限性能结论。

## Day 3 — Independent lab：Dynamic profile（105 分钟）

- **25 分钟**：阅读 T2，写 build-time 与 runtime shape steps。
- **65 分钟**：导出/使用 dynamic ONNX，定义 min/opt/max，build engine；runner 执行
  min/opt/max 与两个 in-range non-opt shapes，按实际 shape 安全 sizing buffers。
- **15 分钟**：测试 out-of-range 与 wrong-rank，保存 diagnostics。

**必须留下**：`dynamic-profile.json`、multi-shape results、buffer policy 和 failure cases。

## Day 4 — Integration：Precision/profile × handoff（110 分钟）

- **15 分钟**：阅读 T3+T4；从 T3 列 handoff required fields，用 T4 只定位
  blueprint/domain。
- **65 分钟**：对 FP32/FP16 × representative shapes 做同一 correctness matrix；区分
  build、first-use、warmed latency。
- **20 分钟**：创建 builder→reviewer handoff，含 objective、scope、version、artifact
  IDs/hashes、decisions、blockers、next action；consumer 先 validate 再接受。
- **10 分钟**：制造 stale hash，确认 fail closed。

**必须留下**：precision-shape matrix、valid handoff 和 stale-handoff rejection。

## Day 5 — Assessment：Quantization preview 与场景题（105 分钟）

- **15 分钟**：阅读 T5 或实际 pin 的同名 “Quantization Errors” 章节。
- **25 分钟**：用 NumPy/PyTorch 小 tensor 演示 scale、zero-point、rounding、clipping；
  **不要求 INT8 engine**。
- **35 分钟**：解释 FP16 不更快、profile miss、buffer oversize/undersize 三个案例。
- **30 分钟**：写 3–5 个 GH-600 handoff/state-sharing scenarios 并链接 checklist。

**必须留下**：Q/DQ worksheet、三个 case explanations、3–5 题解析和 evidence link。

## Day 6 — Synthesis 与 Exit ticket（100 分钟）

- **40 分钟**：clean run FP32/FP16 与 min/opt/max correctness subset。
- **20 分钟**：闭卷解释 profile、context shape、buffer sizing 的顺序。
- **25 分钟**：完成 `precision-and-quantization.md` 与周总结。
- **15 分钟**：冻结 W36 direct-device 使用的 engine/profile/reference。

**必须留下**：Week 35 evidence index、概念笔记和 W36 frozen manifest。

## Exit ticket 与 unlocks

形成性要求：闭卷 5 题首次 ≥4/5、90 秒口述、1 个 unseen input、随机 3 个术语；
未过项在下一周 Day 1 前用新题订正。

- FP16 结论同时包含 correctness、performance、reformat/fallback 与 operating conditions。
- 每个 dynamic request 在 allocation/enqueue 前被 range/rank 验证。
- INT8 未实现被明确标为 non-blocking elective，而非伪装完成。
- **Unlock W36**：FP16 和至少一个 dynamic profile 通过 correctness，handoff 可验证。

## Optional Feed Card（30–45 分钟，不计核心时长）

- **来源/选择规则**：从 [NVIDIA Technical Blog · TensorRT](https://developer.nvidia.com/blog/tag/tensorrt/)
  只选 1 篇讨论 reduced precision、dynamic shapes 或 quantization 的文章；必须先核对
  hardware/TRT version，不能把 FP8/INT8 结论外推到本周 FP16。
- **Artifact**：写 `notes/feed/week-35.md`，固定四栏
  `claim → evidence → scope → decision`，decision 明确是否值得做一个小实验。
- **取消规则**：precision/profile correctness 落后或未完成即取消，不占核心时长。
