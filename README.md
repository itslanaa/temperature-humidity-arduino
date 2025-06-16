# 🌡️ Panduan Lengkap DHT11 ESP8266 WebSocket Dashboard

## 📋 Komponen yang Dibutuhkan

### Hardware:
- **ESP8266** (NodeMCU/Wemos D1 Mini)
- **Sensor DHT11**
- **Kabel Jumper**
- **Breadboard** (opsional)
- **Resistor 10kΩ** (pull-up, opsional tapi direkomendasikan)

### Software:
- **Arduino IDE** (versi 1.8.19 atau lebih baru)
- **Library Arduino** yang diperlukan

## 🔌 Koneksi Hardware

### Skema Wiring DHT11 ke ESP8266:

```
DHT11          ESP8266 (NodeMCU)
------         ------------------
VCC    ------>  3.3V atau 5V
GND    ------>  GND
DATA   ------>  D4 (GPIO 2)
```

### Diagram Koneksi:
```
     ESP8266 NodeMCU
    ┌─────────────────┐
    │                 │
    │ 3.3V ●─────────●│ VCC (DHT11)
    │ GND  ●─────────●│ GND (DHT11) 
    │ D4   ●─────────●│ DATA (DHT11)
    │                 │
    └─────────────────┘
```

**Catatan:** Tambahkan resistor 10kΩ antara VCC dan DATA pin untuk stabilitas yang lebih baik.

## 📚 Instalasi Library Arduino

### 1. Install ESP8266 Board Package:
1. Buka Arduino IDE
2. File → Preferences
3. Tambahkan URL berikut di "Additional Board Manager URLs":
   ```
   http://arduino.esp8266.com/stable/package_esp8266com_index.json
   ```
4. Tools → Board → Board Manager
5. Cari "ESP8266" dan install

### 2. Install Library yang Diperlukan:

Buka **Tools → Manage Libraries** dan install:

#### Library Wajib:
- **DHT sensor library** by Adafruit (versi 1.4.4+)
- **Adafruit Unified Sensor** (dependency untuk DHT)
- **ArduinoJson** by Benoit Blanchon (versi 6.21.3+)
- **WebSockets** by Markus Sattler (versi 2.4.0+)

#### Cara Install:
1. Sketch → Include Library → Manage Libraries
2. Ketik nama library di search box
3. Klik "Install" pada library yang sesuai

### 3. Library Bawaan ESP8266:
Library ini sudah termasuk saat install ESP8266 board:
- ESP8266WiFi
- ESP8266WebServer

## ⚙️ Konfigurasi dan Upload

### 1. Setting Board di Arduino IDE:
- **Board:** "NodeMCU 1.0 (ESP-12E Module)" atau sesuai board Anda
- **Upload Speed:** 115200
- **CPU Frequency:** 80 MHz
- **Flash Size:** 4MB (FS:2MB OTA:~1019KB)
- **Port:** Pilih port COM yang sesuai

### 2. Edit Konfigurasi WiFi:
```cpp
// Ganti dengan kredensial WiFi Anda
const char* ssid = "NAMA_WIFI_ANDA";
const char* password = "PASSWORD_WIFI_ANDA";
```

### 3. Upload Code:
1. Hubungkan ESP8266 ke komputer via USB
2. Pilih board dan port yang tepat
3. Klik tombol "Upload" (panah ke kanan)
4. Tunggu hingga upload selesai

## 🌐 Setup Dashboard HTML

### 1. Update WebSocket URL:
Buka file HTML dashboard dan ubah baris berikut:
```javascript
// Ganti IP dengan IP ESP8266 Anda (lihat Serial Monitor)
ws = new WebSocket('ws://192.168.1.100:81/');
```

### 2. Cari IP Address ESP8266:
1. Buka Serial Monitor (Ctrl+Shift+M)
2. Set baud rate ke 115200
3. Reset ESP8266
4. Catat IP address yang muncul, contoh: `192.168.1.150`
5. Ganti IP di kode JavaScript

### 3. Test Koneksi:
1. Buka browser
2. Akses: `http://IP_ESP8266` (contoh: `http://192.168.1.150`)
3. Anda akan melihat halaman status sensor

## 🚀 Cara Menjalankan

### 1. Jalankan ESP8266:
1. Upload kode ke ESP8266
2. Buka Serial Monitor untuk melihat status
3. Pastikan terhubung ke WiFi dan menampilkan IP address

### 2. Buka Dashboard:
1. Buka file HTML di browser
2. Dashboard akan otomatis mencoba terhubung
3. Jika berhasil, indikator akan menunjukkan "Terhubung ke ESP8266"

## 🔧 Troubleshooting

### ESP8266 tidak terhubung WiFi:
- ✅ Pastikan SSID dan password benar
- ✅ ESP8266 dalam jangkauan WiFi
- ✅ WiFi 2.4GHz (bukan 5GHz)

### Sensor DHT11 error:
- ✅ Periksa koneksi wiring
- ✅ Pastikan DHT11 dapat power yang cukup
- ✅ Tambahkan resistor pull-up 10kΩ
- ✅ Tunggu 2 detik setelah power on sebelum baca sensor

### WebSocket tidak terhubung:
- ✅ Pastikan IP address di HTML sesuai dengan ESP8266
- ✅ Cek firewall komputer
- ✅ Pastikan ESP8266 dan komputer di network yang sama

### Dashboard tidak update:
- ✅ Buka Browser Developer Tools (F12) untuk cek error
- ✅ Pastikan JavaScript tidak terblokir
- ✅ Refresh halaman

## 📊 Fitur Dashboard

### Real-time Monitoring:
- **Suhu** dalam Celsius dengan status (Normal/Dingin/Panas)
- **Kelembaban** dalam persen dengan status (Normal/Kering/Lembab)
- **Heat Index** dengan tingkat bahaya

### Visualisasi:
- **Grafik real-time** untuk 30 data terakhir
- **Animasi** dan efek visual modern
- **Statistik** min, max, dan rata-rata

### Kontrol:
- **Reset data** untuk membersihkan grafik
- **Export CSV** untuk analisis lebih lanjut
- **Status koneksi** real-time

## 🎨 Kustomisasi

### Mengubah Interval Pembacaan:
```cpp
const unsigned long sensorInterval = 2000;  // 2 detik
const unsigned long wsInterval = 2000;      // 2 detik
```

### Mengubah Pin DHT11:
```cpp
#define DHT_PIN 2  // Ganti dengan pin yang diinginkan
```

### Mengubah Threshold Status:
Edit fungsi `updateStatus()` di HTML untuk mengubah batas normal/warning/danger.

## 📱 Akses Mobile

Dashboard sudah responsive dan bisa diakses via smartphone:
1. Pastikan phone terhubung ke WiFi yang sama
2. Buka browser di phone
3. Akses IP ESP8266

## 🔒 Tips Keamanan

- Gunakan WiFi dengan password
- Jangan expose ESP8266 ke internet publik tanpa autentikasi
- Ganti default credentials jika ada

---

**Selamat mencoba! Jika ada pertanyaan, silakan tanya.** 🚀
