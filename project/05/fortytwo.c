#include <linux/init.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/minmax.h>

#define DEV_NAME "fortytwo"
#define LOGIN "cedmulle"
#define LOGIN_LEN (sizeof(LOGIN) - 1)

// merci https://embetronicx.com/tutorials/linux/device-drivers/misc-device-driver/

static ssize_t ft_read(struct file *f, char __user *buf, size_t count, loff_t *ppos)
{
	size_t remain, n;

	if (*ppos >= LOGIN_LEN)
		return (0);
	remain = LOGIN_LEN - *ppos;
	n = min(count, remain);
	if (copy_to_user(buf, LOGIN + *ppos, n))
		return (-EFAULT);
	*ppos += n;
	return (n);
}

static ssize_t ft_write(struct file *f, const char __user *buf, size_t count, loff_t *ppos)
{
	char tmp[LOGIN_LEN + 1];

	if (count != LOGIN_LEN && count != LOGIN_LEN + 1)
		return (-EINVAL);
	if (copy_from_user(tmp, buf, count))
		return (-EFAULT);
	if (count == LOGIN_LEN + 1 && tmp[LOGIN_LEN] != '\n')
		return (-EINVAL);
	if (memcmp(tmp, LOGIN, LOGIN_LEN) != 0)
		return (-EINVAL);
	return (count);
}

static const struct file_operations ft_fops = {
	.owner = THIS_MODULE,
	.read = ft_read,
	.write = ft_write,
};

static struct miscdevice ft_misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEV_NAME,
	.fops = &ft_fops,
	.mode = 0666,
};

static int __init ft_init(void)
{
	int result = misc_register(&ft_misc);

	if (result) {
		pr_err("fortytwo: regsitration KO (%d)\n", result);
		return (result);
	}
	pr_info("fortytwo: registration OK\n");
	return (0);
}

static void __exit ft_exit(void)
{
	misc_deregister(&ft_misc);
	pr_info("fortytwo: deregistration OK\n");
	return ;
}

module_init(ft_init);
module_exit(ft_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("cedmulle <cedmulle@student.42lausanne.ch>");
MODULE_DESCRIPTION("Assignment 05 - misc device /dev/fortytwo");

