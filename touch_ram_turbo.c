#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/input.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("F54");
MODULE_DESCRIPTION("Zero-Latency Touch Booster");

static void turbo_event(struct input_handle *handle, unsigned int type, unsigned int code, int value) {
    if (type == EV_ABS && (code == ABS_MT_TRACKING_ID || code == ABS_MT_POSITION_X)) {
        preempt_disable();
        preempt_enable();
    }
}

static int turbo_connect(struct input_handler *handler, struct input_dev *dev, const struct input_device_id *id) {
    struct input_handle *handle = kzalloc(sizeof(struct input_handle), GFP_KERNEL);
    if (!handle) return -ENOMEM;
    handle->dev = dev;
    handle->handler = handler;
    handle->name = "f54_touch_turbo";
    if (input_register_handle(handle) || input_open_device(handle)) {
        kfree(handle);
        return -EINVAL;
    }
    return 0;
}

static void turbo_disconnect(struct input_handle *handle) {
    input_close_device(handle);
    input_unregister_handle(handle);
    kfree(handle);
}

static const struct input_device_id turbo_ids[] = {
    { .flags = INPUT_DEVICE_ID_MATCH_EVBIT, .evbit = { BIT_MASK(EV_ABS) } },
    { },
};

static struct input_handler turbo_handler = {
    .event = turbo_event,
    .connect = turbo_connect,
    .disconnect = turbo_disconnect,
    .name = "f54_touch_turbo",
    .id_table = turbo_ids,
};

static int __init touch_ram_init(void) {
    return input_register_handler(&turbo_handler);
}

static void __exit touch_ram_exit(void) {
    input_unregister_handler(&turbo_handler);
}

module_init(touch_ram_init);
module_exit(touch_ram_exit);
