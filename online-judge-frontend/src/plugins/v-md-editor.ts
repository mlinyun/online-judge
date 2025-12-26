/**
 * v-md-editor 初始化插件（进阶版 - 基于 CodeMirror）
 *
 * 在 Vue 应用启动时注册 v-md-editor 和相关插件
 * 参考官方文档：https://code-farmer-i.github.io/vue-markdown-editor/zh/examples/codemirror-editor.html
 */
import type { App } from "vue";

// 进阶版编辑器（基于 CodeMirror）
import VueMarkdownEditor from "@kangc/v-md-editor/lib/codemirror-editor";
import "@kangc/v-md-editor/lib/style/codemirror-editor.css";

// 预览组件
import VueMarkdownPreview from "@kangc/v-md-editor/lib/preview";
import "@kangc/v-md-editor/lib/style/preview.css";

// GitHub 主题
import githubTheme from "@kangc/v-md-editor/lib/theme/github.js";
import "@kangc/v-md-editor/lib/theme/style/github.css";

// 代码高亮
import hljs from "highlight.js";

// KaTeX（本地安装，挂载到 window 供 cdn 插件使用）
import katex from "katex";
import "katex/dist/katex.min.css";

// 将 katex 挂载到 window（cdn 插件需要）
(window as unknown as { katex: typeof katex }).katex = katex;

// CodeMirror 编辑器相关资源
import Codemirror from "codemirror";
// mode - 语法模式
import "codemirror/mode/markdown/markdown";
import "codemirror/mode/javascript/javascript";
import "codemirror/mode/css/css";
import "codemirror/mode/htmlmixed/htmlmixed";
import "codemirror/mode/vue/vue";
import "codemirror/mode/clike/clike"; // C/C++/Java
import "codemirror/mode/python/python";
import "codemirror/mode/sql/sql";
import "codemirror/mode/shell/shell";
// edit - 编辑增强
import "codemirror/addon/edit/closebrackets";
import "codemirror/addon/edit/closetag";
import "codemirror/addon/edit/matchbrackets";
// placeholder - 占位符
import "codemirror/addon/display/placeholder";
// active-line - 当前行高亮
import "codemirror/addon/selection/active-line";
// scrollbar - 滚动条
import "codemirror/addon/scroll/simplescrollbars";
import "codemirror/addon/scroll/simplescrollbars.css";
// style - 基础样式
import "codemirror/lib/codemirror.css";

// 插件样式
import "@kangc/v-md-editor/lib/plugins/tip/tip.css";
import "@kangc/v-md-editor/lib/plugins/emoji/emoji.css";
import "@kangc/v-md-editor/lib/plugins/copy-code/copy-code.css";

// 标记是否已初始化
let initialized = false;

/**
 * 获取模块的默认导出（兼容 ESM/CJS）
 */
// eslint-disable-next-line @typescript-eslint/no-explicit-any
function getDefaultExport(module: Record<string, unknown>): any {
    // 尝试多种可能的导出方式
    if (typeof module === "function") return module;
    if (typeof module.default === "function") return module.default;
    const defaultVal = module.default as Record<string, unknown> | undefined;
    if (typeof defaultVal?.default === "function") return defaultVal.default;
    // 如果都不是函数，返回 module 本身
    return module.default || module;
}

/**
 * 异步初始化 v-md-editor（处理 CommonJS 插件兼容性）
 */
async function initVMdEditor(): Promise<void> {
    if (initialized) return;

    // 设置 CodeMirror 实例
    VueMarkdownEditor.Codemirror = Codemirror;

    // 配置主题
    VueMarkdownEditor.use(githubTheme, { Hljs: hljs });
    VueMarkdownPreview.use(githubTheme, { Hljs: hljs });

    // 动态导入插件（处理 CommonJS 模块兼容性）
    const [tipModule, emojiModule, copyCodeModule, katexModule, lineNumberModule] = await Promise.all([
        import("@kangc/v-md-editor/lib/plugins/tip/index"),
        import("@kangc/v-md-editor/lib/plugins/emoji/index"),
        import("@kangc/v-md-editor/lib/plugins/copy-code/index"),
        import("@kangc/v-md-editor/lib/plugins/katex/cdn"),
        import("@kangc/v-md-editor/lib/plugins/line-number/index"),
    ]);

    // 获取插件函数（兼容 ESM/CJS）
    const createTipPlugin = getDefaultExport(tipModule);
    const createEmojiPlugin = getDefaultExport(emojiModule);
    const createCopyCodePlugin = getDefaultExport(copyCodeModule);
    const createKatexPlugin = getDefaultExport(katexModule);
    const createLineNumberPlugin = getDefaultExport(lineNumberModule);

    // 配置编辑器插件
    if (typeof createTipPlugin === "function") {
        VueMarkdownEditor.use(createTipPlugin());
        VueMarkdownPreview.use(createTipPlugin());
    }
    if (typeof createEmojiPlugin === "function") {
        VueMarkdownEditor.use(createEmojiPlugin());
        VueMarkdownPreview.use(createEmojiPlugin());
    }
    if (typeof createCopyCodePlugin === "function") {
        VueMarkdownEditor.use(createCopyCodePlugin());
        VueMarkdownPreview.use(createCopyCodePlugin());
    }
    if (typeof createKatexPlugin === "function") {
        VueMarkdownEditor.use(createKatexPlugin());
        VueMarkdownPreview.use(createKatexPlugin());
    }
    if (typeof createLineNumberPlugin === "function") {
        VueMarkdownEditor.use(createLineNumberPlugin());
        VueMarkdownPreview.use(createLineNumberPlugin());
    }

    initialized = true;
}

/**
 * Vue 插件：安装 v-md-editor
 */
export const vMdEditorPlugin = {
    install: async (app: App): Promise<void> => {
        await initVMdEditor();
        app.use(VueMarkdownEditor);
        app.use(VueMarkdownPreview);
    },
};

/**
 * 异步安装函数（用于 main.ts）
 */
export async function setupVMdEditor(app: App): Promise<void> {
    await initVMdEditor();
    app.use(VueMarkdownEditor);
    app.use(VueMarkdownPreview);
}

export { VueMarkdownEditor, VueMarkdownPreview };
