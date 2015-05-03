CXXFLAGS=-fPIC -I..
MODS=mod_tuto.so mod_7rain.so mod_flag.so mod_geography.so mod_kinetics.so \
mod_stratoscape.so mod_python.so mod_unknown_pleasure.so mod_parasite.so \
mod_waste.so


ifeq ($(shell uname), Darwin)
	VISHOME=/Users/amand/Library/Application\ Support/XBMC/addons/visualization.unknownpleasure/
	XBMCPATH=/Users/amand/tmp/xbmc-12.3-Frodo/
	CXXFLAGS+=-I/Users/Shared/xbmc-depends/macosx10.7_i386/include/python2.6
	LDFLAGS+=-framework OpenGL -ljpeg -lpng -lfreetype
	LDFLAGS+=-lpython2.6
	SHARED=-bundle
	ARCH=x86-osx
else 
	VISHOME=/home/amand/.xbmc/addons/visualization.unknownpleasure/
	XBMCPATH=/home/amand/Code/kodi/
	LDFLAGS+=-lGL -ljpeg -lpng -lfreetype -lpython2.7
	CXXFLAGS+=-I/usr/include/python2.7
	SHARED=-shared
	ARCH=i486-linux
endif

CFLAGS=$(CXXFLAGS)

all: $(MODS)

#OBJSDIR=../sandboxsrc/
OBJSDIR=../
OBJS=$(OBJSDIR)mathematics.o $(OBJSDIR)transform.o $(OBJSDIR)opengl.o $(OBJSDIR)presets.o

mod_tuto.so: mod_tuto.o skybox.o pool.o $(OBJSDIR)shader.o $(OBJS) 
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(SHARED) $^ -o $@ 

mod_7rain.so: mod_7rain.o pool.o $(OBJS) 
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(SHARED) $^ -o $@ 

mod_flag.so: mod_flag.o font.o texture.o $(OBJSDIR)shader.o $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(SHARED) $^ -o $@ 

mod_geography.so: mod_geography.o texture.o $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(SHARED) $^ -o $@ 

mod_kinetics.so: mod_kinetics.o obj.o $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(SHARED) $^ -o $@ 

mod_stratoscape.so: mod_stratoscape.o $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(SHARED) $^ -o $@ 

mod_unknown_pleasure.so: mod_unknown_pleasure.o $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(SHARED) $^ -o $@

mod_parasite.so: mod_parasite.o $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(SHARED) $^ -o $@

mod_python.so: mod_python.o $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(SHARED) $^ -o $@

mod_waste.so: mod_waste.o pool.o texture.o particles.o $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(SHARED) $^ -o $@

include $(XBMCPATH)Makefile.include

installmods:
	cp *.so $(VISHOME)resources/

clean: 
	rm *.o
	rm *.so
