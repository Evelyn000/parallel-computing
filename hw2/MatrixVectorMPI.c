#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "./mvmult.h"
#include "./mvmult.c"
#define datatype MPI_FLOAT
typedef float dtype;
void redistributeVector(
	void *b, 
	void **v, 
	MPI_Datatype dtype, 
	int *n,
	int *nfinal, 
	MPI_Comm grid_comm,
	MPI_Comm col_comm
	)
{
	int id;
	int p;
	int grid_period[2];
	int grid_coord[2];
	int grid_size[2];
	int dest_coord[2];
	int dest_id;
	int datumsize=get_size(dtype);
	MPI_Status status;

	MPI_Comm_rank(grid_comm, &id);
	MPI_Comm_size(grid_comm, &p);

	MPI_Cart_get(grid_comm, 2, grid_size, grid_period, grid_coord);
	if (grid_coord[0]==0)
		*nfinal=BLOCK_SIZE(grid_coord[1], grid_size[1], *n);

	MPI_Bcast(nfinal, 1, MPI_INT, 0, col_comm);
	MPI_Bcast(n, 1, MPI_INT, 0, col_comm);

	*v=my_malloc(id, (*nfinal)*datumsize);
	if (grid_coord[0]==0)
		memcpy(*v, b, (*nfinal)*datumsize);

	MPI_Bcast(*v, *nfinal, dtype, 0, col_comm);
}

void distribute(
	void *b,
	void **v,
	MPI_Datatype dtype,
	int n,
	MPI_Comm grid_comm
	);
int main(int argc, char* argv[])
{
	MPI_Init(&argc, &argv);
    double t;
	int npes, myrank;
	dtype **subs, **storage;
	dtype *b, *localv, *localout, *localin, *c;
	int localvec;
	int dims[2]={0}, periodic[2]={0};
	int mrows, mcols, nvec, nvecfinal;
	int localrow, localcol;
	int datumsize=get_size(datatype);
	int dest_coord[2];
	int dest_id;
	MPI_Comm cart_comm;
	MPI_Comm col_comm;
	MPI_Comm row_comm;
	MPI_Status status;
	int gridcoord[2];
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &npes);
	//printf("%d\n", npes);
	MPI_Dims_create(npes, 2, dims);
	MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periodic, 1, &cart_comm);

	read_checkerboard_matrix("matrix_a", (void***)&subs, (void**)&storage, datatype, &mrows, &mcols, cart_comm);
	print_checkerboard_matrix((void**)subs, datatype, mrows, mcols, cart_comm); 

	MPI_Cart_coords(cart_comm, myrank, 2, gridcoord);
	MPI_Comm_split(cart_comm, gridcoord[0], gridcoord[1], &row_comm);

	MPI_Comm_split(cart_comm, gridcoord[1], gridcoord[0], &col_comm);
	//printf("%d %d %d %d\n", dims[0], dims[1], periodic[0], periodic[1]);
	if (gridcoord[0]==0)
	{
		read_block_vector("matrix_b", (void **)&b, datatype, &nvec, row_comm);
		print_block_vector(b, datatype, nvec, row_comm);
		if (nvec!=mcols)
		{
			printf("dimensions not matched\n");
			MPI_Finalize();
			return 0;
		}
	}
    
    t=0-MPI_Wtime();
	redistributeVector((void*)b, (void**)&localv, datatype, &nvec, &localvec, cart_comm, col_comm);

	localrow=BLOCK_SIZE(gridcoord[0], dims[0], mrows);
	localcol=BLOCK_SIZE(gridcoord[1], dims[1], mcols);
	localout=my_malloc(myrank, localrow*datumsize);
	for (int i=0; i<localrow; i++)
	{
		float c=0.0;
		for (int j=0; j<localcol; j++)
			c+=localv[j]*subs[i][j];
		localout[i]=c;
	}
	if (gridcoord[1]!=0)
		MPI_Send(localout, localrow, datatype, 0, 0, row_comm);
	else {
		c=my_malloc(myrank, localrow*datumsize);
		memcpy(c, localout, localrow*datumsize);
		localin=my_malloc(myrank, localrow*datumsize);
		for (int i=1; i<dims[1]; i++)
		{
			MPI_Recv(localin, localrow, datatype, i, 0, row_comm, &status);
			for (int j=0; j<localrow; j++)
				c[j]+=localin[j];
		}
	}
    t+=MPI_Wtime();
	if (gridcoord[1]==0)
		print_block_vector(c, datatype, mrows, col_comm);

    printf("time: %f\n", t);
    printf("threads: %d\n", npes);


	MPI_Finalize();

}

/*void redistributeVector(
	void *b, 
	void **v, 
	MPI_Datatype dtype, 
	int n,
	int *nfinal, 
	MPI_Comm grid_comm
	)
{
	int id;
	int p;
	int grid_period[2];
	int grid_coord[2];
	int grid_size[2];
	int dest_coord[2];
	int dest_id;
	int datumsize=get_size(dtype);
	MPI_Status status;
	MPI_Comm col_comm;

	MPI_Comm_rank(grid_comm, &id);
	MPI_Comm_size(grid_comm, &p);

	MPI_Cart_get(grid_comm, 2, grid_size, grid_period, grid_coord);

	if (grid_size[0]==grid_size[1])
	{
		if (grid_coord[1]==0)
		{
			*nfinal=BLOCK_SIZE(grid_coord[0], grid_size[0], m);
			*v=my_malloc(id, (*nfinal)*datumsize);
			if (grid_coord[0]==0)
				memcpy(*v, b, (*nfinal)*datumsize);
			else {
				dest_coord[0]=grid_coord[1];
				dest_coord[1]=grid_coord[0];
				MPI_Cart_rank(grid_comm, dest_coord, &dest_id);
				MPI_Send(b, *nfinal, dtype, dest_id, 0, grid_comm, &status);
			}
		}
		else if (grid_coord[0]==0)
		{
			*nfinal=BLOCK_SIZE(grid_coord[1], grid_size[1], n);
			*v=my_malloc(id, (*nfinal)*datumsize);
			dest_coord[0]=grid_coord[1];
			dest_coord[1]=grid_coord[0];
			MPI_Cart_rank(grid_comm, dest_coord, &dest_id);
			MPI_Send(*v, *nfinal, dtype, dest_id, 0, grid_comm, &status);
		}

		MPI_Comm_split(grid_comm, grid_coord[1], grid_coord[0], &col_comm);
		MPI_Bcast(*v, *nfinal, dtype, 0, col_comm);
	}

	else{

	}
}*/
