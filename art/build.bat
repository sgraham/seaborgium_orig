@echo off
:: png manually exported from inkscape at 3 sizes.
:: 'convert' is imagemagick's.
convert -background transparent sg16.png sg32.png sg256.png sg.ico
