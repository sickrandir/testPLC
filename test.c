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
#include "endian.h"
#include "string.h"

extern FILE *err_stream;
extern FILE *out_stream;
extern FILE *in_stream;

faifa_t *faifa;
u_int8_t hpav_intellon_oui[3] = { 0x00, 0xB0, 0x52};
u_int8_t hpav_intellon_macaddr[ETHER_ADDR_LEN] = { 0x00, 0xB0, 0x52, 0x00, 0x00, 0x01 };



struct station_data {
	struct sta_info sta_info;
	float raw_rate; //raw rate obtained with modulation data from A071 frame	
};

struct network_data { 
	int station_count;
	u_int8_t network_id[7];
	u_int8_t sta_mac[6];
	u_int8_t sta_role;
	u_int8_t cco_mac[6];
	float tx_mpdu_collision_perc;
    float tx_mpdu_failure_perc;
    float tx_pb_failure_perc;
    float rx_mpdu_failure_perc;
    float rx_pb_failure_perc;
    float rx_tbe_failure_perc;
	struct station_data *sta_data[];	
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

int get_bits_per_carrier(short unsigned int modulation)
{
	switch(modulation) {
	case NO:
		return 0;
		break;
	case BPSK:
		return 1;
		break;
	case QPSK:
		return 2;
		break;
	case QAM_8:
		return 3;
		break;
	case QAM_16:
		return 4;
		break;
	case QAM_64:
		return 6;
		break;
	case QAM_256:
		return 8;
		break;
	case QAM_1024:
		return 10;
		break;
	default:
		return 0;
		break;
	}	
}

int init_frame(u_int8_t *frame_buf, int frame_len, u_int16_t mmtype)
{
	struct hpav_frame *frame;
	u_int8_t *frame_ptr = frame_buf;
	bzero(frame_buf, frame_len);
	u_int8_t *da;
	da = hpav_intellon_macaddr;
	u_int8_t *sa = NULL;
	/* Set the ethernet frame header */
	int n;
	n = ether_init_header(frame_ptr, frame_len, da, sa, ETHERTYPE_HOMEPLUG_AV);
	frame_ptr += n;
	frame = (struct hpav_frame *)frame_ptr;
	n = sizeof(frame->header);
	frame->header.mmtype = STORE16_LE(mmtype);
	if( (mmtype & HPAV_MM_CATEGORY_MASK) == HPAV_MM_VENDOR_SPEC ) {
		frame->header.mmver = HPAV_VERSION_1_0;
		memcpy(frame->payload.vendor.oui, hpav_intellon_oui, 3);
		n += sizeof(frame->payload.vendor);
	} else {		
		frame->header.mmver = HPAV_VERSION_1_1;
		n += sizeof(frame->payload.public);
	}
	frame_ptr += n;
	frame_len = frame_ptr - (u_int8_t *)frame_buf;
	return frame_len;
}

int send_A038(u_int8_t *frame_buf, int frame_len, int cursor)
{
	u_int8_t *frame_ptr = frame_buf;
	frame_ptr += cursor;
	frame_len = frame_ptr - (u_int8_t *)frame_buf;
		if (frame_len < ETH_ZLEN)
		frame_len = ETH_ZLEN;
	frame_len = faifa_send(faifa, frame_buf, frame_len);
	if (frame_len == -1)
		faifa_printf(err_stream, "Init: error sending frame (%s)\n", faifa_error(faifa)); 
	return frame_len;	
}

int send_A070(u_int8_t *frame_buf, int frame_len, int cursor, u_int8_t macaddr[], u_int8_t tsslot)
{
	u_int8_t *frame_ptr = frame_buf;
	int i,n;
	frame_ptr += cursor;
	struct get_tone_map_charac_request *mm = (struct get_tone_map_charac_request *)frame_ptr;
	for (i = 0; i < 6; i++)
		mm->macaddr[i] = macaddr[i];
	n = sizeof(*mm);
	frame_len = frame_ptr - (u_int8_t *)frame_buf;
		if (frame_len < ETH_ZLEN)
		frame_len = ETH_ZLEN;
	frame_len = faifa_send(faifa, frame_buf, frame_len);
	if (frame_len == -1)
		faifa_printf(err_stream, "Init: error sending frame (%s)\n", faifa_error(faifa)); 
	return frame_len;	
}

void hpav_cast_frame(u_int8_t *frame_ptr, int frame_len, struct ether_header *hdr, struct network_data ***nd)
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
			int n_stas = mm->num_stas;
			**nd= (struct network_data *) malloc(sizeof(struct network_data)+sizeof(struct station_data[n_stas]));
			(**nd)->station_count = n_stas;
			memcpy(&(**nd)->network_id, mm->nid, 7 );	
			memcpy(&(**nd)->network_id, mm->nid, 7 );	
			memcpy(&(**nd)->sta_mac, hdr->ether_shost, ETHER_ADDR_LEN );
			(**nd)->sta_role = mm->sta_role;
			memcpy(&(**nd)->cco_mac, mm->cco_macaddr, ETHER_ADDR_LEN );
			int i;
			struct station_data sta_d[n_stas];
			for (i=0; i < n_stas; i++)
			{
				memcpy(&sta_d[i].sta_info, &mm->stas[i], sizeof(struct sta_info));
				(**nd)->sta_data[i] = &sta_d[i];
			}
			int g = 0;			
			break;
		}
		case 0xA031:
			
			break;
		case 0xA071:
		{
			faifa_printf(out_stream, "ricevo A071\n");
			struct get_tone_map_charac_confirm *mm = (struct get_tone_map_charac_confirm *)frame_ptr;
			if (mm->mstatus != 0x00) {
				faifa_printf(out_stream, "A070-A071 error\n");
				break;
			}
			unsigned int total_bits = 0;
			int i;
			for (i = 0; i < MAX_CARRIERS; i++) {
				faifa_printf(out_stream, "Total bits: %d\n", total_bits);
				total_bits += get_bits_per_carrier(mm->carriers[i].mod_carrier_lo);
				total_bits += get_bits_per_carrier(mm->carriers[i].mod_carrier_hi);
			}
			faifa_printf(out_stream, "Total bits: %d\n", total_bits);
			float bits_per_second = (float) total_bits / 0.0000465;
			faifa_printf(out_stream, "Modulation rate: %4.2f bit/s\n", bits_per_second);
			break;
		}		
	}	
}


void receive_frame(struct network_data **nd)
{
	u_int8_t *buf;
    int l = 1518;
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
			hpav_cast_frame(payload_ptr, payload_len, eth_header, &nd);
			print_blob(frame_ptr, frame_len);				
		}	
	}
	while (!(*eth_type == ntohs(ETHERTYPE_HOMEPLUG)) && !(*eth_type == ntohs(ETHERTYPE_HOMEPLUG_AV)));
}



int main(int argc, char **argv)
{
    char *opt_ifname = NULL;
    out_stream = stdout;
    err_stream = stderr;
    in_stream = stdin;
    
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
    faifa_printf(out_stream, "in main\n");
    sleep(2);
    
    struct network_data *nd;
            
    int c,s;
    u_int8_t mac[6];
    mac[0]=0x00;mac[1]=0x19;mac[2]=0xCB;mac[3]=0xFD;mac[4]=0x68;mac[5]=0x1D;
    u_int8_t frame_buf[1518];
    int frame_len = sizeof(frame_buf);
    c = init_frame(frame_buf, frame_len, 0xA038);
    s = send_A038(frame_buf, frame_len, c);
	receive_frame(&nd);
	int i;
	for (i=0; i<nd->station_count; i++)
	{
		struct station_data *sd = nd->sta_data[i];		
		int z;
		for (z=0; z < 6; z++)
			mac[z] = (u_int8_t) (sd->sta_info.sta_macaddr)+z;
		c = init_frame(frame_buf, frame_len, 0xA070);
		s = send_A070(frame_buf, frame_len, c, mac, 0);
		receive_frame(&nd);	
	} 
	faifa_printf(out_stream, "Ricevuto. Chiudo\n");
    faifa_close(faifa);
	faifa_free(faifa);
	return 0;
}














