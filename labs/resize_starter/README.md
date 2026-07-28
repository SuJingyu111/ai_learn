# `labs/resize_starter/` — G4 之前的 Resize 隔离工作区

## 这个目录存在的原因

仓库里已经有一份可用的 Resize reference 实现：

- `src/cpu/resize_cpu.cpp`
- `src/cuda/resize_cuda.cu`

`CURRICULUM.md`「学术诚信与 reference solution」和 `docs/mastery-gates.md`
「Assessment integrity」都规定：**这两个文件在 G4 通过前视为封存的参考答案。**
Unit 4 的 M4 milestone 要证明的是你能独立推导 Resize 的坐标语义、边界行为和
CUDA 实现——读过 reference 再改个名字，证明不了这件事。

所以 U1–U4 期间所有 Resize 相关的独立实现写在这里，而不是直接改 `src/`。

## 规则

1. **G4 通过前不打开** `src/cpu/resize_cpu.cpp` 和 `src/cuda/resize_cuda.cu`。
2. 本目录的实现必须自带 correctness oracle——手算固定用例，或独立写的 CPU 参考，
   不是从 `src/` 抄来的那一份。
3. 坐标语义以 ONNX Resize schema 为准（见 `include/cuda_ai/resize.h` 顶部的契约
   注释；那段注释描述的是 **API 契约**，不是实现，U4 之前可以读）。
4. G4 通过后才做 differential review：把你的实现和 reference 逐行对照，把差异
   写进 M4 case study。这是 Gate 之后的动作，不是 Gate 之前的捷径。

## 建议布局

```text
labs/resize_starter/
├── cpu/          # 独立 CPU oracle（U4 W22）
├── cuda/         # naive → profiler-driven optimized（U4 W23）
└── tests/        # 手算固定用例、边界、fixed-seed 随机
```

子目录按需创建。本目录不参与顶层 CMake 构建；需要构建时在本目录内自带
`CMakeLists.txt`，避免污染主工程的 reference 边界。
