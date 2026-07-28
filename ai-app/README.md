# Runtime Lab Assistant

这是 48 周 AI Application / MCP / GH-600 sidecar 的渐进式 capstone。

## 最终范围

将当前 CUDA/Orin 项目的稳定 artifacts 暴露为一个最小权限 MCP server：

- resources：architecture、benchmark methodology、environment manifests；
- read-only tools：list/validate/compare experiment results；
- prompt：evidence-first profiling review；
- evals：tool selection、invalid input、stale state、prompt injection；
- deployment：stdio 起步，后续 Streamable HTTP、authorization 和 audit；
- optional action：只运行 allow-listed benchmark，必须 explicit approval。

## Security Defaults

- 默认 read-only。
- 不接受任意 filesystem path。
- 不提供任意 shell、package install、device configuration 或 secret access。
- Tool descriptions 和 MCP output 都按 untrusted data 处理。
- 所有 inputs 使用 typed schema 和 validation。
- Remote server 才考虑 transport authorization；stdio 不复用 HTTP auth flow。
- Material action 必须有 timeout、resource limit、audit 和 human approval。

## Build Order

具体每日任务已经合并进 [`../weeks/`](../weeks/) 的当周文件；A1–A8 的单元边界和
每个单元的累计产品见 [`../AI_APP_TRACK.md`](../AI_APP_TRACK.md) 的八单元安排表。

这里不重复周次映射：之前重复出来的一份已经和 A3/A4 的单元边界对不上了。
需要知道「第几周做什么」时看 [`../weeks/README.md`](../weeks/README.md)，
需要知道「哪个单元交付什么」时看 `AI_APP_TRACK.md`。
