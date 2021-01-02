#ifndef LIB_DMX
#define LIB_DMX

#include <unistd.h>
#include <stdlib.h>
#include <esp_err.h>
#include <driver/rmt.h>
#include "libdmx.h"

#define RMT_TX_CHANNEL 0




/**
 * Init dmx library.
 * Make sure you convert the digital 0 - 3.3v of the ESP32 to RS-485.
 *
 * @param chan_num number of channel including channel 0 set by default to 0
 */
esp_err_t dmx_init(int chan_num);

/**
 * Set the value of a channel
 *
 * @param channel number from 0 to chan_num - 1
 * @param value target value of the channel
 *
 * @return
 *    - ESP_OK: Value was set successfully.
 *    - ESP_ERR_INVALID_ARG: channel number outside 0 - chan_num
 */
esp_err_t dmx_set_channel_value(int channel, uint8_t value);

/**
 * Send dmx data though esp32 RMT
 *
 * @return
 *    - ESP_OK: Value was set successfully.
 *    - ESP_ERR_INVALID_ARG: see rmt_write_items()
 */
esp_err_t dmx_send(void);

#endif /* LIB_DMX */
