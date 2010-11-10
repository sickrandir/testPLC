#include <stdio.h>
#include <stdlib.h>
#include <pcap.h>

#define ETH_ALEN	6
#define ETHER_CRC_LEN	4
#define ETH_FRAME_LEN   1514
#define ETHER_ADDR_LEN	ETH_ALEN
#define ETHER_MAX_LEN   (ETH_FRAME_LEN + ETHER_CRC_LEN)

int main()
{
    pcap_t *pcap;
    char pcap_errbuf[PCAP_ERRBUF_SIZE];
    struct pcap_pkthdr *pktHeader;
    const u_char *pktData;
    int pcap_snaplen = ETHER_MAX_LEN;


	pcap = pcap_open_live("eth0", pcap_snaplen, 1, 100, pcap_errbuf);

    //pcap = pcap_open_offline("/home/kinto/source/faifapy/test.pcap", pcap_errbuf);
    if (pcap == NULL)
    {
        fprintf(stderr, "pcap failed: %s\n", pcap_errbuf);
        exit(EXIT_FAILURE);
    }
	int c = 0;
	//int e = 0;
	while (c < 4)
	{
		while (pcap_next_ex(pcap, &pktHeader, &pktData) == 1)
		{
			printf("Lunghezza pacchetto in byte: %d\n", pktHeader->caplen);
			const char *payload;
			payload = (u_char *)(pktData + 14);
			int i;
			for (i = 0; i<46; i++)
			{
				printf("%x", payload);
				//printf("ciao\n");
				payload = (u_char *)(payload + 1);
			}
			printf("\n");
			c++;
		}
		
	
	}
    pcap_close(pcap);

    return 0;
}




