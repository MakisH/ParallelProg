#include <pthread.h>

#include "histogram.h"

typedef struct {
  pthread_mutex_t mutex;
  pthread_cond_t  cond_work_available;
  int             value_work_available;
  pthread_cond_t  cond_consumer_ready;
  int             value_consumer_ready;
  char            *buffer;
  unsigned int    chunk_size;
} thread_args;

thread_args args = { PTHREAD_MUTEX_INITIALIZER,
              PTHREAD_COND_INITIALIZER,
              0 
              PTHREAD_COND_INITIALIZER,
              0,
              0,
              0 };

int hibernation = 1;



void * consumer_thread( void *arg ) {

  thread_args * input = (thread_args*) args;
  unsigned int * local_histogram = (unsigned int*) calloc(NALPHABET, sizeof(unsigned int*));
  char * local_buffer = NULL;
  int size = 0;

  while ( 1 ) {  
    
    sleep( hibernation ); // ????
    pthread_mutex_lock( input->mutex );
      arg->value_consumer_ready = 1;
      pthread_cond_signal( input->mutex );
      while ( input->value_work_available == 0 );
        if ( pthread_cond_timedwait( input->cond_work_available, input->mutex, &timeout ) == ETIMEDOUT ) {
          printf("Work available condition wait timedout");
          break;
        }
      }
      if ( input->value_work_available == 1 ) {
        local_buffer = input->buffer;
        size = input->size;
      }
    pthread_mutex_unlock( input->mutex );  

    // build histogram
	  for (int i=0; i<size; i++) {
		  if (local_buffer[i] >= 'a' && local_buffer[i] <= 'z')
			  local_histogram[buffer[i]-'a']++;
		  else if(local_buffer[i] >= 'A' && local_buffer[i] <= 'Z')
			  local_histogram[buffer[i]-'A']++;
	  }


  }

  return local_histogram;

}



void * producer_thread( void *arg ) {
  
  thread_args * input = (thread_args*) args;

  struct timespec timeout;
  timeout.tv_sec = time() + 2;

  int size = 0;
  char *buffer = malloc(CHUNKSIZE);

  while ((size = get_chunk(buffer)) > 0) {

    pthread_mutex_lock( &input->mutex );
    
      while ( input->value_consumer_ready == 0 ) {
        if ( pthread_cond_timedwait( input->cond_consumer_ready, input->mutex, &timeout ) == ETIMEDOUT ) {
          printf( "Consumer ready condition wait timed out \n" );
          break;
        }
      }

      if ( input->value_consumer_ready == 1 && input->value_work_available == 1 ) {
        input->buffer = buffer;
        input->size = size;
        pthread_cond_signal( &input->cond_work_available );
      }

    pthread_mutex_unlock( &input->mutex );

  }

  free(buffer);

  return NULL;

}



void get_histogram(unsigned int* histogram,
                   unsigned int num_threads) {
      
  pthread_t thread;

  pthread_create( &thread, NULL, &consumer_thread, NULL);
  

}
