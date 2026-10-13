#include <inttypes.h>
#include <stdint.h>

#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_heap_caps.h"
#include "esp_idf_version.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "system_info";

static void log_features(uint32_t features)
{
    ESP_LOGI(TAG, "Features: Wi-Fi=%s, Bluetooth Classic=%s, BLE=%s",
             (features & CHIP_FEATURE_WIFI_BGN) ? "yes" : "no",
             (features & CHIP_FEATURE_BT) ? "yes" : "no",
             (features & CHIP_FEATURE_BLE) ? "yes" : "no");
}

void app_main(void)
{
    esp_chip_info_t chip_info;
    uint32_t flash_size = 0;

    esp_chip_info(&chip_info);
    ESP_ERROR_CHECK(esp_flash_get_physical_size(NULL, &flash_size));

    ESP_LOGI(TAG, "ESP-IDF version: %s", esp_get_idf_version());
    ESP_LOGI(TAG, "Chip model: %s", CONFIG_IDF_TARGET);
    ESP_LOGI(TAG, "Revision: %d.%d", chip_info.revision / 100,
             chip_info.revision % 100);
    ESP_LOGI(TAG, "CPU cores: %d", chip_info.cores);
    log_features(chip_info.features);
    ESP_LOGI(TAG, "Physical flash size: %" PRIu32 " bytes (%" PRIu32 " MiB)",
             flash_size, flash_size / (1024U * 1024U));
    ESP_LOGI(TAG, "Free heap: %zu bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "Minimum free heap so far: %zu bytes",
             esp_get_minimum_free_heap_size());

    uint32_t heartbeat = 0;
    while (true) {
        ESP_LOGI(TAG, "Heartbeat %" PRIu32 " on core %d", heartbeat++,
                 xPortGetCoreID());
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
