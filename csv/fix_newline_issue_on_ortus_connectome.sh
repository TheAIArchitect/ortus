#!/bin/sh

#########
### NOTE: it seems this is only necessary for older Excel (2011). In the 2016 version, it doesn't seem to be needed.
#########


# To get the "^M" character to appear, you must type it <Ctrl-V><Ctrl-M>
# That is, you must hold control, and then press the v key followed by the m key
# From: http://stackoverflow.com/questions/811193/how-to-convert-the-m-linebreak-to-normal-linebreak-in-a-file-opened-in-vim

# This command replaces all "^M" linebreaks with \r\n linebreaks, and facilitates C++ importing. 

# From: http://stackoverflow.com/questions/10748453/replace-comma-with-newline-in-sed
tr  '\n' <ortus_basic_connectome_excel_out.csv >ortus_basic_connectome.csv
