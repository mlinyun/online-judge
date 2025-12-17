# Element Plus → OJ Token 映射表

> **更新时间**: 2025-12-08  
> **说明**: 记录 Element Plus 组件库变量与 Online Judge 设计 Token 的映射关系

---

## 📊 核心变量映射

### 主题色 (Theme Colors)

| Element Plus         | OJ Token                  | 颜色值 (深色) | 颜色值 (浅色) | 用途             |
| -------------------- | ------------------------- | ------------- | ------------- | ---------------- |
| `--el-color-primary` | `var(--oj-color-primary)` | `#06b6d4`     | `#06b6d4`     | 主强调色（青色） |
| `--el-color-success` | `var(--oj-color-success)` | `#10b981`     | `#10b981`     | 成功状态（绿色） |
| `--el-color-warning` | `var(--oj-color-warning)` | `#f59e0b`     | `#f59e0b`     | 警告状态（橙色） |
| `--el-color-danger`  | `var(--oj-color-danger)`  | `#ef4444`     | `#ef4444`     | 危险状态（红色） |
| `--el-color-info`    | `var(--oj-color-info)`    | `#3b82f6`     | `#3b82f6`     | 信息状态（蓝色） |

---

## 🎨 背景色映射

### 深色主题 (Dark Theme)

| Element Plus                  | OJ Token                | 值        | 说明         |
| ----------------------------- | ----------------------- | --------- | ------------ |
| `--el-bg-color`               | `var(--oj-bg-light)`    | `#1e293b` | 组件基础背景 |
| `--el-bg-color-page`          | `var(--oj-bg-base)`     | `#0b1120` | 页面背景     |
| `--el-bg-color-overlay`       | `var(--oj-bg-light)`    | `#1e293b` | 覆盖层背景   |
| `--el-fill-color`             | `var(--oj-bg-elevated)` | `#334155` | 填充色       |
| `--el-fill-color-light`       | `var(--oj-bg-light)`    | `#1e293b` | 浅填充色     |
| `--el-fill-color-lighter`     | `var(--oj-slate-800)`   | `#1e293b` | 更浅填充色   |
| `--el-fill-color-extra-light` | `var(--oj-slate-900)`   | `#0f172a` | 极浅填充色   |
| `--el-fill-color-dark`        | `var(--oj-slate-600)`   | `#475569` | 深填充色     |
| `--el-fill-color-darker`      | `var(--oj-slate-500)`   | `#64748b` | 更深填充色   |
| `--el-fill-color-blank`       | `transparent`           | -         | 透明填充     |

### 浅色主题 (Light Theme)

| Element Plus                  | OJ Token                  | 值        | 说明         |
| ----------------------------- | ------------------------- | --------- | ------------ |
| `--el-bg-color`               | `#fff`                    | `#ffffff` | 组件基础背景 |
| `--el-bg-color-page`          | `var(--oj-bg-light-base)` | `#f8fafc` | 页面背景     |
| `--el-bg-color-overlay`       | `#fff`                    | `#ffffff` | 覆盖层背景   |
| `--el-fill-color`             | `#f0f2f5`                 | -         | 填充色       |
| `--el-fill-color-light`       | `#f5f7fa`                 | -         | 浅填充色     |
| `--el-fill-color-lighter`     | `#fafafa`                 | -         | 更浅填充色   |
| `--el-fill-color-extra-light` | `#fafcff`                 | -         | 极浅填充色   |
| `--el-fill-color-dark`        | `#ebedf0`                 | -         | 深填充色     |
| `--el-fill-color-darker`      | `#e6e8eb`                 | -         | 更深填充色   |
| `--el-fill-color-blank`       | `#fff`                    | -         | 透明填充     |

---

## 📝 文本色映射

### 深色主题

| Element Plus                  | OJ Token                   | 值        | 对比度  | 说明     |
| ----------------------------- | -------------------------- | --------- | ------- | -------- |
| `--el-text-color-primary`     | `var(--oj-text-primary)`   | `#e2e8f0` | 12.63:1 | 主要文本 |
| `--el-text-color-regular`     | `var(--oj-text-secondary)` | `#94a3b8` | 6.12:1  | 常规文本 |
| `--el-text-color-secondary`   | `var(--oj-text-muted)`     | `#64748b` | 4.51:1  | 次要文本 |
| `--el-text-color-placeholder` | `var(--oj-text-disabled)`  | `#475569` | 3.62:1  | 占位符   |
| `--el-text-color-disabled`    | `var(--oj-text-disabled)`  | `#475569` | 3.62:1  | 禁用文本 |

### 浅色主题

| Element Plus                  | OJ Token                         | 值        | 对比度  | 说明     |
| ----------------------------- | -------------------------------- | --------- | ------- | -------- |
| `--el-text-color-primary`     | `var(--oj-text-light-primary)`   | `#0f172a` | 16.35:1 | 主要文本 |
| `--el-text-color-regular`     | `var(--oj-text-light-secondary)` | `#334155` | 10.16:1 | 常规文本 |
| `--el-text-color-secondary`   | `var(--oj-text-light-muted)`     | `#64748b` | 4.51:1  | 次要文本 |
| `--el-text-color-placeholder` | `var(--oj-text-light-disabled)`  | `#94a3b8` | 4.54:1  | 占位符   |
| `--el-text-color-disabled`    | `var(--oj-text-light-disabled)`  | `#94a3b8` | 4.54:1  | 禁用文本 |

---

## 🔲 边框色映射

### 深色主题

| Element Plus                    | OJ Token              | 值        | 说明     |
| ------------------------------- | --------------------- | --------- | -------- |
| `--el-border-color`             | `var(--oj-slate-700)` | `#334155` | 基础边框 |
| `--el-border-color-light`       | `var(--oj-slate-600)` | `#475569` | 浅边框   |
| `--el-border-color-lighter`     | `var(--oj-slate-700)` | `#334155` | 更浅边框 |
| `--el-border-color-extra-light` | `var(--oj-slate-800)` | `#1e293b` | 极浅边框 |
| `--el-border-color-dark`        | `var(--oj-slate-600)` | `#475569` | 深边框   |
| `--el-border-color-darker`      | `var(--oj-slate-500)` | `#64748b` | 更深边框 |

### 浅色主题

| Element Plus                    | OJ Token  | 值  | 说明     |
| ------------------------------- | --------- | --- | -------- |
| `--el-border-color`             | `#dcdfe6` | -   | 基础边框 |
| `--el-border-color-light`       | `#e4e7ed` | -   | 浅边框   |
| `--el-border-color-lighter`     | `#ebeef5` | -   | 更浅边框 |
| `--el-border-color-extra-light` | `#f2f6fc` | -   | 极浅边框 |
| `--el-border-color-dark`        | `#d4d7de` | -   | 深边框   |
| `--el-border-color-darker`      | `#cdd0d6` | -   | 更深边框 |

---

## 🎯 交互状态映射

### 深色主题

| Element Plus                         | OJ Token                     | 值                     | 说明         |
| ------------------------------------ | ---------------------------- | ---------------------- | ------------ |
| `--el-fill-color-hover`              | `var(--oj-surface-hover)`    | `rgb(51 65 85 / 50%)`  | 悬停填充     |
| `--el-fill-color-pressed`            | `var(--oj-surface-active)`   | `rgb(51 65 85 / 70%)`  | 激活填充     |
| `--el-bg-color-hover`                | `var(--oj-surface-hover)`    | `rgb(51 65 85 / 50%)`  | 悬停背景     |
| `--el-bg-color-overlay-hover`        | `var(--oj-surface-hover)`    | `rgb(51 65 85 / 50%)`  | 覆盖层悬停   |
| `--el-dropdown-menuItem-hover-fill`  | `var(--oj-surface-hover)`    | `rgb(51 65 85 / 50%)`  | 下拉菜单悬停 |
| `--el-select-dropdown-item-hover-bg` | `var(--oj-surface-hover)`    | `rgb(51 65 85 / 50%)`  | 选择项悬停   |
| `--el-menu-item-hover-fill`          | `var(--oj-surface-hover)`    | `rgb(51 65 85 / 50%)`  | 菜单项悬停   |
| `--el-menu-item-active-bg`           | `var(--oj-surface-selected)` | `rgb(6 182 212 / 15%)` | 菜单项激活   |

### 浅色主题

| Element Plus                         | OJ Token                     | 值                       | 说明         |
| ------------------------------------ | ---------------------------- | ------------------------ | ------------ |
| `--el-fill-color-hover`              | `var(--oj-surface-hover)`    | `rgb(241 245 249 / 80%)` | 悬停填充     |
| `--el-fill-color-pressed`            | `var(--oj-surface-active)`   | `rgb(226 232 240 / 90%)` | 激活填充     |
| `--el-bg-color-hover`                | `var(--oj-surface-hover)`    | `rgb(241 245 249 / 80%)` | 悬停背景     |
| `--el-bg-color-overlay-hover`        | `var(--oj-surface-hover)`    | `rgb(241 245 249 / 80%)` | 覆盖层悬停   |
| `--el-dropdown-menuItem-hover-fill`  | `var(--oj-surface-hover)`    | `rgb(241 245 249 / 80%)` | 下拉菜单悬停 |
| `--el-select-dropdown-item-hover-bg` | `var(--oj-surface-hover)`    | `rgb(241 245 249 / 80%)` | 选择项悬停   |
| `--el-menu-item-hover-fill`          | `var(--oj-surface-hover)`    | `rgb(241 245 249 / 80%)` | 菜单项悬停   |
| `--el-menu-item-active-bg`           | `var(--oj-surface-selected)` | `rgb(6 182 212 / 10%)`   | 菜单项激活   |

---

## 💬 组件特定映射

### 按钮 (Button)

| Element Plus                 | OJ Token                            | 深色值    | 浅色值    |
| ---------------------------- | ----------------------------------- | --------- | --------- |
| `--el-button-bg-color`       | `var(--oj-button-primary-bg)`       | `#06b6d4` | `#06b6d4` |
| `--el-button-hover-bg-color` | `var(--oj-button-primary-hover-bg)` | `#0891b2` | `#0891b2` |

### 输入框 (Input)

| Element Plus                   | OJ Token                      | 深色值                | 浅色值    |
| ------------------------------ | ----------------------------- | --------------------- | --------- |
| `--el-input-bg-color`          | `var(--oj-input-bg)`          | `rgb(30 41 59 / 50%)` | `#fff`    |
| `--el-input-border-color`      | `var(--oj-input-border)`      | `#475569`             | `#cbd5e1` |
| `--el-input-text-color`        | `var(--oj-input-text)`        | `#94a3b8`             | `#0f172a` |
| `--el-input-placeholder-color` | `var(--oj-input-placeholder)` | `#64748b`             | `#94a3b8` |

### 表格 (Table)

| Element Plus                    | OJ Token                       | 深色值                | 浅色值    |
| ------------------------------- | ------------------------------ | --------------------- | --------- |
| `--el-table-header-bg-color`    | `var(--oj-table-header-bg)`    | `rgb(30 41 59 / 80%)` | `#f8fafc` |
| `--el-table-row-hover-bg-color` | `var(--oj-table-row-hover-bg)` | `rgb(51 65 85 / 30%)` | `#f1f5f9` |
| `--el-table-border-color`       | `var(--oj-table-border)`       | `rgb(51 65 85 / 50%)` | `#e2e8f0` |

---

## 🔧 字体与尺寸映射

| Element Plus                 | OJ Token                     | 值                        | 说明     |
| ---------------------------- | ---------------------------- | ------------------------- | -------- |
| `--el-font-family`           | `var(--oj-font-family-sans)` | Inter, -apple-system, ... | 字体族   |
| `--el-font-size-extra-large` | `var(--oj-font-size-xl)`     | `1.25rem` (20px)          | 特大字号 |
| `--el-font-size-large`       | `var(--oj-font-size-lg)`     | `1.125rem` (18px)         | 大字号   |
| `--el-font-size-medium`      | `var(--oj-font-size-base)`   | `1rem` (16px)             | 中等字号 |
| `--el-font-size-base`        | `var(--oj-font-size-sm)`     | `0.875rem` (14px)         | 基础字号 |
| `--el-font-size-small`       | `var(--oj-font-size-xs)`     | `0.75rem` (12px)          | 小字号   |

---

## 📐 圆角与过渡映射

| Element Plus                    | OJ Token                | 值               | 说明     |
| ------------------------------- | ----------------------- | ---------------- | -------- |
| `--el-border-radius-base`       | `var(--oj-radius-lg)`   | `0.5rem` (8px)   | 基础圆角 |
| `--el-border-radius-small`      | `var(--oj-radius-md)`   | `0.375rem` (6px) | 小圆角   |
| `--el-border-radius-round`      | `var(--oj-radius-full)` | `9999px`         | 圆形     |
| `--el-border-radius-circle`     | `100%`                  | -                | 完整圆形 |
| `--el-transition-duration`      | `0.3s`                  | -                | 过渡时长 |
| `--el-transition-duration-fast` | `0.2s`                  | -                | 快速过渡 |

---

## ⚠️ 已知问题与解决方案

### 问题 1: 深色模式下拉菜单悬停白色闪烁

**原因**: Element Plus 默认 `--el-fill-color-hover` 在深色模式下可能映射到浅色值

**解决方案**:

```css
:root,
html.dark {
    --el-fill-color-hover: var(--oj-surface-hover);
    --el-dropdown-menuItem-hover-fill: var(--oj-surface-hover);
}
```

### 问题 2: 按钮禁用状态对比度不足

**原因**: 禁用文本色与背景色对比度 < 3:1

**解决方案**:

```css
:root,
html.dark {
    --el-disabled-text-color: var(--oj-text-disabled); /* #475569, 对比度 3.62:1 */
}
```

---

## 📚 参考资源

- [Element Plus 主题变量文档](https://element-plus.org/zh-CN/guide/theming.html)
- [OJ 主题迁移指南](./THEME_MIGRATION.md)
- [OJ 样式审计报告](./STYLE_AUDIT.md)

---

**最后更新**: 2025-12-08  
**维护者**: 前端团队
