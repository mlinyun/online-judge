/** @type {import("stylelint").Config} */
export default {
    extends: [
        "stylelint-config-standard",
        "stylelint-config-recommended-vue",
        "stylelint-config-recess-order",
    ],
    plugins: ["stylelint-order"],
    rules: {
        // Add overrides and additional rules here
    },
    ignoreFiles: ["**/dist/**", "**/node_modules/**", "**/*.js", "**/*.ts"],
};
