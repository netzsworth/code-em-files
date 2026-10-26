# Bài 5 - Exercise 1: Tra cứu sản phẩm

## Biên dịch và chạy

```bash
gcc -std=c11 -Wall -Wextra -pedantic -O2 -o product_search product_search.c
./product_search san_pham.txt SP003
```

Vị trí được in theo chỉ số bắt đầu từ `0`. Mỗi lần gọi `strcmp` giữa mã sản
phẩm trong mảng và khóa truy vấn được tính là một phép so sánh khóa.
Chương trình dùng mảng tĩnh và nhận tối đa `1000` sản phẩm.

## Kiểm thử bắt buộc

Chạy các lệnh sau để bao phủ phần tử đầu, phần tử cuối, khóa ở giữa, khóa
không tồn tại và tệp chỉ có một bản ghi:

```bash
./product_search san_pham.txt SP003
./product_search san_pham.txt SP004
./product_search san_pham.txt SP999
./product_search san_pham.txt SP001
./product_search san_pham_mot_ban_ghi.txt SP001
```

## Độ phức tạp

- Tìm tuần tự: tốt nhất `O(1)`, trung bình `O(N)`, xấu nhất `O(N)`; số phép
  so sánh lần lượt phụ thuộc vào vị trí khóa hoặc bằng `N` nếu không tìm thấy.
- Sắp xếp chèn bản sao: tốt nhất `O(N)`, trung bình và xấu nhất `O(N^2)`.
  Đây là chi phí tiền xử lý trước khi tìm nhị phân.
- Tìm nhị phân: tốt nhất `O(1)`, trung bình và xấu nhất `O(log N)`; mỗi vòng
  lặp thực hiện một phép so sánh khóa.

## Báo cáo

Khi báo cáo, ghi số phép so sánh từ kết quả chạy của ít nhất bốn khóa ở trên.
Lưu ý rằng tìm nhị phân có thêm chi phí sắp xếp một lần, còn tìm tuần tự dùng
ngay mảng gốc chưa sắp xếp.