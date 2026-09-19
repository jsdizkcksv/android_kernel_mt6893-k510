// SPDX-License-Identifier: GPL-2.0-only
/*
 *  Sysfs interface for the universal power supply monitor class
 *
 *  Copyright © 2007  David Woodhouse <dwmw2@infradead.org>
 *  Copyright © 2007  Anton Vorontsov <cbou@mail.ru>
 *  Copyright © 2004  Szabolcs Gyurko
 *  Copyright © 2003  Ian Molton <spyro@f2s.com>
 *
 *  Modified: 2004, Oct     Szabolcs Gyurko
 */

#include <linux/ctype.h>
#include <linux/device.h>
#include <linux/power_supply.h>
#include <linux/slab.h>
#include <linux/stat.h>

#include "power_supply.h"

#define MAX_PROP_NAME_LEN 30

struct power_supply_attr {
	const char *prop_name;
	char attr_name[MAX_PROP_NAME_LEN + 1];
	struct device_attribute dev_attr;
	const char * const *text_values;
	int text_values_len;
};

#define _POWER_SUPPLY_ATTR(_name, _text, _len)	\
[POWER_SUPPLY_PROP_ ## _name] =			\
{						\
	.prop_name = #_name,			\
	.attr_name = #_name "\0",		\
	.text_values = _text,			\
	.text_values_len = _len,		\
}

#define POWER_SUPPLY_ATTR(_name) _POWER_SUPPLY_ATTR(_name, NULL, 0)
#define _POWER_SUPPLY_ENUM_ATTR(_name, _text)	\
	_POWER_SUPPLY_ATTR(_name, _text, ARRAY_SIZE(_text))
#define POWER_SUPPLY_ENUM_ATTR(_name)	\
	_POWER_SUPPLY_ENUM_ATTR(_name, POWER_SUPPLY_ ## _name ## _TEXT)

static const char * const POWER_SUPPLY_TYPE_TEXT[] = {
	[POWER_SUPPLY_TYPE_UNKNOWN]		= "Unknown",
	[POWER_SUPPLY_TYPE_BATTERY]		= "Battery",
	[POWER_SUPPLY_TYPE_UPS]			= "UPS",
	[POWER_SUPPLY_TYPE_MAINS]		= "Mains",
	[POWER_SUPPLY_TYPE_USB]			= "USB",
	[POWER_SUPPLY_TYPE_USB_DCP]		= "USB_DCP",
	[POWER_SUPPLY_TYPE_USB_CDP]		= "USB_CDP",
	[POWER_SUPPLY_TYPE_USB_ACA]		= "USB_ACA",
	[POWER_SUPPLY_TYPE_USB_TYPE_C]		= "USB_C",
	[POWER_SUPPLY_TYPE_USB_PD]		= "USB_PD",
	[POWER_SUPPLY_TYPE_USB_PD_DRP]		= "USB_PD_DRP",
	[POWER_SUPPLY_TYPE_APPLE_BRICK_ID]	= "BrickID",
	[POWER_SUPPLY_TYPE_WIRELESS]		= "Wireless",
static const char * const power_supply_type_text[] = {
#ifdef CONFIG_PISSARRO_CHARGER
	"Unknown", "Battery", "UPS", "Mains", "USB", "USB_FLOAT",
	"USB_DCP", "USB_CDP", "USB_ACA", "USB_C", "USB_PD",
	"USB_PD_DRP", "BrickID", "USB_HVDCP", "USB_HVDCP_3",
	"USB_HVDCP_3P5", "Wireless"
#else
	"Unknown", "Battery", "UPS", "Mains", "USB","USB_FLOAT",
	"USB_DCP", "USB_CDP", "USB_ACA", "USB_C",
	"USB_PD", "USB_PD_DRP", "BrickID",
	"USB_HVDCP", "USB_HVDCP_3", "USB_HVDCP_3P5", "Wireless",
	"Bms", "Charger_Identify", "Batt_verify"
#endif
};

static const char * const POWER_SUPPLY_USB_TYPE_TEXT[] = {
	[POWER_SUPPLY_USB_TYPE_UNKNOWN]		= "Unknown",
	[POWER_SUPPLY_USB_TYPE_SDP]		= "SDP",
	[POWER_SUPPLY_USB_TYPE_DCP]		= "DCP",
	[POWER_SUPPLY_USB_TYPE_CDP]		= "CDP",
	[POWER_SUPPLY_USB_TYPE_ACA]		= "ACA",
	[POWER_SUPPLY_USB_TYPE_C]		= "C",
	[POWER_SUPPLY_USB_TYPE_PD]		= "PD",
	[POWER_SUPPLY_USB_TYPE_PD_DRP]		= "PD_DRP",
	[POWER_SUPPLY_USB_TYPE_PD_PPS]		= "PD_PPS",
	[POWER_SUPPLY_USB_TYPE_APPLE_BRICK_ID]	= "BrickID",
};

static const char * const POWER_SUPPLY_STATUS_TEXT[] = {
	[POWER_SUPPLY_STATUS_UNKNOWN]		= "Unknown",
	[POWER_SUPPLY_STATUS_CHARGING]		= "Charging",
	[POWER_SUPPLY_STATUS_DISCHARGING]	= "Discharging",
	[POWER_SUPPLY_STATUS_NOT_CHARGING]	= "Not charging",
	[POWER_SUPPLY_STATUS_FULL]		= "Full",
};

static const char * const POWER_SUPPLY_CHARGE_TYPE_TEXT[] = {
	[POWER_SUPPLY_CHARGE_TYPE_UNKNOWN]	= "Unknown",
	[POWER_SUPPLY_CHARGE_TYPE_NONE]		= "N/A",
	[POWER_SUPPLY_CHARGE_TYPE_TRICKLE]	= "Trickle",
	[POWER_SUPPLY_CHARGE_TYPE_FAST]		= "Fast",
	[POWER_SUPPLY_CHARGE_TYPE_STANDARD]	= "Standard",
	[POWER_SUPPLY_CHARGE_TYPE_ADAPTIVE]	= "Adaptive",
	[POWER_SUPPLY_CHARGE_TYPE_CUSTOM]	= "Custom",
	[POWER_SUPPLY_CHARGE_TYPE_LONGLIFE]	= "Long Life",
	[POWER_SUPPLY_CHARGE_TYPE_TAPER]	= "Taper",
};

static const char * const POWER_SUPPLY_HEALTH_TEXT[] = {
	[POWER_SUPPLY_HEALTH_UNKNOWN]		    = "Unknown",
	[POWER_SUPPLY_HEALTH_GOOD]		    = "Good",
	[POWER_SUPPLY_HEALTH_OVERHEAT]		    = "Overheat",
	[POWER_SUPPLY_HEALTH_DEAD]		    = "Dead",
	[POWER_SUPPLY_HEALTH_OVERVOLTAGE]	    = "Over voltage",
	[POWER_SUPPLY_HEALTH_UNSPEC_FAILURE]	    = "Unspecified failure",
	[POWER_SUPPLY_HEALTH_COLD]		    = "Cold",
	[POWER_SUPPLY_HEALTH_WATCHDOG_TIMER_EXPIRE] = "Watchdog timer expire",
	[POWER_SUPPLY_HEALTH_SAFETY_TIMER_EXPIRE]   = "Safety timer expire",
	[POWER_SUPPLY_HEALTH_OVERCURRENT]	    = "Over current",
	[POWER_SUPPLY_HEALTH_CALIBRATION_REQUIRED]  = "Calibration required",
	[POWER_SUPPLY_HEALTH_WARM]		    = "Warm",
	[POWER_SUPPLY_HEALTH_COOL]		    = "Cool",
	[POWER_SUPPLY_HEALTH_HOT]		    = "Hot",
};

static const char * const POWER_SUPPLY_TECHNOLOGY_TEXT[] = {
	[POWER_SUPPLY_TECHNOLOGY_UNKNOWN]	= "Unknown",
	[POWER_SUPPLY_TECHNOLOGY_NiMH]		= "NiMH",
	[POWER_SUPPLY_TECHNOLOGY_LION]		= "Li-ion",
	[POWER_SUPPLY_TECHNOLOGY_LIPO]		= "Li-poly",
	[POWER_SUPPLY_TECHNOLOGY_LiFe]		= "LiFe",
	[POWER_SUPPLY_TECHNOLOGY_NiCd]		= "NiCd",
	[POWER_SUPPLY_TECHNOLOGY_LiMn]		= "LiMn",
};

static const char * const POWER_SUPPLY_CAPACITY_LEVEL_TEXT[] = {
	[POWER_SUPPLY_CAPACITY_LEVEL_UNKNOWN]	= "Unknown",
	[POWER_SUPPLY_CAPACITY_LEVEL_CRITICAL]	= "Critical",
	[POWER_SUPPLY_CAPACITY_LEVEL_LOW]	= "Low",
	[POWER_SUPPLY_CAPACITY_LEVEL_NORMAL]	= "Normal",
	[POWER_SUPPLY_CAPACITY_LEVEL_HIGH]	= "High",
	[POWER_SUPPLY_CAPACITY_LEVEL_FULL]	= "Full",
};

static const char * const POWER_SUPPLY_SCOPE_TEXT[] = {
	[POWER_SUPPLY_SCOPE_UNKNOWN]	= "Unknown",
	[POWER_SUPPLY_SCOPE_SYSTEM]	= "System",
	[POWER_SUPPLY_SCOPE_DEVICE]	= "Device",
};

static struct power_supply_attr power_supply_attrs[] = {
	/* Properties of type `int' */
	POWER_SUPPLY_ENUM_ATTR(STATUS),
	POWER_SUPPLY_ENUM_ATTR(CHARGE_TYPE),
	POWER_SUPPLY_ENUM_ATTR(HEALTH),
	POWER_SUPPLY_ATTR(PRESENT),
	POWER_SUPPLY_ATTR(ONLINE),
	POWER_SUPPLY_ATTR(AUTHENTIC),
	POWER_SUPPLY_ENUM_ATTR(TECHNOLOGY),
	POWER_SUPPLY_ATTR(CYCLE_COUNT),
	POWER_SUPPLY_ATTR(VOLTAGE_MAX),
	POWER_SUPPLY_ATTR(VOLTAGE_MIN),
	POWER_SUPPLY_ATTR(VOLTAGE_MAX_DESIGN),
	POWER_SUPPLY_ATTR(VOLTAGE_MIN_DESIGN),
	POWER_SUPPLY_ATTR(VOLTAGE_NOW),
	POWER_SUPPLY_ATTR(VOLTAGE_AVG),
	POWER_SUPPLY_ATTR(VOLTAGE_OCV),
	POWER_SUPPLY_ATTR(VOLTAGE_BOOT),
	POWER_SUPPLY_ATTR(CURRENT_MAX),
	POWER_SUPPLY_ATTR(CURRENT_NOW),
	POWER_SUPPLY_ATTR(CURRENT_AVG),
	POWER_SUPPLY_ATTR(CURRENT_BOOT),
	POWER_SUPPLY_ATTR(POWER_NOW),
	POWER_SUPPLY_ATTR(POWER_AVG),
	POWER_SUPPLY_ATTR(CHARGE_FULL_DESIGN),
	POWER_SUPPLY_ATTR(CHARGE_EMPTY_DESIGN),
	POWER_SUPPLY_ATTR(CHARGE_FULL),
	POWER_SUPPLY_ATTR(CHARGE_EMPTY),
	POWER_SUPPLY_ATTR(CHARGE_NOW),
	POWER_SUPPLY_ATTR(CHARGE_AVG),
	POWER_SUPPLY_ATTR(CHARGE_COUNTER),
	POWER_SUPPLY_ATTR(CONSTANT_CHARGE_CURRENT),
	POWER_SUPPLY_ATTR(CONSTANT_CHARGE_CURRENT_MAX),
	POWER_SUPPLY_ATTR(CONSTANT_CHARGE_VOLTAGE),
	POWER_SUPPLY_ATTR(CONSTANT_CHARGE_VOLTAGE_MAX),
	POWER_SUPPLY_ATTR(CHARGE_CONTROL_LIMIT),
	POWER_SUPPLY_ATTR(CHARGE_CONTROL_LIMIT_MAX),
	POWER_SUPPLY_ATTR(CHARGE_CONTROL_START_THRESHOLD),
	POWER_SUPPLY_ATTR(CHARGE_CONTROL_END_THRESHOLD),
	POWER_SUPPLY_ATTR(INPUT_CURRENT_LIMIT),
	POWER_SUPPLY_ATTR(INPUT_VOLTAGE_LIMIT),
	POWER_SUPPLY_ATTR(INPUT_POWER_LIMIT),
	POWER_SUPPLY_ATTR(ENERGY_FULL_DESIGN),
	POWER_SUPPLY_ATTR(ENERGY_EMPTY_DESIGN),
	POWER_SUPPLY_ATTR(ENERGY_FULL),
	POWER_SUPPLY_ATTR(ENERGY_EMPTY),
	POWER_SUPPLY_ATTR(ENERGY_NOW),
	POWER_SUPPLY_ATTR(ENERGY_AVG),
	POWER_SUPPLY_ATTR(CAPACITY),
	POWER_SUPPLY_ATTR(CAPACITY_ALERT_MIN),
	POWER_SUPPLY_ATTR(CAPACITY_ALERT_MAX),
	POWER_SUPPLY_ATTR(CAPACITY_ERROR_MARGIN),
	POWER_SUPPLY_ENUM_ATTR(CAPACITY_LEVEL),
	POWER_SUPPLY_ATTR(TEMP),
	POWER_SUPPLY_ATTR(TEMP_MAX),
	POWER_SUPPLY_ATTR(TEMP_MIN),
	POWER_SUPPLY_ATTR(TEMP_ALERT_MIN),
	POWER_SUPPLY_ATTR(TEMP_ALERT_MAX),
	POWER_SUPPLY_ATTR(TEMP_AMBIENT),
	POWER_SUPPLY_ATTR(TEMP_AMBIENT_ALERT_MIN),
	POWER_SUPPLY_ATTR(TEMP_AMBIENT_ALERT_MAX),
	POWER_SUPPLY_ATTR(TIME_TO_EMPTY_NOW),
	POWER_SUPPLY_ATTR(TIME_TO_EMPTY_AVG),
	POWER_SUPPLY_ATTR(TIME_TO_FULL_NOW),
	POWER_SUPPLY_ATTR(TIME_TO_FULL_AVG),
	POWER_SUPPLY_ENUM_ATTR(TYPE),
	POWER_SUPPLY_ATTR(USB_TYPE),
	POWER_SUPPLY_ENUM_ATTR(SCOPE),
	POWER_SUPPLY_ATTR(PRECHARGE_CURRENT),
	POWER_SUPPLY_ATTR(CHARGE_TERM_CURRENT),
	POWER_SUPPLY_ATTR(CALIBRATE),
	POWER_SUPPLY_ATTR(MANUFACTURE_YEAR),
	POWER_SUPPLY_ATTR(MANUFACTURE_MONTH),
	POWER_SUPPLY_ATTR(MANUFACTURE_DAY),
	/* Properties of type `const char *' */
	POWER_SUPPLY_ATTR(MODEL_NAME),
	POWER_SUPPLY_ATTR(MANUFACTURER),
	POWER_SUPPLY_ATTR(SERIAL_NUMBER),
};

static struct attribute *
__power_supply_attrs[ARRAY_SIZE(power_supply_attrs) + 1];
#ifndef CONFIG_PISSARRO_CHARGER
static const char * const power_supply_usbc_pr_text[] = {
	"Unknown", "sink only", "source only", "dual-role ports",
	"try source", "try sink", "wrong role"
};
#endif

static struct power_supply_attr *to_ps_attr(struct device_attribute *attr)
{
	return container_of(attr, struct power_supply_attr, dev_attr);
}

static enum power_supply_property dev_attr_psp(struct device_attribute *attr)
{
	return  to_ps_attr(attr) - power_supply_attrs;
}

static ssize_t power_supply_show_usb_type(struct device *dev,
					  const struct power_supply_desc *desc,
					  union power_supply_propval *value,
					  char *buf)
{
	enum power_supply_usb_type usb_type;
	ssize_t count = 0;
	bool match = false;
	int i;

	for (i = 0; i < desc->num_usb_types; ++i) {
		usb_type = desc->usb_types[i];

		if (value->intval == usb_type) {
			count += sprintf(buf + count, "[%s] ",
					 POWER_SUPPLY_USB_TYPE_TEXT[usb_type]);
			match = true;
		} else {
			count += sprintf(buf + count, "%s ",
					 POWER_SUPPLY_USB_TYPE_TEXT[usb_type]);
		}
	}

	if (!match) {
		dev_warn(dev, "driver reporting unsupported connected type\n");
		return -EINVAL;
	}

	if (count)
		buf[count - 1] = '\n';

	return count;
}

static ssize_t power_supply_show_property(struct device *dev,
					  struct device_attribute *attr,
					  char *buf) {
	ssize_t ret;
	struct power_supply *psy = dev_get_drvdata(dev);
	struct power_supply_attr *ps_attr = to_ps_attr(attr);
	enum power_supply_property psp = dev_attr_psp(attr);
	union power_supply_propval value;

	if (psp == POWER_SUPPLY_PROP_TYPE) {
		value.intval = psy->desc->type;
	} else {
		ret = power_supply_get_property(psy, psp, &value);

		if (ret < 0) {
			if (ret == -ENODATA)
				dev_dbg_ratelimited(dev,
					"driver has no data for `%s' property\n",
					attr->attr.name);
			else if (ret != -ENODEV && ret != -EAGAIN)
				dev_err_ratelimited(dev,
					"driver failed to report `%s' property: %zd\n",
					attr->attr.name, ret);
			return ret;
		}
	}

	if (ps_attr->text_values_len > 0 &&
	    value.intval < ps_attr->text_values_len && value.intval >= 0) {
		return sprintf(buf, "%s\n", ps_attr->text_values[value.intval]);
	}

	switch (psp) {
	case POWER_SUPPLY_PROP_USB_TYPE:
		ret = power_supply_show_usb_type(dev, psy->desc,
						&value, buf);
		break;
	case POWER_SUPPLY_PROP_SCOPE:
		ret = sprintf(buf, "%s\n",
			      power_supply_scope_text[value.intval]);
		break;
	case POWER_SUPPLY_PROP_TYPEC_MODE:
		ret = sprintf(buf, "%s\n",
			      power_supply_usbc_text[value.intval]);
		break;
	case POWER_SUPPLY_PROP_TYPEC_POWER_ROLE:
		ret = sprintf(buf, "%s\n",
			      power_supply_usbc_pr_text[value.intval]);
		break;
	case POWER_SUPPLY_PROP_TYPEC_SRC_RP:
		ret = sprintf(buf, "%s\n",
			      power_supply_typec_src_rp_text[value.intval]);
		break;
#ifdef CONFIG_XMUSB350_DET_CHG
	case POWER_SUPPLY_PROP_QC35_VID:
		ret = scnprintf(buf, PAGE_SIZE, "%02x%02x%02x%02x\n",
			       value.arrayval[0], value.arrayval[1],
			       value.arrayval[2], value.arrayval[3]);
		break;
	case POWER_SUPPLY_PROP_QC35_VERSION:
		ret = scnprintf(buf, PAGE_SIZE, "%02x%02x\n",
			       value.arrayval[0], value.arrayval[1]);
		break;
#endif
	case POWER_SUPPLY_PROP_DIE_HEALTH:
	case POWER_SUPPLY_PROP_SKIN_HEALTH:
	case POWER_SUPPLY_PROP_CONNECTOR_HEALTH:
		ret = sprintf(buf, "%s\n",
			      power_supply_health_text[value.intval]);
		break;
	case POWER_SUPPLY_PROP_CHARGE_COUNTER_EXT:
		ret = sprintf(buf, "%lld\n", value.int64val);
		break;
	case POWER_SUPPLY_PROP_MODEL_NAME ... POWER_SUPPLY_PROP_SERIAL_NUMBER:
		ret = sprintf(buf, "%s\n", value.strval);
		break;
	default:
		ret = sprintf(buf, "%d\n", value.intval);
	}
	return ret;
}

static ssize_t power_supply_store_property(struct device *dev,
					   struct device_attribute *attr,
					   const char *buf, size_t count) {
	ssize_t ret;
	struct power_supply *psy = dev_get_drvdata(dev);
	struct power_supply_attr *ps_attr = to_ps_attr(attr);
	enum power_supply_property psp = dev_attr_psp(attr);
	union power_supply_propval value;

	ret = -EINVAL;
	if (ps_attr->text_values_len > 0) {
		ret = __sysfs_match_string(ps_attr->text_values,
					   ps_attr->text_values_len, buf);
	}

	/*
	 * If no match was found, then check to see if it is an integer.
	 * Integer values are valid for enums in addition to the text value.
	 */
	if (ret < 0) {
		long long_val;

		ret = kstrtol(buf, 10, &long_val);
		if (ret < 0)
			return ret;

		ret = long_val;
	}

	value.intval = ret;

	ret = power_supply_set_property(psy, psp, &value);
	if (ret < 0)
		return ret;

	return count;
}

/* Must be in the same order as POWER_SUPPLY_PROP_* */
static struct device_attribute power_supply_attrs[] = {
#ifdef CONFIG_PISSARRO_CHARGER
	/* Properties of type `int' */
	POWER_SUPPLY_ATTR(status),
	POWER_SUPPLY_ATTR(charge_type),
	POWER_SUPPLY_ATTR(health),
	POWER_SUPPLY_ATTR(present),
	POWER_SUPPLY_ATTR(online),
	POWER_SUPPLY_ATTR(authentic),
	POWER_SUPPLY_ATTR(technology),
	POWER_SUPPLY_ATTR(cycle_count),
	POWER_SUPPLY_ATTR(voltage_max),
	POWER_SUPPLY_ATTR(voltage_min),
	POWER_SUPPLY_ATTR(voltage_max_design),
	POWER_SUPPLY_ATTR(voltage_min_design),
	POWER_SUPPLY_ATTR(voltage_now),
	POWER_SUPPLY_ATTR(voltage_avg),
	POWER_SUPPLY_ATTR(voltage_ocv),
	POWER_SUPPLY_ATTR(voltage_boot),
	POWER_SUPPLY_ATTR(current_max),
	POWER_SUPPLY_ATTR(current_now),
	POWER_SUPPLY_ATTR(input_current_now),
	POWER_SUPPLY_ATTR(current_avg),
	POWER_SUPPLY_ATTR(current_boot),
	POWER_SUPPLY_ATTR(power_now),
	POWER_SUPPLY_ATTR(power_avg),
	POWER_SUPPLY_ATTR(charge_full_design),
	POWER_SUPPLY_ATTR(charge_empty_design),
	POWER_SUPPLY_ATTR(charge_full),
	POWER_SUPPLY_ATTR(charge_empty),
	POWER_SUPPLY_ATTR(charge_now),
	POWER_SUPPLY_ATTR(charge_avg),
	POWER_SUPPLY_ATTR(charge_counter),
	POWER_SUPPLY_ATTR(constant_charge_current),
	POWER_SUPPLY_ATTR(constant_charge_current_max),
	POWER_SUPPLY_ATTR(constant_charge_voltage),
	POWER_SUPPLY_ATTR(constant_charge_voltage_max),
	POWER_SUPPLY_ATTR(charge_control_scene),
	POWER_SUPPLY_ATTR(charge_control_limit),
	POWER_SUPPLY_ATTR(charge_control_limit_max),
	POWER_SUPPLY_ATTR(fast_charge_current),
	POWER_SUPPLY_ATTR(thermal_input_current),
	POWER_SUPPLY_ATTR(input_current_now),
	POWER_SUPPLY_ATTR(input_current_limit),
	POWER_SUPPLY_ATTR(energy_full_design),
	POWER_SUPPLY_ATTR(energy_empty_design),
	POWER_SUPPLY_ATTR(energy_full),
	POWER_SUPPLY_ATTR(energy_empty),
	POWER_SUPPLY_ATTR(energy_now),
	POWER_SUPPLY_ATTR(energy_avg),
	POWER_SUPPLY_ATTR(capacity),
	POWER_SUPPLY_ATTR(capacity_alert_min),
	POWER_SUPPLY_ATTR(capacity_alert_max),
	POWER_SUPPLY_ATTR(capacity_level),
	POWER_SUPPLY_ATTR(soc_decimal),
	POWER_SUPPLY_ATTR(soc_decimal_rate),
	POWER_SUPPLY_ATTR(capacity_raw),
	POWER_SUPPLY_ATTR(temp),
	POWER_SUPPLY_ATTR(temp_max),
	POWER_SUPPLY_ATTR(temp_min),
	POWER_SUPPLY_ATTR(temp_connect),
	POWER_SUPPLY_ATTR(temp_alert_min),
	POWER_SUPPLY_ATTR(temp_alert_max),
	POWER_SUPPLY_ATTR(temp_ambient),
	POWER_SUPPLY_ATTR(temp_ambient_alert_min),
	POWER_SUPPLY_ATTR(temp_ambient_alert_max),
	POWER_SUPPLY_ATTR(time_to_empty_now),
	POWER_SUPPLY_ATTR(time_to_empty_avg),
	POWER_SUPPLY_ATTR(time_to_full_now),
	POWER_SUPPLY_ATTR(time_to_full_avg),
	POWER_SUPPLY_ATTR(type),
	POWER_SUPPLY_ATTR(scope),
	POWER_SUPPLY_ATTR(precharge_current),
	POWER_SUPPLY_ATTR(charge_term_current),
	POWER_SUPPLY_ATTR(calibrate),

	/* XM add */
	POWER_SUPPLY_ATTR(chip_ok),
	POWER_SUPPLY_ATTR(i2c_error_count),
	POWER_SUPPLY_ATTR(charge_status),

	/* only for usb_psy */
	POWER_SUPPLY_ATTR(real_type),
	POWER_SUPPLY_ATTR(quick_charge_type),
	POWER_SUPPLY_ATTR(pd_authentication),
	POWER_SUPPLY_ATTR(pd_verify_done),
	POWER_SUPPLY_ATTR(pd_type),
	POWER_SUPPLY_ATTR(apdo_max),
	POWER_SUPPLY_ATTR(power_max),
	POWER_SUPPLY_ATTR(typec_cc_orientation),
	POWER_SUPPLY_ATTR(typec_mode),
	POWER_SUPPLY_ATTR(ffc_enable),
	POWER_SUPPLY_ATTR(connector_temp),
	POWER_SUPPLY_ATTR(typec_burn),
	POWER_SUPPLY_ATTR(sw_cv),
	POWER_SUPPLY_ATTR(input_suspend),
	POWER_SUPPLY_ATTR(jeita_chg_index),
	POWER_SUPPLY_ATTR(cv_wa_count),
#ifdef CONFIG_FACTORY_BUILD
	POWER_SUPPLY_ATTR(cp_vbus),
	POWER_SUPPLY_ATTR(cp_ibus_master),
	POWER_SUPPLY_ATTR(cp_ibus_slave),
#endif

	/* only for battery */
	POWER_SUPPLY_ATTR(thermal_limit_fcc),

	/* only for XMUSB350 */
	POWER_SUPPLY_ATTR(chg_type),
	POWER_SUPPLY_ATTR(hvdcp3_type),
	POWER_SUPPLY_ATTR(fw_version),
	POWER_SUPPLY_ATTR(recheck_count),

	/* only for TI GAUGE */
	POWER_SUPPLY_ATTR(shutdown_delay),
	POWER_SUPPLY_ATTR(capacity_raw),
	POWER_SUPPLY_ATTR(soc_decimal),
	POWER_SUPPLY_ATTR(soc_decimal_rate),
	POWER_SUPPLY_ATTR(resistance),
	POWER_SUPPLY_ATTR(resistance_id),
	POWER_SUPPLY_ATTR(soh),
	POWER_SUPPLY_ATTR(fastcharge_mode),
	POWER_SUPPLY_ATTR(monitor_delay),
	POWER_SUPPLY_ATTR(shutdown_mode),
	POWER_SUPPLY_ATTR(fake_cycle_count),
	POWER_SUPPLY_ATTR(charge_done),

	 /* only for charge pump */
	POWER_SUPPLY_ATTR(bypass),
	POWER_SUPPLY_ATTR(bypass_support),
	POWER_SUPPLY_ATTR(ln_reset_check),

	/* Local extensions */
	POWER_SUPPLY_ATTR(usb_hc),
	POWER_SUPPLY_ATTR(usb_otg),
	POWER_SUPPLY_ATTR(charge_enabled),
	/* Local extensions of type int64_t */
	POWER_SUPPLY_ATTR(charge_counter_ext),
	/* Properties of type `const char *' */
	POWER_SUPPLY_ATTR(model_name),
	POWER_SUPPLY_ATTR(manufacturer),
	POWER_SUPPLY_ATTR(serial_number),
	POWER_SUPPLY_ATTR(device_chem),
#else
	/* Properties of type `int' */
	POWER_SUPPLY_ATTR(status),
	POWER_SUPPLY_ATTR(charge_type),
	POWER_SUPPLY_ATTR(health),
	POWER_SUPPLY_ATTR(present),
	POWER_SUPPLY_ATTR(online),
	POWER_SUPPLY_ATTR(bq_online),
	POWER_SUPPLY_ATTR(authentic),
	POWER_SUPPLY_ATTR(technology),
	POWER_SUPPLY_ATTR(cycle_count),
	POWER_SUPPLY_ATTR(voltage_max),
	POWER_SUPPLY_ATTR(voltage_min),
	POWER_SUPPLY_ATTR(voltage_max_design),
	POWER_SUPPLY_ATTR(voltage_min_design),
	POWER_SUPPLY_ATTR(voltage_now),
	POWER_SUPPLY_ATTR(voltage_avg),
	POWER_SUPPLY_ATTR(voltage_ocv),
	POWER_SUPPLY_ATTR(voltage_boot),
	POWER_SUPPLY_ATTR(current_max),
	POWER_SUPPLY_ATTR(current_now),
	POWER_SUPPLY_ATTR(current_avg),
	POWER_SUPPLY_ATTR(current_boot),
	POWER_SUPPLY_ATTR(power_now),
	POWER_SUPPLY_ATTR(power_avg),
	POWER_SUPPLY_ATTR(charge_full_design),
	POWER_SUPPLY_ATTR(charge_empty_design),
	POWER_SUPPLY_ATTR(charge_full),
	POWER_SUPPLY_ATTR(charge_empty),
	POWER_SUPPLY_ATTR(charge_now),
	POWER_SUPPLY_ATTR(charge_avg),
	POWER_SUPPLY_ATTR(charge_counter),
	POWER_SUPPLY_ATTR(constant_charge_current),
	POWER_SUPPLY_ATTR(constant_charge_current_max),
	POWER_SUPPLY_ATTR(constant_charge_voltage),
	POWER_SUPPLY_ATTR(constant_charge_voltage_max),
	POWER_SUPPLY_ATTR(charge_control_scene),
	POWER_SUPPLY_ATTR(charge_control_limit),
	POWER_SUPPLY_ATTR(charge_control_limit_max),
	POWER_SUPPLY_ATTR(fast_charge_current),
	POWER_SUPPLY_ATTR(thermal_input_current),
	POWER_SUPPLY_ATTR(input_current_now),
	POWER_SUPPLY_ATTR(input_current_limit),
	POWER_SUPPLY_ATTR(energy_full_design),
	POWER_SUPPLY_ATTR(energy_empty_design),
	POWER_SUPPLY_ATTR(energy_full),
	POWER_SUPPLY_ATTR(energy_empty),
	POWER_SUPPLY_ATTR(energy_now),
	POWER_SUPPLY_ATTR(energy_avg),
	POWER_SUPPLY_ATTR(capacity),
	POWER_SUPPLY_ATTR(capacity_alert_min),
	POWER_SUPPLY_ATTR(capacity_alert_max),
	POWER_SUPPLY_ATTR(capacity_level),
	POWER_SUPPLY_ATTR(soc_decimal),
	POWER_SUPPLY_ATTR(soc_decimal_rate),
	POWER_SUPPLY_ATTR(capacity_raw),
	POWER_SUPPLY_ATTR(temp),
	POWER_SUPPLY_ATTR(temp_max),
	POWER_SUPPLY_ATTR(temp_min),
	POWER_SUPPLY_ATTR(temp_connect),
	POWER_SUPPLY_ATTR(temp_alert_min),
	POWER_SUPPLY_ATTR(temp_alert_max),
	POWER_SUPPLY_ATTR(temp_ambient),
	POWER_SUPPLY_ATTR(temp_ambient_alert_min),
	POWER_SUPPLY_ATTR(temp_ambient_alert_max),
	POWER_SUPPLY_ATTR(time_to_empty_now),
	POWER_SUPPLY_ATTR(time_to_empty_avg),
	POWER_SUPPLY_ATTR(time_to_full_now),
	POWER_SUPPLY_ATTR(time_to_full_avg),
	POWER_SUPPLY_ATTR(type),
	POWER_SUPPLY_ATTR(usb_type),
	POWER_SUPPLY_ATTR(scope),
	POWER_SUPPLY_ATTR(night_charging),
	POWER_SUPPLY_ATTR(low_power_charging),
	POWER_SUPPLY_ATTR(precharge_current),
	POWER_SUPPLY_ATTR(charge_term_current),
	POWER_SUPPLY_ATTR(calibrate),
	/* Local extensions */
	POWER_SUPPLY_ATTR(fastcharge_mode),
	POWER_SUPPLY_ATTR(real_type),
	POWER_SUPPLY_ATTR(hvdcp3_type),
	POWER_SUPPLY_ATTR(quick_charge_type),
	POWER_SUPPLY_ATTR(type_recheck),
	POWER_SUPPLY_ATTR(pd_verify_in_process),
#ifdef CONFIG_MTBF_SUPPORT
	POWER_SUPPLY_ATTR(mtbf_current),
#endif
	/* Bq charge pump properties */
	POWER_SUPPLY_ATTR(dp_dm_bq),
	POWER_SUPPLY_ATTR(ti_charge_mode),
	POWER_SUPPLY_ATTR(ti_battery_present),
	POWER_SUPPLY_ATTR(ti_vbus_present),
	POWER_SUPPLY_ATTR(ti_battery_voltage),
	POWER_SUPPLY_ATTR(ti_battery_current),
	POWER_SUPPLY_ATTR(ti_battery_temperature),
	POWER_SUPPLY_ATTR(ti_bus_voltage),
	POWER_SUPPLY_ATTR(ti_bus_current),
	POWER_SUPPLY_ATTR(ti_bus_temperature),
	POWER_SUPPLY_ATTR(ti_die_temperature),
	POWER_SUPPLY_ATTR(ti_alarm_status),
	POWER_SUPPLY_ATTR(ti_fault_status),
	POWER_SUPPLY_ATTR(ti_reg_status),
	POWER_SUPPLY_ATTR(ti_set_bus_protection_for_qc3),
	POWER_SUPPLY_ATTR(ti_reset_check),
	POWER_SUPPLY_ATTR(bq_charge_done),
	POWER_SUPPLY_ATTR(hv_charge_enable),
	POWER_SUPPLY_ATTR(pd_active),
	POWER_SUPPLY_ATTR(pd_authentication),
	POWER_SUPPLY_ATTR(pd_type),
	POWER_SUPPLY_ATTR(apdo_max),
	POWER_SUPPLY_ATTR(power_max),
#ifdef CONFIG_XMUSB350_DET_CHG
	POWER_SUPPLY_ATTR(qc35_chg_type),
	POWER_SUPPLY_ATTR(qc35_error_state),
	POWER_SUPPLY_ATTR(qc35_vin),
	POWER_SUPPLY_ATTR(qc35_version),
	POWER_SUPPLY_ATTR(qc35_vid),
	POWER_SUPPLY_ATTR(qc35_chip_ok),
	POWER_SUPPLY_ATTR(qc35_rerun_apsd),
	POWER_SUPPLY_ATTR(qc35_soft_reset),
	POWER_SUPPLY_ATTR(qc35_mode_select),
	POWER_SUPPLY_ATTR(qc35_intb_enable),
	POWER_SUPPLY_ATTR(qc35_hvdcp_enable),
	POWER_SUPPLY_ATTR(qc35_bc12_enable),
	POWER_SUPPLY_ATTR(qc35_sleep_enable),
	POWER_SUPPLY_ATTR(qc35_hvdcp_dpdm),
#endif
	POWER_SUPPLY_ATTR(shutdown_delay),
	POWER_SUPPLY_ATTR(cold_thermal_level),
	POWER_SUPPLY_ATTR(update_now),
	POWER_SUPPLY_ATTR(chip_ok),
	POWER_SUPPLY_ATTR(termination_current),
	POWER_SUPPLY_ATTR(ffc_termination_current),
	POWER_SUPPLY_ATTR(recharge_vbat),
	POWER_SUPPLY_ATTR(soh),
	POWER_SUPPLY_ATTR(charge_done),
	POWER_SUPPLY_ATTR(force_recharge),
	/* Local extensions */
	POWER_SUPPLY_ATTR(usb_hc),
	POWER_SUPPLY_ATTR(usb_otg),
	POWER_SUPPLY_ATTR(charge_enabled),
	POWER_SUPPLY_ATTR(charging_enabled),
	POWER_SUPPLY_ATTR(typec_mode),
	POWER_SUPPLY_ATTR(typec_cc_orientation),
	POWER_SUPPLY_ATTR(typec_power_role),
	POWER_SUPPLY_ATTR(resistance),
	POWER_SUPPLY_ATTR(resistance_id),
	POWER_SUPPLY_ATTR(input_suspend),
	POWER_SUPPLY_ATTR(connector_temp),
	POWER_SUPPLY_ATTR(vbus_disable),
	POWER_SUPPLY_ATTR(cool_mode),
#ifdef CONFIG_BQ2597X_CHARGE_PUMP
	POWER_SUPPLY_ATTR(cp_vbus),
	POWER_SUPPLY_ATTR(cp_ibus_master),
	POWER_SUPPLY_ATTR(cp_ibus_slave),
	POWER_SUPPLY_ATTR(cp_ibus_total),
	POWER_SUPPLY_ATTR(cp_ibus_delta),
#endif
#ifdef CONFIG_AGATE_CHARGER
	/* bq25790 */
	POWER_SUPPLY_ATTR(usb_current_now),
	POWER_SUPPLY_ATTR(usb_voltage_now),
	POWER_SUPPLY_ATTR(hiz_mode),
	POWER_SUPPLY_ATTR(main_fcc_max),
	POWER_SUPPLY_ATTR(arti_vbus_enable),
	POWER_SUPPLY_ATTR(input_voltage_regulation),
	POWER_SUPPLY_ATTR(force_main_icl),
	POWER_SUPPLY_ATTR(pfm_otg_dis),
	POWER_SUPPLY_ATTR(pfm_fwd_dis),
	POWER_SUPPLY_ATTR(dis_otg_ooa),
	POWER_SUPPLY_ATTR(dis_fwd_ooa),
	/* bq25890 */
	POWER_SUPPLY_ATTR(input_voltage_limit),
	POWER_SUPPLY_ATTR(ti_adc),
	POWER_SUPPLY_ATTR(ti_bypass),
	/* LN8000 */
	POWER_SUPPLY_ATTR(lion_adc_values),
	POWER_SUPPLY_ATTR(lion_chg_status),
	POWER_SUPPLY_ATTR(lion_reg_dump),
#endif
	/* Local extensions of type int64_t */
	POWER_SUPPLY_ATTR(charge_counter_ext),
	/* Properties of type `const char *' */
	POWER_SUPPLY_ATTR(model_name),
	POWER_SUPPLY_ATTR(manufacturer),
	POWER_SUPPLY_ATTR(serial_number),
	POWER_SUPPLY_ATTR(battery_type),
#ifdef CONFIG_AGATE_CHARGER
	/* battery.c */
	POWER_SUPPLY_ATTR(min_icl),
	POWER_SUPPLY_ATTR(input_current_settled),
	POWER_SUPPLY_ATTR(hw_current_max),
	POWER_SUPPLY_ATTR(fcc_delta),
	POWER_SUPPLY_ATTR(cp_switcher_en),
	POWER_SUPPLY_ATTR(fcc_stepper_enable),
	POWER_SUPPLY_ATTR(parallel_batfet_mode),
	POWER_SUPPLY_ATTR(parallel_fcc_max),
	POWER_SUPPLY_ATTR(input_current_limited),
#endif
#endif
	POWER_SUPPLY_ATTR(set_ship_mode),
	POWER_SUPPLY_ATTR(charge_now_error),
	POWER_SUPPLY_ATTR(battery_charging_enabled),
	POWER_SUPPLY_ATTR(step_charging_step),
	POWER_SUPPLY_ATTR(pin_enabled),
	POWER_SUPPLY_ATTR(input_current_max),
	POWER_SUPPLY_ATTR(input_current_trim),
	POWER_SUPPLY_ATTR(input_voltage_settled),
	POWER_SUPPLY_ATTR(bypass_vchg_loop_debouncer),
	POWER_SUPPLY_ATTR(charge_counter_shadow),
	POWER_SUPPLY_ATTR(hi_power),
	POWER_SUPPLY_ATTR(low_power),
	POWER_SUPPLY_ATTR(temp_cool),
	POWER_SUPPLY_ATTR(temp_warm),
	POWER_SUPPLY_ATTR(temp_cold),
	POWER_SUPPLY_ATTR(temp_hot),
	POWER_SUPPLY_ATTR(system_temp_level),
	POWER_SUPPLY_ATTR(resistance_capacitive),
	POWER_SUPPLY_ATTR(resistance_now),
	POWER_SUPPLY_ATTR(flash_current_max),
	POWER_SUPPLY_ATTR(esr_count),
	POWER_SUPPLY_ATTR(buck_freq),
	POWER_SUPPLY_ATTR(boost_current),
	POWER_SUPPLY_ATTR(safety_timer_enabled),
	POWER_SUPPLY_ATTR(flash_active),
	POWER_SUPPLY_ATTR(flash_trigger),
	POWER_SUPPLY_ATTR(force_tlim),
	POWER_SUPPLY_ATTR(dp_dm),
	POWER_SUPPLY_ATTR(charge_qnovo_enable),
	POWER_SUPPLY_ATTR(current_qnovo),
	POWER_SUPPLY_ATTR(voltage_qnovo),
	POWER_SUPPLY_ATTR(rerun_aicl),
	POWER_SUPPLY_ATTR(cycle_count_id),
	POWER_SUPPLY_ATTR(safety_timer_expired),
	POWER_SUPPLY_ATTR(restricted_charging),
	POWER_SUPPLY_ATTR(current_capability),
	POWER_SUPPLY_ATTR(typec_src_rp),
	POWER_SUPPLY_ATTR(pd_allowed),
	POWER_SUPPLY_ATTR(pd_active),
	POWER_SUPPLY_ATTR(pd_in_hard_reset),
	POWER_SUPPLY_ATTR(pd_current_max),
	POWER_SUPPLY_ATTR(pd_usb_suspend_supported),
	POWER_SUPPLY_ATTR(charger_temp),
	POWER_SUPPLY_ATTR(charger_temp_max),
	POWER_SUPPLY_ATTR(parallel_disable),
	POWER_SUPPLY_ATTR(pe_start),
	POWER_SUPPLY_ATTR(soc_reporting_ready),
	POWER_SUPPLY_ATTR(debug_battery),
	POWER_SUPPLY_ATTR(fcc_delta),
	POWER_SUPPLY_ATTR(icl_reduction),
	POWER_SUPPLY_ATTR(parallel_mode),
	POWER_SUPPLY_ATTR(die_health),
	POWER_SUPPLY_ATTR(connector_health),
	POWER_SUPPLY_ATTR(ctm_current_max),
	POWER_SUPPLY_ATTR(hw_current_max),
	POWER_SUPPLY_ATTR(pr_swap),
	POWER_SUPPLY_ATTR(cc_step),
	POWER_SUPPLY_ATTR(cc_step_sel),
	POWER_SUPPLY_ATTR(sw_jeita_enabled),
	POWER_SUPPLY_ATTR(pd_voltage_max),
	POWER_SUPPLY_ATTR(pd_voltage_min),
	POWER_SUPPLY_ATTR(sdp_current_max),
	POWER_SUPPLY_ATTR(connector_type),
	POWER_SUPPLY_ATTR(moisture_detected),
	POWER_SUPPLY_ATTR(batt_profile_version),
	POWER_SUPPLY_ATTR(batt_full_current),
	POWER_SUPPLY_ATTR(recharge_soc),
	POWER_SUPPLY_ATTR(hvdcp_opti_allowed),
	POWER_SUPPLY_ATTR(smb_en_mode),
	POWER_SUPPLY_ATTR(smb_en_reason),
	POWER_SUPPLY_ATTR(esr_actual),
	POWER_SUPPLY_ATTR(esr_nominal),
	POWER_SUPPLY_ATTR(soh),
	POWER_SUPPLY_ATTR(qc_opti_disable),
	POWER_SUPPLY_ATTR(cc_soc),
	POWER_SUPPLY_ATTR(batt_age_level),
	POWER_SUPPLY_ATTR(scale_mode_en),
	POWER_SUPPLY_ATTR(voltage_vph),
	POWER_SUPPLY_ATTR(chip_version),
	POWER_SUPPLY_ATTR(therm_icl_limit),
	POWER_SUPPLY_ATTR(dc_reset),
	POWER_SUPPLY_ATTR(voltage_max_limit),
	POWER_SUPPLY_ATTR(real_capacity),
	POWER_SUPPLY_ATTR(force_main_icl),
	POWER_SUPPLY_ATTR(force_main_fcc),
	POWER_SUPPLY_ATTR(comp_clamp_level),
	POWER_SUPPLY_ATTR(adapter_cc_mode),
	POWER_SUPPLY_ATTR(skin_health),
	POWER_SUPPLY_ATTR(charge_disable),
	POWER_SUPPLY_ATTR(adapter_details),
	POWER_SUPPLY_ATTR(dead_battery),
	POWER_SUPPLY_ATTR(voltage_fifo),
	POWER_SUPPLY_ATTR(cc_uah),
	POWER_SUPPLY_ATTR(operating_freq),
	POWER_SUPPLY_ATTR(aicl_delay),
	POWER_SUPPLY_ATTR(aicl_icl),
	POWER_SUPPLY_ATTR(rtx),
	POWER_SUPPLY_ATTR(cutoff_soc),
	POWER_SUPPLY_ATTR(sys_soc),
	POWER_SUPPLY_ATTR(batt_soc),
	/* Capacity Estimation */
	POWER_SUPPLY_ATTR(batt_ce_ctrl),
	POWER_SUPPLY_ATTR(batt_ce_full),
	/* Resistance Estimaton */
	POWER_SUPPLY_ATTR(resistance_avg),
	POWER_SUPPLY_ATTR(batt_res_filt_cnts),
	POWER_SUPPLY_ATTR(aicl_done),
	POWER_SUPPLY_ATTR(voltage_step),
	POWER_SUPPLY_ATTR(otg_fastroleswap),
	POWER_SUPPLY_ATTR(apsd_rerun),
	POWER_SUPPLY_ATTR(apsd_timeout),
	/* Charge pump properties */
	POWER_SUPPLY_ATTR(cp_status1),
	POWER_SUPPLY_ATTR(cp_status2),
	POWER_SUPPLY_ATTR(cp_enable),
	POWER_SUPPLY_ATTR(cp_switcher_en),
	POWER_SUPPLY_ATTR(cp_die_temp),
	POWER_SUPPLY_ATTR(cp_isns),
	POWER_SUPPLY_ATTR(cp_isns_slave),
	POWER_SUPPLY_ATTR(cp_toggle_switcher),
	POWER_SUPPLY_ATTR(cp_irq_status),
	POWER_SUPPLY_ATTR(cp_ilim),
	POWER_SUPPLY_ATTR(irq_status),
	POWER_SUPPLY_ATTR(parallel_output_mode),
	POWER_SUPPLY_ATTR(alignment),
	POWER_SUPPLY_ATTR(moisture_detection_enabled),
	POWER_SUPPLY_ATTR(cc_toggle_enable),
	POWER_SUPPLY_ATTR(fg_type),
	POWER_SUPPLY_ATTR(charger_status),
	/* Local extensions of type int64_t */
	POWER_SUPPLY_ATTR(charge_counter_ext),
	POWER_SUPPLY_ATTR(charge_charger_state),
	/* Properties of type `const char *' */
	POWER_SUPPLY_ATTR(model_name),
	POWER_SUPPLY_ATTR(ptmc_id),
	POWER_SUPPLY_ATTR(manufacturer),
	POWER_SUPPLY_ATTR(battery_type),
	POWER_SUPPLY_ATTR(cycle_counts),
	POWER_SUPPLY_ATTR(serial_number),
};

static struct attribute *
__power_supply_attrs[ARRAY_SIZE(power_supply_attrs) + 1];

static umode_t power_supply_attr_is_visible(struct kobject *kobj,
					   struct attribute *attr,
					   int attrno)
{
	struct device *dev = kobj_to_dev(kobj);
	struct power_supply *psy = dev_get_drvdata(dev);
	umode_t mode = S_IRUSR | S_IRGRP | S_IROTH;
	int i;

	if (!power_supply_attrs[attrno].prop_name)
		return 0;

	if (attrno == POWER_SUPPLY_PROP_TYPE)
		return mode;

	for (i = 0; i < psy->desc->num_properties; i++) {
		int property = psy->desc->properties[i];

		if (property == attrno) {
			if (psy->desc->property_is_writeable &&
			    psy->desc->property_is_writeable(psy, property) > 0)
				mode |= S_IWUSR;

			return mode;
		}
	}

	return 0;
}

static struct attribute_group power_supply_attr_group = {
	.attrs = __power_supply_attrs,
	.is_visible = power_supply_attr_is_visible,
};

static const struct attribute_group *power_supply_attr_groups[] = {
	&power_supply_attr_group,
	NULL,
};

static void str_to_lower(char *str)
{
	while (*str) {
		*str = tolower(*str);
		str++;
	}
}

void power_supply_init_attrs(struct device_type *dev_type)
{
	int i;

	dev_type->groups = power_supply_attr_groups;

	for (i = 0; i < ARRAY_SIZE(power_supply_attrs); i++) {
		struct device_attribute *attr;

		if (!power_supply_attrs[i].prop_name) {
			pr_warn("%s: Property %d skipped because is is missing from power_supply_attrs\n",
				__func__, i);
			sprintf(power_supply_attrs[i].attr_name, "_err_%d", i);
		} else {
			str_to_lower(power_supply_attrs[i].attr_name);
		}

		attr = &power_supply_attrs[i].dev_attr;

		attr->attr.name = power_supply_attrs[i].attr_name;
		attr->show = power_supply_show_property;
		attr->store = power_supply_store_property;
		__power_supply_attrs[i] = &attr->attr;
	}
}

static int add_prop_uevent(struct device *dev, struct kobj_uevent_env *env,
			   enum power_supply_property prop, char *prop_buf)
{
	int ret = 0;
	struct power_supply_attr *pwr_attr;
	struct device_attribute *dev_attr;
	char *line;

	pwr_attr = &power_supply_attrs[prop];
	dev_attr = &pwr_attr->dev_attr;

	ret = power_supply_show_property(dev, dev_attr, prop_buf);
	if (ret == -ENODEV || ret == -ENODATA) {
		/*
		 * When a battery is absent, we expect -ENODEV. Don't abort;
		 * send the uevent with at least the the PRESENT=0 property
		 */
		return 0;
	}

	if (ret < 0)
		return ret;

	line = strchr(prop_buf, '\n');
	if (line)
		*line = 0;

	return add_uevent_var(env, "POWER_SUPPLY_%s=%s",
			      pwr_attr->prop_name, prop_buf);
}

int power_supply_uevent(struct device *dev, struct kobj_uevent_env *env)
{
	struct power_supply *psy = dev_get_drvdata(dev);
	int ret = 0, j;
	char *prop_buf;

	if (!psy || !psy->desc) {
		dev_dbg(dev, "No power supply yet\n");
		return ret;
	}

	ret = add_uevent_var(env, "POWER_SUPPLY_NAME=%s", psy->desc->name);
	if (ret)
		return ret;

	prop_buf = (char *)get_zeroed_page(GFP_KERNEL);
	if (!prop_buf)
		return -ENOMEM;

	ret = add_prop_uevent(dev, env, POWER_SUPPLY_PROP_TYPE, prop_buf);
	if (ret)
		goto out;

	for (j = 0; j < psy->desc->num_properties; j++) {
		ret = add_prop_uevent(dev, env, psy->desc->properties[j],
				      prop_buf);
		if (ret)
			goto out;
	}

out:
	free_page((unsigned long)prop_buf);

	return ret;
}
