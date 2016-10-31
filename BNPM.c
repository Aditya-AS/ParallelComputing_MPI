#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <mpi.h>
#include <stddef.h>
#include "BNPM.h"

void copyNewsStructure(news *dest, news *src)
{
	if(dest == NULL) {printf("Memory not allocated.\n");return;}
	if(src == NULL){printf("chodu src null hai\n");return;}
	dest->newsId = src->newsId;
	dest->timestamp = src->timestamp;
	strcpy(dest->location, src->location);
	strcpy(dest->category, src->category);
	strcpy(dest->content, src->content);
}
void printNewsReporterMapping(NewsReporterMap *map, ReporterNewsMap *rmap)
{
	int i;
	printf("Reporter to news mapping.\n\n");
	for(i=0;i<numReporters;i++)
	{
		printf("Reporter: %d\t", i+1);
		int j;
		for(j=0;j<numNews;j++)
		{
			if((*rmap)[i][j]==1) printf("%d\t", j);
		}
		printf("\n");
	}

	printf("\nNews to reporter mappint.\n\n");
	for(i=0;i<numNews;i++)
	{
		printf("News: %d\t", i);
		int j;
		printf("num: %d\t", (*map)[i][0]);
		for(j=1;j<(numReporters+1);j++)
		{
			if((*map)[i][j]==1) printf("%d\t", j);
		}
		printf("\n");
	}
}
void printStruct(news* new, FILE *fp)
{
	fprintf(fp,"Id:\t%d\n",new->newsId);
	fprintf(fp,"Location:\t%s\n",new->location);
	fprintf(fp,"EventTime:\t%d\n",new->timestamp);
	fprintf(fp,"Category:\t%s\n",new->category);
	fprintf(fp,"Content:\t%s\n",new->content);
}
void doNewsReporterMapping(NewsReporterMap *map, ReporterNewsMap *rmap)
{
	int i;

	for(i=0;i<numNews;i++)
	{
		int j;
		for(j=0;j<(numReporters+1);j++)
			(*map)[i][j] = 0;
	}

	for(i=0;i<numReporters;i++)
	{
		int j;
		for(j=0;j<numNews;j++)
			(*rmap)[i][j] = 0;
	}

	int n;
	n = 1 + rand() % (numNews/2);

	n = numNews/n;

	for(i=0;i<numNews;i++)
	{
		//printf("i: %d\t", i);
		//news covered by multiple reporters
		//printf("%d %d\n",i,n);
		if(i%n==0)
		{
			int m = 2 + rand() % (numReporters/2);
			//printf("num: %d\t", m);
			int j;
			int o = rand() % numReporters;
			for(j=0;j<m;j++)
			{
				//printf("%d\t", o);
				(*map)[i][o+1] = 1;
				(*map)[i][0]++;
				(*rmap)[o][i] = 1;
				o++;
				o %= numReporters;
			}
		}

		//news covered by a single reporter
		else
		{
			int o = rand() % numReporters;
			//printf("num: 1\t%d\t", o);
			(*map)[i][o+1] = 1;
			(*map)[i][0]++;
			(*rmap)[o][i] = 1;
		}
		//printf("\n");
	}
}

void doEditorReporterMapping(EditorReportMap* ermap, EditorNewsMap* enewsmap, ReporterNewsMap* rmap){
	int i=0;int e=0;
	printf("%d\n",numEditors);
	printf("%d\n",numReporters);
	
		//printf("%d\n",e);
		for(i=0;i<numReporters;i++){
				//printf("%d\t%d\n",e,i);
			int rem = i%numEditors;
			int k;
			(*ermap)[rem][i] = 1; // If Reporter i leaves a reminder of x when divided by numEditors then Map Editor x to Reporter i.

			for(k=0;k<numNews;k++)
			{
				if((*rmap)[i][k]==1) (*enewsmap)[rem][k] = 1;
			}
		}
	//EditorReporting Mapping has been completed. Time to use this info to do Editor News Mapping.
	printf("yoyoo\n");

}

void printEditorReporterMapping(EditorReportMap* ermap, EditorNewsMap *enewsmap){
	int i,j;
	printf("%d\n",*(ermap)[0][5]);
	for(i=0;i<numEditors;i++){
		printf("Editor %d:",i);
		for(j=0;j<numReporters;j++){
			//if(j==5){printf("5 mil gay\n");}

			if((*ermap)[i][j] == 1){printf(" %d",j);fflush(stdout);}
			else{
				;
			}
		}
		printf("\n");
	}

	for(i=0;i<numEditors;i++){
		printf("Editor %d:",i);
		for(j=0;j<numNews;j++){
			//if(j==5){printf("5 mil gay\n");}

			if((*enewsmap)[i][j] == 1){printf(" %d",j);fflush(stdout);}
			else{
				;
			}
		}
		printf("\n");
	}
}

void publishNews(char *filename, news *update)
{
	static int numUpdate[numNews] = {0};
	FILE *fp = fopen(filename, "a+");
	fprintf(fp,"News Update: %d\n", numUpdate[update->newsId]);
	printStruct(update, fp);
	numUpdate[update->newsId]++;
	fclose(fp);
}

char* randomTextGenerator()
{
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
news* fillFile(char* filename, int id)
{
	news* new = (news*)malloc(sizeof(news));
	FILE* fp = fopen(filename, "a+");
	char* location = locations[rand()%LOCATION_SIZE];

	char* category = categories[rand()%CATEGORY_SIZE];
	char* content = randomTextGenerator();

	strcpy(new->location,location);
	strcpy(new->category,category);
	strcpy(new->content,content);

	new->newsId = id;
	new->timestamp = timestamp;

	printStruct(new, fp);
	
	//free(location);free(category);free(content);
	fclose(fp);
	timestamp++;
	return new;
}

void BNPM(int *argc, char **argv[])
{
	NewsReporterMap map = {0};
	ReporterNewsMap rmap = {0};
	//EditorReportMap emap = {0};
	int myid;

	MPI_Init(argc,argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);

	//Definition of NewsType
	MPI_Datatype newstype, oldtypes[2];
	int blockcounts[2];
	MPI_Aint offsets[2], extent_int, extent_char;
	offsets[0] = offsetof(news, newsId);
	oldtypes[0] = MPI_INT;
	blockcounts[0] = 2;

	//MPI_Type_extent(MPI_INT, &extent_int);

	offsets[1] = offsetof(news, location);
	oldtypes[1] = MPI_CHAR;
	blockcounts[1] = 131; // 15 + 15 + 101
	 
	MPI_Type_struct(2, blockcounts, offsets, oldtypes, &newstype);
	MPI_Type_commit(&newstype);

	//Definition of MessageType
	MPI_Datatype messagetype, oldtype[2];
	int blockcount[2];
	MPI_Aint offset[2], extentint;

	offset[0] = offsetof(Message, messageType);
	oldtype[0] = MPI_INT;
	blockcount[0] = 2;

	//MPI_Type_extent(MPI_INT, &extentint);

	offset[1] = offsetof(Message, update);
	oldtype[1] = newstype;
	blockcount[1] = 1;

	MPI_Type_struct(2, blockcount, offset, oldtype, &messagetype);
	MPI_Type_commit(&messagetype);

	//--------------------------------------------------------------------------------------

	//Definition of MapType
	MPI_Datatype maptype, oldtypemap[1];
	int blockcountmap[1];
	MPI_Aint offsetmap[1], extentmap;

	offsetmap[0] = 0;
	oldtypemap[0] = MPI_INT;
	blockcountmap[0] = numNews * (numReporters+1);

	
	MPI_Type_struct(1, blockcountmap, offsetmap, oldtypemap, &maptype);
	MPI_Type_commit(&maptype);

	//Definition of RmapType
	MPI_Datatype rmaptype, oldtypermap[1];
	int blockcountrmap[1];
	MPI_Aint offsetrmap[1], extentrmap;

	offsetrmap[0] = 0;
	oldtypermap[0] = MPI_INT;
	blockcountrmap[0] = numReporters*numNews;

	MPI_Type_struct(1, blockcountrmap, offsetrmap, oldtypermap, &rmaptype);
	MPI_Type_commit(&rmaptype);

	//Definition of WrapperType
	MPI_Datatype wrappertype, oldtypewrap[2];
	int blockcountwrap[2];
	MPI_Aint offsetwrap[2];

	offsetwrap[0] = offsetof(Wrapper, map);
	oldtypewrap[0] = maptype;
	blockcountwrap[0] = 1;

	//MPI_Type_extent(maptype, &extentmap);

	offsetwrap[1] = offsetof(Wrapper, rmap);
	oldtypewrap[1] = rmaptype;
	blockcountwrap[1] = 1;

	MPI_Type_struct(2, blockcount, offsetwrap, oldtypewrap, &wrappertype);
	MPI_Type_commit(&wrappertype);

	//------------------------------------------------------------------------------
	MPI_Datatype emaptype, oldtypeemap[1];
	int blockcountemap[1];
	MPI_Aint offsetemap[1], extentemap;

	offsetemap[0] = 0;
	oldtypeemap[0] = MPI_INT;
	blockcountemap[0] = numReporters*numEditors;

	MPI_Type_struct(1, blockcountemap, offsetemap, oldtypeemap, &emaptype);
	MPI_Type_commit(&emaptype);

	//--------------------------------------------------------------------------------
	MPI_Datatype enewsmaptype, oldtypeenewsmap[1];
	int blockcountenewsmap[1];
	MPI_Aint offsetenewsmap[1], extentemnewsap;

	offsetenewsmap[0] = 0;
	oldtypeenewsmap[0] = MPI_INT;
	blockcountenewsmap[0] = numNews*numEditors;

	MPI_Type_struct(1, blockcountenewsmap, offsetenewsmap, oldtypeenewsmap, &enewsmaptype);
	MPI_Type_commit(&enewsmaptype);
	//-----------------------------------------------------------------------------------

	//editor variables
	Message msg;
	int src, tag;
	MPI_Status status;
	char dest[35];

	int i,j;

	//source variables
	MPI_Request req;
	news* new;
	int id;

	//reporter variables
	int editor_count;
	int editor;
	Record record[numNews];
	int newsid;
	int size;
	int source = 0;
	int reporter;
	news* update;
	news* myLatestCopy;
	int rqstCheck;
	MPI_Status stat;
	EditorReportMap emap1;
	EditorReportMap* emap = &emap1;
	memset(emap, 0, numEditors*numReporters);
	Wrapper wrap; //LOL MAxx, Funny errors. If instead of this, you allocate memory on heap, Editor doesn't work. Gives Seg Fault before the First Recv Completes. Find out why????
	Wrapper* wrapper = &wrap;
	EditorNewsMap enewsmap;
	EditorNewsMap* eNewsMap = &enewsmap;

	//Emulate Process according to rank
	switch(myid)
	{
		
		case 0:
				//source
				//create mapping
				doNewsReporterMapping(&(wrapper->map), &(wrapper->rmap));
				//create wrapper
				printNewsReporterMapping(&(wrapper->map), &(wrapper->rmap));
				//copyIntoWrapper(&wrapper, &map, &rmap);
				doEditorReporterMapping(emap, eNewsMap, &(wrapper->rmap));
				printEditorReporterMapping(emap,eNewsMap);
				//send to everyone
				for(j=1;j<(numEditors+numReporters+1);j++){MPI_Send(wrapper, 1, wrappertype, j, 5, MPI_COMM_WORLD);printf("Wrapper received by %d\n",j);sleep(1);}			

				for(j=1;j<(numEditors+numReporters+1);j++){MPI_Send(emap, 1, emaptype, j, 5, MPI_COMM_WORLD);printf("EMAP received by %d\n",j);sleep(1);}

				for(j=1;j<(numEditors+numReporters+1);j++){MPI_Send(eNewsMap, 1, enewsmaptype, j, 5, MPI_COMM_WORLD);printf("EMAP received by %d\n",j);sleep(1);}
				// The mappings has been shared with all the processes involved. Time to do some real work.

				printf("I am the source.    %d\n",myid);
				for(i=0;i<numNews;i++){
					sprintf(dest, "./news/file%d.txt",i);
					
					new = fillFile(dest,i);
					msg.messageType = 0;
					msg.source = 0;
					//printf("yoyo1\n");
					copyNewsStructure(&(msg.update),new);
					printf("Generating news: %d %d\n", new->newsId,i);
					for(j=1;j<(numReporters+1);j++)

						if(wrapper->map[new->newsId][j] == 1) {printf("sending : %d to: %d with msgtype: %d\n", new->newsId, j, msg.messageType); MPI_Isend(&msg, 1, messagetype, j+1, new->newsId, MPI_COMM_WORLD,&req);}

					//printStruct(new);
					sleep(1);
				}
				while(i<TOTAL_NEWS){
					id = rand()%numNews;
					sprintf(dest, "./news/file%d.txt",id);

					new = fillFile(dest,id);
					msg.messageType = 0;
					msg.source = 0;
					copyNewsStructure(&(msg.update),new);
					printf("Generating news: %d %d\n", new->newsId,i);
					for(j=1;j<(numReporters+1);j++)

						if(wrapper->map[new->newsId][j] == 1) {printf("sending : %d to: %d with msgtype: %d\n", new->newsId, j, msg.messageType); MPI_Isend(&msg, 1, messagetype, j+1, new->newsId, MPI_COMM_WORLD,&req);}

					i++;
					sleep(1);
				}
				printf("About to send broadcast!!\n");
				msg.messageType = -1;
				//int j;
				for(j=1;j<(numReporters+numEditors+1);j++){MPI_Isend(&msg, 1, messagetype, j, 0, MPI_COMM_WORLD,&req);printf("Send\n");}
				printf("broadcast sent\n");
				//exit(-1);
		break;
			

		case 1 ... numEditors:
			//editor
			//recieve the mapping
			printf("I am the Editor.   %d\n",myid);
			MPI_Recv(wrapper, 1, wrappertype, source, 5, MPI_COMM_WORLD, &status); 
			printf("Received the Wrapper Object\n");
			//print the 	mapping
			printNewsReporterMapping(&(wrapper->map), &(wrapper->rmap));
			printf("Successfully printed NewsReporter Mapping\n");

			MPI_Recv(emap, 1, emaptype, source, 5, MPI_COMM_WORLD, &status);
			MPI_Recv(eNewsMap, 1, enewsmaptype, source, 5, MPI_COMM_WORLD, &status);
			printf("Received the  EMAP Object\n");
			printEditorReporterMapping(emap,eNewsMap);
			//printf("%p %p\n", &map, &rmap);
			
			for(i=0;i<numNews;i++)
			{
				editor_count = 0;
				record[i].ready = 0;

				for(j=0;j<numEditors;j++){
					if((*eNewsMap)[j][i] == 1)editor_count++;
				}

				record[i].latestCopy = NULL;
				//record[i].latestCopy->timestamp = -1;
				record[i].numNewsFriends = editor_count - 1;
				record[i].rqst = NULL;
			}

			
			src = MPI_ANY_SOURCE;
			tag = MPI_ANY_TAG;

			while(MPI_Recv(&msg, 1, messagetype, src, tag, MPI_COMM_WORLD, &status) == MPI_SUCCESS)
			{
				if(msg.messageType == -1) {printf("Editor Exiting\n");break;}

				if(status.MPI_SOURCE ==0){
					//this should not happen ever!!!!

					;
				} 
				else if(msg.source <= numEditors) {  //meaning the message was from one of the editors!!!
					
					editor = msg.source;
					newsid = status.MPI_TAG;
					if(msg.messageType == 1)
					{ // this is a reqquest form one of the editors ..
						printf("editor: %d requested for news: %d with time: %d\n", msg.source, newsid, msg.update.timestamp);
						update = &(msg.update);

						//first update of newsid recieved
						if(record[newsid].latestCopy == NULL)
						{
							record[newsid].latestCopy = malloc(sizeof(news));
							copyNewsStructure(record[newsid].latestCopy, update);
							record[newsid].ready = 0;
							msg.messageType = 2;
							printf("sending back positive feedback.\n");
							msg.source = myid;
							MPI_Isend(&msg, 1, messagetype, editor, newsid, MPI_COMM_WORLD, &req);
							continue;
						}

						myLatestCopy = record[newsid].latestCopy;

						//He has the latest copy
						if(update->timestamp > myLatestCopy->timestamp)
						{
							copyNewsStructure(record[newsid].latestCopy, update);
							record[newsid].ready = 0;

							// Message tempMsg;
							// tempMsg.messageType = 2;
							// tempMsg.update = update;
							msg.messageType = 2;
							printf("sending back positive feedback.\n");
							msg.source = myid;
							MPI_Isend(&msg, 1, messagetype, editor, newsid, MPI_COMM_WORLD, &req);
							continue;
						}

						//we have the same copy
						else if(update->timestamp == myLatestCopy->timestamp)
						{
							//He has smaller id. Positive feedback.
							if(editor < myid)
							{
								// Message tempMsg;
								// tempMsg.messageType = 2;
								// tempMsg.update = update;
								msg.messageType = 2;
								printf("sending back positive feedback.\n");
								msg.source = myid;
								MPI_Isend(&msg, 1, messagetype, editor, newsid, MPI_COMM_WORLD, &req);
								continue;
							}

							//I have smaller id. Negative feedback.
							else
							{
								// Message tempMsg;
								// tempMsg.messageType = 3;
								// tempMsg.update = update;
								msg.messageType = 3;
								printf("sending back negative feedback.\n");
								msg.source = myid;
								MPI_Isend(&msg, 1, messagetype, editor, newsid, MPI_COMM_WORLD, &req);
								continue;
							}
						}

						//I have the latest copy
						else
						{
							// Message tempMsg;
							// tempMsg.messageType = 3;
							// tempMsg.update = record[newsid].latestCopy;
							msg.messageType = 3;
							printf("sending back negative feedback.\n");
							copyNewsStructure(&(msg.update), record[newsid].latestCopy);
							msg.source = myid;
							MPI_Isend(&msg, 1, messagetype, editor, newsid, MPI_COMM_WORLD, &req);
							continue;
						}
					}

					//Positive response from an editor
					else if(msg.messageType == 2)
					{
						//Drop the response if not expecting any.
						if(record[newsid].ready == 0)
							continue;

						printf("editor: %d responded positive for news: %d with time: %d\n", editor, newsid, record[newsid].latestCopy->timestamp);

						//see if expecting this specific response
						
						MPI_Request_get_status(record[newsid].rqst[editor-1] , &rqstCheck, &stat);
						if(rqstCheck!=0)
						{
							printf("counting this response.\n");
							record[newsid].ready--;
							if(record[newsid].ready==0)
							{
								printf("all responses recieved for news: %d with time: %d\n", newsid, record[newsid].latestCopy->timestamp);
								// Message tempMsg;
								// tempMsg.messageType = 0;
								// tempMsg.update = record[newsid].latestCopy;
								msg.messageType = 0;
								copyNewsStructure(&(msg.update), record[newsid].latestCopy);
								msg.source = myid;
								MPI_Isend(&msg, 1, messagetype, editor, newsid, MPI_COMM_WORLD, &req);
							}
						}
					}
				}
				else{ //NEWS received from one of the reporter....

					newsid = status.MPI_TAG;
					reporter = msg.source - numEditors;

					printf("for news: %d , msgtype: %d from: %d\n", newsid, msg.messageType, msg.source);

					//exclusive news. publish.
					if(record[newsid].numNewsFriends ==0)
					{
						sprintf(dest, "./publication/out%d.txt", newsid);
						publishNews(dest, &(msg.update));
					}

					//Shared news. Communicate with newsFriends
					else
					{
						//Check if the update is already published

						if(record[newsid].latestCopy != NULL){
							if(record[newsid].latestCopy->timestamp >= msg.update.timestamp){
								continue;
							}
						}

						//Update record for this news update
						record[newsid].ready = record[newsid].numNewsFriends;
						if(record[newsid].latestCopy == NULL) record[newsid].latestCopy = (news*)malloc(sizeof(news));

						copyNewsStructure(record[newsid].latestCopy, &(msg.update));

						// int i;
						// for(i=0;i<record[newsid].numNewsFriends;i++)
						// 	record[newsid].pendingRequests[i]++;

						//if(record[newsid].rqst != NULL) free(record[newsid].rqst);
						record[newsid].rqst = malloc(numEditors * sizeof(MPI_Request));
						
						//Prepare to communicate
						// Message tempMsg;
						// tempMsg.messageType = 1;
						// tempMsg.update = msg.update;
						msg.messageType = 1;
						
						//Send messages to newsFriends
						for(i=0;i<numEditors;i++)
						{
							if((*eNewsMap)[i][newsid] ==1 && (i+1) != myid)
							{
								msg.source = myid;
								printf("Requesting editor: %d for news: %d with time: %d\n", i+1, newsid, msg.update.timestamp);
								MPI_Isend(&msg, 1, messagetype, i+1, newsid, MPI_COMM_WORLD, &(record[newsid].rqst[i]));
							}
						}
					}

				}
				printf("%d\n", status.MPI_TAG);

				sprintf(dest, "./publication/out%d.txt", status.MPI_TAG);
				publishNews(dest, &(msg.update));
			
			}

		break;

		default:
			//reporters

			//recieve the mapping
			printf("WAITING FOR THE MAP AND RMAP\n");
			MPI_Recv(wrapper, 1, wrappertype, 0, 5, MPI_COMM_WORLD, &status);
			printf("received map and ramp\n");
			//print the mapping
			//printf("%p %p\n", &map, &rmap);
			printNewsReporterMapping(&(wrapper->map), &(wrapper->rmap));
			MPI_Recv(emap, 1, emaptype, 0, 5, MPI_COMM_WORLD, &status);
			MPI_Recv(eNewsMap, 1, enewsmaptype, 0, 5, MPI_COMM_WORLD, &status);
			printEditorReporterMapping(emap,eNewsMap);
			
			id = myid - numEditors;

			printf("I am the reporter: %d\n", id);
			//src = MPI_ANY_SOURCE;
			//tag = MPI_ANY_TAG;
			editor = id % numEditors;

			//check if initialized properly
			
			for(i=0;i<numNews;i++)
			{
				record[i].ready = 0;
				record[i].numNewsFriends = wrapper->map[i][0] - 1;
				record[i].latestCopy = NULL;
				//record[i].latestCopy->timestamp = -1;
				record[i].rqst = NULL;
			}

			while(MPI_Recv(&msg, 1, messagetype, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status) == MPI_SUCCESS)
			{
				//end of simulation
				if(msg.messageType == -1) {printf("Reporter Exiting\n");break;}

				//printf("tag: %d , id: %d\n", status.MPI_TAG, status.MPI_SOURCE);

				//Update recieved from source
				if(status.MPI_SOURCE = 1 && status.MPI_TAG == msg.update.newsId && msg.messageType == 0)
				{
					newsid = status.MPI_TAG;
					size = wrapper->map[newsid][0];

					printf("for news: %d , msgtype: %d from: %d\n", newsid, msg.messageType, status.MPI_SOURCE);

					//exclusive news. Send it to editor.
					if(size==1)
					{
						// Message tempMsg;
						// tempMsg.messageType = 0;
						// tempMsg.update = msg.update;
						msg.messageType = 0;
						msg.source = myid;
						MPI_Isend(&msg, 1, messagetype, editor, newsid, MPI_COMM_WORLD, &req);
					}

					//Shared news. Communicate with newsFriends
					else
					{
						//Check if the update is already published

						if(record[newsid].latestCopy != NULL){
							if(record[newsid].latestCopy->timestamp >= msg.update.timestamp){
								continue;
							}
						}

						//Update record for this news update
						record[newsid].ready = record[newsid].numNewsFriends;
						if(record[newsid].latestCopy == NULL) record[newsid].latestCopy = (news*)malloc(sizeof(news));

						copyNewsStructure(record[newsid].latestCopy, &(msg.update));

						// int i;
						// for(i=0;i<record[newsid].numNewsFriends;i++)
						// 	record[newsid].pendingRequests[i]++;

						//if(record[newsid].rqst != NULL) free(record[newsid].rqst);
						record[newsid].rqst = malloc(numReporters * sizeof(MPI_Request));
						
						//Prepare to communicate
						// Message tempMsg;
						// tempMsg.messageType = 1;
						// tempMsg.update = msg.update;
						msg.messageType = 1;
						
						//Send messages to newsFriends
						for(i=1;i<(numReporters+1);i++)
						{
							if(wrapper->map[newsid][i]==1 && i != id)
							{
								msg.source = myid;
								printf("Requesting reporter: %d for news: %d with time: %d\n", i, newsid, msg.update.timestamp);
								MPI_Isend(&msg, 1, messagetype, i + numEditors, newsid, MPI_COMM_WORLD, &(record[newsid].rqst[i-1]));
							}
						}
					}
				}

				//Message recieved from some reporter
				else
				{
					reporter = msg.source - numEditors;
					newsid = status.MPI_TAG;

					//Request from a reporter
					if(msg.messageType==1)
					{
						printf("reporter: %d requested for news: %d with time: %d\n", reporter, newsid, msg.update.timestamp);
						update = &(msg.update);

						//first update of newsid recieved
						if(record[newsid].latestCopy == NULL)
						{
							record[newsid].latestCopy = malloc(sizeof(news));
							copyNewsStructure(record[newsid].latestCopy, update);
							record[newsid].ready = 0;
							msg.messageType = 2;
							printf("sending back positive feedback.\n");
							msg.source = myid;
							MPI_Isend(&msg, 1, messagetype, reporter+numEditors, newsid, MPI_COMM_WORLD, &req);
							continue;
						}

						myLatestCopy = record[newsid].latestCopy;

						//He has the latest copy
						if(update->timestamp > myLatestCopy->timestamp)
						{
							copyNewsStructure(record[newsid].latestCopy, update);
							record[newsid].ready = 0;

							// Message tempMsg;
							// tempMsg.messageType = 2;
							// tempMsg.update = update;
							msg.messageType = 2;
							printf("sending back positive feedback.\n");
							msg.source = myid;
							MPI_Isend(&msg, 1, messagetype, reporter+numEditors, newsid, MPI_COMM_WORLD, &req);
							continue;
						}

						//we have the same copy
						else if(update->timestamp == myLatestCopy->timestamp)
						{
							//He has smaller id. Positive feedback.
							if(reporter < id)
							{
								// Message tempMsg;
								// tempMsg.messageType = 2;
								// tempMsg.update = update;
								msg.messageType = 2;
								printf("sending back positive feedback.\n");
								msg.source = myid;
								MPI_Isend(&msg, 1, messagetype, reporter+numEditors, newsid, MPI_COMM_WORLD, &req);
								continue;
							}

							//I have smaller id. Negative feedback.
							else
							{
								// Message tempMsg;
								// tempMsg.messageType = 3;
								// tempMsg.update = update;
								msg.messageType = 3;
								printf("sending back negative feedback.\n");
								msg.source = myid;
								MPI_Isend(&msg, 1, messagetype, reporter+numEditors, newsid, MPI_COMM_WORLD, &req);
								continue;
							}
						}

						//I have the latest copy
						else
						{
							// Message tempMsg;
							// tempMsg.messageType = 3;
							// tempMsg.update = record[newsid].latestCopy;
							msg.messageType = 3;
							printf("sending back negative feedback.\n");
							copyNewsStructure(&(msg.update), record[newsid].latestCopy);
							msg.source = myid;
							MPI_Isend(&msg, 1, messagetype, reporter+numEditors, newsid, MPI_COMM_WORLD, &req);
							continue;
						}
					}

					//Positive response from a reporter
					else if(msg.messageType==2)
					{
						//Drop the response if not expecting any.
						if(record[newsid].ready == 0)
							continue;

						printf("reporter: %d responded positive for news: %d with time: %d\n", reporter, newsid, record[newsid].latestCopy->timestamp);

						//see if expecting this specific response
						
						MPI_Request_get_status(record[newsid].rqst[reporter-1] , &rqstCheck, &stat);
						if(rqstCheck!=0)
						{
							printf("counting this response.\n");
							record[newsid].ready--;
							if(record[newsid].ready==0)
							{
								printf("all responses recieved for news: %d with time: %d\n", newsid, record[newsid].latestCopy->timestamp);
								// Message tempMsg;
								// tempMsg.messageType = 0;
								// tempMsg.update = record[newsid].latestCopy;
								msg.messageType = 0;
								copyNewsStructure(&(msg.update), record[newsid].latestCopy);
								msg.source = myid;
								MPI_Isend(&msg, 1, messagetype, editor, newsid, MPI_COMM_WORLD, &req);
							}
						}
					}

					//Negative response from a reporter
					else if(msg.messageType==3)
					{
						printf("reporter: %d responded negative for news: %d with time: %d and latest time is: %d\n", reporter, newsid, record[newsid].latestCopy->timestamp, msg.update.timestamp);
						record[newsid].ready = 0;
						copyNewsStructure(record[newsid].latestCopy, &(msg.update));
					}
				}
			}

			break;
	}
	MPI_Finalize();
}

int main(int argc, char *argv[])
{
	time_t t;
	
	//EditorReportMap ermap = {0};
	//EditorReportMap* emap = &ermap;
	srand((unsigned int) time(&t));
	//EditorReportMap* ermap = (EditorReportMap*)malloc(sizeof(EditorReportMap));
	
	//doEditorReporterMapping(ermap);
	//printEditorReporterMapping(ermap);
	//doEditorReporterMapping(emap);
	//printEditorReporterMapping(emap);
	BNPM(&argc, &argv);
}