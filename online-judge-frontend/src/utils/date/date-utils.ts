/**
 * 日期工具类
 */
export class DateUtils {
    /**
     * 格式化日期时间（完整日期 + 完整时间）
     * - 输入非法日期字符串时，返回原字符串
     */
    static formatDateTime(dateString: string, locale: string = "zh-CN"): string {
        const date = new Date(dateString);
        if (Number.isNaN(date.getTime())) return dateString;

        return date.toLocaleString(locale, {
            year: "numeric",
            month: "2-digit",
            day: "2-digit",
            hour: "2-digit",
            minute: "2-digit",
            second: "2-digit",
        });
    }
}
