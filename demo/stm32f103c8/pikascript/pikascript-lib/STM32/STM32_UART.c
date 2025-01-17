#include "STM32_UART.h"
#include "BaseObj.h"
#include "STM32_common.h"
#include "dataStrs.h"
#include <stdint.h>

#define RX_BUFF_LENGTH 64

struct _pika_uart_t {
  UART_HandleTypeDef huart;
  uint8_t id;
  char rxBuff[RX_BUFF_LENGTH];
  uint16_t rxBuffOffset;
  PikaObj *obj;
};

typedef struct _pika_uart_t pika_uart_t;

#ifdef UART1_EXIST
    pika_uart_t pika_uart1;
#endif
#ifdef UART2_EXIST
    pika_uart_t pika_uart2;
#endif
#ifdef UART3_EXIST
    pika_uart_t pika_uart3;
#endif
#ifdef UART4_EXIST
    pika_uart_t pika_uart4;
#endif

static pika_uart_t *getPikaUart(uint8_t id) {
  if (1 == id) {
    return &pika_uart1;
  }
  if (2 == id) {
    return &pika_uart2;
  }
#ifdef UART3_EXIST  
  if (3 == id) {
    return &pika_uart3;
  }
#endif
#ifdef UART4_EXIST
  if (4 == id) {
    return &pika_uart4;
  }
#endif
  return NULL;
}

static void setUartObj(uint8_t id, PikaObj *obj) {
  pika_uart_t *pika_uart = getPikaUart(id);
  pika_uart->obj = obj;
}

static PikaObj *getUartObj(uint8_t id) {
  pika_uart_t *pika_uart = getPikaUart(id);
  if (NULL == pika_uart) {
    return NULL;
  }
  return pika_uart->obj;
}

static USART_TypeDef *getUartInstance(uint8_t id) {
#ifdef UART1_EXIST
  if (1 == id) {
    return USART1;
  }
#endif
#ifdef UART2_EXIST  
  if (2 == id) {
    return USART2;
  }
#endif
#ifdef UART3_EXIST
  if (3 == id) {
    return USART3;
  }
#endif
#ifdef UART4_EXIST
  if (4 == id) {
    return USART4;
  }
#endif
  return NULL;
}

static uint8_t getUartId(UART_HandleTypeDef *huart) {
#ifdef UART1_EXIST
  if (huart == &pika_uart1.huart) {
    return 1;
  }
#endif
#ifdef UART2_EXIST
  if (huart == &pika_uart2.huart) {
    return 2;
  }
#endif
#ifdef UART3_EXIST
  if (huart == &pika_uart3.huart) {
    return 3;
  }
#endif
#ifdef UART4_EXIST
  if (huart == &pika_uart4.huart) {
    return 4;
  }
#endif
  return 0;
}

static UART_HandleTypeDef *getUartHandle(uint8_t id) {
  pika_uart_t *pika_uart = getPikaUart(id);
  if (NULL == pika_uart) {
    return NULL;
  }
  return &(pika_uart->huart);
}

static char *getUartRxBuff(uint8_t id) {
  pika_uart_t *pika_uart = getPikaUart(id);
  if (NULL == pika_uart) {
    return NULL;
  }
  return pika_uart->rxBuff;
}

static uint8_t USART_UART_Init(uint32_t baudRate, uint8_t id) {
  uint8_t errCode = 0;
  UART_HandleTypeDef *huart = getUartHandle(id);
  huart->Instance = getUartInstance(id);
  if(NULL == huart->Instance)
  {
    errCode = 5;
    goto exit;
  }
  huart->Init.BaudRate = baudRate;
  huart->Init.WordLength = UART_WORDLENGTH_8B;
  huart->Init.StopBits = UART_STOPBITS_1;
  huart->Init.Parity = UART_PARITY_NONE;
  huart->Init.Mode = UART_MODE_TX_RX;
  huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart->Init.OverSampling = UART_OVERSAMPLING_16;
#if (defined STM32G070xx) || (defined STM32G030xx)  
      huart->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
      huart->Init.ClockPrescaler = UART_PRESCALER_DIV1;
      huart->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
#endif  
  if (HAL_UART_Init(huart) != HAL_OK) {
    errCode = 1;
    goto exit;
  }
#if (defined STM32G070xx) || (defined STM32G030xx)  
    if (HAL_UARTEx_SetTxFifoThreshold(huart, UART_TXFIFO_THRESHOLD_1_8) !=
      HAL_OK) {
    errCode = 2;
    goto exit;
    }
    if (HAL_UARTEx_SetRxFifoThreshold(huart, UART_RXFIFO_THRESHOLD_1_8) !=
      HAL_OK) {
    errCode = 3;
    goto exit;
    }
    if (HAL_UARTEx_DisableFifoMode(huart) != HAL_OK) {
    errCode = 4;
    goto exit;
    }
#endif
exit:
  return errCode;
}

static void UART_MspInit(UART_HandleTypeDef *uartHandle) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
#ifdef UART1_EXIST	
    if (uartHandle->Instance == USART1) {
        /* USART1 clock enable */
        __HAL_RCC_USART1_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**USART1 GPIO Configuration
        PA9     ------> USART1_TX
        PA10     ------> USART1_RX
        */
        #if (defined STM32G070xx) || (defined STM32G030xx)
            GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
            GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
            GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
            HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);    
        #endif
        #ifdef STM32F103xB
            GPIO_InitStruct.Pin = GPIO_PIN_9;
            GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
            HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

            GPIO_InitStruct.Pin = GPIO_PIN_10;
            GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        #endif
            /* USART1 interrupt Init */
            HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
            HAL_NVIC_EnableIRQ(USART1_IRQn);
        } 
#endif	
#ifdef UART2_EXIST
    if (uartHandle->Instance == USART2) {
        /* USART2 clock enable */
        __HAL_RCC_USART2_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**USART2 GPIO Configuration
        PA2     ------> USART2_TX
        PA3     ------> USART2_RX
        */
        #if (defined STM32G070xx) || (defined STM32G030xx)        
            GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
            GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
            GPIO_InitStruct.Alternate = GPIO_AF1_USART2;
            HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        #endif
        #ifdef STM32F103xB
            GPIO_InitStruct.Pin = GPIO_PIN_2;
            GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
            HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

            GPIO_InitStruct.Pin = GPIO_PIN_3;
            GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        #endif    
        /* USART2 interrupt Init */
        HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(USART2_IRQn);
    }
#endif
#ifdef UART3_EXIST
    if (uartHandle->Instance == USART3) {
    /* USART3 clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**USART3 GPIO Configuration
    PA5     ------> USART3_TX
    PB0     ------> USART3_RX
    */
    #if (defined STM32G070xx)             
        GPIO_InitStruct.Pin = GPIO_PIN_5;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF4_USART3;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_0;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF4_USART3;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    #endif

    #ifdef STM32F103xB
    /**USART3 GPIO Configuration
        PB10     ------> USART3_TX
        PB11     ------> USART3_RX
        */
        GPIO_InitStruct.Pin = GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_11;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    #endif

        /* USART3 interrupt Init */
    #if (defined STM32G070xx) || (defined STM32G030xx)            
        HAL_NVIC_SetPriority(USART3_4_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(USART3_4_IRQn);
    #endif    
    #ifdef STM32F103xB
        HAL_NVIC_SetPriority(USART3_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(USART3_IRQn);
    #endif
    } 
#endif
#ifdef UART4_EXIST
    #if (defined STM32G070xx)            
        if (uartHandle->Instance == USART4) {
        /* USART4 clock enable */
        __HAL_RCC_USART4_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**USART4 GPIO Configuration
        PA0     ------> USART4_TX
        PA1     ------> USART4_RX
        */
        GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF4_USART4;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* USART4 interrupt Init */
        HAL_NVIC_SetPriority(USART3_4_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(USART3_4_IRQn);
      }
    #endif
#endif
}

/* Msp handle interrupt */
#ifdef UART1_EXIST
void USART1_IRQHandler(void) { HAL_UART_IRQHandler(&pika_uart1.huart); }
#endif
#ifdef UART2_EXIST
void USART2_IRQHandler(void) { HAL_UART_IRQHandler(&pika_uart2.huart); }
#endif
#ifdef UART3_EXIST
	#ifdef STM32F103xB
		void USART3_IRQHandler(void) { HAL_UART_IRQHandler(&pika_uart3.huart); }
	#endif
#endif

#if (defined UART3_EXIST) && (defined UART4_EXIST)
    #if defined STM32G070xx           
      void USART3_4_IRQHandler(void) {
        HAL_UART_IRQHandler(&pika_uart3.huart);
        HAL_UART_IRQHandler(&pika_uart4.huart);
      }
    #endif
#endif

void STM32_UART_platformEnable(PikaObj *self, int baudRate, int id) {
    setUartObj(id, self);
    UART_HandleTypeDef *huart = getUartHandle(id);
    huart->Instance = getUartInstance(id);
    UART_MspInit(huart);
    int errCode = USART_UART_Init(baudRate, id);
    if (0 != errCode) {
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] uart init faild.");
    return;
    }
    HAL_UART_Receive_IT(getUartHandle(id), (uint8_t *)getUartRxBuff(id), 1);
}

char *STM32_UART_platformRead(PikaObj *self, int id, int length) {
    Args *buffs = New_strBuff();
    char *readBuff = NULL;
    pika_uart_t *pika_uart = getPikaUart(id);
    if (length >= pika_uart->rxBuffOffset) {
    /* not enough str */
    length = pika_uart->rxBuffOffset;
    }
    readBuff = args_getBuff(buffs, length);
    memcpy(readBuff, pika_uart->rxBuff, length);
    obj_setStr(self, "readBuff", readBuff);
    readBuff = obj_getStr(self, "readBuff");

    /* update rxBuff */
    memcpy(pika_uart->rxBuff, pika_uart->rxBuff + length,
         pika_uart->rxBuffOffset - length);
    pika_uart->rxBuffOffset -= length;
    UART_Start_Receive_IT(
      &pika_uart->huart,
      (uint8_t *)(pika_uart->rxBuff + pika_uart->rxBuffOffset), 1);
exit:
    args_deinit(buffs);
    return readBuff;
}

void STM32_UART_platformWrite(PikaObj *self, char *data, int id) {
    HAL_UART_Transmit(getUartHandle(id), (uint8_t *)data, strGetSize(data), 100);
}

/* Recive Interrupt Handler */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    uint8_t id = getUartId(huart);
    pika_uart_t *pika_uart = getPikaUart(id);

    /* avoid recive buff overflow */
    if (pika_uart->rxBuffOffset + 1 < RX_BUFF_LENGTH) {
    pika_uart->rxBuffOffset++;
    }
    UART_Start_Receive_IT(
      huart, (uint8_t *)(pika_uart->rxBuff + pika_uart->rxBuffOffset), 1);
    goto exit;
exit:
  return;
}

/* support prinf */
int fputc(int ch, FILE *f) {
    HAL_UART_Transmit(&pika_uart1.huart, (uint8_t *)&ch, 1, 0xffff);
    return ch;
}

/* support scanf */
int fgetc(FILE *f) {
    uint8_t ch = 0;
    HAL_UART_Receive(&pika_uart1.huart, &ch, 1, 0xffff);
    return ch;
}