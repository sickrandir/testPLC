
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <netinet/in.h>

#include "faifa.h"
#include "frame.h"
#include <net/ethernet.h>
#include "homeplug.h"
#include "homeplug_av.h"


extern FILE *err_stream;
extern FILE *out_stream;
extern FILE *in_stream;



static void error(char *message)
{
	fprintf(stderr, "%s: %s\n", "faifa", message);
}


//~ void *receive_loop(faifa_t *faifa)
//~ {
	//~ faifa_loop(faifa, (void *)get_frame, faifa);
//~ 
	//~ return faifa;
//~ }
#define HEX_BLOB_BYTES_PER_ROW  16

void print_blob( u_char *buf, int len)
{
	u_int32_t i, d, m = len % HEX_BLOB_BYTES_PER_ROW;

	faifa_printf(out_stream, "Binary Data, %lu bytes", (unsigned long int)len);
	for (i = 0; i < len; i += HEX_BLOB_BYTES_PER_ROW) {
		d = (len - i) / HEX_BLOB_BYTES_PER_ROW;
		faifa_printf(out_stream, "\n%08lu: ", (unsigned long int)i); 
		dump_hex((u_int8_t *)buf + i, (d > 0) ? HEX_BLOB_BYTES_PER_ROW : m, " ");
	}
	faifa_printf(out_stream, "\n"); 

	return len;
}

int dump_hex(void *buf, int len, char *sep)
{
	int avail = len;
	u_int8_t *p = buf;

	while (avail > 0) {
		faifa_printf(out_stream, "%02hX%s", *p, (avail > 1) ? sep : "");
		p++;
		avail--;
	}

	return len;
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
    //~ if (pthread_create(&receive_thread, NULL, (void *)receive_loop, faifa)) {
      //~ perror("error creating thread");
      //~ abort();
    //~ }
    //~ faifa_printf(stdout, "Started receive thread\n");
    faifa_printf(out_stream, "\nChoose the frame type (Ctrl-C to exit): 0x");
    fscanf(in_stream, "%4x", &i);
     
    mmtype = (u_int16_t)(0xFFFF & i);
    
    
    do_frame(faifa, mmtype, NULL, NULL, NULL);
    faifa_printf(stdout, "Spedito\n");
    
    const u_char *buf;
    int l = 90;
    u_int16_t *eth_type;
    do {
		
		l = faifa_recv(faifa, &buf, l);
		struct ether_header *eth_header = (struct ether_header *)buf;
		eth_type = &(eth_header->ether_type);
		u_int8_t *frame_ptr = (u_int8_t *)buf, *payload_ptr;
		int frame_len = l, payload_len;

		payload_ptr = frame_ptr + sizeof(*eth_header);
		payload_len = frame_len - sizeof(*eth_header);
		
		
		/* Check ethertype */
		if (!(*eth_type == ntohs(ETHERTYPE_HOMEPLUG)) && !(*eth_type == ntohs(ETHERTYPE_HOMEPLUG_AV)))
			return;
	}
	while (!(*eth_type == ntohs(ETHERTYPE_HOMEPLUG)) && !(*eth_type == ntohs(ETHERTYPE_HOMEPLUG_AV)));
    
    faifa_printf(stdout, "Binary data: %d\n", l);
    

    
    
    
    //faifa_close(faifa);
	//faifa_free(faifa);

	return 0;
    
    
    //dump_hex_blob(faifa, buf, l);
    
    //~ /* Rejoin the receiving thread */
    //~ if (pthread_join(receive_thread, NULL)) {
		//~ perror("error joining thread");
		//~ abort();
    //~ }
    //~ faifa_printf(stdout, "Closing receive thread\n");
    //~ 
    
  
}






