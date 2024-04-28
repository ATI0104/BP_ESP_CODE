import { defineStore } from "pinia";

// Define the useConfigStore first
export type ConfigStore = ReturnType<typeof useConfigStore>;
export const useConfigStore = defineStore({
  id: "configStore",
  state: (): State => ({
    refreshInterval: 1 * 60 * 1000,
  }),
  getters: {
    API_URL: () => "/api",
  },
});

// Then, define the ConfigStore type using ReturnType<typeof useConfigStore>
export type State = {
  refreshInterval: number;
};
