void FCFS (
    int number_of_jobs,
    const int job_submitted_time [],
    const int job_required_time [],
    int job_sched_start []
){
	int i=0;
	int cur=0;
	for(i=0;i<number_of_jobs;++i){
		if(cur<=job_submitted_time[i]){
			job_sched_start[i]=job_submitted_time[i];
			cur=job_submitted_time[i]+job_required_time[i];
		}
		else {
			job_sched_start[i]=cur;
			cur+=job_required_time[i];
		}
	}
	
}
int job_remain_time[2048];
int findjob(int n){
	int i;
	int p=n;
	for(i=0;i<n;++i){
		if(job_remain_time[i]){
			if(p==n)p=i;
			else if(job_remain_time[p]>job_remain_time[i])
				p=i;
		}
	}
	return p;
}
void SJF (
    int number_of_jobs,
    const int job_submitted_time [],
    const int job_required_time [],
    int job_sched_start []
){
	int i=0;
	int cur=0,curtime=0;
	for(i=0;i<=number_of_jobs;++i)
		job_remain_time[i]=0;
	i=1;
	curtime=job_submitted_time[0];
	job_sched_start[0]=curtime;
	job_remain_time[0]=job_required_time[0];
	while(i<number_of_jobs){
		if(curtime+job_remain_time[cur]<=job_submitted_time[i]){
			curtime+=job_remain_time[cur];
			job_remain_time[cur]=0;
			cur=findjob(number_of_jobs);
			if(job_remain_time[cur]==job_required_time[cur])
				job_sched_start[cur]=curtime;
			if(cur==number_of_jobs){
				cur=i;
				i++;
				job_remain_time[cur]=job_required_time[cur];
				job_sched_start[cur]=curtime=job_submitted_time[cur];;
			}
		}
		else {
			job_remain_time[cur]-=job_submitted_time[i]-curtime;
			curtime=job_submitted_time[i];
			if(job_remain_time[cur]<=job_required_time[i]){
				job_remain_time[i]=job_required_time[i];
				i++;
			}
			else {
				cur=i;
				i++;
				job_remain_time[cur]=job_required_time[cur];
				job_sched_start[cur]=job_submitted_time[cur];
			}
		}
	}
	while((cur=findjob(number_of_jobs))!=number_of_jobs){
		if(job_remain_time[cur]==job_required_time[cur])
			job_sched_start[cur]=curtime;
		curtime+=job_remain_time[cur];
		job_remain_time[cur]=0;
	}
}
