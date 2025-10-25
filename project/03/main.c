// SPDX-License-Identifier: GPL-2.0-only
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/slab.h>

static int do_work(int *my_int, int retval)
{
	int x;
	int y = *my_int;
	int z;

	for (x = 0; x < *my_int; ++x)
		usleep_range(10, 20);
	if (y < 10)
		pr_info("We slept a long time!");
	z = x * y;
	return z;
}

static int __init my_init(void)
{
	int x = 10;

	do_work(&x, x);
	return x;
}

static void __exit my_exit(void)
{
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("cedmulle@student.42lausanne.ch");
MODULE_DESCRIPTION("Assignment 03 - style-compliant example");
