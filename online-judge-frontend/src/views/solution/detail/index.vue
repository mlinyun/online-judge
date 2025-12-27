<script setup lang="ts">
import { computed, onMounted, ref, watch } from "vue";
import { useRoute, useRouter } from "vue-router";
import { ArrowLeft, Calendar, ChatRound, View } from "@element-plus/icons-vue";

import { OjComment, OjMarkdownPreview } from "@/components/common";
import { selectSolution } from "@/api/solution";
import { DateUtils } from "@/utils/date/date-utils";
import type { Api } from "@/types/api/api";

defineOptions({ name: "SolutionDetail" });

const route = useRoute();
const router = useRouter();

const solutionId = computed(() => String(route.params.id || ""));

const loading = ref(false);
const errorMessage = ref<string>("");
const detail = ref<Api.Solution.SelectSolutionResult | null>(null);

const author = computed<Api.User.SimpleUserInfo | undefined>(() => {
    return detail.value?.User ?? undefined;
});

const fetchDetail = async () => {
    if (!solutionId.value) return;

    loading.value = true;
    errorMessage.value = "";
    try {
        const response = await selectSolution(solutionId.value as Api.Solution.SolutionId);
        if (response.data.code === 0 && response.data.data) {
            detail.value = response.data.data;
        } else {
            detail.value = null;
            errorMessage.value = response.data.message || "加载失败";
        }
    } catch (err) {
        console.error("获取题解详情失败:", err);
        detail.value = null;
        errorMessage.value = "网络异常，请稍后重试";
    } finally {
        loading.value = false;
    }
};

const handleBack = () => {
    router.back();
};

watch(solutionId, () => {
    void fetchDetail();
});

onMounted(() => {
    void fetchDetail();
});
</script>

<template>
    <section class="solution-detail-page" aria-label="题解详情">
        <header class="detail-header oj-glass-panel">
            <div class="header-left">
                <el-button class="back-btn" text :icon="ArrowLeft" @click="handleBack">返回</el-button>
                <el-avatar v-if="author?.Avatar" class="author-avatar" :size="36" :src="author.Avatar" />
                <h1 class="page-title">{{ detail?.Title || "题解详情" }}</h1>
            </div>
        </header>

        <div class="detail-body oj-glass-panel">
            <el-skeleton :loading="loading" animated>
                <template #template>
                    <div class="skeleton-block" />
                    <div class="skeleton-block" />
                    <div class="skeleton-block" />
                </template>

                <template #default>
                    <el-result v-if="errorMessage" icon="error" title="加载失败" :sub-title="errorMessage">
                        <template #extra>
                            <el-button type="primary" @click="fetchDetail">重试</el-button>
                        </template>
                    </el-result>

                    <template v-else-if="detail">
                        <div class="meta-row">
                            <div class="meta-item" v-if="author?.NickName">
                                <span class="meta-label">作者：</span>
                                <span class="meta-value">{{ author.NickName }}</span>
                            </div>

                            <div class="meta-item">
                                <el-icon class="meta-icon">
                                    <Calendar />
                                </el-icon>
                                <span>发布：{{ DateUtils.formatDateTime(detail.CreateTime) }}</span>
                            </div>
                            <div class="meta-item">
                                <el-icon class="meta-icon">
                                    <Calendar />
                                </el-icon>
                                <span>更新：{{ DateUtils.formatDateTime(detail.UpdateTime) }}</span>
                            </div>

                            <div class="meta-spacer" />

                            <div class="meta-item meta-stat">
                                <el-icon class="meta-icon">
                                    <View />
                                </el-icon>
                                <span>{{ detail.Views }}</span>
                            </div>
                            <div class="meta-item meta-stat">
                                <el-icon class="meta-icon">
                                    <ChatRound />
                                </el-icon>
                                <span>{{ detail.Comments }}</span>
                            </div>
                        </div>

                        <OjMarkdownPreview class="content" :content="detail.Content" />
                    </template>

                    <el-empty v-else description="题解不存在或已被删除" />
                </template>
            </el-skeleton>
        </div>

        <div class="comment-placeholder oj-glass-panel" aria-label="评论区">
            <OjComment v-if="solutionId" :parent-id="solutionId" parent-type="Solution" />
        </div>
    </section>
</template>

<style scoped>
.solution-detail-page {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-6);
    width: 100%;
    max-width: var(--oj-container-xl);
    padding: var(--oj-spacing-4);
    margin: 0 auto;
}

.detail-header {
    display: flex;
    align-items: center;
    justify-content: space-between;
    padding: var(--oj-spacing-4);
    margin-top: var(--oj-spacing-8);
    border-radius: var(--oj-radius-2xl);
}

.header-left {
    display: flex;
    gap: var(--oj-spacing-3);
    align-items: center;
    min-width: 0;
}

.author-avatar {
    flex: 0 0 auto;
    border: 1px solid var(--oj-glass-border);
}

.page-title {
    margin: 0;
    overflow: hidden;
    text-overflow: ellipsis;
    font-size: var(--oj-font-size-xl);
    font-weight: var(--oj-font-weight-semibold);
    color: var(--oj-text-color);
    white-space: nowrap;
}

.detail-body {
    padding: var(--oj-spacing-6);
    border-radius: var(--oj-radius-2xl);
}

.meta-row {
    display: flex;
    flex-wrap: wrap;
    gap: var(--oj-spacing-3) var(--oj-spacing-5);
    align-items: center;
    margin-bottom: var(--oj-spacing-5);
    font-size: var(--oj-font-size-sm);
    color: var(--oj-text-color-secondary);
}

.meta-item {
    display: inline-flex;
    gap: var(--oj-spacing-2);
    align-items: center;
}

.meta-label {
    color: var(--oj-text-color-secondary);
}

.meta-value {
    color: var(--oj-text-color);
}

.meta-icon {
    color: var(--oj-color-primary);
}

.meta-spacer {
    flex: 1 1 auto;
}

.meta-stat {
    font-variant-numeric: tabular-nums;
}

.content {
    margin-top: var(--oj-spacing-2);
}

.comment-placeholder {
    padding: var(--oj-spacing-6);
    border-radius: var(--oj-radius-2xl);
}

.skeleton-block {
    height: 24px;
    margin: var(--oj-spacing-3) 0;
    border-radius: var(--oj-radius-lg);
}

@media (width >= 640px) {
    .solution-detail-page {
        padding: var(--oj-spacing-6);
    }
}

@media (width >= 1024px) {
    .solution-detail-page {
        padding: var(--oj-spacing-8);
    }
}
</style>
