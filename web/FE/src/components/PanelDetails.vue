<template>
    <div class="panel-details">
        <div class="header">
            <h2 v-if="panel">{{ panel.name }}</h2>
        </div>
        <div class="toggle-options">
            <SelectButton :options="options" v-model="selectedDuration" @change="onDurationChange" />
            <div v-if="isCustomDuration" class="date-pickers">
                <Calendar placeholder="Start Date" v-model="startDate" />
                <Calendar placeholder="End Date" v-model="endDate" />
            </div>
        </div>
        <div id="chart">
            <apexchart type="line" :options="chartOptions" :series="chartSeries" />
        </div>
    </div>
</template>

<script setup lang="ts">
import { ref, watch, onMounted } from 'vue';
import SelectButton from 'primevue/selectbutton';
import Calendar from 'primevue/calendar';
const props = defineProps({
    panel: Object
});
const options = [
    '1 year', '6 months', '1 month', '24 hours', '1 hour', 'custom'
];

const selectedDuration = ref('1m');
const isCustomDuration = ref(false);
const startDate = ref(null);
const endDate = ref(null);
onMounted(() => {
    onDurationChange();
});
function onDurationChange() {
    // Load data for the selected duration or date range
    const now = new Date().getTime();
    let start;
    switch (selectedDuration.value) {
        case '1 year':
            start = new Date(now - 31536000000); // 365 days
            break;
        case '6 month':
            start = new Date(now - 15768000000); // 182.5 days
            break;
        case '1 month':
            start = new Date(now - 2628000000); // 30.44 days
            break;
        case '24 hours':
            start = new Date(now - 86400000); // 24 hours
            break;
        case '1 hour':
            start = new Date(now - 3600000); // 1 hour
            break;
        case 'custom':
            if (startDate.value && endDate.value) {
                start = new Date(startDate.value);
                const end = new Date(endDate.value);
                calculatePower(start, end);
            }
            return;
    }
    if (start) {
        calculatePower(start, new Date()); // Calculating up to the current time
    }
}
function calculatePower(startDate: Date, endDate: Date) {
    const startMs = startDate.getTime();
    const endMs = endDate.getTime();
    chartSeries.value[0].data = [];
    // Filter data within the selected date range
    const panelData = props.panel?.panel_data.filter((d: { created_at: string | number | Date; }) => {
        const createdTime = new Date(d.created_at).getTime();
        return createdTime >= startMs && createdTime <= endMs;
    });
    const arranged_data = panelData?.sort((a: { created_at: string | number | Date; }, b: { created_at: string | number | Date; }) => new Date(a.created_at).getTime() - new Date(b.created_at).getTime());
    arranged_data?.forEach((d: { created_at: string | number | Date; pv_voltage: number; pv_current: number; }) => {
        const createdTime = new Date(d.created_at).getTime();
        const Power = d.pv_voltage * d.pv_current;
        chartSeries.value[0].data.push([createdTime, Power]);
    });

    console.log('Chart data updated for visualization.');
}


const chartOptions = ref({
    chart: {
        type: 'line',
        height: 350,
        zoom: {
            enabled: true,
            type: 'x',
            autoScaleYaxis: true
        },
        toolbar: {
            autoSelected: 'zoom'
        }
    },
    dataLabels: {
        enabled: false
    },
    stroke: {
        curve: 'smooth'
    },
    title: {
        text: '',
        align: 'left'
    },
    markers: {
        size: 1
    },
    xaxis: {
        type: 'datetime',
    },
    tooltip: {
        x: {
            format: 'dd-MMM-yyyy HH:mm'
        }
    }
});
const chartSeries = ref([{
    name: "Power",
    unit: "W",
    data: [] as number[][]
}]);

watch(selectedDuration, (newVal) => {
    isCustomDuration.value = newVal === 'custom';
});

</script>

<style scoped>
.header {
    display: flex;
    justify-content: space-between;
    align-items: center;
}


.toggle-options {
    margin-top: 20px;
    display: flex;
}

.date-pickers {
    display: flex;
    gap: 10px;
    padding-left: 10px;
}

.panel-details {
    padding: 20px;
    width: 60vw;
}
</style>
