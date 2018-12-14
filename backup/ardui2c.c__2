// Refs:
//	Errors page: http://www-numi.fnal.gov/offline_software/srt_public_context/WebDocs/Errors/unix_system_errors.html
//	i2c: https://www.kernel.org/doc/html/v4.14/driver-api/i2c.html

#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/iio/iio.h>    
#include <linux/iio/sysfs.h>
#include <linux/delay.h>


#define ARDUI2C_DEVICE_ADDR  0x11
#define CMD_DEVICE_ID        0x0A
#define CMD_GET_SCALE        0x0B
#define CMD_SINGLE_SHOT_A0   0x0C
#define CMD_SINGLE_SHOT_A1   0x0D


#define VOLTAGE_CHANNEL(num)   						\
	{                               				\
	    .type = IIO_VOLTAGE,        				\
	    .indexed = 1,               				\
	    .channel = (num),          					\
	    .address = (num),           				\
	    .info_mask_separate = BIT(IIO_CHAN_INFO_RAW),           	\
	    .info_mask_shared_by_type = BIT(IIO_CHAN_INFO_SCALE)    	\
	}

struct ardui2c_data{
	struct i2c_client *client;
	struct mutex lock;
};

static int ardui2c_read_raw(struct iio_dev *indio_dev,
		    	    struct iio_chan_spec const *chan, 
		    	    int *val, int *val2, long mask)
{
	
	struct ardui2c_data *data = iio_priv(indio_dev);
	int ret = 0;
	// Note: Arduino is Little Endian and we don't know about this cpu;
	// the data comes in the following format:
	// 	{(uint16_t)voltage0, (uint16_t)voltage1, (uint32_t)scale}
	struct{
		__le16 voltage0;
		__le16 voltage1;
		__le32 scale;
	}__attribute__((__packed__))res;
	
	pr_info("read_raw\n");

	switch(mask){
	case IIO_CHAN_INFO_RAW:

		ret = i2c_master_recv(data->client, (char *)&res, sizeof(res));
		if(ret < 0) return ret;

		switch(chan->channel){
		case 0:
			*val = __le16_to_cpu(res.voltage0);
			break;
		case 1:
			*val = __le16_to_cpu(res.voltage1);
			break;
		default:
			return -EINVAL; // Invalid argument
		}

		printk(KERN_INFO "recv ino: %d, %d, %d", res.voltage0, res.voltage1, res.scale);
		printk(KERN_INFO "recv cpu: %d, %d, %d", __le16_to_cpu(res.voltage0), __le16_to_cpu(res.voltage1), __le32_to_cpu(res.scale));

		return IIO_VAL_INT;	

	case IIO_CHAN_INFO_SCALE:
		*val = le32_to_cpu(res.scale)/1000;
		*val2 = (le32_to_cpu(res.scale)%1000)*1000;
		return IIO_VAL_INT_PLUS_NANO;
	default:
		return -EINVAL;	// Invalid argument
	}
}

static int ardui2c_write_raw(struct iio_dev *indio_dev,
		     	     struct iio_chan_spec const *chan, 
		     	     int val, int val2, long mask)
{
	pr_info("write_raw\n");
	return 0;
}

static const struct iio_chan_spec ardui2c_channels[] = {
	VOLTAGE_CHANNEL(0),
	VOLTAGE_CHANNEL(1),
};

static const struct iio_info ardui2c_info = {
	.read_raw = ardui2c_read_raw,
	.write_raw = ardui2c_write_raw,
};

static int ardui2c_probe(struct i2c_client *client,
			 const struct i2c_device_id *id)
{
	int ret;
	struct iio_dev *indio_dev;
	struct ardui2c_data *data;

	pr_info("->probing ARDUI2C...");

	//Check if I2C bus controller supports the functionality needed by our device
	if(!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)){
		dev_err(&client->dev, "I2C don't support some funcionality needed by this device");
		return -EOPNOTSUPP;
	}

	// Check device ID
	//
	
	// IIO data allocation
	indio_dev = devm_iio_device_alloc(&client->dev, sizeof(*data));
	if(!indio_dev){
		dev_err(&client->dev, "iio allocation failed.\n");
		return -ENOMEM; // Out of memory
	}
	// private data
	data = iio_priv(indio_dev);
	data->client = client;
	i2c_set_clientdata(client, indio_dev);
	mutex_init(&data->lock);

	// public data
	indio_dev->dev.parent = &client->dev;
	indio_dev->info = &ardui2c_info;
	indio_dev->name = id->name;
	indio_dev->modes = INDIO_DIRECT_MODE;
	indio_dev->channels = ardui2c_channels;
	indio_dev->num_channels = ARRAY_SIZE(ardui2c_channels);

	// ardui2c configuration
	/*ret = ardui2c_config(client);
	if(ret < 0) {
		dev_err(&client->dev, "Configuration failed.\n");
		return ret;
	}*/

	// Device registration
	ret = iio_device_register(indio_dev);
	if(ret < 0){
		dev_err(&client->dev, "device_register failed\n");
		//return ret;
	}

	pr_info("\t->probing ARDUI2C... Finished!\n");

	return 0;
}

static int ardui2c_remove(struct i2c_client *client)
{
	struct iio_dev *indio_dev;

	pr_info("Removing ARDUI2C...\n");

	indio_dev = i2c_get_clientdata(client);

	// Device unregistration
	iio_device_unregister(indio_dev);

	pr_info("\tok!\n");

	return 0;
}

//static SIMPLE_DEV_PM_OPS(ardui2c_pm_ops, ardui2c_suspended, NULL);

static const struct i2c_device_id ardui2c_id[] = {
	{ "ardui2c", ARDUI2C_DEVICE_ADDR },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(i2c, ardui2c_id);

static const struct of_device_id ardui2c_of_match[] = {
	{ .compatible = "none,ardui2c", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, ardui2c_of_match); 

static struct i2c_driver ardui2c_driver = {
	.driver     = {
		.name = "ardui2c",
		//.pm = = ardui2c_pm_ops,
		.of_match_table = ardui2c_of_match,
	},
	.probe      = ardui2c_probe,
	.remove     = ardui2c_remove,
	.id_table   = ardui2c_id,
};
module_i2c_driver(ardui2c_driver);
MODULE_AUTHOR("João Antônio Cardoso <joao.maker@gmail.com>");
MODULE_DESCRIPTION("Ardui2c is an adc device based on arduino using I2C.");
MODULE_LICENSE("GPL");
