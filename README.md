# ✍️ ESP32-S3 RFID Writer & Reader

Este repositorio contiene un ejemplo avanzado para el uso del módulo **MFRC522** con un **ESP32-S3**. A diferencia de un lector simple, este código demuestra cómo autenticar sectores de una tarjeta MIFARE Classic, escribir datos personalizados en un bloque específico y verificar la escritura mediante una lectura inmediata.

## 🎯 Objetivo del Proyecto
El código realiza una operación de "escritura de mensaje" en la memoria física de una tarjeta o llavero RFID.
1. Detecta la tarjeta.
2. Se autentica en el **Sector 1 (Bloque 4)** usando la clave de fábrica.
3. Sobrescribe los 16 bytes del bloque con el mensaje: `"VIVA ESPANYA"`.
4. Lee el bloque para confirmar que los datos se grabaron correctamente.

## 🛠️ Esquema de Conexión (SPI)

El ESP32-S3 permite reasignar sus pines SPI. En este proyecto se han configurado los siguientes GPIOs:

| RC522 Pin | ESP32-S3 Pin | Función |
| :--- | :--- | :--- |
| **SDA (SS)** | GPIO 10 | Chip Select |
| **SCK** | GPIO 12 | Reloj SPI |
| **MOSI** | GPIO 11 | Master Out Slave In |
| **MISO** | GPIO 13 | Master In Slave Out |
| **RST** | GPIO 9 | Reset del módulo |
| **3.3V** | 3.3V | Alimentación |
| **GND** | GND | Tierra |

> [!WARNING]
> **Voltaje Crítico:** El módulo RC522 opera a **3.3V**. Conectarlo al pin de 5V del ESP32 dañará el sensor de forma irreversible.

## 📂 Estructura de Memoria Utilizada
El código interactúa con las tarjetas **MIFARE Classic 1K**, que están divididas en sectores y bloques:
- **Bloque seleccionado:** 4 (Sector 1, Bloque 0 del sector).
- **Capacidad por bloque:** 16 Bytes.
- **Clave de acceso:** `0xFF FF FF FF FF FF` (Transport Configuration).

## 🚀 Instalación y Uso

1. **Requisitos:** Tener instalado [PlatformIO](https://platformio.org/) o Arduino IDE con el core de ESP32.
2. **Librería:** Instalar la librería `MFRC522` de Miguel Balboa.
3. **Configuración:**
   - Copia el código en tu archivo principal (`main.cpp` o `.ino`).
   - Abre el Monitor Serial a **115200 baudios**.
4. **Ejecución:**
   - Acerca una tarjeta compatible.
   - Observa la consola para ver el proceso de autenticación y el volcado de datos en Hexadecimal.

## ⚠️ Notas de Seguridad
- **Bloques Trailer:** El código evita escribir en los bloques 3, 7, 11, etc. Estos bloques contienen los "Access Bits". Si se escriben datos incorrectos en ellos, la tarjeta quedará bloqueada permanentemente.
- **Caracteres Especiales:** El mensaje utiliza caracteres ASCII estándar para asegurar la compatibilidad con todos los lectores.

## 📝 Licencia
Este proyecto es de código abierto y puede ser utilizado para fines educativos y de aprendizaje en sistemas embebidos.

---
**Desarrollado para la práctica de protocolos de comunicación SPI.**
