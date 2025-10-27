// SPDX-License-Identifier: GPL-2.0-only
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/minmax.h>
#include <linux/debugfs.h>
#include <linux/jiffies.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/capability.h>

#define LOGIN "cedmulle"
#define LOGIN_LEN (sizeof(LOGIN) - 1)

static struct dentry *ft_dir;

// pour foo (mutex pour prendre le sujet a la lettre)
static char *foo_str;
static size_t foo_len;
static DEFINE_MUTEX(foo_lock);

// specifique a id ////////////////////////////////////////////////////////////////////////
static ssize_t ft_read(struct file *f, char __user *buf, size_t count, loff_t *ppos)
{
	size_t remain, n;

	if (*ppos >= LOGIN_LEN)
		return 0;
	remain = LOGIN_LEN - *ppos;
	n = min(count, remain);
	if (copy_to_user(buf, LOGIN + *ppos, n))
		return -EFAULT;
	*ppos += n;
	return n;
}

static ssize_t ft_write(struct file *f, const char __user *buf, size_t count, loff_t *ppos)
{
	char tmp[LOGIN_LEN + 1];

	if (count != LOGIN_LEN && count != LOGIN_LEN + 1)
		return -EINVAL;
	if (copy_from_user(tmp, buf, count))
		return -EFAULT;
	if (count == LOGIN_LEN + 1 && tmp[LOGIN_LEN] != '\n')
		return -EINVAL;
	if (memcmp(tmp, LOGIN, LOGIN_LEN) != 0)
		return -EINVAL;
	return count;
}

static const struct file_operations ft_fops = {
	.owner = THIS_MODULE,
	.read = ft_read,
	.write = ft_write,
};

// specifique a jiffies ///////////////////////////////////////////////////////////////////
static ssize_t jiffies_read(struct file *f, char __user *buf, size_t count, loff_t *ppos)
{
	char tmp[32];
	int len = scnprintf(tmp, sizeof(tmp), "%lu\n", jiffies);

	return simple_read_from_buffer(buf, count, ppos, tmp, len);
}

static const struct file_operations jiffies_fops = {
	.owner  = THIS_MODULE,
	.read   = jiffies_read,
	.llseek = default_llseek,
};

// specifique a foo ///////////////////////////////////////////////////////////////////////
static ssize_t foo_read(struct file *f, char __user *buf, size_t count, loff_t *ppos)
{
	ssize_t ret;

	if (mutex_lock_interruptible(&foo_lock))
		return -ERESTARTSYS;
	ret = simple_read_from_buffer(buf, count, ppos, foo_str, foo_len);
	mutex_unlock(&foo_lock);
	return ret;
}

static ssize_t foo_write(struct file *f, const char __user *buf, size_t count, loff_t *ppos)
{
	ssize_t ret;
	loff_t pos = 0;

	if (count > PAGE_SIZE)
		return -EINVAL;
	if (!capable(CAP_SYS_ADMIN))
		return -EACCES;
	if (mutex_lock_interruptible(&foo_lock))
		return -ERESTARTSYS;
	foo_len = 0;
	ret = simple_write_to_buffer(foo_str, PAGE_SIZE, &pos, buf, count);
	if (ret >= 0)
		foo_len = ret;
	mutex_unlock(&foo_lock);
	return ret;
}

static const struct file_operations foo_fops = {
	.owner = THIS_MODULE,
	.read  = foo_read,
	.write = foo_write,
	.llseek = default_llseek,
};

// module init/exit ///////////////////////////////////////////////////////////////////////
static int __init ft_init(void)
{
	// creer le repertoire fortytwo dans debugfs
	ft_dir = debugfs_create_dir("fortytwo", NULL);
	if (!ft_dir) {
		pr_err("debugfs-fortytwo: create_dir failed\n");
		return -ENOMEM;
	}
	pr_info("debugfs-fortytwo: dir created\n");
	// creer le fichier id en lecture/ecriture
	if (!debugfs_create_file("id", 0644, ft_dir, NULL, &ft_fops)) {
		pr_err("debugfs-fortytwo: create_file(id) failed\n");
		debugfs_remove_recursive(ft_dir);
		return -ENOMEM;
	}
	pr_info("debugfs-fortytwo: id file created\n");
	// creer le fichier jiffies en lecture seule
	if (!debugfs_create_file("jiffies", 0444, ft_dir, NULL, &jiffies_fops)) {
		pr_err("debugfs-fortytwo: create_file(jiffies) failed\n");
		debugfs_remove_recursive(ft_dir);
		return -ENOMEM;
	}
	pr_info("debugfs-fortytwo: jiffies file created\n");
	// init foo
	foo_str = kmalloc(PAGE_SIZE, GFP_KERNEL);
	if (!foo_str) {
		debugfs_remove_recursive(ft_dir);
		return -ENOMEM;
	}
	pr_info("debugfs-fortytwo: foo buffer allocated\n");
	// creer le fichier foo en lecture/ecriture
	foo_len = 0;
	if (!debugfs_create_file("foo", 0644, ft_dir, NULL, &foo_fops)) {
		pr_err("debugfs-fortytwo: create_file(foo) failed\n");
		kfree(foo_str);
		debugfs_remove_recursive(ft_dir);
		return -ENOMEM;
	}
	pr_info("debugfs-fortytwo: foo file created\n");
	return 0;
}

static void __exit ft_exit(void)
{
	kfree(foo_str);
	pr_info("debugfs-fortytwo: foo freed memory\n");
	debugfs_remove_recursive(ft_dir);
	pr_info("debugfs-fortytwo: dir removed\n");
}

module_init(ft_init);
module_exit(ft_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("cedmulle <cedmulle@student.42lausanne.ch>");
MODULE_DESCRIPTION("Assignment 07 - debugfs fortytwo");
