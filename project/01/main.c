// SPDX-License-Identifier: GPL-2.0-only
#include <linux/init.h>
#include <linux/module.h>

// merci https://fennecj.github.io/lkmpg/

static int __init hello_init(void)
{
	pr_info("Hello world!\n");
	return 0;
}

static void __exit hello_exit(void)
{
	pr_info("Cleaning up module.\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("cedmulle <cedmulle@student.42lausanne.ch>");
MODULE_DESCRIPTION("Assignment 01 - Hello World Kernel Module");
