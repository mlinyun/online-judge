export type OjLanguage = "c" | "cpp" | "go" | "java" | "python2" | "python3" | "javascript" | "typescript";

/**
 * 将 OJ 语言值映射到 Monaco 的 languageId
 */
export const toMonacoLanguageId = (language: string): string => {
    const value = String(language || "")
        .trim()
        .toLowerCase();

    if (value === "c") return "c";
    if (value === "cpp" || value === "c++") return "cpp";
    if (value === "go" || value === "golang") return "go";
    if (value === "java") return "java";

    if (value === "python" || value === "py" || value === "python2" || value === "python3") {
        return "python";
    }

    if (value === "javascript" || value === "js") return "javascript";
    if (value === "typescript" || value === "ts") return "typescript";

    return value || "plaintext";
};
