/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <rtthread.h>


#include "app_x-cube-mems1.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "paho_timer.h"
#include "Ali_iotclient.h"    
#include "stdio.h"
#include "string.h"
#include "bh1750_i2c_drv.h"
#include "oled.h"
#include "Task.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define LENGTH 3 //接收缓冲区的大小
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define  PERIOD_VALUE       (uint32_t)(50000 - 1)  /* Period Value  */
#define  PULSE1_VALUE       PERIOD_VALUE/2   /* Capture Compare 1 Value  */
#define  PULSE2_VALUE       PERIOD_VALUE   /* Capture Compare 1 Value  */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
//extern I2C_HandleTypeDef hi2c1;
uint8_t dat[2] = {0};	       //接收光强数据：dat[0]是高字节，dat[1]是低字节
UART_HandleTypeDef hlpuart1;
UART_HandleTypeDef huart2;      //!!!!!!
UART_HandleTypeDef huart3;

TIM_HandleTypeDef htim15;

/* USER CODE BEGIN PV */
uint8_t RxBuffer2[LENGTH];       //接收缓冲区
uint8_t RxFlag = 0;     //接收完成标志：1表示开启动态显示
static volatile uint8_t button_flags = 0;
net_hnd_t         hnet;
DeviceStatusTypeDef  device_s;
Timer   regular_pub_timer;
Timer   alarm_pub_timer;
uint8_t sendAlarmFlag; //1:means need to send alarm massage, reset it to 0 after sending alarm message
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
//static void MX_LPUART1_UART_Init(void);
static void MX_USART2_UART_Init(void);  //!!!!!!
static void MX_USART3_UART_Init(void);
static void MX_TIM15_Init(void);
/* USER CODE BEGIN PFP */
void initApplication(void);
int initPlatform(void);
static void Button_ISR(void);
void updateDeviceTemperatureStatus(void);
uint32_t isAlarm(void);
int8_t GetTemperatureValue(void);
int8_t GetTempratureThreshold(void);
uint16_t GetLightThreshold(void);       //光照强度阈值的修改
extern int net_if_init(void * if_ctxt);
void startAlarmLedOn(void);
void StopAlarmLed(void);
extern char *myitoa(int value, char *string, int radix);   //将数字转换为字符串
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */

int main(void)
{
  /* USER CODE BEGIN 1 */
  int ret;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  //HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  //SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /*LPUART1被OS占用*/
  //MX_LPUART1_UART_Init();
  MX_USART2_UART_Init();        //!!!!!!
  MX_USART3_UART_Init();
  MX_TIM15_Init();
  MX_MEMS_Init();
  rt_thread_mdelay(100);       //要用魔法对付魔法
  OLED_Init();          //OLED初始化
  OLED_Clear();         //OLED清屏
  
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_IT(&huart2, (uint8_t *)RxBuffer2, LENGTH - 1);        //使能接收中断
  rt_kprintf("\n");
  rt_kprintf("**************************************************************************\n");
  rt_kprintf("***                   STM32 based AliIoT Client Demo                   ***\n");
  rt_kprintf("***                            Without TLS                             ***\n");
  rt_kprintf("***              FW version %d.%d.%d - %s, %s              ***\n", 
           FW_VERSION_MAJOR, FW_VERSION_MINOR, FW_VERSION_PATCH, __DATE__, __TIME__);
  rt_kprintf("**************************************************************************\n");
  
  initApplication();
  
  //WIFI SSID/passowrd config and initialization
  //and config device certificate
  ret = initPlatform();         /*保存输入问题*/ 
  if(ret!=0)
  {
    rt_kprintf("wifi initial failed!\n");
    while(1);
  }
  //connect to Ali Iot platform
  ret = connect2Aliiothub();
  if(ret!=0)
  {
    rt_kprintf("MQTT connection failed!\n");
    while(1);
  }
  //subscribe to topics
  deviceSubscribe();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  //欢迎使用BroJ！
  OLED_ShowCHinese(0+10, 2, 0);
  OLED_ShowCHinese(16+10, 2, 1);
  OLED_ShowCHinese(32+10, 2, 2);
  OLED_ShowCHinese(48+10, 2, 3);
  OLED_ShowString(64+10, 2, "BroJ!", 16);
  
  TaskInit();   //启动MQTT接收线程
//  while(1)
//  {
//    HAL_GPIO_WritePin(LD_light_GPIO_Port, LD_light_Pin, GPIO_PIN_SET);
//    rt_thread_mdelay(500);
//    HAL_GPIO_WritePin(LD_light_GPIO_Port, LD_light_Pin, GPIO_PIN_RESET);
//    rt_thread_mdelay(500);
//  }
  while (1)
  {
    /********* 接收光照强度 **********/
    if(BH1750_Send_Cmd(ONCE_H_MODE) != HAL_OK)
    {
        rt_kprintf("send fail\n");
    }
    rt_thread_mdelay(200);
    if(BH1750_Read_Dat(dat) == HAL_OK)
    {
        //rt_kprintf("Illumination: %5d lux\n", BH1750_Dat_To_Lux(dat));
        SetLightValue(BH1750_Dat_To_Lux(dat));
    }
    else
    {
        rt_kprintf("recv fail\n");
    }
//    if(RxFlag == 1)     //判断是否接收完成
//    {
//      RxFlag = 0;       //清除标志位
//      rt_kprintf("Receive Success!\r\n");
//      rt_kprintf("%s\r\n", RxBuffer2);
//    }
    //MQTT reconnection check
    if(rebuildMQTTConnection()==0)//连接正常
    {
      //doMqttYield();   //尝试接收数据
    /* USER CODE END WHILE */

      MX_MEMS_Process();//读取温湿度
    /* USER CODE BEGIN 3 */
        //update device temperature status
      updateDeviceTemperatureStatus();
      //alarm handler
      if(isAlarm())
      {
        deviceAlarmPub();      
        startAlarmLedOn();   
        sendAlarmFlag =0;
      }
      //判断是否开灯
      if(GetLightValue() < GetLightThreshold())
      {
        HAL_GPIO_WritePin(LD_light_GPIO_Port, LD_light_Pin, GPIO_PIN_SET);
      }
      else
      {
        HAL_GPIO_WritePin(LD_light_GPIO_Port, LD_light_Pin, GPIO_PIN_RESET);
      }
      //判断是否打开风扇
      if(device_s.Tem_status == TEMPERATURE_HIGH_ALARM)
      {
        HAL_GPIO_WritePin(FAN_GPIO_Port, FAN_Pin, GPIO_PIN_SET);
      }
      else
      {
        HAL_GPIO_WritePin(FAN_GPIO_Port, FAN_Pin, GPIO_PIN_RESET);
      }
      //check if regular_pub_timer expired
      //prepared data package, and send to cloud
      if(TimerIsExpired(&regular_pub_timer))    /*不知道是什么问题*/
      {
        //HAL_GPIO_TogglePin(LD_light_GPIO_Port, LD_light_Pin);   //led检测
        deviceStatusPub();      //上传温湿度，和光强
        /*动态显示*/
        if(RxFlag == 1)
        {
          static char Tem_string[6] = {0};
          myitoa(device_s.Tem_value, Tem_string, 10);
          OLED_ShowString(32, 0, "    ", 16);
          OLED_ShowString(32, 0, (uint8_t* )Tem_string, 16);
          myitoa(device_s.Hum_value, Tem_string, 10);
          OLED_ShowString(32, 3, "    ", 16);
          OLED_ShowString(32, 3, (uint8_t* )Tem_string, 16);
          myitoa(device_s.Light_value, Tem_string, 10);
          OLED_ShowString(32, 6, "    ", 16);
          OLED_ShowString(32, 6, (uint8_t* )Tem_string, 16);
        }
        TimerCountdownMS(&regular_pub_timer,REGULAR_TIMER_INTERVAL);
        HAL_GPIO_TogglePin(LD1_GPIO_Port,LD1_Pin);
        rt_thread_mdelay(250);
        HAL_GPIO_TogglePin(LD1_GPIO_Port,LD1_Pin);
      }
    }
    else
    {
      //delay 5s then try to connect to server again
      rt_thread_mdelay(5000);
    }
    rt_thread_mdelay(1000);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage 
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 60;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART3|RCC_PERIPHCLK_LPUART1
                              |RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}



/*实现rt_kprintf()打印字符*/
void rt_hw_console_output(const char *str)
{
    rt_size_t i = 0, size = 0;
    char a = '\r';

    __HAL_UNLOCK(&hlpuart1);

    size = rt_strlen(str);
    for (i = 0; i < size; i++)
    {
        if (*(str + i) == '\n')
        {
            HAL_UART_Transmit(&hlpuart1, (uint8_t *)&a, 1, 1);
        }
        HAL_UART_Transmit(&hlpuart1, (uint8_t *)(str + i), 1, 1);
    }
}

/* 实现 3：finsh 获取一个字符，系统函数，函数名不可更改（转移到board.c） */
/*char rt_hw_console_getchar(void)
{
    int ch = -1;

    if (__HAL_UART_GET_FLAG(&hlpuart1, UART_FLAG_RXNE) != RESET)
    {
        ch = hlpuart1.Instance->RDR & 0xff;
    }
    else
    {
        if(__HAL_UART_GET_FLAG(&hlpuart1, UART_FLAG_ORE) != RESET)
        {
            __HAL_UART_CLEAR_OREFLAG(&hlpuart1);
        }
        rt_thread_mdelay(10);
    }
    return ch;
}*/

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart3, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart3, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief TIM15 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM15_Init(void)
{

  /* USER CODE BEGIN TIM15_Init 0 */
  /* USER CODE END TIM15_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM15_Init 1 */

  /* USER CODE END TIM15_Init 1 */
  htim15.Instance = TIM15;
  htim15.Init.Prescaler = 2399;
  htim15.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim15.Init.Period = 50000;
  htim15.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim15.Init.RepetitionCounter = 0;
  htim15.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim15) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim15, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 25000;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim15, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim15, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM15_Init 2 */

  /* USER CODE END TIM15_Init 2 */
  HAL_TIM_MspPostInit(&htim15);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  HAL_PWREx_EnableVddIO2();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, LD_light_Pin|FAN_Pin|LD1_Pin, GPIO_PIN_RESET);  /*调节光强的led GPIO的修改 & 风扇GPIO的修改*/

  /*Configure GPIO pin : USER_BUTTON_Pin */
  GPIO_InitStruct.Pin = USER_BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USER_BUTTON_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD_light_Pin LD1_Pin */
  GPIO_InitStruct.Pin = LD_light_Pin|FAN_Pin|LD1_Pin;   //调节光强的led GPIO的修改 & 风扇GPIO的修改
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);       //调节光强的led GPIO的修改 & 风扇GPIO的修改

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */
void initApplication(void)
{
  //init device_s.Tem_status as TEMPERATURE_NORMAL 
  SetTemperatureStatus(TEMPERATURE_NORMAL);
  //初始化光强阈值
  getLightThresholdInFlash(&device_s.Light_threshold);
  if(GetLightThreshold()==NULL)
    SetLightThreshold((uint16_t)LED_LIGHT_ON_DEFAULT);
  //read temprature threshold from flash
  //if no content in flash,then set it to default value
  getTempThresholdInFlash(&device_s.Tem_threshold);
  if(GetTempratureThreshold()==NULL)
    SetTempratureThreshold((int8_t)TEMPERATURE_ALARM_DEFAULT);
  rt_kprintf("\nApplication parameter init. Send alarm when temprature>= %d degrees Celsius,turn off Red LED when temprature<%d degrees Celsius\n",GetTempratureThreshold(),GetTempratureThreshold()-TEMPERATURE_ALARM_DELTA);
  //init regulatar publish timer and alarm publish timer
  //and start regular publish timer
  TimerInit(&regular_pub_timer);
  TimerInit(&alarm_pub_timer);
  TimerCountdownMS(&regular_pub_timer,REGULAR_TIMER_INTERVAL);
}

int initPlatform(void)
{
  net_ipaddr_t ipAddr;
  bool skip_reconf = false;
  if (net_init(&hnet, NET_IF, (net_if_init)) != NET_OK)
  {
    //CLOUD_Error_Handler(CLOUD_DEMO_IP_ADDRESS_ERROR);
    return -1;
  }
  
    /* Slight delay since the module seems to take some time prior to being able
   to retrieve its IP address after a connection. */
  rt_thread_mdelay(500);

  rt_kprintf("Retrieving the IP address.\n");
  
  if (net_get_ip_address(hnet, &ipAddr) != NET_OK)
  {
    //CLOUD_Error_Handler(CLOUD_DEMO_IP_ADDRESS_ERROR);
    return -1;
  }
  else
  {
    switch(ipAddr.ipv)
    {
      case NET_IP_V4:
        rt_kprintf("IP address: %d.%d.%d.%d\n", ipAddr.ip[12], ipAddr.ip[13], ipAddr.ip[14], ipAddr.ip[15]);
        break;
      case NET_IP_V6:
      default:
        //CLOUD_Error_Handler(CLOUD_DEMO_IP_ADDRESS_ERROR);
        return -1;
    }
  }
  
  
  skip_reconf = (0==checkDeviceConfig())? 1:0;
  if( skip_reconf ==  true)
  {
     rt_kprintf("Push the User button (Blue) within the next 5 seconds if you want to update "
           "the device security parameters or credentials.\n\n");
    skip_reconf = (Button_WaitForPush(5000) == BP_NOT_PUSHED);
  }
  if( skip_reconf ==  false)
  {
     updateDeviceConfig();
  }

  build_mqtt_topic();
  return 0;
}

/**
  * @brief Update button ISR status
  */
static void Button_ISR(void)
{
  button_flags++;
}

uint32_t HAL_GetTick(void)      //重定向HAL_GetTick()到rt_tock_get()
{
  return rt_tick_get();
}

/**
  * @brief Waiting for button to be pushed
  */
uint8_t Button_WaitForPush(uint32_t delay)
{
  uint32_t time_out = HAL_GetTick()+delay;      
  do
  {
    if (button_flags > 1)
    {   
      button_flags = 0;
      return BP_MULTIPLE_PUSH;
    }
   
    if (button_flags == 1)
    {
      button_flags = 0;
      return BP_SINGLE_PUSH;
    }
  }
  while( HAL_GetTick() < time_out);    
  return BP_NOT_PUSHED;
}


/**
  * @brief  EXTI line detection callback.
  * @param  GPIO_Pin: Specifies the port pin connected to corresponding EXTI line.
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  switch (GPIO_Pin)
  {
    case (USER_BUTTON_Pin):
    {
      Button_ISR();
      break;
    }
    default:
    {
      break;
    }
  }
}

#if (defined(__GNUC__) && !defined(__CC_ARM))
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#define GETCHAR_PROTOTYPE int __io_getchar(void)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#endif /* __GNUC__ */

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART2 and Loop until the end of transmission */
  while (HAL_OK != HAL_UART_Transmit(&hlpuart1, (uint8_t *) &ch, 1, 30000))
  {
    ;
  }
  return ch;
}

/**
  * @brief  Retargets the C library scanf function to the USART.
  * @param  None
  * @retval None
  */
GETCHAR_PROTOTYPE
{
  /* Place your implementation of fgetc here */
  /* e.g. read a character on USART and loop until the end of read */
  uint8_t ch = 0;
  while (HAL_OK != HAL_UART_Receive(&hlpuart1, (uint8_t *)&ch, 1, 30000))
  {
    ;
  }
  return ch;
}


void updateDeviceTemperatureStatus(void)
{
  switch(GetTemperatureStatus())
  {
    case TEMPERATURE_NORMAL:
      if(GetTemperatureValue() >= GetTempratureThreshold())
      {
        SetTemperatureStatus(TEMPERATURE_HIGH_ALARM);
        TimerCountdownMS(&alarm_pub_timer,ALARM_TIMER_INTERVAL);
        sendAlarmFlag=1;
      }
      break;
    case TEMPERATURE_HIGH_ALARM:
      if(GetTemperatureValue() < GetTempratureThreshold()-TEMPERATURE_ALARM_DELTA)
      {
        StopAlarmLed(); 
        SetTemperatureStatus(TEMPERATURE_NORMAL);
      }
      else
      {
          if(TimerIsExpired(&alarm_pub_timer))
         {
           sendAlarmFlag = 1;
           TimerCountdownMS(&alarm_pub_timer,ALARM_TIMER_INTERVAL);
         }
      }
      //TEMPERATURE_HIGH_ALARM  ---- >TEMPERATURE_HIGH_ALARMSTOP is handled in Service_message_handler()
      break;
    case TEMPERATURE_HIGH_ALARMSTOP:
      if(GetTemperatureValue() < GetTempratureThreshold()-TEMPERATURE_ALARM_DELTA)
      {
        SetTemperatureStatus(TEMPERATURE_NORMAL);
        StopAlarmLed();        
      }
      break;
    default:
      break;
    
  }
}

uint32_t isAlarm(void)
{
  return sendAlarmFlag;
}

void SetTemperatureStatus(Temperature_StatusTypeDef  status_value) 
{
  device_s.Tem_status = status_value;
}

Temperature_StatusTypeDef  GetTemperatureStatus(void)
{
  return device_s.Tem_status;
}

int8_t GetTemperatureValue(void)
{
  return device_s.Tem_value;
}

void SetTemperatureValue(int8_t value)
{
  device_s.Tem_value = value;
}

void SetHumValue(uint8_t value)
{
  device_s.Hum_value = value;
}

uint8_t GetHumValue(void)
{
  return device_s.Hum_value;
}

void SetLightValue(uint16_t value)      /*修改*/
{
  device_s.Light_value = value;
}

uint16_t GetLightValue(void)            /*修改*/
{
  return device_s.Light_value;
}

void SetTempratureThreshold(int8_t value)
{
  device_s.Tem_threshold = value;
}

int8_t GetTempratureThreshold(void)
{
  return device_s.Tem_threshold;
}

void SetLightThreshold(uint16_t value)  //光照强度阈值的修改
{
  device_s.Light_threshold = value;
}

uint16_t GetLightThreshold(void)        //光照强度阈值的修改
{
  return device_s.Light_threshold;
}

void StartAlarmLedBlinking(void)
{
  TIM_OC_InitTypeDef sConfigOC = {0};
  
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = PULSE1_VALUE;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim15, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_TIM_PWM_Start(&htim15,TIM_CHANNEL_1);
}

void startAlarmLedOn(void)
{
  TIM_OC_InitTypeDef sConfigOC = {0};
  
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = PULSE2_VALUE;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;

  if (HAL_TIM_PWM_ConfigChannel(&htim15, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_TIM_PWM_Start(&htim15,TIM_CHANNEL_1);  
}

void StopAlarmLed(void)
{
   HAL_TIM_PWM_Stop(&htim15,TIM_CHANNEL_1);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/