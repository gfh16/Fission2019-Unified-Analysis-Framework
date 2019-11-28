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
	{ 0x41086e, "module_layout" },
	{ 0xcfdbba1a, "__register_chrdev" },
	{ 0xb2701ed1, "pci_get_subsys" },
	{ 0xd6ee688f, "vmalloc" },
	{ 0x3bd1b1f6, "msecs_to_jiffies" },
	{ 0x70b2af54, "create_proc_entry" },
	{ 0x859c6dc7, "request_threaded_irq" },
	{ 0x42c8de35, "ioremap_nocache" },
	{ 0x1fedf0f4, "__request_region" },
	{ 0x2107fe08, "pci_bus_write_config_word" },
	{ 0xde95d1ee, "pci_bus_read_config_word" },
	{ 0x97599d5, "dma_alloc_from_coherent" },
	{ 0xffc7c184, "__init_waitqueue_head" },
	{ 0x8e4a7c5b, "__mutex_init" },
	{ 0x884f145, "kmem_cache_alloc_trace" },
	{ 0xc288f8ce, "malloc_sizes" },
	{ 0xf3cc0b6c, "pci_enable_device" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0x8d8aacb9, "remove_proc_entry" },
	{ 0x37a0cba, "kfree" },
	{ 0x999e8297, "vfree" },
	{ 0x7c61340c, "__release_region" },
	{ 0x69a358a6, "iomem_resource" },
	{ 0xedc03953, "iounmap" },
	{ 0xf20dabd8, "free_irq" },
	{ 0x16305289, "warn_slowpath_null" },
	{ 0x3c061aef, "dma_release_from_coherent" },
	{ 0x43041aa8, "dma_ops" },
	{ 0x973873ab, "_spin_lock" },
	{ 0x3c2c5af5, "sprintf" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0xb2fd5ceb, "__put_user_4" },
	{ 0xa1c76e0a, "_cond_resched" },
	{ 0xe914e41e, "strcpy" },
	{ 0x2da418b5, "copy_to_user" },
	{ 0xf2a644fb, "copy_from_user" },
	{ 0xead3b192, "mutex_lock" },
	{ 0x9ccb2622, "finish_wait" },
	{ 0xd62c833f, "schedule_timeout" },
	{ 0x33d92f9a, "prepare_to_wait" },
	{ 0xc8b57c27, "autoremove_wake_function" },
	{ 0x7d0bea0b, "per_cpu__current_task" },
	{ 0x1ece28e2, "mutex_unlock" },
	{ 0x8ff4079b, "pv_irq_ops" },
	{ 0xecde1418, "_spin_lock_irq" },
	{ 0xb72397d5, "printk" },
	{ 0x642e54ac, "__wake_up" },
	{ 0x2e60bace, "memcpy" },
	{ 0xeae3dfd6, "__const_udelay" },
	{ 0x2036d7d2, "module_put" },
	{ 0xacdeb154, "__tracepoint_module_get" },
	{ 0x280f9f14, "__per_cpu_offset" },
	{ 0x9629486a, "per_cpu__cpu_number" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "9E8A6E95652F7FC6A7CA5DB");

static const struct rheldata _rheldata __used
__attribute__((section(".rheldata"))) = {
	.rhel_major = 6,
	.rhel_minor = 4,
};
