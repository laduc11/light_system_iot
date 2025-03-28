# Smart light system with ESP32 WROOM and LoRa
## Build Step
The programe has 2 modes include GATEWAY and NODE. This modo will be choosen at the compile time.
### 1. Gateway mode
Open `platformio.ini` and find field `build_flags`. Add the command `-DUSING_GATEWAY_MODE` like below
```ini
build_flags = 
  -DCORE_DEBUG_LEVEL=0
  -DUSING_GATEWAY_MODE
```

### 2. Node mode
Open `platformio.ini` and find field `build_flags`. Add the command `-DUSING_NODE_MODE` like below
```ini
build_flags = 
  -DCORE_DEBUG_LEVEL=0
  -DUSING_NODE_MODE
```

# Những việc cần làm trong tuần tới
1. Hiện thực cơ chế ACK
2. Hiện thực cơ chế Scan
3. Gửi gói tin giá trị cảm biến định kỳ đến gateway có ACK (hiện tại đang không có)
4. Định nghĩa 1 danh sách các thiết bị trên trên server (hiện tại đang danh sách 2 thiết bị) -> Cần cụ thể các thiết bị được lưu trữ thế nào? Có bao nhiêu trường dữ liệu? Được liên kết với nhau ra sao? -> Làm rõ lại những gì đã có thôi
5. Thiết kế lại Dashboard để trực quan hóa dữ liệu tốt hơn -> Có thể sử dụng các biểu đồ thống kê giá trị từ DHT20 
6. Tìm hiểu cơ chế thêm **device** bằng API. Luồng thực thi kỳ vọng: 
   1. Node gửi gói tin đăng ký (broadcast)
   2. Gateway nhận được gói tin đăng ký
   3. Gateway gửi lệnh đăng ký **device** mới lên **coreIOT** và chờ xác nhận.
   4. Nếu thành công đăng ký lên **coreIOT** -> Gửi lại gói tin xác nhận cho node kèm địa chỉ Gateway. Nếu không thành công hoặc timeout -> Gửi lại yêu cầu. Đề xuất:
      1. Số lần Retry: 5 lần
      2. Timeout: 2 sec

Note: Ở task 6 thì chỉ làm vào các bước tương tác với server, các bước khác có thể giả lập tín 
