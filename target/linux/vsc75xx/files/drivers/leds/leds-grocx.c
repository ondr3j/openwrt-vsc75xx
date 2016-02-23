#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <linux/err.h>

static struct platform_device *pdev;

extern char *led_data;
extern char led_status;

static struct grocx_led {
	struct led_classdev cdev;
	unsigned int gpio;
} grocx_leds[] = {
	{ { .name = "grocx:act"		} },
	{ { .name = "grocx:wlan"	} },
	{ { .name = "grocx:usb1"	} },
	{ { .name = "grocx:usb2"	} },
	{ { .name = "grocx:phone1"	} },
	{ { .name = "grocx:phone2"	} },
	{ { .name = "grocx:hpa"		} },
	{ { .name = "grocx:wps"		} }
};

static void grocx_led_set(struct led_classdev *led_cdev, enum led_brightness value)
{
	struct grocx_led *ldev = container_of(led_cdev, struct grocx_led, cdev);

	if (value != LED_OFF) {
		led_status &= ~(1 << ldev->gpio);
	} else {
		led_status |= (1 << ldev->gpio);
	}
	*led_data = led_status;
}

static enum led_brightness grocx_led_get(struct led_classdev *led_cdev)
{
	struct grocx_led *ldev = container_of(led_cdev, struct grocx_led, cdev);

	return (led_status & (1 << ldev->gpio)) ? LED_OFF : LED_FULL;
}

#ifdef CONFIG_PM
static int grocx_led_suspend(struct platform_device *dev, pm_message_t state)
{
	int i = 0;

	for (; i < ARRAY_SIZE(grocx_leds); ++i) {
		led_classdev_suspend(&grocx_leds[i].cdev);
	}
	return 0;
}

static int grocx_led_resume(struct platform_device *dev)
{
	int i = 0;

	for (; i < ARRAY_SIZE(grocx_leds); ++i) {
		led_classdev_resume(&grocx_leds[i].cdev);
	}
	return 0;
}
#else
#define grocx_led_suspend NULL
#define grocx_led_resume NULL
#endif

static int grocx_led_probe(struct platform_device *pdev)
{
	int ret = 0;
	int i = 0;

	for (; i < ARRAY_SIZE(grocx_leds); ++i) {
		grocx_leds[i].gpio = i;
		grocx_leds[i].cdev.brightness_set = grocx_led_set;
		// grocx_leds[i].cdev.brightness_get = grocx_led_get;
		grocx_leds[i].cdev.brightness = grocx_led_get(&grocx_leds[i].cdev);

		ret = led_classdev_register(&pdev->dev, &grocx_leds[i].cdev);
		if (ret < 0) {
			for (; --i >= 0;) {
				led_classdev_unregister(&grocx_leds[i].cdev);
			}
		}
	}
	return ret;
}

static int grocx_led_remove(struct platform_device *pdev)
{
	int i = 0;

	for (; i < ARRAY_SIZE(grocx_leds); ++i) {
		led_classdev_unregister(&grocx_leds[i].cdev);
	}
	return 0;
}

static struct platform_driver grocx_led_driver = {
	.probe		= grocx_led_probe,
	.remove		= __devexit_p(grocx_led_remove),
	.suspend	= grocx_led_suspend,
	.resume		= grocx_led_resume,
	.driver		= {
		.name		= "grocx-led",
		.owner		= THIS_MODULE,
	},
};

static int __init grocx_led_init(void)
{
	int ret = platform_driver_register(&grocx_led_driver);

	if (ret >= 0) {
		pdev = platform_device_register_simple("grocx-led", -1, NULL, 0);
		if (IS_ERR(pdev)) {
			ret = PTR_ERR(pdev);
			platform_driver_unregister(&grocx_led_driver);
		}
	}
	return ret;
}

static void __exit grocx_led_exit(void)
{
	platform_device_unregister(pdev);
	platform_driver_unregister(&grocx_led_driver);
}

module_init(grocx_led_init);
module_exit(grocx_led_exit);

MODULE_AUTHOR("Ondrej Patrovic <ondrej@patrovic.com>");
MODULE_DESCRIPTION("WebRocX LED driver");
MODULE_LICENSE("GPL");

