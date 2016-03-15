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