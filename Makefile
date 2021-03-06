CC = g++
CFLAGS = -g -O2 
LIBS = -lm

raytracer: raytracer.o util.o light_source.o scene_object.o bmp_io.o
	$(CC) $(CFLAGS) -o raytracer \
	raytracer.o util.o light_source.o scene_object.o bmp_io.o $(LIBS)

clean:
	-rm -f core *.o
	-rm raytracer

run:
	make && ./raytracer && feh view1.bmp view2.bmp view3.bmp
	
gitclean:
	git clean -f && git reset --hard HEAD && git pull


