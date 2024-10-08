#include "esp_camera.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#define CAMERA_MODEL_AI_THINKER 
#include "camera_pins.h"

// WiFi your credentials
const char *ssid = "*****";
const char *password = "*****";

// Server URL
const char* serverUrl = "http://10.128.0.135:5000/upload";

// Definir os semáforos
SemaphoreHandle_t imgReadySemaphore;
SemaphoreHandle_t detectionSemaphore;

// Variável global para armazenar a imagem
camera_fb_t *fb = NULL;

// Função para conectar ao WiFi
void wifiConnect() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi conectado");
}

// Thread de aquisição de imagens
void imageAcquisitionTask(void *pvParameters) {
  while (true) {
    // Captura a imagem
    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Falha ao capturar a imagem");
      continue;
    }
    
    xSemaphoreGive(imgReadySemaphore);
    
    vTaskDelay(5000 / portTICK_PERIOD_MS);  
  }
}

// Thread de envio de imagens
void imageSendTask(void *pvParameters) {
  while (true) {
    // Espera pelo semáforo de imagem pronta
    if (xSemaphoreTake(imgReadySemaphore, portMAX_DELAY) == pdTRUE) {
      if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(serverUrl);
        http.addHeader("Content-Type", "image/jpeg");

        int httpResponseCode = http.POST(fb->buf, fb->len);
        if (httpResponseCode > 0) {
          String response = http.getString();
          Serial.println(httpResponseCode);
          Serial.println(response);
          
          // Libera semáforo para thread de detecção após enviar
          xSemaphoreGive(detectionSemaphore);
        } else {
          Serial.print("Erro no envio da imagem: ");
          Serial.println(httpResponseCode);
        }

        http.end();
      } else {
        Serial.println("Erro de conexão WiFi");
      }
      
      esp_camera_fb_return(fb);  // Libera a memória da imagem após enviar
    }
  }
}

// Thread de recebimento de coordenadas de detecção
void detectionReceiveTask(void *pvParameters) {
  while (true) {
    // Espera que a imagem tenha sido enviada (sincronizado pelo semáforo)
    if (xSemaphoreTake(detectionSemaphore, portMAX_DELAY) == pdTRUE) {
      // Simulação de recebimento de coordenadas de detecção (usando Serial)
      if (Serial.available()) {
        String coords = Serial.readStringUntil('\n');
        Serial.println("Coordenadas de detecção recebidas: " + coords);
        // Processar as coordenadas conforme necessário...
      }
    }
  }
}





void setup() {
  Serial.begin(115200);
  Serial.println();
  
  // Inicializa a câmera
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_QVGA;
  config.pixel_format = PIXFORMAT_JPEG;  
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Falha ao inicializar a câmera, erro 0x%x", err);
    return;
  }

  // Conectar ao Wi-Fi
  wifiConnect();
  
  // Criar semáforos binários
  imgReadySemaphore = xSemaphoreCreateBinary();
  detectionSemaphore = xSemaphoreCreateBinary();

  // Criar as threads de FreeRTOS
  xTaskCreatePinnedToCore(imageAcquisitionTask, "Image Acquisition Task", 8192, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(imageSendTask, "Image Send Task", 8192, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(detectionReceiveTask, "Detection Receive Task", 4096, NULL, 1, NULL, 1);
}

void loop() {
  // O FreeRTOS já gerencia o loop
}
