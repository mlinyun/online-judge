<script setup lang="ts">
/**
 * 判题布局组件
 * 用于判题系统相关页面，简洁的主内容区布局
 */
import { RouterView } from "vue-router";

defineOptions({ name: "JudgeLayout" });
</script>

<template>
    <div class="main-layout">
        <!-- 主内容区域 -->
        <main class="main-content">
            <div class="main-container">
                <RouterView />
            </div>
        </main>
    </div>
</template>

<style scoped>
.main-layout {
    display: flex;
    flex-direction: column;
    min-height: 100vh;
}

.main-content {
    flex: 1;
    min-height: 0;
}

.main-container {
    width: 100%;
    height: 100vh;
    margin-right: auto;
    margin-left: auto;
    overflow: hidden;
}
</style>
