/*
 * sender.hpp
 *
 *  Created on: Jun 17, 2026
 *      Author: hannes
 */

#ifndef SENDER_HPP_
#define SENDER_HPP_

#include "consts.hpp"
#include "sx128x_stm.hpp"

class Sender
{
public:
    Sender(SX1281& radio)
        : sx(radio)
    {
    }

    void tick()
    {
        uint32_t now = HAL_GetTick();
        uint32_t slot = now / SlotTimeMs;

        // only send once per slot
        if(slot == lastSlot)
            return;

        uint32_t slotStart = slot * SlotTimeMs;
        uint32_t txTime    = slotStart + (SlotTimeMs / 2);

        // wait until middle of slot
        if(now < txTime)
            return;

        lastSlot = slot;

        send(slot);
    }

    uint16_t channels[8]{1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500};

private:
    SX1281& sx;
    uint32_t lastSlot = 0;

    void send(uint32_t slot)
    {
        RcPacket p;
        p.txTimeMs = HAL_GetTick();
        memcpy(p.channels, channels, sizeof(p.channels));

        sx.setFrequency(HopTable[slot % HopCount]);
        HAL_Delay(1);
        sx.transmit(reinterpret_cast<uint8_t*>(&p), sizeof(p));
    }
};


#endif /* SENDER_HPP_ */
