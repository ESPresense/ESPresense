#include "SinglePWM.h"

#include "esp_private/periph_ctrl.h"
#include "soc/soc_caps.h"

#include <cmath>

#include "driver/ledc.h"

// One shared 5 kHz / 12-bit low-speed timer; each LED index owns LEDC channel <index>.
#define PWM_TIMER LEDC_TIMER_0
// ESP32: high-speed channels on APB, as the Arduino core used. Its low-speed mode's
// duty_start busy-wait (ledc_ll_set_duty_start) stalled for good under the per-advert LED
// toggling in HIL 1274 -> Interrupt WDT. Later chips only have low-speed mode.
#if SOC_LEDC_SUPPORT_HS_MODE
#define PWM_MODE LEDC_HIGH_SPEED_MODE
#define PWM_CLK LEDC_USE_APB_CLK
#else
#define PWM_MODE LEDC_LOW_SPEED_MODE
#define PWM_CLK LEDC_AUTO_CLK
#endif

SinglePWM::SinglePWM(uint8_t index, ControlType controlType, bool inverted, int pin) : LED(index, controlType) {
    this->inverted = inverted;
    this->pin = pin;
}

void SinglePWM::init() {
    // After a software reset the ESP32 keeps the LEDC channel's duty_start bit set with its
    // timer stopped, and ledc_ll_set_duty_start() spins on that bit forever inside a critical
    // section (Interrupt WDT in HIL 1274). Start from a clean peripheral.
    static bool reset = false;
    if (!reset) {
        reset = true;
        periph_module_reset(PERIPH_LEDC_MODULE);
    }
    ledc_timer_config_t timer = {};
    timer.speed_mode = PWM_MODE;
    timer.duty_resolution = LEDC_TIMER_12_BIT;
    timer.timer_num = PWM_TIMER;
    timer.freq_hz = 5000;
    timer.clk_cfg = PWM_CLK;
    if (ledc_timer_config(&timer) != ESP_OK) return;

    ledc_channel_config_t channel = {};
    channel.gpio_num = pin;
    channel.speed_mode = PWM_MODE;
    channel.channel = (ledc_channel_t)LED::getIndex();
    channel.intr_type = LEDC_INTR_DISABLE;
    channel.timer_sel = PWM_TIMER;
    channel.duty = 0;
    channel.hpoint = 0;
    inited = ledc_channel_config(&channel) == ESP_OK;
}

void SinglePWM::update() {
    setDuty(LED::getState() ? LED::getBrightness() : 0);
}

void SinglePWM::setDuty(uint32_t x) {
    if (!inited) init();
    if (!inited) return;
    uint32_t duty = x >= 255 ? 4096 : (x <= 0 ? 0 : (uint32_t)round(4096.0 * pow(10.0, 0.0055 * (x - 255.0))));
    if (inverted) duty = 4096 - duty;
    ledc_set_duty(PWM_MODE, (ledc_channel_t)LED::getIndex(), duty);
    ledc_update_duty(PWM_MODE, (ledc_channel_t)LED::getIndex());
}

void SinglePWM::service() {
}
