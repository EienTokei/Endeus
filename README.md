# Endeus — The Janus Door within EnD.C

![Status](https://img.shields.io/badge/status-pre--alpha-orange)

> Endeus 是一扇门，它是 EnD.C 中所有门的原型，也是这里的“雅努斯之门”
> 
> 写者借它构建故事，读者借它踏入世界  
> 
> 故事于此落幕与开篇，终结与循环于此相遇
> 
> 这就是 Endeus —— 写者与读者各自经过，而祂不介入；二者从未见面，却完成同一次传递

## 简介

Endeus 是一个轻量级视觉小说引擎，使用 **C++20** 和 **SFML 3.0** 构建，强调叙事与传递的仪式感。它不仅是技术实现，更是对“门”的意象的探索。

## 当前功能

- [x] 显示背景与角色立绘
- [x] 对话框显示台词及角色名
- [x] 鼠标左键推进对话 / 选择选项
- [x] 选项分支及跳转
- [x] 内存泄漏检测（Debug 模式）
- [x] 基础指令集架构
- [x] 类型安全的事件总线
- [x] 图层移动动画（非阻塞，MoveLayer 指令）
- [x] 动画系统架构（Anemoi，支持按图层属性管理动画）
- [x] 数据模型与渲染完全分离（World + WorldManager / Executor / Renderer）
- [x] 类型安全的动画键（AnemosKey）
- [x] 单元测试（GoogleTest，覆盖指令、事件总线及核心数据模型）

## 核心模块

> **注意**：Endeus 目前处于 **pre-alpha** 阶段，架构仍在探索中，以下模块仅为当前最小实现的拆分，未来可能重构。

![架构图](docs/architecture.svg)

- **Director（导演）**  
  负责脚本的执行流程控制：维护指令指针（PC）、处理标签跳转、等待异步操作（如点击、选项选择），并通过事件总线响应外部输入。它不关心具体画面如何渲染，只决定下一步执行哪条指令。  

- **World & WorldManager（世界数据与管理器）**  
  `World` 是纯数据容器，包含所有图层（位置、透明度、可见性）、说话人、当前对话内容。`WorldManager` 提供修改接口（如 `setLayerPosition`），并自动维护脏标记，为未来的快照和回滚预留基础。  

- **Executor（指令执行器）**  
  实现 `IExecutor` 接口，执行具体的画面指令（ShowLayer、HideLayer、MoveLayer、FadeLayer、SetSpeaker、SetContent 等）。它直接修改 `WorldManager` 中的世界状态，并启动动画（通过 `Anemoi`）。  

- **Renderer（渲染器）**  
  只负责绘制：从 `WorldManager` 读取当前世界状态，从 `Anemoi` 获取动画覆盖值（位置/透明度），并将所有图层、对话框、选项按钮绘制到窗口上。同时处理鼠标点击命中测试并发布事件（`ChoiceSelected` / `ActionCompleted`）。  

- **EventBus（事件总线）**  
  类型安全的消息中枢，用于解耦 Director、Renderer 和输入响应。例如鼠标点击 → 发布 `MouseClicked` → Renderer 判断 → 发布 `ChoiceSelected` → Director 跳转。

- **Instruction（指令）**  
  所有脚本动作的静态描述。Director 逐条读取，同步指令由 Executor 立即执行，异步指令（`Wait`、`Choice`）则等待事件完成后再推进。

- **Anemoi（动画系统）**  
  风神，让画面随风而动的动画系统。管理图层的属性动画（移动、淡变等）。基于 `AnemosKey`（图层ID+属性）维护动画映射，确保同一属性同时只有一个动画。动画以非阻塞方式运行，动画当前值会被 `Renderer` 覆盖到精灵上，不污染世界数据。

## 构建与运行

### 环境要求

- C++20 编译器
- CMake 3.20+
- SFML 3.02+（Graphics, Window, System）

### 步骤

1. 克隆仓库  

```bash
git clone https://github.com/EienTokei/Endeus.git
cd Endeus
```

2. 准备素材

根据 [`docs/ASSETS_REQUIRED.md`](docs/ASSETS_REQUIRED.md)  中的要求，将图片和字体等放入 `Endeus/assets/`。

3. 构建

```bash
cmake -B build -S .
cmake --build build --config Release
```

4. 运行

```bash
./bin/Endeus.exe   # Windows
```

## 操作说明

- 鼠标左键：推进对话 / 选择选项
- 点击部分选项后，再次点击任意位置结束

## 许可证

本项目采用 [MIT 许可证](LICENSE)，您可以自由使用、修改和分发。