<template>
    <div class="panels-grid">
        <Panel v-for="panel in panels" :panel="panel" @click="togglePanelDialog(panel)" />
    </div>
    <Dialog v-model:visible="visibleDialog" :modal="true" :closable="true" :showHeader="true" :baseZIndex="10000">
        <PanelDetails :panel="selectedPanel" />
    </Dialog>
</template>

<script setup lang="ts">
import Dialog from 'primevue/dialog';
import Panel from './Panel.vue';
import { usePanelStore } from '@/stores/panelStore';
import { computed, ref } from 'vue';
import PanelDetails from './PanelDetails.vue';
const panelStore = usePanelStore();
const panels = computed(() => panelStore.getPanels)
const visibleDialog = ref(false);
const selectedPanel = ref();
const togglePanelDialog = (panel: any) => {
    selectedPanel.value = panel;
    visibleDialog.value = !visibleDialog.value;
}
</script>

<style scoped>
.panels-grid {
    margin-top: 1vh;
    display: grid;
    grid-template-columns: repeat(auto-fill, minmax(250px, 1fr));
    gap: 20px;
    padding-left: 1vw;
}
</style>
