#include <U8g2lib.h>
#include <Colli2008-project-1_inferencing.h>
#include "edge-impulse-sdk/dsp/image/image.hpp"
#include "esp_camera.h"

// Declaración de la función para obtener datos de la cámara
int ei_camera_get_data(size_t offset, size_t length, float *out_ptr);

// Configuración de la pantalla OLED
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

#define CAMERA_MODEL_ESP_EYE // Configuración de cámara usada

#if defined(CAMERA_MODEL_ESP_EYE)
#define PWDN_GPIO_NUM    -1
#define RESET_GPIO_NUM   -1
#define XCLK_GPIO_NUM    4
#define SIOD_GPIO_NUM    18
#define SIOC_GPIO_NUM    23

#define Y9_GPIO_NUM      36
#define Y8_GPIO_NUM      37
#define Y7_GPIO_NUM      38
#define Y6_GPIO_NUM      39
#define Y5_GPIO_NUM      35
#define Y4_GPIO_NUM      14
#define Y3_GPIO_NUM      13
#define Y2_GPIO_NUM      34
#define VSYNC_GPIO_NUM   5
#define HREF_GPIO_NUM    27
#define PCLK_GPIO_NUM    25
#else
#error "Camera model not selected"
#endif

/* Constantes de configuración */
#define EI_CAMERA_RAW_FRAME_BUFFER_COLS           320
#define EI_CAMERA_RAW_FRAME_BUFFER_ROWS           240
#define EI_CAMERA_FRAME_BYTE_SIZE                 3

/* Variables globales */
static bool debug_nn = false;
static bool is_initialised = false;
uint8_t *snapshot_buf; // Buffer para la captura de imágenes

// Configuración de la cámara
static camera_config_t camera_config = {
    .pin_pwdn = PWDN_GPIO_NUM,
    .pin_reset = RESET_GPIO_NUM,
    .pin_xclk = XCLK_GPIO_NUM,
    .pin_sscb_sda = SIOD_GPIO_NUM,
    .pin_sscb_scl = SIOC_GPIO_NUM,

    .pin_d7 = Y9_GPIO_NUM,
    .pin_d6 = Y8_GPIO_NUM,
    .pin_d5 = Y7_GPIO_NUM,
    .pin_d4 = Y6_GPIO_NUM,
    .pin_d3 = Y5_GPIO_NUM,
    .pin_d2 = Y4_GPIO_NUM,
    .pin_d1 = Y3_GPIO_NUM,
    .pin_d0 = Y2_GPIO_NUM,
    .pin_vsync = VSYNC_GPIO_NUM,
    .pin_href = HREF_GPIO_NUM,
    .pin_pclk = PCLK_GPIO_NUM,

    .xclk_freq_hz = 20000000,
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,

    .pixel_format = PIXFORMAT_JPEG,
    .frame_size = FRAMESIZE_QVGA,

    .jpeg_quality = 12,
    .fb_count = 1,
    .fb_location = CAMERA_FB_IN_PSRAM,
    .grab_mode = CAMERA_GRAB_WHEN_EMPTY,
};

/* Declaración de funciones */
bool ei_camera_init(void);
void ei_camera_deinit(void);
bool ei_camera_capture(uint32_t img_width, uint32_t img_height, uint8_t *out_buf);
void mostrarEnPantalla(const char *mensaje);

/* Configuración inicial */
void setup() {
    Serial.begin(115200);
    while (!Serial);

    // Inicializa la pantalla OLED
    u8g2.begin();
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0, 10, "Inicializando...");
    u8g2.sendBuffer();
    delay(2000);

    // Inicializa la cámara
    if (!ei_camera_init()) {
        Serial.println("Failed to initialize Camera!");
        mostrarEnPantalla("Error: Camara!");
        while (1);
    }

    mostrarEnPantalla("Camara lista!");
    delay(2000);
}

/* Bucle principal */
void loop() {
    // Reserva memoria para la imagen capturada
    snapshot_buf = (uint8_t*)malloc(EI_CAMERA_RAW_FRAME_BUFFER_COLS * EI_CAMERA_RAW_FRAME_BUFFER_ROWS * EI_CAMERA_FRAME_BYTE_SIZE);

    if (snapshot_buf == nullptr) {
        mostrarEnPantalla("Error: Memoria!");
        return;
    }

    ei::signal_t signal;
    signal.total_length = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT;
    signal.get_data = &ei_camera_get_data;

    // Captura la imagen
    if (!ei_camera_capture((size_t)EI_CLASSIFIER_INPUT_WIDTH, (size_t)EI_CLASSIFIER_INPUT_HEIGHT, snapshot_buf)) {
        mostrarEnPantalla("Error: Captura!");
        free(snapshot_buf);
        return;
    }

    // Corre el clasificador
    ei_impulse_result_t result = {0};
    EI_IMPULSE_ERROR err = run_classifier(&signal, &result, debug_nn);

    if (err != EI_IMPULSE_OK) {
        mostrarEnPantalla("Error: Modelo!");
        free(snapshot_buf);
        return;
    }

    // Muestra las predicciones en la pantalla OLED
    for (uint16_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
        if (result.classification[i].value > 0.5) { // Ajusta el umbral según sea necesario
            mostrarEnPantalla(ei_classifier_inferencing_categories[i]);
            break;
        }
    }

    free(snapshot_buf);
    delay(1000);
}

/* Función para inicializar la cámara */
bool ei_camera_init(void) {
    if (is_initialised) return true;

    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK) {
        Serial.printf("Camera init failed with error 0x%x\n", err);
        return false;
    }

    is_initialised = true;
    return true;
}

/* Función para capturar imágenes */
bool ei_camera_capture(uint32_t img_width, uint32_t img_height, uint8_t *out_buf) {
    if (!is_initialised) {
        Serial.println("ERR: Camera is not initialized");
        return false;
    }

    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("Camera capture failed");
        return false;
    }

    bool converted = fmt2rgb888(fb->buf, fb->len, PIXFORMAT_JPEG, snapshot_buf);
    esp_camera_fb_return(fb);

    if (!converted) {
        Serial.println("Conversion failed");
        return false;
    }

    return true;
}

/* Función para mostrar texto en la pantalla OLED */
void mostrarEnPantalla(const char *mensaje) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0, 10, "Bacteria:");
    u8g2.drawStr(0, 30, mensaje);
    u8g2.sendBuffer();
}

/* Implementación de la función para obtener datos de la cámara */
int ei_camera_get_data(size_t offset, size_t length, float *out_ptr) {
    size_t pixel_ix = offset * 3;
    size_t pixels_left = length;
    size_t out_ptr_ix = 0;

    while (pixels_left != 0) {
        out_ptr[out_ptr_ix] = (snapshot_buf[pixel_ix + 2] << 16) + (snapshot_buf[pixel_ix + 1] << 8) + snapshot_buf[pixel_ix];
        out_ptr_ix++;
        pixel_ix += 3;
        pixels_left--;
    }

    return 0;
}
