CXXFLAGS=-fPIC -std=c++98
OBJS=dir.o fft.o hypnomix.o mathematics.o module.o presets.o shader.o transform.o opengl.o
OBJS+=addon.o
SLIB=xbmc_hypnomix.vis
ifeq ($(shell uname), Darwin)
	VISHOME=/Users/amand/Library/Application\ Support/XBMC/addons/visualization.unknownpleasure/
	XBMCPATH=/Users/amand/tmp/xbmc-12.3-Frodo/
	LDFLAGS+=-framework OpenGL -ljpeg -lpng -lfreetype
	ARCH=x86-osx
else 
	VISHOME=/home/amand/.xbmc/addons/visualization.unknownpleasure/
	XBMCPATH=/home/amand/Code/xbmc-13.1-Gotham/
	LDFLAGS+=-lGL -ljpeg -lpng -lfreetype -ldl
	ARCH=i486-linux
endif

$(SLIB): $(OBJS)
ifeq ($(findstring osx,$(ARCH)), osx)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -bundle -o $(SLIB) $(OBJS) 
else
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -shared -o $(SLIB) $(OBJS)
endif

include $(XBMCPATH)Makefile.include

installvis:
	cp $(SLIB) $(VISHOME)xbmc_unknownpleasure.vis
