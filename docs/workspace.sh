doc="texdoc.tex"
pdflatex $doc
i3-msg split horizontal
evince $(basename -s .tex "$doc").pdf &
i3-msg split vertical
alacritty -e sh -c "cd $PWD && nvim $doc" &
when-changed $doc pdflatex $doc 
