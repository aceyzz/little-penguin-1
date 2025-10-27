// SPDX-License-Identifier: GPL-2.0-only
#include <linux/init.h>
#include <linux/module.h>

static int __init hello_init(void)
{
	pr_info("Hello world from a04!\n");
	return 0;
}

static void __exit hello_exit(void)
{
	pr_info("Cleaning up module from a04.\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("cedmulle <cedmulle@student.42lausanne.ch>");
MODULE_DESCRIPTION("Assignment 04 - Hello World Kernel Module");

