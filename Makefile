

all:
	(cd src; make)


ex:
	(cd examples; make)

clean:
	(cd src; make clean)
	(cd examples; make clean)

