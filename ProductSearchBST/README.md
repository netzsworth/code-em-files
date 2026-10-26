QUAN LY CATALOG SAN PHAM BANG CAY BST

Chuong trinh quan ly catalog san pham bang cay tim kiem nhi phan (BST),
voi ma san pham (maSP) lam khoa.

YEU CAU
- GCC hoac Clang.
- Trinh bien dich ho tro chuan C11.

BIEN DICH

gcc -std=c11 -Wall -o bst main.c

CHAY CHUONG TRINH
- Linux/macOS: ./bst
- Windows: bst.exe

HUONG DAN MENU
- 1: Them mot san pham sau khi nhap ma, ten, so luong va don gia.
- 2: Tim san pham theo maSP va in thong tin san pham.
- 3: Xoa san pham theo maSP.
- 4: In tat ca san pham theo thu tu tang dan cua maSP.
- 5: Dem so nut va in chieu cao cay.
- 6: Xoa cay hien tai va nap 7 san pham mau theo thu tu yeu cau.
- 0: Thoat chuong trinh.

KIEM THU THU CONG
1. Chon 6 de nap du lieu mau.
2. Chon 4. Danh sach phai co maSP theo thu tu: 20, 30, 40, 50, 60, 70, 80.
3. Chon 2, nhap 60. Ket qua co ten "Sổ tay", so luong 25 va don gia 25000.
4. Chon 2, nhap 99. Ket qua phai la "Không tìm thấy sản phẩm".
5. Chon 1 va nhap lai maSP 50. Ket qua phai la "Mã sản phẩm đã tồn tại".
6. Chon 5. Ket qua phai la 7 nut va chieu cao 3.
7. De kiem tra xoa la, chon 6, chon 3, nhap 20, sau do chon 4.
8. De kiem tra xoa nut co mot con, chon 6, xoa 20, xoa 30, sau do chon 4.
   Nut 40 phai con trong cay.
9. De kiem tra xoa nut co hai con, chon 6, xoa 70, sau do chon 4.
   Danh sach van phai tang dan.

GHI CHU
- Cay BST nay khong tu can bang, nen hieu nang phu thuoc vao thu tu chen.
- Ma san pham phai la so nguyen duong va khong duoc trung.
- So luong va don gia khong duoc am; ten san pham khong duoc rong.
