/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h> // Added for sscanf
#include "stm32f4xx_hal.h" // For HAL_FLASH functions
#include "stm32f4xx_hal_flash_ex.h" // For HAL_FLASHEx_Erase
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum {
    BOOTLOADER_STATE_IDLE,             // Chờ lệnh "size = ..."
    BOOTLOADER_STATE_RECEIVING_SIZE,   // Đang nhận chuỗi kích thước
    BOOTLOADER_STATE_RECEIVING_DATA,   // Đang nhận dữ liệu firmware
    BOOTLOADER_STATE_UPDATE_COMPLETE   // Cập nhật hoàn tất
} BootloaderState_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile BootloaderState_t bootloaderState = BOOTLOADER_STATE_IDLE;
uint32_t fw_size = 0; // To store the firmware size
// You need a buffer large enough to store the entire firmware.
// The example image shows size = 5784, so allocate at least that much.
// Consider the maximum expected firmware size for your application.
#define MAX_FW_SIZE 8000 // Example maximum firmware size, adjust as needed
uint8_t firmware_buffer[MAX_FW_SIZE]; // Buffer to store received firmware data
uint32_t firmware_buffer_index = 0; // To track current position in firmware_buffer
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define GPIOA_BASE_ARR 0x40020000
#define GPIOD_BASE_ARR 0x40020C00

uint32_t global_var = 0;
char recvData[32];
void button_init(){
	__HAL_RCC_GPIOA_CLK_ENABLE();

	//set PA0 in floating input
	uint32_t* GPIOA_MODER = (uint32_t*)(GPIOA_BASE_ARR + 0x00);
	*GPIOA_MODER &= ~(0b11 << 0); //set PA0 in INPUT mode by clearbit

	uint32_t* GPIOA_PUPDR = (uint32_t*)(GPIOA_BASE_ARR + 0x0C);
	*GPIOA_PUPDR &= ~(0b11 << 0); //set no pull-up, pull-down (floating)
}

char read_button_state(){
	uint32_t* GPIOA_IDR = (uint32_t*)(GPIOA_BASE_ARR + 0x10);
	return (*GPIOA_IDR >> 0 & 1); //read PA0
}

void leds_init(){
	//set PD12, PD13, PD14, PD15 in output push-pull
	__HAL_RCC_GPIOD_CLK_ENABLE();

	uint32_t* GPIOD_MODER = (uint32_t*)(GPIOD_BASE_ARR + 0x00);
	*GPIOD_MODER |= (0b01 << 24);//set PD12 in OUTPUT
	*GPIOD_MODER |= (0b01 << 26);//set PD13 in OUTPUT
	*GPIOD_MODER |= (0b01 << 28);//set PD14 in OUTPUT
	*GPIOD_MODER |= (0b01 << 30);//set PD15 in OUTPUT

	uint32_t* GPIOD_OTYPER = (uint32_t*)(GPIOD_BASE_ARR + 0x04);
	*GPIOD_OTYPER &= ~(0b1111 << 12);//set PD12, PD13, PD14, PD15
}

void led_control(char led_state){
	uint32_t* GPIOD_ODR = (uint32_t*)(GPIOD_BASE_ARR + 0x14);
	if(led_state == 1){
		*GPIOD_ODR |= 1 << 12;
	}else{
		*GPIOD_ODR &= ~(1<<12);
	}
}

void exti0_init()
{
	__HAL_RCC_GPIOD_CLK_ENABLE();
	uint32_t* EXTI_RTSR = (uint32_t*)(0x40013c00 + 0x08);
	uint32_t* EXTI_IMR = (uint32_t*)(0x40013c00 + 0x00);
	*EXTI_RTSR |= (1<<0); //set rising for EXTI0
	*EXTI_IMR |= (1 << 0); //enable EXTI0

	uint32_t*NVIC_ISER0 = (uint32_t*)(0xe000e100);
	*NVIC_ISER0 |= (1<<6); //enable interrupt for event in position 6 vector table (EXTI0)
}

void EXTI0_IRQHandler()
{
	__asm("nop");

	uint32_t* EXTI_RR = (uint32_t*)(0x40013c00 + 0x014);
	*EXTI_RR |= (1<<0); // clear interrupt event flag
}

void custom_exti0_handler()
{
	__asm("nop");

	uint32_t* EXTI_RR = (uint32_t*)(0x40013c00 + 0x014);
	*EXTI_RR |= (1<<0); // clear interrupt event flag
}

void UART_init()
{
	__HAL_RCC_GPIOB_CLK_ENABLE();
	//set PB6 as UART1_Tx(AF07) and PB7 as UART1_Rx(AF07)
	uint32_t* MODER = (uint32_t*)(0x40020400); //GPIOB
	uint32_t* AFRL = (uint32_t*)(0x40020420); //alternate function Low
	//set alternate function mode
	*MODER &= ~(0b1111 << 12);
	*MODER |= (0b10 << 12) | (0b10 << 14);

	*AFRL &= ~(0xff << 24);
	*AFRL |= (7<<24) | (7<<28); //set AF07 for PB6 and PB7

	//UART:
	//	+ baudrate: 9600
	//	+frame:
	//      *data len: 8byte
	//      *parity (none/odd/even): none

	__HAL_RCC_USART1_CLK_ENABLE();
	uint32_t* BRR = (uint32_t*)(0x40011008);
	uint32_t* CR1 = (uint32_t*)(0x4001100c);
	*BRR = (104 << 4) | (3 << 0); 	//set baudrate
	*CR1 &= ~(1 << 10);			//disable parity
	*CR1 &= ~(1 << 12);				//set data len as 8bits data

	*CR1 |= (1<<13) | (1 << 2) | (1 << 3);// enable transmiter, receiver, uart enable

#if 0
	//enable RXNE interrupt -> when RXNE is set, UART1 generate interrupt event send to NVIC
	*CR1 |= (1 << 5);

	//NVIC accept interrupt event, which is send from UART1
	uint32_t* ISER1 = (uint32_t*)(0xE000E104);
	*ISER1 |= 1 << (37 - 32);
#else
	//when RXNE is set, send signal to DMA2, DMA2 copy move data to RAM
	uint32_t* CR3 = (uint32_t*)(0x40011014);
	*CR3 |= (1<<6);
#endif
}

void UART1_Send_1byte(char data)
{
	uint32_t* SR = (uint32_t*)(0x40011000);
	uint32_t* DR = (uint32_t*)(0x40011004);
	while(((*SR >> 7) & 1) == 0); 	// wait data empty
	*DR = data;						//write data to DR to UART transfer data (TX: PB6)
	while(((*SR >> 6) & 1) == 0); 	//wait transmitter of UART1 complete transfer
	*SR &= ~(1 << 6);				//Clear TC flash
}

void UART1_Send_String(char* msg)
{
	int msg_len = strlen(msg);
	for(int i = 0; i < msg_len; i++)
	{
		UART1_Send_1byte(msg[i]);
	}
}

char UART1_Recv_1Byte()
{
	uint32_t* SR = (uint32_t*)(0x40011000);
	uint32_t* DR = (uint32_t*)(0x40011004);
	while(((*SR >> 5) & 1) == 0); // wait RXNE flag to read recv data
	char recv_data = *DR;			//read recv data
	return recv_data;
}

int rx_index = 0;
void USART1_IRQHandler()
{
	uint32_t* DR = (uint32_t*)(0x40011004);
	recvData[rx_index++] = *DR;
}

typedef enum
{
	GREEN_LED = 12,
	ORANGE_LED,
	RED_LED,
	BLUE_LED
}led_num_t;

typedef enum
{
	LED_OFF,
	LED_ON
}led_state_t;

void led_controls(led_num_t led, led_state_t state)
{
	uint32_t* GPIOD_ODR = (uint32_t*)(GPIOD_BASE_ARR + 0x14);
	if(state == LED_ON)
		*GPIOD_ODR |= 1 << led;
	else
		*GPIOD_ODR &= ~(1<<led);
}

void uart1_rx_handler()
{
	uint32_t* DR = (uint32_t*)(0x40011004);
	recvData[rx_index++] = *DR;
	if(strstr(recvData, "LED ON") != NULL)
	{
		led_controls(ORANGE_LED, LED_ON);
		rx_index = 0;
		memset(recvData, 0, sizeof(recvData));
	}
	else if(strstr(recvData, "LED OFF") != NULL)
	{
		led_controls(ORANGE_LED, LED_OFF);
		rx_index = 0;
		memset(recvData, 0, sizeof(recvData));
	}
}
#define DMA2_ADDRESS 0x40026400
void dma2_uart1rx_init()
{
	__HAL_RCC_DMA2_CLK_ENABLE();
	//use DMA2 stream 5 channel 4 --> UART1_Rx (DMA mapping tabble)
	uint32_t* DMA_S5CR = (uint32_t*)(DMA2_ADDRESS + 0x10 + 0x18 * 5);
	uint32_t* DMA_S5NDTR = (uint32_t*)(DMA2_ADDRESS + 0x14 + 0x18 * 5);
	uint32_t* DMA_S5PAR = (uint32_t*)(DMA2_ADDRESS + 0x18 + 0x18 * 5);
	uint32_t* DMA_S5M0AR = (uint32_t*)(DMA2_ADDRESS + 0x1c + 0x18 * 5);

    // Ban đầu, thiết lập DMA để nhận lệnh "size = %d"
	*DMA_S5NDTR = sizeof(recvData); // Kích thước buffer cho lệnh kích thước
	*DMA_S5PAR = 0x40011004;       // Địa chỉ USART1_DR (Peripheral Address)
	*DMA_S5M0AR = (uint32_t)recvData; // Địa chỉ buffer nhận (Memory Address)

	*DMA_S5CR &= ~(0b1 << 0); // Vô hiệu hóa DMA Stream trước khi cấu hình lại
	*DMA_S5CR |= (0b100 << 25); // select channel 4 for stream 5
	*DMA_S5CR |= (0b1 << 10); // enable memory incremen mode
	*DMA_S5CR |= (0b1 << 8); // enable circular mode (for continuous reception, if needed for commands)
	*DMA_S5CR |= (0b1 << 4); // enable tranfer complete interrupt
	*DMA_S5CR |= (0b1 << 0); //enable DMA2 stream 5
	
	// Enable DMA2 Stream 5 interrupt in NVIC
	uint32_t* ISER2 = (uint32_t*)(0xE000E108);
	*ISER2 |= 1 << (68-64);

	// Đặt trạng thái ban đầu
	bootloaderState = BOOTLOADER_STATE_RECEIVING_SIZE;
}

void dma2_stream5_handler()
{
	__asm("NOP");
	//clear interrupt flag -> transfer complete interrupt
	uint32_t* HIFCR = (uint32_t*)(DMA2_ADDRESS + 0x0C);
	*HIFCR |= (1 << 11); // Clear Transfer Complete Flag for Stream 5

	switch (bootloaderState) {
		case BOOTLOADER_STATE_RECEIVING_SIZE: {
			int parsed_size = 0;
			// Ensure recvData is null-terminated for sscanf
			recvData[sizeof(recvData) - 1] = '\0'; // Safety null termination

			// Example: "size = 5784"
			if (sscanf((char*)recvData, "size = %d", &parsed_size) == 1) {
				fw_size = (uint32_t)parsed_size;
				if (fw_size > MAX_FW_SIZE) {
					UART1_Send_String("Error: Firmware size too large!\r\n");
					bootloaderState = BOOTLOADER_STATE_IDLE; // Reset state
					memset(recvData, 0, sizeof(recvData)); // Clear buffer
					// Potentially re-enable DMA for size command here if needed
					return;
				}

				// Disable DMA Stream 5
				uint32_t* DMA_S5CR = (uint32_t*)(DMA2_ADDRESS + 0x10 + 0x18 * 5);
				*DMA_S5CR &= ~(0b1 << 0); // Clear EN bit to disable

				// Reconfigure DMA for firmware data reception
				uint32_t* DMA_S5NDTR = (uint32_t*)(DMA2_ADDRESS + 0x14 + 0x18 * 5);
				uint32_t* DMA_S5M0AR = (uint32_t*)(DMA2_ADDRESS + 0x1c + 0x18 * 5);

				*DMA_S5NDTR = fw_size; // Set number of data items to transfer
				*DMA_S5M0AR = (uint32_t)firmware_buffer; // Set memory address for firmware data

				// Disable circular mode for firmware data transfer (one-shot)
				*DMA_S5CR &= ~(0b1 << 8);

				// Re-enable DMA Stream 5
				*DMA_S5CR |= (0b1 << 0); // Set EN bit to enable

				bootloaderState = BOOTLOADER_STATE_RECEIVING_DATA;
				UART1_Send_String("Please send fw data:\r\n");
			} else {
				UART1_Send_String("Error parsing firmware size. Format: size = <num>\r\n");
				bootloaderState = BOOTLOADER_STATE_IDLE; // Reset state
				// Re-enable DMA for size command
				uint32_t* DMA_S5CR = (uint32_t*)(DMA2_ADDRESS + 0x10 + 0x18 * 5);
				*DMA_S5CR &= ~(0b1 << 0); // Disable first
				uint32_t* DMA_S5NDTR = (uint32_t*)(DMA2_ADDRESS + 0x14 + 0x18 * 5);
				*DMA_S5NDTR = sizeof(recvData); // Reset NDTR for size command
				*DMA_S5CR |= (0b1 << 8); // Re-enable circular mode for size command
				*DMA_S5CR |= (0b1 << 0); // Re-enable DMA Stream 5
			}
			memset(recvData, 0, sizeof(recvData)); // Clear buffer after processing
			break;
		}

		case BOOTLOADER_STATE_RECEIVING_DATA: {
			// In this state, the entire firmware data should have been received
			// because DMA was configured for a single transfer of fw_size bytes (normal mode).
			uint32_t* DMA_S5NDTR = (uint32_t*)(DMA2_ADDRESS + 0x14 + 0x18 * 5);
			if (*DMA_S5NDTR == 0) { // Check if transfer is truly complete (NDTR goes to 0 in normal mode)
				UART1_Send_String("rec fw finish\r\n");
				FLASH_Update_Firmware(firmware_buffer, fw_size); // Call the update function
				bootloaderState = BOOTLOADER_STATE_UPDATE_COMPLETE; // This state implies reset is imminent
			} else {
				// This case should ideally not be reached in normal mode.
				// It might indicate an unexpected DMA behavior or incomplete transfer.
				UART1_Send_String("Error: Firmware data incomplete or unexpected DMA interrupt.\r\n");
				bootloaderState = BOOTLOADER_STATE_IDLE; // Reset state
			}
			break;
		}

		case BOOTLOADER_STATE_UPDATE_COMPLETE: {
			// Firmware update already handled, waiting for reset initiated by FLASH_Update_Firmware.
			break;
		}

		case BOOTLOADER_STATE_IDLE: {
			// This state should only be entered at startup or after an error.
			// If an interrupt fires here, it's unexpected, just ignore or re-initialize.
			break;
		}
	}
}

// Define the start address for your application
#define FLASH_APP_START_ADDRESS 0x08008000UL // Địa chỉ bắt đầu của ứng dụng mới trong Flash (thường là Sector 2)

// Hàm cập nhật firmware vào bộ nhớ Flash
void FLASH_Update_Firmware(uint8_t* firmware_data, uint32_t firmware_size) {
    HAL_StatusTypeDef status;
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t SectorError = 0;

    // 1. Mở khóa Flash
    HAL_FLASH_Unlock();

    // 2. Xóa các sector Flash cần thiết
    // Bạn phải xóa các sector mà firmware mới sẽ được ghi vào.
    // Xác định sector bắt đầu và số lượng sector cần xóa dựa trên FLASH_APP_START_ADDRESS và firmware_size.
    // Đối với STM32F411CEU6 (ví dụ), Sector 2 bắt đầu tại 0x08008000 và có kích thước 16KB (0x4000 byte).
    // Điều chỉnh các giá trị này dựa trên sơ đồ bộ nhớ Flash cụ thể của vi điều khiển STM32F4xx của bạn.
    EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3; // Tham khảo datasheet thiết bị để chọn dải điện áp phù hợp

    // Tìm sector bắt đầu cho ứng dụng
    uint32_t startSector = 0;
    // Các sector điển hình của F411:
    // Sector 0: 0x08000000 - 0x08003FFF (16KB)
    // Sector 1: 0x08004000 - 0x08007FFF (16KB)
    // Sector 2: 0x08008000 - 0x0800BFFF (16KB)
    // ...
    // Vì FLASH_APP_START_ADDRESS là 0x08008000, nó là Sector 2.
    if (FLASH_APP_START_ADDRESS == 0x08000000UL) startSector = FLASH_SECTOR_0;
    else if (FLASH_APP_START_ADDRESS == 0x08004000UL) startSector = FLASH_SECTOR_1;
    else if (FLASH_APP_START_ADDRESS == 0x08008000UL) startSector = FLASH_SECTOR_2;
    else {
        // Xử lý địa chỉ bắt đầu không xác định hoặc xác định sector động
        startSector = FLASH_SECTOR_2; // Mặc định là Sector 2 nếu sử dụng 0x08008000
    }
    EraseInitStruct.Sector        = startSector;

    // Tính toán số lượng sector dựa trên kích thước firmware.
    // Giả sử kích thước sector là 16KB (0x4000 byte) cho các sector ban đầu.
    // Việc tính toán này là đơn giản hóa; một bootloader mạnh mẽ sẽ cần xử lý
    // các kích thước sector khác nhau tùy thuộc vào dòng STM32F4.
    uint32_t sector_size_bytes = 16 * 1024; // 16KB
    uint32_t num_sectors_to_erase = (firmware_size + sector_size_bytes - 1) / sector_size_bytes;
    if (num_sectors_to_erase == 0) num_sectors_to_erase = 1; // Đảm bảo xóa ít nhất một sector

    EraseInitStruct.NbSectors = num_sectors_to_erase;

    status = HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError);

    if (status != HAL_OK) {
        // Xử lý lỗi khi xóa Flash
        UART1_Send_String("Lỗi xóa Flash!
");
        HAL_FLASH_Lock();
        return;
    }

    // 3. Ghi dữ liệu vào Flash
    uint32_t current_address = FLASH_APP_START_ADDRESS;
    for (uint32_t i = 0; i < firmware_size; i++) {
        // Ghi từng byte
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, current_address, firmware_data[i]);
        if (status != HAL_OK) {
            // Xử lý lỗi khi ghi Flash
            UART1_Send_String("Lỗi ghi Flash tại địa chỉ 0x");
            char addr_str[10];
            sprintf(addr_str, "%lx", current_address);
            UART1_Send_String(addr_str);
            UART1_Send_String("!
");
            HAL_FLASH_Lock();
            return;
        }
        current_address++;
    }

    // 4. Khóa Flash
    HAL_FLASH_Lock();

    UART1_Send_String("Cập nhật firmware hoàn tất. Đang reset chip...
");

    // 5. Nhảy đến ứng dụng mới
    // Đây là bước quan trọng để bootloader chuyển quyền điều khiển.
    // Bao gồm:
    // a. Khử khởi tạo các thiết bị ngoại vi và vô hiệu hóa ngắt được sử dụng bởi bootloader.
    // b. Đặt con trỏ ngăn xếp (stack pointer) mới.
    // c. Đặt Thanh ghi Offset Bảng Vector (VTOR) trỏ đến địa chỉ cơ sở của bảng vector của ứng dụng mới.
    // d. Nhảy đến bộ xử lý Reset (Reset Handler) của ứng dụng mới.

    // Vô hiệu hóa ngắt
    __disable_irq();

    // Khử khởi tạo tất cả các thiết bị ngoại vi được bật trong bootloader
    // (Đây là ví dụ đơn giản, bạn có thể cần khử khởi tạo các thiết bị ngoại vi cụ thể như UART, DMA, v.v.)
    HAL_RCC_DeInit(); // Reset RCC về cấu hình mặc định
    HAL_DeInit();     // Khử khởi tạo lớp HAL

    // Đặt con trỏ ngăn xếp mới
    // Từ đầu bảng vector của ứng dụng mới, đọc giá trị đầu tiên (Initial Stack Pointer)
    uint32_t newStackPointer = *(__IO uint32_t*)FLASH_APP_START_ADDRESS;
    __set_MSP(newStackPointer); // Hàm CMSIS để đặt Main Stack Pointer

    // Đặt Thanh ghi Offset Bảng Vector (VTOR)
    // Bảng vector cho ứng dụng mới nằm ở FLASH_APP_START_ADDRESS
    SCB->VTOR = FLASH_APP_START_ADDRESS;

    // Lấy địa chỉ của bộ xử lý Reset của ứng dụng mới
    // Từ bảng vector của ứng dụng mới, đọc giá trị thứ hai (Reset Handler)
    uint32_t newProgramCounter = *(__IO uint32_t*)(FLASH_APP_START_ADDRESS + 4);

    // Đảm bảo bit Thumb được đặt (LSB = 1) cho ARM Cortex-M
    newProgramCounter |= 1; 

    // Ép kiểu địa chỉ thành con trỏ hàm và nhảy
    typedef void (*pFunction)(void);
    pFunction JumpToApplication = (pFunction)newProgramCounter;

    // Nhảy đến ứng dụng mới
    JumpToApplication();

    // Mã không bao giờ nên đến đây
    while(1);
}
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
  /* USER CODE BEGIN 2 */
  leds_init();
  exti0_init();
  UART_init();
  dma2_uart1rx_init();
  //copy vector table to RAM (start from 0x20000000)
  //1
//  uint8_t* ram = (uint8_t*)0x20000000;
//  uint8_t* vttb = (uint8_t*)0x00000000;
//  for(int i = 0; i < 0x198; i++){
//	  ram[i] = vttb[i];
//  }

  //2
  memcpy(0x20000000, 0x00000000, 0x198);
  //talk with arm, when interrupt event goto RAM to find vector table
  uint32_t* VTOR = (uint32_t*)0xe000ed08;
  *VTOR = 0x20000000;
  //when vector table in SRAM
  uint32_t* function_address = (uint32_t*) 0x20000058;
  *function_address = (uint32_t) (custom_exti0_handler) | 1;

  function_address = (uint32_t*)0x200000D4;
  *function_address = (uint32_t)(uart1_rx_handler) | 1;

  function_address = (uint32_t*)0x20000150;
  *function_address = (uint32_t)(dma2_stream5_handler) | 1;
  //when vector table in flash memory

  //register function handler address into 0x58
//  uint32_t* function_address = (uint32_t*) 0x58;
//  *function_address = (uint32_t) (custom_exti0_handler) | 1;
  int index = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  // Remove the old LED and UART test code if it's interfering with the bootloader
	  // This section should ideally be empty or contain only high-level bootloader logic
	  // or a jump to application if no update is needed.

	  // Example: If in IDLE state, perhaps blink an LED or wait for commands
	  if(bootloaderState == BOOTLOADER_STATE_IDLE) {
		  led_controls(BLUE_LED, LED_ON);
		  HAL_Delay(500);
		  led_controls(BLUE_LED, LED_OFF);
		  HAL_Delay(500);
	  }
	  // Once firmware is updated, the chip will reset by FLASH_Update_Firmware
	  // So this loop won't be executed anymore until the bootloader restarts.

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
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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
