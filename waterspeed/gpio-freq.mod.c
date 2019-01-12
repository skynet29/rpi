#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xc54e0a01, "module_layout" },
	{ 0x3ec8886f, "param_ops_int" },
	{ 0xf59f197, "param_array_ops" },
	{ 0x68d7dd81, "cdev_del" },
	{ 0xba0c8841, "class_destroy" },
	{ 0x91c99831, "device_destroy" },
	{ 0xb38a8029, "cdev_add" },
	{ 0x69c77033, "cdev_init" },
	{ 0xa0d5ada0, "device_create" },
	{ 0x7485e15e, "unregister_chrdev_region" },
	{ 0x1e093605, "__class_create" },
	{ 0x29537c9e, "alloc_chrdev_region" },
	{ 0x67c2fa54, "__copy_to_user" },
	{ 0x97255bdf, "strlen" },
	{ 0xb81960ca, "snprintf" },
	{ 0x43b0c9c3, "preempt_schedule" },
	{ 0x1d2e87c6, "do_gettimeofday" },
	{ 0xc5ae0182, "malloc_sizes" },
	{ 0x27e1a049, "printk" },
	{ 0xd6b8e852, "request_threaded_irq" },
	{ 0x65d6d0f0, "gpio_direction_input" },
	{ 0x47229b5c, "gpio_request" },
	{ 0x8836fca7, "kmem_cache_alloc" },
	{ 0xefd6cf06, "__aeabi_unwind_cpp_pr0" },
	{ 0x37a0cba, "kfree" },
	{ 0xfe990052, "gpio_free" },
	{ 0xf20dabd8, "free_irq" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "B34E3D43D1A08076CD14955");
