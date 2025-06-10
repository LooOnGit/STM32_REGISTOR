# STM32 DMA (Direct Memory Access - Truy Cập Bộ Nhớ Trực Tiếp)

DMA được sử dụng để cung cấp khả năng truyền dữ liệu tốc độ cao giữa các ngoại vi và bộ nhớ, cũng như giữa các vùng nhớ với nhau. Dữ liệu có thể được DMA di chuyển nhanh chóng mà không cần CPU phải can thiệp. Điều này giúp giải phóng tài nguyên CPU để thực hiện các tác vụ khác, từ đó tăng hiệu suất tổng thể của hệ thống.

Ví dụ về cách DMA truy cập và chuyển dữ liệu:
```
┌───────────────────┐                ┌─────┐                ┌───────────────────┐
│      UART         │                │     │                │    Biến trong     │
│    Ngoại vi       │ ───────────>   │ DMA │  ───────────> │       RAM        │
│ UART_DR: 0x40013804│                │     │                │ Địa chỉ: 0x20000001│
└───────────────────┘                └─────┘                └───────────────────┘
                                                            Char var[3] = {0}
                                                            ┌─────────┐
                                                            │  Data1  │ var
                                                            ├─────────┤
                                                            │  Data2  │ var + 1
                                                            ├─────────┤
                                                            │  Data3  │ var + 2
                                                            └─────────┘
```

DMA sử dụng hai loại địa chỉ chính:
1. Địa chỉ ngoại vi (Peripheral Address):
   - Địa chỉ cố định của thanh ghi dữ liệu ngoại vi
   - Ví dụ: UART_DR tại địa chỉ 0x40013804

2. Địa chỉ bộ nhớ (Memory Address):
   - Địa chỉ của biến/mảng trong RAM
   - Ví dụ: Mảng var[3] bắt đầu tại địa chỉ 0x20000001
   - Có thể tự động tăng để truy cập các phần tử liên tiếp

Repository này tập trung vào việc tìm hiểu và lập trình DMA trên vi điều khiển STM32.

## Nội dung

1. Tổng quan về DMA
   - DMA là gì?
     * Direct Memory Access - Truy cập bộ nhớ trực tiếp
     * Cho phép chuyển dữ liệu giữa bộ nhớ và ngoại vi mà không cần CPU can thiệp
     * Giải phóng CPU để thực hiện các tác vụ khác trong khi chuyển dữ liệu
     * Tăng hiệu suất hệ thống

   - Cơ chế hoạt động
     * Chuyển dữ liệu từ ngoại vi đến bộ nhớ:
       ```
       ┌──────────────┐      DMA      ┌────────────────────┐
       │    UART      │ ────────────> │ Biến/Mảng trong RAM│
       │  ngoại vi    │               │     (bộ nhớ)       │
       └──────────────┘               └────────────────────┘
       ```
     * Chuyển dữ liệu giữa các vùng nhớ:
       ```
       ┌──────────────┐      DMA      ┌────────────────────┐
       │ Biến trong   │ ────────────> │ Biến/Mảng trong RAM│
       │    RAM       │               │     (bộ nhớ)       │
       └──────────────┘               └────────────────────┘
       ```

   - Cơ chế kích hoạt DMA
     * Kích hoạt từ phần cứng (Hardware trigger):
       - Ngoại vi sẽ tự động tạo yêu cầu DMA
       - Ví dụ: ADC hoàn thành chuyển đổi, UART nhận dữ liệu
     * Kích hoạt từ phần mềm (Software trigger):
       - Lập trình viên chủ động tạo yêu cầu DMA
       - Sử dụng bit MEM2MEM để tạo yêu cầu chuyển dữ liệu

   - Ưu điểm của DMA
     * Giảm tải cho CPU
     * Tốc độ truyền dữ liệu nhanh hơn
     * Tiết kiệm năng lượng
     * Xử lý thời gian thực hiệu quả hơn

2. Kiến trúc DMA trong STM32
   - Bộ điều khiển DMA
     * Nhiều kênh DMA độc lập
     * Mỗi kênh có thể cấu hình riêng
     * Hỗ trợ nhiều chế độ truyền dữ liệu
     * Các mức ưu tiên cho các kênh

   - Stream và Channel
     * Stream: Đường dẫn vật lý để truyền dữ liệu
     * Channel: Kết nối logic giữa ngoại vi và stream
     * Mỗi stream có thể kết nối với nhiều channel khác nhau

3. Các chế độ truyền DMA
   - Bộ nhớ đến Bộ nhớ
     * Chuyển dữ liệu giữa hai vùng nhớ
     * Tốc độ cao nhất trong các chế độ
   
   - Bộ nhớ đến Ngoại vi
     * Chuyển dữ liệu từ bộ nhớ đến ngoại vi
     * Ví dụ: Gửi dữ liệu qua UART
   
   - Ngoại vi đến Bộ nhớ
     * Chuyển dữ liệu từ ngoại vi vào bộ nhớ
     * Ví dụ: Đọc dữ liệu từ ADC

4. Cấu hình DMA
   a. Bật clock cho DMA
      * Sử dụng thanh ghi RCC_AHB1ENR
      * Đặt bit tương ứng với bộ điều khiển DMA

   b. Cấu hình Stream
      * Chọn channel phù hợp với ngoại vi
      * Cấu hình hướng truyền
      * Đặt địa chỉ nguồn và đích
      * Cấu hình kích thước dữ liệu
      * Đặt số lượng dữ liệu cần truyền

   c. Cấu hình Chế độ
      * Chế độ thường: Dừng sau khi hoàn thành
      * Chế độ vòng: Tự động reset và tiếp tục
      * Chế độ bộ đệm kép: Sử dụng hai bộ đệm luân phiên

   d. Cấu hình Ưu tiên
      * Thấp, Trung bình, Cao, Rất cao
      * Quyết định thứ tự ưu tiên giữa các stream

5. Các thanh ghi quan trọng
   - DMA_SxCR: Cấu hình Stream x
   - DMA_SxNDTR: Số lượng dữ liệu Stream x
   - DMA_SxPAR: Địa chỉ ngoại vi Stream x
   - DMA_SxM0AR: Địa chỉ bộ nhớ 0 Stream x
   - DMA_SxFCR: Điều khiển FIFO Stream x

6. Xử lý ngắt DMA
   - Hoàn thành truyền (TC)
   - Truyền một nửa (HT)
   - Lỗi truyền (TE)
   - Lỗi FIFO
   - Lỗi chế độ trực tiếp

7. Ví dụ thực hành
   - DMA với ADC
     * Đọc nhiều kênh ADC liên tục
     * Lưu vào bộ đệm trong RAM
     * Sử dụng chế độ vòng

   - DMA với UART
     * Gửi chuỗi dữ liệu lớn
     * Không chiếm CPU trong quá trình gửi
     * Xử lý ngắt khi hoàn thành

8. Lưu ý quan trọng
   - Đảm bảo căn chỉnh địa chỉ bộ nhớ
   - Tính toán kích thước FIFO phù hợp
   - Xử lý xung đột giữa các stream
   - Xóa cờ sau khi xử lý ngắt
   - Kiểm tra trạng thái trước khi cấu hình lại

9. Sơ đồ khối DMA
   ```
   ┌─────────────────────────────────────────────────────┐
   │                    STM32 DMA                        │
   │                                                     │
   │    Nguồn                Bộ điều khiển       Đích   │
   │  ┌────────┐             ┌──────────────┐  ┌────────┐
   │  │Bộ nhớ/ │  ────────>  │   Stream     │  │Bộ nhớ/ │
   │  │Ngoại vi│    Dữ liệu  │   Channel    │  │Ngoại vi│
   │  └────────┘             │   FIFO       │  └────────┘
   │                         └──────────────┘            │
   │                              │                      │
   │                              │                      │
   │                           Ngắt                      │
   │                              │                      │
   │                              ▼                      │
   │                            NVIC                     │
   └─────────────────────────────────────────────────────┘
   ```

## Tài liệu tham khảo

- [STM32F411xC/E Reference Manual (RM0383)](https://www.st.com/resource/en/reference_manual/dm00119316-stm32f411xc-e-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf)  
  *Ghi chú: Tài liệu chính để tra cứu chi tiết về thanh ghi và cấu hình DMA*

- [STM32F411xC/E Datasheet](https://www.st.com/resource/en/datasheet/stm32f411ce.pdf)  
  *Ghi chú: Chứa thông tin về đặc tính và giới hạn của DMA*

