#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "project2.h"
 
/* ***************************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for Project 2, unidirectional or bidirectional
   data transfer protocols from A to B and B to A.
   Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets may be delivered out of order.

   Compile as gcc -g project2.c student2.c -o p2
**********************************************************************/



/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/
/* 
 * The routines you will write are detailed below. As noted above, 
 * such procedures in real-life would be part of the operating system, 
 * and would be called by other procedures in the operating system.  
 * All these routines are in layer 4.
 */
int base; // sequence number of base packet
int nextseqnum; // sequence number of next packet

int snd_queue_end; // packets buffer end position
struct pkt snd_packet_queue[MAX_QUEUE_LENGTH]; // packets buffer

/* 
 * A_output(message), where message is a structure of type msg, containing 
 * data to be sent to the B-side. This routine will be called whenever the 
 * upper layer at the sending side (A) has a message to send. It is the job 
 * of your protocol to insure that the data in such a message is delivered 
 * in-order, and correctly, to the receiving side upper layer.
 */
void A_output(struct msg message) {
	if(snd_queue_end < MAX_QUEUE_LENGTH ) {
		// Intialize packet info
		strncpy(snd_packet_queue[snd_queue_end].payload, message.data, MESSAGE_LENGTH);

		// Send pkt is not currently sending packet
		if(nextseqnum < (base + WINDOWS_SIZE)) {
			// Send packet to receiver
			A_prepare_send_pkt(TIMER_RTT);
		}

		// Increment num of queuing pkt
		snd_queue_end++;
	}
}

/* 
 * A_input(packet), where packet is a structure of type pkt. This routine 
 * will be called whenever a packet sent from the B-side (i.e., as a result 
 * of a tolayer3() being done by a B-side procedure) arrives at the A-side. 
 * packet is the (possibly corrupted) packet sent from the B-side.
 */
void A_input(struct pkt packet) {
    
    //  Move window to packet.acknum + 1 after received ack
	if(not_corrupted(packet) && (packet.acknum >= base)) {
		base = packet.acknum + 1;
		if(base == nextseqnum) {
			stopTimer(AEntity);
		} else {
			// Start timer
			startTimer(AEntity, TIMER_RTT);
		}
	}
}

/*
 * A_timerinterrupt()  This routine will be called when A's timer expires 
 * (thus generating a timer interrupt). You'll probably want to use this 
 * routine to control the retransmission of packets. See starttimer() 
 * and stoptimer() in the writeup for how the timer is started and stopped.
 */
void A_timerinterrupt() {
	// Start timer
	startTimer(AEntity, TIMER_RTT);

    // Retransmit all packets not acknowledged
	int i;
	for(i = base; i < nextseqnum; i++) {
		// Revise pakect information
		snd_packet_queue[i] = prepare_pkt(i, 0, snd_packet_queue[i].payload);
		// Udt send message
		tolayer3(AEntity, snd_packet_queue[i]);
	}
}  

/* The following routine will be called once (only) before any other    */
/* entity A routines are called. You can use it to do any initialization */
void A_init() {
    
    // Set base and next sequence number both to zero;
	int base = 0;
	int nextseqnum = 0;
	int snd_queue_end = 0;
}

/*
 * Prepare a packet and send it to layer 3
 * time_interval The time interval for retransmission
 */
void A_prepare_send_pkt(int time_interval) {

	// Revise pakect information
	snd_packet_queue[nextseqnum] = prepare_pkt(nextseqnum, 0, snd_packet_queue[nextseqnum].payload);
	// Udt send message
	tolayer3(AEntity, snd_packet_queue[nextseqnum]);

	if(base == nextseqnum) {
		// Start timer
		startTimer(AEntity, time_interval);
	}
    
    // Move to next sequence number to next postion
	nextseqnum++;
}
