# Step 014 — 测试骨架：手算 fixture 与容差

**你会做出**：Phase 00 全部内容的回归测试，接进 `ctest`。

**为什么是现在**：Phase 01 开始就是数学了，改动会频繁破坏前面的东西。
现在把地基锁住。仓库里已经有一套现成的骨架可以照抄。

## 需要知道的（5 分钟）

看一眼 `engine/tests/test_support.h`：它有一个带 padding 哨兵的 `HostImage`，
可以证明"没有人写出了逻辑边界"。你的 Tensor 测试需要同样的能力。

`engine/tests/test_resize.cpp` 展示了手算 fixture 的写法。

## 动手

1. 建 `engine/tests/test_tensor.cpp`，注册进 `CMakeLists.txt`
   （照抄 `resize_tests` 那几行，改名字）。
2. 覆盖：strides 推导、flat offset、越界、reshape 共享与失败、
   slice 的 offset、permute 后的 strides、contiguous 的复制与数据顺序。
3. **每个断言的期望值都手算写死**，不要用另一份代码算出来对比——
   那只能证明两份代码一致，不能证明它们对。
4. 加一条"没写出边界"的检查：分配比需要更大的 storage，
   在尾部填哨兵值，操作后确认哨兵没变。

## 跑起来

```bash
./scripts/build_and_test.sh cpu
```

## 你应该看到

```
1/2 Test #1: resize_correctness ...............   Passed
2/2 Test #2: tensor_correctness ...............   Passed

100% tests passed out of 2
```

## 卡住了

- 测试全过但你不信 → 故意把 `contiguous_strides()` 的累乘方向改反，
  重新编译。**如果测试还是全过，说明你的测试没覆盖到它。**
  这个手法（变异测试）之后每加一批测试都值得做一次。

---

### 🎯 Additional goal — 你的测试到底覆盖了什么（可选）

**假设**：现有测试对 strides 的覆盖是充分的，但对 offset 的覆盖有洞——
把 `flat_offset` 里的 `offset_` 项删掉，只有 slice 相关的测试会失败。

**做什么**：逐个引入下面这些变异，每次只改一处，记录哪些测试失败：

1. `contiguous_strides()` 累乘方向反转
2. `flat_offset()` 去掉 `+ offset_`
3. `slice()` 忘记更新 offset
4. `permute()` 只重排 shape 不重排 strides
5. `is_contiguous()` 恒返回 true

**测什么**：每个变异下失败的测试**名字**和**数量**。做一张 5×N 的表。

**你可能会发现**：变异 4 和 5 可能一个测试都不失败——因为断言只看了元数据，
没有真正读数据验证。如果是这样，补一条"permute 后 `at(0,1,0)` 应该等于 12"。

**为什么会这样**：只断言元数据的测试无法区分"元数据对但数据读错"。
真实 runtime 里这类 bug 表现为"shape 全对，结果是噪声"。

**写下来**：变异表 → 哪些格子是空的 → 你补了什么测试 → 补完后表变成什么样。
