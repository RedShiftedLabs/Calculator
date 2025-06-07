#!/bin/bash

# Script to convert PNG to ICNS for macOS app icons

if [ ! -f "resources/Calculator.png" ]; then
    echo "Error: Calculator.png not found in resources folder"
    exit 1
fi

# Create temporary iconset directory
mkdir -p Calculator.iconset

# Generate different sizes for iconset
# macOS requires multiple resolutions in the iconset
sips -z 16 16 resources/Calculator.png --out Calculator.iconset/icon_16x16.png
sips -z 32 32 resources/Calculator.png --out Calculator.iconset/icon_16x16@2x.png
sips -z 32 32 resources/Calculator.png --out Calculator.iconset/icon_32x32.png
sips -z 64 64 resources/Calculator.png --out Calculator.iconset/icon_32x32@2x.png
sips -z 128 128 resources/Calculator.png --out Calculator.iconset/icon_128x128.png
sips -z 256 256 resources/Calculator.png --out Calculator.iconset/icon_128x128@2x.png
sips -z 256 256 resources/Calculator.png --out Calculator.iconset/icon_256x256.png
sips -z 512 512 resources/Calculator.png --out Calculator.iconset/icon_256x256@2x.png
sips -z 512 512 resources/Calculator.png --out Calculator.iconset/icon_512x512.png
sips -z 1024 1024 resources/Calculator.png --out Calculator.iconset/icon_512x512@2x.png

# Convert iconset to icns
iconutil -c icns Calculator.iconset

# Move to resources folder
mv Calculator.icns resources/

# Clean up
rm -rf Calculator.iconset

echo "Successfully created resources/Calculator.icns"
