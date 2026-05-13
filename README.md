<h1 align="center"> AniStitch </h1>
<p align="center">
  Program to stitch images from anime together <br>
  Uses Qt (GUI) and OpenCV (Stitching, Image/Video loading) <br>
</p>

## Info
Program to stitch images from anime together. Works on things other than anime too. <br>
Takes video/image files as input. <br>
Stitches chunks of images in an inverted pyramid style. Each chunk in a different thread.

For example, with 100 images:
- Split the 100 images into chunks of 10
- Stitch those 10 chunks -> 1 stitched image
  - The resulted stitched image from the chunks is added to a finished images list
  - Repeat until all chunks are processed
- Splits the finished images list into another chunks of 10
- Repeat until the finished images list contains only 1 image (the final stitched image)

## Dependencies (Arch Linux)
`qt6-base` <br>
`opencv` <br>
`opencl runtime (optional)` <br>
`cmake (compile)` <br>

## Compile (Arch Linux)
Install the dependencies then run: <br>
```bash
git clone https://github.com/nisemono0/AniStitch.git && cd ./AniStitch
```
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build -j
```
The binary is `./build/AniStitch` <br>
