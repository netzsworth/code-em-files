# Quản lý bệnh nhân phòng khám

## Mô tả

Chương trình mô phỏng một hệ thống quản lý bệnh nhân đơn giản bằng ngôn ngữ C, sử dụng giao diện dòng lệnh (TUI). Người dùng thao tác với chương trình thông qua menu `switch-case`.

Chương trình sử dụng:

- Mảng có sức chứa tối đa 100 bệnh nhân để lưu danh sách đã đăng ký.
- Sắp xếp chèn để sắp xếp bệnh nhân theo mức độ ưu tiên.
- Tìm kiếm nhị phân để tìm bệnh nhân theo mã.
- Hàng đợi cài đặt bằng mảng để mô phỏng thứ tự tiếp nhận tại quầy.
- Không sử dụng cấp phát bộ nhớ động.

## Nhập dữ liệu

Khi khởi động, danh sách bệnh nhân rỗng. Chọn chức năng `1` để thêm bệnh nhân và nhập:

- Mã bệnh nhân: số nguyên, không được trùng.
- Tên bệnh nhân: không được để trống, tối đa 39 ký tự.
- Mức ưu tiên: số nhỏ hơn có mức ưu tiên cao hơn.

## Menu chức năng

```text
1. Them benh nhan
2. Hien thi danh sach
3. Sap xep theo uu tien
4. Tim benh nhan theo ma
5. Tao hang doi theo uu tien
6. Tiep nhan benh nhan tiep theo
7. Hien thi hang doi
0. Thoat
```

- `1`: Thêm bệnh nhân mới vào danh sách.
- `2`: Hiển thị danh sách hiện tại.
- `3`: Sắp xếp danh sách theo mức ưu tiên, sau đó theo mã.
- `4`: Tạo bản sao, sắp xếp bản sao theo mã và tìm kiếm nhị phân.
- `5`: Tạo lại hàng đợi theo thứ tự ưu tiên.
- `6`: Lấy bệnh nhân đầu hàng đợi để tiếp nhận.
- `7`: Hiển thị hàng đợi theo thứ tự FIFO.
- `0`: Thoát chương trình.

## Quy tắc sắp xếp

Danh sách được sắp xếp theo các quy tắc sau:

1. Mức độ ưu tiên tăng dần.
2. Nếu hai bệnh nhân có cùng mức ưu tiên, mã bệnh nhân tăng dần.

Ví dụ, nếu nhập các mã `105`, `102`, `108`, `101`, `106` với dữ liệu như bài tập ban đầu, thứ tự sau khi sắp xếp là:

```text
102, 106, 101, 105, 108
```

## Chức năng chính

### Sắp xếp theo mức độ ưu tiên

Sử dụng Insertion Sort. Khi hai bệnh nhân có cùng mức ưu tiên, chương trình so sánh tiếp mã bệnh nhân.

### Tìm kiếm theo mã

Chương trình tạo một bản sao của danh sách, sắp xếp bản sao theo mã tăng dần, sau đó sử dụng Binary Search để tìm bệnh nhân. Nếu không tìm thấy, chương trình in `KHONG TIM THAY`.

Binary Search phải được thực hiện trên danh sách đã sắp xếp theo mã, không phải danh sách đã sắp xếp theo ưu tiên.

### Hàng đợi bệnh nhân

Khi chọn chức năng `5`, tất cả bệnh nhân được sao chép và sắp xếp theo ưu tiên trước khi đưa vào hàng đợi. Chức năng `6` lấy bệnh nhân ở đầu hàng, còn chức năng `7` hiển thị từ `front` đến `rear`.

## Độ phức tạp

- Insertion Sort: `O(n^2)` trong trường hợp xấu nhất, khi mảng ban đầu được sắp xếp theo thứ tự ngược.
- Binary Search: `O(log n)` trong trường hợp xấu nhất, vì sau mỗi bước phạm vi tìm kiếm giảm khoảng một nửa.
- Enqueue: `O(1)`.
- Dequeue: `O(1)`.

## Biên dịch và chạy

Đứng tại thư mục gốc của kho mã nguồn, chạy:

```bash
gcc -std=c99 -Wall -Wextra -pedantic \
    DSA/ClinicManagement/clinic_management.c \
    -o clinic_management

./clinic_management
```