#include <linux/delay.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/timekeeping.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shapovalov Vitaliy");
MODULE_DESCRIPTION("The timer saves the current time every minute in /tmp/current_time.txt in hh:mm format");
MODULE_VERSION("0.01");

#define PATH "/tmp/current_time.txt"	/* .txt format is optional */

struct task_struct *ts;

static int timer_thread(void *arg)
{
	struct file *filp;
	ktime_t tv;
	int hr, min, oldmin;
	char string[20];

	(void)arg;

	while(!kthread_should_stop()) {
		filp = filp_open(PATH, O_CREAT | O_RDWR, 0666);
		if (IS_ERR(filp)) {
			return -1;
		}

		tv = ktime_get_real() / 1000000000;
		hr = (tv / 3600 + 3) % 24;		/* msk time */
		min = (tv / 60) % 60;

		sprintf(string, "%d:%d", hr, min);

		if (oldmin != min) {
			kernel_write(filp, string, strlen(string), filp->f_pos);	/* \0 is not needed */
		}

		filp_close(filp, NULL);

		oldmin = min;
		msleep(1000);
	}

	return 0;
}

static int __init kmodule_init(void)
{
	ts = kthread_create(timer_thread, NULL, "timer");
	if (ts) {
		wake_up_process(ts);
	} else {
		return -1;
	}

	return 0;
}

static void __exit kmodule_exit(void)
{
	kthread_stop(ts);
}

module_init(kmodule_init);
module_exit(kmodule_exit);