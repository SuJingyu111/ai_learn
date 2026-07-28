# 开课前：入学诊断与桥接课

这门课程**不假设你有 ML/AI 工作经历**。它假设你已经具备一定的 C/C++、Linux
和系统调试经验，并用入学诊断决定 Python、数学、GitHub 或 Orin 环境是否需要先补。
不要凭“感觉会了”跳过先修；可以凭证据 test out。

## 先理解时间结构

- 正课：48 个内容周，8 个 Unit，每个 Unit 6 周。
- 推荐日程：每周 6 天，每天 90–120 分钟。
- 一年版本：在 Unit 2、4、6、8 后各放 1 个 buffer/remediation week，共 52 周。
- 如果入学诊断暴露明显缺口，先加 1–2 周 Course 0；这不算落后。
- Calendar week 只是排期，Mastery Gate 才是进度。未通过 Gate 时不得靠翻页进入下一单元。

## 90 分钟入学诊断

先运行 `mkdir -p notes/diagnostics`，再在 `notes/diagnostics/entry.md` 记录所有命令、
答案、耗时和不确定项。诊断允许查命令语法，不允许照抄完整解法。

### A. Python / NumPy，25 分钟

1. 创建 `3×4` 的 `float32` array，打印 shape、strides、dtype、itemsize。
2. 做 transpose 和切片，判断它们是否共享 storage，并用代码验证。
3. 不调用现成 layout helper，把一个小型 HWC array 转成 CHW。
4. 写 3 个 assertions，覆盖正常、odd shape 和 padded row stride。

通过标准：代码可运行；能区分 element stride 与 byte stride；不把 reshape、view、
copy 混为一谈。

### B. 运行时数学，20 分钟

闭卷完成：

1. 计算一个 `2×3` 矩阵与 `3×2` 矩阵相乘后的 shape 和两个输出元素。
2. 写出二维 row-major array 的 offset 公式。
3. 对 `H=7, W=7, K=3, stride=2, padding=1, dilation=1` 计算 Conv 输出尺寸。
4. 解释绝对误差和相对误差分别在什么情况下会误导。

通过标准：shape/offset 全部正确；四题至少 80%，订正后 100%。

### C. C++ / Linux / 构建，20 分钟

1. 在 clean build directory 完成 CPU configure、build、test。
2. 解释 stack、heap、RAII、ownership 和 lifetime 的区别。
3. 读一个失败测试的 stderr，指出 first actionable error。

建议命令：

```bash
cmake -S . -B build-cpu -DCMAKE_BUILD_TYPE=Release -DENABLE_CUDA=OFF
cmake --build build-cpu --parallel
ctest --test-dir build-cpu --output-on-failure
```

通过标准：clean build/test 可复现；能解释谁拥有 buffer、何时释放、失败如何传播。

### D. Git / GitHub / Actions，15 分钟

1. 解释 working tree、index、commit、branch、PR 的关系。
2. 读 `.github/workflows/ci.yml`，指出 trigger、permissions、jobs 和 artifact。
3. 解释为什么 PR title、issue body 和 tool output 都应被视为 untrusted input。

通过标准：能画出 issue → branch → commit → PR → checks → review 的控制流。若没有
GitHub 账号或远程仓库，可先做本地纸面/fixture 版本，但 Week 2 前需准备一个练习仓库。

### E. Orin 设备清单，10 分钟

记录实际设备，而不是使用计划中的示例版本：

- Orin 型号和内存；
- JetPack / L4T；
- CUDA、TensorRT、cuDNN；
- compiler、CMake、Python；
- power mode、`jetson_clocks` 状态；
- host 与 Orin 的部署方式。

设备暂时不可用不会阻塞 Unit 1–2；但必须记录获取计划。Unit 3 的 CUDA Gate 和
Unit 7–8 的 Orin Gate 不能用桌面 CPU 结果代替。

## 诊断结果与桥接路径

| 结果 | 动作 |
|---|---|
| A、B、C、D 全部通过 | 直接进入 Week 1；仍需完成每周 Exit Ticket |
| 仅一项不通过 | 先做 6 天 Course 0A，再重测该项的新题 |
| 两项或以上不通过 | 做 Course 0A + 0B，共 2 周，再使用新题重测 |
| C++/Linux 明显薄弱 | 暂缓正课；这门课不是 C++ 入门课 |
| 暂无 Orin | 可先学到 Unit 2；Unit 3 前准备 CUDA 环境，Unit 7 前必须有 Orin |

### Course 0A：Python、数组与数学语言

| Day | 90–120 分钟任务 | 必须留下 |
|---:|---|---|
| 1 | Python functions、modules、venv；完成最小脚本和 assertions | 环境记录与脚本 |
| 2 | NumPy shape/dtype/indexing/broadcast | 10 个 array 小实验 |
| 3 | storage、stride、slice、transpose、copy/view | 地址表和共享存储验证 |
| 4 | 向量、矩阵、MatMul；手算后用 NumPy 验证 | 8 道题及订正 |
| 5 | Conv output shape、FLOPs/bytes 的运行时直觉 | 6 道 shape 题 |
| 6 | 新题重测 A+B；低于 80% 时只补失败项 | diagnostic retest |

主材料：

- [NumPy absolute beginners guide](https://numpy.org/doc/stable/user/absolute_beginners.html)
- [D2L: Data Manipulation](https://d2l.ai/chapter_preliminaries/ndarray.html)
- [D2L: Linear Algebra](https://d2l.ai/chapter_preliminaries/linear-algebra.html)

### Course 0B：GitHub、Actions 与可复现环境

| Day | 90–120 分钟任务 | 必须留下 |
|---:|---|---|
| 1 | Git working tree/index/commit/branch | local branch transcript |
| 2 | GitHub Flow：issue→branch→PR→review | 一条完整练习链 |
| 3 | Actions workflow/event/job/runner | workflow 结构图 |
| 4 | 最小权限、untrusted contexts、artifact | threat checklist |
| 5 | clean build、版本 manifest、失败日志 | reproduction transcript |
| 6 | 新题重测 C+D；完成 Orin inventory | diagnostic retest |

主材料：

- [Introduction to Git](https://learn.microsoft.com/en-us/training/modules/intro-to-git/)
- [Introduction to GitHub](https://learn.microsoft.com/en-us/training/modules/introduction-to-github/)
- [Introduction to GitHub Actions](https://learn.microsoft.com/en-us/training/modules/introduction-to-github-actions/)

## 入学完成条件

只有以下条件都满足，才在 `notes/diagnostics/entry.md` 写 `READY FOR WEEK 01`：

- 所有必修诊断项首次或重测达到 80%，订正后 100%；
- CPU clean build/test 有完整 transcript；
- Orin inventory 已完成，或有明确设备准备日期；
- 能用 3 分钟解释自己为何从 Runtime 而不是模型训练切入 AI；
- 已阅读 [`CURRICULUM.md`](CURRICULUM.md) 和
  [`docs/mastery-gates.md`](docs/mastery-gates.md)。
