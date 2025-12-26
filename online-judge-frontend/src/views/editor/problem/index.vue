<script setup lang="ts">
/**
 * 题目编辑器页面（管理员）
 *
 * 支持新增/编辑题目，参数结构遵循 Api.Problem.EditProblemParams
 */
import { computed, onMounted, ref } from "vue";
import { useRoute, useRouter } from "vue-router";
import { ArrowLeft, Plus } from "@element-plus/icons-vue";
import { ElMessage, ElMessageBox } from "element-plus";
import { OjMarkdownEditor } from "@/components/common";
import { editProblem, selectProblemInfoByAdmin } from "@/api/problem";
import type { Api } from "@/types/api/api";

defineOptions({ name: "ProblemEditor" });

const route = useRoute();
const router = useRouter();

type EditMode = "insert" | "update";
const editMode = computed<EditMode>(() => (route.query.mode === "update" ? "update" : "insert"));

const problemId = computed<string | undefined>(() => {
    const id = route.query.id;
    return typeof id === "string" ? id : undefined;
});

const title = ref("");
const description = ref("");
const timeLimit = ref<number>(1000);
const memoryLimit = ref<number>(256);

const isSpj = ref(false);
const spj = ref(
    "#include <stdio.h>\n" +
        "\n" +
        "#include <fstream>\n" +
        "using namespace std;\n" +
        "\n" +
        "int main(int argc, char *argv[]) {\n" +
        '    FILE *f_out = fopen(argv[2], "r");   // 测试输出\n' +
        '    FILE *f_user = fopen(argv[3], "r");  // 用户输出\n' +
        "    int ret = 0;                         // AC=0, WA=1\n" +
        "\n" +
        "    int a, b;\n" +
        "    // 获取输入输出\n" +
        '    fscanf(f_out, "%d", &a);\n' +
        '    fscanf(f_user, "%d", &b);\n' +
        "    // 比较答案\n" +
        "    if (a == b) {\n" +
        "        ret = 0;\n" +
        "    } else {\n" +
        "        ret = 1;\n" +
        "    }\n" +
        "\n" +
        "    fclose(f_out);\n" +
        "    fclose(f_user);\n" +
        "\n" +
        "    return ret;  // 返回结果，返回值为0时，答案正确，为1时，答案错误\n" +
        "}"
);

const tagsInput = ref("");

type TestCase = Api.Problem.TestInfo;
const testCases = ref<TestCase[]>([{ Input: "", Output: "" }]);

const loading = ref(false);
const submitting = ref(false);

const pageTitle = computed(() => (editMode.value === "insert" ? "新增题目" : "编辑题目"));

const parseTags = (raw: string): Api.Tag.Tags => {
    const items = raw
        .split(/[，,\s]+/)
        .map((x) => x.trim())
        .filter(Boolean);
    return items as Api.Tag.Tags;
};

const validateForm = (): boolean => {
    const t = title.value.trim();
    if (t.length < 2) {
        ElMessage.warning("题目标题不能少于 2 个字符");
        return false;
    }

    const d = description.value.trim();
    if (d.length < 2) {
        ElMessage.warning("题目描述不能少于 2 个字符");
        return false;
    }

    if (!Number.isFinite(timeLimit.value) || timeLimit.value <= 0) {
        ElMessage.warning("时间限制需为正数（毫秒）");
        return false;
    }

    if (!Number.isFinite(memoryLimit.value) || memoryLimit.value <= 0) {
        ElMessage.warning("内存限制需为正数（MB）");
        return false;
    }

    const tags = parseTags(tagsInput.value);
    if (!tags.length) {
        ElMessage.warning("请至少填写 1 个标签");
        return false;
    }

    if (!testCases.value.length) {
        ElMessage.warning("请至少添加 1 组测试用例");
        return false;
    }

    for (const [i, item] of testCases.value.entries()) {
        const output = (item.Output || "").trim();
        if (!output) {
            ElMessage.warning(`第 ${i + 1} 组测试用例的输出不能为空`);
            return false;
        }
    }

    if (isSpj.value && !spj.value.trim()) {
        ElMessage.warning("已启用 SPJ，请填写特判代码");
        return false;
    }

    if (editMode.value === "update" && !problemId.value) {
        ElMessage.error("缺少题目ID，无法编辑");
        return false;
    }
    return true;
};

const buildPayload = (): Api.Problem.EditProblemParams => {
    const tags = parseTags(tagsInput.value);
    const editType: Api.Problem.EditProblemParams["EditType"] = editMode.value === "update" ? "Update" : "Insert";

    const ProblemData: Api.Problem.ProblemData = {
        ProblemId: (problemId.value || "") as Api.Problem.ProblemId,
        Title: title.value.trim(),
        Description: description.value,
        TimeLimit: timeLimit.value,
        MemoryLimit: memoryLimit.value,
        JudgeNum: testCases.value.length,
        Tags: tags,
        IsSPJ: isSpj.value,
        ...(isSpj.value ? { SPJ: spj.value } : {}),
        TestInfo: testCases.value.map((x) => ({ Input: x.Input, Output: x.Output })),
    };

    return {
        EditType: editType,
        ProblemData,
    };
};

const fetchProblemData = async () => {
    if (!problemId.value) return;

    loading.value = true;
    try {
        const response = await selectProblemInfoByAdmin(problemId.value as Api.Problem.ProblemId);
        if (response.data.code === 0 && response.data.data) {
            const data = response.data.data;
            title.value = data.Title || "";
            description.value = data.Description || "";
            timeLimit.value = Number(data.TimeLimit || 2000);
            memoryLimit.value = Number(data.MemoryLimit || 128);
            isSpj.value = !!data.IsSPJ;
            spj.value = data.SPJ || "";
            tagsInput.value = (data.Tags || []).join(" ");

            const list = (data.TestInfo || []).map((x) => ({
                Input: x.Input || "",
                Output: x.Output || "",
            }));
            testCases.value = list.length ? list : [{ Input: "", Output: "" }];
        } else {
            ElMessage.error(response.data.message || "获取题目信息失败");
            router.back();
        }
    } catch (err) {
        console.error("获取题目信息失败:", err);
        ElMessage.error("网络异常，请稍后重试");
        router.back();
    } finally {
        loading.value = false;
    }
};

const handleAddCase = () => {
    testCases.value.push({ Input: "", Output: "" });
};

const handleRemoveCase = async (index: number) => {
    if (testCases.value.length <= 1) {
        ElMessage.warning("至少保留 1 组测试用例");
        return;
    }
    try {
        await ElMessageBox.confirm("确认删除该测试用例？", "删除确认", {
            type: "warning",
            confirmButtonText: "删除",
            cancelButtonText: "取消",
            confirmButtonClass: "el-button--danger",
        });
    } catch {
        return;
    }
    testCases.value.splice(index, 1);
};

const handleSubmit = async () => {
    if (!validateForm()) return;

    submitting.value = true;
    try {
        const payload = buildPayload();
        const response = await editProblem(payload);
        if (response.data.code === 0) {
            router.push({ name: "admin-problem" });
        }
    } catch (err) {
        console.error("题目编辑失败:", err);
        ElMessage.error("提交失败，请稍后重试");
    } finally {
        submitting.value = false;
    }
};

const handleCancel = async () => {
    const hasDirty =
        !!title.value.trim() ||
        !!description.value.trim() ||
        !!tagsInput.value.trim() ||
        !!spj.value.trim() ||
        testCases.value.some((x) => (x.Input || "").trim() || (x.Output || "").trim());
    if (hasDirty) {
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
    if (editMode.value === "update") {
        void fetchProblemData();
    }
});
</script>

<template>
    <section class="problem-editor" aria-label="题目编辑">
        <header class="editor-header oj-glass-panel">
            <div class="header-left">
                <el-button class="back-btn" text :icon="ArrowLeft" @click="handleBack">返回</el-button>
                <h1 class="page-title">{{ pageTitle }}</h1>
            </div>
            <div class="header-right">
                <el-button class="cancel-btn" @click="handleCancel">取消</el-button>
                <el-button class="submit-btn" type="primary" :loading="submitting" @click="handleSubmit">
                    {{ editMode === "insert" ? "提交" : "保存修改" }}
                </el-button>
            </div>
        </header>

        <div class="editor-main">
            <el-skeleton :loading="loading" animated>
                <template #template>
                    <div class="skeleton-form">
                        <div class="skeleton-input" />
                        <div class="skeleton-editor" />
                        <div class="skeleton-input" />
                    </div>
                </template>

                <template #default>
                    <div class="form-section oj-glass-panel">
                        <div class="form-item">
                            <label class="form-label">
                                <span class="label-text">题目标题</span>
                                <span class="label-required">*</span>
                            </label>
                            <el-input v-model="title" placeholder="标题" clearable maxlength="100" show-word-limit />
                        </div>

                        <div class="form-item">
                            <label class="form-label">
                                <span class="label-text">题目描述</span>
                                <span class="label-required">*</span>
                            </label>
                            <OjMarkdownEditor v-model="description" class="desc-editor" />
                        </div>

                        <div class="grid-row">
                            <div class="form-item">
                                <label class="form-label">
                                    <span class="label-text">时间限制（ms）</span>
                                    <span class="label-required">*</span>
                                </label>
                                <el-input-number v-model="timeLimit" :min="1" :max="600000" controls-position="right" />
                            </div>
                            <div class="form-item">
                                <label class="form-label">
                                    <span class="label-text">内存限制（MB）</span>
                                    <span class="label-required">*</span>
                                </label>
                                <el-input-number
                                    v-model="memoryLimit"
                                    :min="1"
                                    :max="262144"
                                    controls-position="right"
                                />
                            </div>
                        </div>

                        <div class="form-item">
                            <label class="form-label">
                                <span class="label-text">标签</span>
                                <span class="label-required">*</span>
                            </label>
                            <el-input v-model="tagsInput" placeholder="标签（用空格/逗号分隔）" clearable />
                        </div>

                        <div class="form-item-inline">
                            <label class="form-label">
                                <span class="label-text">启用 SPJ</span>
                            </label>
                            <el-switch v-model="isSpj" />
                        </div>

                        <div v-if="isSpj" class="form-item">
                            <label class="form-label">
                                <span class="label-text">SPJ 代码</span>
                                <span class="label-required">*</span>
                            </label>
                            <el-input v-model="spj" type="textarea" :autosize="{ minRows: 10, maxRows: 30 }" />
                        </div>

                        <div class="form-item">
                            <div class="cases-header">
                                <label class="form-label">
                                    <span class="label-text">测试用例</span>
                                    <span class="label-required">*</span>
                                </label>
                                <el-button type="primary" plain :icon="Plus" @click="handleAddCase">新增用例</el-button>
                            </div>

                            <div class="cases-list">
                                <div v-for="(c, idx) in testCases" :key="idx" class="case-item oj-glass-panel">
                                    <div class="case-head">
                                        <div class="case-title">用例 {{ idx + 1 }}</div>
                                        <el-button type="danger" text @click="handleRemoveCase(idx)">删除</el-button>
                                    </div>
                                    <div class="case-grid">
                                        <div class="case-field">
                                            <div class="case-label">输入</div>
                                            <el-input
                                                v-model="c.Input"
                                                type="textarea"
                                                :autosize="{ minRows: 4, maxRows: 12 }"
                                            />
                                        </div>
                                        <div class="case-field">
                                            <div class="case-label">输出</div>
                                            <el-input
                                                v-model="c.Output"
                                                type="textarea"
                                                :autosize="{ minRows: 4, maxRows: 12 }"
                                            />
                                        </div>
                                    </div>
                                </div>
                            </div>
                        </div>
                    </div>
                </template>
            </el-skeleton>
        </div>
    </section>
</template>

<style scoped>
.problem-editor {
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
    gap: var(--oj-spacing-5);
    padding: var(--oj-spacing-5);
    border-radius: var(--oj-radius-xl);
}

.form-item {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-2);
}

.form-item-inline {
    display: flex;
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

.desc-editor {
    width: 100%;
    min-height: 520px;
}

.grid-row {
    display: grid;
    grid-template-columns: repeat(2, minmax(0, 1fr));
    gap: var(--oj-spacing-4);
}

.cases-header {
    display: flex;
    gap: var(--oj-spacing-3);
    align-items: center;
    justify-content: space-between;
}

.cases-list {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-4);
}

.case-item {
    padding: var(--oj-spacing-4);
    border-radius: var(--oj-radius-xl);
}

.case-head {
    display: flex;
    align-items: center;
    justify-content: space-between;
    margin-bottom: var(--oj-spacing-3);
}

.case-title {
    font-weight: var(--oj-font-weight-semibold);
    color: var(--oj-text-color);
}

.case-grid {
    display: grid;
    grid-template-columns: repeat(2, minmax(0, 1fr));
    gap: var(--oj-spacing-4);
}

.case-field {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-2);
}

.case-label {
    font-size: var(--oj-font-size-sm);
    color: var(--oj-text-color-secondary);
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

@media (width <=900px) {
    .grid-row {
        grid-template-columns: 1fr;
    }

    .case-grid {
        grid-template-columns: 1fr;
    }
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
