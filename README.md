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

## 分支说明

本项目使用 `main` + `develop` 双分支策略：

- **`main`**：当前可运行的最新版本（仍可能包含缺陷）。
- **`develop`**：日常开发分支，新功能先在此分支集成，稳定后合并到 `main`。

如需构建最新开发版，请切换到 `develop` 分支：

```bash
git checkout develop
```

## 当前功能

- [x] 显示背景与角色立绘
- [x] 对话框显示台词及角色名
- [x] 鼠标左键推进对话
- [x] 二选一分支
- [x] 内存泄漏检测（Debug 模式）

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

根据 `docs/ASSETS_REQUIRED.md` 中的要求，将图片和字体放入 `Endeus/assets/`。

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
