#include <Colli2008-project-1_inferencing.h>
#include <eloquent_esp32cam.h>
#include <eloquent_esp32cam/edgeimpulse/fomo.h>
#include <U8g2lib.h>
#include <Wire.h>

using eloq::camera;
using eloq::ei::fomo;

// Definición de la pantalla OLED I2C
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, 255, 13, 15);

/**
 * Configuración inicial
 */
void setup() {
    delay(3000);
    Serial.begin(115200);
    Serial.println("__EDGE IMPULSE FOMO (NO-PSRAM)__");

    // Inicializar la pantalla OLED
    u8g2.begin();
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_pcsenior_8f);
    u8g2.drawStr(2, 16, "Inicializando...");
    u8g2.sendBuffer();

    // Configuración de la cámara
    camera.pinout.aithinker();
    camera.brownout.disable();
    camera.resolution.yolo(); // Solo funciona en 96x96 RGB565
    camera.pixformat.rgb565();

    // Inicializar la cámara
    while (!camera.begin().isOk()) {
        Serial.println(camera.exception.toString());
        u8g2.clearBuffer();
        u8g2.drawStr(2, 16, "Error camara");
        u8g2.sendBuffer();
        delay(2000);
    }

    Serial.println("Camera OK");
    u8g2.clearBuffer();
    u8g2.drawStr(2, 16, "Camara OK");
    u8g2.sendBuffer();
    delay(2000);
}

/**
 * Bucle principal
 */
void loop() {
    // Capturar imagen
    if (!camera.capture().isOk()) {
        Serial.println(camera.exception.toString());
        return;
    }

    // Ejecutar FOMO
    if (!fomo.run().isOk()) {
        Serial.println(fomo.exception.toString());
        return;
    }

    // Mostrar resultados
    Serial.printf("Found %d object(s) in %dms\n", fomo.count(), fomo.benchmark.millis());

    // Si no se detecta ningún objeto, salir
    if (!fomo.foundAnyObject()) {
        u8g2.clearBuffer();
        u8g2.drawStr(2, 16, "No detectado");
        u8g2.sendBuffer();
        delay(200);
        return;
    }
    // Si se detecta un solo objeto
    Serial.printf("Found %s at (x = %d, y = %d) (size %d x %d). Proba is %.2f\n",
                  fomo.first.label,
                  fomo.first.x,
                  fomo.first.y,
                  fomo.first.width,
                  fomo.first.height,
                  fomo.first.proba);

    // Mostrar en pantalla OLED
    u8g2.clearBuffer();
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "Objeto: %s", fomo.first.label);
    u8g2.drawStr(2, 16, buffer);
    snprintf(buffer, sizeof(buffer), "Conf: %.2f", fomo.first.proba);
    u8g2.drawStr(2, 32, buffer);
    u8g2.sendBuffer();

    // Si se detectan múltiples objetos
    if (fomo.count() > 1) {
        fomo.forEach([](int i, bbox_t bbox) {
            Serial.printf("#%d) Found %s at (x = %d, y = %d) (size %d x %d). Proba is %.2f\n",
                          i + 1,
                          bbox.label,
                          bbox.x,
                          bbox.y,
                          bbox.width,
                          bbox.height,
                          bbox.proba);
        });
    }

    delay(200);  // Esperar antes de la próxima iteración
}
