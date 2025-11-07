// FILE: src/types/index.ts (UPDATED - Added SystemStatus interface)
// ============================================

export interface ConfigData {
  targetMoisture: number;
  targetHumidityMin: number;
  targetHumidityMax: number;
  targetTemperatureMin: number;
  targetTemperatureMax: number;
}

export interface ControlData {
  manual_heater_enable: boolean;
  manual_heater_state: boolean;
  manual_fan_collector_enable: boolean;
  manual_fan_collector_state: boolean;
  manual_fan_panel_enable: boolean;
  manual_fan_panel_state: boolean;
  manual_exhaust_enable: boolean;
  manual_exhaust_state: boolean;
}

export interface SensorData {
  suhu: number;
  humidity: number;
  kadarAir: number;
  berat: number;
  berat_awal?: number;
}

export interface StatusData {
  pengeringan: string;
  pemanas: string;
  kipas: string;
  exhaust: string;
  door_open: boolean;
  temp_protection: boolean;
  humidity_control: boolean;
  temperature_control: boolean;
}

export interface WeatherData {
  main: string;
  desc: string;
  temp: number;
}

export interface TimerData {
  enabled: boolean;
  duration: number;
  remaining: number;
}

export interface SystemStatus {
  status: string;
  last_update: string;
  current_session: string;
  process_status: string;
  last_error: string;
  wifi_ssid?: string;
  wifi_rssi?: number;
  ip_address?: string;
  sensor_dht22?: boolean;
  sensor_hx711?: boolean;
  version?: string;
  last_boot?: string;
}

export interface SessionInfo {
  session_id: string;
  start_time: string;
  end_time?: string;
  initial_weight: number;
  final_weight?: number;
  final_moisture?: number;
  initial_humidity?: number;
  humidity_target_min?: number;
  humidity_target_max?: number;
  initial_temperature?: number;
  temperature_target_min?: number;
  temperature_target_max?: number;
  status: string;
}

export interface SessionDataPoint {
  timestamp: string;
  temperature: number;
  humidity: number;
  moisture: number;
  weight: number;
  status: string;
  heater: string;
  fan: string;
  exhaust: string;
  humidity_control?: boolean;
  temperature_control?: boolean;
  notifications?: string[];
}
