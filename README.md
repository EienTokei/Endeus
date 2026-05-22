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
- [x] 单元测试（GoogleTest，覆盖指令、事件总线及核心数据模型 WorldModel）

## 核心模块

> **注意**：Endeus 目前处于 **pre-alpha** 阶段，架构仍在探索中，以下模块仅为当前最小实现的拆分，未来可能重构。

- **Director（导演）**  
  负责脚本的执行流程控制：维护指令指针（PC）、处理标签跳转、等待异步操作（如点击、选项选择），并通过事件总线响应外部输入。它不关心具体画面如何渲染，只决定下一步执行哪条指令。

- **Runtime（运行时）**  
  管理游戏的实际状态：纹理资源、图层（Layer）、对话框文字、选项按钮等。它执行具体的画面指令（显示/隐藏图层、设置说话人/内容），处理鼠标点击的命中测试（选项区域），并将所有内容绘制到窗口上。

- **EventBus（事件总线）**  
  类型安全的消息中枢，用于解耦 Director、Runtime 和输入响应。例如鼠标点击 → 发布 `MouseClicked` → Runtime 判断是否点中选项 → 发布 `ChoiceSelected` → Director 跳转到对应标签。

- **Instruction（指令）**  
  所有脚本动作的静态描述。Director 逐条读取，同步指令由 Runtime 立即执行，异步指令则等待事件完成后再推进。

- **WorldModel（世界模型）**  
  纯数据模型，存储所有逻辑状态：图层（位置、透明度、可见性）、对话框文本、选项列表。与渲染逻辑分离，支持脏标记高效同步，并为动画系统和存档提供稳定接口。

## 构建与运行

### 环境要求

- C++20 编译器
- CMake 3.20+
- SFML 3.02+（Graphics, Window, System）

### 步骤

1. 克隆仓库  

```bash
git clone https://github.com/yourname/Endeus.git
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
- 点击选项后，再次点击任意位置结束

## 许可证

本项目采用 [MIT 许可证](LICENSE)，您可以自由使用、修改和分发。