## C++ 編譯問題


目前使用 MySQL 的 API，在編譯上的時候可能會沒抓到 API 的函式呼叫，例如：
```cpp
connection_ = mysql_init(nullptr);
```
導致 `undefined reference to 'mysql_init'` 這個錯誤語句。

原因是因為沒有連結到 mysqlclient 這個 lib，所以在 `CMakeLists.txt` 裡面改一下即可：
```txt
target_link_libraries(library_system PRIVATE
    mysqlclient
)
```