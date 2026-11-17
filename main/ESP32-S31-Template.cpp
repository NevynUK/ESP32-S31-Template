#include <cinttypes>
#include <cstdint>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#if CONFIG_TEMPLATE_STATUS_LED
    #include "led_strip.h"
#endif

static const char *TAG = "app";

#if CONFIG_TEMPLATE_STATUS_LED

static led_strip_handle_t status_led_init()
{
    led_strip_config_t strip_config = {};
    strip_config.strip_gpio_num = CONFIG_TEMPLATE_STATUS_LED_GPIO;
    strip_config.max_leds = 1;
    strip_config.led_model = LED_MODEL_WS2812;
    strip_config.color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB;

    led_strip_rmt_config_t rmt_config = {};
    rmt_config.clk_src = RMT_CLK_SRC_DEFAULT;
    rmt_config.resolution_hz = 10 * 1000 * 1000; // 10 MHz: 0.1 us per tick

    led_strip_handle_t strip = nullptr;
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &strip));
    ESP_ERROR_CHECK(led_strip_clear(strip));
    return strip;
}

#endif

extern "C" void app_main(void)
{
#if CONFIG_TEMPLATE_STATUS_LED
    // Walk the hue round the colour wheel at full saturation.  Every colour
    // appearing in order (red -> yellow -> green -> cyan -> blue -> magenta)
    // shows that all three channels work and the GRB colour order is right;
    // a wrong order shows up as the sequence running backwards or skipping.
    //
    // The hue is derived from elapsed time rather than stepped per update, so
    // the cycle time stays correct whatever the FreeRTOS tick rate is.
    constexpr uint32_t CYCLE_MS = CONFIG_TEMPLATE_STATUS_LED_CYCLE_MS;
    constexpr uint32_t UPDATE_MS = 20; // ~50 updates per second

    ESP_LOGI(TAG, "Colour wheel on WS2812, GPIO %d, %" PRIu32 " ms per cycle", CONFIG_TEMPLATE_STATUS_LED_GPIO, CYCLE_MS);
    led_strip_handle_t led = status_led_init();
    const TickType_t start = xTaskGetTickCount();
    TickType_t last_wake = start;
    const TickType_t update_ticks = pdMS_TO_TICKS(UPDATE_MS) > 0 ? pdMS_TO_TICKS(UPDATE_MS) : 1;

    while (true)
    {
        const uint32_t elapsed_ms = (xTaskGetTickCount() - start) * portTICK_PERIOD_MS;
        const uint16_t hue = static_cast<uint16_t>((elapsed_ms % CYCLE_MS) * 360 / CYCLE_MS);
        ESP_ERROR_CHECK(led_strip_set_pixel_hsv(led, 0, hue, 255, CONFIG_TEMPLATE_STATUS_LED_BRIGHTNESS));
        ESP_ERROR_CHECK(led_strip_refresh(led));
        xTaskDelayUntil(&last_wake, update_ticks);
    }
#else
    ESP_LOGI(TAG, "Status LED disabled (menuconfig: Template Application)");
#endif
}
