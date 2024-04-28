// Definition of types used in the application

export interface panel_data {
  id: number;
  pv_voltage: number;
  pv_current: number;
  bypass_state: boolean;
  created_at: Date;
  report_interval: number;
}

export interface panel {
  deveui: string;
  name: string;
  created_at: Date;
  updated_at: Date;
  panel_data: Array<panel_data>;
}

export interface send_data {
  dev_eui: string;
  report_interval: number;
  bypass: boolean;
  reset: boolean;
}

export interface data_from_esp {
  deveui: string | null;
  appeui: string | null;
  appkey: string | null;
}
