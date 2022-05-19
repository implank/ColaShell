#include "../drivers/gxconsole/dev_cons.h"
#include <mmu.h>
#include <env.h>
#include <printf.h>
#include <pmap.h>
#include <sched.h>

extern char *KERNEL_SP;
extern struct Env *curenv;
extern int occ=0;
int sys_acquire(int sysno,int a1,int a2,int a3,int a4,int a5){
  if(occ==0){
    curenv->flag=1;
    occ=1;
    return 0;
  }
  else return -1;
}
int sys_release(int sysno,int a1,int a2,int a3,int a4,int a5){
  if(occ==1&&curenv->flag==1){
    occ=0;
    curenv->flag=0;
    return 0;
  }
  else return -1;
}
void sys_putchar(int sysno, int c, int a2, int a3, int a4, int a5)
{
  if(occ==1&&curenv->flag){
    printcharc((char) c);
  } 
    return;
}
void *memcpy(void *destaddr, void const *srcaddr, u_int len)
{
    char *dest = destaddr;
    char const *src = srcaddr;

    while (len-- > 0)
    {
        *dest++ = *src++;
    }

    return destaddr;
}
u_int sys_getenvid(void)
{
    return curenv->env_id;
}
void sys_yield(void)
{
    bcopy((void *)KERNEL_SP - sizeof(struct Trapframe),
          (void *)TIMESTACK - sizeof(struct Trapframe),
          sizeof(struct Trapframe));
    sched_yield();
}
int sys_env_destroy(int sysno, u_int envid)
{
    int r;
    struct Env *e;

    if ((r = envid2env(envid, &e, 1)) < 0)
    {
        return r;
    }

    printf("[%08x] destroying %08x\n", curenv->env_id, e->env_id);
    env_destroy(e);
    return 0;
}
int sys_set_pgfault_handler(int sysno, u_int envid, u_int func, u_int xstacktop)
{
    struct Env *env;
    int ret;
    ret = envid2env(envid, &env, 0);
    if (ret < 0)
    {
        return ret;
    }
    env->env_pgfault_handler = func;
    env->env_xstacktop = xstacktop;

    return 0;
}
int sys_mem_alloc(int sysno, u_int envid, u_int va, u_int perm)
{
    struct Env *env;
    struct Page *ppage;
    int ret;
    perm = perm & (BY2PG - 1); //取后10位
    if (va >= UTOP)
    {
        return -E_INVAL;
    }
    if ((perm & PTE_COW) || (!(perm & PTE_V)))
    {
        return -E_INVAL;
    }
    ret = envid2env(envid, &env, 1);
    if (ret < 0)
    {
        return ret;
    }
    ret = page_alloc(&ppage);
    if (ret < 0)
    {
        return ret;
    }
    ret = page_insert(env->env_pgdir, ppage, va, perm);
    if (ret < 0)
    {
        return ret;
    }
    return 0;
}

int sys_mem_map(int sysno, u_int srcid, u_int srcva, u_int dstid, u_int dstva, u_int perm)
{
    int ret;
    u_int round_srcva, round_dstva;
    struct Env *srcenv;
    struct Env *dstenv;
    struct Page *ppage;
    Pte *ppte;

    perm = perm & (BY2PG - 1); //取后10位

    ppage = NULL;
    ret = 0;
    round_srcva = ROUNDDOWN(srcva, BY2PG); //源虚拟地址页对齐?
    round_dstva = ROUNDDOWN(dstva, BY2PG); //目标进程虚拟地址页对齐?
    if (round_srcva >= UTOP || round_dstva >= UTOP)
    {
        return -E_INVAL;
    }
    if (!(perm & PTE_V))
    {
        return -E_INVAL;
    }
    ret = envid2env(srcid, &srcenv, 0);
    if (ret < 0)
    {
        return ret;
    }
    ret = envid2env(dstid, &dstenv, 0);
    if (ret < 0)
    {
        return ret;
    }
    ppage = page_lookup(srcenv->env_pgdir, round_srcva, &ppte);
    if (ppage == NULL)
    {
        return -E_INVAL;
    }
    if (((*ppte & PTE_R) == 0) && ((perm & PTE_R) != 0))
    {
        return -E_INVAL;
    }
    ret = page_insert(dstenv->env_pgdir, ppage, round_dstva, perm);
    if (ret < 0)
    {
        return ret;
    }
    return 0;
}
int sys_mem_unmap(int sysno, u_int envid, u_int va)
{
    int ret;
    struct Env *env;
    if (va >= UTOP)
    {
        return -E_INVAL;
    }
    ret = envid2env(envid, &env, 0);
    if (ret < 0)
    {
        return ret;
    }
    page_remove(env->env_pgdir, va);

    return 0;
    //	panic("sys_mem_unmap not implemented");
}
int sys_env_alloc(void)
{
    // Your code here.
    int r;
    struct Env *e;

    r = env_alloc(&e, curenv->env_id);
    if (r < 0)
    {
        return r;
    }
    bcopy((void *)KERNEL_SP - sizeof(struct Trapframe), (void *)&(e->env_tf), sizeof(struct Trapframe));
    e->env_tf.pc = e->env_tf.cp0_epc;
    e->env_tf.regs[2] = 0; // child process returns 0
    e->env_status = ENV_NOT_RUNNABLE;
    e->env_pri = curenv->env_pri;

    return e->env_id;
}
int sys_set_env_status(int sysno, u_int envid, u_int status)
{
    struct Env *env;
    int ret;

    if (status != ENV_RUNNABLE && status != ENV_NOT_RUNNABLE && status != ENV_FREE)
        return -E_INVAL;
    ret = envid2env(envid, &env, 0);
    if (ret)
        return ret;
    if (env->env_status != ENV_RUNNABLE && status == ENV_RUNNABLE)
        LIST_INSERT_HEAD(&env_sched_list[0], env, env_sched_link);
    if (env->env_status == ENV_RUNNABLE && status != ENV_RUNNABLE)
        LIST_REMOVE(env, env_sched_link);
    env->env_status = status;

    return 0;

    return 0;
}
int sys_set_trapframe(int sysno, u_int envid, struct Trapframe *tf)
{

    return 0;
}
void sys_panic(int sysno, char *msg)
{
    panic("%s", TRUP(msg));
}
void sys_ipc_recv(int sysno, u_int dstva)
{
    if (dstva >= UTOP)
    {
        return;
    }
    curenv->env_ipc_recving = 1;
    curenv->env_ipc_dstva = dstva;
    curenv->env_status = ENV_NOT_RUNNABLE;
    sys_yield();
}

int sys_ipc_can_send(int sysno, u_int envid, u_int value, u_int srcva, u_int perm)
{
    int r;
    struct Env *e;
    struct Page *p;

    perm = perm & (BY2PG - 1); //取后10位
    if (srcva >= UTOP)
    {
        return -E_INVAL;
    }
    r = envid2env(envid, &e, 0);
    if (r < 0)
    {
        return r;
    }
    if (e->env_ipc_recving == 0)
    {
        return -E_IPC_NOT_RECV;
    }
    e->env_ipc_recving = 0;
    e->env_ipc_from = curenv->env_id;
    e->env_ipc_value = value;
    e->env_ipc_perm = perm;
    e->env_status = ENV_RUNNABLE;

    if (srcva != 0)
    {
        p = page_lookup(curenv->env_pgdir, srcva, NULL);
        if (p == NULL)
        {
            return -E_INVAL;
        }
        page_insert(e->env_pgdir, p, e->env_ipc_dstva, perm);
    }
    return 0;
}
