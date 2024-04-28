import "./assets/main.css";
// HACK: No typescript declaration file available: https://github.com/apexcharts/vue3-apexcharts/issues/2
// eslint-disable-next-line
// @ts-ignore
import VueApexCharts from "vue3-apexcharts";
import { createApp } from "vue";
import { createPinia } from "pinia";
import PrimeVue from "primevue/config";
import App from "./App.vue";

const app = createApp(App);
app.use(createPinia());
app.use(PrimeVue);
app.use(VueApexCharts);
app.mount("#app");
