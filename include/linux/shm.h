#ifndef _LINUX_SHM_H_
#define _LINUX_SHM_H_

#include <linux/list.h>
#include <asm/page.h>
#include <uapi/linux/shm.h>
#include <asm/shmparam.h>

struct shmid_kernel /* private to the kernel */
{	
	struct kern_ipc_perm	shm_perm;
	struct file		*shm_file;
	unsigned long		shm_nattch;
	unsigned long		shm_segsz;
	time_t			shm_atim;
	time_t			shm_dtim;
	time_t			shm_ctim;
	pid_t			shm_cprid;
	pid_t			shm_lprid;
	struct user_struct	*mlock_user;

	/*
	 * The task created the shm object, for
	 * task_lock(shp->shm_creator)
	 */
	struct task_struct	*shm_creator;

	/*
	 * List by creator. task_lock(->shm_creator) required for read/write.
	 * If list_empty(), then the creator is dead already.
	 */
	struct list_head	shm_clist;
	struct ipc_namespace	*ns;
};

/* shm_mode upper byte flags */
#define	SHM_DEST	01000	/* segment will be destroyed on last detach */
#define SHM_LOCKED      02000   /* segment will not be swapped */
#define SHM_HUGETLB     04000   /* segment will use huge TLB pages */
#define SHM_NORESERVE   010000  /* don't check for reservations */

/* Bits [26:31] are reserved */

/*
 * When SHM_HUGETLB is set bits [26:31] encode the log2 of the huge page size.
 * This gives us 6 bits, which is enough until someone invents 128 bit address
 * spaces.
 *
 * Assume these are all power of twos.
 * When 0 use the default page size.
 */
#define SHM_HUGE_SHIFT  26
#define SHM_HUGE_MASK   0x3f
#define SHM_HUGE_2MB    (21 << SHM_HUGE_SHIFT)
#define SHM_HUGE_1GB    (30 << SHM_HUGE_SHIFT)

#ifdef CONFIG_SYSVIPC
struct sysv_shm {
	struct list_head shm_clist;
};

long do_shmat(int shmid, char __user *shmaddr, int shmflg, unsigned long *addr,
	      unsigned long shmlba);
bool is_file_shm_hugepages(struct file *file);
void exit_shm(struct task_struct *task);
#define shm_init_task(task) INIT_LIST_HEAD(&(task)->sysvshm.shm_clist)
#else
struct sysv_shm {
	/* empty */
};

static inline long do_shmat(int shmid, char __user *shmaddr,
			    int shmflg, unsigned long *addr,
			    unsigned long shmlba)
{
	return -ENOSYS;
}
static inline bool is_file_shm_hugepages(struct file *file)
{
	return false;
}
static inline void exit_shm(struct task_struct *task)
{
}
static inline void shm_init_task(struct task_struct *task)
{
}
#endif

#endif /* _LINUX_SHM_H_ */
