#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>

char *getGroupName(gid_t);
char *getOwnerName(uid_t);
static void UnixLs(char *, bool, bool, bool);
static void PrintLongFormat (struct stat, char*, char*);
static void PrintFiles(char *, char *, struct stat, bool, bool);
static void PrintDirectories(char *, bool, bool, bool);

int main(int argc, char *argv[])
{
    int j;
    bool i = false, l = false, R = false;
    // No Option and current directory
    if (argc == 1){
        UnixLs(".", false, false, false);
    }
    else{
    	char *tmp = *(++argv);
    	while (strncmp(tmp, "-", 1) == 0){
    		int len = strlen(tmp);
            // Find all options in argvs
            for (j = 1; j < len; j++){
               if (tmp[j] == 'i'){
                   i = true;
               }
               else if (tmp[j] == 'l'){
                   l = true;
               }
               else if (tmp[j] == 'R'){
                   R = true;
               }
            }
            tmp = *(++argv);
            // There are only options
            if (tmp == NULL){
            	// Current directory
  				UnixLs(".", i, l, R);
  				return 0;
  			}	
    	}
    	while (tmp != NULL){
    		UnixLs(tmp, i, l, R);
    		tmp = *(++argv);
    	}
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
char *getGroupName(gid_t gidNum){
	struct group *grp;
	grp = getgrgid(gidNum);
	if (grp == NULL){
		perror("getgrgid() error: ");
		return NULL;
	}
	else{
		return grp->gr_name;
	}
}

char *getOwnerName(uid_t uidNum){
	struct passwd *pw;
	pw = getpwuid(uidNum);
	if (pw == NULL){
		perror("getpwuid() error: ");
		return NULL;
	}
	else{
		return pw->pw_name;
	}
}

static void PrintLongFormat(struct stat buf, char *filename, char *filepath){
	char column1[11], column6[20];
	snprintf(column1, 11, "%c%c%c%c%c%c%c%c%c%c", 
		(S_ISDIR(buf.st_mode)) ? 'd' : '-',
		(buf.st_mode & S_IRUSR) ? 'r' : '-',
		(buf.st_mode & S_IWUSR) ? 'w' : '-',
		(buf.st_mode & S_IXUSR) ? 'x' : '-',
		(buf.st_mode & S_IRGRP) ? 'r' : '-',
		(buf.st_mode & S_IWGRP) ? 'w' : '-',
		(buf.st_mode & S_IXGRP) ? 'x' : '-',
		(buf.st_mode & S_IROTH) ? 'r' : '-',
		(buf.st_mode & S_IWOTH) ? 'w' : '-',
		(buf.st_mode & S_IXOTH) ? 'x' : '-');
	if (S_ISLNK(buf.st_mode)){
		column1[0] = 'l';
	}
	printf("%s ", column1);
	printf("%2d ", (int) buf.st_nlink);
	printf("%s ", getOwnerName(buf.st_uid));
	printf("%s ", getGroupName(buf.st_gid));
	printf("%10ld ", (long) buf.st_size);
	strftime(column6, 20, "%b %d %Y %H:%M", localtime(&(buf.st_mtime)));
	printf("%s ", column6);
	if (S_ISLNK(buf.st_mode)){
		char buff[256];
		int numBytes = readlink(filepath, buff, 255);
		if (numBytes == -1){
			perror("readlin() error: ");
		}
		buff[numBytes] = '\0';
		printf("%s -> %s\n", filename, buff); 
	}
	else{
		printf("%s\n", filename);
	}
}
static void PrintFiles(char *filename, char *filepath, struct stat buf, bool i, bool l){
	if (i == true){
		printf("%8ld ", (long) buf.st_ino);
	}
	if (l == true){
		PrintLongFormat(buf, filename, filepath);
	}
	else{
		printf("%s\n", filename);
	}
}
static void PrintDirectories(char *dirpath, bool i, bool l, bool R){
    // Local Variables
    DIR *dirp;
    struct dirent *dp;
    struct stat buf;
    // opendir
    dirp = opendir(dirpath);
    if (dirp == NULL){
        perror("opendir() error: ");
        return;
    }
    if (R == true){
    	printf("%s:\n", dirpath);
    }
    // Iterate in the directory 
    errno = 0;
    while ((dp = readdir(dirp))){
        // Skip hidden files or directories
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0 ){
            continue;
        }
        // In the case of different directories from the current, make it the full path of the file
        char filepath[512];
        snprintf(filepath, 512, "%s/%s", dirpath, dp->d_name);
        // Stat()
        if (lstat(filepath, &buf) == -1){
            perror("stat() error: ");
            return;
        }
        PrintFiles(dp->d_name, filepath, buf, i, l);
    }
    printf("\n");
    if (errno != 0){
        perror("readdir() error: ");
    }
    //Rewind and print subdirectories
 	if (R == true){
 		rewinddir(dirp);
 		errno = 0;
 		while ((dp = readdir(dirp))){
 			if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0 ){
            	continue;
        	}
	        // In the case of different directories from the current, make it the full path of the file
	        char filepath[512];
	        snprintf(filepath, 512, "%s/%s", dirpath, dp->d_name);
	        // Stat()
	        if (lstat(filepath, &buf) == -1){
	            perror("stat() error: ");
	            return;
	        }
	        if (S_ISDIR(buf.st_mode)){
	        	PrintDirectories(filepath, i, l, R);        	
	        }
 		}
 		if (errno != 0){
        	perror("readdir() error: ");
    	}
 	}
    if (closedir(dirp) == -1){
        perror("closedir() error: ");
    }

}
static void UnixLs(char *entry, bool i, bool l, bool R){
    struct stat buf;
    // In the case of different directories from the current, make it the full path of the file
    if (lstat(entry, &buf) == -1){
        perror("stat() error: ");
        return;
    }
    if (S_ISREG(buf.st_mode)){
    	PrintFiles(entry, entry, buf, i, l);
    }
    else{
    	PrintDirectories(entry, i, l, R);
    }
}