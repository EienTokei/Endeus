# Endeus 引擎日志规范

## 日志级别

| 级别 | 用途 | 示例 |
|------|------|------|
| `error` | 不可恢复错误 | 字体加载失败、脚本语法错误 |
| `warn` | 可恢复异常，需关注 | 图层不存在、跳转到未知标签 |
| `info` | 里程碑事件 | 引擎启动/关闭、资源加载完成、存档成功 |
| `debug` | 开发时关注的业务流程 | 执行指令、标签跳转、选项显示、对话变更 |
| `trace` | 极细粒度调试信息，默认不输出 | 动画每帧进度、鼠标坐标、图层属性微小变化 |

> Debug 配置下 `trace` 默认不输出（`SPDLOG_ACTIVE_LEVEL` 为 `SPDLOG_LEVEL_DEBUG`），需要时可临时调整。

## 消息格式

- 统一使用 `动作: 参数1=值1, 参数2=值2` 形式。
- 不需要添加类名前缀（日志已包含文件名和行号）。
- 关键参数必须有值，避免模糊的“已执行”。
- 表示变化/方向时推荐使用 `->`，前后加空格。

### 示例

> ShowLayer: id=bg_janus, texture=bg_janus, pos=(0,0), alpha=0, order=0
> 
> MoveLayer: id=door, from (600,200) -> (200,200), duration=3s
> 
> Jump: to=reader (pc 5 -> 28)
> 
> SetContent: append=false, text='......'

### 布尔值

使用 `true` / `false` 小写。

> SetContent: append=true, text='Hello'

### 字符串内容

如果内容可能很长，截断并加省略号，用单引号包裹。

> SetContent: text='这是一段很长很长很长的对话内容...'

## 重复日志原则

- 同一业务操作**只在最上层（Executor/Director）打印一条 debug 日志**。
- 下层（WorldManager/Anemoi 内部）不打印 debug 级别的相同信息，如需细节使用 `trace`。
- 如果两个模块打印的信息互补（如 Executor 输出文本内容，WorldManager 输出模式），考虑合并到 Executor 一条日志中。
- 本质相同的系列操作（如保存/恢复各个快照），日志内容（级别、格式、详细程度）应保持一致，避免因模块不同而产生不对称的观测体验。

---

*本规范为推荐性约定，如有冲突以代码可读性和调试效率优先。*