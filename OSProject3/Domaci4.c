#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <semaphore.h>
#include <ctype.h>
#include <limits.h>

int wrk = 0;
int wch = 0;
pthread_t *workers;
pthread_t watcher[3];
int freeWch[3] = {-1, -1, -1};
int *freeWrk;
int size;

pthread_mutex_t free_lock;
sem_t semWch;
sem_t semWrk;

typedef struct
{
	int id;
	char *path;
}workerResurs;

typedef struct
{
	int id;
	char *path;
}watcherResurs;

typedef struct
{
	int sum;
	char *name;
}resultSet;

resultSet result_set[100];
int res_cnt = 0;

void *searchPrimary(void *_args);

void *searchDir(void *_args)
{

	char *args = (char*)_args;
	char *path[PATH_MAX + 1];
	DIR *dir;
	struct dirent *sd;
	printf("%s", args);

	dir = opendir(args);
	if(dir == NULL)
	{
		printf("ERROR\n");
		closedir(dir);
		exit(1);
	}
	
	
		
	while((sd = readdir(dir)) != NULL)
	{
			
			
		if(sd->d_type == 8)
		{
				
				
				
			strcpy(path, args);
			strcat(path, "/");
			strcat(path, sd->d_name);

			sem_wait(&semWrk);
			pthread_mutex_lock(&free_lock);
			int tmp;
			for(int i = 0; i < sizeof(freeWrk); i++)
			{
				if(freeWrk[i] == -1)
				{
						tmp = i;
						freeWrk[i] = 1;
						break;
				}
			}

			workerResurs arg = (workerResurs){tmp, path};
			pthread_create(&(workers[tmp]), NULL, searchPrimary, &arg);
			pthread_join(workers[tmp], NULL);
			sem_post(&semWrk);
		}

	}

		closedir(dir);


}

void *searchPrimary(void *_args)
{
	pthread_mutex_unlock(&free_lock);
	workerResurs *s = (workerResurs*)_args;
	char *args = s->path;
	int id = s->id;
	printf("ID------>>%d\n", id);
	int res = 0;
	printf("%s\n", args);
	
	FILE *fp;
	fp = fopen(args, "r");
	int c;
	if(fp)
	{
		int num = 0;
		int flag = 0;
		while((c = fgetc(fp)) != EOF)
		{
			if(isdigit(c))
			{
				flag = 1;
				num = num*10;
				num += c - '0';
			}else
			{
				if(flag == 1)
				{
					if(isPrimary(num)) res++;
					flag = 0;
					num = 0;
				}
			}
						
		}
		
		fclose(fp);
	}
	
	pthread_mutex_lock(&free_lock);
	freeWrk[id] = -1;
	(result_set + res_cnt)->name = args;

	(result_set + res_cnt)->sum = res;
	res_cnt++;


	pthread_mutex_unlock(&free_lock);
	
}

int main(int argc, char const *argv[])
{

	pthread_mutex_init(&free_lock, NULL);
	sem_init(&semWch, 0, 3);
	int n;
	printf("Unesite broj worker niti:\n");
	scanf("%d", &n);
	size = n;
	sem_init(&semWrk, 0, n);
	workers = malloc(n * sizeof(pthread_t));
	freeWrk = malloc(n * sizeof(int));
	for(int i = 0; i < n; i++)
	{
		freeWrk[i] = -1;
	}

	for(int i = 0; i < n; i++)
	{
		printf("%d\n", freeWrk[i]);
	}




	
	while(1)
	{
		printf("New Command: \n");
		char name[30];
		scanf("%s", name);
		char *tmp = malloc(13*sizeof(char));
		int i;

		for(i = 0; i < strlen(name); i++)
		{
			if(name[i] == '-') break;
			tmp[i] = name[i];
		}

		if(!strcmp(tmp, "add_dir"))
		{

			char *dirName = malloc(PATH_MAX * sizeof(char));
			int cnt = 0;
			for(int j = i + 1; j < strlen(name); j++)
			{

				dirName[cnt++] = name[j];
			}
			printf("dirName ->>>%s", dirName);
			char buf[PATH_MAX];
			strcpy(buf, dirName);
			printf("%s", buf);

			char *g = dirName;
			if(wch <= 3)
			{
				pthread_create(&(watcher[wch++]), NULL, searchDir, buf);
			}else
			{
				printf("No more watchers\n");
			}
			free(dirName);

		}else if(!strcmp(tmp, "remove_dir"))
		{
			printf("REMOVE DIR\n");

		}else if(!strcmp(tmp, "result"))
		{
			pthread_mutex_lock(&free_lock);
			printf("RESULT:\n");
			for(int i = 0; i < res_cnt; i++)
			{
				
				printf("Path: %s---->>>", (result_set + i)->name);
				printf("Number of primes: %d\n", (result_set + i)->sum);
			}
			pthread_mutex_unlock(&free_lock);

		}else if(!strcmp(tmp, "end"))
		{
			printf("END\n");
			break;
		}else
		{
			printf("Nepoznata komanda\n");
		}


		free(tmp);


	}
	
	
	return 0;
}

int isPrimary(int num)
{
	if(num == 1 || num == 2 || num == 3) return 1;
	for(int i = 2; i <= num/2 + 1; i++)
	{
		if(num % i == 0) return 0;
	}

	return 1;
}