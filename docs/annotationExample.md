# IC10 注解语法完整示例

<details>
   <summary>中文版</summary>

## 第一部分：词法级判定规则

- `#>` 和 `#:` 必须紧接 `@` 标签，且标签名必须在预定义集合中，否则降级为普通注释。
- 必须使用 ASCII 冒号 `:` (0x3A)，不能使用全角冒号 `：` (U+FF1A)。
- 文档标签（`DOC_TAGS`）：`device`, `end-device`, `enum`, `end-enum`, `name`, `desc`, `value`, `slot`, `logic`, `mode`, `logicSlot`, `connect`
- 类型提示标签（`TYPE_HINT_TAGS`）：`type`, `desc`

以下写法均会降级为普通注释：
```ic10
# #> no-at-sign              ← 缺少 @
# #> @unknownTag x           ← @unknownTag 不在 DOC_TAGS 中
# #: no-at-sign              ← 缺少 @
# #: @unknownTag x           ← @unknownTag 不在 TYPE_HINT_TAGS 中
# alias x d0 #：@type Furnace   ← 使用了全角冒号 '：' 非 ':'
```

## 第二部分：文档注释 — 设备块 (`#> @device ... #> @end-device`)

- 块入口：首条必须是 `@device` 或 `@enum`，否则报错。
- 块结束：必须对应 `@end-device` 或 `@end-enum`，否则报错。
- `@desc` 支持 `$link` 引用（以 `$` 开头表示链接，否则为纯文本）。

### 2.1 最简设备块
```ic10
#> @device
#> @name SimpleSensor
#> @end-device
```

### 2.2 完整设备块（所有标签）
```ic10
#> @device
#> @name Furnace
#> @desc 冶炼炉窑，用于熔化金属矿石
#> @slot 0 input
#> @slot 1 output
#> @slot 2 input 输入槽 2 矿石槽
#> @logic Pressure r
#> @logic Temperature r
#> @logic Setting rw
#> @logic On w
#> @logic Charge wr
#> @mode 0
#> @mode 1 运行态
#> @mode 2 超温保护态
#> @logicSlot Quantity
#> @logicSlot Charge
#> @logicSlot Oxidizer
#> @connect 0
#> @connect 1 进气
#> @end-device
```

### 2.3 字段详解
- `@name <name>` — 设备类型名称（必须）
- `@desc <description>` — 描述（支持 `$link`）
- `@slot <index> <direction> [desc]` — 方向：`input` / `output`，其他值视为默认
- `@logic <name> <access>` — 访问权限：`r`（读）、`w`（写）、`rw` 或 `wr`（读写）
- `@mode <index> [desc]` — 模式索引及描述
- `@logicSlot <name>` — 逻辑槽名称
- `@connect <index> [desc]` — 连接索引及描述

### 2.4 使用 `$link` 引用描述
```ic10
#> @device
#> @name AdvancedGasSensor
#> @desc $./locals/devices.AdvancedGasSensor.desc
#> @logic Pressure r $./locals/devices.AdvancedGasSensor.logics.Pressure.desc
#> @logic Temperature r $./locals/devices.AdvancedGasSensor.logics.Temperature.desc
#> @end-device
```

## 第三部分：文档注释 — 枚举块 (`#> @enum ... #> @end-enum`)

- `@name <name>` — 枚举类型名称（必须）
- `@desc <description>` — 描述（支持 `$link`）
- `@value <name> <value> [desc]` — 枚举值名称、数值（或 `None`）及描述

### 3.1 最简枚举块
```ic10
#> @enum
#> @name YesNo
#> @end-enum
```

### 3.2 完整枚举块
```ic10
#> @enum
#> @name Color
#> @desc 颜色枚举，用于显示设备
#> @value Black 7 黑色
#> @value Blue 0
#> @value Green 2 绿色
#> @value Red 4 红色
#> @value White 6 白色
#> @end-enum
```

### 3.3 使用 `None` 值
```ic10
#> @enum
#> @name DisplayMode
#> @desc 显示模式枚举
#> @value Default 0 默认
#> @value Percent 1 百分比
#> @value Kelvin None 开尔文
#> @value String None 字符串
#> @end-enum
```

### 3.4 使用 `$link` 的枚举
```ic10
#> @enum
#> @name ReagentMode
#> @desc $./locals/enums.ReagentMode.desc
#> @value Contents 0 $./locals/enums.ReagentMode.enums.Contents.desc
#> @value Required 1 $./locals/enums.ReagentMode.enums.Required.desc
#> @value Recipe 2 $./locals/enums.ReagentMode.enums.Recipe.desc
#> @end-enum
```

## 第四部分：类型提示 — `alias #: @type` / `@desc`

- `@type <value>` — 关联的设备类型名称（不能为枚举类型名，否则报错）。
- `@desc <value>` — 描述。
- 标签顺序可任意；值可用双引号括起以支持空格和特殊字符。
- 类型提示会影响后续指令中对别名操作数的合法性检查（如 `l`、`s`、`lb`、`ls` 等）。

### 4.1 alias + 仅 `@type`
```ic10
alias furnace d0 #: @type Furnace
```

### 4.2 alias + 仅 `@desc`
```ic10
alias tempReg r0 #: @desc 温度寄存器
```

### 4.3 alias + `@type` + `@desc` 同顺序
```ic10
alias valve d1 #: @type Valve @desc 主流量控制阀
```

### 4.4 alias + `@desc` + `@type` 反顺序
```ic10
alias sensor d2 #: @desc 气体混合传感器 @type Furnace
```

### 4.5 alias + 带引号值
```ic10
alias outletReg r3 #: @type "Furnace-v2" @desc "主输出槽，含 @lock"
```

### 4.6 alias + 描述中的空格
```ic10
alias statusReg r4 #: @desc 状态寄存器，用于存储上一次错误码
```

### 4.7 alias + 寄存器引用
```ic10
alias stackPtr r15 #: @type __register__ @desc 栈指针寄存器
```

## 第五部分：类型提示 — `define #: @type` / `@desc`

- 对 `define`，`@type` 仅两个魔法字符串生效：
  - `__register__` — 将常量视为寄存器类型
  - `__device__` — 将常量视为设备类型
- 其他值无特殊语义。

### 5.1 define + `@type __register__`
```ic10
define ra 1 #: @type __register__ @desc 返回地址寄存器，用于子程序调用
```

### 5.2 define + `@type __device__`
```ic10
define db 2 #: @type __device__ @desc 自身设备引脚，指向 IC10 所在设备
```

### 5.3 define + 普通 `@desc` (无 `@type`)
```ic10
define MAX_PRESSURE 101325 #: @desc 标准大气压力值，单位 Pa
```

### 5.4 define + `@type` + `@desc`
```ic10
define STACK_BASE 3 #: @type __register__ @desc 栈底寄存器
```

### 5.5 define + HASH/STR 值
```ic10
define DEV_FURNACE HASH("StructureFurnace") #: @desc 炉窑设备哈希值
define STR_HELLO STR("Hello") #: @desc 示例字符串常量
```

## 第六部分：完整端到端示例

### 声明类型
```ic10
#> @device
#> @name Reactor
#> @desc 化学反应器
#> @slot 0 input 原料入口
#> @slot 1 output 产物出口
#> @logic Pressure r 反应器压力
#> @logic Temperature r 反应器温度
#> @logic Setting rw 温度设定
#> @logic On w 启停开关
#> @logicSlot Quantity 数量槽
#> @logicSlot Oxidizer 氧化剂槽
#> @logicSlot Fuel 燃料槽
#> @mode 0 待机
#> @mode 1 运行中
#> @mode 2 停机中
#> @connect 0 入口管
#> @connect 1 出口管
#> @end-device

#> @enum
#> @name GasType
#> @desc 气体类型枚举
#> @value Oxygen 0
#> @value Nitrogen 1
#> @value Carbon 2
#> @value Hydrogen 3
#> @end-enum
```

### 绑定别名
```ic10
alias reactor d0 #: @type Reactor @desc 主反应器
alias display d1 #: @desc 七段显示器
alias pressure r0 #: @desc 暂存压力读数
alias setting  r1 #: @desc 暂存温度设定值
```

### 使用别名编写指令
```ic10
read_loop:
    l pressure reactor Pressure    # 读取反应器压力 (Pressure ∈ Reactor.logics)
    s reactor Setting setting      # 设置温度 (Setting ∈ Reactor.logics)
    yield
    j read_loop
```

> 语义检查：指令中的逻辑名、逻辑槽名、槽索引会从对应设备类型中查找，若不存在则产生诊断。

## 第七部分：参数拆分规则汇总

- **文档注释标签（除 `@desc`）**：按空白分割参数。  
  例如：`@slot 0 input 矿石槽` → `["0","input","矿石槽"]`
- **`@desc` 标签**：将剩余内容整体作为一个参数（保留空格）。  
  例如：`@desc 炉窑 熔化 金属 矿石` → `["炉窑 熔化 金属 矿石"]`
- **类型提示标签（`@type` / `@desc`）**：
  - 若值以 `"` 开头，则匹配闭合引号（支持空格和 `@`），未闭合则取到行尾。
  - 否则，值取到下一个 `@` 或行尾，去除首尾空白。
- **描述中的 `$link`**：若描述参数以 `$` 开头，则视为链接引用（去掉 `$`）；否则视为纯文本。

</details>

---

# IC10 Annotation Syntax Complete Example

## Part 1: Lexer-Level Syntax Rules

- `#>` and `#:` must be immediately followed by an `@` tag, and the tag name must belong to a predefined set; otherwise, the line degrades to a normal comment.
- ASCII colon `:` (0x3A) is required; full-width colon `：` (U+FF1A) is not accepted.
- Doc tags (`DOC_TAGS`): `device`, `end-device`, `enum`, `end-enum`, `name`, `desc`, `value`, `slot`, `logic`, `mode`, `logicSlot`, `connect`
- Type‑hint tags (`TYPE_HINT_TAGS`): `type`, `desc`

The following examples all degrade to normal comments:
```ic10
# #> no-at-sign              ← missing @
# #> @unknownTag x           ← @unknownTag not in DOC_TAGS
# #: no-at-sign              ← missing @
# #: @unknownTag x           ← @unknownTag not in TYPE_HINT_TAGS
# alias x d0 #：@type Furnace   ← uses full-width colon '：' instead of ':'
```

## Part 2: Documentation Comment — Device Block (`#> @device ... #> @end-device`)

- Block start: the first tag must be `@device` or `@enum` (otherwise an error is raised).
- Block end: must be `@end-device` or `@end-enum` respectively (otherwise an error).
- `@desc` supports `$link` references (a leading `$` indicates a link; otherwise plain text).

### 2.1 Minimal Device Block
```ic10
#> @device
#> @name SimpleSensor
#> @end-device
```

### 2.2 Full Device Block (All Tags)
```ic10
#> @device
#> @name Furnace
#> @desc 冶炼炉窑，用于熔化金属矿石
#> @slot 0 input
#> @slot 1 output
#> @slot 2 input 输入槽 2 矿石槽
#> @logic Pressure r
#> @logic Temperature r
#> @logic Setting rw
#> @logic On w
#> @logic Charge wr
#> @mode 0
#> @mode 1 运行态
#> @mode 2 超温保护态
#> @logicSlot Quantity
#> @logicSlot Charge
#> @logicSlot Oxidizer
#> @connect 0
#> @connect 1 进气
#> @end-device
```

### 2.3 Field Details
- `@name <name>` — Device type name (required)
- `@desc <description>` — Description (supports `$link`)
- `@slot <index> <direction> [desc]` — Direction: `input` / `output`; other values default to unset
- `@logic <name> <access>` — Access: `r` (read), `w` (write), `rw` or `wr` (read‑write)
- `@mode <index> [desc]` — Mode index and optional description
- `@logicSlot <name>` — Logic slot name
- `@connect <index> [desc]` — Connection index and optional description

### 2.4 Using `$link` for Description References
```ic10
#> @device
#> @name AdvancedGasSensor
#> @desc $./locals/devices.AdvancedGasSensor.desc
#> @logic Pressure r $./locals/devices.AdvancedGasSensor.logics.Pressure.desc
#> @logic Temperature r $./locals/devices.AdvancedGasSensor.logics.Temperature.desc
#> @end-device
```

## Part 3: Documentation Comment — Enum Block (`#> @enum ... #> @end-enum`)

- `@name <name>` — Enum type name (required)
- `@desc <description>` — Description (supports `$link`)
- `@value <name> <value> [desc]` — Enum value name, numeric value (or `None`), and optional description

### 3.1 Minimal Enum Block
```ic10
#> @enum
#> @name YesNo
#> @end-enum
```

### 3.2 Full Enum Block
```ic10
#> @enum
#> @name Color
#> @desc 颜色枚举，用于显示设备
#> @value Black 7 黑色
#> @value Blue 0
#> @value Green 2 绿色
#> @value Red 4 红色
#> @value White 6 白色
#> @end-enum
```

### 3.3 Using `None` Value
```ic10
#> @enum
#> @name DisplayMode
#> @desc 显示模式枚举
#> @value Default 0 默认
#> @value Percent 1 百分比
#> @value Kelvin None 开尔文
#> @value String None 字符串
#> @end-enum
```

### 3.4 Enum with `$link`
```ic10
#> @enum
#> @name ReagentMode
#> @desc $./locals/enums.ReagentMode.desc
#> @value Contents 0 $./locals/enums.ReagentMode.enums.Contents.desc
#> @value Required 1 $./locals/enums.ReagentMode.enums.Required.desc
#> @value Recipe 2 $./locals/enums.ReagentMode.enums.Recipe.desc
#> @end-enum
```

## Part 4: Type Hints — `alias #: @type` / `@desc`

- `@type <value>` — Associates a device type name (must not be an enum type name, otherwise an error is raised).
- `@desc <value>` — Description.
- Tag order is arbitrary; values can be quoted with double quotes to include spaces and special characters.
- Type hints affect subsequent instruction legality checks (e.g., `l`, `s`, `lb`, `ls`) for the alias.

### 4.1 alias with `@type` Only
```ic10
alias furnace d0 #: @type Furnace
```

### 4.2 alias with `@desc` Only
```ic10
alias tempReg r0 #: @desc 温度寄存器
```

### 4.3 alias with `@type` then `@desc`
```ic10
alias valve d1 #: @type Valve @desc 主流量控制阀
```

### 4.4 alias with `@desc` then `@type` (order independent)
```ic10
alias sensor d2 #: @desc 气体混合传感器 @type Furnace
```

### 4.5 alias with Quoted Values
```ic10
alias outletReg r3 #: @type "Furnace-v2" @desc "主输出槽，含 @lock"
```

### 4.6 alias with Spaces in Description
```ic10
alias statusReg r4 #: @desc 状态寄存器，用于存储上一次错误码
```

### 4.7 alias with Register Reference
```ic10
alias stackPtr r15 #: @type __register__ @desc 栈指针寄存器
```

## Part 5: Type Hints — `define #: @type` / `@desc`

- For `define`, `@type` only recognises two magic strings:
  - `__register__` — treats the constant as a register type
  - `__device__` — treats the constant as a device type
- Any other value has no special semantic effect.

### 5.1 define with `@type __register__`
```ic10
define ra 1 #: @type __register__ @desc 返回地址寄存器，用于子程序调用
```

### 5.2 define with `@type __device__`
```ic10
define db 2 #: @type __device__ @desc 自身设备引脚，指向 IC10 所在设备
```

### 5.3 define with Plain `@desc` (no `@type`)
```ic10
define MAX_PRESSURE 101325 #: @desc 标准大气压力值，单位 Pa
```

### 5.4 define with `@type` and `@desc`
```ic10
define STACK_BASE 3 #: @type __register__ @desc 栈底寄存器
```

### 5.5 define with HASH/STR Values
```ic10
define DEV_FURNACE HASH("StructureFurnace") #: @desc 炉窑设备哈希值
define STR_HELLO STR("Hello") #: @desc 示例字符串常量
```

## Part 6: Full End‑to‑End Example

### Declare Types
```ic10
#> @device
#> @name Reactor
#> @desc 化学反应器
#> @slot 0 input 原料入口
#> @slot 1 output 产物出口
#> @logic Pressure r 反应器压力
#> @logic Temperature r 反应器温度
#> @logic Setting rw 温度设定
#> @logic On w 启停开关
#> @logicSlot Quantity 数量槽
#> @logicSlot Oxidizer 氧化剂槽
#> @logicSlot Fuel 燃料槽
#> @mode 0 待机
#> @mode 1 运行中
#> @mode 2 停机中
#> @connect 0 入口管
#> @connect 1 出口管
#> @end-device

#> @enum
#> @name GasType
#> @desc 气体类型枚举
#> @value Oxygen 0
#> @value Nitrogen 1
#> @value Carbon 2
#> @value Hydrogen 3
#> @end-enum
```

### Bind Aliases to Types
```ic10
alias reactor d0 #: @type Reactor @desc 主反应器
alias display d1 #: @desc 七段显示器
alias pressure r0 #: @desc 暂存压力读数
alias setting  r1 #: @desc 暂存温度设定值
```

### Use Aliases in Instructions
```ic10
read_loop:
    l pressure reactor Pressure    # reads reactor pressure (Pressure ∈ Reactor.logics)
    s reactor Setting setting      # sets temperature (Setting ∈ Reactor.logics)
    yield
    j read_loop
```

> Semantic checking: logic names, logic slot names, and slot indices are looked up in the corresponding device type; if not found, a diagnostic is issued.

## Part 7: Argument Splitting Rules Summary

- **Doc‑comment tags (except `@desc`)**: split by whitespace.  
  Example: `@slot 0 input 矿石槽` → `["0","input","矿石槽"]`
- **`@desc` tag**: takes the entire remaining content as a single argument (preserving spaces).  
  Example: `@desc 炉窑 熔化 金属 矿石` → `["炉窑 熔化 金属 矿石"]`
- **Type‑hint tags (`@type` / `@desc`)**:
  - If the value starts with `"`, it matches the closing quote (supports spaces and `@`); if unclosed, takes to end of line.
  - Otherwise, the value runs until the next `@` or end of line, with leading/trailing whitespace trimmed.
- **`$link` in descriptions**: if a description argument starts with `$`, it is treated as a link reference (the `$` is removed); otherwise it is plain text.