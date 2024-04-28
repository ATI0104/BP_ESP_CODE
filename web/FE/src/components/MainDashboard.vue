<template>
    <div>
        <div class="side_to_side">
            <div>
                <h1>Your photovoltaic system generated {{ totalKWh }} Wh</h1>
                <h2>During the last day {{ DayKwh }} Wh</h2>
            </div>
            <Button @click="show_add_panel_dialog()">Add new Panel</Button>
        </div>
        <Divider />
        <PanelsGrid />
    </div>
    <Dialog v-model:visible="add_panel_dialog" :modal="true" :closable="true" :showHeader="true" :baseZIndex="10000">
        <AddPanel @hide="show_add_panel_dialog" />
    </Dialog>
</template>

<script setup lang="ts">
import Dialog from 'primevue/dialog';
import PanelsGrid from '@/components/PanelsGrid.vue';
import Divider from 'primevue/divider';
import { computed, onMounted, ref } from 'vue';
import { usePanelStore } from '@/stores/panelStore';
import { useConfigStore } from '@/stores/configStore';
import AddPanel from '@/components/AddPanel.vue';
import Button from 'primevue/button';
const panelStore = usePanelStore();
const configStore = useConfigStore();
const DayKwh = computed(() => panelStore.getDaykwh.toFixed(2));
const totalKWh = computed(() => panelStore.getTotalkwh.toFixed(2));
const add_panel_dialog = ref(false);
onMounted(() => {
    LoadPanels();
});
function LoadPanels() {
    panelStore.LoadPanels();
    setTimeout(LoadPanels, configStore.$state.refreshInterval);
}
function show_add_panel_dialog() {
    if (add_panel_dialog.value) {
        add_panel_dialog.value = false;
    } else {
        add_panel_dialog.value = true;
    }
}
</script>

<style scoped>
Divider {
    margin-top: 2vw;
    margin-bottom: 1vw;
}

Button {
    margin-top: 1%;
    margin-right: 1%;
    margin-bottom: 1%;
}

.side_to_side {
    display: flex;
    justify-content: space-between;
}
</style>