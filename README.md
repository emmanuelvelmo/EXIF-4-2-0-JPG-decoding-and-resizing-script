<p align="left">
  <strong>About this script (tested on Android's Samsung smartphones JPG images)</strong>
  <br/>
  This script resizes multiple JPG files from a gallery folder (which has to be called "imagedir" on the Desktop) while maintaining the original orientation of the image (portrait or landscape). This script automatically performs a centered crop to fit a 16:9 or 9:16 aspect ratio, then resizes images to a resolution of 1920×1080 (horizontal) or 1080×1920 (vertical) depending on the orientation initial of the image.
</p>

<p align="left">
  <strong>JPG structure</strong>
  <br/>
  FFD8 - SOI (Start of Image)

  FFE0 - APP0 (Application-Specific Marker)
  Segment length (2 bytes)
  Application identifier ("JFIF", "EXIF", etc.) (variable)
  Application version (2 bytes, typically)
  Image resolution units (1 byte)
  Resolution X (2 bytes)
  Resolution Y (2 bytes)
  Thumbnail width (1 byte)
  Thumbnail height (1 byte)
  Thumbnail data (3thumbnail widththumbnail height bytes, if a thumbnail exists)
  
  FFDB - DQT (Define Quantization Table)
  Segment length (2 bytes)
  Quantization table information (1 byte: 4 bits for the precision element and 4 bits for the table index)
  Quantization table (64 bytes in case of 8-bit precision, 128 bytes in case of 16-bit precision)
  
  FFC0 - SOF0 (Start of Frame)
  Segment length (2 bytes)
  Sample precision (1 byte)
  Image height (2 bytes)
  Image width (2 bytes)
  Number of image components (1 byte)
  Component specifications (3 bytes per component: 1 byte for component identifier, 1 byte for sampling factors, and 1 byte for quantization table index)
  
  FFC4 - DHT (Define Huffman Table)
  Segment length (2 bytes)
  Huffman table information (1 byte: 4 bits for the table type and 4 bits for the table index)
  Length code counts (16 bytes)
  Huffman symbols (variable)
  
  FFDA - SOS (Start of Scan)
  Segment length (2 bytes)
  Number of image components (1 byte)
  Component specifications (2 bytes per component: 1 byte for component identifier and 1 byte for Huffman table indexes)
  Start of selection spectrum (1 byte)
  End of selection spectrum (1 byte)
  Successive approach positions (1 byte)
  
  FFD9 - EOI (End of Image)
</p>

<p align="left">
  <strong>Input Image (JPG) (4032×2268)</strong>
  <br/>
  <img src="https://github.com/emmanuelvelmo/EXIF-4-2-0-JPG-decoding-and-resizing-script/assets/51292782/f922a55b-9399-49bd-9265-76edfd54cea1"/>
</p>

<p align="left">
  <strong>Huffman decoding + subsampling</strong>
  <br/>
  <img src="https://github.com/emmanuelvelmo/EXIF-4-2-0-JPG-decoding-and-resizing-script/assets/51292782/9a07853f-8041-4f8b-88c4-6896399e6cbd"/>
</p>

<p align="left">
  <strong>Inverse discrete cosine transform</strong>
  <br/>
  <img src="https://github.com/emmanuelvelmo/EXIF-4-2-0-JPG-decoding-and-resizing-script/assets/51292782/2a52c6cd-0645-4e7c-9c68-fe100888fdc8"/>
</p>

<p align="left">
  <strong>RGB conversion / output Image (BMP) (1920×1080)</strong>
  <br/>
  <img src="https://github.com/emmanuelvelmo/EXIF-4-2-0-JPG-decoding-and-resizing-script/assets/51292782/9d7c2e43-1118-4a8e-8447-0026c1c11fde"/>
</p>
