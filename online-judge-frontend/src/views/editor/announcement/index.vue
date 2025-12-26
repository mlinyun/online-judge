<script setup lang="ts">
/**
 * 公告编辑器页面
 *
 * 支持新增和编辑公告，使用 OjMarkdownEditor 组件
 */
import { computed, onMounted, ref } from "vue";
import { useRoute, useRouter } from "vue-router";
import { ArrowLeft, InfoFilled } from "@element-plus/icons-vue";
import { ElMessage, ElMessageBox } from "element-plus";
import { OjMarkdownEditor } from "@/components/common";
import { insertAnnouncement, selectAnnouncementByEdit, updateAnnouncement } from "@/api/announcement";
import type { Api } from "@/types/api/api";

defineOptions({ name: "AnnouncementEditor" });

const route = useRoute();
const router = useRouter();

// 编辑模式：insert 新增 / update 编辑
type EditMode = "insert" | "update";
const editMode = computed<EditMode>(() => {
    return route.query.mode === "update" ? "update" : "insert";
});

// 公告 ID（编辑模式使用）
const announcementId = computed<string | undefined>(() => {
    const id = route.query.id;
    return typeof id === "string" ? id : undefined;
});

// 表单数据
const title = ref("");
const content = ref("");
const level = ref(1);
const active = ref(true);

// 加载状态
const loading = ref(false);
const submitting = ref(false);

// 编辑器引用
const editorRef = ref<InstanceType<typeof OjMarkdownEditor> | null>(null);

// 页面标题
const pageTitle = computed(() => {
    return editMode.value === "insert" ? "发布公告" : "编辑公告";
});

/**
 * 获取公告数据（编辑模式）
 */
const fetchAnnouncementData = async () => {
    if (!announcementId.value) return;

    loading.value = true;
    try {
        const response = await selectAnnouncementByEdit(announcementId.value);
        if (response.data.code === 0 && response.data.data) {
            const data = response.data.data;
            title.value = data.Title || "";
            content.value = data.Content || "";
            level.value = data.Level || 1;
            active.value = data.Active ?? true;
        } else {
            ElMessage.error(response.data.message || "获取公告信息失败");
            router.back();
        }
    } catch (err) {
        console.error("获取公告信息失败:", err);
        ElMessage.error("网络异常，请稍后重试");
        router.back();
    } finally {
        loading.value = false;
    }
};

/**
 * 表单验证
 */
const validateForm = (): boolean => {
    if (!title.value.trim()) {
        ElMessage.warning("请输入公告标题");
        return false;
    }
    if (title.value.trim().length > 100) {
        ElMessage.warning("标题长度不能超过 100 个字符");
        return false;
    }
    if (!content.value.trim()) {
        ElMessage.warning("请输入公告内容");
        return false;
    }
    if (content.value.trim().length < 2) {
        ElMessage.warning("内容不能少于 2 个字符");
        return false;
    }
    if (level.value < 1 || level.value > 5) {
        ElMessage.warning("公告等级范围为 1-5");
        return false;
    }
    return true;
};

/**
 * 提交公告（新增）
 */
const handleInsert = async () => {
    const params: Api.Announcement.InsertAnnouncementParams = {
        Title: title.value.trim(),
        Content: content.value,
        Level: level.value,
        Active: active.value,
    };

    submitting.value = true;
    try {
        const response = await insertAnnouncement(params);
        if (response.data.code === 0) {
            router.push({ name: "admin-announcement" });
        }
    } catch (err) {
        console.error("发布公告失败:", err);
        ElMessage.error("发布失败，请稍后重试");
    } finally {
        submitting.value = false;
    }
};

/**
 * 提交公告（更新）
 */
const handleUpdate = async () => {
    if (!announcementId.value) return;

    const params: Api.Announcement.UpdateAnnouncementParams = {
        AnnouncementId: announcementId.value,
        Title: title.value.trim(),
        Content: content.value,
        Level: level.value,
        Active: active.value,
    };

    submitting.value = true;
    try {
        const response = await updateAnnouncement(params);
        if (response.data.code === 0) {
            router.push({ name: "admin-announcement" });
        }
    } catch (err) {
        console.error("更新公告失败:", err);
        ElMessage.error("更新失败，请稍后重试");
    } finally {
        submitting.value = false;
    }
};

/**
 * 提交表单
 */
const handleSubmit = async () => {
    if (!validateForm()) return;

    if (editMode.value === "insert") {
        await handleInsert();
    } else {
        await handleUpdate();
    }
};

/**
 * 取消编辑
 */
const handleCancel = async () => {
    // 检查是否有未保存的内容
    if (title.value.trim() || content.value.trim()) {
        try {
            await ElMessageBox.confirm("当前有未保存的内容，确认要离开吗？", "离开确认", {
                type: "warning",
                confirmButtonText: "离开",
                cancelButtonText: "继续编辑",
            });
        } catch {
            return;
        }
    }
    router.back();
};

/**
 * 返回上一页
 */
const handleBack = () => {
    handleCancel();
};

onMounted(() => {
    if (editMode.value === "update" && announcementId.value) {
        fetchAnnouncementData();
    }
});
</script>

<template>
    <section class="announcement-editor" aria-label="公告编辑">
        <!-- 顶部导航栏 -->
        <header class="editor-header oj-glass-panel">
            <div class="header-left">
                <el-button class="back-btn" text :icon="ArrowLeft" @click="handleBack">返回</el-button>
                <h1 class="page-title">{{ pageTitle }}</h1>
            </div>
            <div class="header-right">
                <el-button class="cancel-btn" @click="handleCancel">取消</el-button>
                <el-button class="submit-btn" type="primary" :loading="submitting" @click="handleSubmit">
                    {{ editMode === "insert" ? "发布公告" : "保存修改" }}
                </el-button>
            </div>
        </header>

        <!-- 编辑区域 -->
        <div class="editor-main">
            <el-skeleton :loading="loading" animated>
                <template #template>
                    <div class="skeleton-form">
                        <div class="skeleton-input" />
                        <div class="skeleton-editor" />
                    </div>
                </template>

                <template #default>
                    <!-- 表单区域 -->
                    <div class="form-section oj-glass-panel">
                        <!-- 标题输入 -->
                        <div class="form-item">
                            <label class="form-label">
                                <span class="label-text">公告标题</span>
                                <span class="label-required">*</span>
                            </label>
                            <el-input
                                v-model="title"
                                class="title-input"
                                placeholder="请输入公告标题..."
                                maxlength="100"
                                show-word-limit
                                clearable
                            />
                        </div>

                        <!-- 配置项 -->
                        <div class="form-row">
                            <div class="form-item form-item-inline">
                                <label class="form-label">
                                    <span class="label-text">公告等级</span>
                                    <el-tooltip content="等级越高越靠前显示（1-5）" placement="top">
                                        <el-icon class="label-tip">
                                            <InfoFilled />
                                        </el-icon>
                                    </el-tooltip>
                                </label>
                                <el-input-number
                                    v-model="level"
                                    class="level-input"
                                    :min="1"
                                    :max="5"
                                    controls-position="right"
                                />
                            </div>

                            <div class="form-item form-item-inline">
                                <label class="form-label">
                                    <span class="label-text">立即发布</span>
                                    <el-tooltip content="开启后公告将立即对用户可见" placement="top">
                                        <el-icon class="label-tip">
                                            <InfoFilled />
                                        </el-icon>
                                    </el-tooltip>
                                </label>
                                <el-switch v-model="active" class="active-switch" />
                            </div>
                        </div>

                        <!-- Markdown 编辑器 -->
                        <div class="form-item">
                            <label class="form-label">
                                <span class="label-text">公告内容</span>
                                <span class="label-required">*</span>
                            </label>
                            <OjMarkdownEditor
                                ref="editorRef"
                                v-model="content"
                                height="500px"
                                placeholder="请输入公告内容，支持 Markdown 语法..."
                            />
                        </div>
                    </div>
                </template>
            </el-skeleton>
        </div>
    </section>
</template>

<style scoped>
.announcement-editor {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-6);
    max-width: 1200px;
    padding: var(--oj-spacing-6);
    margin: 0 auto;
}

/* Header */
.editor-header {
    position: sticky;
    top: var(--oj-spacing-4);
    z-index: 100;
    display: flex;
    flex-wrap: wrap;
    gap: var(--oj-spacing-4);
    align-items: center;
    justify-content: space-between;
    padding: var(--oj-spacing-4) var(--oj-spacing-5);
    border: 1px solid var(--oj-glass-border);
    border-radius: var(--oj-radius-xl);
}

.editor-header::before {
    position: absolute;
    inset: 0;
    pointer-events: none;
    content: "";
    background:
        radial-gradient(400px circle at 10% 50%, rgb(var(--oj-color-primary-rgb) / 12%) 0%, transparent 50%),
        radial-gradient(300px circle at 90% 50%, rgb(var(--oj-color-primary-rgb) / 8%) 0%, transparent 50%);
    border-radius: inherit;
    opacity: 0.8;
}

.header-left {
    position: relative;
    z-index: 1;
    display: flex;
    gap: var(--oj-spacing-3);
    align-items: center;
}

.header-right {
    position: relative;
    z-index: 1;
    display: flex;
    gap: var(--oj-spacing-3);
    align-items: center;
}

.back-btn {
    color: var(--oj-text-color-secondary);
    transition: color 0.2s ease;
}

.back-btn:hover {
    color: var(--oj-color-primary);
}

.page-title {
    margin: 0;
    font-size: var(--oj-font-size-xl);
    font-weight: var(--oj-font-weight-semibold);
    color: var(--oj-text-color);
}

.cancel-btn {
    --el-button-bg-color: transparent;
    --el-button-border-color: var(--oj-glass-border);
    --el-button-text-color: var(--oj-text-color-secondary);
    --el-button-hover-bg-color: var(--oj-surface-hover);
    --el-button-hover-border-color: var(--oj-border-hover);
    --el-button-hover-text-color: var(--oj-text-color);
}

.submit-btn {
    --el-color-primary: var(--oj-button-primary-bg);
    --el-button-bg-color: var(--oj-button-primary-bg);
    --el-button-border-color: var(--oj-button-primary-bg);
    --el-button-hover-bg-color: var(--oj-button-primary-hover-bg);
    --el-button-hover-border-color: var(--oj-button-primary-hover-bg);
    --el-button-active-bg-color: var(--oj-button-primary-hover-bg);
    --el-button-active-border-color: var(--oj-button-primary-hover-bg);
}

/* Main Editor Area */
.editor-main {
    flex: 1;
}

.skeleton-form {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-6);
}

.skeleton-input {
    height: 56px;
    background: var(--oj-glass-bg);
    border-radius: var(--oj-radius-lg);
}

.skeleton-editor {
    height: 500px;
    background: var(--oj-glass-bg);
    border-radius: var(--oj-radius-xl);
}

/* Form Section */
.form-section {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-5);
    padding: var(--oj-spacing-6);
    border: 1px solid var(--oj-glass-border);
    border-radius: var(--oj-radius-xl);
}

.form-item {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-2);
}

.form-item-inline {
    flex-direction: row;
    align-items: center;
    justify-content: flex-start;
}

.form-row {
    display: flex;
    flex-wrap: wrap;
    gap: var(--oj-spacing-6);
    align-items: flex-start;
}

.form-label {
    display: flex;
    gap: var(--oj-spacing-1);
    align-items: center;
    font-size: var(--oj-font-size-sm);
    font-weight: var(--oj-font-weight-medium);
    color: var(--oj-text-color-secondary);
}

.label-text {
    color: var(--oj-text-color);
}

.label-required {
    color: var(--oj-color-danger);
}

.label-tip {
    font-size: var(--oj-font-size-sm);
    color: var(--oj-text-color-muted);
    cursor: help;
    transition: color 0.2s ease;
}

.label-tip:hover {
    color: var(--oj-color-primary);
}

/* Title Input */
.title-input {
    --el-input-bg-color: var(--oj-input-bg);
    --el-input-border-color: var(--oj-input-border);
    --el-input-hover-border-color: var(--oj-border-hover);
    --el-input-focus-border-color: var(--oj-input-focus-border);
    --el-input-text-color: var(--oj-input-text);
    --el-input-placeholder-color: var(--oj-input-placeholder);
}

.title-input :deep(.el-input__wrapper) {
    padding: var(--oj-spacing-3) var(--oj-spacing-4);
    font-size: var(--oj-font-size-base);
    background-color: var(--oj-input-bg);
    border-radius: var(--oj-radius-lg);
    box-shadow: 0 0 0 1px var(--oj-input-border) inset;
    transition: all 0.2s ease;
}

.title-input :deep(.el-input__wrapper:hover) {
    background-color: var(--oj-bg-hover);
    box-shadow: 0 0 0 1px var(--oj-border-hover) inset;
}

.title-input :deep(.el-input__wrapper.is-focus) {
    background-color: var(--oj-input-bg);
    box-shadow:
        0 0 0 1px var(--oj-input-focus-border) inset,
        0 0 0 3px var(--oj-color-primary-soft) !important;
}

/* Level Input */
.level-input {
    --el-input-bg-color: var(--oj-input-bg);
    --el-input-border-color: var(--oj-input-border);
    --el-input-hover-border-color: var(--oj-border-hover);
    --el-input-focus-border-color: var(--oj-input-focus-border);
    --el-input-text-color: var(--oj-input-text);

    margin-left: var(--oj-spacing-3);
}

.level-input :deep(.el-input__wrapper) {
    background-color: var(--oj-input-bg);
    border-radius: var(--oj-radius-md);
    box-shadow: 0 0 0 1px var(--oj-input-border) inset;
}

.level-input :deep(.el-input__wrapper:hover) {
    box-shadow: 0 0 0 1px var(--oj-border-hover) inset;
}

.level-input :deep(.el-input__wrapper.is-focus) {
    box-shadow:
        0 0 0 1px var(--oj-input-focus-border) inset,
        0 0 0 3px var(--oj-color-primary-soft) !important;
}

/* Active Switch */
.active-switch {
    --el-switch-on-color: var(--oj-color-primary);
    --el-switch-off-color: var(--oj-text-color-muted);

    margin-left: var(--oj-spacing-3);
}

/* Responsive */
@media (width <=768px) {
    .announcement-editor {
        padding: var(--oj-spacing-4);
    }

    .editor-header {
        position: relative;
        top: 0;
        flex-direction: column;
        gap: var(--oj-spacing-3);
        align-items: stretch;
    }

    .header-left,
    .header-right {
        justify-content: center;
    }

    .page-title {
        font-size: var(--oj-font-size-lg);
    }

    .form-section {
        padding: var(--oj-spacing-4);
    }

    .form-row {
        flex-direction: column;
        gap: var(--oj-spacing-4);
    }
}
</style>
