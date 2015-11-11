#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int counter=0;

void *Prima (void *args)
{
	int *n = (int *) args;	
	int p,q;
	for(p=2; p<*n; p++)
	{
		int cek=0;
		for (q=2; q<p; q++)
		{
			if(p%q == 0)
			{
				cek=1;
			}
		}
		if(cek==0)
		{
			printf("%d ", p);
			counter=counter+1;
		}
	}
	if(*n<=2)
	{
		counter=0;
	}
}

void *Thread1 (void *args)
{
	int *tanda = (int *) args;
	FILE *inp, *out;
	*tanda=1;

	inp = fopen("file1.txt","r");
	out = fopen("file2.txt","w");
	char temp;

	while(1)
	{
		temp=fgetc(inp);
		if(temp==EOF) break;
		else fputc(temp,out);
	}

	fclose(inp);
	fclose(out);
	*tanda=2;
}

void *Thread2 (void *args)
{
	int *tanda = (int *) args;
	FILE *inp, *out;

	while(1)
	{
		if(*tanda==0) continue;
		else
		{
			inp=fopen("file2.txt","r");
			out=fopen("file3.txt","w");			
			break;
		}
	}
	char temp;

	while(1)
	{
		temp=fgetc(inp);
		if(temp==EOF)
		{
			if(*tanda==1) continue;
			else break;
		}
		else fputc(temp,out);
	}

	fclose(inp);
	fclose(out);
}

void main()
{
	int tc;

	do
	{
		printf("1. Menghitung Bilangan Prima kurang dari N\n");
		printf("2. Aplikasi Multithread untuk menyalin isi file\n");
		printf("0. Keluar\n");
		printf("Masukkan pilihan: ");
		scanf("%d",&tc);
		if(tc==1)
		{
			int n;

			printf("Masukkan nilai N: ");
			scanf("%d",&n);

			int *pointer=&n;

			pthread_t thread;
			pthread_create(&thread,NULL,Prima,(void *)pointer);
			pthread_join(thread,NULL);

			printf("\nJumlah bilangan prima kurang dari %d adalah %d\n",n,counter);
		}
		else if(tc==2)
		{
			int tanda=0;
			pthread_t t1, t2;
			pthread_create(&t1,NULL,Thread1,&tanda);
			pthread_create(&t2,NULL,Thread2,&tanda);
			pthread_join(t1,NULL);
			pthread_join(t2,NULL);
		}
		else if(tc==0)
		{
			break;
		}
		else printf("Input Salah!!!\n");
	} while(tc!=0);
}
