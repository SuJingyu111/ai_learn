# Step 008 — 多维索引 `at(i,j,k)` 与越界检查

**你会做出**：能按逻辑索引读写元素，越界时立刻失败。

**为什么是现在**：Step 006 只算了 offset，没人验证它对不对。现在把它接到真实
内存上，用手算值断言。同时把越界检查加上——Phase 07 上了 GPU 之后，
越界会变成静默的数据损坏，那时候再加就晚了。

## 动手

1. `template <typename T> T& Tensor::at(std::initializer_list<int64_t> idx)`。
2. 检查：索引个数 == rank，每个 `0 <= idx[i] < dim(i)`，
   请求的 `T` 与 `dtype_` 匹配。
3. 越界信息要**带上是哪一维、给了多少、上界多少**。
   `"index out of range"` 这种信息在调 Conv 的时候等于没有。

## 跑起来

填一个 `(2,3,4)` 的张量为 `0..23`，然后：

```bash
cmake --build build-cpu --parallel && ./build-cpu/tt index-demo
```

## 你应该看到

与 Step 006 手算的完全一致：

```
at(1,2,3) = 23
at(0,1,2) = 6
at(2,0,0) -> error: index 2 out of range for dim 0 (size 2)
at(0,0)   -> error: expected 3 indices, got 2
```

## 卡住了

- `at(1,2,3)` 得到 11 或 17 → strides 用错了顺序，回到 Step 006。
- dtype 不匹配也能跑 → 你的 `at<T>` 没检查类型，加上。
  用 `float` 读一个 `i64` 张量是真实会发生的 bug。
