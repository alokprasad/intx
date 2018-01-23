/*************************************************
Name : capsense_key_pad.c
Other: Vasu deva rao .K

This module is ment for to access the capsense key pad
through the i2c bus
*************************************************/


#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/irq.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/i2c.h>




#define CAPSENSE_INIT_CMD           {0x0A, 0x0A}
#define CAPSENSE_START_SCAN_CMD     {0x01, 0x01}
#define CAPSENSE_STOP_SCAN_CMD      {'S', 'S'}
#define CAPSENSE_BZR_ON_CMD         {0x03, 0x03}
#define CAPSENSE_BZR_OFF_CMD        {0x02, 0x02}


#define CAPSENSE_INIT_RESPONSE {0x0B, 0x0B}

#define CAPSENSE_CMD_LEN   0x02
#define CAPSENSE_DATA_LEN  0x02

//first  bit is for key pad init
#define CAPSENSE_INIT_STATE (0x01)

//2 nd and 3 ed bits for key pad scan states
#define CAPSENSE_START_SCAN ((0x01) << (0x01))
#define CAPSENSE_STOP_SCAN  ((0x02) << (0x01))

//4 th bit for buzzer on & off
#define CAPSENSE_BZR_ON     ((0x01) << (0x03))
#define CAPSENSE_BZR_OFF    ((0x00) << (0x03))


#define CAPSENSE_KEY_PAD_NAME   "Capsense_keyPad" 
#define CAPSENSE_KEY_PAD_ADDR   0x05

#define CAPSENSE_IRQ_LINE    IRQ_EINT9
#define CAPSENSE_IRQ_PIN     S3C2410_GPG1 
#define CAPSENSE_IRQ_NAME    "capsense_irq"
#define CAPSENSE_IRQ_TYPE    IRQ_TYPE_EDGE_BOTH 

static struct i2c_device_id capsense_key_pad_id[] = {
													{CAPSENSE_KEY_PAD_NAME, CAPSENSE_KEY_PAD_ADDR},
													{ },
													};

MODULE_DEVICE_TABLE (i2c, capsense_key_pad_id);

struct capsense_irq_desc
{
   int irq;
   int pin;
   int irq_type;
   char *irq_name;
};

struct i2c_capsense_key_pad
{
	struct capsense_irq_desc  irq_desc;
	struct mutex              capsense_mutex;
	struct i2c_client         *capsense_client;
	unsigned char             capsense_cmd[CAPSENSE_CMD_LEN];
	unsigned char             capsense_data[CAPSENSE_DATA_LEN];
	atomic_t                  capsense_state;
	atomic_t                  capsense_refcount;
	int                       capsense_address;
};

struct i2c_capsense_key_pad *capsen_prv;

static DECLARE_WAIT_QUEUE_HEAD(capsense_waitq);


static int capsense_probe(struct i2c_client *client, const struct i2c_dev_id *id)
{

		if(id->driver_data != CAPSENSE_KEY_PAD_ADDR)
			return -EINVAL;

		if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C))
            return -EIO;

	    capsen_prv = kzmalloc(sizeof(struct capsense_key_pad), GFP_KERNEL);
		if(!capsen_prv)
			return -ENOMEM;

		capsen_prv->irq_desc.irq        = CAPSENSE_IRQ_LINE; 
		capsen_prv->irq_desc.pin        = CAPSENSE_IRQ_PIN; 
		capsen_prv->irq_desc.irq_type   = CAPSENSE_IRQ_TYPE;
		capsen_prv->irq_desc.irq_name   = CAPSENSE_IRQ_NAME;

		mutex_init(&capsen_prv->capsense_mutex);

		capsen_prv->capsense_address    = id->driver_data;
		capsen_prv->capsense_client     = client;

		i2c_set_clientdata(client, capsen_prv);

		if(misc_register(&misc) == 0)
			return -1;

		return 0;
}

static int capsense_key_pad_open(struct *inode, struct file *file)
{
  int err = 0;
  unsigned char init_cmd[] = CAPSENSE_INIT_CMD;

	mutex_lock(capsen_prv->capsense_mutex);
	if(atomic_read(&capsen_prv->capsense_state) & CAPSENSE_INIT_STATE) 
	{
		atomic_inc(&capsen_prv->capsense_refcount);
		mutex_unlock(capsen_prv->capsense_mutex);
		return 0;
	}

	err = request_irq(capsen_prv->irq_desc.irq, capsense_irq_hndlr, capsen_prv->irq_desc.irq_type|SA_SHIRQ, 
					  capsen_prv->irq_desc.irq_name, (void*)&capsen_prv->irq_desc);
	if(err)
	 	goto fail;

	//send init cmd
	 err = i2c_master_send(capsen_prv->client, init_cmd, CAPSENSE_CMD_LEN);
	 if(err)
	 	goto fail;
	
	// wait for interrupt 
	err = wait_event_interruptible_timeout(capsense_waitq, 0, HZ * 0.1);
	if(err)
	{
		err = -ETIMEDOUT;
	 	goto fail;
	}

	err = wait_for_irq_pin_high(button_irqs->pin)
	if(err)
		goto fail;

	// read the init response 
	err = i2c_master_recv(capsen_prv->client, capsen_prv->capsense_data, CAPSENSE_DATA_LEN);
	if(err)
		goto fail;

	err = check_capsense_init_resp(capsense_prv->capsense_data, CAPSENSE_DATA_LEN);
	if(err)
		goto fail;
	
	file->private_data = (void *)capsense_prv;

	// increment the counter 
	atomic_inc(&capsen_prv->capsense_refcount);

	//int as init done 
	atomic_set(&capsen_prv->capsense_state, CAPSENSE_INIT_STATE);

	// release mutux
	mutex_unlock(capsen_prv->capsense_mutex);

	return 0;

	fail:
		mutex_unlock(capsen_prv->capsense_mutex);
		return err;
}

static int capsense_close(struct inode *inode, struct file *file)
{
	struct i2c_capsense_key_pad *capsen_data = (struct i2c_capsense_key_pad*)file->private_data;

	if(!capsen_data)
		return -1;

	mutex_lock(capsen_data->capsense_mutex);
	if(atomic_read(&capsen_data->capsense_state) & CAPSENSE_INIT_STATE) 
	{
		atomic_dec(&capsen_data->capsense_refcount);
		if(atomic_read(&capsen_data->capsense_refcount) == 0)
		{
			free_irq(capsen_data->irq_desc.irq, (void *)&capsen_data->irq_desc);
			atomic_read(&capsen_data->capsense_refcount, 0);
			file->private_data = NULL;
		}
	}
	mutex_unlock(capsen_data->capsense_mutex);

	return 0;
}

//needs to implement capsense_read
//needs to implement capsense_ioctl

static struct file_operations dev_fops = {
	.owner	=   THIS_MODULE,
	.open	=   capsense_key_pad_open,
	.release	=  capsense_close , 
	.read	=   capsense_read,
	.ioctl =    capsense_ioctl,
};

static struct miscdevice misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "capsense-keypad"; 
	.fops = &dev_fops,
};

static int capsense_remove(struct i2c_client *client)
{
	struct i2c_capsense_key_pad *capsen_data = i2c_get_clientdata(client);

	misc_deregister(&misc);
	kfree(capsen_data);
	return 0;
}

static int wait_for_irq_pin_high(int pin)
{
	unsigned long start = jiffies;
	unsigned long end  =  jiffies + (HZ *2); 

	while(!timer_after(start, end))
	{
	 	if(s3c2410_gpio_getpin(pin))
			return 0;
	}
	return -1;
}

static irqreturn_t capsense_irq_hndlr(int irq, void *irq_data)
{
  struct capsense_irq_desc *irq_desc = (struct capsense_irq_desc *)irq_data;
  int down;

  down = s3c2410_gpio_getpin(irq_desc->pin);
  if(down == 0)
  {
  	wake_up_interruptible(&capsense_waitq);
  }

  return IRQ_RETVAL(IRQ_HANDLED);
}


static int check_capsense_init_resp(unsigned char *data, int len)
{
	unsigned char init_resp[] = CAPSENSE_INIT_RESPONSE;

	if(data[0] == init_resp[0] && data[1] == init_resp[1])
		return 0;
	
	return -1;
}

static struct i2c_driver capsense_driver = {
	.driver = {
		.name = "capsense-keypad";
		.owner = THIS_MODULE;
	}
	.probe    = capsense_probe;
	.remove   = capsense_remove;
	.id_table = capsense_key_pad_id;
};

static int capsense_init(void)
{
	/*Registering a Chip Driver
     To register this I2C chip driver, the function      	i2c_add_driver should be called with a pointer to the struct 	i2c_driver:*/

	return i2c_add_driver(&capsense_driver);
}

static void capsense_exit(void)
{
	/* unregister the driver*/
	i2c_del_driver(&capsense_driver);	
}

module_init(capsense_init);
module_exit(capsense_exit);

