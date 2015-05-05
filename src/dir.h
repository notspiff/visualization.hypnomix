#ifndef __DIR_H__
#define __DIR_H__


struct list {
	char *filename;
	struct list *prev;
} list_t;


struct dir {
	struct list *li;
	unsigned int nb;
} dir_t;


int dirListing(struct dir *dir, const char *path, const char *ext);
void listDestroy(struct list *list);


#endif
