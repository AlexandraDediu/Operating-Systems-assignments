#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATH_LEN 3000

#define __DEBUG

#ifdef __DEBUG
void debug_info (const char *file, const char *function, const int line)
{
    fprintf(stderr, "DEBUG. ERROR PLACE: File=\"%s\", Function=\"%s\", Line=\"%d\"\n", file, function, line);
}

#define ERR_MSG(DBG_MSG) { \
        perror(DBG_MSG); \
        debug_info(__FILE__, __FUNCTION__, __LINE__); \
}

#else                   // with no __DEBUG just displays the error message

#define ERR_MSG(DBG_MSG) { \
        perror(DBG_MSG); \
}

#endif


void listDir(char *dirName, int rec, int perm, int size_greater)
{
    DIR* dir;
    struct dirent *dirEntry;
    struct stat inode;
    char name[MAX_PATH_LEN];

    dir = opendir(dirName);
    if (dir == 0)
    {
        ERR_MSG ("Error opening directory");
        exit(4);
    }
    // iterate the directory contents
    while ((dirEntry=readdir(dir)) != 0)
    {
        if(strcmp(dirEntry->d_name,".")!=0 && strcmp(dirEntry->d_name,"..")!=0)
        {
            // build the complete path to the element in the directory
            snprintf(name, MAX_PATH_LEN, "%s/%s",dirName,dirEntry->d_name);

            // get info about the directory's element
            lstat (name, &inode);
            if(size_greater==0)
            {
            	if (inode.st_mode & S_IXUSR && perm==1)		
            printf("%s\n", name); 
            else if(perm==0){
            	printf("%s\n", name); 
            } 
            }

            else
            {
            	if(S_ISREG(inode.st_mode)==1 && inode.st_size>=size_greater)
            	{
            		if (inode.st_mode & S_IXUSR && perm==1)		
           			printf("%s\n", name); 
            		else if(perm==0){
            			printf("%s\n", name); 
            		}
            	}
            }
 
            
            
            if(rec==1 && S_ISDIR(inode.st_mode)){
            	listDir(name, rec, perm, size_greater);

            }
        }
    }
    closedir(dir);

}

typedef struct __attribute__((__packed__)){
	char SECTION_NAME[18];
	unsigned char SECTION_TYPE;
	unsigned int SECT_OFFSET;
	unsigned int SECT_SIZE;
}SECTION_HEADER;


typedef struct {
	unsigned char VERSION;
	unsigned char NO_OF_SECTIONS;
	SECTION_HEADER *SECTION_HEADERS;
    short HEADER_SIZE;
    char MAGIC;
}headerStruct;


int verify( char *filePath){
  int openFile=open(filePath, O_RDONLY);
  lseek(openFile, -1, SEEK_END);
  headerStruct header;
  read(openFile, &header.MAGIC, sizeof(header.MAGIC));
  if(header.MAGIC!='n'){
  	printf("ERROR\n");
     printf("wrong magic");
     return -1;}

   lseek(openFile, -3, SEEK_END);
   read(openFile, &header.HEADER_SIZE, 2);

   	lseek(openFile, -header.HEADER_SIZE, SEEK_END);
   	read(openFile, &header.VERSION, sizeof(header.VERSION));
 	if((120>header.VERSION) || (header.VERSION>156)){
      	printf("ERROR\n");
     	printf("wrong version");
    	return -1;
	}

   	read(openFile, &header.NO_OF_SECTIONS, sizeof(header.NO_OF_SECTIONS));
 	if((5 > header.NO_OF_SECTIONS) || (header.NO_OF_SECTIONS > 12)){
     	printf("ERROR\n");
     	printf("wrong sect_nr");
     	return -1;
 	}

 	SECTION_HEADER section;
 	for(int i=0; i<header.NO_OF_SECTIONS; i++){
	 	read(openFile, &section, sizeof(SECTION_HEADER));
	   	if((section.SECTION_TYPE != 72) && (section.SECTION_TYPE != 46) &&
	   		(section.SECTION_TYPE != 16) && (section.SECTION_TYPE != 23)){
	   	 	printf("ERROR\n");
	     	printf("wrong sect_types");
	     	return -1;
	   	}
 	}
 	close(openFile);
 return 0;
}

void parse(char *filePath){
  int openFile=open(filePath, O_RDONLY);
  lseek(openFile, -1, SEEK_END);
  headerStruct header;

  read(openFile, &header.MAGIC, sizeof(header.MAGIC));

  lseek(openFile, -3, SEEK_END);
  read(openFile, &header.HEADER_SIZE, 2);

  lseek(openFile, -header.HEADER_SIZE, SEEK_END);
  read(openFile, &header.VERSION, sizeof(header.VERSION));

  read(openFile, &header.NO_OF_SECTIONS, sizeof(header.NO_OF_SECTIONS));

  printf("SUCCESS\n");
  printf("version=%d\n", header.VERSION);
  printf("nr_sections=%d\n", header.NO_OF_SECTIONS);
  SECTION_HEADER section;
 	for(int i=0; i<header.NO_OF_SECTIONS; i++){
	 	read(openFile, &section, sizeof(SECTION_HEADER));

	 	printf("section%d: ", i+1);
	 	for(int j=0; j<18; j++){
	 		if(section.SECTION_NAME[j] == 0){
	 			break;
	 		}
	 		printf("%c", section.SECTION_NAME[j]);
	 	}
	 	printf(" %d %d\n", section.SECTION_TYPE, section.SECT_SIZE);
	}


}


char Buffaras[1000000];

void extract(char *filePath, int section, int line){
  int openFile=open(filePath, O_RDONLY);
  lseek(openFile, -1, SEEK_END);
  headerStruct header;

  read(openFile, &header.MAGIC, sizeof(header.MAGIC));

  lseek(openFile, -3, SEEK_END);
  read(openFile, &header.HEADER_SIZE, 2);

  lseek(openFile, -header.HEADER_SIZE, SEEK_END);
  read(openFile, &header.VERSION, sizeof(header.VERSION));

  read(openFile, &header.NO_OF_SECTIONS, sizeof(header.NO_OF_SECTIONS));

  
  	SECTION_HEADER sectiuta;
  	int i=0;
 	for(; i<header.NO_OF_SECTIONS; i++){
	 	read(openFile, &sectiuta, sizeof(SECTION_HEADER));
	 	if(i+1 == section){
	 		break;
	 	}
	 }

	 //printf("%x ", sectiuta.SECT_OFFSET);
	lseek(openFile, sectiuta.SECT_OFFSET, SEEK_SET);
	read(openFile, Buffaras, sectiuta.SECT_SIZE);

	int currLine = 1;
	printf("SUCCESS\n");
	for(int i=0; i<sectiuta.SECT_SIZE; i++){
		currLine += (Buffaras[i] == '\n');
		if(currLine == line){
			int k ;
			if(Buffaras[i] == '\n'){
				k = i+1;
			}
			else{
				k = i;
			}
			for(; Buffaras[k] != '\n' && Buffaras[k] != '\0'; k++){
				printf("%c", Buffaras[k]);
			}
			break;
		}
	}

}

void findall (char *dirName){	
DIR* dir;
    struct dirent *dirEntry;
    struct stat inode;
    char name[MAX_PATH_LEN];

    dir = opendir(dirName);
    if (dir == 0)
    {
        ERR_MSG ("Error opening directory");
        exit(4);
    }
    // iterate the directory contents
    while ((dirEntry=readdir(dir)) != 0)
    {
        if(strcmp(dirEntry->d_name,".")!=0 && strcmp(dirEntry->d_name,"..")!=0)
        {
            // build the complete path to the element in the directory
            snprintf(name, MAX_PATH_LEN, "%s/%s",dirName,dirEntry->d_name);

            // get info about the directory's element
            lstat (name, &inode);
           
            
            if(S_ISREG(inode.st_mode) && verify(name)==0){


            	int openFile=open(name, O_RDONLY);
  lseek(openFile, -1, SEEK_END);
  headerStruct header;

  read(openFile, &header.MAGIC, sizeof(header.MAGIC));

  lseek(openFile, -3, SEEK_END);
  read(openFile, &header.HEADER_SIZE, 2);

  lseek(openFile, -header.HEADER_SIZE, SEEK_END);
  read(openFile, &header.VERSION, sizeof(header.VERSION));

  read(openFile, &header.NO_OF_SECTIONS, sizeof(header.NO_OF_SECTIONS));
  SECTION_HEADER section;
  int ok=1;
 	for(int i=0; i<header.NO_OF_SECTIONS; i++){
	 	read(openFile, &section, sizeof(SECTION_HEADER));
	 	
	 	if(section.SECT_SIZE >=1305) ok=0;
	 }
	 if(ok==1) {printf("%s\n", name);}
	 
   }
   else if(S_ISDIR(inode.st_mode))
            {
            	findall(name);
            }

       
   }
}
   closedir(dir);
  }


int main(int argc, char **argv){

    if(argc >= 2){

        if(strcmp(argv[1], "variant") == 0){
            printf("47096\n");
        }

        else if(strcmp(argv[1], "list") == 0)
        { 
            if(strstr(argv[2], "path"))
            {
                printf("SUCCESS\n");
                listDir(argv[2]+5,0, 0,0);
            }
            else
            {
                if(strcmp(argv[2], "recursive") == 0)
                {
                    if(strstr(argv[3], "path"))
                    {
                        printf("SUCCESS\n");
                        listDir(argv[3]+5, 1, 0,0);
                    }
                    else if(strcmp(argv[3], "has_perm_execute")==0){

                    	if(strstr(argv[4], "path")){
                    		printf("SUCCESS\n");
                    	   listDir(argv[4]+5, 1,1,0);
                    	}
                    	

                	}
                } 
                else if (strstr(argv[2], "size_greater=")){
                	int size_greater=atoi(argv[2]+13);
                    if(strcmp(argv[3], "has_perm_execute")==0){
                    
                     if(strstr(argv[4], "path")){
                     	printf("SUCCESS\n");
						listDir(argv[4]+5, 0,1, size_greater);
                     }
                    
                  }
                  else if(strstr(argv[3], "path")){
                     	printf("SUCCESS\n");
						listDir(argv[3]+5, 0,0, size_greater);
                  }
                 
                  else if(strcmp(argv[3], "recursive")==0){
                    if(strcmp(argv[4], "has_perm_execute")==0){
                    
                     if(strstr(argv[5], "path")){
                     	printf("SUCCESS\n");
						listDir(argv[5]+5, 1,1, size_greater);
                     }
                    }
                    else if(strstr(argv[4], "path")){
                     	printf("SUCCESS\n");
						listDir(argv[4]+5, 1,0, size_greater);
                     }
                    
                  }
               
                }
             	
             	else if(strcmp(argv[2], "has_perm_execute") == 0)
                {
                    if(strstr(argv[3], "path"))
                    {
                        printf("SUCCESS\n");
                        listDir(argv[3]+5, 0, 1,0);
                    }
                    else if(strcmp(argv[3], "recursive")==0){

                    	if(strstr(argv[4], "path")){
                    		printf("SUCCESS\n");
                    	   listDir(argv[4]+5, 1,1,0);
                    	}

                    }
                }
            }
       }
   

   
       else if(strcmp(argv[1], "parse")==0){
       	char buff[101];
       	sscanf(argv[2], "path=%s", buff);
       	//printf("%s", buff);
        	if(verify(buff) == 0){
        		parse(buff);
          	}
        }

        else if(strcmp(argv[1], "extract")==0){
        	char buff [101];
            int section, line;

            sscanf(argv[2], "path=%s", buff);
            sscanf(argv[3], "section=%d", &section);
            sscanf(argv[4], "line=%d", &line);

            extract(buff, section, line);
          }
        else if(strcmp(argv[1], "findall")==0){

        	
        	printf("SUCCESS\n");
        	
        	findall(argv[2]+5);
        	
        }
   } 

   
    return 0;
}
