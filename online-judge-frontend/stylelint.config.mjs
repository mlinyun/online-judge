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
        "order/properties-order": null, // 禁用属性顺序检查，允许更灵活的样式组织
        "selector-pseudo-class-no-unknown": [
            true,
            {
                ignorePseudoClasses: ["deep"], // 允许 :deep() 伪类选择器
            },
        ],
        "selector-pseudo-element-no-unknown": [
            true,
            {
                ignorePseudoElements: ["v-deep"], // 允许 ::v-deep() 伪元素选择器
            },
        ],
    },
    ignoreFiles: ["**/dist/**", "**/node_modules/**", "**/*.js", "**/*.ts"],
};
