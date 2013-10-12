#include "application.h"
#include "core_util.h"

#define CACHE_PATH2			"/dev_hdd0/game/PSCHANNEL/CACHE"

static string update = "";

const char* get_update_message(){
	return update.c_str();
}

void LoadConfig(const char* filename, int *current_lang){
	fstream f;
	f.open(filename,ios::in);
	if(f.fail())
		return;
	f >> (*current_lang);

	f.close();
}

void SaveConfig(const char* filename, int *current_lang){
	fstream f;
	f.open(filename,ios::out|ios::trunc);
	if(f.fail())
		return;
	f << (*current_lang);

	f.close();
}

int LoadStrings(const char* filename, string *str, int n){
	fstream f;
	f.open(filename,ios::in);
	if(f.fail())
		return -1;
	for(int i=0;i<n && !f.eof();i++)
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


int LoadStringsFromXML(const char* filename, PSC_Application2 *App){
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

int progress(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow){ //not really used.
	return 0;
}

int BrewologyLoadStringsFromXML(const char* filename, PSC_Application *App){
	IrrXMLReader* xml = createIrrXMLReader(filename);
	App->n_elem    = 1014;
	App->name      = new string[1020];
	App->version   = new string[1020];
	App->author    = new string[1020];
	App->img_file  = new string[1020];
	App->type_file = new int   [1020];
	App->pkg       = new links [1020];

/*
	App->link      = new string[1020];
	App->pkg_file  = new string[1020];
	App->file_size = new u32   [1020];
*/
	int i=-1;
	bool name_app = false;
	App->downloaded = false;
	while(xml && xml->read()){
 		switch(xml->getNodeType())
		{
			case EXN_NONE:
				break;
			case EXN_ELEMENT:
			{
				if (!strcmp("error", xml->getNodeName())){
					xml->read();
					xml->read();
					update = xml->getNodeData();
					return PSC_OLDVERSION;
				}
				if (!strcmp("homebrew", xml->getNodeName())){
					++i;
					App->type_file[i] = PSC_PACKAGE;
					break;
				}else if (!strcmp("name", xml->getNodeName())){ //homebrew name
					xml->read();
					App->name[i] = xml->getNodeData();
					break;
				}else if (!strcmp("author", xml->getNodeName())){
					xml->read();
					App->author[i] = xml->getNodeData();
					break;
				}else if (!strcmp("icon", xml->getNodeName())){
					xml->read();
					App->img_file[i] = xml->getNodeData();
					break;
				}else if (!strcmp("pkgs", xml->getNodeName())){
					string name[50];
					string link[50];
					u32    size[50];
					u8     pkg=0;
					do{
						switch(xml->getNodeType()){
							case EXN_ELEMENT:
							{
								if (!strcmp("name", xml->getNodeName())){
									xml->read();
									name[pkg] = xml->getNodeData();
								}else if (!strcmp("version", xml->getNodeName())){
									xml->read();
									App->version[i] = xml->getNodeData();
									break;
								}else if (!strcmp("size", xml->getNodeName())){
									xml->read();
									size[pkg] = atoi(xml->getNodeData());
								}else if (!strcmp("location", xml->getNodeName())){
									xml->read();
									link[pkg] = xml->getNodeData();
									++pkg;
								}
							}break;
							case EXN_ELEMENT_END:
							{
								if(!strcmp("pkgs", xml->getNodeName()))
									goto back;
							}break;
						}
					}while(pkg<50 && xml->read());
					back:
					
					App->pkg[i].pkg_num = pkg;
					App->pkg[i].link    = new string [pkg];
					App->pkg[i].name    = new string [pkg];
					App->pkg[i].size    = new u32    [pkg];
					
					for(u8 j=0; j<pkg; ++j){
						App->pkg[i].link[j] = link[j];
						App->pkg[i].size[j] = size[j];
						App->pkg[i].name[j] = name[j];
						App->pkg[i].name[j] += ".pkg";
					}
					
					break;
				}

				break;
			}
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
	i++;
	App->n_elem = i;
	for(;i < 1020;++i){
		App->name[i]        = "";
		App->author[i]      = "";
		App->version[i]     = "";
	//	App->link[i]        = "";
	//	App->pkg_file[i]    = "";
		App->img_file[i]    = "";
	//	App->file_size[i]   =  0;
		App->type_file[i]   = -1;
		App->pkg[i].pkg_num =  0;
		
	}

	delete xml;
	return 1;
}

void BrewologyDownloadImages(PSC_Application *App, const char* text, void (*handler)(const char *msg)){
	if(!App->downloaded){
		string link;
		for(int i=0 ; i < App->n_elem ; ++i){
			char perc[128];
			snprintf(perc,128,"%s %.2f %%", text, i*100.f / App->n_elem);
			if(handler!=NULL)
				handler(perc);
			link = App->img_file[i].c_str();
			App->img_file[i] =  CACHE_PATH2"/";
			App->img_file[i] += App->name[i];
			App->img_file[i] += ".png";
			download_file(link.c_str(),App->img_file[i].c_str(), progress, NULL);
			if(get_size(App->img_file[i].c_str()) == 161)
				download_file("www.your.website.com/default.png",App->img_file[i].c_str(), progress, NULL);
			App->img_file[i] =  App->name[i];
			App->img_file[i] += ".png";
		}
	}
	App->downloaded = 1;
}

void FreePSCApplication(PSC_Application *App){
	delete [] App->name;
	delete [] App->author;
	delete [] App->version;
//	delete [] App->link;
//	delete [] App->pkg_file;
	delete [] App->img_file;
//	delete [] App->file_size;
	delete [] App->type_file;
	
	delete [] App->pkg;
}

void FreePSNUpdate(PSN_Update *App){
	delete [] App;
}


int checkUpdate(const char* TITLE_ID, int env_choose){
	if((TITLE_ID[0]=='N' && TITLE_ID[1]=='P') ||
	   (TITLE_ID[0]=='B' && TITLE_ID[1]=='L') ||
	   (TITLE_ID[0]=='B' && TITLE_ID[1]=='C') ||
	   (TITLE_ID[0]=='X' && TITLE_ID[1]=='C') ||
	   (TITLE_ID[0]=='U' && TITLE_ID[1]=='L') ||
	   (TITLE_ID[0]=='M' && TITLE_ID[1]=='R')){

		const char *env[3] = {"np","sp-int","prod-qa"};
		int size = strlen(TITLE_ID)*2+49+strlen(env[env_choose])*2;
		char *xml_url = (char*) malloc(size);

		snprintf(xml_url,size,"https://a0.ww.%s.dl.playstation.net/tpl/%s/%s/%s-ver.xml",env[env_choose],env[env_choose],TITLE_ID,TITLE_ID);
		download_game_update(xml_url,CACHE_PATH2"/game-update.xml", progress, NULL);
		free((void*)xml_url);
	
		size = get_size(CACHE_PATH2"/game-update.xml");
		if(size == 16 || size == 241 || size == 0){ //Update not found
			file_delete(CACHE_PATH2"/game-update.xml");
			return 0;
		}
	
		if(get_download_status()==DOWNLOAD_FAILED){
			file_delete(CACHE_PATH2"/game-update.xml");
			return 0;
		}
		file_delete(CACHE_PATH2"/game-update.xml");
		return 1;
	}else 
		return 0;
}


int updateFinder(const char* TITLE_ID, int env_choose, PSN_Update* game){
	const char *env[3] = {"np","sp-int","prod-qa"};
	int size = strlen(TITLE_ID)*2+49+strlen(env[env_choose])*2;
	char *xml_url = (char*) malloc(size);

	snprintf(xml_url,size,"https://a0.ww.%s.dl.playstation.net/tpl/%s/%s/%s-ver.xml",env[env_choose],env[env_choose],TITLE_ID,TITLE_ID);
	download_game_update(xml_url,CACHE_PATH2"/game-update.xml", progress, NULL);
	free((void*)xml_url);
	
	size = get_size(CACHE_PATH2"/game-update.xml");
	if(size == 16 || size == 241 || size == 0){ //Update not found
		file_delete(CACHE_PATH2"/game-update.xml");
		return 0;
	}
	
	if(get_download_status()==DOWNLOAD_FAILED){
		file_delete(CACHE_PATH2"/game-update.xml");
		return 0;
	}
	
	
	IrrXMLReader* xml = createIrrXMLReader(CACHE_PATH2"/game-update.xml");
	int i=-1;
	PSN_Update game_tmp;
	game_tmp.version = new string  [10];
	game_tmp.link    = new string  [10];
	game_tmp.size    = new uint32_t[10];
	game_tmp.min_ver = new float   [10];
	game_tmp.pkgs	 = 0;
	while(xml && xml->read() && i<10){
		//printf("debug: %s\n", xml->getNodeData());
 		switch(xml->getNodeType())
		{
			case EXN_NONE:
				break;
			case EXN_ELEMENT:
			{
				if (!strcmp("package", xml->getNodeName())){
					++i;
					char tmp[10];
					game_tmp.version[i] = xml->getAttributeValue("version");
					game_tmp.size[i]    = xml->getAttributeValueAsInt("size");
					game_tmp.link[i]    = xml->getAttributeValue("url");
					game_tmp.min_ver[i] = xml->getAttributeValueAsFloat("ps3_system_ver");
					game_tmp.pkgs++;
					break;
				}else if (!strcmp("TITLE", xml->getNodeName())){
					xml->read();
					game_tmp.name = xml->getNodeData();
					break;
				}
			}
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
	game->name    = game_tmp.name.c_str();
	game->version = new string  [game_tmp.pkgs];
	game->link    = new string  [game_tmp.pkgs];
	game->size    = new uint32_t[game_tmp.pkgs];
	game->min_ver = new float   [game_tmp.pkgs];
	game->pkgs    = game_tmp.pkgs;
	for(u8 j=0; j<game_tmp.pkgs; ++j){
		game->version[j] = game_tmp.version[j];
		game->link[j]    = game_tmp.link[j];
		game->size[j]    = game_tmp.size[j];
		game->min_ver[j] = game_tmp.min_ver[j];
	}
	
	delete [] game_tmp.version;
	delete [] game_tmp.link;
	delete [] game_tmp.size;
	
	file_delete(CACHE_PATH2"/game-update.xml");
	return 1;
}



