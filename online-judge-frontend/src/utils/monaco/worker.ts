// Vite 中 Monaco 的 Worker 需要显式声明（否则会在运行时报错：Uncaught Error: Unexpected usage）。
// 这里用 `?worker` 让 Vite/rolldown-vite 自动打包成独立 worker chunk。

import EditorWorker from "monaco-editor/esm/vs/editor/editor.worker?worker";
import JsonWorker from "monaco-editor/esm/vs/language/json/json.worker?worker";
import CssWorker from "monaco-editor/esm/vs/language/css/css.worker?worker";
import HtmlWorker from "monaco-editor/esm/vs/language/html/html.worker?worker";
import TsWorker from "monaco-editor/esm/vs/language/typescript/ts.worker?worker";

export const ensureMonacoWorkers = (): void => {
    const globalAny = globalThis as unknown as {
        MonacoEnvironment?: {
            getWorker?: (workerId: string, label: string) => Worker;
        };
    };

    if (globalAny.MonacoEnvironment?.getWorker) return;

    globalAny.MonacoEnvironment = {
        getWorker(_workerId: string, label: string) {
            if (label === "json") return new JsonWorker();
            if (label === "css" || label === "scss" || label === "less") return new CssWorker();
            if (label === "html" || label === "handlebars" || label === "razor") return new HtmlWorker();
            if (label === "typescript" || label === "javascript") return new TsWorker();
            return new EditorWorker();
        },
    };
};
