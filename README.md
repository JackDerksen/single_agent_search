# CMPT 355 - Project 1
### Jack Derksen, Rordon Crawley, Robin Huppertz, Tuan Huynh

---

## Building
I wrote the Makefile with MinGW, MSYS2, and WSL in mind (i.e., not raw MSVC + nmake, because Microsoft's gonna Microsoft).

You should just be able to build the project with:
```
make
./bin/AB      # Unix
bin\AB.exe    # Windows
```

## Roadmap
- [x] State class with large/small disk stacks, equality comparison, output printing, and hashing
    - [x] Sanity tests performed in main file to verify hashing correctness
