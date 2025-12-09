/** @type {import("stylelint").Config} */
export default {
    extends: [
        "stylelint-config-standard",
        "stylelint-config-recommended-vue",
        "stylelint-config-recess-order",
    ],
    plugins: ["stylelint-order"],
    rules: {
        "no-empty-source": null, // 允许空的样式源文件
        "number-max-precision": 6, // 允许更高精度的数字
        "selector-class-pattern": [
            // 添加如下规则允许多种 class 风格
            "^[A-Za-z0-9:_/\\\\-]+$",
            {
                message: "Expected class selector to match the project's class naming conventions",
            },
        ],
    },
    ignoreFiles: ["**/dist/**", "**/node_modules/**", "**/*.js", "**/*.ts", "**/*d.ts"],
};
