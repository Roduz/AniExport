There are several ways to use this tool, the simplest is to drag and drop an .ANI file onto the .EXE. exporting a .GIF file to the tool's path. Running the tool manually through the command line will allow for more options including png export, background colors, different palettes, and only lines for example. This can also be done by running a .BAT file (a TXT file with the command arguments). Note that there will be a colors loss, merging down to 256 due to restrictions of the GIF format. The color quantization code reduces any noticeable effect.

Required File Entry: -ani path/file.ani

Optional File Entries: -out path/file
-pmap path/file.pmap -cmap path/file_colormap.pal
-pal path/file_1p.pal -dyepal path/file_1p_dye.pal

Optional Values:
-bg r g b a <or> -bg r g b (sets background color range 0 - 255)

Optional Flags:
-gif (output will be a gif file)
-png (output will be a series of png files)
-movie (output will be a series of png frame to used for a movie)
-lines (will only process lines layer files)
-colors (will only process colors layer files)
-shade (will only process shade layer files)
-dye (will only process dye layer files)
-palout (outputs the palette data as a png file)
-help <or> -h (calls help)

Optional values will be filled out with defaults and existing files.
Output will be set at the exe location.

In this example $(ParentDir) is where the git project is located.
AniExport.exe -ani $(ParentDir)test\data\attack\supers\armskip\loop.ani -pal $(ParentDir)test\palettes\cerebella_5p.pal -gif -out $(ParentDir)armskip_loop
