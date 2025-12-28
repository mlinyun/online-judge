import type * as Monaco from "monaco-editor/esm/vs/editor/editor.api";

import { ensureMonacoWorkers } from "./worker";
import { configureMonaco } from "./setup";

let monacoPromise: Promise<typeof Monaco> | null = null;

/**
 * 懒加载 Monaco（包含所需的语言贡献 + worker 配置）。
 *
 * 说明：
 * - Monaco 的 ESM 版本需要显式导入 language contribution 才能注册语言。
 * - Vite 需要显式配置 MonacoEnvironment.getWorker。
 */
export const loadMonaco = async (): Promise<typeof Monaco> => {
    if (monacoPromise) return monacoPromise;

    monacoPromise = (async () => {
        ensureMonacoWorkers();

        // Monaco 样式（必须引入，否则编辑器布局/字体等会异常）
        // 注意：monaco-editor 的 ESM 目录不包含 editor.main.css，这里从 min/ 路径引入。
        await import("monaco-editor/min/vs/editor/editor.main.css");

        // 基础 Editor API
        const monaco = await import("monaco-editor/esm/vs/editor/editor.api");

        // 语言：OJ 常用（C/C++/Go/Java/Python/JS/TS）
        await Promise.all([
            import("monaco-editor/esm/vs/basic-languages/cpp/cpp.contribution"),
            import("monaco-editor/esm/vs/basic-languages/go/go.contribution"),
            import("monaco-editor/esm/vs/basic-languages/java/java.contribution"),
            import("monaco-editor/esm/vs/basic-languages/python/python.contribution"),
            // TS/JS 语义服务 + 语言注册
            import("monaco-editor/esm/vs/language/typescript/monaco.contribution"),
            // 这些语言会用到对应 worker（保持 label 一致）
            import("monaco-editor/esm/vs/language/json/monaco.contribution"),
            import("monaco-editor/esm/vs/language/css/monaco.contribution"),
            import("monaco-editor/esm/vs/language/html/monaco.contribution"),
        ]);

        configureMonaco(monaco);
        return monaco;
    })();

    return monacoPromise;
};
