/*
 * sx128x_stm.hpp
 *
 *  Created on: Jun 14, 2026
 *      Author: hannes
 */

#ifndef SX128X_STM_HPP_
#define SX128X_STM_HPP_

#pragma once

#include "stm32g0xx_ll_gpio.h"
#include "stm32g0xx_ll_spi.h"
#include <stdint.h>


class SX1281
{
public:
    SX1281(
        SPI_TypeDef* spi,
        GPIO_TypeDef* csPort,
        uint32_t csPin,
        GPIO_TypeDef* busyPort,
        uint32_t busyPin,
        GPIO_TypeDef* rstPort,
        uint32_t rstPin,
		GPIO_TypeDef* skyCpsPort,
		uint32_t skyCpsPin,
		GPIO_TypeDef* skyCsdPort,
		uint32_t skyCsdPin,
		GPIO_TypeDef* skyCtxPort,
		uint32_t skyCtxPin,
		GPIO_TypeDef* skyAntselPort,
		uint32_t skyAntselPin)
        :
        spi_(spi),
        csPort_(csPort),
        csPin_(csPin),
        busyPort_(busyPort),
        busyPin_(busyPin),
        rstPort_(rstPort),
        rstPin_(rstPin),
		skyCpsPort_(skyCpsPort),
    	skyCpsPin_(skyCpsPin),
		skyCsdPort_(skyCsdPort),
    	skyCsdPin_(skyCsdPin),
		skyCtxPort_(skyCtxPort),
    	skyCtxPin_(skyCtxPin),
		skyAntselPort_(skyAntselPort),
    	skyAntselPin_(skyAntselPin)
    {
    }

    bool begin()
    {
    	LL_SPI_Enable(spi_);
    	sky_bypass();

        reset();
        setStandby();
        setPacketTypeLoRa();
        setFrequency(2445000000UL);
        setTxPower(-5); // ca. 22dB Amp Gain + Antenna Gain (20dBm Iso Eu limit)
        setLoRaParams(
            0x70,   // SF7
            0x18,   // BW 812kHz
            0x01);  // CR 4/5

        setPacketParams(
            12,     // preamble
            20,      // payload length
            true,   // crc
            false); // explicit header

        setBufferBaseAddress(0x00, 0x00);
        setIrqConfig();
        clearIrqStatus(0xFFFF);

        return true;
    }

    void reset()
    {
        LL_GPIO_ResetOutputPin(rstPort_, rstPin_);
        HAL_Delay(2);
        LL_GPIO_SetOutputPin(rstPort_, rstPin_);
        HAL_Delay(20);
        waitBusy();
    }

    void sky_set_ant(int num)
    {
    	if(num) LL_GPIO_SetOutputPin(sky_antsel_GPIO_Port, sky_antsel_Pin);
    	else LL_GPIO_ResetOutputPin(sky_antsel_GPIO_Port, sky_antsel_Pin);
    }
    void sky_toggle_ant()
    {
    	LL_GPIO_TogglePin(sky_antsel_GPIO_Port, sky_antsel_Pin);
    }

    void setStandby()
    {
        uint8_t p = 0x00; // RC
        writeCommand(0x80, &p, 1);
    }

    void setPacketTypeLoRa()
    {
        uint8_t p = 0x01;
        writeCommand(0x8A, &p, 1);
    }

    void setFrequency(uint32_t hz)
    {
        uint32_t rf =
            (uint32_t)(((uint64_t)hz << 18) / 52000000ULL);

        uint8_t p[3] =
        {
            (uint8_t)(rf >> 16),
            (uint8_t)(rf >> 8),
            (uint8_t)(rf)
        };

        writeCommand(0x86, p, sizeof(p));
    }

    void setTxPower(int8_t dbm)
    {
        uint8_t p[2] =
        {
            (uint8_t)dbm+18,
            0xE0
        };

        writeCommand(0x8E, p, sizeof(p));
    }

    void setLoRaParams(uint8_t sf, uint8_t bw, uint8_t cr)
    {
        uint8_t p[3] ={sf, bw, cr};

        writeCommand(0x8B, p, sizeof(p));

//        uint8_t cmd[3] ={0x09, 0x25, 0x37}; // Todo: Based on SF
//        writeCommand(0x18, cmd, sizeof(cmd));
    }

    void setPacketParams(
        uint16_t preambleSymbols,
        uint8_t payloadLen,
        bool crcEnabled,
        bool implicitHeader)
    {
        uint8_t p[5];

        p[0] = (uint8_t)(preambleSymbols);

        p[1] = implicitHeader ? 0x80 : 0x00;
        p[2] = payloadLen;
        p[3] = crcEnabled ? 0x20 : 0x00;
        p[4] = 0x40;

        writeCommand(0x8C, p, sizeof(p));
    }

    void setBufferBaseAddress(
        uint8_t txBase,
        uint8_t rxBase)
    {
        uint8_t p[2] =
        {
            txBase,
            rxBase
        };

        writeCommand(0x8F, p, sizeof(p));
    }

    void transmit(
        const uint8_t* data,
        uint8_t len)
    {
    	sky_set_ant(0);
    	sky_tx_amp();
        writeBuffer(0, data, len);

        uint8_t timeout[3] =
        {
            0x00,
            0x00,
            0x00
        };

        clearIrqStatus(0xFFFF);

        writeCommand(0x83, timeout, sizeof(timeout));

        while (!(getIrqStatus() & 0x0001))
        {
        }

        clearIrqStatus(0xFFFF);
        sky_bypass();
    }

    int receive(
        uint8_t* data,
        uint32_t timeout_ms = 5000)
    {
    	sky_rx_lna();
    	uint8_t timeout[3] =
    	{
    	    static_cast<uint8_t>(0x02), // 1 ms base
    	    static_cast<uint8_t>((timeout_ms >> 8) & 0xFF),
    	    static_cast<uint8_t>(timeout_ms & 0xFF)
    	};

        clearIrqStatus(0xFFFF);

        writeCommand(0x82, timeout, sizeof(timeout));

        while (!(getIrqStatus()))
        {
        	asm("NOP");
        }
        if(!(getIrqStatus() & 0x0002)) return 0;

        uint8_t payloadLength;
        uint8_t startPointer;

        getRxBufferStatus(
            payloadLength,
            startPointer);

        readBuffer(
            startPointer,
            data,
            payloadLength);

        getPacketStatus();

        clearIrqStatus(0xFFFF);
        sky_bypass();
        return payloadLength;
    }

    int lastSNRDb{};
    int lastRSSIDbm{};

private:
    SPI_TypeDef* spi_;

    GPIO_TypeDef* csPort_;
    uint32_t csPin_;

    GPIO_TypeDef* busyPort_;
    uint32_t busyPin_;

    GPIO_TypeDef* rstPort_;
    uint32_t rstPin_;

    GPIO_TypeDef* skyCpsPort_;
    uint32_t skyCpsPin_;

    GPIO_TypeDef* skyCsdPort_;
    uint32_t skyCsdPin_;

    GPIO_TypeDef* skyCtxPort_;
    uint32_t skyCtxPin_;

    GPIO_TypeDef* skyAntselPort_;
    uint32_t skyAntselPin_;

private:
    void csLow()
    {
        LL_GPIO_ResetOutputPin( csPort_, csPin_);
    }

    void csHigh()
    {
        LL_GPIO_SetOutputPin(csPort_, csPin_);
    }

    void waitBusy()
    {
        while (LL_GPIO_IsInputPinSet(busyPort_, busyPin_)){}
    }

    uint8_t spiTransfer(uint8_t tx)
    {
        while (!LL_SPI_IsActiveFlag_TXE(spi_))
        {
        }

        LL_SPI_TransmitData8(spi_, tx);

        while (!LL_SPI_IsActiveFlag_RXNE(spi_))
        {
        }

        return LL_SPI_ReceiveData8(spi_);
    }

    void writeCommand(
        uint8_t opcode,
        const uint8_t* data,
        uint8_t len)
    {
        waitBusy();

        csLow();

        spiTransfer(opcode);

        for (uint8_t i = 0; i < len; i++)
        {
            spiTransfer(data[i]);
        }

        csHigh();

        waitBusy();
    }

    void readCommand(
        uint8_t opcode,
        uint8_t* data,
        uint8_t len)
    {
        waitBusy();

        csLow();

        spiTransfer(opcode);
        spiTransfer(0x00);

        for (uint8_t i = 0; i < len; i++)
        {
            data[i] = spiTransfer(0x00);
        }

        csHigh();

        waitBusy();
    }

    void writeBuffer(
        uint8_t offset,
        const uint8_t* data,
        uint8_t len)
    {
        waitBusy();

        csLow();

        spiTransfer(0x1A);
        spiTransfer(offset);

        for (uint8_t i = 0; i < len; i++)
        {
            spiTransfer(data[i]);
        }

        csHigh();

        waitBusy();
    }

    void readBuffer(
        uint8_t offset,
        uint8_t* data,
        uint8_t len)
    {
        waitBusy();

        csLow();

        spiTransfer(0x1B);
        spiTransfer(offset);
        spiTransfer(0x00);

        for (uint8_t i = 0; i < len; i++)
        {
            data[i] = spiTransfer(0x00);
        }

        csHigh();

        waitBusy();
    }

    void setIrqConfig()
    {
    	// TxDone IRQ on DIO1, RxDone IRQ on DIO2 and HeaderError and RxTxTimeout IRQ on DIO3
        uint8_t p[8] ={0x40, 0x23, 0x00, 0x01, 0x00, 0x02, 0x40, 0x20};
        writeCommand(0x8D, p, sizeof(p));
    }

    uint16_t getIrqStatus()
    {
        uint8_t r[2];

        readCommand(0x15, r, 2);

        return ((uint16_t)r[0] << 8) | r[1];
    }

    void clearIrqStatus(uint16_t mask)
    {
        uint8_t p[2] =
        {
            (uint8_t)(mask >> 8),
            (uint8_t)(mask)
        };

        writeCommand(0x97, p, sizeof(p));
    }

    void getRxBufferStatus(
        uint8_t& payloadLength,
        uint8_t& startPointer)
    {
        uint8_t r[2];

        readCommand(0x17, r, 2);

        payloadLength = r[0];
        startPointer  = r[1];
    }

    void getPacketStatus()
    {
        uint8_t buf[5];

        readCommand(0x1D, buf, sizeof(buf));

        lastRSSIDbm = -buf[0] / 2;
        lastSNRDb = ((int8_t)buf[1]) / 4;
    }

    void sky_sleep()
    {
    	LL_GPIO_ResetOutputPin(skyCpsPort_, skyCpsPin_);
    	LL_GPIO_ResetOutputPin(skyCsdPort_, skyCsdPin_);
    	LL_GPIO_ResetOutputPin(skyCtxPort_, skyCtxPin_);
    }
    void sky_bypass()
    {
    	LL_GPIO_ResetOutputPin(skyCpsPort_, skyCpsPin_);
    	LL_GPIO_SetOutputPin(skyCsdPort_, skyCsdPin_);
    	LL_GPIO_ResetOutputPin(skyCtxPort_, skyCtxPin_);
    }
    void sky_rx_lna()
    {
    	LL_GPIO_SetOutputPin(skyCpsPort_, skyCpsPin_);
    	LL_GPIO_SetOutputPin(skyCsdPort_, skyCsdPin_);
    	LL_GPIO_ResetOutputPin(skyCtxPort_, skyCtxPin_);
    }
    void sky_tx_amp()
    {
    	LL_GPIO_ResetOutputPin(skyCpsPort_, skyCpsPin_);
    	LL_GPIO_SetOutputPin(skyCsdPort_, skyCsdPin_);
    	LL_GPIO_SetOutputPin(skyCtxPort_, skyCtxPin_);
    }
    void sky_ant(int val)
    {
    	if(val) LL_GPIO_ResetOutputPin(skyAntselPort_, skyAntselPin_);
    	else LL_GPIO_SetOutputPin(skyAntselPort_, skyAntselPin_);
    }
};


#endif /* SX128X_STM_HPP_ */
