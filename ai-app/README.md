# Runtime Lab Assistant

引擎之外的第二条序列：把引擎产出的 benchmark artifacts 暴露为一个最小权限 MCP server。

## 最终范围

将引擎产出的稳定 artifacts 暴露为一个最小权限 MCP server：

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

这是 **Phase B** 的产物，在 Phase 09 之后开始——那时引擎才有真实的 benchmark
产物值得读取。步骤会在你走到那里时写出来。

主题索引见 [`../reference/AI_APP_TRACK.md`](../reference/AI_APP_TRACK.md)。
