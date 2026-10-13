#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#include "esp_attr.h"
#include "esp_bit_defs.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LIGHT_SLEEP_TIME_US (2ULL * 1000ULL * 1000ULL)
#define DEEP_SLEEP_TIME_US  (5ULL * 1000ULL * 1000ULL)

static const char *TAG = "sleep_modes";
RTC_DATA_ATTR static uint32_t deep_sleep_boot_count;

static const char *wakeup_cause_name(uint32_t causes)
{
    if (causes == 0) {
        return "power-on/reset";
    }
    if (causes & BIT(ESP_SLEEP_WAKEUP_TIMER)) {
        return "timer";
    }
    if (causes & BIT(ESP_SLEEP_WAKEUP_EXT0)) {
        return "EXT0";
    }
    if (causes & BIT(ESP_SLEEP_WAKEUP_EXT1)) {
        return "EXT1";
    }
    return "other";
}

void app_main(void)
{
    deep_sleep_boot_count++;
    int ordinary_ram_value = 41;
    const uint32_t causes = esp_sleep_get_wakeup_causes();

    ESP_LOGI(TAG, "app_main started");
    ESP_LOGI(TAG, "Wake-up cause: %s (bitmap 0x%08" PRIx32 ")",
             wakeup_cause_name(causes), causes);
    ESP_LOGI(TAG, "RTC boot counter: %" PRIu32, deep_sleep_boot_count);

    ESP_LOGI(TAG, "Light-sleep next: RAM value=%d", ordinary_ram_value);
    ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(LIGHT_SLEEP_TIME_US));
    fflush(stdout);
    ESP_ERROR_CHECK(esp_light_sleep_start());

    ordinary_ram_value++;
    ESP_LOGI(TAG, "Resumed after Light-sleep: RAM value=%d",
             ordinary_ram_value);

    ESP_ERROR_CHECK(esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL));
    ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(DEEP_SLEEP_TIME_US));

    ESP_LOGI(TAG, "Entering Deep-sleep. The CPU will restart in 5 seconds.");
    ESP_LOGI(TAG, "Ordinary RAM will be lost; RTC boot counter can survive.");
    fflush(stdout);
    vTaskDelay(pdMS_TO_TICKS(100));
    esp_deep_sleep_start();
}
