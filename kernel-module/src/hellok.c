#include "asm/string_64.h"
#include "linux/jiffies.h"
#include "linux/kobject.h"
#include "linux/printk.h"
#include "linux/sysfs.h"
#include <linux/kernel.h> 
#include <linux/module.h> 
#include <linux/proc_fs.h> 
#include <linux/uaccess.h> 
#include <linux/version.h> 
#include <linux/timer.h>
 
#define PROCFS_STRING "Hello from kernel module!\n"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ilya Anufriev");
MODULE_DESCRIPTION("Periodically writes a string to the /proc file.");

#define PROCFS_FN_SIZE 32

#define INFTC_MODULE_NAME "infotecs_module"

// /proc directory entry created by proc_create
static struct proc_dir_entry *out_proc_file; 

// entry used for secure /proc filename change
struct proc_dir_entry *swap_proc_file;

// amount of lines that are going to be written into the /proc/hellok  
static int line_count = 0;

// timer that calls timer_callback function 
static struct timer_list k_timer;

// procfs output file name
static char procfs_filename[PROCFS_FN_SIZE + 1] = "hellok";

// buffer filename is used when reading the name from 
// /sys/.../proc_file_name file to avoid overwriting procfs_filename
// with potentially invalid file name
static char buffer_filename[sizeof(procfs_filename)];

// time period between /proc/hellok file writes
// is exposed to sys. Default value is 1;
static int timer_seconds = 1;

// function that exposes local variable timer_seconds to user
static ssize_t timer_seconds_show(struct kobject *kobj,
                                  struct kobj_attribute *attr,
                                  char *buf);

// function that stores user input into the local variable timer_seconds
static ssize_t timer_seconds_store(struct kobject *kobj,
                                   struct kobj_attribute *attr,
                                   const char *buf,
                                   size_t count);

// each timer_seconds seconds timer this function will increment
// line_count in the output file and reinitialize timer
static void timer_callback(struct timer_list *timer);

// this function will output a certain amount of lines of the PROCFS_STRING
// if procfs file will be read.
static ssize_t procfile_read(struct file *file_pointer, char __user *buffer, 
                             size_t buffer_length, loff_t *offset);

// proc_ops defines which operations with procfs file are possible.
// we only define read operation since write is useless. 
static const struct proc_ops proc_file_fops = { 
    .proc_read = procfile_read, 
}; 

// this function gets called every time user writes to the 
// /sys/kernel/<module_name>/proc_file_name
static ssize_t procfs_filename_store(struct kobject *kobj,
                                     struct kobj_attribute *attr,
                                     const char *buf,
                                     size_t count);

// this function gets called every time user reads from the 
// /sys/kernel/<module_name>/proc_file_name
static ssize_t procfs_filename_show(struct kobject *kobj,
                                    struct kobj_attribute *attr,
                                    char *buf);

// defining a kobject used to expose local variables to /sys dir
static struct kobject *module_kobj;

// defining timer_seconds attributes
static struct kobj_attribute timer_seconds_attr = 
    __ATTR(timer_seconds, 0660, &timer_seconds_show, &timer_seconds_store);

// defining procfs_filename attributes
static struct kobj_attribute procfs_filename_attr = 
    __ATTR(proc_file_name, 0660, &procfs_filename_show, &procfs_filename_store);

static int __init hellok_init(void) 
{ 
    pr_info("Initializing hellok");

	// initialize default /proc entry
    out_proc_file = proc_create(procfs_filename, 
                                0644, 
                                NULL, 
                                &proc_file_fops); 
    
    // if initialization fails, exit with an error code
    if (out_proc_file == NULL) { 
        pr_alert("Error:Could not initialize /proc/%s\n", procfs_filename); 
        return -ENOMEM; 
    }

    pr_info("/proc/%s created\n", procfs_filename);

    // initialize kobject
    module_kobj = kobject_create_and_add(INFTC_MODULE_NAME, kernel_kobj);

    // if initialization fails, wrap it up and exit
    if (module_kobj == NULL) {
        pr_alert("Error: Could not initialize kobject \"%s\"\n", 
                 module_kobj->name
            );
        return -ENOMEM;
    }

    // create /sys dir entry for timer_seconds. If it fails, exit
    if (sysfs_create_file(module_kobj, &timer_seconds_attr.attr)) {
        pr_alert("Error: Could not create /sys entry for %s\n",
                timer_seconds_attr.attr.name
            );

        return -ENOMEM;
    }

    // create /sys dir entry for procfs_filename. If it fails, exit
    if (sysfs_create_file(module_kobj, &procfs_filename_attr.attr)) {
        pr_alert("Error: Could not create /sys entry for %s\n",
                procfs_filename_attr.attr.name
            );

        return -ENOMEM;
    }

	// initialilze timer
    timer_setup(&k_timer, timer_callback, 0);

    mod_timer(&k_timer, jiffies + msecs_to_jiffies(timer_seconds));
    
    return 0; 
} 

static void __exit hellok_exit(void) 
{
    proc_remove(out_proc_file);
    del_timer(&k_timer);
    kobject_put(module_kobj);
}

module_init(hellok_init);
module_exit(hellok_exit);

static ssize_t timer_seconds_show(struct kobject *kobj,
                                  struct kobj_attribute *attr,
                                  char *buf) 
{
    return sprintf(buf, "%d\n", timer_seconds);
}

static ssize_t timer_seconds_store(struct kobject *kobj,
                                   struct kobj_attribute *attr,
                                   const char *buf,
                                   size_t count) 
{
    int timer_seconds_buf;

    sscanf(buf, "%du", &timer_seconds_buf);

    if (timer_seconds_buf <= 0) {
        pr_info("timer_seconds <= 0, not changed.\n");
        return count;
    }

    timer_seconds = timer_seconds_buf;
    pr_info("timer_period changed by user. ");
    pr_info("Now using %d seconds for timer_period.\n", timer_seconds);

    return count;
}

static void timer_callback(struct timer_list *timer) 
{
    line_count++;
    mod_timer(timer, jiffies + msecs_to_jiffies(timer_seconds * 1000));
}

static ssize_t procfile_read(struct file *file_pointer, char __user *buffer, 
                             size_t buffer_length, loff_t *offset) 
{
    // basically, lines are not contained anywhere, it's just one
    // line that gets printed line_count times
    int len = sizeof(PROCFS_STRING); 

    ssize_t end = len * line_count;

    if (*offset >= end) { 
        return 0; 
    }

    // copy the string to userspace memory
    if (copy_to_user(buffer, PROCFS_STRING, len)) {
        pr_info("copy_to_user failed\n"); 
        return 0;
    }

    *offset += len;
    
    // returning non-zero value will make this function to be called
    // again in a loop untill zero is returned. Since we have break 
    // conditions, we can just return len until EOF is hit or copy_to_user
    // fails. 
    return len; 
}

static ssize_t procfs_filename_store(struct kobject *kobj,
                                     struct kobj_attribute *attr,
                                     const char *buf,
                                     size_t count) 
{
    // if the name is too long - discard the request.
    if (count > PROCFS_FN_SIZE) {
        pr_alert("Error: new filename too long, not changed.\n");
        return count;
    }

    sscanf(buf, "%s", buffer_filename);

    swap_proc_file = proc_create(
            buffer_filename, 0644, NULL, &proc_file_fops
        );

    // proc_create can fail if directory already exists. 
    // It will print debug info into the log, but the kernel module 
    // itself will handle the error and will not fail it's execution 
    if (swap_proc_file == NULL) { 
        pr_alert("Error: Could not change proc directory to /proc/%s\n",
                buffer_filename
            );
        return count;  
    } 

    strcpy(procfs_filename, buffer_filename);

    pr_info("Now using /proc/%s as an output file.\n", procfs_filename);

    swap(out_proc_file, swap_proc_file);

    proc_remove(swap_proc_file);
   
    // reset line count to maintain the illusion that the vfs file was 
    // actually changed to another one
    line_count = 0;

    return count;
}

static ssize_t procfs_filename_show(struct kobject *kobj,
                                    struct kobj_attribute *attr,
                                    char *buf) 
{
    return sprintf(buf, "%s\n", procfs_filename);
}