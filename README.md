# ⚡ Arduino & ESP32 Lab

![Neovim](https://img.shields.io/badge/Editor-Neovim_v0.11+-57A143?style=for-the-badge&logo=neovim&logoColor=white)
![Plugin](https://img.shields.io/badge/Plugin-Yuukiflow/Arduino--Nvim-blue?style=for-the-badge&logo=lua&logoColor=white)
![C++](https://img.shields.io/badge/Language-C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)

Colección de proyectos personales, prototipos de IoT, sensores ambientales y experimentos de Computer Vision.

> **Nota:** Este repositorio es un "monorepo" que contiene tanto los sketches como las librerías modificadas necesarias. Desarrollado enteramente en **Neovim** usando el plugin [Arduino-Nvim](https://github.com/yuukiflow/Arduino-Nvim) para un flujo de trabajo sin salir del editor.

## 📂 Estructura del Proyecto

El repositorio está organizado por tipo de hardware y funcionalidad:

### 🧠 AI & Computer Vision (Edge Impulse)
Proyectos que utilizan ESP32-CAM y modelos de Machine Learning (TinyML).

| Directorio | Descripción | Hardware |
| :--- | :--- | :--- |
| `Colli2008-project-1...` | **Inferencia AI:** Proyecto exportado de Edge Impulse para detección de objetos/sonido. | ESP32 / Nano BLE |
| `esp32_camera` | Configuración base y streaming para el módulo de cámara. | ESP32-CAM (AI Thinker) |
| `pantalla_camara_adre` | Integración de cámara con salida a pantalla (OLED/TFT). | ESP32-CAM + Display |

### 🌡️ Sensores y Monitoreo
Instrumentación para medir calidad del aire y condiciones ambientales.

| Directorio | Descripción | Sensores |
| :--- | :--- | :--- |
| `Sensor-de-CO2` | **Monitor de Calidad de Aire:** Medición de ppm de CO2 con visualización en pantalla. | MH-Z19B + OLED |
| `DHTtester` | Pruebas básicas de temperatura y humedad. | DHT11 / DHT22 |
| `libraries/MH-Z19` | Librerías específicas y drivers para el sensor de CO2 infrarrojo. | N/A |

### 🛠️ Tests y Prototipos (Sandbox)
Zona de pruebas para componentes individuales.

*   **`led`**: Pruebas básicas de blink y PWM (el "Hola Mundo").
*   **`Random` / `Patata`**: Scripts de prueba y algoritmos sueltos.
*   **`teste` / `TESTER`**: Unit testing manual de componentes nuevos.

---

## 💻 Entorno de Desarrollo (Workflow)

Este proyecto utiliza un entorno 100% integrado en **Neovim**, abandonando el IDE clásico de Arduino.

*   **IDE:** Neovim (v0.11+)
*   **Gestor:** Plugin `yuukiflow/Arduino-Nvim`
*   **Selección:** Interfaz visual con `Telescope` para placas y puertos.
*   **LSP:** `clangd` para autocompletado inteligente en C++.

---

## 🚀 Uso (Comandos Neovim)

Todo se maneja mediante comandos dentro del editor. No se requiere terminal externa.

### 1. Iniciar Proyecto
Abrir el archivo principal (`.ino`) en Neovim. El plugin detectará automáticamente el entorno Arduino.

```vim
nvim Sensor-de-CO2/Sensor-de-CO2.ino
```

### 2. Configuración (Telescope)
Usa los atajos del plugin para configurar el hardware (normalmente con `<Leader>a`):

*   **Seleccionar Placa:** `:InoBoard` (o `<Leader>ab`)
    *   *Despliega lista de placas en Telescope.*
*   **Seleccionar Puerto:** `:InoPort` (o `<Leader>ap`)
    *   *Detecta `/dev/ttyUSB0`, `/dev/ttyACM0`, etc.*

### 3. Compilar y Subir
*   **Verificar/Compilar:** `:InoCheck` (o `<Leader>ac`)
*   **Subir a la placa:** `:InoUpload` (o `<Leader>au`)
*   **Monitor Serie:** `:InoMonitor` (o `<Leader>as`)

> *Las dependencias y librerías externas ya están incluidas en la carpeta `/libraries`, asegurando portabilidad total sin depender del Library Manager global.*

---
*Hecho con ❤️ en Chile.*

