#include "include.h"

/**
 * @brief SPI user event handler.
 * @param event
 */
void spiEventHandler(nrf_drv_spi_evt_t const * p_event,
                            void *                  p_context)
{
    spiXferDone = true;
    NRF_LOG_INFO("Transfer completed\r\n");
}

void initSPI(void)
{
  nrf_drv_spi_config_t spiConfig;
  spiXferDone = false;
  spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);
  spiConfig.ss_pin   = CS;
  spiConfig.mosi_pin = MOSI;
//  spiConfig.miso_pin = MISO;
  spiConfig.sck_pin  = CLK;
  spiConfig.irq_priority = SPI_DEFAULT_CONFIG_IRQ_PRIORITY;
  spiConfig.orc = 0xFF;
  spiConfig.frequency = NRF_DRV_SPI_FREQ_4M;
  spiConfig.mode = NRF_DRV_SPI_MODE_3;
  spiConfig.bit_order = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST;

  APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spiConfig, spiEventHandler, NULL));
}

void writeSPI(uint8_t *pData, uint8_t len)
{
  memset(rxBuff, 0, RX_BUFF_LEN);
  spiXferDone = false;
  APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, pData, len, rxBuff, 1));

  while (!spiXferDone)
  {
      __WFE();
  }
}

uint32_t readSPI(uint8_t *pData, uint8_t len)
{
  // Reset rx buffer and transfer done flag
  memset(rxBuff, 0, RX_BUFF_LEN);
  spiXferDone = false;
  APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, pData, len, rxBuff, RX_BUFF_LEN));

  while (!spiXferDone)
  {
      __WFE();
  } 
  return (rxBuff[0] & (rxBuff[1]<<8) & 
          (rxBuff[2]<<16) & (rxBuff[3]<<24));
}