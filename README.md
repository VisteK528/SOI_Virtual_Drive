# Simple Virtual Drive Project

This repository contains code for simple virtual drive created for Operating Systems course at Faculty of Electronics and Information Technology, Warsaw University of Technology.

## About
Virtual drive is created inside a binary file. The file consists of 2 main blocks:
- header block
- data block

Header block contains information about the disk itself such as its name, size and default settings as well as information about stored files and their locations.

Data block contains as name suggests - stored data. This block is divided into smaller data blocks, each of 4096 bytes. This way there internal and external fragmentation may occur.

## Usage
Application is provided in form of CLI tool which allows to:
1. Create a virtual drive in single file with given name and size (in blocks)
```bash
./svd create drive.img 1
```
2. Copy file from host system into virtual drive
```bash
./svd copyin drive.img example1.txt
```
3. Copy file from virtual drive into host system
```bash
./svd copyout drive.img example1.txt example1_out.txt
```
4. Remove file from virtual drive
```bash
./svd rm drive.img example1.txt
```
5. Delete virtual drive
```bash
./svd delete drive.img
```
6. Display information about current usage (list of following drive blocks with information about: adress, block type, size, state)
```bash
vistek@vistek-desktop:~/SOI_Virtual_Drive$ ./svd map drive.img 
===================================================
                 Disk data block map               
===================================================

---------------------------------------------------
| Block number |    Adress range [dec]   | Status |
---------------------------------------------------
|            0 |          0 -       4096 |  FREE  |
|            1 |       4096 -       8192 |  FREE  |
|            2 |       8192 -      12288 |  FREE  |
|            3 |      12288 -      16384 |  FREE  |
...
```
7. Display information about drive
```bash
vistek@vistek-desktop:~/SOI_Virtual_Drive$ ./svd about drive.img 
=======================================
Summary information about virtual drive
=======================================

=======================================
              General Info             
=======================================
Drive name: drive.img
Drive size [bytes]: 1080384 
Saved files: 0 

=======================================
          Drive settings Info                  
=======================================
Max filename length: 32
Max files in drive: 128 
Max blocks: 25600 
Block size [bytes]: 4096 
Max drive size [bytes]: 104857600 
```

## Author
Piotr Patek, 2025