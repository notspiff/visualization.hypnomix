#ifndef __POOL_H__
#define __POOL_H__


struct pool {
	char *pool;
	unsigned int *delete;
	unsigned int id;
	unsigned int deleteid;
	unsigned int nbitems;
	unsigned int size;
} pool_t;


void poolInit(struct pool *po, unsigned int nbitems, unsigned int size);
void poolDestroy(struct pool *po);
void poolAdd(struct pool *po, void *item);
void poolDelete(struct pool *po, unsigned int id);
void poolFlush(struct pool *po);


#endif
