# Unit 7 · Week 39 — Device-resident Preprocessing 与 Audit Accountability

> **课程角色**：把 M2 冻结的 model/input/dataset/normalize/layout、M4/G4 验证的
> exact Resize semantics 与 CPU/CUDA oracle、M6 验证的 direct-device contract，
> 组成 beta 的稳定 device-resident producer，同时消除不必要 D2H→H2D。
> Sidecar 建立 who/what/why/result 可关联 audit record。
>
> 课程导航：[总纲](../CURRICULUM.md) · [资料索引](../docs/resources.md) ·
> [Mastery Gates](../docs/mastery-gates.md)

## 硬先修与 remediation

- **硬先修**：W38 declared-supported matrix 全过；M2 的
  model/input/dataset/normalize/layout manifest；M4/G4 的 exact Resize
  mapping/border semantics 与 CPU/CUDA oracle；M6 的 direct-device address path。
- TensorRT input API 继续使用设备 pin；JetPack/TensorRT/engine 变化必须重建 manifest，
  不接受 `latest` 文档捷径。
- **Remediation**：任一 stage correctness 不明时，Day 1–2 退回 separate kernels 并逐
  stage copy-back；fusion 是可选，不能用它掩盖 semantic mismatch。

## Observable objectives（可观察目标）

完成后能够：

1. 为 resize→normalize→HWC→CHW 标出所有 buffers、bytes、owners 与 device transitions。
2. 逐 stage/full tensor 与 independent reference 比较。
3. 将 final device buffer 直接交给 TensorRT 并证明消除的 copy。
4. 生成脱敏、可关联、能回答 who/what/why/result 的 audit records。

## 术语表

| 术语 | 本周可操作定义 |
|---|---|
| Device-resident | producer 与 consumer 之间数据保持在 device memory |
| Intermediate oracle | 能单独判断某 stage semantics 的 expected tensor |
| Buffer reuse | 仅在 prior async use 完成后复用同一 storage |
| Provenance | artifact 的 source/version/hash/environment 证据链 |
| Audit record | actor/action/target/reason/result/evidence/timestamp 的结构化记录 |

## 必修理论材料（分钟计入对应 Day）

| ID | 精确 URL 与章节/页面 | 分钟 | Day |
|---|---|---:|---:|
| T1 | [CUDA Programming Guide — 1.2 Programming Model](https://docs.nvidia.com/cuda/cuda-programming-guide/01-introduction/programming-model.html) — “1.2.3 GPU Memory” | 20 | 1 |
| T2 | [TensorRT Archives](https://docs.nvidia.com/deeplearning/tensorrt/archives/) — 已 pin C++ guide 的 tensor address / input shape 页面 | 15 | 3 |
| T3 | [Managing agent sessions](https://docs.github.com/en/copilot/how-tos/copilot-on-github/use-copilot-agents/manage-and-track-agents) — “Review session logs / Trace commits to session logs / Stop and archive” 的可追溯机制 | 10 | 4 |
| T4 | [GH-600 Study Guide](https://learn.microsoft.com/en-us/credentials/certifications/resources/study-guides/gh-600) — **仅用于** blueprint/domain/skill mapping，不作为 audit/accountability 教材 | 5 | 4 |

## 带问题阅读

- 哪个 intermediate 最能快速区分 RGB/BGR、normalization 与 layout 错误？
- “没有显式 memcpy”能否证明没有 hidden copy？
- audit log 应保存什么才能追责，又必须 redaction 什么？

## Worked example

```text
uint8 HWC input
 -> resize float/HWC
 -> normalize float/HWC
 -> layout float/CHW in d_input
 -> context.setTensorAddress("input", d_input)
```

每个 arrow 有 shape/dtype/bytes/stream/owner；audit 记录 artifact hash 和结果，不记录
token、Authorization header 或完整私人 prompt。

## 累计项目 increment

- `artifacts/week-39/preprocess-stage-correctness.json`
- `artifacts/week-39/device-path-inventory.md`
- `ai-app/audit-schema.md`
- `artifacts/week-39/audit-samples.jsonl`
- `notes/ai-app/week-39.md`

## 本周必须产出

- staged CUDA preprocessing、per-stage/full correctness 与 buffer inventory。
- direct runtime input、eliminated-copy evidence 与 no-fusion/fusion decision。
- audit schema、三条合法 records、redaction test 和 GH evidence。

## Day 1 — 理论与 stage/buffer model（105 分钟）

- **20 分钟**：阅读 T1，复述 host/device memory 与 stream ordering。
- **25 分钟**：从 frozen contract 画 stages、intermediates、shape/dtype/bytes。
- **50 分钟**：整理 CPU/PyTorch intermediate references 与 three fixed inputs。
- **10 分钟**：手算 corner pixel/channel，检查 RGB/BGR/range/layout。

**必须留下**：stage diagram、buffer-size table 和 reference tensor index。

## Day 2 — Guided lab：Separate correct stages（110 分钟）

- **15 分钟**：预注册每 stage tolerance 与 first-mismatch diagnostic。
- **75 分钟**：组合 resize→normalize→layout kernels，reuse explicit stream/
  preallocated buffers；每 stage 可独立 copy-back compare，再做 full tensor compare。
- **10 分钟**：测试 padded stride/odd size。
- **10 分钟**：保存 correctness JSON。

**必须留下**：stage pipeline、per-stage/full correctness 和 boundary result。

## Day 3 — Independent lab：Direct runtime input（105 分钟）

- **15 分钟**：阅读 T2，核对 input address/shape contract。
- **70 分钟**：将 final CUDA buffer 绑定为 TensorRT input；对比 CPU preprocessing+
  runtime final output；记录地址、shape、bytes 与 output-ready boundary。
- **20 分钟**：用 trace/copy inventory 证明消除 D2H→H2D，未知 gap 明确标出。

**必须留下**：device-resident path、end-to-end correctness 和 copy evidence。

## Day 4 — Integration：Audit schema（110 分钟）

- **15 分钟**：阅读 T3+T4；从 T3 提取 actor/session/artifact trace 机制，用 T4
  只定位 blueprint/domain。
- **55 分钟**：创建 `ai-app/audit-schema.md`，字段含 actor/workflow/action/target/
  rationale/approval/result/artifact IDs/correlation ID/timestamps；写 redaction rules。
- **30 分钟**：生成 success、denied、failed 三条本地 records，验证可从 action 追到 evidence。
- **10 分钟**：注入 secret-like fixture，确认被 redact。

**必须留下**：audit schema、三条 samples 和 redaction test。

## Day 5 — Assessment：Pipeline truth table × accountability（105 分钟）

- **60 分钟**：随机选 normal/boundary/odd inputs 复跑 stage/full/final checks；为每个
  failure location 写可观察 signal。
- **30 分钟**：写 3–5 个 audit/accountability scenarios，覆盖 missing actor、
  misleading reason、partial result、secret leakage、unlinked artifact。
- **15 分钟**：更新 GH checklist 与 limitations。

**必须留下**：truth table、3–5 题解析和 evidence links。

## Day 6 — Synthesis 与 Exit ticket（100 分钟）

- **40 分钟**：clean run device-resident representative case。
- **20 分钟**：闭卷解释每个 buffer owner 与 transition。
- **25 分钟**：写周总结；根据 profiler evidence 决定 fusion backlog（允许不做）。
- **15 分钟**：准备 W40 same/cross-stream cases 与 NVTX ranges。

**必须留下**：Week 39 evidence index、fusion decision 和 W40 timeline plan。

## Exit ticket 与 unlocks

形成性要求：闭卷 5 题首次 ≥4/5、90 秒口述、1 个 unseen input、随机 3 个术语；
未过项在下一周 Day 1 前用新题订正。

- 能用 intermediate oracle 定位 preprocess semantic failure。
- direct-device path 有 address/copy/timeline 证据，不以代码表面推断。
- audit records 可关联且不泄露 secrets。
- **Unlock W40**：device-resident correctness 通过并有明确 stream/lifetime contract。

## Optional Feed Card（30–45 分钟，不计核心时长）

- **来源/选择规则**：从 [GitHub Blog · Security](https://github.blog/security/)
  只选 1 篇含 audit、provenance、traceability 或 redaction 机制的文章；没有可核验
  mechanism/evidence 的观点文不选。
- **Artifact**：写 `notes/feed/week-39.md`，固定四栏
  `claim → evidence → scope → decision`，并指出 audit schema 是否需要 test 而非直接改。
- **取消规则**：任一 preprocessing stage oracle 或 direct-device evidence 落后即取消。
