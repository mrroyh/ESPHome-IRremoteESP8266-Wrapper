#pragma once
#include "esphome/core/component.h"
#include "esphome/components/climate/climate.h"
#include "esphome/components/remote_base/remote_transmitter.h"

namespace esphome {
namespace haier_ir {

enum class HaierVariant : uint8_t { kClassic, k160, k176, kYRW02 };

class HaierIrClimate : public Component, public climate::Climate {
 public:
  void set_transmitter(remote_base::RemoteTransmitterComponent *tx) { this->tx_ = tx; }
  void set_variant(HaierVariant v) { this->variant_ = v; }

  void setup() override {}
  climate::ClimateTraits traits() override;
  void control(const climate::ClimateCall &call) override;
  void dump_config() override;

 protected:
  void build_frame_and_send_();  // builds timings for current state and transmits via tx_
  void encode_classic_(std::vector<uint32_t> &durations);
  void encode_160_(std::vector<uint32_t> &durations);
  void encode_176_(std::vector<uint32_t> &durations);
  void encode_yrw02_(std::vector<uint32_t> &durations);

  // helpers
  void push_mark_space_(std::vector<uint32_t> &d, uint32_t mark_us, uint32_t space_us);

  remote_base::RemoteTransmitterComponent *tx_{nullptr};
  HaierVariant variant_{HaierVariant::kClassic};

  // Climate state mirror (target temp, mode, fan, swing)
  // ESPHome already keeps these; we just mirror to encode into a frame.
};

}  // namespace haier_ir
}  // namespace esphome
