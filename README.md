# CMPT 355 - Lab Project 1
### Jack Derksen, Rordon Crawley, Robin Huppertz, Tuan Huynh

---

## Building
I wrote the Makefile with MinGW, MSYS2, and WSL in mind (i.e., not raw MSVC + nmake, because Microsoft's gonna Microsoft).

You should just be able to build the project with:
```
make
./bin/hello      # Unix
bin\hello.exe    # Windows
```
