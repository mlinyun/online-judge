/**
 * 主题切换组合式函数
 * 支持深色/浅色/系统自动三种模式
 */
import { ref, onMounted } from "vue";

export type ThemeMode = "dark" | "light" | "system";

const THEME_STORAGE_KEY = "oj-theme-mode";

// 全局主题状态
const themeMode = ref<ThemeMode>("dark");
const isDark = ref(true);

/**
 * 获取系统偏好的主题
 */
function getSystemTheme(): "dark" | "light" {
    if (typeof window === "undefined") return "dark";
    return window.matchMedia("(prefers-color-scheme: dark)").matches ? "dark" : "light";
}

/**
 * 应用主题到 DOM
 */
function applyTheme(theme: "dark" | "light") {
    const root = document.documentElement;

    if (theme === "dark") {
        root.classList.remove("light");
        root.classList.add("dark");
        isDark.value = true;
    } else {
        root.classList.remove("dark");
        root.classList.add("light");
        isDark.value = false;
    }
}

/**
 * 更新主题
 */
function updateTheme(mode: ThemeMode) {
    themeMode.value = mode;

    // 保存到 localStorage
    localStorage.setItem(THEME_STORAGE_KEY, mode);

    // 应用主题
    const effectiveTheme = mode === "system" ? getSystemTheme() : mode;
    applyTheme(effectiveTheme);
}

/**
 * 切换主题 (在 dark 和 light 之间切换)
 */
function toggleTheme() {
    const newMode = isDark.value ? "light" : "dark";
    updateTheme(newMode);
}

/**
 * 初始化主题
 */
function initTheme() {
    // 从 localStorage 读取保存的主题
    const savedMode = localStorage.getItem(THEME_STORAGE_KEY) as ThemeMode | null;

    if (savedMode && ["dark", "light", "system"].includes(savedMode)) {
        themeMode.value = savedMode;
    } else {
        // 默认使用深色主题 (Cyber 风格)
        themeMode.value = "dark";
    }

    // 应用主题
    const effectiveTheme = themeMode.value === "system" ? getSystemTheme() : themeMode.value;
    applyTheme(effectiveTheme);

    // 监听系统主题变化
    if (typeof window !== "undefined") {
        const mediaQuery = window.matchMedia("(prefers-color-scheme: dark)");
        mediaQuery.addEventListener("change", (e) => {
            if (themeMode.value === "system") {
                applyTheme(e.matches ? "dark" : "light");
            }
        });
    }
}

/**
 * 主题切换组合式函数
 */
export function useTheme() {
    onMounted(() => {
        initTheme();
    });

    return {
        /** 当前主题模式 (dark/light/system) */
        themeMode,
        /** 当前是否为深色主题 */
        isDark,
        /** 设置主题模式 */
        setTheme: updateTheme,
        /** 切换主题 (dark <-> light) */
        toggleTheme,
        /** 初始化主题 (在 App 启动时调用) */
        initTheme,
    };
}

export default useTheme;
