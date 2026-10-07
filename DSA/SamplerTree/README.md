# SamplerTree

Đây là một chương trình C đơn giản để nhập và lưu trữ thông tin cây nhị phân bằng mảng.

Mỗi nút có các thông tin sau:

- `data`: số nguyên
- `content`: chuỗi ký tự
- `left`: chỉ số nút con trái
- `right`: chỉ số nút con phải
- `isLeaf`: cho biết nút có phải là nút lá hay không

## Cấu trúc chính

Trong file có một cấu trúc `Node` và một mảng `nodes[100]`:

```c
struct Node {
    int data;
    char content[100];
    int left, right;
    int isLeaf;
};
Node nodes[100];
```

Tức là cây không được tạo bằng con trỏ, mà được lưu trong mảng theo kiểu index-based.

## Chức năng của chương trình

### `init()`
Khởi tạo tất cả các phần tử của mảng `nodes`:

- `data = 0`
- `content = ""`
- `left = -1`
- `right = -1`
- `isLeaf = 0`

### `input()`
Nhập dữ liệu cho từng nút từ người dùng:

- số lượng nút
- dữ liệu của từng nút
- nội dung chuỗi cho từng nút
- có phải nút lá không
- nếu không phải nút lá thì nhập chỉ số của nút trái và phải

### `loadSamplerTree()`
Gọi `init()` rồi `input()` để bắt đầu tạo cây.

### Các hàm duyệt cây

- `preorder(i)`: duyệt node, trái, phải
- `inorder(i)`: trái, node, phải
- `postorder(i)`: trái, phải, node

Mỗi hàm in ra thông tin như:

```c
printf("Node %d: data = %d, content = %s, isLeaf = %d\n", i, nodes[i].data, nodes[i].content, nodes[i].isLeaf);
```

## Chạy chương trình

Vào thư mục chứa file `SamplerTree.c` rồi gõ:

```bash
gcc SamplerTree.c -o SamplerTree
./SamplerTree
```
