/**
 * 主题切换组合式函数
 * 支持深色/浅色/系统自动三种模式
 * 使用 Pinia 进行状态管理和持久化
 */
import { onMounted, watch } from "vue";
import { storeToRefs } from "pinia";
import { useSettingStore } from "@/stores/modules/setting";

/**
 * 应用主题到 DOM
 */
function applyTheme(theme: "dark" | "light") {
    const root = document.documentElement;

    if (theme === "dark") {
        root.classList.remove("light");
        root.classList.add("dark");
    } else {
        root.classList.remove("dark");
        root.classList.add("light");
    }
}

/**
 * 初始化主题
 */
function initTheme() {
    const settingStore = useSettingStore();

    // 应用初始主题
    applyTheme(settingStore.effectiveTheme);

    // 监听主题变化并应用到 DOM
    watch(
        () => settingStore.effectiveTheme,
        (newTheme) => {
            applyTheme(newTheme);
        }
    );

    // 监听系统主题变化
    if (typeof window !== "undefined") {
        const mediaQuery = window.matchMedia("(prefers-color-scheme: dark)");
        mediaQuery.addEventListener("change", () => {
            // 当主题模式为 system 时，effectiveTheme 会自动响应系统变化
            if (settingStore.themeMode === "system") {
                applyTheme(settingStore.effectiveTheme);
            }
        });
    }
}

/**
 * 主题切换组合式函数
 */
export function useTheme() {
    const settingStore = useSettingStore();

    // 使用 storeToRefs 保持响应式状态
    const { themeMode, effectiveTheme, isDark } = storeToRefs(settingStore);

    onMounted(() => {
        initTheme();
    });

    return {
        /** 当前主题模式 (dark/light/system) */
        themeMode,
        /** 当前实际生效的主题 */
        effectiveTheme,
        /** 当前是否为深色主题 */
        isDark,
        /** 设置主题模式 */
        setTheme: settingStore.setTheme,
        /** 切换主题 (dark <-> light) */
        toggleTheme: settingStore.toggleTheme,
        /** 初始化主题 (在 App 启动时调用) */
        initTheme,
    };
}

export default useTheme;
