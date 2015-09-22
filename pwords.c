#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

pthread_t thread[4];
pthread_mutex_t lock;

typedef struct dict {
  char *word;
  int count;
  struct dict *next;
} dict_t;

typedef struct words
{
	dict_t *wordDict;
    char wordbuffer[1024];
	
}word_s;


char *make_word( char *word ) {
  return strcpy( malloc( strlen( word )+1 ), word );
}

void *make_dict(void *wordIn) {
  char * word = (char *) wordIn;

   pthread_mutex_lock (&lock);
  dict_t *nd = (dict_t *) malloc( sizeof(dict_t) );
  nd->word = make_word( word );
  nd->count = 1;
  nd->next = NULL;
  pthread_mutex_unlock (&lock);
  return (void *)nd;
}

void *insert_word( void * listing){

word_s *inGet = malloc(sizeof (*inGet));
inGet = (word_s *) listing;
dict_t *d =(dict_t *) inGet->wordDict;
char * word = (char *)inGet->wordbuffer;

  dict_t *nd;
  dict_t *pd = NULL;	
  dict_t *di = d;	

  while(di && ( strcmp(word, di->word ) >= 0) ) { 
    if( strcmp( word, di->word ) == 0 ) { 
      di->count++;		
      return (void *)d;			
    }
    pd = di;			
    di = di->next;
  }

  void * stat;
  pthread_create(&thread[3], NULL, make_dict, (void *) word);
  pthread_join(thread[3], &stat);

  dict_t *newD = malloc(sizeof(*newD));
  newD = (dict_t *) stat;
  
  newD->next = di;		
  if (pd) {
    pd->next = newD;
    return (void *) d;			
  }
 
  return (void * )newD;
}


void * print_dict( void *dict) {

    dict_t  * d = NULL;
	d = ( dict_t *) dict;
  while (d) {
    printf("[%d] %s\n", d->count, d->word);
    d = d->next;
  }
}

int get_word( char *buf, int n, FILE *infile) {
  int inword = 0;
  int c;  
  while( (c = fgetc(infile)) != EOF ) {
    if (inword && !isalpha(c)) {
      buf[inword] = '\0';	
      return 1;
    } 
    if (isalpha(c)) {
      buf[inword++] = c;
    }
  }
  return 0;			
}





void *words( void * in ) {

  FILE *infile = NULL;
  infile = (FILE * ) in;
  dict_t *wd = NULL;
  char wordbuf[1024] ;

  void * statis;

  word_s *send = malloc(sizeof (*send));
  

int d=0;
  
  while( get_word( wordbuf, 1024, infile ) ) {

  	send->wordDict = wd;

 	strcpy(send->wordbuffer, wordbuf); 
 	pthread_create(&thread[2], NULL, insert_word, (void *) send);
  	pthread_join(thread[2], &statis);
  	wd = statis;

  }

 
  free (send);
  return (void *) wd;
}

int
main( int argc, char *argv[] ) {
  dict_t *d = NULL;
  FILE *infile = stdin;
  if (argc >= 2) {
    infile = fopen (argv[1],"r");
  }
  if( !infile ) {
    printf("Unable to open %s\n",argv[1]);
    exit( EXIT_FAILURE );
  }

 void * status;

 int rc = pthread_create(&thread[0], NULL, words, (void *) infile);
 pthread_join(thread[0], &status);

 int rp = pthread_create(&thread[1], NULL, print_dict, (void *) status);

 
 pthread_join(thread[1], NULL);

  fclose( infile );
}
