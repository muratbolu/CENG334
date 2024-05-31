example-2048.img: Unmodified image

example-2048-baseline.img: Image with the guarantees given in the homework text

example-2048-blockbitmap.img: example-2048-baseline.img with deleted block bitmaps

example-2048-inodebitmap.img: example-2048-baseline.img with deleted inode bitmaps

example-2048-bitmap.img: example-2048-baseline.img with deleted bitmaps

example-2048-baseline-pointer.img: example-2048-baseline.img with some missing pointers

example-2048-blockbitmap-pointer.img: example-2048-blockbitmap.img with some missing pointers

example-2048-inodebitmap-pointer.img: example-2048-inodebitmap.img with some missing pointers

example-2048-bitmap-pointer.img: example-2048-bitmap.img with some missing pointers

identifier: 0x01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00

example terminal command: ./recext2fs ./example-2048-bitmap-pointer.img 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00