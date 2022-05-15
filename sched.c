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
	int i=0,n=number_of_jobs;
	int cur=0,curtime=0;
	for(i=0;i<=number_of_jobs;++i)
		job_remain_time[i]=0;
	i=0;
	while(i<n){
		cur=findjob(n);
		if(cur!=n){
			job_sched_start[cur]=curtime;
			curtime+=job_required_time[cur];
			job_remain_time[cur]=0;
			while(job_submitted_time[i]<=curtime&&i<n)
				job_remain_time[i]=job_required_time[i],i++;
		}
		else {
			curtime=job_submitted_time[i];
			while(curtime==job_submitted_time[i]&&i<n){
				job_remain_time[i]=job_required_time[i];
				i++;
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
