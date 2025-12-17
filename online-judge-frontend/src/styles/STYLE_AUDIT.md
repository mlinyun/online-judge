Online Judge Frontend — Styles Audit

日期: 2025-12-08T09:35:04Z
作者: Copilot CLI (自动生成)

## 概述

对 src/styles 下的样式进行全面审查（tokens、themes、element 覆盖、utilities、reset 等）。目标：确保深色/浅色两套主题在视觉上不冲突、可读性良好、并提供优化建议和优先待办事项供后续迭代。

## 检视的文件

- src/styles/tokens/colors.css
- src/styles/tokens/typography.css
- src/styles/tokens/spacing.css
- src/styles/tokens/breakpoints.css
- src/styles/themes/theme.css
- src/styles/element/theme.css
- src/styles/base/reset.css
- src/styles/utilities/utilities.css
- src/styles/index.css

## 逐文件分析（要点）

1. tokens/colors.css

- 定义了大量语义变量（背景、主题色、语义色、文本色、边框、glass、渐变、难度/状态色等），结构清晰。
- 发现：原先浅色文本变量色值（--oj-text-light-secondary 等）偏浅，已修改为更深的 slate 系列以提升对比度。
- 建议：补充一组明确的 RGB 变量（如 --oj-text-light-secondary-rgb）以便做透明度计算和渐进式样式。

2. tokens/typography.css & tokens/spacing.css

- 字体、字号、行高、间距、radius、z-index 等语义化较完善。
- 建议：确保所有组件使用语义 token（--oj-font-size-base 等）而非硬编码像素，便于主题切换与缩放。

3. themes/theme.css

- 提供 html.dark 与 html.light 两套主题值（--oj-page-bg、--oj-content-bg、文本色、输入/按钮/卡片/滚动条等）。
- 发现冲突点：部分基础样式（reset.css）使用了固定颜色（例如：h1 原先使用 --oj-text-inverse 导致浅色模式标题为白色），已修复为使用 --oj-text-color。
- 建议：明确将所有 "content"、"card"、"glass" 的浅/深两侧变量对齐命名（例如 --oj-card-bg / --oj-card-bg-dark / --oj-card-bg-light），并在注释中标注用途。

4. element/theme.css

- 将 Element Plus 的 CSS 变量映射到 OJ 的 tokens，做了大量覆盖。
- 发现冲突点：Element Plus 默认在某些组件（如下拉/选择项）可能套用自身内置的 hover/active 颜色，导致深色模式下出现白色 hover 背景（已通过覆盖变量并添加特定选择器 html.dark .el-xxx:hover 修复）。
- 建议：尽可能通过变量覆盖（--el-bg-color-hover、--el-fill-color 等）优先控制样式，减少使用高优先级的选择器或 !important；若不得已使用选择器覆盖，限制范围并在注释中写明原因与影响组件。

5. base/reset.css

- 定义全局基本样式（body 背景/文字、滚动条、h1-h6、pre/code、表格与 input/placeholder 等）。
- 发现冲突点：code/pre 初始使用深色固值（--oj-slate-800），导致浅色主题下可读性问题，已改为使用 theme 提供的 --oj-content-bg-soft 与 --oj-text-color。
- 建议：所有与背景/文字相关的属性都应使用主题变量（--oj-...）而非色值或 slate 常量。

6. utilities/utilities.css

- 大量 UI 组件样式基于主题变量：card、btn、tag、input、nav、sidebar 等，语义性强。
- 发现问题：部分组件初始颜色依赖深色专属变量（例如 .oj-tag-default 原为 slate 背景），已改为使用 --oj-content-bg-soft。
- 建议：对常用组件增加 CSS 变量扩展点（如 --oj-btn-primary-text、--oj-tag-default-bg），以便在极端情况下仅覆盖变量而不改写样式。

## 全局冲突与风险清单

- 硬编码色值：少量文件曾直接使用 slate 系列深色值或白色（已逐步替换），剩余应全面 audit。
- 高优先级覆盖：为修复 Element Plus 问题，使用了 html.dark .el-xxx:hover 的选择器和 !important，长期可能造成维护成本与覆盖盲区。
- Text contrast（文字对比）：浅色主题次要/禁用色需要保证 WCAG 对比（建议对比度>=4.5:1 对于一般正文）；已调整部分变量但需要用工具批量检测页面关键页面。
- 变量命名一致性：部分 token 命名混用（--oj-text-secondary vs --oj-text-color-secondary），建议统一并文档化。

## 优化建议（优先级排序）

1. 高优先级：统一语义变量并移除硬编码

- 将所有颜色/背景/边框引用改为使用 tokens（完成率部分已做）。
- 在 tokens 中增加 hover/focus/active 的 semantic tokens（例如 --oj-surface-hover、--oj-surface-active）。

2. 中优先级：减少高权重选择器与 !important 使用

- 将 Element Plus 覆盖尽量映射到 --el- 变量，作为首选策略；仅在组件级别加选择器覆盖且注明原因。

3. 中优先级：对比度与可访问性检测

- 编写脚本或使用在线工具批量检测关键页面色彩对比，列出对比度 < 4.5 的元素。

4. 低优先级：增强可定制性与文档

- 提供一个主题变量映射表（Element Plus → OJ tokens）以便开发者扩展。
- 提供一个 style guide（色板、主要用例、禁忌色）文件。

## 建议的结构调整（短期落地）

- 在 src/styles/tokens 中新增 files：
    - accessibility.css（记录对比度阈值与建议色）
    - rgba-helpers.css（提供基于 RGB 的透明度 token）
- 在 src/styles 目录根放置 STYLE_AUDIT.md（已生成）与 THEME_MIGRATION.md（示例如何新增 token 并应用）

## 待办事项（可导出为 issue）

1.（高）Audit: 找出剩余硬编码色并替换为 token（预计 1 天）2.（高）添加 semantic surface tokens（--oj-surface-\*, --oj-surface-hover 等）并替换现有使用（0.5-1 天）3.（中）创建 Element Plus 变量映射表并移除多余的选择器覆盖（1-2 天）4.（中）运行 WCAG 对比检测，输出低对比元素清单（0.5 天），修复关键页面 5.（中）将样式测试加入 CI（视觉回归或至少构建与 lint）6.（低）撰写样式使用指南与 token 文档（1-2 天）

## 快速修复与回滚策略

- 已做的修复（h1、code/pre、浅色文本、Element Plus hover）可随时回滚（git commit 已提交本地仓库）。
- 若发现覆盖选择器带来副作用，优先将覆盖移动到 element/theme.css 的变量层面，再作为最后手段使用选择器限定覆盖。

## 测试与验收清单

- [ ] 深色模式：主页面、题目页、题库、管理页、用户页的头部、下拉、侧栏、卡片、表格、表单、编辑器视觉检查
- [ ] 浅色模式：同上
- [ ] 对比度检测脚本运行并确认主要正文、按钮文本、导航项对比度 >= 4.5
- [ ] 人工审查交互状态（hover/focus/active）是否一致且无白色突兀块

如果你同意，我可以：

- 把上面的待办逐项创建为 GitHub issue（需要 repo 权限）
- 继续自动化替换剩余硬编码色（我可以生成一个替换补丁并提交）
- 生成 THEME_MIGRATION.md 与 Element Plus → OJ token 的映射表

---

报告结束。若需要我接着生成 issue、补丁或具体代码更改清单，请回复要执行的项。
