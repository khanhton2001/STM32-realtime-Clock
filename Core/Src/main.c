/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "software_timer.h"
#include "led_7seg.h"
#include "button.h"
#include "lcd.h"
#include "picture.h"
#include "ds3231.h"
/* USER CODE END Includes */


extern int status = 1  ;
int hour = 9 ;
int minute = 11 ;
int second = 23 ;
int alarm_second = 1 ;
int alarm_minute = 0 ;
int alarm_hour = 0 ;
/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t count_led_debug = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void system_init();
void test_LedDebug();
void displayTime();
void updateTime();
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

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_SPI1_Init();
  MX_FSMC_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  system_init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
 lcd_Clear(BLACK);
 updateTime();
  while (1)
  {
	 fsm_automatic_run() ;
	 fsm_mannual_run() ;
     alarm_timer_run() ;

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void system_init(){
	  HAL_GPIO_WritePin(OUTPUT_Y0_GPIO_Port, OUTPUT_Y0_Pin, 0);
	  HAL_GPIO_WritePin(OUTPUT_Y1_GPIO_Port, OUTPUT_Y1_Pin, 0);
	  HAL_GPIO_WritePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin, 0);
	  timer_init();
	  led7_init();
	  button_init();
	  lcd_init();
	  ds3231_init();
	  setTimer2(50);
	  setTimer3(0) ;
	  setTimer_MOD(500) ; // blink 2HZ
}

void test_LedDebug(){
	count_led_debug = (count_led_debug + 1)%20;
	if(count_led_debug == 0){
		HAL_GPIO_TogglePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin);
	}
}

void fsm_automatic_run(){

  switch(status){

	case 1:
	  while(!flag_timer2);
		  flag_timer2 = 0;
		  button_Scan();
		  ds3231_ReadTime();
		  displayTime();
		  test_button() ;
		  break ;
  }

}

void alarm_timer_run(){



	 if(flag_timer_alarm == 1){
			 alarm_second-- ;
			 if((alarm_second == 0)&&(alarm_minute > 0)){
				 alarm_minute-- ;

				 if((alarm_minute == 0)&&(alarm_hour > 0)){
					 alarm_hour-- ;
					 alarm_minute = 59 ;
				 }
				 lcd_ShowIntNum(90, 70, alarm_minute, 2, RED, BLACK, 24);
				 alarm_second = 60 ;
			 }

			 flag_timer_alarm = 0 ;
			 lcd_ShowIntNum(140, 70, alarm_second, 2, RED, BLACK, 24);
		 }


		 if((alarm_second == 0)&&(alarm_minute==0)&&(alarm_hour == 0)){
			 alarm_second = 0 ;
			 setTimer_alarm(0);
			 lcd_StrCenter(0, 2, "TIME IS OVER !!!", RED, BLUE, 16, 1);
			 lcd_ShowIntNum(140, 70, alarm_second, 2, RED, BLACK, 24);
	     }

}

void fsm_mannual_run(){
	switch(status){
	case 2: // modify hour
		setTimer3(50) ;
		setTimer2(0) ;


        //blink hour 2Hz
		if(flag_timer_MOD == 0){lcd_ShowIntNum(70, 100, ds3231_hours, 2, GREEN, BLACK, 24); }
     	if(flag_timer_MOD == 1){flag_timer_MOD = 0 ;lcd_ShowIntNum(70, 100, ds3231_hours, 2, BLACK, BLACK, 24);}


		while(!flag_timer3);
				  flag_timer3 = 0;
				  button_Scan();
				  if(button_count[3] == 1){	hour = hour + 1;
				                            if(hour > 24){ hour = 1 ;}
				                            updateTime() ;
				                            ds3231_ReadTime();
				                            displayTime();      }

				  if(button_count[12] == 1){ status = 4 ; }
				  if(button_count[4] == 1){ status  = 5 ; }
				  test_button() ;
				  break ;

	case 3: //back to auto
		setTimer3(0);
		setTimer2(50);
		status = 1 ;
		break ;

	case 4:  // modify minute
		setTimer3(50) ;
		setTimer2(0) ;

		//blink minute 2HZ
		if(flag_timer_MOD == 0){lcd_ShowIntNum(110, 100, ds3231_min, 2, GREEN, BLACK, 24); }
     	if(flag_timer_MOD == 1){ flag_timer_MOD = 0 ; lcd_ShowIntNum(110, 100, ds3231_min, 2, BLACK, BLACK, 24);}

				while(!flag_timer3);
						  flag_timer3 = 0;
						  button_Scan();
						  if(button_count[3] == 1){	minute = minute + 1;
						                            if(minute > 60){minute = 1 ;}
						                            updateTime() ;
						                            ds3231_ReadTime();
						                            displayTime();}

						  if(button_count[12] == 1){ status = 4 ; }
						  test_button() ;
						  break ;


	case 5: //modify alarm second

		lcd_ShowIntNum(40, 70, alarm_hour, 2, RED, BLACK, 24);
		lcd_ShowIntNum(90, 70, alarm_minute, 2, RED, BLACK, 24);

		if(flag_timer_MOD == 0){lcd_ShowIntNum(140, 70, alarm_second, 2, RED, BLACK, 24); }
		if(flag_timer_MOD == 1){flag_timer_MOD = 0; lcd_ShowIntNum(140, 70, alarm_second, 2, BLACK, BLACK, 24);}
		while(!flag_timer3);
			flag_timer3 = 0;
		    button_Scan();
			if(button_count[3] == 1){ alarm_second++ ; if(alarm_second > 60){ alarm_second = 1 ; }  }
								  //ds3231_ReadTime();

								  if(button_count[12] == 1){ //alarm_num = alarm_num * 1000 ;
								  status = 6 ; }
								  test_button() ;
		                          break ;

	case 6:  //modify minute alarm
		lcd_ShowIntNum(140, 70, alarm_second, 2, RED, BLACK, 24);
		lcd_ShowIntNum(40, 70, alarm_hour, 2, RED, BLACK, 24);


		if(flag_timer_MOD == 0){lcd_ShowIntNum(90, 70, alarm_minute, 2, RED, BLACK, 24); }
	    if(flag_timer_MOD == 1){flag_timer_MOD = 0; lcd_ShowIntNum(90, 70, alarm_minute, 2, BLACK, BLACK, 24);}

		while(!flag_timer3);
			flag_timer3 = 0;
			button_Scan();
		    if(button_count[3] == 1){  alarm_minute++;  if(alarm_minute > 60){ alarm_minute = 1 ; }}

		    if(button_count[12] == 1){ 	status = 7 ;}
			test_button() ;

		break ;


	case 7://modify minute hour
		lcd_ShowIntNum(140, 70, alarm_second, 2, RED, BLACK, 24);
		lcd_ShowIntNum(90, 70, alarm_minute, 2, RED, BLACK, 24);

		if(flag_timer_MOD == 0){lcd_ShowIntNum(40, 70, alarm_hour, 2, RED, BLACK, 24); }
	    if(flag_timer_MOD == 1){flag_timer_MOD = 0; lcd_ShowIntNum(40, 70, alarm_hour, 2, BLACK, BLACK, 24);}

		while(!flag_timer3);
				flag_timer3 = 0;
				button_Scan();
				if(button_count[3] == 1){  alarm_hour++;  if(alarm_hour > 24){ alarm_hour = 1 ; }  }

				if(button_count[12] == 1){
											status = 3 ;
											lcd_ShowIntNum(40, 70, alarm_hour, 2, RED, BLACK, 24);
										    lcd_ShowIntNum(90, 70, alarm_minute, 2, RED, BLACK, 24);
											lcd_ShowIntNum(140, 70, alarm_second, 2, RED, BLACK, 24);
											setTimer_alarm(1000) ;}
			   test_button() ;


		break ;











	}
}

void test_7seg(){
	led7_SetDigit(0, 0, 0);
	led7_SetDigit(5, 1, 0);
	led7_SetDigit(4, 2, 0);
	led7_SetDigit(7, 3, 0);
}
void test_button(){
	for(int i = 0; i < 16; i++){

		if(button_count[0] == 1){	status = 2; }
		if(button_count[1] == 1){	status = 3; }

		//if(button_count[1] == 1){	setTimer2(50) ;}

		if(button_count[11] == 1){
			lcd_StrCenter(0, 2, "REAL TIME CLOCK !!!", RED, BLUE, 16, 1);
		}

		if(button_count[15] == 1){
					lcd_StrCenter(0, 2, "REAL TIME CLOCK !!!", WHITE, BLUE, 16, 1);
				}
	}
}

void updateTime(){
	ds3231_Write(ADDRESS_YEAR, 23);
	ds3231_Write(ADDRESS_MONTH, 11);
	ds3231_Write(ADDRESS_DATE, 16);
	ds3231_Write(ADDRESS_DAY, 5);
	ds3231_Write(ADDRESS_HOUR, hour);
	ds3231_Write(ADDRESS_MIN, minute);
	ds3231_Write(ADDRESS_SEC, second);
}

uint8_t isButtonUp()
{
    if (button_count[3] == 1)
        return 1;
    else
        return 0;
}
uint8_t isButtonDown()
{
    if (button_count[7] == 1)
        return 1;
    else
        return 0;
}
void displayTime(){
	lcd_ShowIntNum(70, 100, ds3231_hours, 2, GREEN, BLACK, 24);
	lcd_ShowIntNum(110, 100, ds3231_min, 2, GREEN, BLACK, 24);
	lcd_ShowIntNum(150, 100, ds3231_sec, 2, GREEN, BLACK, 24);
	lcd_ShowIntNum(20, 130, ds3231_day, 2, YELLOW, BLACK, 24);
	lcd_ShowIntNum(70, 130, ds3231_date, 2, YELLOW, BLACK, 24);
	lcd_ShowIntNum(110, 130, ds3231_month, 2, YELLOW, BLACK, 24);
	lcd_ShowIntNum(150, 130, ds3231_year, 2, YELLOW, BLACK, 24);
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
  __disable_irq();
  while (1)
  {
  }
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
