FILES = latex-kurs.pdf
SRC = latex-kurs.tex \
	topics/aboutla.tex \
	topics/structure.tex \
	topics/design.tex \
	topics/math.tex \
	topics/graphics.tex \
	topics/listof.tex \
	topics/userdefined.tex \
	topics/references.tex \
	topics/bib.tex \
	topics/extra.tex

PDFLATEX = pdflatex
DVIPDF = dvipdf
LATEX = latex
BIBTEX = bibtex

all: $(FILES)

$(FILES): $(SRC)

clean:
	$(RM) $(FILES)

mrproper:
	$(RM) -rf *.aux *.bbl *.blg *.log *.nav *.out *.snm *.toc *.vrb

bib:
	$(BIBTEX) latex-kurs

full:
	$(PDFLATEX) latex-kurs
	$(BIBTEX) latex-kurs
	$(PDFLATEX) latex-kurs
	$(PDFLATEX) latex-kurs

%.pdf: %.tex
	$(PDFLATEX) $<

%.pdf: %.dvi
	$(DVIPDF) $<

%.dvi: %.tex
	$(LATEX) $<

