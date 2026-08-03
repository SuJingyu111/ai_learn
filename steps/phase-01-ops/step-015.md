# Step 015 — 逐元素算子框架与 `relu`

**你会做出**：一个能对任意 shape 的张量逐元素施加函数的框架，第一个实例是 relu。

**为什么是现在**：逐元素算子是最简单的一类，正好用来把"算子"这个概念的骨架
搭起来：输入输出怎么传、shape 怎么校验、非 contiguous 怎么办。

## 动手

1. `engine/include/tt/ops.h`：`Tensor relu(const Tensor&)`。
2. 内部用一个通用的 `map_elementwise(in, out, fn)`，按**逻辑索引**遍历，
   不要假设 contiguous——Step 011 已经能造出非连续张量了。
3. 输出张量新分配，shape 与 dtype 同输入。先不做 in-place。

## 跑起来

```bash
cmake --build build-cpu --parallel && ./build-cpu/tt op-demo relu
```

## 你应该看到

```
in : -2 -1 0 1 2
out:  0  0 0 1 2
```

对一个 permute 过的非连续张量也要正确：

```
permuted input contiguous=no -> relu ok
```

## 卡住了

- 非连续输入结果错乱 → 你用裸指针线性遍历了。改用 `at()` 或按 strides 计算。
  慢没关系，Phase 06 才优化。
