// SPDX-License-Identifier: GPL-2.0-only
#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/nsproxy.h>
#include <linux/rbtree.h>
#include <linux/slab.h>
#include <linux/path.h>
#include <linux/dcache.h>
#include <linux/string.h>
#include <linux/mount.h>
#include <fs/mount.h>

static int mymounts_show(struct seq_file *m, void *v)
{
	struct mnt_namespace *ns = current->nsproxy ? current->nsproxy->mnt_ns : NULL;
	struct rb_root mounts;
	struct mount *mnt, *tmp;
	char *buf;

	if (!ns)
		return 0;

	buf = kmalloc(PATH_MAX, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;
	mounts = ns->mounts;

	rbtree_postorder_for_each_entry_safe(mnt, tmp, &mounts, mnt_node) {
		struct path p = { .mnt = &mnt->mnt, .dentry = mnt->mnt.mnt_root };
		char *mp = d_path(&p, buf, PATH_MAX);
		const char *src;

		src = mnt->mnt_devname ? mnt->mnt_devname : mnt->mnt.mnt_sb->s_type->name;

		if (!IS_ERR(mp)) {
			const char *lab = src;

			seq_printf(m, "%-8s\t%s\n", lab, mp);
		}
	}
	kfree(buf);
	return 0;
}

static int mymounts_open(struct inode *inode, struct file *file)
{
	return single_open(file, mymounts_show, NULL);
}

static const struct proc_ops mymounts_ops = {
	.proc_open    = mymounts_open,
	.proc_read    = seq_read,
	.proc_lseek   = seq_lseek,
	.proc_release = single_release,
};

static int __init mymounts_init(void)
{
	pr_info("mymounts-09: module loaded\n");
	if (!proc_create("mymounts", 0444, NULL, &mymounts_ops))
		return -ENOMEM;
	return 0;
}

static void __exit mymounts_exit(void)
{
	remove_proc_entry("mymounts", NULL);
	pr_info("mymounts-09: module unloaded\n");
}

module_init(mymounts_init);
module_exit(mymounts_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("cedmulle <cedmulle@student.42lausanne.ch>");
MODULE_DESCRIPTION("Assignment 09 - /proc/mymounts (rbtree traversal)");

