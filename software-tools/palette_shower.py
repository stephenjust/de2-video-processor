import sys
import numpy
import tkinter
in_file = sys.argv[1]
top = tkinter.Tk()

w = tkinter.Canvas(top, bg="white", height=540, width=540)
w.pack()
data = numpy.fromfile(in_file, dtype=numpy.uint16)
data = data.astype(numpy.uint32)
#from https://stackoverflow.com/q/5415360
data888 = ((data & 0xF800) << 8) + ((data & 0x07E0) << 5) + ((data & 0x001F) << 3)

k = 0
for i in range(1,17):
				for j in range(1,17):
								fill_color = "#%06x" % data888[k]
								print(fill_color)
								w.create_rectangle(j*30, i*30, j*30+30, i*30+30, width=0, fill=fill_color)
								k = k + 1
for i in range(1,17):
				for j in range(1,17):
								value = str(j + 16*(i-1) )
								w.create_text(j*30+14, i*30+16, text=value) 
top.mainloop()


#		red = ( the_palette[i*3] >> 3 ) << 11
#		green = (the_palette[i*3+1] >> 2 ) << 5
#		blue = the_palette[i*3+2] >> 3
#		result_565 = red | green | blue
