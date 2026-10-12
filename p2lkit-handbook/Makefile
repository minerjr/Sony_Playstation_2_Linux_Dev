# Makefile for building p2lkit-handbook documentation

HBXML := foverview.xml intro.xml kernel.xml kithwsw.xml p2lkit-handbook.xml \
         performance.xml resources.xml software.xml
SINGLE_XSL := /usr/share/sgml/docbook/stylesheet/xsl/nwalsh/html/docbook.xsl
MULTIPLE_XSL := /usr/share/sgml/docbook/stylesheet/xsl/nwalsh/html/chunk.xsl
XSLTPROC := /usr/bin/xsltproc
SVGS := figures/PS2BlockDiagram.svg
PSS  := $(SVGS:.svg=.ps) \
	figures/PS2BD-EEcore.ps \
	figures/PS2BD-FPU.ps \
	figures/PS2BD-GIF.ps \
	figures/PS2BD-GS.ps \
	figures/PS2BD-IOP.ps \
	figures/PS2BD-IPU.ps \
	figures/PS2BD-VPUs.ps

PNGS := $(PSS:.ps=.png)

.SUFFIXES: .svg .ps .png

.ps.png:
	gs -sDEVICE=ppmraw -r180x180 -sOutputFile=- -sNOPAUSE -q $< -c showpage -c quit | pnmcrop | pnmscale -xysize 600 600 | pnmmargin -white 10 | pnmtopng > $@

all: $(PNGS) html-single html-chapters
	echo "Finished building Playstation 2 Kit Handbook"

clean:
	rm -rf htmlbook-chapters htmlbook-single
	rm -f figures/*.png

html-chapters: $(HBXML)
	rm -rf htmlbook-chapters
	mkdir htmlbook-chapters
	mkdir htmlbook-chapters/figures
	cp -a figures/* htmlbook-chapters/figures
	$(XSLTPROC) -o htmlbook-chapters $(MULTIPLE_XSL) p2lkit-handbook.xml
	mv *.html htmlbook-chapters

html-single: $(HBXML)
	rm -rf htmlbook-single 
	mkdir htmlbook-single
	mkdir htmlbook-single/figures
	cp -a figures/* htmlbook-single/figures
	$(XSLTPROC) $(SINGLE_XSL) p2lkit-handbook.xml > htmlbook-single/p2lkit-handbook.html

.PHONY: all
