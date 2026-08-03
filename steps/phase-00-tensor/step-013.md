# Step 013 — 从文件读写张量（你自己的 `.tt` 格式）

**你会做出**：`tt save` / `tt load`，把张量存成文件再读回来。

**为什么是现在**：Phase 05 要和 onnxruntime 逐层对拍，那时需要一个"把我的中间
张量和 Python 那边的中间张量放在一起比"的通道。现在做好，那时直接用。
同时这也是你第一次处理二进制格式——Phase 03 解析 ONNX 时会用上同样的手感。

## 动手

1. 定一个最简单的格式，写进注释：magic(4B) + version(u32) + dtype(u32) +
   rank(u32) + dims(i64 × rank) + 原始数据。**小端**，写清楚。
2. 只保存 contiguous 张量（不连续的先 `contiguous()`）——
   这样文件里的字节顺序就是逻辑顺序，Python 侧 `np.fromfile` 直接能读。
3. 读回来时校验 magic 和 version，不匹配立刻报错。

## 跑起来

```bash
cmake --build build-cpu --parallel
./build-cpu/tt save /tmp/a.tt && ./build-cpu/tt load /tmp/a.tt
```

## 你应该看到

存进去和读出来的元数据与前 8 个值完全一致：

```
saved  shape=(2,3,4) dtype=f32 -> /tmp/a.tt (128 bytes)
loaded shape=(2,3,4) dtype=f32 values: 0 1 2 3 4 5 6 7 ...
```

128 = 头部 32 字节 + 数据 96 字节。**如果你的头部大小不同，把这个数字改成你的。**

用 Python 确认格式确实是通用的：

```bash
python3 -c "
import numpy as np
print(np.fromfile('/tmp/a.tt', dtype=np.float32, offset=32)[:8])"
```

```
[0. 1. 2. 3. 4. 5. 6. 7.]
```

## 卡住了

- Python 读出来是乱码 → offset 写错了，或者你写了 padding 但没算进去。
