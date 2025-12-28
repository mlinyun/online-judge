// Monaco Editor 在 monaco-editor/esm 路径下的模块声明。
//
// 说明：
// - Vite/rolldown-vite 运行时可以解析这些 ESM 子路径。
// - 但 TypeScript 的类型解析可能无法从这些子路径拿到 d.ts。
// - 这里提供最小声明，让 `vue-tsc --build` 通过。

declare module "monaco-editor/esm/vs/editor/editor.api" {
    export * from "monaco-editor";
}

// 语言贡献（仅副作用导入，用于注册语言）
// 这里显式列出本项目用到的几个路径，避免不同 TS 版本对通配声明匹配存在差异。
declare module "monaco-editor/esm/vs/basic-languages/cpp/cpp.contribution";
declare module "monaco-editor/esm/vs/basic-languages/go/go.contribution";
declare module "monaco-editor/esm/vs/basic-languages/java/java.contribution";
declare module "monaco-editor/esm/vs/basic-languages/python/python.contribution";

declare module "monaco-editor/esm/vs/language/typescript/monaco.contribution";
declare module "monaco-editor/esm/vs/language/json/monaco.contribution";
declare module "monaco-editor/esm/vs/language/css/monaco.contribution";
declare module "monaco-editor/esm/vs/language/html/monaco.contribution";

// Worker：`?worker` 让 Vite 打包成独立 Worker chunk
declare module "*?worker" {
    const WorkerFactory: {
        new (): Worker;
    };
    export default WorkerFactory;
}
