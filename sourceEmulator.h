struct news{
	int newsId;
	int timestamp;
	char location[15];
	char category[15];
	char content[101];
};

typedef struct news news;
char* randomTextGenerator();
news* fillFile(char* filename, int id, bool firstTime);
void generateNews();

