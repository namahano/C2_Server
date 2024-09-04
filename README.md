# C2 Server

## compile

```
For Linux:
i686-w64-mingw32-g++ -std=c++11 maldev.cpp -o maldev.exe -s -lws2_32 -Wno-write-strings -fno-exceptions -fmerge-all-constants -static-libstdc++ -static-libgcc

For Windows:
g++ -std=c++11 maldev.cpp -o maldev.exe -s -lws2_32 -Wno-write-strings -fno-exceptions -fmerge-all-constants -static-libstdc++ -static-libgcc
```

