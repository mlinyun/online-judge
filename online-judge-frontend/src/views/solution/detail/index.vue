<script setup lang="ts">
import { useRoute } from "vue-router";
defineOptions({ name: "SolutionDetail" });

const route = useRoute();
const solutionId = route.params.id as string;
</script>

<template>
    <div class="solution-detail">
        <h1>SolutionDetail</h1>
        <p>题解 ID: {{ solutionId }}</p>
    </div>
</template>

<style scoped></style>
