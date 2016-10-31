//program to show the implementation of basic derived data types of MPI
#include <mpi.h>
#include <stddef.h>

//Test structure
typedef struct test{
	int one;
	double two;
} Test;


int main(int argc, char *argv[]){
	const int tag = 0;
	int size, rank;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	//number of items inside structure Test
	const int nitems = 2;

	//count of item of each type inside Test in order
	int blocklengths[2] = {1, 1};

	//data types present inside Test in order
	MPI_Datatype types[2] = {MPI_INT, MPI_DOUBLE};

	//name of derived data type
	MPI_Datatype mpi_test_type;

	//array to store starting address of each item inside Test
	MPI_Aint offsets[2];

	//offset of each item in Test with respect to base address of Test
	offsets[0] = offsetof(Test, one);
	offsets[1] = offsetof(Test, two);

	//create the new derived data type
	MPI_Type_create_struct(nitems, blocklengths, offsets, types, &mpi_test_type);

	//commit the new data type
	MPI_Type_commit(&mpi_test_type);

	//get rank of current process
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if(rank == 0){
		Test send;
		send.one = 1;
		send.two = 2.0;
		const int dest = 1;
		MPI_Send(&send, 1, mpi_test_type, dest, tag, MPI_COMM_WORLD);
		print("\nRank %d sending %d, %lf\n", rank, send.one, send.two);
	}
	if(rank == 1){
		MPI_Status status;
		const int src = 0;
		Test recv;
		MPI_Recv(&recv, 1, mpi_test_type, src, tag, MPI_COMM_WORLD, &status);
		printf("\nRank %d received %d %lf\n", rank, recv.one, recv.two);
	}

	//free the derived data type
	MPI_Type_free(&mpi_test_type);
	MPI_Finalize()
	return 0;
}