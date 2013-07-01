/*
 * BeagleXM: Driver for Leopard Module Board
 *
 * Copyright (C) 2011 Texas Instruments Inc
 * Author: Vaibhav Hiremath <hvaibhav@ti.com>
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/io.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/regulator/consumer.h>

#include <mach/gpio.h>

#include <media/mt9v113.h>
#include <media/ov5640.h>

#include <../drivers/media/video/isp/isp.h>

#include "devices.h"

#define CAM_USE_XCLKA			0
#define LEOPARD_RESET_GPIO		98
#define POWER_DOWN_GPIO			167

static struct regulator *cam_1v5;	//beagle_cam_digital
static struct regulator *cam_1v8;	//beagle_cam_io
static struct regulator *cam_2v8;	//beagle_cam_analog

static int ov5640_read_reg(struct i2c_client *client, unsigned short reg)
{
	printk(KERN_INFO "%s,ENTER \n",__func__);
        int err = 0;
        struct i2c_msg msg[1];
        unsigned char data[2];
        unsigned char val = 0;

        if (!client->adapter)
                return -ENODEV;

        msg->addr = client->addr;
        msg->flags = 0;
        msg->len = I2C_TWO_BYTE_TRANSFER;
        msg->buf = data;
        data[0] = (reg & I2C_TXRX_DATA_MASK_UPPER) >> I2C_TXRX_DATA_SHIFT;
        data[1] = (reg & I2C_TXRX_DATA_MASK);
        err = i2c_transfer(client->adapter, msg, 1);
	printk(KERN_INFO "%s, I2c Write err = %d\n",__func__,err);
        if (err >= 0) {
                msg->flags = I2C_M_RD;
                msg->len = I2C_ONE_BYTE_TRANSFER;       /* 1 byte read */
                err = i2c_transfer(client->adapter, msg, 1);
		printk(KERN_INFO "%s, I2c Read err = %d\n",__func__,err);
                if (err >= 0) {
                        val = (data[0] & I2C_TXRX_DATA_MASK);
                }
        }
        return (int)(0xff & val);
}

static int beagle_ov5640_s_power(struct v4l2_subdev *subdev, int on)
{
	printk(KERN_INFO "%s, ENTER\n",__func__);
	struct isp_device *isp = v4l2_dev_to_isp_device(subdev->v4l2_dev);

	if (!cam_2v8 || !cam_1v8) {
	//if (!cam_2v8 || !cam_1v8 || !cam_1v5) {
		dev_err(isp->dev, "No regulator available\n");
		return -ENODEV;
	}
	if (on) {
		/* Check Voltage-Levels */
		if(regulator_get_voltage(cam_2v8) != 2800000)
			regulator_set_voltage(cam_2v8,2800000,2800000);
		
		if(regulator_get_voltage(cam_1v8) != 1800000)
			regulator_set_voltage(cam_1v8,1800000,1800000);

		//if(regulator_get_voltage(cam_1v5) != 1500000)
		//	regulator_set_voltage(cam_1v5,1500000,1500000);

		/* Request POWER_DOWN_GPIO and set to output */
		gpio_request(POWER_DOWN_GPIO,"CAM_PWRDN");
		gpio_direction_output(POWER_DOWN_GPIO,true);

		/*
		 * Power Up Sequence
		 */
		/* Set POWER_DOWN to 1*/
		gpio_set_value(POWER_DOWN_GPIO,1);
		/* Set RESET_BAR to 0 */
		gpio_set_value(LEOPARD_RESET_GPIO, 0);
		/* Turn on VDD */
		regulator_enable(cam_1v8);	//VOVDD
		/* T0 */
		mdelay(1);
		regulator_enable(cam_2v8);	//AVDD
#if 0
		/* T1 */
		mdelay(1);
		regulator_enable(cam_1v5);	//DVDD
#endif
		/* T2 */
		mdelay(7);
		/* Set POWER_DOWN to 0 */
		gpio_set_value(POWER_DOWN_GPIO,0);
		/* T3 */
		mdelay(2);
		/* Set RESET_BAR to 1 */
		gpio_set_value(LEOPARD_RESET_GPIO, 1);

		/* Enable EXTCLK */
		if (isp->platform_cb.set_xclk)
			isp->platform_cb.set_xclk(isp, 24000000, CAM_USE_XCLKA);
		/*
		 * Wait at least 70 CLK cycles (w/EXTCLK = 24MHz):
		 * ((1000000 * 70) / 24000000) = aprox 3 us.
		 */
		/*
		 * Wait at least 100 CLK cycles (w/EXTCLK = 24MHz):
		 * ((1000000 * 100) / 24000000) = aprox 5 us.
		 */
		/* T4 */
		mdelay(25);
		while(1){
			struct i2c_client *client = v4l2_get_subdevdata(subdev);

			printk(KERN_INFO "%s,detect ov5640 MID= 0x%x\n",__func__, ov5640_read_reg(client, 0x300a)); 
			//printk(KERN_INFO "%s,detect ov5640 LID= 0x%x\n",__func__, ov5640_read_reg(client, 0x300b));
			mdelay(1000);

		}
	} else {
		/*
		 * Power Down Sequence
		 */
		if (regulator_is_enabled(cam_2v8))
			regulator_disable(cam_2v8);
		if (regulator_is_enabled(cam_1v8))
			regulator_disable(cam_1v8);
		//if (regulator_is_enabled(cam_1v5))
		//	regulator_disable(cam_1v5);

		if (isp->platform_cb.set_xclk)
			isp->platform_cb.set_xclk(isp, 0, CAM_USE_XCLKA);
	}

	printk(KERN_DEBUG "%s,EXIT\n", __func__);
	return 0;
}

static int beagle_ov5640_configure_interface(struct v4l2_subdev *subdev,
					      u32 pixclk)
{
	printk(KERN_DEBUG "%s,ENTER\n", __func__);
	struct isp_device *isp = v4l2_dev_to_isp_device(subdev->v4l2_dev);

	if (isp->platform_cb.set_pixel_clock)
		isp->platform_cb.set_pixel_clock(isp, pixclk);

	printk(KERN_DEBUG "%s,EXIT\n", __func__);
	return 0;
}

static struct ov5640_platform_data beagle_ov5640_platform_data = {
	.s_power		= beagle_ov5640_s_power,
	.configure_interface	= beagle_ov5640_configure_interface,
};


#define OV5640_I2C_BUS_NUM		2

static struct i2c_board_info beagle_camera_i2c_devices[] = {
	{
		I2C_BOARD_INFO(OV5640_MODULE_NAME, OV5640_I2C_ADDR),
		.platform_data = &beagle_ov5640_platform_data,
	},
};

static struct isp_subdev_i2c_board_info beagle_camera_primary_subdevs[] = {
	{
		.board_info = &beagle_camera_i2c_devices[0],
		.i2c_adapter_id = OV5640_I2C_BUS_NUM,
	},
	{ NULL, 0 },
};

static struct isp_v4l2_subdevs_group beagle_camera_subdevs[] = {
	{
		.subdevs = beagle_camera_primary_subdevs,
		.interface = ISP_INTERFACE_PARALLEL,
		.bus = {
			.parallel = {
				.data_lane_shift	= 2,
				.clk_pol		= 0,
				.bridge			= 3,
			},
		},
	},
	{ NULL, 0 },
};

static struct isp_platform_data beagle_isp_platform_data = {
	.subdevs = beagle_camera_subdevs,
};

static int __init beagle_cam_init(void)
{
	printk(KERN_DEBUG "%s,ENTER\n", __func__);
	/*
	 * Regulator supply required for camera interface
	 */
	cam_2v8 = regulator_get(NULL, "cam_2v8");
	if (IS_ERR(cam_2v8)) {
		printk(KERN_ERR "cam_1v8 regulator missing\n");
		return PTR_ERR(cam_1v8);
	}
	cam_1v8 = regulator_get(NULL, "cam_1v8");
	if (IS_ERR(cam_1v8)) {
		printk(KERN_ERR "cam_1v8 regulator missing\n");
		regulator_put(cam_1v8);
		return PTR_ERR(cam_1v8);
	}
#if 0
	cam_1v5 = regulator_get(NULL, "cam_1v5");
	if (IS_ERR(cam_1v5)) {
		printk(KERN_ERR "cam_1v5 regulator missing\n");
		regulator_put(cam_1v5);
		return PTR_ERR(cam_1v5);
	}
#endif
	/*
	 * Sensor reset GPIO
	 */
	if (gpio_request(LEOPARD_RESET_GPIO, "cam_rst") != 0) {
		printk(KERN_ERR "beagle-cam: Could not request GPIO %d\n",
				LEOPARD_RESET_GPIO);
		regulator_put(cam_2v8);
		regulator_put(cam_1v8);
	//	regulator_put(cam_1v5);
		return -ENODEV;
	}
	/* set to output mode */
	gpio_direction_output(LEOPARD_RESET_GPIO, 0);

	omap3_init_camera(&beagle_isp_platform_data);

	printk(KERN_DEBUG "%s,EXIT\n", __func__);
	return 0;
}

static void __exit beagle_cam_exit(void)
{
	if (regulator_is_enabled(cam_2v8))
		regulator_disable(cam_2v8);
	regulator_put(cam_2v8);
	if (regulator_is_enabled(cam_1v8))
		regulator_disable(cam_1v8);
	regulator_put(cam_1v8);
#if 0
	if (regulator_is_enabled(cam_1v5))
		regulator_disable(cam_1v5);
	regulator_put(cam_1v5);
#endif

	gpio_free(LEOPARD_RESET_GPIO);
}

module_init(beagle_cam_init);
module_exit(beagle_cam_exit);

MODULE_AUTHOR("Texas Instruments");
MODULE_DESCRIPTION("BeagleXM Camera Module");
MODULE_LICENSE("GPL");
