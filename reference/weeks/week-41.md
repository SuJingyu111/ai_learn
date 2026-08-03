# Unit 7 · Week 41 — Orin Power/Thermal Reliability 与 GitHub MCP/CI

> **课程角色**：给 beta 加入 Orin operating-point contract、sustained telemetry 与
> reliability repetition；Sidecar 在只读范围体验 GitHub MCP，并划清 remote service、
> MCP tool 与标准 CI job 的职责。
>
> 课程导航：[总纲](../CURRICULUM.md) · [资料索引](../docs/resources.md) ·
> [Mastery Gates](../docs/mastery-gates.md)

## 硬先修与 remediation

- **硬先修**：W40 clean pipeline、timeline、high-severity security status；可重复的
  Orin command 与实际 L4T/JetPack/TensorRT pin。
- 先读取实际设备的 `/etc/nv_tegra_release`，再从
  [Jetson archived documentation](https://docs.nvidia.com/jetson/archives/)
  解析到同一 L4T release 的 Developer Guide；把最终选中的精确 URL 记录进 artifact。
  2026-07-26 的 archive 当前示例是 r39.2，但它**绝不能覆盖实机版本**。
- **Remediation**：无 power telemetry 时仍记录 temperature/clocks/utilization 与
  `unavailable` 原因；没有 GitHub/org 权限时用 redacted config/read-only fixtures，
  不请求管理员权限、不伪造 remote pass。

## Observable objectives（可观察目标）

完成后能够：

1. 定义 power mode、clocks、thermal window、duration 与 stop conditions。
2. 报告 cold/warm/steady latency、throughput、memory、temperature/telemetry。
3. 运行 repeated/early-stop/restart cases 并识别 drift/throttling。
4. 配置或演练 read-only GitHub MCP scope，并说明 standard CI 的独立 gate。

## 术语表

| 术语 | 本周可操作定义 |
|---|---|
| Operating point | power mode、clocks、cooling、ambient/load 条件组合 |
| Sustained window | 排除 cold/startup 后、时长足以观察 thermal drift 的区间 |
| Throttling | thermal/power limit 导致 clocks/performance 被约束 |
| p95 | latency samples 的第 95 百分位，不等同 worst case |
| GitHub MCP | 通过 MCP 暴露 GitHub tools/resources 的 integration |
| CI gate | deterministic workflow 对 build/test/artifacts 的 pass/fail 控制 |

## 必修理论材料（分钟计入对应 Day）

| ID | 精确 URL 与章节/页面 | 分钟 | Day |
|---|---|---:|---:|
| T1 | [Jetson archived documentation](https://docs.nvidia.com/jetson/archives/) — 按 `/etc/nv_tegra_release` 进入同版 Developer Guide 的 “Platform Power and Performance → Power Mode Controls / Thermal Management”，并记录 resolved URL | 20 | 1 |
| T2 | [Jetson archived documentation](https://docs.nvidia.com/jetson/archives/) — 按实机 release 进入同版 “Jetson Linux Development Tools → Tegrastats Utility → Reported Statistics / Running tegrastats”，并记录 resolved URL | 15 | 2 |
| T3 | [Using the GitHub MCP Server from Copilot Chat](https://docs.github.com/en/copilot/how-tos/copilot-on-github/copilot-for-github-tasks/using-the-github-mcp-server-from-copilot-chat) — setup、toolsets、read-only use | 20 | 4 |
| T4 | [GitHub Actions workflow syntax](https://docs.github.com/en/actions/reference/workflows-and-actions/workflow-syntax) — `permissions`、`timeout-minutes`、`concurrency` | 15 | 5 |

> **仅作定位示例（2026-07-26）**：若且仅若实机确为 r39.2，可使用
> [r39.2 Orin power/performance](https://docs.nvidia.com/jetson/archives/r39.2/DeveloperGuide/SD/PlatformPowerAndPerformance/JetsonOrinNanoSeriesJetsonOrinNxSeriesAndJetsonAgxOrinSeries.html)
> 与
> [r39.2 Tegrastats Utility](https://docs.nvidia.com/jetson/archives/r39.2/DeveloperGuide/AT/JetsonLinuxDevelopmentTools/TegrastatsUtility.html)；
> 其他版本必须回到 archive 选择对应 release。

## 带问题阅读

- 冷机最好值为何不能代表 deployment behavior？
- `tegrastats` 的每个字段能证明什么、不能证明什么？
- GitHub MCP read-only 查询与 CI required check 分别由谁控制、留下何种 evidence？

## Worked example

```text
power_mode=P, clocks=default, fan/cooling=C
warm-up=30 frames, sustained=20 min
report: first, warm median/p95, steady median/p95, throughput
telemetry: start/end/max temp, clocks, throttling signal, memory
```

GitHub MCP 只读取 workflow run/PR metadata；它不能把“agent 说通过”变成 CI pass，
也不能拥有 merge、secret 或 device-config 权限。

## 累计项目 increment

- `artifacts/week-41/orin-operating-point.md`
- `artifacts/week-41/sustained-baseline.csv`
- `artifacts/week-41/reliability-report.md`
- `notes/ai-app/week-41.md`（GitHub MCP read-only scope）
- `.github/workflows/ci.yml`（scope audit，不虚构 GPU coverage）

## 本周必须产出

- Orin power/thermal measurement contract、sustained raw data 与 reliability analysis。
- repeated/restart/early-stop evidence 和 stop-condition log。
- GitHub MCP read-only configuration lab、CI scope table 与 3–5 个 GH scenarios。

## Day 1 — 理论与 operating-point contract（105 分钟）

- **20 分钟**：阅读与实际 L4T 匹配的 T1；URL 不匹配则先解析 archive。
- **25 分钟**：冻结 power mode、clocks、cooling、background load、warm-up、duration、
  sample/stop conditions；不擅自锁最高 clocks。
- **50 分钟**：运行短 baseline confirmation，超过 W40 漂移阈值先调查。
- **10 分钟**：记录 environment/engine/model/input hashes。

**必须留下**：`orin-operating-point.md`、baseline confirmation 和 drift verdict。

## Day 2 — Guided lab：Sustained telemetry（110 分钟）

- **15 分钟**：阅读实际版本 T2，标出可观测字段与单位。
- **70 分钟**：运行 sustained beta，采 first/warm/steady latency、throughput、memory、
  temperatures、clocks、power rails（可用时）与 throttling signal。
- **15 分钟**：按 cold/warm/steady 分窗计算 median/p95。
- **10 分钟**：保存 raw CSV 与 exact commands。

**必须留下**：`sustained-baseline.csv`、telemetry excerpt 和 windowed summary。

## Day 3 — Independent lab：Reliability repetition（105 分钟）

- **15 分钟**：预注册 one-frame、odd count、hundreds frames、early stop、restart。
- **70 分钟**：运行 cases，检查 ordering/checksum、memory high-water、cleanup、
  subsequent-run state；发生 thermal/overcurrent signal 时按 stop condition 停止。
- **20 分钟**：区分 deterministic bug、environment drift 与 measurement noise。

**必须留下**：`reliability-report.md`、failure/restart traces 和 classification。

## Day 4 — Integration：GitHub MCP read-only lab（110 分钟）

- **20 分钟**：阅读 T3，核对当前 surface、toolset 与 permission prerequisites。
- **55 分钟**：在有权限环境配置最小 read-only GitHub MCP，仅查询一个 repo 的 file/
  PR/workflow metadata；无权限则用 redacted config+expected trace，不写远端状态。
- **25 分钟**：测试 untrusted issue/log instruction 与 attempted write；必须拒绝或 ask，
  不使用 secrets，不 merge。
- **10 分钟**：记录 MCP、GitHub auth、repo permissions 三层边界。

**必须留下**：read-only config/trace、permission matrix 和 denial evidence。

## Day 5 — Assessment：CI scope 与 scenarios（105 分钟）

- **15 分钟**：阅读 T4。
- **50 分钟**：审计 `.github/workflows/ci.yml`：CPU build/tests、permissions、timeout、
  concurrency、artifacts；写 hosted CI 能/不能验证表，Orin GPU 另设 manual/self-hosted gate。
- **30 分钟**：写 3–5 个 GitHub MCP/CI scenarios，解释 agent result 不等于 required check。
- **10 分钟**：更新 GH checklist evidence。

**必须留下**：CI scope table、3–5 题解析和 evidence links。

## Day 6 — Synthesis 与 Exit ticket（100 分钟）

- **40 分钟**：复跑一个 steady window 与 restart subset。
- **20 分钟**：用数据解释 cold/warm/steady 与 throttling status。
- **25 分钟**：写周总结、known operating limits 与 next action。
- **15 分钟**：准备 W42 MCP evidence-assistant allowlisted artifact set。

**必须留下**：Week 41 evidence index、operating-limit summary 和 W42 allowlist input。

## Exit ticket 与 unlocks

形成性要求：闭卷 5 题首次 ≥4/5、90 秒口述、1 个 unseen input、随机 3 个术语；
未过项在下一周 Day 1 前用新题订正。

- 性能数字均绑定 Orin/JetPack/power/clocks/thermal/model/input。
- sustained result 不拿冷机最好值替代，stop condition 被遵守。
- GitHub MCP 与 standard CI 的权限/证据边界可解释。
- **Unlock W42**：beta sustained/restart 无 blocker，allowlisted artifacts 已冻结。

## Optional Feed Card（30–45 分钟，不计核心时长）

- **来源/选择规则**：从 [NVIDIA AI Podcast](https://ai-podcast.nvidia.com/)
  只选 1 期 show notes 明确涉及 edge/robotics/deployment/reliability 的节目；找不到
  与 Orin operating point 相关的可检验 claim 就跳过。
- **Artifact**：写 `notes/feed/week-41.md`，固定四栏
  `claim → evidence → scope → decision`，区分嘉宾观点与本机 telemetry。
- **取消规则**：sustained/restart evidence 或 CI scope audit 落后时直接取消。
