# Simple Virtual Drive Project - internal fragmentation scenario

1. Create the drive (256 data blocks of 4096 -> 1MB)
```bash
./svd create drive.img 256 
```

2. Copy example1.txt (22 bytes) and example2.txt (19 bytes) files into the drive
```bash
./svd copyin drive.img example1.txt
./svd copyin drive.img example2.txt 
```

```bash
vistek@vistek-desktop:~/SOI_Virtual_Drive$ ./svd copyin drive.img example1.txt 
Copied 'example1.txt' into virtual disk (22 bytes, blocks 0-0).
vistek@vistek-desktop:~/SOI_Virtual_Drive$ ./svd copyin drive.img example2.txt 
Copied 'example2.txt' into virtual disk (19 bytes, blocks 1-1).
```

As we can see first file is stored in the first data block and second file is stored in second data block.
Because each of them is far smaller than default block size (4096) - internal fragmentation occurred.