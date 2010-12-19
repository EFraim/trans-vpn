#include <stdint.h>
#include <string.h>


void TransferInit()
{
	rxPacketQueue.startPtr = rxPacketQueue.buffer;
	rxPacketQueue.endPtr = rxPacketQueue.buffer;

	txPacketQueue.startPtr = txPacketQueue.buffer;
	txPacketQueue.endPtr = txPacketQueue.buffer;

}



/**
Adds a packet to the queue
return 0 if failed, 1 if succeeded
*/
uint8_t AddPacketToTxQueue(uint16_t length, uint8_t* pPacket)
{

	packetQueue_t* pQueue = &txPacketQueue;
	//RNDIS_Packet_Message_t pRNDISHeader = pPacket;

	uint8_t* tempEndPtr = pQueue->endPtr;

	//does the header fit?
	if (tempEndPtr + sizeof(RNDIS_Packet_Message_t) - 1 >= &(pQueue->buffer[BUFFER_LENGTH]))
	{
		//move the end ptr to the beginning
		tempEndPtr = pQueue->buffer;
	}

	
	//check if there is room in the queue for the packet + header field
	if (tempEndPtr < pQueue->startPtr)
	{
		uint16_t availableSpace = pQueue->startPtr - tempEndPtr;

		if (availableSpace < length)
		{
			return 0;
		}
	}
	else if (tempEndPtr > pQueue->startPtr)
	{
		uint16_t availableSpace = &(pQueue->buffer[BUFFER_LENGTH]) - tempEndPtr + pQueue->startPtr - pQueue->buffer;
		
		if (availableSpace < length)
		{
			return 0;
		}
	}

	//does the packet fit in the buffer?
	if (tempEndPtr + length - 1 >= &(pQueue->buffer[BUFFER_LENGTH]))
	{
		//wrap around

		//write the end of the buffer
		uint16_t writeLen = &(pQueue->buffer[BUFFER_LENGTH]) - tempEndPtr;
		memcpy(tempEndPtr, pPacket, writeLen);

		//go to the beginning and write the rest
		tempEndPtr = pQueue->buffer;
		memcpy(tempEndPtr, pPacket + writeLen, length - writeLen);

		//DEBUG_PRINT_HIGH("tempEndPtr, length, writelen\n");
		//DEBUG_PRINT_HEX_HIGH(tempEndPtr,8);
		//DEBUG_PRINT_HEX_HIGH(length,4);
		//DEBUG_PRINT_HEX_HIGH(writeLen,4);


		//update the end pointer (finalize the copy)
		pQueue->endPtr = tempEndPtr + length - writeLen;

		return 2;
	}
	else
	{
		//no wrap around

		memcpy(tempEndPtr, pPacket, length);
				
		//update the end pointer (finalize the copy)
		pQueue->endPtr = tempEndPtr + length;

		return 1;
	}


}




