#include "haier_ir.h"
#include "esphome/core/log.h"
#include <cmath>

namespace esphome {
namespace haier_ir {

static const char *const TAG = "haier_ir.climate";

climate::ClimateTraits HaierIrClimate::traits() {
  climate::ClimateTraits t;
  t.set_supported_modes({
      climate::CLIMATE_MODE_OFF,
      climate::CLIMATE_MODE_AUTO,
      climate::CLIMATE_MODE_COOL,
      climate::CLIMATE_MODE_HEAT,
      climate::CLIMATE_MODE_DRY,
      climate::CLIMATE_MODE_FAN_ONLY});
  t.set_supported_fan_modes({
      climate::CLIMATE_FAN_AUTO,
      climate::CLIMATE_FAN_LOW,
      climate::CLIMATE_FAN_MEDIUM,
      climate::CLIMATE_FAN_HIGH});
  t.set_supported_swing_modes({
      climate::CLIMATE_SWING_OFF,
      climate::CLIMATE_SWING_VERTICAL});
  t.set_visual_min_temperature(16);
  t.set_visual_max_temperature(30);
  t.set_visual_temperature_step(1.0f);
  t.set_supports_current_temperature(false);
  t.set_supports_two_point_target_temperature(false);
  return t;
}

void HaierIrClimate::dump_config() {
  ESP_LOGCONFIG(TAG, "Haier IR (remote_transmitter-backed)");
  ESP_LOGCONFIG(TAG, "  Variant: %s",
    variant_ == HaierVariant::k160 ? "160" :
    variant_ == HaierVariant::k176 ? "176" :
    variant_ == HaierVariant::kYRW02 ? "yrw02" : "classic");
}

void HaierIrClimate::control(const climate::ClimateCall &call) {
  if (call.get_mode().has_value()) this->mode = *call.get_mode();
  if (call.get_target_temperature().has_value()) this->target_temperature = *call.get_target_temperature();
  if (call.get_fan_mode().has_value()) this->fan_mode = *call.get_fan_mode();
  if (call.get_swing_mode().has_value()) this->swing_mode = *call.get_swing_mode();

  // Transmit the updated frame using remote_transmitter
  this->build_frame_and_send_();
  this->publish_state();
}

void HaierIrClimate::push_mark_space_(std::vector<uint32_t> &d, uint32_t mark_us, uint32_t space_us) {
  d.push_back(mark_us);
  d.push_back(space_us);
}

void HaierIrClimate::build_frame_and_send_() {
  if (!this->tx_) {
    ESP_LOGE(TAG, "No remote_transmitter attached");
    return;
  }
  std::vector<uint32_t> durations;
  durations.reserve(300);  // enough for full AC frame with repeats

  switch (variant_) {
    case HaierVariant::kClassic: this->encode_classic_(durations); break;
    case HaierVariant::k160:     this->encode_160_(durations);     break;
    case HaierVariant::k176:     this->encode_176_(durations);     break;
    case HaierVariant::kYRW02:   this->encode_yrw02_(durations);   break;
  }

  remote_base::RemoteTransmitData txd;
  txd.set_carrier_frequency(38000);  // Haier uses ~38 kHz
  // Assuming 50% default duty from remote_transmitter is fine.
  for (size_t i = 0; i + 1 < durations.size(); i += 2) {
    txd.mark(durations[i]);
    txd.space(durations[i + 1]);
  }
  this->tx_->transmit(txd);
}

/*********** ENCODERS ***********
 * Fill `durations` with [mark, space, mark, space, ...] in microseconds.
 * You need to plug the correct timings/bit order for each variant.
 */

void HaierIrClimate::encode_classic_(std::vector<uint32_t> &d) {
  // TODO: header mark/space
  // this->push_mark_space_(d, H_HDR_MARK, H_HDR_SPACE);

  // TODO: build state bytes based on this->mode, this->target_temperature, this->fan_mode, this->swing_mode
  // uint8_t frame[N] = {...};
  // TODO: compute checksum if required.

  // TODO: emit bits (MSB/LSB per spec). For each bit:
  //   - push data mark + 0-bit space or 1-bit space
  // Example (pseudo):
  // for each bit in frame:
  //   push_mark_space_(d, H_BIT_MARK, bit ? H_ONE_SPACE : H_ZERO_SPACE);

  // TODO: final mark (no trailing space required by ESPHome).
}

void HaierIrClimate::encode_160_(std::vector<uint32_t> &d) {
  // TODO: implement 160-bit frame timings and bit order
}

void HaierIrClimate::encode_176_(std::vector<uint32_t> &d) {
  // TODO: implement 176-bit frame timings and bit order
}

void HaierIrClimate::encode_yrw02_(std::vector<uint32_t> &d) {
  // TODO: implement YRW02 timings and bit order
}

}  // namespace haier_ir
}  // namespace esphome
