import OpenGL
OpenGL.ERROR_LOGGING = False
from OpenGL.GL import *
import TRANSFORM as tr


vtx = [ 0.0, 1.0, 0.0,
	1.0, -1.0, 0.0,
	-1.0, -1.0, 0.0 ]
nrm = [ 0.0, 0.0, 1.0,
	0.0, 0.0, 1.0,
	0.0, 0.0, 1.0 ]
clr = [ 1.0, 0.0, 0.0, 1.0,
	0.0, 1.0, 0.0, 1.0,
	0.0, 0.0, 1.0, 1.0 ]


def init(hyp):
	print "%s: init()\n" % (__name__)
	# do stuff here
	print hyp
#	print "context: %%\n" % (glXGetCurrentContext());
	print glGetString(GL_VERSION);

def destroy():
	print "%s: destroy()\n" % (__name__)
	# do stuff here

def draw(pg, samples):
	global vtx;
	global clr;
	global nrm;
	tr.identity();
	tr.modelRotate(0.0, 0.0, 0.0);
	tr.viewTranslate(0.0, 0.0, -6.0);
	tr.MVP();

	#glClearColor(1.0, 0.0, 0.0, 1.0)
	#glClear(GL_COLOR_BUFFER_BIT);

	glVertexAttribPointer(pg["pos"], 3, GL_FLOAT, GL_FALSE, 0, vtx);
	if(pg["nrm"] != -1):
		glVertexAttribPointer(pg["nrm"], 3, GL_FLOAT, GL_FALSE, 0, nrm);
	glVertexAttribPointer(pg["clr"], 4, GL_FLOAT, GL_FALSE, 0, clr);
	glDrawArrays(GL_TRIANGLES, 0, 3);
