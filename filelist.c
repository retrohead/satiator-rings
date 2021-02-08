#include "filelist.h"
#include <jo/jo.h>
#include <string.h>
#include <stdlib.h>

file_ent * file_list_create(const char *dir, int *entries, int (*filter)(file_ent *entry)) {
    *entries = 0;
    if (s_opendir(dir))
        return NULL;
    char statbuf[280];
    s_stat_t *st = (s_stat_t*)statbuf;
    int nfiles = 0;
    int listsz = 64;
    file_ent *list = jo_malloc(listsz * sizeof(file_ent));
    if (!list) {
        //menu_error("System error", "Error allocating file list");
        return NULL;
    }
    int len;
    while ((len = s_stat(NULL, st, sizeof(statbuf)-1)) > 0) {
        st->name[len] = 0;
        // UNIX hidden files, except .. when present
        if (st->name[0] == '.' && strcmp(st->name, ".."))
            continue;
        // thanks Windows
        if (!strncmp(st->name, "System Volume Information", strlen("System Volume Information")))
            continue;
        if (st->attrib & AM_DIR) {
            list[nfiles].name = jo_malloc(len + 2);
            strcpy(list[nfiles].name, st->name);
            list[nfiles].name[len] = '/';
            list[nfiles].name[len+1] = 0;
            list[nfiles].isdir = 1;
        } else {
            strcpy(list[nfiles].name, st->name);
            list[nfiles].isdir = 0;
        }

        if (filter && !filter(&list[nfiles]))
            continue;

        if (!list[nfiles].name) {
            jo_free(list);
            return NULL;
        }

        nfiles++;
        if (nfiles == listsz) {
            listsz *= 2;
            //list = jo_realloc(list, listsz * sizeof(file_ent));
			break;
        }
    }
    *entries = nfiles;
    return list;
}

static int compar_list(const void *pa, const void *pb) {
    const file_ent *a = pa, *b = pb;
    if (a->isdir && !b->isdir)
        return -1;
    if (!a->isdir && b->isdir)
        return 1;
    return strcmp(a->name, b->name);
}

void file_list_sort(file_ent *list, int n_entries) {
    qsort(list, n_entries, sizeof(file_ent), compar_list);
}

void file_list_free(file_ent *list, int n_entries) {
    int i;
    for (i=0; i<n_entries; i++)
        jo_free(list[i].name);
    jo_free(list);
}
