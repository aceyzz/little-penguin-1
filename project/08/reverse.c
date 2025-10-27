// SPDX-License-Identifier: GPL-2.0-only
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/uaccess.h>
#include <linux/string.h>

#define DEVNAME "reverse"

static DEFINE_MUTEX(buf_lock);
static char buf[PAGE_SIZE];
static size_t buf_len;

static ssize_t myfd_read(struct file *fp, char __user *ubuf, size_t len, loff_t *ppos)
{
	ssize_t ret;
	char *tmp;
	size_t i;

	mutex_lock(&buf_lock);
	if (!buf_len) {
		mutex_unlock(&buf_lock);
		return 0;
	}
	tmp = kmalloc(buf_len + 1, GFP_KERNEL);

	if (!tmp) {
		mutex_unlock(&buf_lock);
		return -ENOMEM;
	}
	for (i = 0; i < buf_len; i++)
		tmp[i] = buf[buf_len - 1 - i];
	tmp[buf_len] = '\0';
	ret = simple_read_from_buffer(ubuf, len, ppos, tmp, buf_len);

	kfree(tmp);
	mutex_unlock(&buf_lock);
	return ret;
}

static ssize_t myfd_write(struct file *filp, const char __user *ubuf, size_t len, loff_t *ppos)
{
	ssize_t ret;
	size_t to_copy;

	mutex_lock(&buf_lock);

	to_copy = min(len, (size_t)(PAGE_SIZE - 1));
	*ppos = 0;
	ret = simple_write_to_buffer(buf, PAGE_SIZE - 1, ppos, ubuf, to_copy);

	if (ret >= 0) {
		buf_len = ret;
		buf[buf_len] = '\0';
	}
	mutex_unlock(&buf_lock);
	return ret;
}

static const struct file_operations myfd_fops = {
	.owner = THIS_MODULE,
	.read = myfd_read,
	.write = myfd_write,
	.llseek = noop_llseek,
};

static struct miscdevice myfd_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVNAME,
	.fops = &myfd_fops,
};

static int __init myfd_init(void)
{
	int ret;

	ret = misc_register(&myfd_device);

	if (ret)
		return ret;
	return 0;
}

static void __exit myfd_cleanup(void)
{
	misc_deregister(&myfd_device);
}

module_init(myfd_init);
module_exit(myfd_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Louis Solofrizzo <louis@ne02ptzero.me>");
MODULE_DESCRIPTION("Reverse buffer misc device");

