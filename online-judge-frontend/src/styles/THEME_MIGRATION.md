# Online Judge 主题迁移指南

> **文档更新时间**: 2025-12-08  
> **作者**: Copilot CLI (自动生成)  
> **目的**: 指导开发者如何新增/修改主题 Token 并正确应用到组件中

---

## 📋 目录

1. [概述](#概述)
2. [主题架构](#主题架构)
3. [新增 Token 流程](#新增-token-流程)
4. [Element Plus 变量映射](#element-plus-变量映射)
5. [最佳实践](#最佳实践)
6. [常见问题](#常见问题)

---

## 概述

Online Judge 采用**层级化主题系统**，确保深色/浅色主题无缝切换，并与 Element Plus 组件库完美集成。

### 核心原则

1. **Token 优先**: 所有颜色/间距/字体使用语义化 Token（CSS 变量）
2. **主题无关**: Token 在 `tokens/` 定义，具体值在 `themes/` 映射
3. **渐进增强**: 新增 Token 不破坏现有组件
4. **可访问性**: 确保 WCAG 2.1 AA 级别对比度

---

## 主题架构

```
src/styles/
├── tokens/              # 设计 Token（抽象层）
│   ├── colors.css       # 颜色系统
│   ├── typography.css   # 字体系统
│   ├── spacing.css      # 间距系统
│   ├── breakpoints.css  # 响应式断点
│   ├── accessibility.css # 可访问性 Token
│   └── rgba-helpers.css # RGBA 透明度辅助
├── themes/              # 主题实现（具体值）
│   └── theme.css        # 深色/浅色主题映射
├── element/             # Element Plus 覆盖
│   └── theme.css        # Element Plus 变量映射
├── base/                # 基础样式
│   └── reset.css        # 全局重置
├── utilities/           # 工具类
│   └── utilities.css    # 通用 UI 组件类
└── index.css            # 样式入口（按顺序导入）
```

---

## 新增 Token 流程

### Step 1: 定义基础 Token

在 `tokens/colors.css` 中添加**颜色常量**（深色主题为主）：

```css
/* tokens/colors.css */
:root {
    /* 新增：品牌辅助色 */
    --oj-color-accent: #8b5cf6; /* 紫色 */
    --oj-color-accent-light: #a78bfa;
    --oj-color-accent-dark: #7c3aed;
    --oj-color-accent-soft: rgb(139 92 246 / 10%);

    /* RGB 值（用于透明度计算） */
    --oj-color-accent-rgb: 139 92 246;
}
```

### Step 2: 在 RGBA Helpers 中添加透明度变体

在 `tokens/rgba-helpers.css` 中添加：

```css
/* tokens/rgba-helpers.css */
:root {
    /* Accent 紫色透明度 */
    --oj-accent-alpha-90: rgb(var(--oj-color-accent-rgb) / 0.9);
    --oj-accent-alpha-80: rgb(var(--oj-color-accent-rgb) / 0.8);
    --oj-accent-alpha-20: rgb(var(--oj-color-accent-rgb) / 0.2);
    --oj-accent-alpha-10: rgb(var(--oj-color-accent-rgb) / 0.1);
}
```

### Step 3: 在主题中映射为语义变量

在 `themes/theme.css` 中为深色/浅色主题分别映射：

```css
/* themes/theme.css */

/* 深色主题 */
:root,
html.dark {
    --oj-highlight-bg: var(--oj-accent-alpha-10);
    --oj-highlight-text: var(--oj-color-accent);
    --oj-highlight-border: var(--oj-color-accent);
}

/* 浅色主题 */
html.light {
    --oj-highlight-bg: rgb(139 92 246 / 5%);
    --oj-highlight-text: var(--oj-color-accent-dark);
    --oj-highlight-border: var(--oj-color-accent);
}
```

### Step 4: 在组件中使用

在 Vue 组件或 `utilities/utilities.css` 中使用：

```css
/* utilities/utilities.css */
.oj-highlight {
    padding: var(--oj-spacing-2) var(--oj-spacing-3);
    color: var(--oj-highlight-text);
    background-color: var(--oj-highlight-bg);
    border: 1px solid var(--oj-highlight-border);
    border-radius: var(--oj-radius-md);
}
```

---

## Element Plus 变量映射

### Element Plus → OJ Token 映射表

| Element Plus 变量         | OJ Token                                                  | 说明     |
| ------------------------- | --------------------------------------------------------- | -------- |
| `--el-color-primary`      | `var(--oj-color-primary)`                                 | 主色     |
| `--el-bg-color`           | `var(--oj-bg-light)` (深色) / `#fff` (浅色)               | 背景     |
| `--el-text-color-primary` | `var(--oj-text-primary)` / `var(--oj-text-light-primary)` | 文本     |
| `--el-border-color`       | `var(--oj-slate-700)` (深色) / `#dcdfe6` (浅色)           | 边框     |
| `--el-fill-color-hover`   | `var(--oj-surface-hover)`                                 | 悬停填充 |
| `--el-fill-color-pressed` | `var(--oj-surface-active)`                                | 激活填充 |

### 覆盖 Element Plus 组件样式

**优先级规则**（从高到低）：

1. **变量覆盖**（推荐）: 在 `element/theme.css` 中修改 `--el-*` 变量
2. **伪类选择器**: 仅在变量无法覆盖时使用（需加注释说明原因）
3. **避免 `!important`**: 除非绝对必要

#### 示例：覆盖下拉菜单悬停样式

```css
/* element/theme.css - 深色主题 */
:root,
html.dark {
    /* 推荐：使用变量覆盖 */
    --el-dropdown-menuItem-hover-fill: var(--oj-surface-hover);
    --el-dropdown-menuItem-hover-color: var(--oj-text-color);
}

/* 仅在变量无法覆盖时使用选择器（需注释原因） */
/* html.dark .el-dropdown-menu__item:hover {
    background-color: var(--oj-surface-hover) !important;
} */
```

---

## 最佳实践

### ✅ 推荐做法

```css
/* 1. 使用语义化 Token */
.my-component {
    background-color: var(--oj-surface-base); /* ✅ 语义清晰 */
    color: var(--oj-text-color); /* ✅ 主题自适应 */
}

/* 2. 使用 RGBA 辅助变量 */
.overlay {
    background-color: var(--oj-black-alpha-60); /* ✅ 透明度可控 */
}

/* 3. 使用间距 Token */
.card {
    padding: var(--oj-padding-md); /* ✅ 统一间距 */
    gap: var(--oj-gap-md);
}
```

### ❌ 避免做法

```css
/* 1. 硬编码颜色 */
.my-component {
    background-color: #1e293b; /* ❌ 浅色主题不适配 */
    color: #e2e8f0; /* ❌ 对比度未验证 */
}

/* 2. 硬编码像素值 */
.card {
    padding: 16px; /* ❌ 不符合设计系统 */
    gap: 12px;
}

/* 3. 直接使用 Element Plus 类名 */
.el-button {
    background: #06b6d4 !important; /* ❌ 破坏主题一致性 */
}
```

---

## 常见问题

### Q1: 如何确保对比度符合 WCAG 标准？

**A**: 使用 `tokens/accessibility.css` 中的建议色值：

```css
/* 深色主题 - 已验证对比度 >= 4.5 */
--oj-text-color: var(--oj-a11y-dark-safe-text); /* #e2e8f0 */
--oj-text-color-secondary: var(--oj-a11y-dark-safe-secondary); /* #94a3b8 */
```

### Q2: 如何处理 Element Plus 组件深色模式白色闪烁？

**A**: 确保在 `element/theme.css` 中覆盖了所有悬停状态变量：

```css
:root,
html.dark {
    --el-fill-color-hover: var(--oj-surface-hover); /* 替换默认白色 */
    --el-bg-color-hover: var(--oj-surface-hover);
    --el-dropdown-menuItem-hover-fill: var(--oj-surface-hover);
}
```

### Q3: 如何新增一个自定义表面状态？

**A**: 在 `colors.css` 和 `theme.css` 中同时定义：

```css
/* tokens/colors.css */
:root {
    --oj-surface-disabled: rgb(51 65 85 / 30%);
}

/* themes/theme.css - 深色主题 */
:root,
html.dark {
    --oj-surface-disabled: rgb(51 65 85 / 30%);
}

/* themes/theme.css - 浅色主题 */
html.light {
    --oj-surface-disabled: rgb(241 245 249 / 50%);
}
```

### Q4: 如何测试主题切换效果？

**A**: 在浏览器开发者工具中：

```javascript
// 切换到浅色主题
document.documentElement.classList.remove("dark");
document.documentElement.classList.add("light");

// 切换到深色主题
document.documentElement.classList.remove("light");
document.documentElement.classList.add("dark");
```

---

## 版本历史

- **v1.0** (2025-12-08): 初始版本，完成核心主题系统
- **v1.1** (2025-12-08): 新增语义表面色 Token (`--oj-surface-*`)
- **v1.2** (2025-12-08): 新增可访问性 Token 和 RGBA 辅助变量

---

## 相关文档

- [STYLE_AUDIT.md](./STYLE_AUDIT.md) - 样式审计报告
- [Element Plus 主题定制](https://element-plus.org/zh-CN/guide/theming.html)
- [WCAG 2.1 对比度标准](https://www.w3.org/WAI/WCAG21/Understanding/contrast-minimum.html)

---

**需要帮助？** 请在项目 Issue 中提问或联系前端团队。
