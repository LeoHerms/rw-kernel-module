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
char msg[100];

static ssize_t my_read(struct file *file_pointer, char *user_space_buffer, size_t count, loff_t *offset) {
	size_t len = strlen(msg);

	if (*offset >= len) {
                return 0;
        }

	// Using this API
	// copy_to_user(destination, source, number of bytes)
	int result;
	result = copy_to_user(user_space_buffer, msg, len);
	printk("my_read: entry\n");

	*offset += len;

	return len;	// return the number of bytes that are written back to the user space
}

static ssize_t	my_write(struct file *file_pointer, const char *user_space_buffer, size_t count, loff_t *offset) {
	if (count >= 100) {
		return -ENOMEM;
	}
	// Use the copy_x_user API
	if (copy_from_user(msg, user_space_buffer, count)) {
		return -EFAULT;
	}								
	printk("my_write: entry\n");
	msg[count] = '\0';
	*offset += count;
	return count;
}

struct proc_ops driver_proc_ops = {		// Driver operations
	.proc_read = my_read,
	.proc_write = my_write
};

static int my_init (void) {	
	printk("my_init: entry\n");		// Logging entry
	custom_proc_node = proc_create("my_driver", 0666, NULL, &driver_proc_ops);	// Maybe add a check?
						
	if (custom_proc_node == NULL) {		// I think this is safe enough?
		printk("my_init: error");
		return -1;
	}

	strcpy(msg, "Hey there!\n");		// Make this safer (IDK if strcpy is all that safe)

	printk("my_init: exit\n");		// Logging exit
	return 0;
}

static void my_exit (void) {
	printk("my_exit: entry\n");		// Logging entry
	proc_remove(custom_proc_node);		
	printk("my_exit: exit\n");		// Logging exit
}

module_init(my_init);
module_exit(my_exit);
