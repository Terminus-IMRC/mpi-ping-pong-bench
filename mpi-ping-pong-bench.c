#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"

#ifndef TIMES
#warning TIMES is not defined. Using default value 10000.
#define TIMES 1000
#endif

int main(int argc, char *argv[])
{
	int i;
	int rank, size;
	long long memsize;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int processor_name_len;
	char processor_name_of_rank1[MPI_MAX_PROCESSOR_NAME];
	int processor_name_len_of_rank1;
	char *p;
	double start, end, wtime_sum=0.0;
	FILE *fp;
	char filename[0xffff];

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if((!rank)&&(argc!=2)){
		fprintf(stderr, "%s: invalid the number of the arguments.\n", argv[0]);
		fprintf(stderr, "usage: %s <memory size>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if((!rank)&&(size!=2)){
		fprintf(stderr, "%s: this program runs only on 2 machines.\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	MPI_Get_processor_name(processor_name, &processor_name_len);
	if(rank){
		MPI_Send(&processor_name_len, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		MPI_Send(processor_name, processor_name_len+1, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
	}else{
		MPI_Recv(&processor_name_len_of_rank1, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&processor_name_of_rank1, processor_name_len_of_rank1+1, MPI_CHAR, 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		if((processor_name_len==processor_name_len_of_rank1)&&(!strncmp(processor_name, processor_name_of_rank1, processor_name_len))){
			fprintf(stderr, "%s: run on different hosts.\n", argv[0]);
		fprintf(stderr, "(Attempted to run on %s.)\n", processor_name);
			exit(EXIT_FAILURE);
		}
	}

	memsize=atoll(argv[1]);
	p=(char*)malloc(sizeof(char)*memsize);
	if(!p){
		fprintf(stderr, "%s: allocating %lld bytes of memory failed.\n", argv[0], memsize);
		exit(EXIT_FAILURE);
	}

	for(i=0; i<TIMES; i++)
		if(!rank){
			start=MPI_Wtime();
			MPI_Send(p, memsize, MPI_CHAR, 1, 2*i+0+2, MPI_COMM_WORLD);
			MPI_Recv(p, memsize, MPI_CHAR, 1, 2*i+1+2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			end=MPI_Wtime();
			wtime_sum+=end-start;
		}else{
			MPI_Recv(p, memsize, MPI_CHAR, 0, 2*i+0+2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Send(p, memsize, MPI_CHAR, 0, 2+i+1+2, MPI_COMM_WORLD);
		}

	free(p);

	if(!rank){
		sprintf(filename, "res-m%lldx%d.txt", memsize, TIMES);
		fp=fopen(filename, "w");
		fprintf(fp, "%f\n", wtime_sum/TIMES);
		fclose(fp);
	}

	MPI_Finalize();
	return 0;
}
