
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>


#include "faifa.h"
#include "frame.h"

extern FILE *err_stream;
extern FILE *out_stream;
extern FILE *in_stream;



static void error(char *message)
{
	fprintf(stderr, "%s: %s\n", "faifa", message);
}


void *receive_loop(faifa_t *faifa)
{
	faifa_loop(faifa, (void *)get_frame, faifa);

	return faifa;
}

void get_frame()
{
  
  
    

}




int main(int argc, char **argv)
{
    char *opt_ifname = NULL;
    out_stream = stdout;
    err_stream = stderr;
    in_stream = stdin;
    faifa_t *faifa;
//     u_int16_t mmtype;
//     mmtype = 0xA000;
//     do_frame(faifa, mmtype, NULL, NULL, NULL);
    
    opt_ifname = "eth0";
    faifa = faifa_init();
	if (faifa == NULL) {
		error("can't initialize Faifa library");
		return -1;
	}
    if (faifa_open(faifa, opt_ifname) == -1) {
		error("error opening interface");
		faifa_free(faifa);
		return -1;
    }
  
    pthread_t receive_thread;
    u_int16_t mmtype;
    unsigned int i;
    /* Create a receiving thread */
    if (pthread_create(&receive_thread, NULL, (void *)receive_loop, faifa)) {
      perror("error creating thread");
      abort();
    }
    faifa_printf(stdout, "Started receive thread\n");
    faifa_printf(out_stream, "\nChoose the frame type (Ctrl-C to exit): 0x");
    fscanf(in_stream, "%4x", &i);
    fprintf(out_stream,"%4x", i);     
    mmtype = (u_int16_t)(0xFFFF & i);
    fprintf(out_stream,"%d", mmtype);   
    
    /* Keep asking the user for a mmtype to send */
    do_frame(faifa, mmtype, NULL, NULL, NULL);
    faifa_printf(stdout, "Spedito\n");
    do_frame(faifa, mmtype, NULL, NULL, NULL); 
    /* Rejoin the receiving thread */
    if (pthread_join(receive_thread, NULL)) {
		perror("error joining thread");
		abort();
    }
    faifa_printf(stdout, "Closing receive thread\n");
    
  
}






