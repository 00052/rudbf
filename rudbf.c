#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <locale.h>
#include <wchar.h>
#include <iconv.h>
#include <errno.h>
int levels[200]={0},size=0;
void usage() {
	puts("Rudbf 1.0 (poilynx@gmx.com)\n"
	     "\tUsage:rudbf -f <udbf file>");
}


void print_bin(char *buf,int size) {
    int i;
    for(i=0;i<size;i++)
        printf("%0.2hhX",(unsigned char)buf[i]);
}


int put(int *list,int *size,char c) {
    int i;
    for(i=0;i<(*size);i++){
        if(list[i]==c) {
            list[100+i]++;
            return 0;
        }
    }

    list[*size]=(int)c;
    (*size)++;
    return 1;
}

int main(int argc,char *argv[])
{
    int i=0,j;
    char item[0x100],name[21],uname[20],passwd[20],unknow[20],status[20];
    char timeout;
    unsigned char type,uk1,uk2,uk3,uk4,uk5,uk6;

    FILE *file;

    if(argc<=2){
	    usage();
	    return 1;
    }
    if(strcmp(argv[1],"-f")){
	    printf("Unknow option '%s'.\n",argv[1]);
	    return 1;
    }

    file = fopen(argv[2],"rb");

    if(NULL==file){
	    printf("Open file failed,error %d\n",errno);
	    return 2;
    }

    while(fread(item,1,0x100,file)==0x100) {
        //printf("3 size = %d\n",size);
        if(item[42]==0)
            return 3;
        strncpy(name,item+22,20);
        name[20]=0;
        strncpy(passwd,item+11,11);
        passwd[11]=0;
        strncpy(uname,item,11);
        uname[11]=0;
        type = item[42];
        timeout = item[44];
        uk1 = item[72];
        uk2 = item[73];
        uk3 = item[63];
        uk4 = item[64];

        put(levels,&size,item[42]);
        {
            char name_utf8[40];

            char *p_name_utf8=name_utf8;
            char *p_name_gbk=name;

            size_t fromsize=strlen(p_name_gbk)+1;
            int name_str_len=fromsize;
            size_t tosize=sizeof(name_utf8);

            //char **from=&p_name_gbk;
            //char **to=&p_name_utf8;
            iconv_t ic;
            ic = iconv_open("UTF-8","GBK");
            strcpy(name_utf8,"unknow");
            iconv(ic,&p_name_gbk,&fromsize,&p_name_utf8,&tosize);
            iconv_close(ic);
            if(!strcmp(name_utf8,"N/A"))
                continue;
            fputs(name_utf8,stdout);
            for(j=0;j<20-name_str_len;j++)
            {
                putchar(' ');
            }
	    printf("%11s %11s %c %0.2X %0.2X %0.2X %0.2X %0.2X\n",uname,passwd,timeout?'Y':'N',type,uk1,uk2,uk3,uk4);
        }
	i++;
    }
    return 0;
}


