<template>
    <Card style="width: auto; overflow: hidden">
        <template #header>
            <img alt="" src="/panel.png" />
        </template>
        <template #title>{{ name }}</template>
    </Card>
</template>
<script setup lang="ts">
import Card from 'primevue/card';
import { defineProps } from 'vue';
import { panel } from '@/types';
import { ref } from 'vue';
const props = defineProps({
    panel: Object as () => panel,
});
const name = ref(props.panel?.name ?? '');
</script>
<style>
img {
    width: 100%;
    height: 100%;
}

.p-card-title {
    font-size: 2rem;
    text-align: center;
}

.p-card-content {
    display: none;
}
</style>