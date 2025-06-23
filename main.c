#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/random.h>

#define MAX_DEPTH 4

struct proc_entry {
    struct task_struct *thread;
    int depth;
    pid_t parent_id;
    struct list_head list;
};

struct dob_entry {
    int day, month, year;
    struct list_head list;
};

static LIST_HEAD(proc_list);
static LIST_HEAD(dob_list);

static void display_process_hierarchy(struct proc_entry *entry, int level) {
    char prefix[32] = {0};
    int i;
    for (i = 0; i < level; i++)
        strcat(prefix, " ");
    printk(KERN_INFO "%s|- Process (PID: %d, Parent: %d, Depth: %d)\n", prefix, entry->thread->pid, entry->parent_id, entry->depth);
}

static int process_thread_fn(void *arg) {
    struct proc_entry *entry = (struct proc_entry *)arg;
    display_process_hierarchy(entry, entry->depth);
    while (!kthread_should_stop())
        ssleep(1);
    return 0;
}

static void spawn_process(int depth, pid_t parent_id) {
    if (depth >= MAX_DEPTH)
        return;

    struct proc_entry *entry = kmalloc(sizeof(struct proc_entry), GFP_KERNEL);
    if (!entry)
        return;

    entry->depth = depth;
    entry->parent_id = parent_id;
    entry->thread = kthread_run(process_thread_fn, entry, "custom_process_%d", depth);
    list_add_tail(&entry->list, &proc_list);

    if (depth < MAX_DEPTH - 1)
        spawn_process(depth + 1, entry->thread->pid);
}

static void generate_dob(int *day, int *month, int *year) {
    unsigned int r_day, r_month, r_year;
    get_random_bytes(&r_year, sizeof(r_year));
    get_random_bytes(&r_month, sizeof(r_month));
    get_random_bytes(&r_day, sizeof(r_day));

    *year = 1950 + (r_year % 75);
    *month = 1 + (r_month % 12);

    int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if ((*month == 2) && ((*year % 4 == 0 && *year % 100 != 0) || (*year % 400 == 0)))
        days_in_month[1] = 29;

    *day = 1 + (r_day % days_in_month[*month - 1]);
}

static void create_dob_entries(void) {
    for (int i = 0; i < 5; i++) {
        struct dob_entry *entry = kmalloc(sizeof(struct dob_entry), GFP_KERNEL);
        if (!entry)
            return;

        generate_dob(&entry->day, &entry->month, &entry->year);
        list_add_tail(&entry->list, &dob_list);
        printk(KERN_INFO "[PID: %d] Created DOB: %d/%d/%d\n", current->pid, entry->day, entry->month, entry->year);
    }
}

static void cleanup_processes(void) {
    struct proc_entry *entry, *tmp;
    list_for_each_entry_safe_reverse(entry, tmp, &proc_list, list) {
        printk(KERN_INFO "|- Removing Process (PID: %d)\n", entry->thread->pid);
        kthread_stop(entry->thread);
        list_del(&entry->list);
        kfree(entry);
    }
}

static void cleanup_dobs(void) {
    struct dob_entry *entry, *tmp;
    list_for_each_entry_safe(entry, tmp, &dob_list, list) {
        list_del(&entry->list);
        kfree(entry);
    }
}

static int __init module_start(void) {
    printk(KERN_INFO "Jackfruit Kernel Module Loaded\n");
    spawn_process(0, 1);  // Start with init process as parent
    create_dob_entries();
    return 0;
}

static void __exit module_end(void) {
    cleanup_processes();
    cleanup_dobs();
    printk(KERN_INFO "Jackfruit Kernel Module Removed\n");
}

module_init(module_start);
module_exit(module_end);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("SHIVAM ANAND - PES2UG23CS549");
MODULE_DESCRIPTION("Kernel Module for Process and DOB Management");
