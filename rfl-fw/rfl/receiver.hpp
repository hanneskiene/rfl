/*
 * receiver.hpp
 *
 *  Created on: Jun 17, 2026
 *      Author: hannes
 */

#ifndef RECEIVER_HPP_
#define RECEIVER_HPP_

#include "consts.hpp"
#include "sx128x_stm.hpp"

class Receiver
{
public:
    Receiver(SX1281& radio)
        : sx(radio)
    {
    }

    bool tick()
    {
        if(state == SEARCH)
            return search();

        return track();
    }

    bool receiving()
    {
    	return state == TRACK;
    }

    uint16_t channels[8]{};
    int lastRSSI = 0;
    float avg_delta  = 0.f;
private:

    enum
    {
        SEARCH,
        TRACK
    } state = SEARCH;

    bool search()
    {
        RcPacket p;

        sx.setFrequency(HopTable[searchHop]);

        int len = sx.receive(
            reinterpret_cast<uint8_t*>(&p),
            SearchListenMs);

        searchHop = (searchHop + 1) % HopCount;

        if(len != sizeof(RcPacket))
            return false;

        syncOffset = HAL_GetTick() - p.txTimeMs;
        lastPacketTime = HAL_GetTick();

        memcpy(channels, p.channels, sizeof(channels));
        lastRSSI = sx.lastRSSIDbm;

        state = TRACK;
        printf("Tracking\n");
        return true;
    }

    bool track()
    {
        uint32_t now = HAL_GetTick();

        if(now - lastPacketTime > LinkTimeoutMs)
        {
            state = SEARCH;
            avg_delta = 0.f;
            return false;
        }

        uint32_t txTime = now - syncOffset;
        uint32_t slot = txTime / SlotTimeMs;

        // slot changed → reset RX state for new slot
        if(slot != currentSlot)
        {
            currentSlot = slot;
            slotRxDone = false;

            sx.setFrequency(HopTable[currentSlot % HopCount]);
            sx.sky_toggle_ant();
        }

        // if we already successfully received a packet in this slot → do nothing
        if(slotRxDone)
            return true;

        uint32_t slotEnd =
            syncOffset + (currentSlot + 1) * SlotTimeMs;

        uint32_t remaining =
            (slotEnd > now) ? (slotEnd - now) : 1;

        RcPacket p;

        int len = sx.receive(
            reinterpret_cast<uint8_t*>(&p),
            remaining);

        if(len != sizeof(RcPacket))
            return false;

        // successful reception → lock out further RX in this slot
        slotRxDone = true;

        syncOffset = HAL_GetTick() - p.txTimeMs;
        avg_delta = avg_delta * 0.9f + 0.1f * (HAL_GetTick() - lastPacketTime);
        lastPacketTime = HAL_GetTick();

        memcpy(channels, p.channels, sizeof(channels));
        lastRSSI = sx.lastRSSIDbm;

        return true;
    }

    SX1281& sx;

    uint32_t syncOffset = 0;
    uint32_t currentSlot = 0;
    uint32_t lastPacketTime = 0;
    uint8_t searchHop = 0;
    bool slotRxDone = false;
};



#endif /* RECEIVER_HPP_ */
