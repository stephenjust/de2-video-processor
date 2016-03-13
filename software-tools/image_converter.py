import sys
from PIL import Image
in_file = sys.argv[1]
out_file = sys.argv[2]

im = Image.open(in_file)
quantized = im.convert(mode='P', colors=256, palette=Image.ADAPTIVE)
the_palette = quantized.getpalette()

#save proper bmp with palette and 24 bit colors
quantized.save(out_file, "BMP")

#Write 565 colors to palette
palette_file = open("{}.pal".format(out_file), "wb")
if the_palette is not None:
	for i in range (0, 256):
		red = ( the_palette[i*3] >> 3 ) << 11
		green = (the_palette[i*3+1] >> 2 ) << 5
		blue = the_palette[i*3+2] >> 3
		result_565 = red | green | blue
		palette_file.write( result_565.to_bytes(2, byteorder="little", signed=False) )
palette_file.close()

