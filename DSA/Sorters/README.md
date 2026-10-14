# Quản lý xét học bổng

Chương trình đọc danh sách sinh viên từ tệp CSV, kiểm tra dữ liệu, lọc
sinh viên đủ điều kiện học bổng, sắp xếp danh sách và xuất các báo cáo
kết quả.

## Biên dịch

Đứng trong thư mục `Sorters` và chạy:

```bash
gcc -std=c11 -Wall -Wextra -O2 -o hocbong main.c
```

Chương trình được biên dịch từ `main.c`. Tệp này đưa mã nguồn của
`student.c` và `sorts.c` vào cùng một đơn vị biên dịch, vì vậy không cần
các tệp tiêu đề `.h`.

## Chạy chương trình

Có thể truyền đường dẫn đến tệp CSV khi chạy:

```bash
./hocbong students.csv
```

Nếu không truyền đường dẫn, chương trình sử dụng `students.csv` theo mặc
định. Đường dẫn cũng có thể được nhập hoặc thay đổi tại mục 1 trong menu.

## Các chức năng trong menu

1. Đọc tệp CSV và kiểm tra dữ liệu.
2. Lọc sinh viên đủ điều kiện học bổng.
3. Chọn tiêu chí xếp hạng:
   - Xếp hạng chính: GPA giảm dần, số tín chỉ giảm dần, mã sinh viên
     tăng dần.
   - Chỉ xếp theo GPA giảm dần để kiểm tra tính ổn định.
4. Xếp hạng bằng một trong ba thuật toán:
   - Insertion Sort.
   - Selection Sort.
   - Bubble Sort.
5. Hiển thị danh sách Top K và thống kê.
6. Ghi các tệp kết quả.
7. So sánh cả ba thuật toán trên cùng một dữ liệu.
0. Thoát chương trình.

## Điều kiện học bổng

Một sinh viên đủ điều kiện khi:

- GPA từ `3.20` trở lên.
- Có ít nhất `24` tín chỉ.
- Không vi phạm kỷ luật.

## Các tệp nguồn

- `main.c`: Menu, trạng thái chương trình và điểm bắt đầu thực thi.
- `student.c`: Khai báo cấu trúc dữ liệu, đọc và kiểm tra CSV, lọc sinh
  viên, ghi tệp kết quả.
- `sorts.c`: Các hàm so sánh, ba thuật toán sắp xếp và các hàm kiểm tra
  thứ tự, tính ổn định.

## Tệp dữ liệu mẫu

- `students.csv`: Dữ liệu mẫu gồm 8 dòng.
- `students_errors.csv`: Dữ liệu dùng để kiểm tra các trường hợp lỗi khi
  đọc CSV.
- `students_stability.csv`: Dữ liệu dùng để kiểm tra tính ổn định của
  Selection Sort khi chỉ xếp theo GPA.

## Các tệp kết quả

Khi chọn chức năng ghi tệp, chương trình tạo:

- `valid.csv`: Danh sách sinh viên đủ điều kiện.
- `rejected.csv`: Các bản ghi bị loại do dữ liệu không hợp lệ hoặc không
  đủ điều kiện.
- `ranked.csv`: Danh sách sinh viên đủ điều kiện sau khi xếp hạng.
- `summary.txt`: Báo cáo tổng hợp quá trình xét học bổng.

## Quy ước thống kê

Mỗi phép gán một phần tử được tính là một lần di chuyển. Một phép hoán
đổi gồm ba phép gán nên được tính là ba lần di chuyển.
