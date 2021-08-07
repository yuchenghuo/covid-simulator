#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mm_malloc.h>

struct Data {
	char* country;
	char* time;
	char* type;
	char* total;
	char* per;
};

int strl(char* str){
	int n=0;
	while(str[n]!='\0') ++n;
	return n;
}

char* copyOutStr(char* str){
	// printf(str);
	int len = strlen(str)+1;
	static char* nstr;
	nstr = (char*) malloc(len*sizeof(char));
	for(int i=0;i<len;++i){
		nstr[i] = str[i];
	}
	// printf(nstr);
	return nstr;
}

struct Data copyOutData(struct Data data){
	static struct Data ndat;
	ndat.country = copyOutStr(data.country);
	ndat.time = copyOutStr(data.time);
	ndat.type = copyOutStr(data.type);
	ndat.total = copyOutStr(data.total);
	ndat.per = copyOutStr(data.per);
	return ndat;
}

int fetchData(long long int*const target){
	FILE *fp = popen("node fetch.js", "r");
	if(fp == NULL){
		printf("Runtime got an error.\n");
		return 0;
	}

	int ch;
	int prev=0;
	int structi = 0;
	int dlen = 1;
	char* tmp = (char*)malloc(255*sizeof(char));
	int tmpi=0;
	struct Data data;
	static struct Data* result;
	result = (struct Data*)malloc(sizeof(struct Data));
	while((ch=fgetc(fp))!=EOF){
		if(ch==';' && ch==prev){
			result[dlen-1] = copyOutData(data);
			result = (struct Data*) realloc(result, (dlen+1)*sizeof(struct Data));
			dlen++;
			structi=0;
		}else if(ch==';'){
			// next var
			tmp[tmpi]='\0';
			// printf(tmp);
			switch(structi){
				case 0:
					data.country = copyOutStr(tmp);
					break;
				case 1:
					data.time = copyOutStr(tmp);
					break;
				case 2:
					data.type = copyOutStr(tmp);
					break;
				case 3:
					data.total = copyOutStr(tmp);
					break;
				case 4:
					data.per = copyOutStr(tmp);
					break;
			}
			structi++;
			tmpi=0;
		}else{
			tmp[tmpi++]=(char)ch;
		}
		prev = ch;
	}
	pclose(fp);
	*target = (long long) &result;
	printf("Done\n");
	return dlen-1;
}

