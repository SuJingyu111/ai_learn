# Week 18 — Unit 3：CUDA Transform M3 与 Hard Gate

## Unit / 课程角色

- **Unit 3 · CUDA Foundations Lab，第六周 / 结课周。** 用一个陌生但有限的 normalize + HWC→CHW transform specification 证明 indexing、layout、error、timing、sanitizer 能形成完整工程闭环。
- 课程导航：[总纲](../CURRICULUM.md) · [官方资料索引](../docs/resources.md) · [统一评分规则](../docs/mastery-gates.md)。
- **Sidecar 依赖位置**：调试 Week 17 local read-only MCP server；使用 Inspector 观察，不以截图替代 automated tests。

## 硬先修与 Remediation

- **硬先修**：Week 17 Exit ticket 全过；能从 clean build 运行 correctness、error fixture、event timing 与 memcheck；local MCP server 无任意 shell/path/write。
- **开周诊断（15 分钟）**：给 `N=1003`、block `128` 手算 grid、最后 block active lanes、read/write bytes；解释 launch check 与 execution check。
- **未通过时**：回 Week 17 重做 `N=33` worked example 与一条 sanitizer before/after；不得用 M3 gate 题当练习题，也不得降低 gate cases。

## Observable Objectives

1. 从自然语言 contract 独立实现 `out[c,h,w] = clamp(alpha[c] * in[h,w,c] + beta[c], lo, hi)`，覆盖 odd/non-block-multiple dimensions、1/3/4 channels 与 padded stride。
2. 建立 CPU oracle、guard/sentinel、invalid-input policy 与 first-mismatch report。
3. 分离 correctness、kernel event time、host end-to-end time；保存 raw samples 与环境。
4. 对正常 target 得到 clean sanitizer result，对隔离故障能解释 diagnostic。
5. 用 MCP Inspector 与 automated fixtures定位 lifecycle/schema/tool error，并保持 local server read-only。

## 术语表

| 术语 | 本周可操作定义 |
|---|---|
| contract | dtype、shape/layout/stride、formula、clamp、invalid input、ownership 的可测试约定 |
| oracle | 独立于 CUDA kernel 的可信 expected-output 生成方式 |
| guard/sentinel | 检测 active region 外写入的已知模式 |
| first mismatch | 首个失败 index、expected、actual、error 的最小复现 |
| effective bandwidth | 由明确 useful read/write bytes 除以 kernel elapsed time |
| Inspector | 交互检查 MCP capability/primitive/message 的开发工具，不是回归测试替代品 |
| hard gate blocker | 即使总分达到 80，也会因 correctness/security/reproducibility 失败而不通过 |

## 必读理论材料（时间已计入对应 Day）

| Day | 材料与精确章节 | 分钟 |
|---:|---|---:|
| 1 | [CUDA Programming Guide — Writing CUDA SIMT Kernels](https://docs.nvidia.com/cuda/cuda-programming-guide/02-basics/writing-cuda-kernels.html)，读 “Writing CUDA SIMT Kernels”、thread/block indexing 与 bounds | 20 |
| 1 | [Compute Sanitizer — Memcheck Tool](https://docs.nvidia.com/cuda/compute-sanitizer/index.html)，读 “Memcheck Tool”、memory access error 与 leak/error reporting | 15 |
| 2 | [MCP 2025-11-25 — Tools](https://modelcontextprotocol.io/specification/2025-11-25/server/tools)，读 input/output schema、structured content、tool errors | 20 |
| 2 | [MCP Inspector Repository](https://github.com/modelcontextprotocol/inspector)，读 README 的 Quick Start、UI Mode、CLI Mode 与 local server debugging | 15 |

## 带问题阅读

- grid-stride loop 与 one-thread-per-element 各自改变什么 contract，哪些不改变？
- clamp 顺序、NaN/Inf、`N=0` 应由哪一层定义？
- 为什么 warm-up、raw samples、source identity 是 reproducibility blocker？
- Inspector 看到一个成功 response，仍缺少哪些 automatic/security evidence？
- 工具参数 schema validation error 与 server internal error 应如何区分？

## Worked Example / 手算

给 `H=1,W=2,C=3` 的 HWC input `[(0,10,20),(2,12,22)]`，每 channel
`alpha=[1,0.5,0.25]`、`beta=[0,-1,-2]`、`lo=0`、`hi=10`：

- 手算 normalize 后的两个 RGB pixels，再按 CHW 顺序写 6 个 expected values。
- 对 `H=3,W=5,C=3`、block `(4,2)` 写 grid、最后合法 `(x,y)` 与 HWC/CHW offsets。
- 计算 float32 每 output element useful bytes：1 次 image read、per-channel scale/bias 读取 convention、1 次 write；说明 constant/cache reuse 假设。
- 为 malformed MCP argument 写 expected tool-execution error，并说明 protocol 连接为何应继续可用。

## 累计项目 Increment

- `labs/cuda_foundations/transform_m3/`：normalize + HWC→CHW student implementation、CPU oracle、stride/channel tests、benchmark、sanitizer target、report。
- `ai-app/client-tests/inspector/`：Inspector observation note + automated invalid fixtures。
- `notes/curriculum/week-18.md`：practice evidence；`notes/gates/gate-3.md`：正式分数、blockers、verdict 与 remediation。

## 本周必须产出

- M3 normalize/HWC→CHW contract、independent implementation、correctness matrix、timing CSV、sanitizer evidence。
- MCP Inspector trace、自动 regression、error taxonomy 与无副作用证明。
- 一次完整 Hard Gate submission。

## Hard Gate G3 / Milestone M3（统一 100 分）

| 维度 | 分值 | 本周证据 |
|---|---:|---|
| Knowledge | 25 | indexing/layout、error boundary、event timing、sanitizer/Inspector 解释 |
| Unseen practical | 30 | 新 H/W/C/stride transform、sentinel、invalid path、read-only MCP tests |
| Cumulative project | 30 | clean M3 build/run、raw samples、environment、sanitizer/Inspector artifacts |
| Oral defense | 15 | unseen case 口述、受限结论、Exit ticket |

**通过规则：`PASS` 需总分 ≥80/100、Knowledge 首次 ≥20/25、mandatory tests 100%，且 correctness、security、reproducibility blockers 全过。** 70–79 且无 critical blocker 为 `CONDITIONAL`；其余为 `RETAKE`。Blocker 不得用高分抵消。

## Day 1 — Theory：Contract、Indexing、Sanitizer（105 分钟）

- **20 分钟｜必修理论**：阅读 Programming Guide 指定章节；画 `N=1003/block=128` 的 grid 与最后 block。
- **15 分钟｜必修理论**：阅读 memcheck 指定章节；区分 precise/imprecise、host API 与 memory-access diagnostics。
- **30 分钟｜contract workshop**：冻结 dtype、H/W/C、HWC input/CHW output、stride unit、per-channel scale/bias、clamp、NaN/Inf、zero dims、null、alias、stream/ownership；未定义项不能进入实现。
- **25 分钟｜worked example**：完成 `1×2×3` expected、odd-size offsets、useful bytes。
- **15 分钟｜retrieval quiz**：闭卷回答 6 个术语并订正。
- **必须留下**：M3 contract v1、grid/address 手算、sanitizer diagnostic map。

## Day 2 — Guided Practice：Oracle、Kernel、Inspector（105 分钟）

- **20 分钟｜必修理论**：阅读 MCP Tools stable spec 指定章节。
- **15 分钟｜必修理论**：阅读 MCP Inspector 指定章节，记录它能观察与不能证明的内容。
- **45 分钟｜guided lab**：先写 CPU oracle/known cases，再实现最简单 CUDA kernel；覆盖 `1×1×1`、`1×2×3`、`3×5×3`、`9×33×4`、padded input/output stride 与 sentinel。
- **15 分钟｜MCP guided**：用 Inspector 连接本地 server，查看 capability、tool schema、正常 call 与 invalid argument。
- **10 分钟｜验证**：把 Inspector observation 转成一条 automated assertion。
- **必须留下**：oracle/kernel、known-case table、Inspector note 与自动 assertion。

## Day 3 — Independent Lab：Unseen Inputs 与 Debugging（105 分钟）

- **55 分钟｜Core independent**：在不看 gate cases 的前提下生成 fixed-seed random H/W/C、per-channel params 与 padded strides；加入 first-mismatch `(c,h,w)`、max error、guard report，修到 clean。
- **30 分钟｜Sidecar independent**：用 Inspector/fixture 复现 unknown tool、wrong type、oversized value、server internal exception；分类为 protocol/tool-execution/internal，随后合法 request 必须成功。
- **20 分钟｜sanitizer**：对正常小型 coverage set 运行 memcheck；对隔离错误 variant 保存 diagnostic，修复后重跑。
- **必须留下**：independent cases、四类 MCP debug traces、normal/fault sanitizer evidence。

## Day 4 — Integration：Correctness → Timing → Evidence（105 分钟）

- **60 分钟｜Core integration**：冻结 warm-up/iterations/power state；先 correctness，再采 kernel event 与 host end-to-end raw samples，计算 median/p95/effective bandwidth，禁止把 validation 放进 kernel region。
- **30 分钟｜Sidecar integration**：建立 non-interactive regression：initialize、list、valid call、四个 invalid cases、timeout、shutdown；输出 machine-readable summary。
- **15 分钟｜evidence audit**：从 clean directory 复跑，检查 source identity、tool versions、commands、raw data links。
- **必须留下**：timing CSV/report、MCP regression summary 与 clean-run transcript。

## Day 5 — Assessment Rehearsal（105 分钟）

- **45 分钟｜Core rehearsal**：用一组自建、非 gate 输入做 25/30/30/15 自评；发现错误只回到对应 objective，不读取未来参考实现。
- **35 分钟｜MCP/security rehearsal**：测试伪 path、额外 property、prompt injection text、crash recovery；确认 server 不增加权限、不泄露 log。
- **25 分钟｜submission pack**：冻结 contract、source identity、case manifest、raw data、sanitizer、MCP results 与 limitations；列 blocker 状态。
- **必须留下**：preflight score、修复记录、不可变 submission manifest。

## Day 6 — Hard Gate G3 / M3（120 分钟 / 100 分）

- **45 分钟 / 25 分｜Knowledge**：闭卷解释 unseen layout/launch/error/timing trace，并回答 capability/schema/authorization 边界。
- **35 分钟 / 30 分｜Unseen practical**：运行隐藏/未预演 H/W/C/stride、invalid、sentinel、MCP deny cases；任何 false pass 为 blocker。
- **25 分钟 / 30 分｜Cumulative project**：从 clean directory 执行 documented M3 command；核对 raw timing、environment、sanitizer、machine-readable MCP report；无法复现为 blocker。
- **15 分钟 / 15 分｜Oral defense**：口述一个新 fused tensor transform 如何沿用本周方法，给出 limitation 和下一实验。
- **必须留下**：`notes/gates/gate-3.md`、逐项分数、blocker checklist、verdict（`PASS`/`CONDITIONAL`/`RETAKE`）与证据链接。

## Optional Feed Card（30–45 分钟，不计入每日 90–120 分钟）

- **准入 / 取消**：仅在 M3 submission 已冻结且 Gate verdict 为 `PASS` 后做；若 Gate 落后、存在 blocker 或需 remediation，直接取消，Feed Card 不参与评分。
- **来源与选择规则**：从 [MCP Inspector Releases](https://github.com/modelcontextprotocol/inspector/releases) 选择最新一个与本地 Inspector major/minor 匹配的稳定版条目；只跟进一个 schema validation、transport/debugging 或 security 变化，并追到对应 PR/文档。
- **30–45 分钟流程**：5 分钟核对 installed/release version → 15–20 分钟读 release note 与 PR → 10–15 分钟写 `claim → evidence → scope → decision`；必须用本周 harmless fixture复核，不能用截图替代。
- **artifact**：`notes/feed/week-18.md`，含版本矩阵、上游链接、本地 before/after 与一个 upgrade/hold 决策。

## Exit Ticket / Unlocks

- **PASS**：≥80 且 correctness/security/reproducibility blockers 全过，解锁 Unit 4 profiler 与 image-operator optimization。
- **CONDITIONAL / RETAKE**：按 [Mastery Gates](../docs/mastery-gates.md) 使用新题/新输入重测；保留原分数和失败 evidence，不覆盖历史记录。
