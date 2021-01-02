#include <unistd.h>
#include <stdlib.h>
#include <esp_err.h>
#include <driver/rmt.h>
#include <esp_log.h>
#include "libdmx.h"

#define TAG "libdmx"

static int dmx_channel_number;
static rmt_item32_t *dmx_data;

#define CHANNEL_OFFSET(CHAN) (1 + (CHAN) * 5)

esp_err_t dmx_init(int chan_num) {
    if (chan_num < 0 || chan_num > 513) {
        ESP_LOGE(TAG, "To many channels %d > 513", chan_num);
        return ESP_ERR_INVALID_ARG;
    }
    dmx_channel_number = chan_num;
    dmx_data = malloc((1 + chan_num * 5) * sizeof(rmt_item32_t));
    if (!dmx_data)
        return ESP_ERR_NO_MEM;

    dmx_data[0].duration0 = 100;
    dmx_data[0].level0 = 0;
    dmx_data[0].duration1 = 12;
    dmx_data[0].level1 = 1;
    for (int i = 0; i < chan_num; ++i) {
        for (int j = 0; j < 5; ++j) {
            dmx_data[CHANNEL_OFFSET(i) + j].duration0 = 4;
            dmx_data[CHANNEL_OFFSET(i) + j].duration1 = 4;
        }
        dmx_data[CHANNEL_OFFSET(i)].level0 = 0;
        dmx_data[CHANNEL_OFFSET(i) + 4].duration1 = 8;
        dmx_data[CHANNEL_OFFSET(i) + 4].level1 = 1;
    }
    dmx_set_channel_value(0, 0);

    rmt_config_t config = RMT_DEFAULT_CONFIG_TX(CONFIG_DMX_TX_GPIO, RMT_TX_CHANNEL);
    config.tx_config.idle_output_en = true;
    config.tx_config.idle_level = RMT_IDLE_LEVEL_HIGH;

    ESP_ERROR_CHECK(rmt_config(&config));
    ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0, 0));
    return ESP_OK;
}

static void dmx_set_channel_bit(int channel, int bit_num, bool level) {
    rmt_item32_t *item = &dmx_data[CHANNEL_OFFSET(channel) + (bit_num + 1) / 2];
    if (!((bit_num + 1) % 2))
        item->level0 = level;
    else
        item->level1 = level;
}

esp_err_t dmx_set_channel_value(int channel, uint8_t value) {
    if (channel < 0 || channel >= dmx_channel_number)
        return ESP_ERR_INVALID_ARG;

    for (int i = 0; i < 8; ++i)
        dmx_set_channel_bit(channel, i, value & (1 << i));

    return ESP_OK;
}

esp_err_t dmx_send(void) {
    return rmt_write_items(RMT_TX_CHANNEL, dmx_data, 1 + dmx_channel_number * 5, true);
}
