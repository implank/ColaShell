#include <env.h>
#include <pmap.h>
#include <printf.h>

/* Overview:
 *  Implement simple round-robin scheduling.
 *
 *
 * Hints:
 *  1. The variable which is for counting should be defined as 'static'.
 *  2. Use variable 'env_sched_list', which is a pointer array.
 *  3. CANNOT use `return` statement!
 */
/*** exercise 3.15 ***/
int ts[3]={1,2,4};
int tt[3][2]={{2,1},{0,2},{1,0}};
void sched_yield(void)
{
	static int count = 0; // remaining time slices of current env
	static int point = 0; // current env_sched_list index    
	static struct Env *e=NULL;
	/*  hint:
	 *  1. if (count==0), insert `e` into `env_sched_list[1-point]`
	 *     using LIST_REMOVE and LIST_INSERT_TAIL.
	 *  2. if (env_sched_list[point] is empty), point = 1 - point;
	 *     then search through `env_sched_list[point]` for a runnable env `e`, 
	 *     and set count = e->env_pri
	 *  3. count--
	 *  4. env_run()
	 *
	 *  functions or macros below may be used (not all):
	 *  LIST_INSERT_TAIL, LIST_REMOVE, LIST_FIRST, LIST_EMPTY
	 */
	printf("\n");
	if(count>0&&e&&e->env_status==ENV_RUNNABLE){
		count--;
		env_run(e);
		return;
	}
	if(e&&count==0){
		int dst=tt[point][e->env_pri%2];
		LIST_REMOVE(e,env_sched_link);
		LIST_INSERT_TAIL(&env_sched_list[dst],e,env_sched_link);
	}
	LIST_FOREACH(e,&env_sched_list[point],env_sched_link){
		if((e->env_status==ENV_RUNNABLE)&&(e->env_pri>0)){
			count=e->env_pri*ts[point];
			count--;
			env_run(e);
		}
	}
	point=(point+1)%3;
	LIST_FOREACH(e,&env_sched_list[point],env_sched_link){
		if((e->env_status==ENV_RUNNABLE)&&(e->env_pri>0)){
			count=e->env_pri*ts[point];
			count--;
			env_run(e);
		}
	}
	point=(point+1)%3;
	LIST_FOREACH(e,&env_sched_list[point],env_sched_link){
		if((e->env_status==ENV_RUNNABLE)&&(e->env_pri>0)){
			count=e->env_pri*ts[point];
			count--;
			env_run(e);
		}
	}
	panic("no env in yield");
}
