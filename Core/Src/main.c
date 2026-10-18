/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body - STM32F103C8T6 "Blue Pill" hello world.
 *
 *                   Hand-written equivalent of the CubeMX-generated code:
 *                     - System clock: 72 MHz from 8 MHz HSE crystal (PLL x9)
 *                     - OLED:         0.96" SSD1306 (128x64) on software I2C,
 *                                    PB8 = SCL, PB9 = SDA: "hello world"
 *                                    marquee across the top + a 48x48
 *                                    smiley face that blinks
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "oled_i2c.h"
#include "face.h"

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
/* Kept in RAM for on-the-fly debugging: 0 = OLED found & inited,
 * -1 = no I2C ACK at 0x78/0x3C, -2 = NACK during init sequence. */
volatile int oled_rc = -99;
volatile uint32_t oled_step; /* marquee frame counter (debugger-visible) */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void DelayMs(uint32_t ms);
void Error_Handler(void);

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* 0.96" SSD1306 OLED on software I2C (PB8=SCL, PB9=SDA).
   * The SSD1306's onboard controller needs time to boot after power-on;
   * probing right away loses that race and the display never ACKs
   * (screen stays black until a reset). Wait first, then keep retrying
   * in the loop until the display is found. */
  DelayMs(200);
  oled_rc = OLED_Init();

  /* USER CODE BEGIN WHILE */
  /* "hello world" marquee across the top (y = 2), 48x48 smiley face
   * below it (top-left at 40,12); the face blinks every 80 frames. */
  const char *msg = "hello world";
  const int16_t msg_w = 6 * 11;    /* 6 px advance x 11 chars = 66 px */
  int16_t x = (int16_t)OLED_WIDTH; /* start fully off-screen right */
  oled_step = 0U;
  uint32_t frame = 0U;
  while (1)
  {
    if (oled_rc != 0)
    {
      /* display not (yet) ready after power-on: keep retrying the init */
      DelayMs(50);
      oled_rc = OLED_Init();
    }
    else
    {
      OLED_FillScreen(0x00);
      Face_Smiley(40, 12, (uint8_t)((frame % 80U) >= 75U));
      OLED_ShowString(x, 2, msg); /* 8-row font, top of the screen */
      OLED_Refresh();              /* ~230 ms I2C push = the frame pace */
      x -= 2;
      if (x + msg_w - 1 < 0) /* last pixel off the left edge */
      {
        DelayMs(100); /* beat of darkness between passes */
        x = (int16_t)OLED_WIDTH;
      }
    }
    oled_step++;
    frame++;
  }
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 *        8 MHz HSE -> PLL x9 = 72 MHz SYSCLK
 *        AHB = 72 MHz, APB1 = 36 MHz, APB2 = 72 MHz, flash latency 2 ws
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1; /* 8 MHz crystal, not divided */
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
 * @brief  SysTick-free blocking delay (CPU cycle counting, 72 MHz).
 *
 *         This board's clone silicon has a history of corrupted SCB/SysTick
 *         registers at runtime; HAL_Delay would then hang forever. A pure
 *         busy-wait cannot hang. Accuracy: +/-20% (fine for a blink).
 * @param  ms delay in milliseconds
 * @retval None
 */
static void DelayMs(uint32_t ms)
{
  volatile uint32_t i = ms * 12000U; /* ~12M loop iterations/s at 72 MHz */
  while (i--)
  {
    __NOP();
  }
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler */
}
