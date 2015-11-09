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

			pthread_t t1;
			pthread_create(&t1,NULL,Prima,(void *)pointer);
			pthread_join(t1,NULL);

			printf("\nJumlah bilangan prima kurang dari %d adalah %d\n",n,counter);
		}
		else if(tc==2)
		{
			printf("2\n");
		}
		else if(tc==0)
		{
			break;
		}
		else printf("Input Salah!!!\n");
	} while(tc!=0);
}
