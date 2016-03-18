-- ---------------------------------------------------------------------------
-- Geometry Package
--
-- The Geometry package contains common types, components, and functions
-- useful when implementing graphics operations.
--
-- Author: Stephen Just
-- ---------------------------------------------------------------------------
LIBRARY ieee;
USE ieee.numeric_std.all;

PACKAGE geometry IS

	-- Represents a point in 2D-space
	TYPE point_t IS RECORD
		x : signed (15 downto 0);
		y : signed (15 downto 0);
	END RECORD point_t;

	-- Represents a line segment joining two points
	TYPE line_t IS RECORD
		p1 : point_t;
		p2 : point_t;
	END RECORD line_t;

	-- Represents a rectangle
	TYPE rect_t is RECORD
		p1 : point_t;
		p2 : point_t;
	END RECORD rect_t;

	-- Represents a buffer storing pixel data
	TYPE pixbuf_t IS RECORD
		address : unsigned (31 downto 0);
		rect    : rect_t;
	END RECORD pixbuf_t;

	-- rect_width, rect_height: Get the width or height of a rectangle
	-- Arguments:
	--    rect: The rectangle to get the dimension of
	-- Returns:
	--    Rectangle dimension
	FUNCTION rect_width  ( rect : rect_t ) RETURN unsigned ;
	FUNCTION rect_height ( rect : rect_t ) RETURN unsigned ;

	-- inner_rect_point: Get the coordinate of a point in a small rectangle relative to an enclosing rectangle
	-- Arguments:
	--    inner_rect: Smaller inner rectangle, fully enclosed by an outer rectangle
	--    inner_rect_topleft: The top left coordinate of the inner rectangle, relative to the outer rectangle
	--    inner_point: Point inside the inner rectangle, relative to the inner rectangle
	-- Returns:
	--    Point inside the inner rectangle, relative to the outer rectangle
	--
	--  _______________________________________________________________
	-- |---------------->v_____________________________                |
	-- |                 |                     v       |               |
	-- |                 |-------------------->o       |               |
	-- |                 |                             |               |
	-- |                 |_____________________________|               |
	-- |                                                               |
	-- |                                                               |
	-- |_______________________________________________________________|
	FUNCTION inner_rect_point ( inner_rect : rect_t; inner_rect_topleft, inner_point : point_t ) RETURN point_t ;

	-- pixbuf_pixel_address: Get the address of a pixel within a pixel buffer
	FUNCTION pixbuf_pixel_address ( pixbuf : pixbuf_t; point : point_t ) RETURN unsigned ;

	-- clip: Clip a signed or unsigned value to a range
	-- Arguments:
	--   x: The value to clip
	--   min: The minimum value of the clipping range
	--   max: The maximum value of the clipping range
	-- Returns:
	--   Clipped value of x
	FUNCTION clip ( x, min, max : signed ) RETURN signed ;
	FUNCTION clip ( x, min, max : unsigned ) RETURN unsigned ;

END PACKAGE;

PACKAGE BODY geometry IS

	FUNCTION rect_width  ( rect : rect_t ) RETURN unsigned IS
	BEGIN
		RETURN unsigned(rect.p2.x - rect.p1.x + x"1");
	END FUNCTION rect_width;

	FUNCTION rect_height ( rect : rect_t ) RETURN unsigned IS
	BEGIN
		RETURN unsigned(rect.p2.y - rect.p1.y + x"1");
	END FUNCTION rect_height;


	FUNCTION inner_rect_point ( inner_rect : rect_t; inner_rect_topleft, inner_point : point_t ) RETURN point_t IS
		VARIABLE point : point_t;
	BEGIN
		point.x := inner_rect.p1.x + inner_rect_topleft.x;
		point.y := inner_rect.p1.y + inner_rect_topleft.y;
		RETURN point;
	END FUNCTION inner_rect_point;


	FUNCTION pixbuf_pixel_address ( pixbuf : pixbuf_t; point : point_t ) RETURN unsigned  IS
		VARIABLE buf_width : unsigned (15 downto 0);
	BEGIN
		buf_width := rect_width(pixbuf.rect);
		RETURN pixbuf.address + unsigned(point.x) + unsigned(point.y) * buf_width;
	END FUNCTION pixbuf_pixel_address;


	FUNCTION clip ( x, min, max : signed ) RETURN signed IS
	BEGIN
		IF x < min THEN
			RETURN min;
		ELSIF x > max THEN
			RETURN max;
		ELSE
			RETURN x;
		END IF;
	END FUNCTION clip;

	FUNCTION clip ( x, min, max : unsigned ) RETURN unsigned IS
	BEGIN
		IF x < min THEN
			RETURN min;
		ELSIF x > max THEN
			RETURN max;
		ELSE
			RETURN x;
		END IF;
	END FUNCTION clip;
  
END PACKAGE BODY geometry;