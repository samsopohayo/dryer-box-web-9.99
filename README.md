// FILE: README.md
// ============================================

# Pengering Cabai Web App

Web application untuk monitoring dan kontrol sistem pengering cabai berbasis ESP32.

## Fitur Utama

1. **Dashboard** - Monitoring real-time suhu, kelembapan, dan kadar air
2. **Kontrol & Timer** - Kontrol manual heater, fan, exhaust dan pengaturan timer
3. **Rekap Pengeringan** - History dan export PDF data pengeringan

## Tech Stack

- Vue 3 + TypeScript
- Vite
- Tailwind CSS
- Firebase (Auth + Realtime Database)
- Pinia (State Management)
- jsPDF (Export PDF)

## Setup Project

1. Install dependencies:

```bash
npm install
```

2. Konfigurasi Firebase:

- Buat project di Firebase Console
- Enable Authentication (Email/Password)
- Enable Realtime Database
- Copy kredensial Firebase ke `src/firebase/config.ts`

3. Jalankan development server:

```bash
npm run dev
```

4. Build untuk production:

```bash
npm run build
```

## Struktur Firebase Database

```
/
├── sensor/
│   ├── suhu
│   ├── humidity
│   ├── kelembaban
│   ├── berat
│   └── berat_awal
├── status/
│   ├── pengeringan
│   ├── pemanas
│   ├── kipas
│   ├── exhaust
│   ├── door_open
│   ├── temp_protection
│   └── humidity_control
├── weather/
│   ├── main
│   ├── desc
│   └── temp
├── config/
│   ├── targetMoisture
│   ├── targetHumidityMin
│   └── targetHumidityMax
├── control/
│   ├── manual_heater_enable
│   ├── manual_heater_state
│   ├── manual_fan_enable
│   ├── manual_fan_state
│   ├── manual_exhaust_enable
│   └── manual_exhaust_state
├── timer/
│   ├── enabled
│   ├── duration
│   └── remaining
├── system/
│   ├── status
│   ├── last_update
│   ├── current_session
│   ├── process_status
│   └── last_error
└── sessions/
    └── {session_id}/
        ├── info/
        └── data/
```

## Firebase Security Rules

Tambahkan rules berikut di Firebase Console:

```json
{
  "rules": {
    "sensor": {
      ".read": "auth != null",
      ".write": true
    },
    "status": {
      ".read": "auth != null",
      ".write": true
    },
    "weather": {
      ".read": "auth != null",
      ".write": true
    },
    "config": {
      ".read": "auth != null",
      ".write": "auth != null"
    },
    "control": {
      ".read": "auth != null",
      ".write": "auth != null"
    },
    "timer": {
      ".read": "auth != null",
      ".write": "auth != null"
    },
    "system": {
      ".read": "auth != null",
      ".write": true
    },
    "sessions": {
      ".read": "auth != null",
      ".write": "auth != null"
    }
  }
}
```

## Catatan Penggunaan

1. **Login/Register**: Gunakan email dan password untuk autentikasi
2. **Dashboard**: Menampilkan data real-time dari ESP32
3. **Kontrol**: Toggle manual untuk heater/fan/exhaust dan atur timer
4. **History**: Pilih session untuk melihat detail dan download PDF

## Integrasi dengan ESP32

ESP32 akan mengirim data ke Firebase sesuai struktur di atas. Web app akan:

- Listen ke perubahan data real-time
- Menampilkan di UI
- Mengirim command control kembali ke Firebase untuk dibaca ESP32

## License

MIT
</div>
</main>
</div>

  </div>
</template>

<script setup lang="ts">
import { computed, onMounted, ref } from 'vue'
import { useDryerStore } from '@/stores/dryer'
import Header from '@/components/Header.vue'
import Sidebar from '@/components/Sidebar.vue'
import SensorCard from '@/components/SensorCard.vue'
import StatusCard from '@/components/StatusCard.vue'
import ChartMonitoring from '@/components/ChartMonitoring.vue'

const dryerStore = useDryerStore()
const chartData = ref<any[]>([])

const networkStatus = computed(() => {
  return dryerStore.systemStatus.status === 'ready' ? 'Stabil' : 'Tidak Stabil'
})

const wifiSSID = ref('Sam akhirnya sidang')

const sensorDHT22 = computed(() => {
  return dryerStore.sensorData.suhu > 0 ? 'OK' : 'ERROR'
})

const sensorHX711 = computed(() => {
  return dryerStore.sensorData.berat >= 0 ? 'OK' : 'ERROR'
})

const errorStatus = computed(() => {
  return dryerStore.systemStatus.last_error || 'None'
})

onMounted(async () => {
  if (dryerStore.systemStatus.current_session) {
    await dryerStore.fetchSessionData(dryerStore.systemStatus.current_session)
    
    const dataPoints = Object.values(dryerStore.currentSessionData)
    chartData.value = dataPoints.slice(0, 7).map(point => ({
      time: point.timestamp.split(' ')[1],
      moisture: point.moisture,
      humidity: point.humidity,
      temp: point.temperature
    }))
  }
})
</script>
