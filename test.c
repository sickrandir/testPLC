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

struct hpav_frame_structs{
	u_int16_t	mmtype;
	char 		*desc;
	void		*data_struct;	 	
	
};

static void error(char *message)
{
	fprintf(stderr, "%s: %s\n", "faifa", message);
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
}

void hpav_cast_frame(u_int8_t *frame_ptr, int frame_len, struct ether_header *hdr)
{
	struct hpav_frame *frame = (struct hpav_frame *)frame_ptr;
	if( (frame->header.mmtype & HPAV_MM_CATEGORY_MASK) == HPAV_MM_VENDOR_SPEC ) {
		frame_ptr = frame->payload.vendor.data;
		frame_len -= sizeof(frame->payload.vendor);
	} else {
		frame_ptr = frame->payload.public.data;
		frame_len -= sizeof(frame->payload.public);
	}
	switch(frame->header.mmtype) 
	{
		case 0xA039:
		{
			struct network_info_confirm *mm = (struct network_info_confirm *)frame_ptr;
			break;
		}
		case 0xA031:
			
			break;
		case 0xA071:
			
			break;
		
	}
	
	
	
}

int main(int argc, char **argv)
{
    char *opt_ifname = NULL;
    out_stream = stdout;
    err_stream = stderr;
    in_stream = stdin;
    faifa_t *faifa;
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
    u_int16_t mmtype;
    mmtype = 0xA038;
    do_frame(faifa, mmtype, NULL, NULL, NULL);
    u_char *buf;
    int l;
    u_int16_t *eth_type;
    do {
		l = faifa_recv(faifa, buf, l);
		struct ether_header *eth_header = (struct ether_header *)buf;
		eth_type = &(eth_header->ether_type);
        u_int8_t *frame_ptr = (u_int8_t *)buf, *payload_ptr;
		int frame_len = l, payload_len;
		payload_ptr = frame_ptr + sizeof(*eth_header);
		payload_len = frame_len - sizeof(*eth_header);
		//if((*eth_type == ntohs(ETHERTYPE_HOMEPLUG)) || (*eth_type == ntohs(ETHERTYPE_HOMEPLUG_AV))) {
		if((*eth_type == ntohs(ETHERTYPE_HOMEPLUG_AV))) {
			hpav_cast_frame(payload_ptr, payload_len, eth_header);
			print_blob(frame_ptr, frame_len);				
		}	
	}
	while (!(*eth_type == ntohs(ETHERTYPE_HOMEPLUG)) && !(*eth_type == ntohs(ETHERTYPE_HOMEPLUG_AV)));
    faifa_close(faifa);
	faifa_free(faifa);
	return 0;
}














