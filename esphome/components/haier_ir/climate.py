import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate, remote_transmitter
from esphome.const import CONF_ID

haier_ir_ns = cg.esphome_ns.namespace("haier_ir")
HaierIrClimate = haier_ir_ns.class_("HaierIrClimate", cg.Component, climate.Climate)

CONF_TRANSMITTER_ID = "transmitter_id"
CONF_VARIANT = "variant"
VARIANTS = ["classic", "160", "176", "yrw02"]

CONFIG_SCHEMA = climate.CLIMATE_SCHEMA.extend(
    {
        cv.GenerateID(CONF_ID): cv.declare_id(HaierIrClimate),
        cv.Required(CONF_TRANSMITTER_ID): cv.use_id(remote_transmitter.RemoteTransmitterComponent),
        cv.Optional(CONF_VARIANT, default="classic"): cv.one_of(*VARIANTS, lower=True),
    }
).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await climate.register_climate(var, config)

    tx = await cg.get_variable(config[CONF_TRANSMITTER_ID])
    cg.add(var.set_transmitter(tx))

    variant_enum = haier_ir_ns.enum("HaierVariant")
    mapping = {
        "classic": variant_enum.kClassic,
        "160":     variant_enum.k160,
        "176":     variant_enum.k176,
        "yrw02":   variant_enum.kYRW02,
    }
    cg.add(var.set_variant(mapping[config[CONF_VARIANT]]))
