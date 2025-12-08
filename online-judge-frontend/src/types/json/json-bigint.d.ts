/**
 * json-bigint 模块类型声明
 * 用于处理 JavaScript 中 int64 精度丢失问题
 */
declare module "json-bigint" {
    interface JSONBigOptions {
        /** 是否将大整数解析为字符串，默认 false */
        storeAsString?: boolean;
        /** 是否总是将整数解析为 BigInt，默认 false */
        alwaysParseAsBig?: boolean;
        /** 是否使用原生 BigInt，默认 false */
        useNativeBigInt?: boolean;
        /** 数字校验的精度范围 */
        protoAction?: "error" | "ignore" | "preserve";
        /** 构造函数行为 */
        constructorAction?: "error" | "ignore" | "preserve";
    }

    interface JSONBigStatic {
        /**
         * 解析 JSON 字符串
         * @param text JSON 字符串
         * @param reviver 可选的转换函数
         */
        parse(text: string, reviver?: (key: string, value: unknown) => unknown): unknown;

        /**
         * 将值转换为 JSON 字符串
         * @param value 要转换的值
         * @param replacer 可选的替换函数或数组
         * @param space 可选的缩进空格数
         */
        stringify(
            value: unknown,
            replacer?: ((key: string, value: unknown) => unknown) | (string | number)[] | null,
            space?: string | number
        ): string;
    }

    /**
     * 创建带配置的 JSONBig 实例
     */
    function JSONBig(options?: JSONBigOptions): JSONBigStatic;

    const defaultExport: JSONBigStatic & typeof JSONBig;
    export default defaultExport;
}
