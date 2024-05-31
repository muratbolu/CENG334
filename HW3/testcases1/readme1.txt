example-1024.img: Unmodified image

example-1024-baseline.img: Image with the guarantees given in the homework text

example-1024-blockbitmap.img: example-1024-baseline.img with deleted block bitmaps

example-1024-inodebitmap.img: example-1024-baseline.img with deleted inode bitmaps

example-1024-bitmap.img: example-1024-baseline.img with deleted bitmaps

example-1024-baseline-pointer.img: example-1024-baseline.img with some missing pointers

example-1024-blockbitmap-pointer.img: example-1024-blockbitmap.img with some missing pointers

example-1024-inodebitmap-pointer.img: example-1024-inodebitmap.img with some missing pointers

example-1024-bitmap-pointer.img: example-1024-bitmap.img with some missing pointers

identifier: 0x01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00

example terminal command: ./recext2fs ./example-1024-bitmap-pointer.img 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00