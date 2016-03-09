# DE2 Hardware Accelerated Graphics

This project is an ECE 492 Capstone Design Project at the University
of Alberta. The contributors to this project are Stephen Just, Stefan
Martynkiw and Mason Strong.

## Building and Running the Project (non-persistent)
There are several build steps to run our code on a DE2 board.

1. Open `de2_video_processor.qsf` in Quartus II 12.1SP1
2. Double-click on "Compile Design" in the "Tasks" pane
3. From the "Processing" menu, select "Update Memory Initialization File"
4. Double-click on "Compile Design" in the "Tasks" pane (again, yes, really)
5. Program the DE2 with the generated SOF file
6. From the "Tools" menu in Quartus II, select "Nios II Software Build-Tools
for Eclipse"
7. Select a workspace (the `software` directory in this tree works fine)
8. Import our Eclipse projects into your workspace. (File > Import... >
General > Existing Projects Into Workspace, select the `software` directory as
the search directory)
9. Right-click on a `*_bsp` project, select "Nios II", "Generate BSP"
10. Right click on a project, select "Run As" > "Nios II Hardware"

## Asset Files
Copy the contents of the `data` directory to the root of a FAT16-formatted
SD card.
