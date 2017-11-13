#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#define buffer 10
# define MAX 50
pthread_mutex_t mutex;
pthread_cond_t p_cond;
pthread_cond_t c_cond;
struct Product{
	int p_id;
	int count;
};
int count=0;
int sum = 0;
struct Product s[buffer];

struct Product produce(int p_id){
	struct Product temp;
	temp.p_id=p_id;
	temp.count = sum;
	return temp;
}

void *producer(void *arg){
	struct Product temp;
	int id=(int)(*((int *)arg));
	while(1){
		if(sum==MAX){
			break;
		}
		pthread_mutex_lock(&mutex);
		if(sum==MAX){
			pthread_mutex_unlock(&mutex);
			printf("线程p_thread %d 结束\n",id);
			pthread_exit(0);
		}
		while(count==10){
			pthread_cond_wait(&p_cond,&mutex);
			if(sum==MAX){
				pthread_mutex_unlock(&mutex);
				printf("线程p_thread %d 结束\n",id);
				pthread_exit(0);
			}
		}
		sum = sum+1;
		temp = produce(id);
		s[count]=temp;
		count = count+1;
		printf("生产者 %d 生产第 %d 个产品\n",id,sum);
		if(count==1)
			pthread_cond_signal(&c_cond);
		pthread_mutex_unlock(&mutex);
		usleep(1);
	}
	printf("线程p_thread %d 结束\n",id);
	pthread_exit(0);
}
void *consumer(void *arg){
	struct Product temp;
	int id = (int)(*((int *)arg));
	while(1){
		if(sum == MAX && count ==0){
			printf("线程c_thread %d 结束\n",id);
			pthread_exit(0);
		}	
		pthread_mutex_lock(&mutex);
		if(sum == MAX && count ==0){
			pthread_mutex_unlock(&mutex);
			printf("线程c_thread %d 结束\n",id);
			pthread_exit(0);
		}	
		while(count==0){
			pthread_cond_wait(&c_cond,&mutex);
			if(sum == MAX && count ==0){
				pthread_mutex_unlock(&mutex);
				printf("线程c_thread %d 结束\n",id);
				pthread_exit(0);
			}
		}
		temp = s[count-1];
		printf("消费者 %d 消费第 %d 个产品\n",id,temp.count);
		count = count-1;
		if(count==9)
			pthread_cond_signal(&p_cond);
		pthread_mutex_unlock(&mutex);
		usleep(1);
	}
	printf("线程c_thread %d 结束\n",id);
	pthread_exit(0);
}

int main(){
	int a[3]={1,2,3};
	int b[2]={1,2};
	pthread_t p_thread1,p_thread2,p_thread3;
	pthread_t c_thread1,c_thread2;
	pthread_mutex_init(&mutex,0);
	pthread_cond_init(&p_cond,0);
	pthread_cond_init(&c_cond,0);
	pthread_create(&c_thread1,0,consumer,&b[0]);
	pthread_create(&c_thread2,0,consumer,&b[1]);	
	pthread_create(&p_thread1,0,producer,&a[0]);
	pthread_create(&p_thread2,0,producer,&a[1]);
	pthread_create(&p_thread3,0,producer,&a[2]);
	pthread_join(p_thread3,0);
	pthread_join(p_thread2,0);
	pthread_join(p_thread1,0);
	pthread_join(c_thread2,0);
	pthread_join(c_thread1,0);
	pthread_cond_destroy(&p_cond);
	pthread_cond_destroy(&c_cond);
	pthread_mutex_destroy(&mutex);
	return 0;
}
