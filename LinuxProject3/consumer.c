#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#define NUM 5

int buf[NUM] = { 0 };
int num = 0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t flag = PTHREAD_COND_INITIALIZER;

/*
 *ʹ�û�������������
 */
void *producer(void *info)
{
	int count = 0;
	
	while (1)
	{
		pthread_mutex_lock(&lock);
		
		if (num < NUM)
		{
			buf[num] = count++;
			printf("put %d\n", buf[num++]);
		}
		
		pthread_mutex_unlock(&lock);
	}
}

/*
 *ʹ�û�������������
 */
void *consumer(void *info)
{
	while (1)
	{
		/*
		 * ���������ϼ���
		 * ���������߳��޷���ȡ��ǰ��
		 * ���㻥��
		 * ���޷�����ǰ�������޵ȴ�
		 */
		pthread_mutex_lock(&lock);
		
		if (num > 0)
		{
			printf("get %d\n", buf[--num]);
			buf[num] = 0;
		}
		else
		{
			printf("no food\n");
		}
		
		sleep(1);
		pthread_mutex_unlock(&lock);
	}
}

/*
 *ʹ�û�����������������������
 */
void *producer2(void *info)
{
	int count = 0;
	
	while (1)
	{
		pthread_mutex_lock(&lock);
		
		if (num < NUM)
		{
			buf[num] = count++;
			printf("put %d\n", buf[num++]);
			pthread_cond_signal(&flag);
		}
		else
		{
			pthread_cond_wait(&flag, &lock);
		}
		
		pthread_mutex_unlock(&lock);
	}
}

/*
 *ʹ�û�����������������������
 */
void *consumer2(void *info)
{
	while (1)
	{
		pthread_mutex_lock(&lock);
		
		if (num > 0)
		{
			printf("get %d\n", buf[--num]);
			buf[num] = 0;
			pthread_cond_signal(&flag);
		}
		else
		{
			printf("no food\n");
			/*
			 *����ȴ����ͷ���
			 *��ֹռ����
			 */
			pthread_cond_wait(&flag, &lock);
		}
		
		sleep(1);
		pthread_mutex_unlock(&lock);
	}
}

int main()
{
	pthread_t t1, t2;
	
	pthread_create(&t1, NULL, consumer, NULL);
	pthread_create(&t2, NULL, producer, NULL);
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	
	return 0;
}