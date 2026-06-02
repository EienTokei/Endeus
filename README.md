# Endeus — The Janus Door within EnD.C

![Status](https://img.shields.io/badge/version-0.5.0-blue)
![Status](https://img.shields.io/badge/status-alpha-orange)

> Endeus 是一扇门，它是 EnD.C 中所有门的原型，也是这里的“雅努斯之门”
> 
> 写者借它构建故事，读者借它踏入世界  
> 
> 故事于此落幕与开篇，终结与循环于此相遇
> 
> 这就是 Endeus —— 写者与读者各自经过，而祂不介入；二者从未见面，却完成同一次传递

## 简介

Endeus 是一个轻量级视觉小说引擎，使用 **C++20** 和 **SFML 3.0** 构建，强调叙事与传递的仪式感。它不仅是技术实现，更是对“门”的意象的探索。

## 分支说明

本项目使用 `main` + `develop` 双分支策略：

- **`main`**：当前可运行的最新版本（仍可能包含缺陷）。
- **`develop`**：日常开发分支，新功能先在此分支集成，稳定后合并到 `main`。

如需构建最新开发版，请切换到 `develop` 分支：

```bash
git checkout develop
```

## 当前功能

- [x] 显示背景与角色立绘（支持多图层、层级顺序、透明度）
- [x] 对话框显示台词及角色名（支持 UTF-8 中文）
- [x] 鼠标左键推进对话 / 选择选项
- [x] 选项分支及标签跳转（Jump）
- [x] 等待点击指令（Wait）
- [x] 图层移动动画（MoveLayer，非阻塞）
- [x] 图层淡变动画（FadeLayer，非阻塞）
- [x] 动画系统 Anemoi（基于 `std::variant` 类型安全，支持按图层属性管理动画）
- [x] 数据模型与渲染完全分离（World + WorldManager / Executor / Renderer）
- [x] 世界快照与回滚（WhiteAlbum，解决标签回溯时的状态残留）
- [x] 类型安全的事件总线（Leyline）
- [x] 完善的日志系统（spdlog，支持多级别输出和文件记录）
- [x] 单元测试（GoogleTest，覆盖指令、事件总线、世界管理器）
- [x] 内存泄漏检测（Debug 模式）

## 核心模块

> **注意**：Endeus 目前处于 **alpha** 阶段，架构虽已具雏形，但仍在不断探索中，以下模块仅为当前最小实现的拆分，不排除重构的可能。

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

- **WhiteAlbum（白色相簿）**  
  快照管理器，用于保存和恢复标签对应的世界状态。当 `Jump` 到过去的标签时，自动恢复该标签第一次执行时的场景，解决了回滚时图层残留的问题。

## 构建与运行

### 环境要求

- C++20 编译器
- CMake 3.20+
- SFML 3.02+（Graphics, Window, System）
- spdlog

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

## 已知限制

- 剧本目前硬编码在 `Engine::buildScripts()` 中，尚未支持从外部文件加载
- 无音频支持（BGM / 音效）
- 无存档 / 读档功能
- 不支持逐字显示和自动模式
- ...

以上功能将在后续版本中逐步添加。

## 许可证

本项目采用 [MIT 许可证](LICENSE)，您可以自由使用、修改和分发。
