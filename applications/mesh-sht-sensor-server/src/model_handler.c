#include <stdio.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/mesh/models.h>
#include <dk_buttons_and_leds.h>
#include "model_handler.h"

static const struct device *dev;

static int sht31_temperature_get(struct bt_mesh_sensor *sensor, struct bt_mesh_msg_ctx *ctx, struct sensor_value *rsp)
{
	int err;

	err = sensor_sample_fetch(dev);
	if(err) {
		printk("Error sensor_sample_fetch (%d)\n", err);	
	}

	err = sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, rsp);
	if (err) {
		printk("Error getting temperature sensor data (%d)\n", err);
	} else {
		printf("SHT3XD: %.2f Cel\n", sensor_value_to_double(rsp));	
	}
	return err;
}

static struct bt_mesh_sensor sht31_temperature = {
	.type = &bt_mesh_sensor_present_amb_temp,
	.get = sht31_temperature_get,
};

static int sht31_humidity_get(struct bt_mesh_sensor *sensor, struct bt_mesh_msg_ctx *ctx, struct sensor_value *rsp)
{
	int err;

	//sensor_sample_fetch(dev);
	err = sensor_channel_get(dev, SENSOR_CHAN_HUMIDITY, rsp);
	if (err) {
		printk("Error getting humidity sensor data (%d)\n", err);
	} else {
		printf("SHT3XD: %0.2f %%RH\n", sensor_value_to_double(rsp));	
	}

	return err;
}

static struct bt_mesh_sensor sht31_humidity = {
	.type = &bt_mesh_sensor_present_amb_rel_humidity,
	.get = sht31_humidity_get,
};


static struct bt_mesh_sensor *const sensors[] = {
	&sht31_temperature,
	&sht31_humidity
};

static struct bt_mesh_sensor_srv sensor_srv =
	BT_MESH_SENSOR_SRV_INIT(sensors, ARRAY_SIZE(sensors));


static void button_handler_cb(uint32_t pressed, uint32_t changed)
{
}

static struct button_handler button_handler = {
	.cb = button_handler_cb,
};

/** Configuration server definition */
static struct bt_mesh_cfg_srv cfg_srv = {
	.relay = IS_ENABLED(CONFIG_BT_MESH_RELAY),
	.beacon = BT_MESH_BEACON_ENABLED,
	.frnd = IS_ENABLED(CONFIG_BT_MESH_FRIEND),
	.gatt_proxy = IS_ENABLED(CONFIG_BT_MESH_GATT_PROXY),
	.default_ttl = 7,

	/* 3 transmissions with 20ms interval */
	.net_transmit = BT_MESH_TRANSMIT(2, 20),
	.relay_retransmit = BT_MESH_TRANSMIT(2, 20),
};

/* Set up a repeating delayed work to blink the DK's LEDs when attention is
 * requested.
 */
static struct k_delayed_work attention_blink_work;

static void attention_blink(struct k_work *work)
{
	static int idx;
	const uint8_t pattern[] = {
		BIT(0) | BIT(1),
		BIT(1) | BIT(2),
		BIT(2) | BIT(3),
		BIT(3) | BIT(0),
	};
	dk_set_leds(pattern[idx++ % ARRAY_SIZE(pattern)]);
	k_delayed_work_submit(&attention_blink_work, K_MSEC(30));
}

static void attention_on(struct bt_mesh_model *mod)
{
	k_delayed_work_submit(&attention_blink_work, K_NO_WAIT);
}

static void attention_off(struct bt_mesh_model *mod)
{
	k_delayed_work_cancel(&attention_blink_work);
	dk_set_leds(DK_NO_LEDS_MSK);
}

static const struct bt_mesh_health_srv_cb health_srv_cb = {
	.attn_on = attention_on,
	.attn_off = attention_off,
};

static struct bt_mesh_health_srv health_srv = {
	.cb = &health_srv_cb,
};

BT_MESH_HEALTH_PUB_DEFINE(health_pub, 0);

static struct bt_mesh_elem elements[] = {
	BT_MESH_ELEM(1,
		BT_MESH_MODEL_LIST(
			BT_MESH_MODEL_CFG_SRV(&cfg_srv),
			BT_MESH_MODEL_HEALTH_SRV(&health_srv,&health_pub),
			BT_MESH_MODEL_SENSOR_SRV(&sensor_srv)),
		BT_MESH_MODEL_NONE),
};

static const struct bt_mesh_comp comp = {
	.cid = CONFIG_BT_COMPANY_ID,
	.elem = elements,
	.elem_count = ARRAY_SIZE(elements),
};

static void initial_sensor_read() {
	int rc;
	struct sensor_value temp, hum;
	printk("before\n");
	rc = sensor_sample_fetch(dev);
	printk("after\n");
	if (rc == 0) {
		rc = sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP,&temp);
	}
	if (rc == 0) {
		rc = sensor_channel_get(dev, SENSOR_CHAN_HUMIDITY,
					&hum);
	}
	if (rc != 0) {
		printk("SHT3XD: failed: %d\n", rc);
		return;
	}

	
	printf("SHT3XD: %.2f Cel ; %0.2f %%RH\n", sensor_value_to_double(&temp), sensor_value_to_double(&hum));
}

const struct bt_mesh_comp *model_handler_init(void)
{
	k_delayed_work_init(&attention_blink_work, attention_blink);
	
	dev = device_get_binding("SHT3XD");

	if (dev == NULL) {
		printk("Could not initiate SHT3XD sensor\n");
	} else {
		printk("SHT3XD sensor (%s) initiated\n", dev->name);
	}

	initial_sensor_read();

	dk_button_handler_add(&button_handler);

	return &comp;
}
