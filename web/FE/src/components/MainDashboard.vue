<template>
    <div>
        <h1>Your photovoltaic system generated {{ totalKWh }} Wh</h1>
        <h2>During the last day {{ DayKwh }} Wh</h2>
        <Divider />
        <PanelsGrid />
    </div>
</template>

<script setup lang="ts">
import PanelsGrid from '@/components/PanelsGrid.vue';
import Divider from 'primevue/divider';
import { computed, onMounted } from 'vue';
import { usePanelStore } from '@/stores/panelStore';
import { useConfigStore } from '@/stores/configStore';
const panelStore = usePanelStore();
const configStore = useConfigStore();
const DayKwh = computed(() => panelStore.getDaykwh.toFixed(2));
const totalKWh = computed(() => panelStore.getTotalkwh.toFixed(2));

onMounted(() => {
    LoadPanels();
});
function LoadPanels() {
    panelStore.LoadPanels();
    setTimeout(LoadPanels, configStore.$state.refreshInterval);
}
</script>

<style scoped>
Divider {
    margin-top: 2vw;
    margin-bottom: 1vw;
}
</style>