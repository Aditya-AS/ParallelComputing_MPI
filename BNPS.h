#ifndef _BNPS_H
#define _BNPS_H

#include <mpi.h>

#define numNews 25
#define numReporters 3
#define LOCATION_SIZE 20
#define CATEGORY_SIZE 6
#define TOTAL_NEWS 200


/* Assuming only one source generating process */
char* categories[] = {"SPORTS", "BUSINESS", "NATIONAL", "INTERNATIONAL", "ENTERTAINMENT", "POLITICS"};
char* locations[] = {"DELHI","MUMBAI","PUNE","HYDERABAD","KANPUR","KHARAGPUR","BHOPAL","CHENNAI","TRIVENDRUM",
					 "PILANI","LOHARU","CHIRAWA","ASANSOL","MATHURA","BANGALORE","NAGALAND","SHILLONG","JAMMU","AHMEDABAD","KOLKATA" };
char* topics[] = {"JNU", "SECTION377", "PARLIAMENT", "USELECTIONS", "WORLDT20", "FOOTBALLLEAGUE"};

int timestamp = 0;

typedef int NewsReporterMap[numNews][numReporters+1];
typedef int ReporterNewsMap[numReporters][numNews];



typedef struct wrapper
{
	NewsReporterMap map;
	ReporterNewsMap rmap;
} Wrapper;

typedef struct news
{
	int newsId;
	int timestamp;
	char location[15];
	char category[15];
	char content[101];
} news;

typedef struct message
{
	int messageType;
	int source;
	news update;
} Message;

typedef struct record
{
	int ready;
	news* latestCopy;
	MPI_Request *rqst;
	int numNewsFriends;
	//int* NewsFriends;
	//int* pendingRequests;
} Record;

#endif