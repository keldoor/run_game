/*
	options model class
	
	(c) roman filippov, 2012
*/
#include <cstdlib>
#include <cassert>
#include "options.h"

Options::Options() {
	toDefaults();	
}

void Options::toDefaults() {
	if (!loadFromFile("DEFAULT_FILENAME")) {
		
		s = new settings;
		s->size = 10;
		s->walls = 1;
		s->enemies = 1;
		s->fullscreen = 0;
		s->edges = 1;
		
		save("DEFAULT_FILENAME");
	}
}

bool Options::loadFromFile(string &filename="DEFAULT_FILENAME") {
	FILE *f = fopen(filename,"rb");
	assert (f != NULL);
	s = new settings;
	
	if (fread(s,sizeof(settings),1,f) == 1) {
		fclose(f);
		return true;
	}
	fclose(f);
	return false;
}

void Options::save(string &filename="OPTIONS_FILENAME") {
	FILE *f = fopen(filename,"wb");
	assert(f != NULL)
	
	int n = fwrite(s,sizeof(settings),1,f);
	assert(n == 1);
	fclose(f);
}

settings *Options::getSettings() {
	return s;
}

void Options::optionsScene() {
	printf("Options !\n");
}
	
Options::~Options() {
	save();
}
