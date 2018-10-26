#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>

#define PROC_NAME "task_list"

/* This function is called when the module is loaded. */
static int mod_init(void)
{

	struct task_struct *tsk;
	printk(KERN_INFO "mod %s created\n", PROC_NAME);
	printk(KERN_INFO "Task list\n");

	// start from init process
	tsk = pid_task(find_vpid(1), PIDTYPE_PID);
	printk(KERN_INFO "%20s\t%20s\t%20s\n", "CMD", "PID", "STATE");
	for_each_process(tsk) {
		printk( KERN_INFO "%20s\t%20d\t%20ld\n", tsk->comm, tsk->pid, tsk->state);
	}

	return 0;
}

/* This function is called when the module is removed. */
static void mod_exit(void) {
	printk( KERN_INFO "mod %s removed\n", PROC_NAME);
}

/* Macros for registering module entry and exit points. */
module_init( mod_init );
module_exit( mod_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Task List Module");
MODULE_AUTHOR("SGG");
