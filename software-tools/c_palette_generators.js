// Generates palettes for C



/*
   ECE 492: Hardware Accelerated Graphics
   Stefan Martynkiw, Stephen Just, Mason Strong

   Sunday, Feb, 14, 2016

   This script makes palette arrays for C.
*/   
   
/* Select the base colour to make a palette off of */
var base_colour;
var vary_r;

for (var i = 0; i < 256; i++) {

    

}



// RGB 323 = 8 bits in
// RGB 565 = 16 bits

outputlist = [];

for (var i = 0; i < 256; i++) {

    // Build up the binary string as we need it.
    // Should be 8 digits long, no matter what.

    var s = "00000000" + i.toString(2);
    var number =  s.substr(s.length-8);
    //console.log(number);

    // Perform operations on number.

    
    //Values 
    var r;
    var g;
    var b;

    /*
     * Altera-provided code:
     * /opt/altera/12.1/ip/University_Program/Audio_Video/Video/altera_up_avalon_video_rgb_resampler/altera_up_avalon_video_rgb_resampler.vhd

    `elsif USE_8_BITS_IN
    r <= (stream_in_data( 7 DOWNTO  5) & stream_in_data( 7 DOWNTO  5) & 
			    stream_in_data( 7 DOWNTO  5) & stream_in_data( 7));
	    g <= (stream_in_data( 4 DOWNTO  2) & stream_in_data( 4 DOWNTO  2) & 
			    stream_in_data( 4 DOWNTO  2) & stream_in_data( 4));
	    b <=  stream_in_data( 1 DOWNTO  0) & stream_in_data( 1 DOWNTO  0) & 
			    stream_in_data( 1 DOWNTO  0) & stream_in_data( 1 DOWNTO  0) & 
			    stream_in_data( 1 DOWNTO  0);
    */

    r = number.slice(0,3); // 3 bits (0, 1, 2)
    g = number.slice(3, 6); // 3 bits (3, 4, 5)
    b = number.slice(6, 8); // 2 bits (6, 7)

/*
`elsif USE_16_BITS_OUT
	converted_data(15 DOWNTO 11) <= r( 9 DOWNTO  5);
	converted_data(10 DOWNTO  5) <= g( 9 DOWNTO  4);
	converted_data( 4 DOWNTO  0) <= b( 9 DOWNTO  5);
*/ 

    // Output values
    var r_out = r + r + r + r[0]; //10 bits
    var g_out = g + g + g + g[0]; // 9 bits
    var b_out = b + b + b + b + b; //10 bits

    var converted_data = r_out.slice(0, 5) + g_out.slice(0,6) + b_out.slice(0,5);
    //console.log("Number split up: " + [r, g, b]);
    //console.log([r_out, g_out, b_out]);
    
    outputlist.push([number, converted_data]);

}





/* Now we need to make a nice output in hex for the C people */
//console.log(outputlist);

var c_array = "{";
for (var i = 0; i < outputlist.length; i++) {

    c_array += "0x" + parseInt(outputlist[i][1], 2).toString(16) + " ";

    if (i < outputlist.length-1) {
        c_array += ", ";
    }

    if (i%8 == 0 && i > 0) {
       c_array += "\n";
    }

}
c_array += "}";

//console.log(c_array);


var c_array = "{";
for (var i = 0; i < 32; i++) {

    c_array += "0x0" + parseInt(i, 10).toString(16) + "0 ";

    if (i < outputlist.length-1) {
        c_array += ", ";
    }

    if (i%8 == 0 && i > 0) {
       c_array += "\n";
    }

}
c_array += "}";

console.log(c_array);


