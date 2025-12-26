/**
 * v-md-editor 类型声明
 *
 * 由于 @kangc/v-md-editor 没有提供官方类型定义，这里手动声明模块
 */

/* eslint-disable @typescript-eslint/no-explicit-any */

// 轻量版编辑器
declare module "@kangc/v-md-editor" {
    import type { DefineComponent, Plugin } from "vue";

    const VueMarkdownEditor: DefineComponent<any, any, any> & {
        use: (plugin: any, opts?: any) => void;
    } & Plugin;

    export default VueMarkdownEditor;
}

// 进阶版编辑器（基于 CodeMirror）
declare module "@kangc/v-md-editor/lib/codemirror-editor" {
    import type { DefineComponent, Plugin } from "vue";

    const VueMarkdownEditor: DefineComponent<any, any, any> & {
        use: (plugin: any, opts?: any) => void;
        Codemirror: any;
    } & Plugin;

    export default VueMarkdownEditor;
}

// 预览组件
declare module "@kangc/v-md-editor/lib/preview" {
    import type { DefineComponent, Plugin } from "vue";

    const VueMarkdownPreview: DefineComponent<any, any, any> & {
        use: (plugin: any, opts?: any) => void;
    } & Plugin;

    export default VueMarkdownPreview;
}

// GitHub 主题
declare module "@kangc/v-md-editor/lib/theme/github.js" {
    const githubTheme: any;
    export default githubTheme;
}

// 插件
declare module "@kangc/v-md-editor/lib/plugins/tip/index" {
    const createTipPlugin: () => any;
    export default createTipPlugin;
}

declare module "@kangc/v-md-editor/lib/plugins/emoji/index" {
    const createEmojiPlugin: () => any;
    export default createEmojiPlugin;
}

declare module "@kangc/v-md-editor/lib/plugins/katex/cdn" {
    const createKatexPlugin: () => any;
    export default createKatexPlugin;
}

declare module "@kangc/v-md-editor/lib/plugins/copy-code/index" {
    const createCopyCodePlugin: () => any;
    export default createCopyCodePlugin;
}

declare module "@kangc/v-md-editor/lib/plugins/line-number/index" {
    const createLineNumberPlugin: () => any;
    export default createLineNumberPlugin;
}

// CSS 模块声明
declare module "@kangc/v-md-editor/lib/style/base-editor.css";
declare module "@kangc/v-md-editor/lib/style/codemirror-editor.css";
declare module "@kangc/v-md-editor/lib/style/preview.css";
declare module "@kangc/v-md-editor/lib/theme/style/github.css";
declare module "@kangc/v-md-editor/lib/plugins/tip/tip.css";
declare module "@kangc/v-md-editor/lib/plugins/emoji/emoji.css";
declare module "@kangc/v-md-editor/lib/plugins/copy-code/copy-code.css";

// CodeMirror 相关 CSS 模块
declare module "codemirror/lib/codemirror.css";
declare module "codemirror/addon/scroll/simplescrollbars.css";
