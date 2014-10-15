#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pool.h"


void poolInit(struct pool *po, unsigned int nbitems, unsigned int size)
{
	po->pool = malloc(nbitems*size);
	po->delete = malloc(nbitems*sizeof(unsigned int));
	bzero(po->delete, sizeof(unsigned int)*nbitems);
	po->nbitems = nbitems;
	po->size = size;
	po->id = 0;
	po->deleteid = 0;
}


void poolDestroy(struct pool *po)
{
	free(po->pool);
	po->nbitems = 0;
	po->id = 0;
}


void poolAdd(struct pool *po, void *item)
{
	if(po->id < po->nbitems) {
		memcpy((char *)po->pool+po->size*po->id, item, po->size);
		po->id++;
	} else {
		fprintf(stderr, "WARNING: pool full (%d/%d)\n", po->id, po->nbitems);
	}
}


void poolDelete(struct pool *po, unsigned int id)
{
	if(id >= po->nbitems) {
		fprintf(stderr, "WARNING: id out of range\n");
	} else {
		po->delete[po->deleteid++] = id;
	}
/* fprintf(stderr, "POOL DELETE: delete[%d]=%d\n", po->deleteid, id); */
}


static void sort(unsigned int *ids, unsigned int len)
{
	int i, j;
	unsigned int tmp;

	for(i = 0; i < len; i++) {
		for(j = i; j < len; j++) {
			if(ids[i] > ids[j]) {
				tmp = ids[i];
				ids[i] = ids[j];
				ids[j] = tmp;
			}		
		}
	}
}


void poolFlush(struct pool *po)
{	
	int i;
	unsigned int id;

	if(po->deleteid > 0) {
		sort(po->delete, po->deleteid); /* FIXME: to use if deletedid*deleteid < nbitems */
		for(i = po->deleteid-1; i >= 0; i--) { 
			id = po->delete[i];
			if(id != po->id-1) {
				memcpy((char *)po->pool+po->size*id, 
					(char*)po->pool+po->size
						*(po->id-1), po->size);
			}
			if(--po->id < 0) {
				po->id = 0;
			}
		}
		po->deleteid = 0;
	}
}
