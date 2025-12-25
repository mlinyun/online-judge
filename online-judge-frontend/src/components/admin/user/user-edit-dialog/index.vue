<script setup lang="ts">
import { computed, reactive, ref, watch } from "vue";
import { Close, RefreshRight } from "@element-plus/icons-vue";
import type { FormInstance, FormRules } from "element-plus";
import { selectUserUpdateInfo, updateUserInfo } from "@/api/user";
import type { Api } from "@/types/api/api";

defineOptions({ name: "UserEditDialog" });

const props = defineProps<{
    modelValue: boolean;
    userId?: Api.User.UserId;
}>();

const emit = defineEmits<{
    (e: "update:modelValue", value: boolean): void;
    (e: "updated"): void;
}>();

const open = computed({
    get: () => props.modelValue,
    set: (val: boolean) => emit("update:modelValue", val),
});

const loading = ref(false);
const submitting = ref(false);
const loadError = ref("");

type FormModel = {
    Avatar: string;
    NickName: string;
    School: string;
    Major: string;
    PersonalProfile: string;
};

const form = reactive<FormModel>({
    Avatar: "",
    NickName: "",
    School: "",
    Major: "",
    PersonalProfile: "",
});

const rules: FormRules<FormModel> = {
    NickName: [{ required: true, message: "请输入昵称", trigger: "blur" }],
    School: [{ required: true, message: "请输入学校", trigger: "blur" }],
    Major: [{ required: true, message: "请输入专业", trigger: "blur" }],
};

const formRef = ref<FormInstance>();

const requestSeq = ref(0);

const dialogTitle = computed(() => {
    if (form.NickName.trim()) return `编辑用户：${form.NickName.trim()}`;
    if (props.userId) return `编辑用户：${String(props.userId)}`;
    return "编辑用户";
});

const resetForm = () => {
    form.Avatar = "";
    form.NickName = "";
    form.School = "";
    form.Major = "";
    form.PersonalProfile = "";
};

const fetchEditableUser = async () => {
    if (!props.userId) return;

    const seq = ++requestSeq.value;
    loading.value = true;
    loadError.value = "";

    try {
        const response = await selectUserUpdateInfo(props.userId);
        if (seq !== requestSeq.value) return;

        if (response.data.code === 0 && response.data.data) {
            const data = response.data.data;
            form.Avatar = data.Avatar || "";
            form.NickName = data.NickName || "";
            form.School = data.School || "";
            form.Major = data.Major || "";
            form.PersonalProfile = data.PersonalProfile || "";
        } else {
            resetForm();
            loadError.value = response.data.message || "加载失败";
        }
    } catch (err) {
        console.error("获取用户编辑信息失败:", err);
        resetForm();
        loadError.value = "网络异常，请稍后重试";
    } finally {
        if (seq === requestSeq.value) loading.value = false;
    }
};

const handleSubmit = async () => {
    if (!props.userId) {
        ElMessage({ type: "warning", message: "缺少用户 ID", showClose: true });
        return;
    }

    try {
        await formRef.value?.validate?.();
    } catch {
        return;
    }

    submitting.value = true;
    try {
        const NickName = form.NickName.trim();
        const School = form.School.trim();
        const Major = form.Major.trim();
        const Avatar = form.Avatar.trim();

        const payload: Api.User.UpdateUserInfoParams = {
            UserId: props.userId,
            School,
            Major,
            ...(NickName ? { NickName } : {}),
            PersonalProfile: form.PersonalProfile,
            ...(Avatar ? { Avatar } : {}),
        };

        const response = await updateUserInfo(payload);
        if (response.data.code === 0) {
            emit("updated");
            open.value = false;
        }
    } catch (err) {
        console.error("更新用户信息失败:", err);
    } finally {
        submitting.value = false;
    }
};

watch(
    () => [props.modelValue, props.userId] as const,
    ([visible, userId]) => {
        if (!visible) return;
        if (!userId) {
            resetForm();
            loadError.value = "请选择一个用户";
            return;
        }
        fetchEditableUser();
    },
    { immediate: false }
);

watch(
    () => props.modelValue,
    (visible) => {
        if (!visible) {
            loadError.value = "";
            submitting.value = false;
        }
    }
);
</script>

<template>
    <el-dialog
        v-model="open"
        :append-to-body="true"
        :show-close="false"
        width="560px"
        class="user-edit-dialog"
        destroy-on-close
    >
        <template #header>
            <div class="dialog-header">
                <div class="header-title">{{ dialogTitle }}</div>
                <div class="header-actions">
                    <el-button
                        class="icon-btn"
                        circle
                        :icon="RefreshRight"
                        :disabled="loading"
                        @click="fetchEditableUser"
                    />
                    <el-button class="icon-btn" circle :icon="Close" @click="open = false" />
                </div>
            </div>
        </template>

        <div class="dialog-shell">
            <el-skeleton :loading="loading" animated>
                <template #template>
                    <div class="skeleton-block" />
                    <div class="skeleton-block" />
                    <div class="skeleton-block" />
                </template>

                <template #default>
                    <el-empty v-if="loadError" :description="loadError">
                        <el-button type="primary" plain @click="fetchEditableUser">重试加载</el-button>
                    </el-empty>

                    <el-form v-else ref="formRef" :model="form" :rules="rules" label-width="92px">
                        <el-form-item label="昵称" prop="NickName">
                            <el-input v-model="form.NickName" clearable placeholder="请输入昵称" />
                        </el-form-item>

                        <el-form-item label="学校" prop="School">
                            <el-input v-model="form.School" clearable placeholder="请输入学校" />
                        </el-form-item>

                        <el-form-item label="专业" prop="Major">
                            <el-input v-model="form.Major" clearable placeholder="请输入专业" />
                        </el-form-item>

                        <el-form-item label="头像" prop="Avatar">
                            <el-input v-model="form.Avatar" clearable placeholder="头像 URL（可选）" />
                        </el-form-item>

                        <el-form-item label="简介" prop="PersonalProfile">
                            <el-input
                                v-model="form.PersonalProfile"
                                type="textarea"
                                :autosize="{ minRows: 3, maxRows: 6 }"
                                placeholder="个人简介（可选）"
                            />
                        </el-form-item>
                    </el-form>
                </template>
            </el-skeleton>
        </div>

        <template #footer>
            <div class="dialog-footer">
                <el-button @click="open = false">取消</el-button>
                <el-button class="save-btn" type="primary" :loading="submitting" @click="handleSubmit">保存</el-button>
            </div>
        </template>
    </el-dialog>
</template>

<style scoped>
.user-edit-dialog {
    --el-color-primary: rgb(var(--oj-color-primary-rgb));
    --el-dialog-padding-primary: 0;
    --dialog-bg: rgb(15 23 42 / 95%);

    position: relative;
    overflow: hidden;
    background: var(--dialog-bg);
    border: 1px solid rgb(var(--oj-color-primary-rgb) / 20%);
    border-radius: var(--oj-radius-xl);
    box-shadow:
        0 0 0 1px rgb(var(--oj-color-primary-rgb) / 8%) inset,
        0 25px 50px -12px rgb(0 0 0 / 50%),
        0 0 80px rgb(var(--oj-color-primary-rgb) / 8%);
    backdrop-filter: blur(20px);
}

.user-edit-dialog::before {
    position: absolute;
    inset: 0;
    pointer-events: none;
    content: "";
    background:
        repeating-linear-gradient(90deg, rgb(var(--oj-color-primary-rgb) / 3%) 0 1px, transparent 1px 24px),
        repeating-linear-gradient(0deg, rgb(var(--oj-color-primary-rgb) / 3%) 0 1px, transparent 1px 24px);
    border-radius: inherit;
    opacity: 0.6;
}

.user-edit-dialog::after {
    position: absolute;
    top: 0;
    right: 0;
    left: 0;
    height: 120px;
    pointer-events: none;
    content: "";
    background: radial-gradient(
        ellipse 80% 100% at 50% -20%,
        rgb(var(--oj-color-primary-rgb) / 15%) 0%,
        transparent 70%
    );
    border-radius: var(--oj-radius-xl) var(--oj-radius-xl) 0 0;
}

.dialog-header {
    position: relative;
    z-index: 2;
    display: flex;
    gap: var(--oj-spacing-3);
    align-items: center;
    justify-content: space-between;
    padding: var(--oj-spacing-4) var(--oj-spacing-5);
    border-bottom: 1px solid rgb(var(--oj-color-primary-rgb) / 12%);
}

.header-title {
    overflow: hidden;
    text-overflow: ellipsis;
    font-size: 1.05rem;
    font-weight: var(--oj-font-weight-semibold);
    color: var(--oj-text-color);
    white-space: nowrap;
    text-shadow: 0 0 20px rgb(var(--oj-color-primary-rgb) / 30%);
}

.header-actions {
    display: flex;
    flex-shrink: 0;
    gap: var(--oj-spacing-2);
    align-items: center;
}

.icon-btn {
    --el-button-size: 32px;
    --el-button-bg-color: rgb(var(--oj-color-primary-rgb) / 10%);
    --el-button-border-color: rgb(var(--oj-color-primary-rgb) / 20%);
    --el-button-hover-bg-color: rgb(var(--oj-color-primary-rgb) / 18%);
    --el-button-hover-border-color: rgb(var(--oj-color-primary-rgb) / 35%);
    --el-button-active-bg-color: rgb(var(--oj-color-primary-rgb) / 22%);
    --el-button-active-border-color: rgb(var(--oj-color-primary-rgb) / 45%);
    --el-button-text-color: var(--oj-text-color);

    transition: all 0.2s ease;
}

.icon-btn:hover {
    box-shadow: 0 0 12px rgb(var(--oj-color-primary-rgb) / 25%);
}

/* ===== 内容区域 ===== */
.dialog-shell {
    position: relative;
    z-index: 1;
    padding: var(--oj-spacing-5);
}

.dialog-shell :deep(.el-form) {
    position: relative;
    z-index: 1;
}

.dialog-shell :deep(.el-form-item) {
    margin-bottom: var(--oj-spacing-4);
}

.dialog-shell :deep(.el-form-item__label) {
    font-weight: 500;
    color: var(--oj-text-color-muted);
}

.dialog-shell :deep(.el-input__wrapper),
.dialog-shell :deep(.el-textarea__inner) {
    background-color: rgb(var(--oj-color-primary-rgb) / 5%);
    border-radius: var(--oj-radius-lg);
    box-shadow: 0 0 0 1px rgb(var(--oj-color-primary-rgb) / 15%) inset;
    transition: all 0.25s ease;
}

.dialog-shell :deep(.el-input__wrapper:hover),
.dialog-shell :deep(.el-textarea__inner:hover) {
    background-color: rgb(var(--oj-color-primary-rgb) / 8%);
    box-shadow: 0 0 0 1px rgb(var(--oj-color-primary-rgb) / 25%) inset;
}

.dialog-shell :deep(.el-input__wrapper.is-focus),
.dialog-shell :deep(.el-textarea__inner:focus) {
    background-color: rgb(var(--oj-color-primary-rgb) / 6%);
    box-shadow:
        0 0 0 1px rgb(var(--oj-color-primary-rgb) / 40%) inset,
        0 0 0 3px rgb(var(--oj-color-primary-rgb) / 12%),
        0 0 16px rgb(var(--oj-color-primary-rgb) / 15%) !important;
}

.dialog-shell :deep(.el-input__inner),
.dialog-shell :deep(.el-textarea__inner) {
    color: var(--oj-text-color);
}

.dialog-shell :deep(.el-input__inner::placeholder),
.dialog-shell :deep(.el-textarea__inner::placeholder) {
    color: var(--oj-text-color-muted);
    opacity: 0.6;
}

/* ===== Footer 区域 ===== */
.dialog-footer {
    position: relative;
    z-index: 2;
    display: flex;
    gap: var(--oj-spacing-3);
    justify-content: flex-end;
    padding: var(--oj-spacing-4) var(--oj-spacing-5);
    border-top: 1px solid rgb(var(--oj-color-primary-rgb) / 12%);
}

.dialog-footer :deep(.el-button) {
    min-width: 80px;
    border-radius: var(--oj-radius-lg);
    transition: all 0.2s ease;
}

.dialog-footer :deep(.el-button:not(.el-button--primary)) {
    --el-button-bg-color: rgb(var(--oj-color-primary-rgb) / 8%);
    --el-button-border-color: rgb(var(--oj-color-primary-rgb) / 20%);
    --el-button-hover-bg-color: rgb(var(--oj-color-primary-rgb) / 14%);
    --el-button-hover-border-color: rgb(var(--oj-color-primary-rgb) / 30%);
    --el-button-text-color: var(--oj-text-color);
}

.save-btn {
    --el-button-bg-color: var(--oj-button-primary-bg);
    --el-button-border-color: var(--oj-button-primary-bg);
    --el-button-hover-bg-color: var(--oj-button-primary-hover-bg);
    --el-button-hover-border-color: var(--oj-button-primary-hover-bg);
    --el-button-active-bg-color: var(--oj-button-primary-hover-bg);
    --el-button-active-border-color: var(--oj-button-primary-hover-bg);
}

.save-btn:hover {
    box-shadow: 0 0 20px rgb(var(--oj-color-primary-rgb) / 35%);
}

/* ===== Skeleton ===== */
.skeleton-block {
    height: 44px;
    margin-bottom: var(--oj-spacing-3);
    background: rgb(var(--oj-color-primary-rgb) / 8%);
    border-radius: var(--oj-radius-lg);
}

/* ===== 响应式 ===== */
@media (width <=640px) {
    .user-edit-dialog {
        width: 92vw !important;
    }

    .dialog-header,
    .dialog-shell,
    .dialog-footer {
        padding-right: var(--oj-spacing-4);
        padding-left: var(--oj-spacing-4);
    }
}
</style>
