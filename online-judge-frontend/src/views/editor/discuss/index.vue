<script setup lang="ts">
/**
 * 讨论编辑器页面
 *
 * 支持新增与编辑讨论：
 * - insert: 调用 insertDiscuss
 * - update: 调用 selectDiscussByEdit + updateDiscuss
 */
import { computed, onMounted, ref } from "vue";
import { useRoute, useRouter } from "vue-router";
import { ArrowLeft } from "@element-plus/icons-vue";
import { ElMessage, ElMessageBox } from "element-plus";
import { OjMarkdownEditor } from "@/components/common";
import { insertDiscuss, selectDiscussByEdit, updateDiscuss } from "@/api/discussion";
import type { Api } from "@/types/api/api";

defineOptions({ name: "DiscussEditor" });

const route = useRoute();
const router = useRouter();

type EditMode = "insert" | "update";
const editMode = computed<EditMode>(() => {
    return route.query.mode === "update" ? "update" : "insert";
});

const discussId = computed<string | undefined>(() => {
    const id = route.query.id;
    return typeof id === "string" ? id : undefined;
});

const parentId = computed<string>(() => {
    const pid = route.query.parentId;
    return typeof pid === "string" && pid.trim() ? pid.trim() : "0";
});

const title = ref("");
const content = ref("");

const loading = ref(false);
const submitting = ref(false);

const pageTitle = computed(() => (editMode.value === "insert" ? "发布讨论" : "编辑讨论"));

const getAfterSubmitRoute = () => {
    if (route.name === "discussion-write") {
        const q = parentId.value !== "0" ? { ParentId: parentId.value } : undefined;
        return { name: "discussion-list", query: q } as const;
    }

    return { name: "admin-discussion" } as const;
};

const fetchDiscussData = async () => {
    if (!discussId.value) return;

    loading.value = true;
    try {
        const response = await selectDiscussByEdit(discussId.value);
        if (response.data.code === 0 && response.data.data) {
            const data = response.data.data;
            title.value = data.Title || "";
            content.value = data.Content || "";
        } else {
            ElMessage.error(response.data.message || "获取讨论信息失败");
            router.back();
        }
    } catch (err) {
        console.error("获取讨论信息失败:", err);
        ElMessage.error("网络异常，请稍后重试");
        router.back();
    } finally {
        loading.value = false;
    }
};

const validateForm = (): boolean => {
    const t = title.value.trim();
    if (!t) {
        ElMessage.warning("请输入讨论标题");
        return false;
    }
    if (t.length > 50) {
        ElMessage.warning("标题长度不能超过 50 个字符");
        return false;
    }
    const c = content.value.trim();
    if (!c) {
        ElMessage.warning("请输入讨论内容");
        return false;
    }
    if (c.length < 2) {
        ElMessage.warning("内容不能少于 2 个字符");
        return false;
    }
    return true;
};

const handleInsert = async () => {
    const params: Api.Discuss.InsertDiscussParams = {
        Title: title.value.trim(),
        Content: content.value,
        ParentId: parentId.value,
    };

    submitting.value = true;
    try {
        const response = await insertDiscuss(params);
        if (response.data.code === 0) {
            router.push(getAfterSubmitRoute());
        }
    } catch (err) {
        console.error("发布讨论失败:", err);
        ElMessage.error("发布失败，请稍后重试");
    } finally {
        submitting.value = false;
    }
};

const handleUpdate = async () => {
    if (!discussId.value) return;

    const params: Api.Discuss.UpdateDiscussParams = {
        DiscussId: discussId.value,
        Title: title.value.trim(),
        Content: content.value,
    };

    submitting.value = true;
    try {
        const response = await updateDiscuss(params);
        if (response.data.code === 0) {
            router.push(getAfterSubmitRoute());
        }
    } catch (err) {
        console.error("更新讨论失败:", err);
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
    if (editMode.value === "update" && discussId.value) {
        fetchDiscussData();
    }
});
</script>

<template>
    <section class="discuss-editor" aria-label="讨论编辑">
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
                                <span class="label-text">讨论标题</span>
                                <span class="label-required">*</span>
                            </label>
                            <el-input v-model="title" class="title-input" placeholder="请输入讨论标题..." maxlength="50"
                                show-word-limit clearable />
                        </div>

                        <div class="form-item">
                            <label class="form-label">
                                <span class="label-text">讨论内容</span>
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
.discuss-editor {
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
}
</style>
