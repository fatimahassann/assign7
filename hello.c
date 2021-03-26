#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h> 
#include <linux/netlink.h>
#include <linux/spinlock.h>
#include <net/sock.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <asm/uaccess.h>
#include <linux/vmalloc.h>
#include </usr/include/x86_64-linux-gnu/asm/unistd_64.h>
#include <linux/icmp.h>
#include <linux/cred.h>
#include <linux/fs.h>
#include <linux/kallsyms.h>

MODULE_LICENSE("GPL");
/*char sys[50]="/boot/System.map-";*/
char sys[50]="/proc/kallsyms";
const char* src;
sys_call_ptr_t *fork_addr;
long unsigned *p1;
long unsigned * sys_call;
int counter=0; /*counter to keep track of the forks */
struct myfile 
{
	struct file *f;
	mm_segment_t fs;
	loff_t pos;
};


asmlinkage long (*fork_fun)(unsigned long f, void* s, void* parent_tid, void* child_tid, unsigned long tls);

/*fork_fun oldfork;*/

struct myfile* open_file_for_read(char* filename)
{	
	struct myfile * mf=kmalloc(sizeof(struct myfile),GFP_KERNEL);	

	mf->f = filp_open(filename,0,0);
	mf->pos=0;


	if(IS_ERR(mf->f))
	{	
		printk(KERN_ALERT "error");
		mf->f=NULL;
	}
	return mf;
}

volatile int read_from_file_until (struct myfile *mf, char *buf, unsigned long vlen)
{
    
	int read;

	mf->fs=get_fs();
	set_fs(KERNEL_DS);

	read= vfs_read(mf->f, buf, vlen, &mf->pos);

	set_fs(mf->fs);
	 
	return read;
}

void close_file(struct myfile *mf)
{
	if(mf)
		filp_close(mf->f,NULL);

	kfree(mf);
}

asmlinkage long hook_fun(unsigned long f, void* s, void* parent_pid, void* child_pid, unsigned long tls)
{
	counter++;
	if(counter%10==0)
	{	printk(KERN_ALERT "the number is incremented by 10, counter= ");
		printk(KERN_ALERT "%d\n", counter);
	}

	return fork_fun(f,s,parent_pid,child_pid,tls);
}


static int __init hello_init(void)
{
	printk(KERN_ALERT "Hello World CSCE-3402 :) \n");
	
	/*struct myfile * mf=kmalloc(sizeof(struct myfile), GFP_KERNEL);
        mf = open_file_for_read("/proc/version");
	

	int r;
	char *buf=kmalloc(1000,GFP_KERNEL);
	r=read_from_file_until(mf,buf,1000);
	
	int counter=0;
	int i=0;
	while(counter!=3)
	{
		if(buf[i]==' ')
			counter++;
		i++;
	}
	char buf2[i];
	strncpy(buf2,buf,i);
	buf2[i]=NULL;

	char*p_buf2=buf2;
	strsep(&p_buf2, " ");
	strsep(&p_buf2," ");
	src=p_buf2;
	 strcat(sys,src);
	

	close_file(mf);	
	kfree(buf);


	strim(sys);
	
	struct myfile * mf2=kmalloc(sizeof(struct myfile), GFP_KERNEL);	
	mf2 = open_file_for_read("/proc/kallsyms");

	int r2;
	char* buf3=kmalloc(3420000, GFP_KERNEL);
	r2=read_from_file_until(mf2,buf3,3420000);

	char search[15]=" sys_call_table";

	int j=0;
	int k=0;
	int ind=0;


	for(j=0;j<strlen(buf3)-15;j++)
	{
		if(buf3[j]==search[k])
		{
			k++;
			if(k==15)
			{
				ind=j;
			}
		}
		else {
			j-=k;
			k=0;
		}
	}

	char ans[17];
	int c=0;
	for(j=ind-32;j<ind-15;j++)
	{	
		ans[c]=buf3[j];

		c++;
	}
	close_file(mf2);
	kfree(buf3);
	printk(KERN_ALERT "%s\n", ans);*/

	sys_call=(void *)kallsyms_lookup_name("sys_call_table");
	printk(KERN_ALERT "%lu\n", sys_call);



/*	const char * in = ans;
	char addr[20]="0x"; 
	strncat(addr,in,17);
	strim(addr);
	addr[20]=NULL;
	printk(KERN_ALERT "%s\n", addr);
	
	const char * input=addr; 
	long unsigned p;

	kstrtoul(input,16,&p);
        p1= p; */
	fork_addr=sys_call[__NR_clone];

	printk(KERN_ALERT "%lu\n", fork_addr);
	fork_fun=sys_call[__NR_clone];

	write_cr0(read_cr0() & (~0x10000));

	sys_call[__NR_clone]=(sys_call_ptr_t*)hook_fun; 
	
	write_cr0(read_cr0() | 0x10000);

	return 0;
}

static void __exit hello_cleanup(void)
{
	printk(KERN_ALERT "Bye Bye CSCE-3402 :) \n");

	write_cr0(read_cr0() & (~0x10000));

	sys_call[__NR_clone]=(sys_call_ptr_t*)fork_addr;

	write_cr0(read_cr0() | 0x10000);

}

module_init(hello_init);
module_exit(hello_cleanup);

