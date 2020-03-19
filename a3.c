
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>


#define RESP_NAME "RESP_PIPE_47096"
#define REQ_NAME "REQ_PIPE_47096"
#define SHM_KEY 14946

typedef struct __attribute__((__packed__)){
	char SECTION_NAME[18];
	unsigned char SECTION_TYPE;
	unsigned int SECT_OFFSET;
	unsigned int SECT_SIZE;
}SECTION_HEADER;

typedef char* pchar;

int main(void)
{
	int fd1 = -1;
	int fd2 = -1;
	int no = 47096;

	char *success= "SUCCESS";
	int success_len=strlen(success);
	char *error= "ERROR";
	int error_len=strlen(error);

	char *connect = "CONNECT";
	int connect_len = strlen(connect);

	char *pong = "PONG";
	int pong_len = strlen(pong);
	
	char *sharedMem="CREATE_SHM";
	char sharedMem_len=(char)strlen(sharedMem);
	
	char *writeToShm="WRITE_TO_SHM";
	int writeToShm_len=strlen(writeToShm);

	char *mapFile= "MAP_FILE";
	int mapFile_len=strlen(mapFile);

	char *readFromFileOffset="READ_FROM_FILE_OFFSET";
	int readFromFileOffset_len=strlen(readFromFileOffset);

	char *readFromFileSection="READ_FROM_FILE_SECTION";
	int readFromFileSection_len=strlen(readFromFileSection);

	char *readFromLogicalSpaceOffset="READ_FROM_LOGICAL_SPACE_OFFSET";
	int readFromLogicalSpaceOffset_len=strlen(readFromLogicalSpaceOffset);
	
	char* str = (char*)malloc(sizeof(char)*30);
	int str_len = sizeof(str);

	
	int shmId;
	int number;
	pchar memorie;

	off_t sizeMap;
	char *data = NULL;

	char fileName[256];
	int fdMapFile;


	if(mkfifo(RESP_NAME, 0600) != 0){
		perror("ERROR\ncannot create the response pipe");
		exit(1);
	}

	fd1 = open(REQ_NAME, O_RDONLY);
	if(fd1 == -1){
		perror("ERROR\ncannot open the request pipe");
		exit(1);
	}

	fd2 = open(RESP_NAME, O_WRONLY);
	if(fd2 == -1){
		perror("ERROR\ncannot open the request pipe");
		exit(1);
	}

	write(fd2, &connect_len, sizeof(char));
	write(fd2, connect, connect_len);

	int isExit = 1;
	while(isExit){

		read(fd1, &str_len, sizeof(char));
		read(fd1, str, str_len);
		str[str_len] = '\0';

		if(strcmp(str, "EXIT") == 0){
			isExit = 0;
		}

		else if(strcmp(str, "PING")==0){
			write(fd2, &str_len, sizeof(char));
			write(fd2, str, str_len);
			write(fd2, &pong_len, sizeof(char));
			write(fd2, pong, pong_len);
			write(fd2, &no, sizeof(int));
		}

		else if(strcmp(str, "CREATE_SHM") == 0){
			read(fd1, &number, sizeof(int));
			shmId=shmget(SHM_KEY, number, IPC_CREAT| 0664);

			if(shmId<0){
				write(fd2, &sharedMem_len, sizeof(char));
				write(fd2, sharedMem, sharedMem_len);
				write(fd2, &error_len, sizeof(char));
				write(fd2, error, error_len);
				return 1;
			}

			memorie = (pchar)shmat(shmId, NULL, O_RDWR);
			write(fd2, &sharedMem_len, sizeof(char));
			write(fd2, sharedMem, sharedMem_len);
			write(fd2, &success_len, sizeof(char));
			write(fd2, success, success_len);

		}

		else if(strcmp(str, "WRITE_TO_SHM")==0){  
			int offset;
			int value;

			read(fd1, &offset, sizeof(int));
			read(fd1, &value, sizeof(int));

			if((offset + sizeof(int)) < number){ 
				*(int*)&memorie[offset]=value;

				write(fd2, &writeToShm_len, sizeof(char));
				write(fd2, writeToShm, writeToShm_len);

				write(fd2, &success_len, sizeof(char));
				write(fd2, success, success_len);
			}
			else{
				write(fd2, &writeToShm_len, sizeof(char));
				write(fd2, writeToShm, writeToShm_len);
				write(fd2, &error_len, sizeof(char));
				write(fd2, error, error_len);
			}

		}

		else if(strcmp(str, "MAP_FILE")==0){
			char fileNameSize = 0;

			read(fd1, &fileNameSize, sizeof(char));
			read(fd1, fileName, fileNameSize);


			fileName[(int)fileNameSize] = '\0';
			printf("%s\n", fileName);

			fdMapFile=open(fileName, O_RDONLY);

			if(fdMapFile==-1){
				
				write(fd2, &mapFile_len, sizeof(char));
				write(fd2, mapFile, mapFile_len);

				write(fd2, &error_len, sizeof(char));
				write(fd2, error, error_len);

				return 1;
			}

			sizeMap = lseek(fdMapFile, 0, SEEK_END);
			lseek(fdMapFile, 0, SEEK_SET);

			data=(char*)mmap(NULL,sizeMap, PROT_READ, MAP_PRIVATE, fdMapFile, 0);

			if(data == (void*)-1) {
				write(fd2, &mapFile_len, sizeof(char));
				write(fd2, mapFile, mapFile_len);
				write(fd2, &error_len, sizeof(char));
				write(fd2, error, error_len);
				return 1;

			}

			write(fd2, &mapFile_len, sizeof(char));
			write(fd2, mapFile, mapFile_len);

			write(fd2, &success_len, sizeof(char));
			write(fd2, success, success_len);
		}

		else if(strcmp(str, "READ_FROM_FILE_OFFSET")==0){
			int offset;
			int no_of_bytes;
			read(fd1, &offset, sizeof(int));
			read(fd1, &no_of_bytes, sizeof(int));

			if((offset+no_of_bytes)<sizeMap){
				memcpy(memorie, &data[offset], no_of_bytes);

				write(fd2, &readFromFileOffset_len, sizeof(char));
				write(fd2, readFromFileOffset, readFromFileOffset_len);
				write(fd2, &success_len, sizeof(char));
				write(fd2, success, success_len);
			}
			else{
				write(fd2, &readFromFileOffset_len, sizeof(char));
				write(fd2, readFromFileOffset, readFromFileOffset_len);

				write(fd2, &error_len, sizeof(char));
				write(fd2, error, error_len);
			}

		}

		else if(strcmp(str, "READ_FROM_FILE_SECTION")==0){
			int offset;
			int no_of_bytes;
			int section_no;
			short nr;
			//SECTION_HEADER *section;

			read(fd1, &offset, sizeof(int));
			read(fd1, &no_of_bytes, sizeof(int));
			read(fd1, &section_no, sizeof(int));

			nr = *(short *)&memorie[sizeMap-1-2];

			if(section_no+1 < nr){
				write(fd2, &readFromFileSection_len, sizeof(char));
				write(fd2, readFromFileSection, readFromFileSection_len);
				write(fd2, &error_len, sizeof(char));
				write(fd2, error, error_len);
			}

			write(fd2, &readFromFileSection_len, sizeof(char));
			write(fd2, readFromFileSection, readFromFileSection_len);
			write(fd2, &success_len, sizeof(char));
			write(fd2, success, success_len);
		}

		else if(strcmp(str, "READ_FROM_LOGICAL_SPACE_OFFSET")==0){
			int logical_offset;
			int no_of_bytes;

			read(fd1, &logical_offset, sizeof(int));
			read(fd1, &no_of_bytes, sizeof(int));

			write(fd2, &readFromLogicalSpaceOffset_len, sizeof(char));
			write(fd2, readFromLogicalSpaceOffset, readFromLogicalSpaceOffset_len);
			write(fd2, &success_len, sizeof(char));
			write(fd2, success, success_len);
		}

	}

	close(fd2);
	unlink(RESP_NAME);
	close(fd1);
	unlink(REQ_NAME);

	return 0;
}

