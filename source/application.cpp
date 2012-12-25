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

#include "application.h"

int LoadStrings(const char* filename, string *str, int n){
	fstream f;
	f.open(filename,ios::in);
	if(f.fail())
		return -1;
	for(int i=0;i<n;i++)
		getline(f,str[i],'\n');

	f.close();
	return 1;
}


int LoadMainStringsFromXML(const char* filename, string *str){
	IrrXMLReader* xml = createIrrXMLReader(filename);
	int i=0;  //should be 5 categories.
	while(xml && xml->read() && i< 5)
	{
 		switch(xml->getNodeType())
		{
			case EXN_ELEMENT:
			{
				if (!strcmp("file", xml->getNodeName())){
					str[i] = xml->getAttributeValue("id");
					i++;
				}
			}
				break;
			case EXN_NONE:
				break;
			case EXN_ELEMENT_END:
				break;
			case EXN_TEXT:
				break;
			case EXN_COMMENT:
				break;
			case EXN_CDATA:
				break;
			case EXN_UNKNOWN:
				break;
		}
	}
	delete xml;
	return 1;
}

/* 
 * Example of structure of the XML
 * <elements n="10"></elements> //needed to define the number of the elements. it needs to be written once.
 * this is what will it read.
 * <app>
 *   <NameApp name="App1"/>
 *   <Version v="0.1"/>
 *   <AppID id="APPID0001"/>
 *   <Link link="http://xxx/.../name_of_ps3_pkg.pkg"></Link>
 *   <ImgLink link="http://xxx/.../lolol.png"></ImgLink>
 *   <Image img="lolol.png"></Image>
 *   <Type type="1"></Type>
 * </app>
 * 
 * <app>
 *  ... (SAME ABOVE)
 * </app>
 *  ...
 */


int LoadStringsFromXML(const char* filename, PSC_Application *App){
	IrrXMLReader* xml = createIrrXMLReader(filename);
	int i=-1;
	while(xml && xml->read()){
 		switch(xml->getNodeType())
		{
			case EXN_ELEMENT:
			{
				if (!strcmp("elements", xml->getNodeName())){
					App->n_elem  = xml->getAttributeValueAsInt("n");
					App->name      = new string[App->n_elem+1];
					App->version   = new string[App->n_elem+1];
					App->link      = new string[App->n_elem+1];
					App->pkg_file  = new string[App->n_elem+1];
					App->img_file  = new string[App->n_elem+1];
					App->type_file = new int[App->n_elem+1];
				}
				if (!strcmp("app", xml->getNodeName()))
					i++;
				if (!strcmp("NameApp", xml->getNodeName()))
					App->name[i]    = xml->getAttributeValue("name");
				if (!strcmp("Version", xml->getNodeName()))
					App->version[i] = xml->getAttributeValue("v");
				if (!strcmp("Link", xml->getNodeName()))
					App->link[i]    = xml->getAttributeValue("link");
				if (!strcmp("Pkg", xml->getNodeName()))
					App->pkg_file[i]    = xml->getAttributeValue("file");
				if (!strcmp("Image", xml->getNodeName()))
					App->img_file[i]    = xml->getAttributeValue("img");
				if (!strcmp("Type", xml->getNodeName()))
					App->type_file[i]   = xml->getAttributeValueAsInt("type");
				break;
			}
			case EXN_NONE:
				break;
			case EXN_ELEMENT_END:
				break;
			case EXN_TEXT:
				break;
			case EXN_COMMENT:
				break;
			case EXN_CDATA:
				break;
			case EXN_UNKNOWN:
				break;
		}
		if(i == App->n_elem)
			break;

	}
	if(i > App->n_elem)
		App->n_elem = i;

	App->name[App->n_elem] =      "";
	App->version[App->n_elem] =   "";
	App->link[App->n_elem] =      "";
	App->pkg_file[App->n_elem] =  "";
	App->img_file[App->n_elem] =  "";
	App->type_file[App->n_elem] = -1;
	delete xml;
	return 1;
}
