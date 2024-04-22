import { defineStore } from "pinia";
import axios from "axios";
import axiosService from "@/services/axiosService";
import type { panel } from "@/types";
export type State = {
  panels: Array<panel>;
  totalkwh: number;
  daykwh: number;
};
export type PanelStore = ReturnType<typeof usePanelStore>;

export const usePanelStore = defineStore({
  id: "panelStore",
  state: (): State => ({
    panels: [],
    totalkwh: 0,
    daykwh: 0,
  }),
  getters: {
    getPanels: (state) => state.panels,
    getPanelByDevEui: (state) => (devEui: string) =>
      state.panels.find((panel) => panel.deveui === devEui),
    getTotalkwh: (state) => state.totalkwh,
    getDaykwh: (state) => state.daykwh,
  },
  actions: {
    async LoadPanels() {
      axiosService()
        .get("/GetPanels")
        .then((response: any) => {
          this.$state.panels = response.data; // Update panels with fresh data
          this.CalculateTotalkwh();
          this.CalculateDaykwh();
        })
        .catch((error: any) => {
          if (axios.isAxiosError(error)) {
            console.error("API Error:", error.response?.data);
          } else {
            console.error("Network Error:", error);
          }
        });
    },
    async CalculateTotalkwh() {
      let totalkwh = 0;
      this.$state.panels.forEach((panel) => {
        const panelData = panel.panel_data;
        for (let j = 1; j < panelData.length; j++) {
          const currentData = panelData[j];
          const previousData = panelData[j - 1];
          const intervalHours =
            (new Date(currentData.created_at).getTime() -
              new Date(previousData.created_at).getTime()) /
            3600000; // Convert ms to hours
          totalkwh +=
            currentData.pv_voltage * currentData.pv_current * intervalHours;
        }
      });
      this.totalkwh = totalkwh;
    },
    async CalculateDaykwh() {
      let daykwh = 0;
      this.$state.panels.forEach((panel) => {
        const panelData = panel.panel_data;
        const time = new Date().getTime();
        for (let j = 1; j < panelData.length; j++) {
          const currentData = panelData[j];
          const previousData = panelData[j - 1];
          const currentDataTime = new Date(currentData.created_at).getTime();
          const previousDataTime = new Date(previousData.created_at).getTime();
          if (currentDataTime > time - 86400000) {
            const intervalHours =
              (currentDataTime - previousDataTime) / 3600000; // Convert ms to hours
            daykwh +=
              currentData.pv_voltage * currentData.pv_current * intervalHours;
          }
        }
      });
      this.$state.daykwh = daykwh;
    },
  },
});
