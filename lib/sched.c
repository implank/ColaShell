#include <env.h>
#include <pmap.h>
#include <printf.h>
/* Overview:
 *  Implement simple round-robin scheduling.
 *
 * Hints:
 *  1. The variable which is for counting should be defined as 'static'.
 *  2. Use variable 'env_sched_list', which is a pointer array.
 *  3. CANNOT use `return` statement!
 */
/*** exercise 3.15 ***/
static int runtime=0;
void sched_yield(void){
	static int count = 0;
	static int point = 0;
	static struct Env *e=NULL;
	runtime++;
	//printf("begin %d\n",runtime);
	if(count&&curenv&&curenv->env_status==ENV_RUNNABLE){
		count--;
		//printf("end %d\n",runtime);
		env_run(curenv);
		return;
	}
	LIST_FOREACH(e,&env_sched_list[point],env_sched_link){
		if(e->env_status==ENV_RUNNABLE){
			count=e->env_pri;
			LIST_REMOVE(e,env_sched_link);
			LIST_INSERT_TAIL(&env_sched_list[1-point],e,env_sched_link);
			count--;
	//printf("end %d\n",runtime);
			env_run(e);
			return;
		}
	}
	point=1-point;
	LIST_FOREACH(e,&env_sched_list[point],env_sched_link){
		if(e->env_status==ENV_RUNNABLE){
			count=e->env_pri;
			LIST_REMOVE(e,env_sched_link);
			LIST_INSERT_TAIL(&env_sched_list[1-point],e,env_sched_link);
			count--;
		//printf("end %d\n",runtime);
			env_run(e);
			return;
		}
	}
	panic("^^^^^^NO ENV FIND^^^^^^");
}
