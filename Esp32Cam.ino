//Esp32Cam.ino

#include "esp_camera.h"
#include <WiFi.h>
#include "esp_http_server.h"  // Required for the HTTP server

#define CAMERA_MODEL_AI_THINKER  // Camera model
#include "camera_pins.h"
#include "InternetDaten.h"


const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

// Function to Start the Camera Server
void startCameraServer() {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 80;

    httpd_handle_t server = NULL;
    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t stream_uri = {
            .uri = "/stream",
            .method = HTTP_GET,
            .handler = [](httpd_req_t* req) -> esp_err_t {
                camera_fb_t* fb = NULL;
                esp_err_t res = ESP_OK;

                static const char* stream_content_type = "multipart/x-mixed-replace; boundary=frame";
                static const char* frame_boundary = "\r\n--frame\r\n";
                static const char* frame_content_type = "Content-Type: image/jpeg\r\n\r\n";

                res = httpd_resp_set_type(req, stream_content_type);
                if (res != ESP_OK) {
                    return res;
                }

                while (true) {
                    fb = esp_camera_fb_get();
                    if (!fb) {
                        Serial.println("Camera capture failed");
                        res = ESP_FAIL;
                    } else {
                        res = httpd_resp_send_chunk(req, frame_boundary, strlen(frame_boundary));
                        if (res == ESP_OK) {
                            res = httpd_resp_send_chunk(req, frame_content_type, strlen(frame_content_type));
                        }
                        if (res == ESP_OK) {
                            res = httpd_resp_send_chunk(req, (const char*)fb->buf, fb->len);
                        }
                        esp_camera_fb_return(fb);
                    }
                    if (res != ESP_OK) {
                        break;
                    }
                    delay(50);  // Small delay to prevent overloading
                }
                return res;
            },
            .user_ctx = NULL
        };

        httpd_register_uri_handler(server, &stream_uri);
        Serial.println("HTTP server started for streaming");
    } else {
        Serial.println("Failed to start HTTP server");
    }
}

void setup() {
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println("Initializing...");

    // Configure the Camera
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
    config.xclk_freq_hz = 20000000;       // Stable clock speed
    config.pixel_format = PIXFORMAT_JPEG; // Use JPEG for smaller file sizes
    config.frame_size = FRAMESIZE_QVGA;   // Lower resolution for faster performance
    config.jpeg_quality = 10;             // Balanced quality and speed
    config.fb_location = CAMERA_FB_IN_PSRAM; // Use PSRAM for frame buffer
    config.fb_count = 2;                  // Double buffering for smooth streaming
    config.grab_mode = CAMERA_GRAB_WHEN_EMPTY; // Avoid frame skipping

    // Initialize the Camera
    if (psramFound()) {
        config.jpeg_quality = 8;          // Adjust quality when PSRAM is available
        config.fb_count = 3;              // Use triple buffering for better performance
    } else {
        config.frame_size = FRAMESIZE_SVGA; // Fallback for non-PSRAM
        config.fb_location = CAMERA_FB_IN_DRAM;
    }

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Camera init failed with error 0x%x\n", err);
        return;
    }

    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");

    // Start the Camera Server
    startCameraServer();

    Serial.print("Camera Ready! Stream at: http://");
    Serial.print(WiFi.localIP());
    Serial.println("/stream");
}

void loop() {
    delay(10); // Minimal delay to prevent CPU overloading
}