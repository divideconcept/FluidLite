examples: example/sf_/Boomwhacker.sf2 #example/sf_/Boomwhacker.sf3

example/%: out/example
	@echo Run example
	@LD_LIBRARY_PATH=out $^ $* $*.pcm

clean:
	@rm -rf out

out/example: src/main.c out/libfluidlite.a
	@$(CC) -o $@ $< -I../include -Lout -lfluidlite

out/libfluidlite.a:
	@mkdir -p out
	@cd out && cmake ../../ && make
