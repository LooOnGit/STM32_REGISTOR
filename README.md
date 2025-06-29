# 🔧 STM32F411 I2C Programming with Registers

## 📝 Overview
This repository contains examples and documentation for programming the I2C peripheral on STM32F411 microcontroller using direct register manipulation (without HAL library).

## ⚡ Features
- Direct register programming for I2C
- No HAL dependency
- Optimized for STM32F411 microcontroller
- Clear examples and documentation

## 🛠️ Hardware Requirements
- STM32F411 Development Board
- I2C compatible sensors/devices
- Connection wires
- Pull-up resistors (typically 4.7kΩ) for SCL and SDA lines

## 📚 Lý Thuyết I2C (I2C Theory)

### 1. Giới Thiệu
I2C (Inter-Integrated Circuit) là một giao thức truyền thông nối tiếp được phát triển bởi Philips Semiconductor (nay là NXP) vào năm 1982. Đây là một giao thức rất phổ biến và được sử dụng chủ yếu cho truyền thông dữ liệu khoảng cách ngắn.

Đặc điểm nổi bật:
- Sử dụng chỉ 2 dây để truyền thông
- Giao thức đồng bộ master-slave
- Truyền thông hai chiều (bidirectional)
- Chế độ half-duplex (một chiều tại một thời điểm)
- Hỗ trợ nhiều tốc độ truyền khác nhau

### 2. Cấu Trúc Phần Cứng
1. **Đường Truyền**:
   - **SCL (Serial Clock)**: Đường tín hiệu xung nhịp do Master điều khiển
   - **SDA (Serial Data)**: Đường tín hiệu dữ liệu hai chiều
   - Cả hai đường đều là open-drain và cần điện trở pull-up

2. **Các Thiết Bị**:
   - **Master**: Khởi tạo truyền thông và tạo xung clock
   - **Slave**: Đáp ứng khi được Master gọi
   - Có thể có nhiều Master và nhiều Slave trên cùng một bus
   - Mỗi Slave có một địa chỉ duy nhất

### 3. Cấu Trúc Frame và Timing

#### 3.1 Cấu Trúc Frame Cơ Bản
```
|--START--|--Slave Address--|--R/W--|--ACK--|----Data----|--ACK--|--STOP--|
   1 bit      7 bits         1 bit   1 bit    8 bits     1 bit    1 bit
```

#### 3.2 Chi Tiết Từng Phần

1. **Điều Kiện START (S)**
   - SDA chuyển từ HIGH → LOW khi SCL = HIGH
   - Đánh dấu bắt đầu truyền thông
   - Master có quyền điều khiển độc quyền bus sau START

2. **Địa Chỉ Slave (7 bit)**
   - Truyền từ MSB → LSB
   - Bit 7 (MSB) được gửi đầu tiên
   - Ví dụ địa chỉ 0x48 = 0b1001000:
     ```
     Bit 7 → Bit 6 → Bit 5 → Bit 4 → Bit 3 → Bit 2 → Bit 1
       1   →   0   →   0   →   1   →   0   →   0   →   0
     ```

3. **Bit Read/Write (R/W)**
   - Bit thứ 8 sau địa chỉ
   - 0: Master → Slave (Write)
   - 1: Slave → Master (Read)

4. **Bit ACK/NACK**
   - Sau mỗi byte (8 bit)
   - Do bên nhận gửi (Receiver)
   - ACK = 0: Xác nhận đã nhận
   - NACK = 1: Không xác nhận/Kết thúc đọc

5. **Byte Dữ Liệu (8 bit)**
   - Truyền từ MSB → LSB
   - SDA chỉ được thay đổi khi SCL = LOW
   - SDA phải giữ ổn định khi SCL = HIGH
   - Có thể truyền nhiều byte liên tiếp

6. **Điều Kiện STOP (P)**
   - SDA chuyển từ LOW → HIGH khi SCL = HIGH
   - Giải phóng bus
   - Cho phép các thiết bị khác sử dụng bus

#### 3.3 Các Kiểu Frame Phổ Biến

1. **Frame Ghi Đơn Giản**
```
START → [A6:A0] → W → ACK → [D7:D0] → ACK → STOP
  S   →   0x48  → 0 → ACK →  0xFF   → ACK →   P
```

2. **Frame Đọc Đơn Giản**
```
START → [A6:A0] → R → ACK → [D7:D0] → NACK → STOP
  S   →   0x48  → 1 → ACK →  Data   → NACK →   P
```

3. **Frame Ghi Thanh Ghi**
```
START → [A6:A0] → W → ACK → [REG] → ACK → [DATA] → ACK → STOP
  S   →   0x48  → 0 → ACK → 0x00  → ACK → 0xFF  → ACK →   P
```

4. **Frame Đọc Thanh Ghi**
```
START → [A6:A0] → W → ACK → [REG] → ACK → START → [A6:A0] → R → ACK → [DATA] → NACK → STOP
  S   →   0x48  → 0 → ACK → 0x00  → ACK →   S   →   0x48  → 1 → ACK →  Data  → NACK →   P
```

#### 3.4 Quy Tắc Timing Quan Trọng

1. **Thời Gian Setup/Hold**
   - tSU;STA: Thời gian setup cho START (0.6µs @ 100kHz)
   - tHD;STA: Thời gian hold cho START (0.6µs @ 100kHz)
   - tSU;DAT: Thời gian setup cho DATA (0.1µs @ 100kHz)
   - tHD;DAT: Thời gian hold cho DATA (0µs @ 100kHz)
   - tSU;STO: Thời gian setup cho STOP (0.6µs @ 100kHz)

2. **Thời Gian SCL**
   - tLOW: Thời gian SCL ở mức thấp (4.7µs @ 100kHz)
   - tHIGH: Thời gian SCL ở mức cao (4.0µs @ 100kHz)
   - tBUF: Thời gian bus free giữa STOP và START (4.7µs @ 100kHz)

### 4. Đặc Điểm Chính
- **Tốc độ truyền**:
  - Chế độ chuẩn: 100kHz
  - Chế độ nhanh: 400kHz
  - Chế độ nhanh plus: 1MHz
  
- **Cấu trúc Master-Slave**:
  - Master: Điều khiển đường SCL, khởi tạo và kết thúc truyền thông
  - Slave: Đáp ứng theo yêu cầu của Master
  - Hỗ trợ nhiều Master (Multi-master)
  - Cả Master và Slave đều có thể gửi/nhận dữ liệu

- **Địa chỉ**:
  - Địa chỉ 7-bit hoặc 10-bit
  - Có thể kết nối tới 128 thiết bị (7-bit) hoặc 1024 thiết bị (10-bit)

### 5. Ví Dụ Thực Tế: Đọc Cảm Biến MPU6050

1. **Khởi Tạo Đọc**
```
START → 0x68 → W → ACK → 0x3B → ACK → START → 0x68 → R → ACK
```

2. **Đọc Dữ Liệu**
```
[ACCEL_X_H] → ACK → [ACCEL_X_L] → ACK → [ACCEL_Y_H] → ACK → [ACCEL_Y_L] → ACK → [ACCEL_Z_H] → ACK → [ACCEL_Z_L] → NACK → STOP
```

### 6. Các Lỗi Thường Gặp và Cách Khắc Phục
1. **Bus Error (BERR)**:
   - Điều kiện START hoặc STOP không hợp lệ
   - Nhiễu trên đường truyền
   - **Khắc phục**: 
     - Kiểm tra điện trở pull-up
     - Giảm chiều dài dây
     - Tăng cường chống nhiễu

2. **Arbitration Lost (ARLO)**:
   - Xảy ra trong hệ thống multi-master
   - Master mất quyền điều khiển bus
   - **Khắc phục**:
     - Kiểm tra xung đột địa chỉ
     - Cài đặt ưu tiên cho các master

3. **Acknowledge Failure (AF)**:
   - Slave không phản hồi ACK
   - Địa chỉ hoặc dữ liệu không hợp lệ
   - **Khắc phục**:
     - Kiểm tra địa chỉ slave
     - Kiểm tra nguồn cấp cho slave
     - Xác nhận slave sẵn sàng

### 7. Yêu Cầu Phần Cứng và Tính Toán
1. **Điện trở Pull-up**:
   - Cần có điện trở pull-up trên cả 2 đường SDA và SCL
   - Công thức tính:
     ```
     Rp(min) = (Vdd - Vol_max) / Iol
     Rp(max) = tr / (0.8473 × Cb)
     ```
     Trong đó:
     - Vdd: Điện áp nguồn
     - Vol_max: Điện áp mức thấp tối đa
     - Iol: Dòng điện sink
     - tr: Thời gian rise time
     - Cb: Điện dung bus

2. **Kết nối vật lý**:
   - Dạng "Wired-AND"
   - Open-drain hoặc open-collector
   - Điện áp hoạt động thông thường: 3.3V hoặc 5V
   - Chiều dài bus tối đa phụ thuộc vào điện dung bus (400pF max)

## �� Pin Configuration
| Pin Name | Pin Number | Function |
|----------|------------|----------|
| SCL1     | PB6       | I2C1 Clock |
| SDA1     | PB7       | I2C1 Data |
| SCL2     | PB10      | I2C2 Clock |
| SDA2     | PB3       | I2C2 Data |

## 💻 Register Configuration Steps

### 1. Enable Clock Access
```c
// Enable I2C1 clock
RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

// Enable GPIOB clock
RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
```

### 2. Configure GPIO Pins
```c
// Configure PB6 (SCL) and PB7 (SDA) as alternate function
GPIOB->MODER &= ~(GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0);
GPIOB->MODER |= (GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1);

// Set alternate function to AF4 (I2C)
GPIOB->AFR[0] |= (4 << GPIO_AFRL_AFRL6_Pos) | (4 << GPIO_AFRL_AFRL7_Pos);

// Configure as open drain
GPIOB->OTYPER |= (GPIO_OTYPER_OT6 | GPIO_OTYPER_OT7);

// Enable pull-up
GPIOB->PUPDR |= (GPIO_PUPDR_PUPDR6_0 | GPIO_PUPDR_PUPDR7_0);
```

### 3. Configure I2C Parameters
```c
// Reset I2C
I2C1->CR1 |= I2C_CR1_SWRST;
I2C1->CR1 &= ~I2C_CR1_SWRST;

// Set I2C clock frequency
I2C1->CR2 = (42 << I2C_CR2_FREQ_Pos); // APB1 clock = 42MHz

// Configure CCR for 100KHz
I2C1->CCR = 210; // CCR = Fpclk1/(2 * Fi2c) = 42MHz/(2 * 100KHz)

// Configure TRISE
I2C1->TRISE = 43; // TRISE = (Fpclk1 * Trise) + 1 = (42MHz * 1000ns) + 1

// Enable I2C
I2C1->CR1 |= I2C_CR1_PE;
```

## 📚 Basic I2C Operations

### Write Operation
```c
void I2C_Write(uint8_t address, uint8_t data) {
    // Wait until I2C is not busy
    while(I2C1->SR2 & I2C_SR2_BUSY);
    
    // Generate START condition
    I2C1->CR1 |= I2C_CR1_START;
    
    // Wait for START condition to be generated
    while(!(I2C1->SR1 & I2C_SR1_SB));
    
    // Send slave address
    I2C1->DR = address << 1;
    
    // Wait for address to be sent
    while(!(I2C1->SR1 & I2C_SR1_ADDR));
    
    // Clear ADDR flag by reading SR2
    (void)I2C1->SR2;
    
    // Send data
    I2C1->DR = data;
    
    // Wait for data to be sent
    while(!(I2C1->SR1 & I2C_SR1_BTF));
    
    // Generate STOP condition
    I2C1->CR1 |= I2C_CR1_STOP;
}
```

### Read Operation
```c
uint8_t I2C_Read(uint8_t address) {
    uint8_t data;
    
    // Wait until I2C is not busy
    while(I2C1->SR2 & I2C_SR2_BUSY);
    
    // Enable ACK
    I2C1->CR1 |= I2C_CR1_ACK;
    
    // Generate START condition
    I2C1->CR1 |= I2C_CR1_START;
    
    // Wait for START condition to be generated
    while(!(I2C1->SR1 & I2C_SR1_SB));
    
    // Send slave address with read bit
    I2C1->DR = (address << 1) | 1;
    
    // Wait for address to be sent
    while(!(I2C1->SR1 & I2C_SR1_ADDR));
    
    // Clear ADDR flag by reading SR2
    (void)I2C1->SR2;
    
    // Disable ACK
    I2C1->CR1 &= ~I2C_CR1_ACK;
    
    // Generate STOP condition
    I2C1->CR1 |= I2C_CR1_STOP;
    
    // Wait for data to be received
    while(!(I2C1->SR1 & I2C_SR1_RXNE));
    
    // Read data
    data = I2C1->DR;
    
    return data;
}
```

## ⚠️ Common Issues and Solutions
1. **Bus Error (BERR)**
   - Check pull-up resistors on SCL and SDA lines
   - Verify correct pin configuration
   - Check for short circuits

2. **Acknowledge Failure (AF)**
   - Verify slave address is correct
   - Check if slave device is powered and connected
   - Ensure proper timing configuration

3. **Arbitration Lost (ARLO)**
   - Check for multiple masters on the bus
   - Verify proper pull-up resistor values

## 📖 References
- [STM32F411 Reference Manual](https://www.st.com/resource/en/reference_manual/rm0383-stm32f411xce-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [I2C Specification](https://www.nxp.com/docs/en/user-guide/UM10204.pdf)

## 🤝 Contributing
Feel free to contribute to this project by:
- Reporting bugs
- Suggesting enhancements
- Creating pull requests
- Sharing your experience

## 📄 License
This project is licensed under the MIT License - see the LICENSE file for details.
