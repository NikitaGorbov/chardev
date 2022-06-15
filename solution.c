#include <linux/init.h>
#include <linux/limits.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nikita Gorbov <nikita.gorbov.2001@gmail.com>");
MODULE_DESCRIPTION("Char device driver");
MODULE_VERSION("0.01");
#define DEVICE_NAME "solution_node"

static int calculate_buf_size(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
static int major_num = 240;
static int a;
static int b;
module_param(b, int, 0660);
module_param(a, int, 0660);

static char *kernel_buf;
static int device_open_count;
static struct file_operations file_ops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release
};

static int calculate_buf_size(void)
{
	int temp = INT_MAX;
	int buf_size = 1;

	while (temp) {
		temp /= 10;
		buf_size++;
	}
	return buf_size;
}

static ssize_t device_read(struct file *flip, char __user *buffer, size_t len, loff_t *offset)
{
	int size;
	/* Read from file only once */
	if (*offset > 0)
		return 0;
	size = sprintf(kernel_buf, "%d", a + b);
	if (copy_to_user(buffer, kernel_buf, size))
		return -EFAULT;
	*offset += size;
	return size;
}

static ssize_t device_write(struct file *flip, const char *buffer, size_t len, loff_t *offset)
{
	printk(KERN_ALERT "Cannot write into device. Device is read-only.\n");
	return -EINVAL;
}

static int device_open(struct inode *inode, struct file *file)
{
	if (device_open_count)
		return -EBUSY;
	device_open_count++;
	try_module_get(THIS_MODULE);
	return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
	device_open_count--;
	module_put(THIS_MODULE);
	return 0;
}

static int __init solution_init(void)
{
	int res;

	kernel_buf = kmalloc(calculate_buf_size(), GFP_KERNEL);
	if (!kernel_buf) {
		return -ENOMEM;
	}
	res = register_chrdev(major_num, DEVICE_NAME, &file_ops);
	if (res < 0)
		printk(KERN_ALERT "Could not register device, error: %d\n", res);
	return res;
}

static void __exit solution_exit(void)
{
	kfree(kernel_buf);
	unregister_chrdev(major_num, DEVICE_NAME);
}

module_init(solution_init);
module_exit(solution_exit);
