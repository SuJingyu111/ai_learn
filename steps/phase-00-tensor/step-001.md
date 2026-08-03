# Step 001 — 一个能编译的空引擎与 `tt` 命令行

**你会做出**：一个叫 `tt` 的可执行文件，跑起来打印版本号然后退出。

> **`tt` 是什么**：**tiny tensor**，这个引擎的名字。它会出现在三个地方：
> 命令行程序名（`./build-cpu/tt dump`）、C++ 命名空间（`tt::Tensor`）、
> 以及 Step 013 里你自己定的张量文件后缀（`.tt`）。
>
> 名字是你的，想换随时换——**但现在就换**，别等到几十个文件都用上之后。

**为什么是现在**：后面每一步都以"跑一下看看"结束。先把那个"跑一下"准备好，
否则你会在写完 Tensor 之后才发现没有地方能观察它。

## 动手

1. 建 `engine/include/tt/version.h`，定义 `namespace tt { constexpr const char* kVersion = "0.0.1"; }`
2. 建 `engine/apps/tt_main.cpp`：`main` 接受 `argc/argv`，无参数时打印用法，
   `tt version` 打印版本。
3. 在 `CMakeLists.txt` 里加上目标（放在 `resize_demo` 附近）：

```cmake
add_executable(tt engine/apps/tt_main.cpp)
target_include_directories(tt PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/engine/include)
cuda_ai_warnings(tt)
```

## 跑起来

```bash
cmake --build build-cpu --parallel && ./build-cpu/tt version
```

## 你应该看到

```
tt 0.0.1
```

无参数时应该打印用法并以**非零**退出码结束——之后你会用退出码判断步骤是否通过：

```bash
./build-cpu/tt ; echo "exit=$?"
```

```
usage: tt <command> [args]
exit=2
```

## 卡住了

- `cuda_ai_warnings` 未定义 → 你把 `add_executable` 放在了那个 function 定义之前。
- 链接报错找不到 `main` → 检查文件确实被加进了 `add_executable`。
