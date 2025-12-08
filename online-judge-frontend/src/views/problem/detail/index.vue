<script setup lang="ts">
import { useRoute } from "vue-router";
defineOptions({ name: "ProblemDetail" });

const route = useRoute();
const problemId = route.params.id as string;
</script>

<template>
    <div class="problem-detail">
        <h1>ProblemDetail</h1>
        <p>题目 ID: {{ problemId }}</p>
    </div>
</template>

<style scoped></style>
