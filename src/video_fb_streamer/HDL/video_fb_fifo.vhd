-- ---------------------------------------------------------------------------
-- Video output data FIFO
--
-- This component takes video data in 16-bit chunks from memory into a FIFO,
-- associates it with its start and end of packet markers, and then outputs
-- the video data in 8-bit chunks as requested by the video pipeline.
--
-- Author: Stephen Just
-- ---------------------------------------------------------------------------

LIBRARY ieee;
USE ieee.std_logic_1164.all;
USE ieee.numeric_std.all;

-- Library containing FIFO components
LIBRARY altera_mf;
USE altera_mf.altera_mf_components.all;

ENTITY video_fb_fifo IS 
	GENERIC(
		PIXEL_BITS_IN          : integer := 16;
		PIXEL_BITS_OUT         : integer := 8;
		ALMOST_EMPTY_THRESHOLD : integer := 64
	);

	PORT (
		in_clk                   : in    std_logic;
		out_clk                  : in    std_logic;
		reset                    : in    std_logic;

		in_startofpacket         : in    std_logic;
		in_endofpacket           : in    std_logic;
		in_pixdata               : in    std_logic_vector (PIXEL_BITS_IN-1 downto 0);

		out_startofpacket        : out   std_logic;
		out_endofpacket          : out   std_logic;
		out_pixdata              : out   std_logic_vector (PIXEL_BITS_OUT-1 downto 0);

		in_req                   : in    std_logic;
		out_req                  : in    std_logic;

		full                     : out   std_logic;
		empty                    : out   std_logic;
		almost_empty             : out   std_logic
	);

END video_fb_fifo;

ARCHITECTURE Behaviour OF video_fb_fifo IS

	SIGNAL fifo_data_in    : std_logic_vector (PIXEL_BITS_IN+3 downto 0);
	SIGNAL fifo_data_out   : std_logic_vector (PIXEL_BITS_IN+3 downto 0);
	SIGNAL fifo_write_used : std_logic_vector (6 downto 0);
	SIGNAL fifo_read       : std_logic := '0';
	SIGNAL fifo_word       : std_logic := '0';

BEGIN

	-- Output fifo clock
	PROCESS (out_clk) IS
	BEGIN
		IF rising_edge(out_clk) THEN
			IF reset = '1' THEN
				fifo_word <= '0';
			ELSIF out_req = '1' THEN
				fifo_word <= NOT fifo_word;
			END IF;
		END IF;
	END PROCESS;

	-- Combinational logic
	fifo_read <= '1' WHEN fifo_word = '0' AND out_req = '1' ELSE '0';
	fifo_data_in <= ('0' & in_endofpacket & in_pixdata(15 downto 8) & in_startofpacket & '0' & in_pixdata(7 downto 0));
	out_pixdata <= fifo_data_out(7 downto 0) WHEN fifo_read = '0' ELSE
						fifo_data_out(17 downto 10) WHEN fifo_read = '1' ELSE
						(others => '0');
	out_startofpacket <= fifo_data_out(9) WHEN fifo_read = '0' ELSE '0';
	out_endofpacket <= fifo_data_out(18) WHEN fifo_read = '1' ELSE '0';
	almost_empty <= '1' WHEN (to_integer(unsigned(fifo_write_used)) < ALMOST_EMPTY_THRESHOLD) ELSE '0';
	full <= '1' WHEN (to_integer(unsigned(fifo_write_used)) > 120) ELSE '0'; -- Indicate full a little bit early

	-- Instantiate an altera-provided dual-clock FIFO
	f0 : dcfifo
	GENERIC MAP(
		intended_device_family  => "Cyclone II",
		add_ram_output_register => "OFF",
		lpm_numwords            => 128,
		lpm_showahead           => "ON",
		lpm_type                => "dcfifo",
		lpm_width               => PIXEL_BITS_IN+4,
		lpm_widthu              => 7,
		overflow_checking       => "OFF",
		underflow_checking      => "OFF",
		use_eab                 => "ON"
	)
	PORT MAP(
		wrclk				=> in_clk,
		wrreq				=> in_req,
		wrusedw        => fifo_write_used,
		wrfull         => open,
		data				=> fifo_data_in,

		rdclk          => out_clk,
		rdreq				=> fifo_read,
		rdempty        => empty,
		q					=> fifo_data_out,

		aclr           => reset
	);

END Behaviour;
