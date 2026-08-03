# Unit 6 · Week 34 — TensorRT C++ Buffers、Enqueue 与 Lifetime

> **课程角色**：把 W33 的 inspect-only 程序变成最小可验证 C++ inference runner；
> 重点是 byte sizing、tensor address、stream ordering、RAII 与 failure cleanup。
> Sidecar 聚焦 parallel agent 的 workspace/tool isolation。
>
> 课程导航：[总纲](../CURRICULUM.md) · [资料索引](../docs/resources.md) ·
> [Mastery Gates](../docs/mastery-gates.md)

## 硬先修与 remediation

- **硬先修**：W33 的 version pin、FP32 engine manifest、I/O metadata、fixed input/reference。
- TensorRT 文档继续使用 `artifacts/week-33/trt-version-pin.md` 的实际版本 archive；
  version/engine hash 改变必须重新建立 baseline，禁止改用 `latest` 页面。
- **Remediation**：若 inspector 不可运行，Day 1 的后 45 分钟只修 deserialize/metadata；
  若 shape/dtype 未知，不分配 device buffer。无 Orin 时允许写 RAII/static tests，
  但 enqueue/correctness exit ticket 记为 blocked。

## Observable objectives（可观察目标）

完成后能够：

1. 用 checked arithmetic 计算每个 I/O tensor bytes 并拒绝 overflow/unknown shape。
2. 用 C++ RAII 管理 runtime、engine、context、stream 与 device buffers。
3. 解释 `enqueue` 返回、GPU work 完成和 host 可读 output 的不同时间点。
4. 证明两个模拟 agents 的 files/tools/workspaces 互相隔离且越权会 fail closed。

## 术语表

| 术语 | 本周可操作定义 |
|---|---|
| Tensor address | execution context 中 name→device pointer 的绑定 |
| Enqueue | 向指定 CUDA stream 提交 inference；返回不等于 output 已可读 |
| RAII | 对象 lifetime 与资源 acquire/release 绑定，覆盖 early return |
| Output-ready boundary | event/stream sync 证明 output 可被下一 consumer 使用的点 |
| Scope isolation | 每个 agent 只有声明过的 paths、tools、credentials 与 output |

## 必修理论材料（分钟计入对应 Day）

| ID | 精确 URL 与章节/页面 | 分钟 | Day |
|---|---|---:|---:|
| T1 | [TensorRT Archives](https://docs.nvidia.com/deeplearning/tensorrt/archives/) — 已 pin Developer Guide 的 “C++ API / Performing Inference” 与 tensor-address APIs | 25 | 1 |
| T2 | [CUDA Runtime API — API synchronization behavior](https://docs.nvidia.com/cuda/cuda-runtime-api/api-sync-behavior.html) — memcpy 与 synchronization behavior | 15 | 2 |
| T3 | [GitHub Copilot Agents application card](https://docs.github.com/en/copilot/responsible-use/agents) — “Key terms / Key features / Safety components and mitigations” 的 ephemeral environment、firewall、directory/tool permissions | 10 | 4 |
| T4 | [GH-600 Study Guide](https://learn.microsoft.com/en-us/credentials/certifications/resources/study-guides/gh-600) — **仅用于** blueprint/domain/skill mapping，不作为 isolation 教材 | 5 | 4 |

## 带问题阅读

- 为什么按 opt shape 分配 bytes 会在其他 shape 上越界？
- 哪个 event/synchronization 才能证明 output 可读，而不是只证明 API 已返回？
- “agent instruction 说不要越权”为何不能替代 filesystem/tool enforcement？

## Worked example

对 `FP32[1,3,224,224]`：

```text
elements = 1*3*224*224 = 150528
bytes = checked_mul(elements, sizeof(float)) = 602112
owner: DeviceBuffer<float>
producer: H2D/preprocess -> enqueue(stream S) -> event output_ready
consumer: only after event/sync
```

任何 dimension 为负、乘法 overflow、name 不存在都在分配前失败；不能猜 bytes 后继续。

## 累计项目 increment

- `labs/runtime/week18_trt_runner.cpp`
- `artifacts/week-34/correctness.json`
- `artifacts/week-34/failure-matrix.md`
- `notes/concepts/tensorrt-build-runtime.md`
- `notes/ai-app/week-34.md`（scope matrix 与 isolation traces）

## 本周必须产出

- C++ deserialize→bind→enqueue→validate 路径和 cross-backend correctness。
- lifetime diagram、两个 safe failure paths、cleanup evidence。
- 两个隔离 worker fixtures、越权拒绝证据与 3–5 个 GH-600 scenarios。

## Day 1 — 理论与 buffer contract（105 分钟）

- **25 分钟**：阅读 T1，记录实际版本中的 API 名称与 deprecated/unsupported 差异。
- **25 分钟**：为每个 I/O tensor 写 shape×dtype→bytes 的 checked calculation。
- **45 分钟**：在 runner 增加 RAII owners 与 allocation plan；unknown/dynamic shape
  只报错，不进入 allocation。
- **10 分钟**：画 creation/destruction order。

**必须留下**：buffer-size table、API-version note 和 RAII ownership diagram。

## Day 2 — Guided lab：bind、enqueue、output-ready（110 分钟）

- **15 分钟**：阅读 T2，回答 memcpy/stream sync 的观察边界。
- **70 分钟**：分配/reuse buffers、绑定 tensor addresses、在 explicit stream enqueue，
  只在 output-read boundary 同步；运行 fixed input 并与 saved reference 比较。
- **15 分钟**：加入 CUDA API、enqueue 与 async error checks。
- **10 分钟**：保存 command、max error 与 event timeline。

**必须留下**：runner end-to-end path、`correctness.json` 和 output-ready evidence。

## Day 3 — Independent lab：failure 与 cleanup（105 分钟）

- **15 分钟**：预注册 wrong name、wrong dtype/shape、corrupt engine、allocation failure。
- **70 分钟**：实现至少两个安全 failure fixtures；验证 partial resources 释放、无
  silent success、无 half-valid output，再跑正常路径 regression。
- **20 分钟**：用可用 sanitizer/resource log 检查 early return 与 destruction order。

**必须留下**：`failure-matrix.md`、diagnostics、cleanup log 和 regression result。

## Day 4 — Integration：runner × agent isolation（110 分钟）

- **15 分钟**：阅读 T3+T4；从 T3 提取 workspace/tool isolation 的 enforcement
  条件，用 T4 只定位 blueprint/domain。
- **60 分钟**：完成 PyTorch/ORT/TRT 同 input comparator，并把 raw output 与 manifest
  写到唯一 artifact 目录。
- **25 分钟**：用两个本地 worker fixtures 分别读取 metadata/correctness；配置独立
  input/output scope，测试 cross-workspace read 与未 allowlist tool 均被拒绝。
- **10 分钟**：记录 coordinator 只接受声明过的 artifact/hash。

**必须留下**：cross-backend table、scope matrix 和两条 denial traces。

## Day 5 — Assessment：lifetime defense（105 分钟）

- **60 分钟**：从 clean process 连续运行正常、wrong-shape、正常三次；检查 sticky
  error、leak、buffer reuse 与 output ordering。
- **30 分钟**：写 3–5 个 GH-600 scenarios，覆盖 excessive scope、shared workspace、
  secret request、untrusted handoff 与 cleanup；解释错误选项。
- **15 分钟**：更新 `docs/gh600-checklist.md` evidence links 与 residual risks。

**必须留下**：repeat-run evidence、3–5 题解析和 checklist link。

## Day 6 — Synthesis 与 Exit ticket（100 分钟）

- **40 分钟**：clean build/run runner 与 failure subset。
- **20 分钟**：闭卷画 engine/context/stream/buffers 的 lifetime。
- **25 分钟**：写 “Mechanism→Evidence→Limitation→Next action”。
- **15 分钟**：冻结 W35 FP32 baseline、representative inputs 与 allowed shape set。

**必须留下**：Week 34 evidence index、最终 lifetime 图和 W35 baseline manifest。

## Exit ticket 与 unlocks

形成性要求：闭卷 5 题首次 ≥4/5、90 秒口述、1 个 unseen input、随机 3 个术语；
未过项在下一周 Day 1 前用新题订正。

- 能手算一个 tensor 的 bytes，并说明 overflow/unknown shape 的 fail point。
- 能指出 enqueue return 与 output-ready 的证据边界。
- failure 后再次正常运行且无残留状态。
- **Unlock W35**：C++ runner correctness 通过，buffer/stream lifetime 可被 reviewer 复述。

## Optional Feed Card（30–45 分钟，不计核心时长）

- **来源/选择规则**：从 [NVIDIA Technical Blog · CUDA](https://developer.nvidia.com/blog/tag/cuda/)
  只选 1 篇含 memory ownership、async ordering、C++ resource management 或 failure
  cleanup 的文章；只有性能口号、无可复现实验条件者不选。
- **Artifact**：写 `notes/feed/week-34.md`，固定四栏
  `claim → evidence → scope → decision`，并注明与本周 runner 的适用/不适用边界。
- **取消规则**：correctness、failure 或 cleanup 任务落后时直接取消，不补做。
