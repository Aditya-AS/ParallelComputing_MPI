#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include "sourceEmulator.h"
#define INITIAL_NEWS 100
#define LOCATIONS 50
#define LOCATION_SIZE 20
#define CATEGORY_SIZE 6
#define TOPICS_SIZE 6
#define TOTAL_NEWS 500
int timestamp = 0;

/* Assuming only one source generating process */
char* categories[] = {"SPORTS", "BUSINESS", "NATIONAL", "INTERNATIONAL", "ENTERTAINMENT", "POLITICS"};
char* locations[] = {"DELHI","MUMBAI","PUNE","HYDERABAD","KANPUR","KHARAGPUR","BHOPAL","CHENNAI","TRIVENDRUM",
					 "PILANI","LOHARU","CHIRAWA","ASANSOL","MATHURA","BANGALORE","NAGALAND","SHILLONG","JAMMU","AHMEDABAD","KOLKATA" };
char* topics[] = {"JNU", "SECTION377", "PARLIAMENT", "USELECTIONS", "WORLDT20", "FOOTBALLLEAGUE"};
char* randomTextGenerator(){
	time_t t;
	//srand((unsigned) time(&t));
	char* news = (char*)malloc(sizeof(char)*101);
	int i=0;
	for(i=0;i<100;i++){
		char c = 65 + (char)(rand() % 57) ;
		news[i] = c;
		if(news[i] == '\n') i--;
	}
	news[i] = '\0';
	return news;
}
news* fillFile(char* filename, int id, bool firstTime){
	news* new = (news*)malloc(sizeof(news));
	FILE* fp = fopen(filename, "w+");
	char* location = locations[rand()%LOCATION_SIZE];

	char* category = categories[rand()%CATEGORY_SIZE];
	char* content = randomTextGenerator();

	strcpy(new->location,location);
	strcpy(new->category,category);
	strcpy(new->content,content);

	new->newsId = id;
	new->timestamp = timestamp;

	fprintf(fp,"Id:\t%d\n",id);
	fprintf(fp,"Location:\t%s\n",location);
	fprintf(fp,"EventTime:\t%d\n",etime);
	fprintf(fp,"Category:\t%s\n",category);
	fprintf(fp,"Content:\t%s\n",content);
	
	free(location);free(category);free(content);
	fclose(fp);
	timestamp++;
	return new;
}
void printStruct(news* new){
	fprintf(stdout,"Id:\t%d\n",new->newsId);
	fprintf(stdout,"Location:\t%s\n",new->location);
	fprintf(stdout,"EventTime:\t%d\n",new->timestamp);
	fprintf(stdout,"Category:\t%s\n",new->category);
	fprintf(stdout,"Content:\t%s\n",new->content);
}

void generateNews(){
	char* dest = (char*)malloc(sizeof(char)*35);
	int i=0;
	int reporter = 0;
	for(i=0;i<INITIAL_NEWS;i++){
		sprintf(dest, "./news/file%d.txt",i);
		//MPI_Send()
		MPI_Request req;
		news* new = fillFile(dest,i,true);
		Message msg;
		msg.messagetype = 0;
		copyNewsStructure(&(msg.update),new);
		MPI_Isend(&msg, 1, messagetype, reporter, new->newsId, MPI_COMM_WORLD,&req);
		//printStruct(new);
	}
	while(i<TOTAL_NEWS){
		int id = rand()%INITIAL_NEWS;
		sprintf(dest, "./news/file%d.txt",id);
		//MPI_Send()
		MPI_Request req;
		news* new = fillFile(dest,i,true);
		Message msg;
		msg.messagetype = 0;
		copyNewsStructure(&(msg.update),new);
		MPI_Isend(&msg, 1, messagetype, reporter, new->newsId, MPI_COMM_WORLD,&req);
	}
	//
	Message msg;
	msg.messagetype = -1;
	MPI_Bcast(&msg, 1, messagetype, 1, MPI_COMM_WORLD);
}

int main(int argc, char* argv[]){
	srand ( time(NULL) ); //Always use srand in the beginning once so that random numbers between different executions 
		
	generateNews();				  //are also random and not just the same as previous run.
	
}

/*
	MPI_Datatype newstype, oldtypes[2]; 
int          blockcounts[2];
MPI_Aint    offsets[2], extent_int, extent_char;
offsets[0] = 0;
oldtypes[0] = MPI_INT;
blockcounts[0] = 2;

MPI_Type_extent(MPI_INT, &extent_int);

offsets[1] = 2 * extent_int;
oldtypes[1] = MPI_CHAR;
blockcounts[1] = 131;
 
MPI_Type_struct(2, blockcounts, offsets, oldtypes, &newstype);
MPI_Type_commit(&newstype);

------------------------------------------------------------------------

MPI_Datatype messagetype, oldtype[2];
int blockcount[2];
MPI_Aint offset[2], extentint;

offset[0] = 0;
oldtype[0] = MPI_INT;
blockcount[0] = 1;

MPI_Type_extent(MPI_INT, &extentint);

offset[1] = extentint;
oldtype[1] = newstype;
blockcount[1] = 1;

MPI_Type_struct(2, blockcount, offset, oldtype, &messagetype);
MPI_Type_commit(&messagetype);

*/
