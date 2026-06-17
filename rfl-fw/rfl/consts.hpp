/*
 * consts.hpp
 *
 *  Created on: Jun 17, 2026
 *      Author: hannes
 */

#ifndef CONSTS_HPP_
#define CONSTS_HPP_


static constexpr uint32_t SlotTimeMs = 20;
static constexpr uint32_t SearchListenMs = 50;
static constexpr uint32_t LinkTimeoutMs = 500;

static constexpr uint32_t HopTable[] =
{
    2405000000,
    2410000000,
    2415000000,
    2420000000,
    2425000000,
    2430000000,
    2435000000,
    2440000000
};

static constexpr uint8_t HopCount =
    sizeof(HopTable) / sizeof(HopTable[0]);

struct RcPacket
{
    uint32_t txTimeMs;
    uint16_t channels[8];
};


#endif /* CONSTS_HPP_ */
