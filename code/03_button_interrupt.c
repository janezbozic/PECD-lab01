#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>

#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define BOOT_BUTTON_GPIO GPIO_NUM_0
#define DEBOUNCE_TIME_MS 200

static const char *TAG = "button_interrupt";
static TaskHandle_t button_task_handle;

static void button_isr_handler(void *argument)
{
    (void)argument;
    BaseType_t higher_priority_task_woken = pdFALSE;

    vTaskNotifyGiveFromISR(button_task_handle, &higher_priority_task_woken);
    if (higher_priority_task_woken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

static void button_task(void *argument)
{
    (void)argument;
    TickType_t last_accepted_press = 0;
    uint32_t accepted_presses = 0;

    while (true) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        const TickType_t now = xTaskGetTickCount();

        if ((now - last_accepted_press) < pdMS_TO_TICKS(DEBOUNCE_TIME_MS)) {
            continue;
        }

        last_accepted_press = now;
        accepted_presses++;
        ESP_LOGI(TAG, "BOOT press accepted: count=%" PRIu32, accepted_presses);
    }
}

void app_main(void)
{
    const gpio_config_t button_config = {
        .pin_bit_mask = 1ULL << BOOT_BUTTON_GPIO,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE,
    };

    ESP_ERROR_CHECK(gpio_config(&button_config));

    BaseType_t task_result = xTaskCreate(
        button_task, "button_task", 3072, NULL, 10,
        &button_task_handle);
    if (task_result != pdPASS) {
        ESP_LOGE(TAG, "Failed to create button task");
        abort();
    }

    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    ESP_ERROR_CHECK(gpio_isr_handler_add(
        BOOT_BUTTON_GPIO, button_isr_handler, NULL));

    ESP_LOGI(TAG, "Ready. Briefly press BOOT; do not hold it while resetting.");
}
