#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>

MODULE_LICENSE("GPL");						// Provide license
MODULE_AUTHOR("Leoncio Hermosillo <hermosilloleoncio@gmail.com");						
MODULE_DESCRIPTION("Currently, a simple read and write kernel module");
// Add to description later additions of
// - error handling and security
// - concurrency and synchronization
// - enhanced functionality (ioctl, seek, logging)
// - cleanup paths and robustness

static struct proc_dir_entry *custom_proc_node;
static char msg[102] = {0};				// It's 102 to allow for 100 characters to be fit in

static ssize_t my_read(struct file *file_pointer, char *user_space_buffer, size_t count, loff_t *offset) {
	size_t len = strlen(msg);
	size_t bytes_to_copy = min(count, len - *offset);

	if (copy_to_user(user_space_buffer, msg + *offset, bytes_to_copy)) {
		return -EFAULT;
	}	

	*offset += bytes_to_copy;
	return bytes_to_copy;
}

static ssize_t	my_write(struct file *file_pointer, const char *user_space_buffer, size_t count, loff_t *offset) {
	if (count >= sizeof(msg)) {	// Removing equals causes seg fault when reading back input of size 99
		return -ENOSPC;
	}
	
	if (!user_space_buffer) {
		return -EINVAL;		// Invalid argument
	}

	if (copy_from_user(msg, user_space_buffer, count)) {
		return -EFAULT;
	}

	msg[count] = '\0';
	*offset += count;
	return count;
}

struct proc_ops driver_proc_ops = {		// Driver operations
	.proc_read = my_read,
	.proc_write = my_write
};

static int __init my_init (void) {	
	printk("my_init: entry\n");		// Logging entry
	custom_proc_node = proc_create("my_driver", 0666, NULL, &driver_proc_ops);	// Maybe add a check?
						
	if (custom_proc_node == NULL) {		// I think this is safe enough?
		printk("my_init: error");
		return -1;
	}

	strscpy(msg, "Hey there!\n", sizeof(msg));		// Make this safer (IDK if strcpy is all that safe)

	printk("my_init: exit\n");		// Logging exit
	return 0;
}

static void __exit my_exit (void) {
	printk("my_exit: entry\n");		// Logging entry
	proc_remove(custom_proc_node);		
	printk("my_exit: exit\n");		// Logging exit
}

module_init(my_init);
module_exit(my_exit);
