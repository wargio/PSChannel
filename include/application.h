/*
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

 This program was created by Grazioli Giovanni Dante <wargio@libero.it>.
*/

#ifndef __PLAYSTATION_CHANNEL_LANGUAGE_H__
#define __PLAYSTATION_CHANNEL_LANGUAGE_H__

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <stdint.h>
#include "irrXML.h"

#define PSC_NO_TYPE		0
#define PSC_PACKAGE		1
#define PSC_THEME		2
#define PSC_IMAGE		3
#define PSC_VIDEO		4

using namespace std;
using namespace irr;	//namespace for irrXML
using namespace io;	//namespace for irrXML

typedef struct {
	uint32_t n_elem;
	string *name;		//Homebrew Name
	string *version;	//Version
	string *link;		//Link Package
	string *pkg_file;	//Image Link
	string *img_file;	//Image File
	int    *type_file;	//Type of the File
} PSC_Application;

int LoadStrings(const char* filename, string *str, int n);
int LoadStringsFromXML(const char* filename, PSC_Application *App);
int LoadMainStringsFromXML(const char* filename, string *str);

#endif
