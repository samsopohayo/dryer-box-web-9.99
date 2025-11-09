// FILE: src/stores/dryer.ts (UPDATED - Added auto_mode support)
import { defineStore } from "pinia";
import { ref, computed } from "vue";
import { database } from "@/firebase/config";
import { ref as dbRef, onValue, set, get } from "firebase/database";
import type {
  SensorData,
  StatusData,
  WeatherData,
  ConfigData,
  ControlData,
  SessionDataPoint,
  SystemStatus,
} from "@/types";

export const useDryerStore = defineStore("dryer", () => {
  const configData = ref<ConfigData>({
    targetMoisture: 0,
    targetHumidityMin: 0,
    targetHumidityMax: 0,
    targetTemperatureMin: 0,
    targetTemperatureMax: 0,
  });

  const controlData = ref<ControlData>({
    auto_mode: true, // Default to AUTO mode
    manual_heater_enable: false,
    manual_heater_state: false,
    manual_fan_collector_enable: false,
    manual_fan_collector_state: false,
    manual_fan_panel_enable: false,
    manual_fan_panel_state: false,
    manual_exhaust_enable: false,
    manual_exhaust_state: false,
  });

  const sensorData = ref<SensorData>({
    suhu: 0,
    humidity: 0,
    kadarAir: 0,
    berat: 0,
    berat_awal: 0,
  });

  const statusData = ref<StatusData>({
    pengeringan: "Siap",
    pemanas: "Mati",
    kipas: "Mati",
    exhaust: "Tutup",
    door_open: false,
    temp_protection: false,
    humidity_control: false,
    temperature_control: false,
  });

  const weatherData = ref<WeatherData>({
    main: "",
    desc: "",
    temp: 0,
  });

  const systemStatus = ref<SystemStatus>({
    status: "ready",
    last_update: "",
    current_session: "",
    process_status: "idle",
    last_error: "None",
    wifi_ssid: undefined,
    wifi_rssi: undefined,
    ip_address: undefined,
    sensor_dht22: undefined,
    sensor_hx711: undefined,
    version: undefined,
    last_boot: undefined,
  });

  const sessions = ref<Record<string, any>>({});
  const currentSessionData = ref<Record<string, SessionDataPoint>>({});

  const overallStatus = computed(() => {
    if (statusData.value.door_open) return "Pintu Terbuka";
    if (statusData.value.temp_protection) return "Proteksi Suhu";
    if (sensorData.value.kadarAir <= 15) return "Kering";
    if (sensorData.value.humidity <= 50) return "Normal";
    return "Lembab";
  });

  const initializeListeners = () => {
    onValue(dbRef(database, "config"), (snapshot) => {
      if (snapshot.exists()) {
        configData.value = snapshot.val();
      }
    });

    onValue(dbRef(database, "control"), (snapshot) => {
      if (snapshot.exists()) {
        const data = snapshot.val();
        controlData.value = {
          auto_mode: data.auto_mode !== false, // Default true if not set
          manual_heater_enable: data.manual_heater_enable || false,
          manual_heater_state: data.manual_heater_state || false,
          manual_fan_collector_enable:
            data.manual_fan_collector_enable || false,
          manual_fan_collector_state: data.manual_fan_collector_state || false,
          manual_fan_panel_enable: data.manual_fan_panel_enable || false,
          manual_fan_panel_state: data.manual_fan_panel_state || false,
          manual_exhaust_enable: data.manual_exhaust_enable || false,
          manual_exhaust_state: data.manual_exhaust_state || false,
        };
      }
    });

    onValue(dbRef(database, "sensor"), (snapshot) => {
      if (snapshot.exists()) {
        sensorData.value = snapshot.val();
      }
    });

    onValue(dbRef(database, "status"), (snapshot) => {
      if (snapshot.exists()) {
        statusData.value = snapshot.val();
      }
    });

    onValue(dbRef(database, "system"), (snapshot) => {
      if (snapshot.exists()) {
        const data = snapshot.val();
        systemStatus.value = {
          status: data.status || "ready",
          last_update: data.last_update || "",
          current_session: data.current_session || "",
          process_status: data.process_status || "idle",
          last_error: data.last_error || "None",
          wifi_ssid: data.wifi_ssid,
          wifi_rssi: data.wifi_rssi,
          ip_address: data.ip_address,
          sensor_dht22: data.sensor_dht22,
          sensor_hx711: data.sensor_hx711,
          version: data.version,
          last_boot: data.last_boot,
        };
      }
    });

    onValue(dbRef(database, "weather"), (snapshot) => {
      if (snapshot.exists()) {
        weatherData.value = snapshot.val();
      }
    });

    onValue(dbRef(database, "sessions"), (snapshot) => {
      if (snapshot.exists()) {
        sessions.value = snapshot.val();
      }
    });
  };

  const updateControl = async (path: string, value: boolean) => {
    await set(dbRef(database, `control/${path}`), value);
  };

  const updateConfig = async (config: Partial<ConfigData>) => {
    const updates: any = {};
    if (config.targetMoisture !== undefined) {
      updates.targetMoisture = config.targetMoisture;
    }
    if (config.targetHumidityMin !== undefined) {
      updates.targetHumidityMin = config.targetHumidityMin;
    }
    if (config.targetHumidityMax !== undefined) {
      updates.targetHumidityMax = config.targetHumidityMax;
    }
    if (config.targetTemperatureMin !== undefined) {
      updates.targetTemperatureMin = config.targetTemperatureMin;
    }
    if (config.targetTemperatureMax !== undefined) {
      updates.targetTemperatureMax = config.targetTemperatureMax;
    }
    await set(dbRef(database, "config"), updates);
  };

  const fetchSessionData = async (sessionId: string) => {
    try {
      const snapshot = await get(dbRef(database, `sessions/${sessionId}/data`));

      if (snapshot.exists()) {
        currentSessionData.value = snapshot.val();
        console.log(
          `Loaded ${
            Object.keys(snapshot.val()).length
          } data points for session ${sessionId}`
        );
      } else {
        console.warn(`No data found for session ${sessionId}`);
        currentSessionData.value = {};
      }
    } catch (error) {
      console.error("Error fetching session data:", error);
      currentSessionData.value = {};
    }
  };

  return {
    configData,
    controlData,
    sensorData,
    statusData,
    weatherData,
    systemStatus,
    sessions,
    currentSessionData,
    overallStatus,
    initializeListeners,
    updateControl,
    updateConfig,
    fetchSessionData,
  };
});
