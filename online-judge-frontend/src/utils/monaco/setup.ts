import type * as Monaco from "monaco-editor/esm/vs/editor/editor.api";

let configured = false;

type MonacoTypescriptFacade = {
    typescriptDefaults: {
        setEagerModelSync: (enabled: boolean) => void;
        setCompilerOptions: (options: Record<string, unknown>) => void;
    };
    javascriptDefaults: {
        setEagerModelSync: (enabled: boolean) => void;
        setCompilerOptions: (options: Record<string, unknown>) => void;
    };
    ScriptTarget: { ES2020: unknown };
    ModuleKind: { ESNext: unknown };
    ModuleResolutionKind: { NodeJs: unknown };
};

export const configureMonaco = (monaco: typeof Monaco): void => {
    if (configured) return;
    configured = true;

    // TypeScript / JavaScript 智能提示与语义分析配置
    // 注意：这些 API 在运行时由 typescript 的 contribution 注入。
    // 由于本项目使用动态 import 加载 contribution，TS 类型层面无法可靠感知增强后的字段。
    const ts = (monaco.languages as unknown as { typescript?: MonacoTypescriptFacade }).typescript;
    if (!ts?.typescriptDefaults || !ts?.javascriptDefaults) {
        // typescript contribution 可能尚未完成注入；不要在这里抛错阻断页面。
        return;
    }

    ts.typescriptDefaults.setEagerModelSync(true);
    ts.javascriptDefaults.setEagerModelSync(true);

    const tsTarget = ts.ScriptTarget?.ES2020;
    const jsTarget = ts.ScriptTarget?.ES2020;
    const moduleKind = ts.ModuleKind?.ESNext;
    const moduleResolution = ts.ModuleResolutionKind?.NodeJs;

    ts.typescriptDefaults.setCompilerOptions({
        ...(tsTarget ? { target: tsTarget } : {}),
        ...(moduleKind ? { module: moduleKind } : {}),
        ...(moduleResolution ? { moduleResolution } : {}),
        allowNonTsExtensions: true,
        strict: true,
        noEmit: true,
        esModuleInterop: true,
        skipLibCheck: true,
        typeRoots: ["node_modules/@types"],
    });

    ts.javascriptDefaults.setCompilerOptions({
        ...(jsTarget ? { target: jsTarget } : {}),
        allowNonTsExtensions: true,
        noEmit: true,
        checkJs: false,
    });
};
