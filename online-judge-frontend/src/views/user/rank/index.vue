<script setup lang="ts">
import RankingBoard from "@/components/RankingBoard.vue";

defineOptions({ name: "UserRank" });
</script>

<template>
    <div class="user-rank-page">
        <RankingBoard />
    </div>
</template>

<style scoped>
.user-rank-page {
    box-sizing: border-box;
    width: 100%;
    min-height: 100vh;
    padding: 20px;

    /* 添加一个基础背景，以便展示玻璃拟态效果 */
    background-image: radial-gradient(circle at 50% 50%, rgb(64 158 255 / 10%) 0%, transparent 50%);
    background-attachment: fixed;
}
</style>
