/*
	options model class
	
	(c) roman filippov, 2012
*/
#ifndef _OPTIONS_H_
#define _OPTIONS_H_
#define OPTIONS_FILENAME "config.rgc"
#define DEFAULT_FILENAME "default.rgc"
#include <SFML/System.hpp>
#include <string>
#include <vector>
#include "applayer.h"

/* different options records number, such as resolution,size,walls and so on */
#define OPT_SIZE 7

/* defines minimum maximum size of board */
#define MIN_SIZE 5
#define MAX_SIZE 20

/* defines maximum count of enemies can be */
#define MAX_ENEMIES s->size/5

/* defines different map sizes */
#define MAP_SMALL 10
#define MAP_AVERAGE 30
#define MAP_LARGE 60
#define MAP_HUGE 100

/* defines minimal supported resolution width */
#define MINIMUM_WIDTH 800

/* defines game font size */
#define FONT_SIZE app->getFontSize()

using std::string;
using std::vector;

struct Settings {
public:
	int resolution;
	int difficulty;
	int size;
	int walls;
	int enemies;
	bool fullscreen;
	bool edges;
};

class Application;

class Options : public AppLayer {
private:
	Settings *s;
	vector<sf::String> items;
	vector<sf::String> items_values;
public:
	Options(Application *apl);
	~Options();
	void toDefaults();
	bool loadFromFile(const string &filename = DEFAULT_FILENAME);	
	void save(const string &filename = OPTIONS_FILENAME) const;
	Settings *getSettings() const; 
	void getSettingsToArray(int *&p) const;
	void writeSettingsToStrings();
	void optionsItemsDraw();
	void changeOption(int num);
	void optionsScene();
};

inline Settings *Options::getSettings() const {
	return s;
}
#endif
