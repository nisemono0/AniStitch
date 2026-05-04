<h1 align="center"> AniStitch </h1>
<p align="center">
  Program to stitch anime images together <br>
  Uses Qt (GUI) and OpenCV (Stitching, Image/Video loading) <br>
</p>

## Info
Program to stitch anime images from video/image files together. <br>
Stitches images in an inverted pyramid style. <br>

For an example with 100 images:
- Split the 100 images into chunks of 10
- Stitch those 10 chunks -> 1 stitched image
  - The resulted stitched image from the chunks is added to a finished images list
- Splits the finished images list into chunks of 10
- Repeat until finished list contains only 1 stitched image being the full stitched image

> [!NOTE]
> For now the stitcher does not work with transparent images. <br>
> If a stitch has transparency, it will be filled with black instead of being transparent. <br>
> Will add transparency support SoonTM. <br>

## Dependencies (Arch Linux)
`qt6-base` <br>
`opencv` <br>
`opencl runtime (optional)` <br>
`cmake (compile)` <br>

## Compile (Arch Linux)
Install the dependencies then run: <br>
`cmake -B build -DCMAKE_BUILD_TYPE=Release` <br>
`cmake --build build` <br>
The binary is `./build/AniStitch`. <br>
