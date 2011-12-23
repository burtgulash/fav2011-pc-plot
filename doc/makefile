latexfile = plot
figures   = figures/figure1.eps figures/figure2.eps figures/figura1.eps
helpfiles = makefile


TEX = latex
VIEWER = evince


show : pdf
	$(VIEWER) $(latexfile).pdf &

pdf : $(latexfile).pdf

ps  : $(latexfile).ps

tarball : $(latexfile).tar.gz

clean :
	rm *.dvi *.aux *.log $(latexfile).ps

$(latexfile).dvi : $(figures) $(latexfile).tex
	$(TEX) $(latexfile)

$(latexfile).pdf : $(latexfile).ps
	ps2pdf $(latexfile).ps $(latexfile).pdf


$(latexfile).ps  : $(latexfile).dvi
	dvips -Ppdf $(latexfile).dvi

$(latexfile).tar.gz : $(figures) $(latexfile).tex $(helpfiles)
	tar -c $^ | gzip > $@
