import sys
from PIL import Image, ImageOps
in_file = sys.argv[1]
out_file = sys.argv[2]

im = Image.open(in_file)
#params on quantize: #colors, method(0,1,2), kmeans, palette=None
#a particular palette can be used by opening a bmp with the palette desired,
#and passing it as palette=InstanceWithPaletteDesired
#quantized = im.quantize(256, 1, 256)
#quantized = ImageOps.posterize(im,2)
quantized = im.convert(mode='P', colors=256, palette="WEB")
the_palette = quantized.getpalette()

#TODO: save with 8 total bits of color....
binary_output = open("binary_out", "w")
binary_output.write(quantized.tobytes())
binary_output.close()

#save proper bmp with palette and 24 bit colors
quantized.save(out_file, "BMP")

#Write 565 colors to palette
palette_file = open(out_file + "_palette.txt", "w")
if the_palette is not None:
	for i in range (0, 768):
		if (i%3 == 0):#red
			item = (the_palette[i]/8)<<11
			palette_file.write("%05d" % item )
		if (i%3 == 1):#green
			item = (the_palette[i]/4)<<5
			palette_file.write("%06d" % item )
		if (i%3 == 2):#blue
			item = (the_palette[i]/8)
			palette_file.write("%05d\n" % item )
palette_file.close()

