#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <limits.h>
#include <ctype.h>
#include <dirent.h>
#include <grp.h>
#include <stdint.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <locale.h>
#include <langinfo.h>
#include <sys/param.h>
#include <setjmp.h>
#include <fcntl.h>
#include <time.h> 
#define FORMAT "%5s %s\t%8s %s\n"
#define MAXCOM 1000
#define MAXLIST 1000
#define clear() printf("\033[H\033[J")
#define OUT 0
#define IN 1
struct passwd *pwd1;
struct group *grp;
struct tm *tm;
char datestring[256];
#define BUFF_SIZE 1024
char gs[1000];
const char s3[2] = ".";
const char s4[2] = "..";

int takeInput(char *str)
{
	char *buf;
	buf = readline("$ ");
	if (strlen(buf) != 0)
	{
		add_history(buf);
		strcpy(str, buf);
		return 0;
	}
	else
	{
		return 1;
	}
}

void printDir()
{
	char cwd[1024];
	char hostname[HOST_NAME_MAX];
	gethostname(hostname, HOST_NAME_MAX);
	//printf("%s",hostname);
	//printf("%s",username);
	//printf("%s",gethostname(hostname, HOST_NAME_MAX));
	//printf("%s",getlogin_r(username, LOGIN_NAME_MAX));
	char *username = getenv("USER");
	printf("%s@%s:", username, hostname);
	getcwd(cwd, sizeof(cwd));
	printf("%s", cwd);
}

void print_history_list()
{
	register HIST_ENTRY **his_list;
	register int i;
	his_list = history_list();
	if (his_list)
		for (i = 0; his_list[i]; i++)
			printf("%d: %s\n", i + history_base, his_list[i]->line);
}

unsigned countWords(char *str)
{
	int state = OUT;
	unsigned wc = 0;
	while (*str)
	{
		if (*str == ' ' || *str == '\n' || *str == '\t')
			state = OUT;
		else if (state == OUT)
		{
			state = IN;
			++wc;
		}
		++str;
	}
	return wc;
}

int main()
{
	char inputString[MAXCOM], *parsedArgs[MAXLIST], *cp = "cp", *ps = "ps", *end = "exit", copystr[MAXCOM], *ls = "ls", *mv = "mv", *grep = "grep";
	char *parsedArgsPiped[MAXLIST];
	char *delimeters = " \t\n\f\r\v";
	int execFlag = 0;
	clear();
	//takeInput(inputString);
	while (1)
	{
		printDir();
		//fgets(inputString, MAXCOM, stdin);
		if (takeInput(inputString))
			continue;
		char *token;
		strcpy(copystr, inputString);
		token = strtok(inputString, delimeters);
		// if((*token == *cf)||(*token==*ch))
		// {printf("inside fork AV");
		// 	execFlag = processString(copystr, parsedArgs, parsedArgsPiped);
		// 	if (execFlag == 1)
		//  		execArgs(parsedArgs);
		// 	if (execFlag == 2)
		// 		execArgsPiped(parsedArgs, parsedArgsPiped);
		// }
		if (strcmp(token, ls) == 0)
		{
			const char s[2] = " ";
			char *stoken = strtok(NULL, delimeters);
			DIR *dir;
			struct dirent *files;
			struct stat info;
			char buf[1024];
			if (stoken == NULL)
			{
				getcwd(gs, 999);
				dir = opendir(gs);
				while ((files = readdir(dir)) != NULL)
				{
					if (!(strcmp(files->d_name, ".") == 0 || strcmp(files->d_name, "..") == 0))
						printf("%s\t", files->d_name);
				}
				closedir(dir);
				printf("\n");
			}
			else if (strcmp(stoken, "-l") == 0 || strcmp(stoken, "-al") == 0 || strcmp(stoken, "-la") == 0 || strcmp(stoken, "-l-a") == 0 || strcmp(stoken, "-a-l") == 0)
			{
				getcwd(gs, 999);
				dir = opendir(gs);
				while ((files = readdir(dir)) != NULL)
				{
					if (!(strcmp(files->d_name, ".") == 0 || strcmp(files->d_name, "..") == 0))
					{
						if (stat(files->d_name, &info) == -1)
							continue;

						if (S_ISDIR(info.st_mode))
							printf("d");
						else
							printf("-");
						if (S_IRUSR & info.st_mode)
							printf("r");
						else
							printf("-");
						if (info.st_mode & S_IWUSR)
							printf("w");
						else
							printf("-");
						if (info.st_mode & S_IXUSR)
							printf("x");
						else
							printf("-");
						if (info.st_mode & S_IRGRP)
							printf("r");
						else
							printf("-");
						if (info.st_mode & S_IWGRP)
							printf("w");
						else
							printf("-");
						if (info.st_mode & S_IXGRP)
							printf("x");
						else
							printf("-");
						if (info.st_mode & S_IROTH)
							printf("r");
						else
							printf("-");
						if (info.st_mode & S_IWOTH)
							printf("w");
						else
							printf("-");
						if (info.st_mode & S_IXOTH)
							printf("x");
						else
							printf("-");
						//	printf("%4d",info.st_nlink);
						if ((pwd1 = getpwuid(info.st_uid)) != NULL)
							printf(" %-8.8s", pwd1->pw_name);
						else
							printf(" %-8d", info.st_uid);
						if ((grp = getgrgid(info.st_gid)) != NULL)
							printf(" %-8.8s", grp->gr_name);
						else
							printf(" %-8d", info.st_gid);
						printf(" %9jd ", (intmax_t)info.st_size);
						tm = localtime(&info.st_mtime);
						/* Get localized date string. */
						strftime(datestring, sizeof(datestring), nl_langinfo(D_T_FMT), tm);
						printf("%s ", datestring);
						printf(" %d ", files->d_type);
						printf("%s \n", files->d_name);
					}
				}
				//	printf("\n");
				closedir(dir);
			}

			else if (strcmp(stoken, "-a") == 0)
			{
				getcwd(gs, 999);
				dir = opendir(gs);
				while ((files = readdir(dir)) != NULL)
				{
					if (strcmp(files->d_name, ".") || strcmp(files->d_name, ".."))
						printf("%s\t", files->d_name);
				}
				closedir(dir);
				printf("\n");
			}
			else if (strcmp(stoken, "&") == 0)
			{
				getcwd(gs, 999);
				dir = opendir(gs);
				while ((files = readdir(dir)) != NULL)
				{
					if (!(strcmp(files->d_name, ".") == 0 || strcmp(files->d_name, "..") == 0))
						printf("%s\n", files->d_name);
				}
				closedir(dir);
			}
			else
			{
				dir = opendir(stoken);
				while ((files = readdir(dir)) != NULL)
				{
					if (!(strcmp(files->d_name, ".") == 0 || strcmp(files->d_name, "..") == 0))
						printf("%s\n", files->d_name);
				}
				closedir(dir);
			}
		}
		else if (strcmp(token, ps) == 0)
		{
			DIR *dir;
			struct dirent *ent;
			int i, fd_self, fd;
			unsigned long time, stime;
			char flag, *tty;
			char cmd[256], tty_self[256], path[256], time_s[256];
			FILE *file;
			dir = opendir("/proc");
			fd_self = open("/proc/self/fd/0", O_RDONLY);
			sprintf(tty_self, "%s", ttyname(fd_self));
			printf(FORMAT, "PID", "TTY", "TIME", "CMD");

			while ((ent = readdir(dir)) != NULL)
			{
				flag = 1;
				for (i = 0; ent->d_name[i]; i++)
					if (!isdigit(ent->d_name[i]))
					{
						flag = 0;
						break;
					}

				if (flag)
				{
					sprintf(path, "/proc/%s/fd/0", ent->d_name);
					fd = open(path, O_RDONLY);
					tty = ttyname(fd);
					if (tty && strcmp(tty, tty_self) == 0)
					{
						sprintf(path, "/proc/%s/stat", ent->d_name);
						file = fopen(path, "r");
						fscanf(file, "%d%s%c%c%c", &i, cmd, &flag, &flag, &flag);
						cmd[strlen(cmd) - 1] = '\0';
						for (i = 0; i < 11; i++)
							fscanf(file, "%lu", &time);
						fscanf(file, "%lu", &stime);
						time = (int)((double)(time + stime) / sysconf(_SC_CLK_TCK));
						sprintf(time_s, "%02lu:%02lu:%02lu",
								(time / 3600) % 3600, (time / 60) % 60, time % 60);
						printf(FORMAT, ent->d_name, tty + 5, time_s, cmd + 1);
						fclose(file);
					}
					close(fd);
				}
			}
			close(fd_self);
		}
		else if (strcmp(token, cp) == 0)
		{
			int srcFD, destFD, nbread, nbwrite;
			char *buff[BUFF_SIZE];
			int len = countWords(copystr);
			if (len != 3)
			{
				printf("\nUsage: cpcmd source_file destination_file\n");
			}
			char *token1 = strtok(NULL, delimeters);
			srcFD = open(token1, O_RDONLY);

			if (srcFD == -1)
			{
				printf("\nError opening file %s errno = %d\n", token1, errno);
			}
			char *token2 = strtok(NULL, delimeters);
			destFD = open(token2, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

			if (destFD == -1)
			{
				printf("\nError opening file %s errno = %d\n", token2, errno);
			}
			while ((nbread = read(srcFD, buff, BUFF_SIZE)) > 0)
			{
				if (write(destFD, buff, nbread) != nbread)
					printf("\nError in writing data to %s\n", token2);
			}
			if (nbread == -1)
				printf("\nError in reading data from %s\n", token1);
			if (close(srcFD) == -1)
				printf("\nError in closing file %s\n", token1);
			if (close(destFD) == -1)
				printf("\nError in closing file %s\n", token2);
		}
		else if (strcmp(token, mv) == 0)
		{
			int len = countWords(copystr);
			//printf("%d", len);
			int src; //source file
			int dst; //destination file
			int n;	 // tracking variable
			char buf[1];
			char *token1, *token2;
			if (len != 3)
			{
				printf("usage ./a.out sourceFileName destinationFileName\n");
				//exit(1);
			}
			token1 = strtok(NULL, delimeters);
			src = open(token1, O_RDONLY);
			if (src == -1)
			{
				perror("can't find the source file");
				//exit(1);
			}
			token2 = strtok(NULL, delimeters);
			dst = creat(token2, 0777);
			dst = open(token2, O_WRONLY);
			if (dst == -1)
			{
				perror("can't create or open destination file");
				//exit(1);
			}
			while ((n = read(src, buf, 1)) > 0)
			{
				write(dst, buf, 1);
			}
			close(src);
			remove(token1);
			close(dst);
		}
		else if (strcmp(token, grep) == 0)
		{
			//int len = countWords(copystr);
			
		}
		else if (strcmp(token, end) == 0)
		{
			printf("Exiting Shell...\n");
			exit(0);
		}
	}
	return 0;
}
