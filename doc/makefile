latexfile = A10B0632P

TEX = pdflatex
VIEWER = evince


show : $(latexfile).pdf
	$(VIEWER) $< &

$(latexfile).pdf : $(latexfile).tex
	$(TEX) --shell-escape $<

clean :
	rm *.dvi *.aux *.log
