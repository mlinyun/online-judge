<script setup lang="ts">
import { computed } from "vue";
import { PieChart, PriceTag } from "@element-plus/icons-vue";
import type { Api } from "@/types/api/api";

defineOptions({ name: "ProblemSidebar" });

type ProblemRow = Api.Problem.SelectProblemListItem;

interface Props {
    total: number;
    solvedCount: number;
    allTags: string[];
    selectedTags: string[];
    hovered?: ProblemRow | null;
}

const props = withDefaults(defineProps<Props>(), {
    hovered: null,
});

const emit = defineEmits<{
    toggleTag: [tag: string];
}>();

const solvedRate = computed(() => {
    const total = Number(props.total || 0);
    const solved = Number(props.solvedCount || 0);
    if (!total) return 0;
    return Math.max(0, Math.min(1, solved / total));
});

const solvedRateText = computed(() => `${(solvedRate.value * 100).toFixed(1)}%`);

const verdictItems = computed(() => {
    const row = props.hovered;
    if (!row) return [];
    return [
        { k: "AC", v: row.ACNum, cls: "ac" },
        { k: "WA", v: row.WANum, cls: "wa" },
        { k: "CE", v: row.CENum, cls: "ce" },
        { k: "RE", v: row.RENum, cls: "re" },
        { k: "TLE", v: row.TLENum, cls: "tle" },
        { k: "MLE", v: row.MLENum, cls: "mle" },
        { k: "SE", v: row.SENum, cls: "se" },
    ];
});

const isTagSelected = (tag: string) => props.selectedTags.includes(tag);
</script>

<template>
    <aside class="sidebar" aria-label="题库侧边栏">
        <!-- Stats Card -->
        <div class="card oj-glass-panel">
            <div class="card-title">
                <el-icon class="card-ic">
                    <PieChart />
                </el-icon>
                <span>刷题统计</span>
            </div>

            <div class="ring-wrap">
                <div class="ring" :style="{ '--p': `${solvedRate * 100}%` }" aria-label="通过率">
                    <div class="ring-center">
                        <div class="ring-v">{{ solvedRateText }}</div>
                        <div class="ring-k">通过率</div>
                    </div>
                </div>
            </div>

            <div class="stats">
                <div class="stat">
                    <span class="k">已通过</span>
                    <span class="v">{{ solvedCount }}</span>
                </div>
                <div class="stat">
                    <span class="k">题目总数</span>
                    <span class="v">{{ total }}</span>
                </div>
            </div>

            <div class="bar">
                <div class="bar-fill" :style="{ width: `${solvedRate * 100}%` }" />
            </div>
        </div>

        <!-- Tags Cloud -->
        <div class="card oj-glass-panel">
            <div class="card-title">
                <el-icon class="card-ic">
                    <PriceTag />
                </el-icon>
                <span>热门标签</span>
            </div>

            <div class="tags">
                <button v-for="t in allTags" :key="t" type="button" class="tag"
                    :class="{ 'is-active': isTagSelected(t) }" @click="emit('toggleTag', t)">
                    {{ t }}
                </button>
            </div>
        </div>

        <!-- Hovered Problem Verdict -->
        <div class="card oj-glass-panel">
            <div class="card-title">
                <span class="mono">评测分布</span>
                <span class="sub" v-if="hovered">{{ hovered.Title }}</span>
                <span class="sub" v-else>鼠标悬停题目行查看</span>
            </div>

            <div v-if="hovered" class="verdicts">
                <div v-for="it in verdictItems" :key="it.k" class="verdict" :class="it.cls">
                    <span class="vk">{{ it.k }}</span>
                    <span class="vv">{{ it.v }}</span>
                </div>
            </div>

            <el-empty v-else description="" />
        </div>
    </aside>
</template>

<style scoped>
.sidebar {
    display: flex;
    flex-direction: column;
    gap: var(--oj-spacing-5);
}

.card {
    padding: var(--oj-spacing-6);
    border-radius: var(--oj-radius-2xl);
}

.card-title {
    display: flex;
    gap: var(--oj-spacing-2);
    align-items: center;
    margin-bottom: var(--oj-spacing-4);
    font-size: var(--oj-font-size-lg);
    font-weight: var(--oj-font-weight-bold);
    color: var(--oj-text-color);
}

.card-ic {
    color: var(--oj-color-primary);
}

.sub {
    margin-left: auto;
    font-size: var(--oj-font-size-xs);
    font-weight: var(--oj-font-weight-medium);
    color: var(--oj-text-color-muted);
}

.mono {
    font-family: var(--oj-font-family-mono);
}

.ring-wrap {
    display: flex;
    justify-content: center;
    margin-bottom: var(--oj-spacing-4);
}

.ring {
    display: grid;
    place-items: center;
    width: 132px;
    height: 132px;
    background: conic-gradient(var(--oj-color-primary) var(--p), rgb(var(--oj-color-primary-rgb) / 10%) 0);
    border-radius: 999px;
}

.ring-center {
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    width: 96px;
    height: 96px;
    background: var(--oj-glass-bg-dark);
    border: 1px solid var(--oj-glass-border);
    border-radius: 999px;
}

.ring-v {
    font-family: var(--oj-font-family-mono);
    font-size: var(--oj-font-size-lg);
    font-weight: var(--oj-font-weight-bold);
    color: var(--oj-text-color);
}

.ring-k {
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color-muted);
}

.stats {
    display: grid;
    gap: var(--oj-spacing-2);
    margin-bottom: var(--oj-spacing-3);
    font-size: var(--oj-font-size-sm);
}

.stat {
    display: flex;
    justify-content: space-between;
    color: var(--oj-text-color-secondary);
}

.stat .v {
    font-family: var(--oj-font-family-mono);
    color: var(--oj-text-color);
}

.bar {
    height: 6px;
    overflow: hidden;
    background: rgb(var(--oj-color-primary-rgb) / 10%);
    border-radius: 999px;
}

.bar-fill {
    height: 100%;
    background: var(--oj-color-primary);
}

.verdicts {
    display: grid;
    grid-template-columns: repeat(2, minmax(0, 1fr));
    gap: var(--oj-spacing-2);
}

.verdict {
    display: flex;
    justify-content: space-between;
    padding: var(--oj-spacing-2) var(--oj-spacing-3);
    font-size: var(--oj-font-size-sm);
    background: var(--oj-glass-bg-light);
    border: 1px solid var(--oj-glass-border);
    border-radius: var(--oj-radius-lg);
}

.vk {
    font-family: var(--oj-font-family-mono);
    color: var(--oj-text-color-muted);
}

.vv {
    font-family: var(--oj-font-family-mono);
    color: var(--oj-text-color);
}

@media (width <=768px) {
    .sidebar {
        gap: var(--oj-spacing-4);
    }

    .card {
        padding: var(--oj-spacing-4);
    }

    .card-title {
        margin-bottom: var(--oj-spacing-3);
        font-size: var(--oj-font-size-md);
    }

    .ring {
        width: 112px;
        height: 112px;
    }

    .ring-center {
        width: 84px;
        height: 84px;
    }

    .verdicts {
        grid-template-columns: 1fr;
    }
}

.verdict.ac .vv {
    color: var(--oj-color-success);
}

.verdict.wa .vv,
.verdict.re .vv,
.verdict.tle .vv,
.verdict.mle .vv,
.verdict.se .vv {
    color: var(--oj-color-danger);
}

.verdict.ce .vv {
    color: var(--oj-color-warning);
}

.tags {
    display: flex;
    flex-wrap: wrap;
    gap: var(--oj-spacing-2);
}

.tag {
    padding: var(--oj-spacing-1) var(--oj-spacing-3);
    font-size: var(--oj-font-size-xs);
    color: var(--oj-text-color-secondary);
    cursor: pointer;
    background: var(--oj-glass-bg-light);
    border: 1px solid var(--oj-glass-border);
    border-radius: 999px;
    transition:
        background-color 0.15s ease,
        color 0.15s ease,
        border-color 0.15s ease;
}

.tag:hover {
    color: var(--oj-text-color);
    border-color: rgb(var(--oj-color-primary-rgb) / 40%);
}

.tag.is-active {
    color: var(--oj-color-primary);
    background: rgb(var(--oj-color-primary-rgb) / 18%);
    border-color: rgb(var(--oj-color-primary-rgb) / 45%);
}
</style>
