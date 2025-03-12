#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(.gnu.linkonce.this_module) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section(__versions) = {
	{ 0x6e5e9ea5, "module_layout" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0xb84635fe, "device_destroy" },
	{ 0xe9e73cc4, "__register_chrdev" },
	{ 0xfb578fc5, "memset" },
	{ 0xc5850110, "printk" },
	{ 0x4c9d28b0, "phys_base" },
	{ 0xef4606af, "device_create" },
	{ 0x7cd8d75e, "page_offset_base" },
	{ 0xb8b9f817, "kmalloc_order_trace" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x37a0cba, "kfree" },
	{ 0x49ce5766, "remap_pfn_range" },
	{ 0x201d0f70, "class_destroy" },
	{ 0xa640d12, "__class_create" },
	{ 0x6228c21f, "smp_call_function_single" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "82A10BCABA7E6E0FAC04070");
