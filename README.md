# Lập Trình UART Sử Dụng Thanh Ghi Trên STM32

## Tổng Quan
Repository này chứa các ví dụ và tài liệu về lập trình UART (Bộ Thu Phát Không Đồng Bộ Vạn Năng) bằng cách điều khiển trực tiếp các thanh ghi trên vi điều khiển STM32.

## UART là gì?
- UART là một hardware hoặc ngoại vi được tích hợp trong MCU (Micro Controller Unit).
- Asynchronous Rx/Tx: được gọi là truyền thông bất đồng bộ, vì clock hoạt động của bộ UART của thiết bị truyền và thiết bị nhận có thể khác nhau. Do đó không phân biệt chủ - tớ (master - slave) point to point. Các hệ thống là ngang cấp trong chuẩn truyền UART.
- Đối với truyền dữ liệu một hệ thống sử dụng 1 mạch đạo động riêng - 2 hệ thống 2 mạch đạo động độc lập nhưng phải cùng tần số.

### Sơ Đồ Kết Nối UART Giữa 2 MCU
```
MCU1                              MCU2
┌──────────┐                    ┌──────────┐
│          │        TX─────►RX  │          │
│  UART    │                    │   UART   │
│          │        RX◄─────TX  │          │
│          │                    │          │
└──────────┘                    └──────────┘
    GND ────────────────────────── GND
```

### Tại Sao Cần Chung Chân GND?
1. Điện áp tham chiếu:
   - UART truyền dữ liệu bằng các mức điện áp (logic 0 và 1)
   - Để xác định chính xác mức logic, các thiết bị cần có cùng một điểm tham chiếu (ground)
   - Nếu không có ground chung, mỗi thiết bị sẽ có điểm tham chiếu riêng, dẫn đến đọc sai mức logic

2. Đảm bảo truyền dữ liệu chính xác:
   - Khi một thiết bị gửi logic 1 (ví dụ 3.3V), thiết bị nhận phải đo được điện áp này so với ground
   - Nếu ground khác nhau, điện áp đo được có thể khác với điện áp thực tế, gây ra lỗi truyền

3. Tránh dòng điện không mong muốn:
   - Nếu không có ground chung, có thể xuất hiện dòng điện không mong muốn giữa các thiết bị
   - Điều này có thể gây hỏng cổng UART hoặc thậm chí là toàn bộ thiết bị

## Mục Lục
1. [Giới Thiệu về UART](#giới-thiệu-về-uart)
2. [Các Thanh Ghi UART](#các-thanh-ghi-uart)
3. [Các Bước Cấu Hình](#các-bước-cấu-hình)
4. [Code Mẫu](#code-mẫu)

## Giới Thiệu về UART
UART là giao thức truyền thông phần cứng cho phép giao tiếp nối tiếp giữa các thiết bị. Các đặc điểm chính bao gồm:
- Truyền thông không đồng bộ
- Hoạt động song công đầy đủ (full-duplex)
- Tốc độ truyền (baud rate) có thể cấu hình. Tốc độ tuyền là khoảng thời gian 1 bit được truyền đi.
- Bit bắt đầu và bit kết thúc để đồng bộ hóa khung dữ liệu

## Cấu Trúc Khung Dữ liệu UART (Data Framing)
Một khung dữ liệu UART bao gồm các thành phần sau:

```
┌──────┬──────────────┬──────────┬─────────┐
│Start │    Data      │ Parity   │  Stop   │
│ bit  │  (7-8 bits)  │   bit    │   bit   │
│  0   │   LSB→MSB    │(optional)│    1    │
└──────┴──────────────┴──────────┴─────────┘

1    0    1    0    0    0    0    1    1    0    1    1
┐    ┌────┐         ┌────┐    ┌────────┐         ┌────┐
│    │    │         │    │    │        │         │    │
└────┘    └─────────┘    └────┘        └─────────┘    └──
idle start         Data                              P  stop
```

Trong đó:
- idle: Trạng thái chờ (mức cao) trạng thái mà bật lên rồi nhưng chưa truyền dữ liệu.
- start: Bit bắt đầu (mức thấp)
- Data: Dữ liệu cần truyền (LSB truyền trước)
- P: Parity bit (bit kiểm tra chẵn lẻ - tùy chọn)
- stop: Bit kết thúc (mức cao)

### Start bit
- Là bit đầu tiên được truyền trong 1 Frame
- Báo hiệu cho thiết bị nhận có một gói dữ liệu sắp được truyền đến
- Luôn có giá trị 0 (đây là bit mặc định)

### Data bits
- Dữ liệu cần truyền, thường là 7 hoặc 8 bit
- Bit có trọng số nhỏ nhất LSB (least significant bit) được truyền trước
- Sau đó đến MSB (most significant bit)

### Parity bit (tùy chọn)
- Dùng để kiểm tra dữ liệu truyền có đúng không
- Kiểm tra chẵn (even): đếm số bit 1 trong Data - Nếu là số chẵn thì 1, số lẻ là 0
- Kiểm tra lẻ (odd): đếm số bit 1 trong Data - Nếu là số chẵn thì 0, số lẻ là 1

### Stop bit
- Là 1 hoặc các bit báo cho thiết bị rằng các bit dữ liệu đã gửi xong
- Thiết bị nhận sẽ tiến hành kiểm tra khung truyền nhằm đảm bảo tính đúng đắn của dữ liệu
- Bit Stop có giá trị 1 (đây là bit mặc định)

Lưu ý khi làm việc với UART:
- Baud Rate (tốc độ truyền) phải được cài đặt giống nhau ở cả phần gửi và nhận
- Độ dài dữ liệu cần truyền (7 hoặc 8 bit) phải được cấu hình đồng nhất
- Có thể sử dụng hoặc không sử dụng bit kiểm tra chẵn lẻ tùy theo yêu cầu

## Cấu Trúc Khung Dữ Liệu UART (Data Framing)
Các thanh ghi chính để cấu hình UART:

### 1. Thanh Ghi Điều Khiển 1 (USART_CR1)
- Bit 13: Bật USART (UE)
- Bit 3: Bật Bộ Phát (TE)
- Bit 2: Bật Bộ Thu (RE)
- Bits 15:14: Chế độ lấy mẫu quá mức

### 2. Thanh Ghi Tốc Độ Truyền (USART_BRR)
- Dùng để cấu hình tốc độ truyền
- Công thức: BRR = fCK/(16 * Tốc độ Baud)

### 3. Thanh Ghi Trạng Thái (USART_SR)
- Bit 7: TXE (Thanh ghi truyền dữ liệu trống)
- Bit 5: RXNE (Thanh ghi đọc dữ liệu không trống)

## Các Bước Cấu Hình
1. Bật clock cho GPIO
2. Bật clock cho UART
3. Cấu hình chân GPIO cho UART
   - Chân TX là Alternate Function Push-Pull
   - Chân RX là Input Floating
4. Cấu hình thông số UART:
   - Tốc độ truyền
   - Độ dài từ
   - Số bit dừng
   - Kiểm tra chẵn lẻ
   - Chế độ (TX/RX)
5. Bật UART

## Code Mẫu
Khởi tạo UART cơ bản sử dụng thanh ghi:

```c
// Bật clock cho GPIOA
RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

// Bật clock cho UART2
RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

// Cấu hình PA2 (TX) và PA3 (RX) cho UART
GPIOA->MODER &= ~(GPIO_MODER_MODER2_Msk | GPIO_MODER_MODER3_Msk);
GPIOA->MODER |= (GPIO_MODER_MODER2_1 | GPIO_MODER_MODER3_1);  // Chế độ hàm thay thế

// Cấu hình thanh ghi hàm thay thế
GPIOA->AFR[0] |= (7 << GPIO_AFRL_AFRL2_Pos) | (7 << GPIO_AFRL_AFRL3_Pos);

// Cấu hình UART
USART2->BRR = SystemCoreClock/16/9600;  // Tốc độ baud 9600
USART2->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;  // Bật TX, RX và UART
```

### Gửi Dữ Liệu
```c
void UART_GuiKyTu(char c) {
    while(!(USART2->SR & USART_SR_TXE));  // Đợi cho đến khi bộ đệm TX trống
    USART2->DR = c;
}
```

### Nhận Dữ Liệu
```c
char UART_NhanKyTu(void) {
    while(!(USART2->SR & USART_SR_RXNE));  // Đợi cho đến khi nhận được dữ liệu
    return USART2->DR;
}
```