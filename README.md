# asw2json

Utility to write a JSON file using .cpp .h files generated by  https://github.com/manicken/SoundFontDecoder.git.

## Workflow
1. Run the script rewrite-instrument contained in the tools subdirectory. It takes a single parameter which is the name of the soundfont file to process minus the .sf2 extension. For example "./rewrite-instrument irish2".
2. Run the json2asw executable. This will produce a json file called instrument.jso
3. Copy the file to a micro sd card for the teensy, rename it to something sensible.
4. The examples subdirectory contains a sketch to play a note from a json file soundfont loaded from disc.
