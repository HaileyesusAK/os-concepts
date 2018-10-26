#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/sched.h>

#define BUFFER_SIZE 128
#define PROC_NAME "pid"

static ssize_t proc_read(struct file *file, char __user *buf, size_t count, loff_t *pos);
static ssize_t proc_write(struct file *file, const char *buf, size_t count, loff_t *pos);

static struct file_operations proc_ops = {
	.owner = THIS_MODULE,
	.read  = proc_read,
	.write = proc_write
};

static long pid;

/* This function is called when the module is loaded. */
static int proc_init(void)
{

	// creates the /proc/pid entry
	// the following function call is a wrapper for
	// proc_create_data() passing NULL as the last argument
	proc_create(PROC_NAME, 0, NULL, &proc_ops);
	printk(KERN_INFO "/proc/%s created\n", PROC_NAME);

	return 0;
}

/* This function is called when the module is removed. */
static void proc_exit(void) {

	// removes the /proc/jiffies entry
	remove_proc_entry(PROC_NAME, NULL);

	printk( KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

/**
 * This function is called each time /proc/pid is read.
 *
 * This function is called repeatedly until it returns 0, so
 * there must be logic that ensures it ultimately returns 0
 * once it has collected the data that is to go into the
 * corresponding /proc file.
 */

static ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
{
	int rv = 0;
	char buffer[BUFFER_SIZE];
	static int completed = 0;
	struct task_struct* tsk;

	if (completed) {
			completed = 0;
			return 0;
	}
	completed = 1;
	tsk = pid_task(find_vpid(pid), PIDTYPE_PID);
	if(tsk) {
		rv = sprintf(buffer, "%s\t%ld\t%ld\n", tsk->comm, pid, tsk->state);

		// copies the contents of buffer to userspace usr_buf
		if(copy_to_user(usr_buf, buffer, rv))
			rv = -1;
	}

	return rv;
}

/**
 * This function is called each time we write to the /proc file system.
 */

static ssize_t proc_write(struct file *file, const char *usr_buf, size_t count, loff_t *pos)
{
	/* allocate kernel memory */
	char *k_mem = kmalloc(count, GFP_KERNEL);

	/* copies user space usr_buf to kernel memory */
	if(copy_from_user(k_mem, usr_buf, count)) {
		printk(KERN_INFO "Error copying from user\n");
		return -1;
	}

	printk(KERN_INFO "%s∖n", k_mem);

	/* extract and save the process id from the string */
	sscanf(k_mem, "%ld", &pid);

	/* return kernel memory */
	kfree(k_mem);

	return count;
}

/* Macros for registering module entry and exit points. */
module_init( proc_init );
module_exit( proc_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Pid Module");
MODULE_AUTHOR("SGG");
