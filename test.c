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


struct hpav_frame_structs hpav_frame_structs[] = {
	//~ {
		//~ .mmtype = 0x0014,
		//~ .desc = "Central Coordination Discover List Request",
		//~ .data_struct = init_empty_frame,
	//~ }, {
		//~ .mmtype = 0x0015,
		//~ .desc = "Central Coordination Discover List Confirm",
		//~ .data_struct = cc_discover_list_confirm,
	//~ }, {
		//~ .mmtype = 0x6004,
		//~ .desc = "Encrypted Payload Indicate",
		//~ .data_struct = cm_enc_payload_indicate,
	//~ }, {
		//~ .mmtype = 0x6005,
		//~ .desc = "Encrypted Payload Response",
		//~ .data_struct = cm_enc_payload_response,
	//~ }, {
		//~ .mmtype = 0x6008,
		//~ .desc = "Set Key Request",
		//~ .data_struct = cm_set_key_request,
	//~ }, {
		//~ .mmtype = 0x6009,
		//~ .desc = "Set Key Confirm",
		//~ .data_struct = cm_set_key_confirm,
	//~ }, {
		//~ .mmtype = 0x600C,
		//~ .desc = "Get Key Request",
		//~ .data_struct = cm_get_key_request,
	//~ }, {
		//~ .mmtype = 0x600D,
		//~ .desc = "Get Key Confirm",
		//~ .data_struct = cm_get_key_confirm,
	//~ }, {
		//~ .mmtype = 0x6020,
		//~ .desc = "Get Bridge Infos Request",
		//~ .data_struct = init_empty_frame,
	//~ }, {
		//~ .mmtype = 0x6021,
		//~ .desc = "Get Bridge Infos Confirm",
		//~ .data_struct = cm_brigde_infos_confirm,
	//~ }, {
		//~ .mmtype = 0x6038,
		//~ .desc = "Get Network Infos Request",
		//~ .data_struct = init_empty_frame,
	//~ }, {
		//~ .mmtype = 0x6039,
		//~ .desc = "Get Network Infos Confirm",
		//~ .data_struct = cm_get_network_infos_confirm,
	//~ }, {
		//~ .mmtype = 0x6048,
		//~ .desc = "Get Network Stats Request",
		//~ .data_struct = init_empty_frame,
	//~ }, {
		//~ .mmtype = 0x6049,
		//~ .desc = "Get Network Stats Confirm",
		//~ .data_struct = cm_get_network_stats_confirm,
	//~ }, {
		//~ .mmtype = 0xA000,
		//~ .desc = "Get Device/SW Version Request",
		//~ .data_struct = init_empty_frame,
	//~ }, {
		//~ .mmtype = 0xA001,
		//~ .desc = "Get Device/SW Version Confirm",
		//~ .data_struct = get_device_sw_version_confirm,
	//~ }, {
		//~ .mmtype = 0xA004,
		//~ .desc = "Write MAC Memory Request",
		//~ .data_struct = write_mac_memory_request,
		//~ .data_struct = write_mac_memory_request,
	//~ }, {
		//~ .mmtype = 0xA005,
		//~ .desc = "Write MAC Memory Confirm",
		//~ .data_struct = write_mac_memory_confirm,
	//~ }, {
		//~ .mmtype = 0xA008,
		//~ .desc = "Read MAC Memory Request",
		//~ .data_struct = read_mac_memory_request,
		//~ .data_struct = read_mac_memory_request,
	//~ }, {
		//~ .mmtype = 0xA009,
		//~ .desc = "Read MAC Memory Confirm",
		//~ .data_struct = read_mac_memory_confirm,
	//~ }, {
		//~ .mmtype = 0xA00C,
		//~ .desc = "Start MAC Request",
		//~ .data_struct = start_mac_request,
		//~ .data_struct = start_mac_request,
	//~ }, {
		//~ .mmtype = 0xA00D,
		//~ .desc = "Start MAC Confirm",
		//~ .data_struct = start_mac_confirm,
	//~ }, {
		//~ .mmtype = 0xA010,
		//~ .desc = "Get NVM parameters Request",
	//~ }, {
		//~ .mmtype = 0xA011,
		//~ .desc = "Get NVM parameters Confirm",
		//~ .data_struct = hpav_dump_nvram_params_confirm,
	//~ }, {
		//~ .mmtype = 0xA01C,
		//~ .desc = "Reset Device Request",
		//~ .data_struct = init_empty_frame,
	//~ }, {
		//~ .mmtype = 0xA01D,
		//~ .desc = "Reset Device Confirm",
		//~ .data_struct = reset_device_confirm,
	//~ }, {
		//~ .mmtype = 0xA020,
		//~ .desc = "Write Module Data Request",
		//~ .data_struct = write_mod_data_request,
	//~ }, {
		//~ .mmtype = 0xA021,
		//~ .desc = "Write Module Data Confirm",
		//~ .data_struct = write_mod_data_confirm,
	//~ }, {
		//~ .mmtype = 0xA022,
		//~ .desc = "Write Module Data Indicate",
	//~ }, {
		//~ .mmtype = 0xA024,
		//~ .desc = "Read Module Data Request",
	//~ }, {
		//~ .mmtype = 0xA025,
		//~ .desc = "Read Module Data Confirm",
		//~ .data_struct = start_mac_confirm,
	//~ }, {
		//~ .mmtype = 0xA028,
		//~ .desc = "Write Module Data to NVM Request",
	//~ }, {
		//~ .mmtype = 0xA029,
		//~ .desc = "Write Module Data to NVM Confirm",
		//~ .data_struct = start_mac_confirm,
	//~ }, {
		//~ .mmtype = 0xA02C,
		//~ .desc = "Get Watchdog Report Request",
		//~ .data_struct = get_watchdog_report_request,
	//~ }, {
		//~ .mmtype = 0xA02E,
		//~ .desc = "Get Watchdog Report Indicate",
		//~ .data_struct = get_watchdog_report_indicate,
	//~ }, {
		//~ .mmtype = 0xA030,
		//~ .desc = "Get Link Statistics Request",
		//~ .data_struct = link_statistics_request,
	//~ }, {
		//~ .mmtype = 0xA031,
		//~ .desc = "Get Link Statistics Confirm",
		//~ .data_struct = link_statistics_confirm,
	//~ }, {
		//~ .mmtype = 0xA034,
		//~ .desc = "Sniffer Mode Request",
		//~ .data_struct = sniffer_request,
		//~ .data_struct = sniffer_request,
	//~ }, {
		//~ .mmtype = 0xA035,
		//~ .desc = "Sniffer Mode Confirm",
		//~ .data_struct = sniffer_confirm,
	//~ }, {
		//~ .mmtype = 0xA036,
		//~ .desc = "Sniffer Mode Indicate",
		//~ .data_struct = sniffer_indicate,
	//~ }, {
		//~ .mmtype = 0xA038,
		//~ .desc = "Network Info Request (Vendor-Specific)",
		//~ .data_struct = init_empty_frame,
	//~ }, {
		//~ .mmtype = 0xA039,
		//~ .desc = "Network Info Confirm (Vendor-Specific)",
		//~ .data_struct = network_info_confirm,
	//~ }, {
		//~ .mmtype = 0xA040,
		//~ .desc = "Check Points Request",
		//~ .data_struct = check_points_request,
	//~ }, {
		//~ .mmtype = 0xA042,
		//~ .desc = "Check Points Indicate",
		//~ .data_struct = check_points_indicate,
	//~ }, {
		//~ .mmtype = 0xA048,
		//~ .desc = "Loopback Request",
		//~ .data_struct = loopback_request,
	//~ }, {
		//~ .mmtype = 0xA049,
		//~ .desc = "Loopback Confirm",
		//~ .data_struct = loopback_confirm,
	//~ }, {
		//~ .mmtype = 0xA04C,
		//~ .desc = "Loopback Status Request",
		//~ .data_struct = init_empty_frame,
	//~ }, {
		//~ .mmtype = 0xA04D,
		//~ .desc = "Loopback Status Confirm",
		//~ .data_struct = hpav_dump_loopback_status_confirm,
	//~ }, {
		//~ .mmtype = 0xA050,
		//~ .desc = "Set Encryption Key Request",
		//~ .data_struct = hpav_init_set_enc_key_request,
	//~ }, {
		//~ .mmtype = 0xA051,
		//~ .desc = "Set Encryption Key Confirm",
		//~ .data_struct = hpav_dump_set_enc_key_confirm,
	//~ }, {
		//~ .mmtype = 0xA054,
		//~ .desc = "Get Manufacturing String Request",
		//~ .data_struct = init_empty_frame,
	//~ }, {
		//~ .mmtype = 0xA055,
		//~ .desc = "Get Manufacturing String Confirm",
		//~ .data_struct = get_manuf_string_confirm,
	//~ }, {
		//~ .mmtype = 0xA058,
		//~ .desc = "Read Configuration Block Request",
		//~ .data_struct = init_empty_frame,
	//~ }, {
		//~ .mmtype = 0xA059,
		//~ .desc = "Read Configuration Block Confirm",
		//~ .data_struct = read_config_block_confirm,
	//~ }, {
		//~ .mmtype = 0xA05C,
		//~ .desc = "Set SDRAM Configuration Request",
	//~ }, {
		//~ .mmtype = 0xA05D,
		//~ .desc = "Set SDRAM Configuration Confirm",
		//~ .data_struct = set_sdram_config_confirm,
	//~ }, {
		//~ .mmtype = 0xA062,
		//~ .desc = "Embedded Host Action Required Indicate",
	//~ }, {
		//~ .mmtype = 0xA063,
		//~ .desc = "Embedded Host Action Required Response",
	//~ }, {
		//~ .mmtype = 0xA068,
		//~ .desc = "Get Device Attributes Request",
		//~ .data_struct = get_devices_attrs_request,
	//~ }, {
		//~ .mmtype = 0xA069,
		//~ .desc = "Get Device Attributes Confirm",
		//~ .data_struct = get_devices_attrs_confirm,
	//~ }, {
		//~ .mmtype = 0xA06C,
		//~ .desc = "Get Ethernet PHY Settings Request",
		//~ .data_struct = get_enet_phy_settings_request,
	//~ }, {
		//~ .mmtype = 0xA06D,
		//~ .desc = "Get Ethernet PHY Settings Confirm",
		//~ .data_struct = get_enet_phy_settings_confirm,
	//~ }, {
		//~ .mmtype = 0xA070,
		//~ .desc = "Get Tone Map Caracteristics Request",
		//~ .data_struct = get_tone_map_charac_request,
	//~ }, {
		//~ .mmtype = 0xA071,
		//~ .desc = "Get Tone Map Characteristics Confirm",
		//~ .data_struct = get_tone_map_charac_confirm,
	//~ }
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

//~ static int hpav_mmtype2index(u_int16_t mmtype)
//~ {
	//~ unsigned int i;
//~ 
	//~ for (i = 0; i < ARRAY_SIZE(hpav_frame_structs); i++) {
		//~ if (hpav_frame_structs[i].mmtype == mmtype)
			//~ return i;
	//~ }
//~ 
	//~ return -1;
//~ }

static int hpav_cast_frame(u_int8_t *frame_ptr, int frame_len, struct ether_header *hdr)
{
	struct hpav_frame *frame = (struct hpav_frame *)frame_ptr;
	//~ int i;

	//~ if( (i = hpav_mmtype2index(STORE16_LE(frame->header.mmtype))) < 0 ) {
		//~ faifa_printf(out_stream, "\nUnknow MM type : %04X\n", frame->header.mmtype);
		//~ return 0;
	//~ }

	if( (frame->header.mmtype & HPAV_MM_CATEGORY_MASK) == HPAV_MM_VENDOR_SPEC ) {
		frame_ptr = frame->payload.vendor.data;
		frame_len -= sizeof(frame->payload.vendor);
	} else {
		frame_ptr = frame->payload.public.data;
		frame_len -= sizeof(frame->payload.public);
	}
	
	struct cm_get_network_infos_confirm *mm = (struct cm_get_network_infos_confirm *)frame_ptr;
	int avail = frame_len;
	avail -= sizeof(*mm);
	frame_ptr += (frame_len - avail);
	
	//~ /* Call the frame specific cast */
	//~ if (hpav_frame_structs[i].cast_frame != NULL)
		//~ struct hpav_frame_structs[i].data_struct *mm = (struct hpav_frame_structs[i].data_struct *)frame_ptr;
		//~ frame_ptr += hpav_frame_ops[i].dump_frame(frame_ptr, frame_len, hdr);
//~ 
	return (frame_ptr - (u_int8_t *)frame);
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














