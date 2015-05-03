#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <dirent.h>
#include <string.h>

#include "dir.h"



static struct list *listAdd(struct list *li, const char *filename)
{
	struct list *tmp;
	tmp = malloc(sizeof(struct list));
	tmp->filename = malloc(strlen(filename)+1);
	strcpy(tmp->filename, filename);
// fprintf(stderr, "%s\n", filename);
	tmp->prev = li;
	return tmp;
}


void listDestroy(struct list *li)
{
	struct list *tmp;

	while(li != NULL) {
		tmp = li;
		li = li->prev;
		free(tmp->filename);
		free(tmp);
	}
}


void listDump(struct list *li)
{
	int i = 0;

	while(li != NULL) {
		printf("ListDump(): %d) %s\n", i++, li->filename);
		li = li->prev;
	}
}


int dirListing(struct dir *dir, const char *path, const char *ext)
{
	DIR *dp;	
	struct dirent *ep;
	dir->li = NULL;
	int lname, lext;

	dp = opendir(path);
	if(dp == NULL) {
		fprintf(stderr, "ERROR: modloader(): %s not found\n", path);
		return -1;
	}
	if(ext != NULL) {
		lext = strlen(ext);
	}
	dir->nb = 0;
	while((ep = readdir(dp))) {
		if(ext != NULL) {
		lname = strlen(ep->d_name);
// fprintf(stderr, "DEBUG=%s\n", ep->d_name + lname - lext);
			if(!strncmp(ext, ep->d_name + lname - lext, lext)) {
				dir->li = listAdd(dir->li, ep->d_name);
				dir->nb++;
			}
		} else {
			dir->li = listAdd(dir->li, ep->d_name);
			dir->nb++; /* FIXME: put nb++ elsewhere... */
		}
	}
	closedir(dp);
listDump(dir->li);
	return 0;
}
