#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
MODULE_LICENSE("GPL");



static irqreturn_t sample_irq(int irq, void *dev_id)
{
	printk("irq %d\n", irq);

	return IRQ_RETVAL(1);
}

int init_module( void )
{

  request_irq(1, sample_irq, IRQF_SHARED, "sample", 0);	
  return 0;
}

void cleanup_module( void )
{
  /* Stop the tasklet before we exit */

  return;
}
