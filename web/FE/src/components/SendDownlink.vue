<template>
    <h3 id="conf">Configure Downlink</h3>
    <div id="form">
        <label>Report Interval:</label>
        <Calendar v-model="report_interval" timeOnly :show-seconds="true" />
        <label>Bypass:</label>
        <InputSwitch v-model="bypass" />
        <label>Reset:</label>
        <InputSwitch v-model="reset" />
        <Button @click="send_downlink">Send Downlink</Button>
    </div>
</template>

<script setup lang="ts">
import { ref, onMounted } from 'vue';
import Button from 'primevue/button';
import Calendar from 'primevue/calendar';
import InputSwitch from 'primevue/inputswitch';
import axiosService from '@/services/axiosService';

const props = defineProps({
    panel: Object
});

const bypass = ref(false);
const reset = ref(false);
const report_interval = ref(new Date());

onMounted(() => {
    try {
        const latestData = props.panel?.panel_data.reduce((prev: any, current: any) => {
            return (prev.id > current.id) ? prev : current;
        });
        const baseDate = new Date();
        baseDate.setHours(0, 0, 0, 0); // reset to midnight
        const timeInMillis = latestData ? latestData.report_interval * 1000 : 0;
        report_interval.value = new Date(baseDate.getTime() + timeInMillis);
        bypass.value = latestData?.bypass;
        reset.value = latestData?.reset;
    }
    catch {
        const baseDate = new Date();
        baseDate.setHours(0, 0, 0, 0); // reset to midnight
        report_interval.value = new Date(baseDate.getTime() + 60000);
        bypass.value = false;
        reset.value = false;
    }
});


function send_downlink() {
    const baseDate = new Date();
    baseDate.setHours(0, 0, 0, 0);
    const reportSeconds = (report_interval.value.getTime() - baseDate.getTime()) / 1000;
    return axiosService().post('/SendData', {
        dev_eui: props.panel?.deveui,
        report_interval: reportSeconds,
        bypass: bypass.value,
        reset: reset.value
    });
}
</script>

<style>
h3#conf {
    padding-top: 1vh;
}

div#form {
    display: grid;
    grid-template-columns: 10vw 100px;
    grid-gap: 1rem;
    padding: 1rem;
}

.p-inputswitch-input {
    margin: inherit;
}
</style>
