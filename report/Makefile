repo.pdf: repo.dvi
	dvipdfmx repo.dvi
	rm repo.dvi

repo.dvi: repo.tex
	platex -interaction=batchmode repo.tex
	platex -interaction=batchmode repo.tex
