#ifndef PWORDS_H
#define PWORDS_H

#include <pthread.h>
#include "queue.h"

#define MAXWORD 1024
#define DEBUG false

typedef struct dict {
  char *word;
  int count;
  struct dict *next;
} dict_t;

typedef struct SharedObj{
  FILE *infile;
  dict_t *dict;
  pthread_mutex_t filelock;
  pthread_cond_t signal_cond;
} so_t;

typedef struct ThreadManage{
  so_t *so;
  int num;
  int exec_count;
}tm_t;

void *consumer_thread(void *);

char *make_word( char * );

dict_t *make_dict(char *);

void insert_word( dict_t **, char * );

void print_dict(dict_t *);

int get_word(FILE *,q_t *);

char *get_line(FILE *);

void thread_clean(so_t*);
#endif
