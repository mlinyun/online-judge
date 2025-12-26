<script setup lang="ts">
/**
 * 题解编辑器页面
 *
 * 支持新增与编辑题解：
 * - insert: 调用 insertSolution（需要 ParentId）
 * - update: 调用 selectSolutionByEdit + updateSolution
 */
import { computed, onMounted, ref } from "vue";
import { useRoute, useRouter } from "vue-router";
import { ArrowLeft } from "@element-plus/icons-vue";
import { ElMessage, ElMessageBox } from "element-plus";
import { OjMarkdownEditor } from "@/components/common";
import { insertSolution, selectSolutionByEdit, updateSolution } from "@/api/solution";
import type { Api } from "@/types/api/api";

defineOptions({ name: "SolutionEditor" });

const route = useRoute();
const router = useRouter();

type EditMode = "insert" | "update";
const editMode = computed<EditMode>(() => {
    return route.query.mode === "update" ? "update" : "insert";
});

const solutionId = computed<string | undefined>(() => {
    const id = route.query.id;
    return typeof id === "string" ? id : undefined;
});

const parentId = computed<string | undefined>(() => {
    const pid = route.query.parentId;
    if (typeof pid !== "string") return undefined;
    const trimmed = pid.trim();
    return trimmed ? trimmed : undefined;
});

const title = ref("");
const content = ref("");
const isPublic = ref(true);

const loading = ref(false);
const submitting = ref(false);

const pageTitle = computed(() => (editMode.value === "insert" ? "发布题解" : "编辑题解"));

const fetchSolutionData = async () => {
    if (!solutionId.value) return;

    loading.value = true;
    try {
        const response = await selectSolutionByEdit(solutionId.value);
        if (response.data.code === 0 && response.data.data) {
            const data = response.data.data;
            title.value = data.Title || "";
            content.value = data.Content || "";
            isPublic.value = !!data.Public;
        } else {
            ElMessage.error(response.data.message || "获取题解信息失败");
            router.back();
        }
    } catch (err) {
        console.error("获取题解信息失败:", err);
        ElMessage.error("网络异常，请稍后重试");
        router.back();
    } finally {
        loading.value = false;
    }
};

const validateForm = (): boolean => {
    const t = title.value.trim();
    if (!t) {
        ElMessage.warning("请输入题解标题");
        return false;
    }
    if (t.length > 50) {
        ElMessage.warning("标题长度不能超过 50 个字符");
        return false;
    }

    const c = content.value.trim();
    if (!c) {
        ElMessage.warning("请输入题解内容");
        return false;
    }
    if (c.length < 2) {
        ElMessage.warning("内容不能少于 2 个字符");
        return false;
    }

    if (editMode.value === "insert" && !parentId.value) {
        ElMessage.warning("缺少题目ID（ParentId），无法发布题解");
        return false;
    }

    return true;
};

const handleInsert = async () => {
    if (!parentId.value) return;

    const params: Api.Solution.InsertSolutionParams = {
        Title: title.value.trim(),
        Content: content.value,
        ParentId: parentId.value as Api.Problem.ProblemId,
        Public: isPublic.value,
    };

    submitting.value = true;
    try {
        const response = await insertSolution(params);
        if (response.data.code === 0) {
            router.push({ name: "admin-solution" });
        }
    } catch (err) {
        console.error("发布题解失败:", err);
        ElMessage.error("发布失败，请稍后重试");
    } finally {
        submitting.value = false;
    }
};

const handleUpdate = async () => {
    if (!solutionId.value) return;

    const params: Api.Solution.UpdateSolutionParams = {
        SolutionId: solutionId.value,
        Title: title.value.trim(),
        Content: content.value,
        Public: isPublic.value,
    };

    submitting.value = true;
    try {
        const response = await updateSolution(params);
        if (response.data.code === 0) {
            router.push({ name: "admin-solution" });
        }
    } catch (err) {
        console.error("更新题解失败:", err);
        ElMessage.error("更新失败，请稍后重试");
    } finally {
        submitting.value = false;
    }
};

const handleSubmit = async () => {
    if (!validateForm()) return;
    if (editMode.value === "insert") {
        await handleInsert();
    } else {
        await handleUpdate();
    }
};

const handleCancel = async () => {
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

const handleBack = () => {
    handleCancel();
};

onMounted(() => {
    if (editMode.value === "update" && solutionId.value) {
        fetchSolutionData();
    }
});
</script>

<template>
    <section class="solution-editor" aria-label="题解编辑">
        <header class="editor-header oj-glass-panel">
            <div class="header-left">
                <el-button class="back-btn" text :icon="ArrowLeft" @click="handleBack">返回</el-button>
                <h1 class="page-title">{{ pageTitle }}</h1>
            </div>
            <div class="header-right">
                <el-button class="cancel-btn" @click="handleCancel">取消</el-button>
                <el-button class="submit-btn" type="primary" :loading="submitting" @click="handleSubmit">
                    {{ editMode === "insert" ? "发布" : "保存修改" }}
                </el-button>
            </div>
        </header>

        <div class="editor-main">
            <el-skeleton :loading="loading" animated>
                <template #template>
                    <div class="skeleton-form">
                        <div class="skeleton-input" />
                        <div class="skeleton-editor" />
                    </div>
                </template>

                <template #default>
                    <div class="form-section oj-glass-panel">
                        <div class="form-item">
                            <label class="form-label">
                                <span class="label-text">题解标题</span>
                                <span class="label-required">*</span>
                            </label>
                            <el-input
                                v-model="title"
                                class="title-input"
                                placeholder="请输入题解标题..."
                                maxlength="50"
                                show-word-limit
                                clearable
                            />
                        </div>

                        <div class="form-item form-item-inline">
                            <label class="form-label">
                                <span class="label-text">是否公开</span>
                            </label>
                            <el-switch v-model="isPublic" />
                        </div>

                        <div class="form-item">
                            <label class="form-label">
                                <span class="label-text">题解内容</span>
                                <span class="label-required">*</span>
                            </label>
                            <OjMarkdownEditor v-model="content" class="content-editor" />
                        </div>
                    </div>
                </template>
            </el-skeleton>
        </div>
    </section>
</template>

<style scoped>
.solution-editor {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-6);
    padding: var(--oj-spacing-4);
}

.editor-header {
    display: flex;
    gap: var(--oj-spacing-4);
    align-items: center;
    justify-content: space-between;
    padding: var(--oj-spacing-4);
    border-radius: var(--oj-radius-xl);
}

.header-left {
    display: flex;
    gap: var(--oj-spacing-3);
    align-items: center;
    min-width: 0;
}

.page-title {
    margin: 0;
    font-size: var(--oj-font-size-lg);
    font-weight: var(--oj-font-weight-semibold);
    color: var(--oj-text-color);
    white-space: nowrap;
}

.header-right {
    display: flex;
    flex-shrink: 0;
    gap: var(--oj-spacing-2);
}

.editor-main {
    width: 100%;
}

.form-section {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-4);
    padding: var(--oj-spacing-5);
    border-radius: var(--oj-radius-xl);
}

.form-item {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-2);
}

.form-item-inline {
    flex-direction: row;
    gap: var(--oj-spacing-3);
    align-items: center;
    justify-content: flex-start;
}

.form-label {
    display: flex;
    gap: var(--oj-spacing-2);
    align-items: center;
    font-weight: var(--oj-font-weight-semibold);
    color: var(--oj-text-color);
}

.label-required {
    color: var(--el-color-danger);
}

.title-input {
    width: 100%;
}

.content-editor {
    width: 100%;
    min-height: 520px;
}

.skeleton-form {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-4);
}

.skeleton-input {
    height: 44px;
    border-radius: var(--oj-radius-lg);
}

.skeleton-editor {
    height: 520px;
    border-radius: var(--oj-radius-lg);
}

@media (width <=640px) {
    .editor-header {
        flex-direction: column;
        align-items: stretch;
    }

    .header-right {
        justify-content: flex-end;
    }

    .form-item-inline {
        justify-content: space-between;
    }
}
</style>
