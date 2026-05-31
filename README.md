<h1 align="center"> AniStitch </h1>
<p align="center">
  Program to stitch images from anime together <br>
  Uses Qt (GUI) and OpenCV (Stitching, Image/Video loading) <br>
</p>

## Info
Program to stitch images from anime together. Works on things other than anime too. <br>
Takes video/image files as input. Can be given a list of files as argument to load them directly. <br>
Stitches chunks of images in an inverted pyramid style. Each chunk in a different thread.

For example, with 100 images:
- Split the 100 images into chunks of 10
- Stitch those 10 chunks -> 1 stitched image
  - The resulted stitched image from the chunks is added to a finished images list
  - Repeat until all chunks are processed
- Splits the finished images list into another chunks of 10
- Repeat until the finished images list contains only 1 image (the final stitched image)

## Shortcuts
All shortcuts are also shown inside the status tip when hovering over a widget.
### Main window
You can also right click on the Load button to trigger a reload.
Keybind | Description
|:-:|:-:|
`Ctrl+L` | Open load files dialog
`Ctrl+R` | Reload the last files
`Ctrl+D` | Remove selected images
`Ctrl+C` | Clear all images
`C` | Open crop dialog
`Ctrl+S` | Open custom stitcher dialog
`P` | Stitch images in PANORAMA mode
`S` | Stitch images in SCAN mode
`Ctrl+I` | Open logs dialog
`Ctrl+Q` | Quit program

### Logs dialog
Keybind | Description
|:-:|:-:|
`Ctrl+S` | Save logs to disk
`Ctrl+C` | Clear logs
`Ctrl+Q` | Close dialog

### Crop dialog
While the crop dialog is open you can make selection on the image.
Keybind | Description
|:-:|:-:|
`Tab/Shift+Tab` | Select next/previous widget
`V` | Crop using set values
`S` | Crop using set selection
`R` | Reset crop values/selection
`Q` | Close dialog

### Stitcher settings dialog
Some settings are SCAN/PANORAMA only. This is shown in the statusbar.
Keybind | Description
|:-:|:-:|
`Tab/Shift+Tab` | Select next/previous widget
`S` | Stitch images in SCAN mode
`P` | Stitch images in PANORAMA mode
`R` | Reset values to default
`Q` | Close dialog

### Stitched image dialog
You can left/right click on the image area to change the background. <br>
This makes it easier to spot transparency in images.
Keybind | Description
|:-:|:-:|
`Tab/Shift+Tab` | Select next/previous stitched image
`Ctrl+S` | Save the current stitched image
`Ctrl+Q` | Close dialog

## Compile (Arch Linux)
Run the following commands: <br>
- Install dependencies
```bash
sudo pacman -Syu git cmake gcc qt6-base opencv
```
- Clone and compile
```bash
git clone https://github.com/nisemono0/AniStitch.git && cd ./AniStitch
```
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build -j
```
The binary is in `./build/AniStitch`. <br>

## Compile (Windows/MinGW)
For Windows `MSYS2` needs to be installed. <br>
From a `MSYS2` environment, run the following commands: <br>
- Install dependencies
```bash
pacman -Syu git mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-qt6-base mingw-w64-ucrt-x86_64-opencv
```
- Clone and compile
```bash
git clone https://github.com/nisemono0/AniStitch.git && cd ./AniStitch
```
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build -j
```
- Deploy
```bash
mkdir ./build/AniStitchRelease/ && cd ./build/AniStitchRelease && mv ../AniStitch.exe .
```
```bash
windeployqt ./AniStitch.exe
```
```bash
ldd ./AniStitch.exe | grep /ucrt64 | awk '{print $3}' | xargs -i cp {} .
```
- Copy the program to desktop. Replace `<your-username-here>` with your Windows username
```bash
cd .. && mv ./AniStitchRelease/ /C/Users/<your-username-here>/Desktop/
```
There should be a `AniStitchRelease` folder on the Desktop now. <br>
The `.exe` is `AniStitch.exe` in the `AniStitchRelease` folder.
