#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "dual_core";

typedef struct {
    const char *name;
    TickType_t period;
} task_config_t;

static const task_config_t task_a_config = {
    .name = "Task A",
    .period = pdMS_TO_TICKS(700),
};

static const task_config_t task_b_config = {
    .name = "Task B",
    .period = pdMS_TO_TICKS(1100),
};

static void periodic_task(void *argument)
{
    const task_config_t *config = (const task_config_t *)argument;
    uint32_t iteration = 0;

    while (true) {
        ESP_LOGI(TAG, "%s: iteration=%" PRIu32 ", core=%d",
                 config->name, iteration++, xPortGetCoreID());
        vTaskDelay(config->period);
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "Creating one pinned task on each CPU core");

    BaseType_t result_a = xTaskCreatePinnedToCore(
        periodic_task, "task_a", 3072, (void *)&task_a_config,
        5, NULL, 0);
    BaseType_t result_b = xTaskCreatePinnedToCore(
        periodic_task, "task_b", 3072, (void *)&task_b_config,
        5, NULL, 1);

    if (result_a != pdPASS || result_b != pdPASS) {
        ESP_LOGE(TAG, "Failed to create both tasks");
        abort();
    }

    ESP_LOGI(TAG, "app_main is finished; the two tasks continue running");
}
