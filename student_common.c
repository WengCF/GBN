#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "project2.h"

/*
 * This method calculate bit order dependent check sum of packet
 * @param packet The target packet to calculate checksum
 */
int getPktChecksum(struct pkt packet) {

	// Fletcher's checksum
	uint32_t sum1 = 0;
	uint32_t sum2 = 0;
	packet.checksum = 0;
	uint16_t *data = (uint16_t *)&packet;
	
	int index;

	for( index = 0; index < (sizeof(struct pkt)/ 2); index++) {
		sum1 = (sum1 + data[index]) % 0xFFFF;
		sum2 = (sum2 + sum1) % 0xFFFF;
	}

	return ((sum2 << 16) | sum1);
}

/*
 * This method detech whether the packet is corrupted
 * packet The packte to be checked
 * return 1 is not corrupted
 */
int not_corrupted(struct pkt packet) {

	uint32_t sum1 = 0;
	uint32_t sum2 = (uint32_t)(packet.checksum);
	
	sum1 = getPktChecksum(packet);

	// compare checksum get from packet
	return sum1 == sum2;
}

/*
 * Generate a packet with sequence number, ack number and message
 * @param packet The packet to be revised
 * @param seqnum The sequence number of the packet
 * @param payload The message sending
 */
struct pkt prepare_pkt(int seqnum, int acknum, char payload[]) {
	struct pkt packet;

	// Assign sequence number to packet
	packet.seqnum = seqnum;
	// Assign acknum number to packet
	packet.acknum = acknum;

	if(payload != NULL) {
		strncpy(packet.payload, payload, MESSAGE_LENGTH);
	} else {
		payload = NULL;
	}
	// Assign check sum to packet
	packet.checksum = getPktChecksum(packet);

	return packet;
}
