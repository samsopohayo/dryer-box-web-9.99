// FILE: src/types/index.ts
// ============================================

export interface ConfigData {
  targetMoisture: number;
  targetHumidityMin: number;
  targetHumidityMax: number;
}

export interface ControlData {
  manual_heater_enable: boolean;
  manual_heater_state: boolean;
  manual_fan_enable: boolean;
  manual_fan_state: boolean;
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

export interface SessionInfo {
  session_id: string;
  start_time: string;
  end_time?: string;
  initial_weight: number;
  final_weight?: number;
  final_moisture?: number;
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
}
