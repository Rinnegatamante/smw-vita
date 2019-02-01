
#ifdef _XBOX
	#include <xtl.h>
#endif

#ifdef _WIN32
	#ifndef _XBOX
		#define WIN32_LEAN_AND_MEAN
		#include <windows.h>
	#endif
#else
    #include <dirent.h>
#endif

#include "global.h"
#include "dirlist.h"
#include <ctype.h>
#include <algorithm>
#include <iostream>
using std::cout;
using std::endl;
using std::string;

extern const char * g_szMusicCategoryNames[MAXMUSICCATEGORY];
extern short g_iDefaultMusicCategory[MAXMUSICCATEGORY];

extern std::string stripPathAndExtension(const std::string &path);

///////////// SimpleFileList ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SimpleFileList::SimpleFileList()
{

}

SimpleFileList::~SimpleFileList()
{
	filelist.clear();
}

void SimpleFileList::init(const std::string &path, const std::string &extension, bool fAlphabetize)
{
	DirectoryListing d;
	d.init(path, extension);
	std::string curname;
	while(d(curname))
	{
		filelist.insert(filelist.end(), d.fullName(curname));
	}

	currentIndex = 0;

	if(filelist.empty())
	{
		//printf("ERROR: Empty directory!\n");
		//exit(0);
		currentIndex = -1;
	}

	//Alphabetize the list, ignoring author
	if(fAlphabetize)
	{
		short iSize = filelist.size();
		std::string * names = new std::string[iSize];

		//Get only the names of the files, no author information
		for(short i = 0; i < iSize; i++)
		{
			names[i] = stripPathAndExtension(filelist[i]);
			std::transform(names[i].begin(), names[i].end(), names[i].begin(), tolower);
		}

		//Now bubblesort them
		bool fDone = false;
		while(!fDone)
		{
			fDone = true;
			for(short i = 0; i < iSize - 1; i++)
			{
				if(names[i].compare(names[i + 1]) > 0)
				{
					fDone = false;
					std::string tempName = names[i];
					names[i] = names[i + 1];
					names[i + 1] = tempName;

					std::string tempFullName = filelist[i];
					filelist[i] = filelist[i + 1];
					filelist[i + 1] = tempFullName;
				}
			}
		}

		delete [] names;
	}
}

void SimpleFileList::next()
{
	if(filelist.empty())
		return;

	if(currentIndex + 1 == int(filelist.size()))
		currentIndex = 0;
	else
		currentIndex++;
}

void SimpleFileList::prev()
{
	if(filelist.empty())
		return;

	if(currentIndex == 0)
		currentIndex = filelist.size() - 1;
	else
		currentIndex--;
}

const char * SimpleFileList::GetIndex(unsigned int index)
{
	if(index < filelist.size())
		return filelist[index].c_str();

	return NULL;
}


void SimpleFileList::SetCurrentName(const std::string &name)
{
	if(filelist.empty())
		return;

	for(unsigned short i = 0; i < filelist.size(); i++)
	{
		if(!strcmp(filelist[i].c_str(), name.c_str()))
		{
			currentIndex = i;
			break;
		}
	}
}

///////////// SkinList ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SkinListNode::SkinListNode(std::string skinName, std::string skinPath)
{
	sSkinName = skinName;
	sSkinPath = skinPath;
}

		
SkinList::SkinList()
{

}

void SkinList::init()
{
	DirectoryListing d;
	d.init(convertPath("gfx/skins/"), ".bmp");
	std::string curname;
	while(d(curname))
	{
		std::string sShortSkinName = stripCreatorAndDotMap(curname);
		SkinListNode * node = new SkinListNode(sShortSkinName, d.fullName(curname));
		
		if(skins.empty())
		{
			skins.push_back(node);
		}
		else
		{
			std::vector<SkinListNode*>::iterator itr = skins.begin(), lim = skins.end();

			while(itr != lim)
			{
				if(sShortSkinName.compare((*itr)->sSkinName) < 0)
					break;

				itr++;
			}

			skins.insert(itr, node);
		}
	}
}

const char * SkinList::GetIndex(unsigned int index)
{
	if(index < skins.size())
		return skins[index]->sSkinPath.c_str();

	return NULL;
}

const char * SkinList::GetSkinName(unsigned int index)
{
	if(index < skins.size())
		return skins[index]->sSkinName.c_str();

	return NULL;
}

///////////// SimpleDirectoryList ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SimpleDirectoryList::SimpleDirectoryList()
{

}
void SimpleDirectoryList::init(const std::string &path)
{
	DirectoryListing d;
	d.init(path);
	std::string curname;
	while(d.NextDirectory(curname))
	{
		filelist.insert(filelist.end(), d.fullName(curname));
	}
	if(filelist.empty())
	{
		//printf("ERROR: Empty directory.  %s\n", path.c_str());
		//exit(0);
	}

	currentIndex = 0;
}


///////////// MusicList ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MusicList::MusicList()
{

}

MusicList::~MusicList()
{
	for(unsigned int i = 0; i < entries.size(); i++)
	{
		delete entries[i];
	}

	entries.clear();
}

void MusicList::init()
{
	DirectoryListing d;
	d.init(convertPath("music/game/"));
	std::string currentdir;
	while(d.NextDirectory(currentdir))
	{
		MusicEntry *m = new MusicEntry(d.fullName(currentdir));
		if (!m->fError)
			entries.push_back(m);
		else
			delete m;
	}
	
	if(entries.empty())
	{
		//printf("ERROR: Empty Music directory!\n");
		//exit(0);
	}

	currentIndex = 0;
}

string MusicList::GetMusic(int musicID)
{
	return entries[currentIndex]->GetMusic(musicID);
}

void MusicList::SetRandomMusic(int iMusicCategory, const char * szMapName, const char * szBackground)
{
	CurrentMusic = entries[currentIndex]->GetRandomMusic(iMusicCategory, szMapName, szBackground);
}

void MusicList::SetNextMusic(int iMusicCategory, const char * szMapName, const char * szBackground)
{
	CurrentMusic = entries[currentIndex]->GetNextMusic(iMusicCategory, szMapName, szBackground);
}

string MusicList::GetCurrentMusic()
{
	return CurrentMusic;
}


void MusicList::next()
{
	if(currentIndex+1 == int(entries.size()))
		currentIndex = 0;
	else
		currentIndex++;
}

void MusicList::prev()
{
	if(currentIndex == 0)
		currentIndex = entries.size()-1;
	else
		currentIndex--;
}

void MusicList::UpdateEntriesWithOverrides()
{
	for(unsigned short i = 0; i < entries.size(); i++)
	{
		entries[i]->UpdateWithOverrides();
	}
}


///////////// MusicEntry ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MusicEntry::MusicEntry(const std::string & musicdirectory)
{
	fError = false;
	fUsesMapOverrides = false;
	fUsesBackgroundOverrides = false;

	iCurrentMusic = 0;

	int i, k;

	char * szDir = (char*)(musicdirectory.c_str());
	
	for(k = 0; k < MAXMUSICCATEGORY; k++)
		numsongsforcategory[k] = 0;

	char musiclistname[FILEBUFSIZE];

	char cDirSeperator = getDirectorySeperator().c_str()[0];
    char * p = strrchr(szDir, cDirSeperator);
    if (!p) p=szDir; else p++;
    strcpy(musiclistname, p);

	for(i = (int)strlen(musiclistname); i >= 0; i--)
	{
		if(musiclistname[i] == '.')
		{
			musiclistname[i] = '\0';
			break;
		}
	}

	name = musiclistname;

	std::string musicfile = musicdirectory + getDirectorySeperator() + std::string("Music.txt");

	FILE * in = fopen(musicfile.c_str(), "r");

	if(!in)
	{
		printf("Error: Could not open: %s\n", musicfile.c_str());
		fError = true;
		return;
	}

	int iNumFile = 0;

	//Run through the global overrides and add them to the map overrides of this music entry
	//if there are any overrides then for each override
	/*
	{
		if(mapoverride.find(pszName) == mapoverride.end())
			mapoverride[pszName] = new MusicOverride();
	  
		songFileNames.push_back(sPath);

		fUsesMapOverrides = true;
		mapoverride[pszName]->songs.push_back(iNumFile);
		iNumFile++;
	}
	*/

	int iAddToCategory = -1;
	char szBuffer[256];
	while(fgets(szBuffer, 256, in))
	{
		//Ignore comment lines
		if(szBuffer[0] == '#' || szBuffer[0] == ' ' || szBuffer[0] == '\t' || szBuffer[0] == '\n' || szBuffer[0] == '\r')
			continue;

		//Chop off line ending
		int stringLength = strlen(szBuffer);
		for(k = 0; k < stringLength; k++)
		{
			if(szBuffer[k] == '\r' || szBuffer[k] == '\n')
			{
				szBuffer[k] = '\0';
				break;
			}
		}

		//If we found a category header
		if(szBuffer[0] == '[')
		{
			if(!_stricmp(szBuffer, "[land]"))
				iAddToCategory = 0;
			else if(!_stricmp(szBuffer, "[underground]"))
				iAddToCategory = 1;
			else if(!_stricmp(szBuffer, "[underwater]"))
				iAddToCategory = 2;
			else if(!_stricmp(szBuffer, "[castle]"))
				iAddToCategory = 3;
			else if(!_stricmp(szBuffer, "[platforms]"))
				iAddToCategory = 4;
			else if(!_stricmp(szBuffer, "[ghost]"))
				iAddToCategory = 5;
			else if(!_stricmp(szBuffer, "[bonus]"))
				iAddToCategory = 6;
			else if(!_stricmp(szBuffer, "[battle]"))
				iAddToCategory = 7;
			else if(!_stricmp(szBuffer, "[desert]"))
				iAddToCategory = 8;
			else if(!_stricmp(szBuffer, "[clouds]"))
				iAddToCategory = 9;
			else if(!_stricmp(szBuffer, "[snow]"))
				iAddToCategory = 10;
			else if(!_stricmp(szBuffer, "[maps]"))
				iAddToCategory = MAXMUSICCATEGORY;
			else if(!_stricmp(szBuffer, "[backgrounds]"))
				iAddToCategory = MAXMUSICCATEGORY + 1;

			continue;
		}

		//Cap the number of songs at MAXCATEGORYTRACKS for a category
		if(iAddToCategory > -1 && iAddToCategory < MAXMUSICCATEGORY && numsongsforcategory[iAddToCategory] >= MAXCATEGORYTRACKS)
			continue;

		if(iNumFile < 4 || iAddToCategory > -1)
		{
			if(iAddToCategory == MAXMUSICCATEGORY || iAddToCategory == MAXMUSICCATEGORY + 1)
			{
				char * pszName = strtok(szBuffer, ",\n");
				
				if(!pszName)
					continue;

				if(iAddToCategory == MAXMUSICCATEGORY)
				{
					if(mapoverride.find(pszName) == mapoverride.end())
						mapoverride[pszName] = new MusicOverride();
				}
				else
				{
					if(backgroundoverride.find(pszName) == backgroundoverride.end())
						backgroundoverride[pszName] = new MusicOverride();
				}

				char * pszMusic = strtok(NULL, ",\n");
				while(pszMusic)
				{
					std::string sPath = musicdirectory + getDirectorySeperator() + convertPartialPath(std::string(pszMusic));

					if(File_Exists(sPath.c_str()))
					{
						songFileNames.push_back(sPath);

						if(iAddToCategory == MAXMUSICCATEGORY)
						{
							fUsesMapOverrides = true;
							mapoverride[pszName]->songs.push_back(iNumFile);
						}
						else
						{
							fUsesBackgroundOverrides = true;
							backgroundoverride[pszName]->songs.push_back(iNumFile);
						}

						iNumFile++;
					}
					pszMusic = strtok(NULL, ",\n");
				}
			}
			else
			{
				std::string sPath = musicdirectory + getDirectorySeperator() + convertPartialPath(std::string(szBuffer));

				if(File_Exists(sPath.c_str()))
				{
					songFileNames.push_back(sPath);

					//Don't add to category lists if this is one of the four special music tracks
					if(iNumFile >= 4)
					{
						songsforcategory[iAddToCategory][numsongsforcategory[iAddToCategory]] = iNumFile;
						numsongsforcategory[iAddToCategory]++;
					}
					iNumFile++;
				}
			}
		}
	}

	fclose(in);

	//Now read labeled subdirectories like "Land", "Underground", "Castle", etc for more songs

	for(short iMusicCategory = 0; iMusicCategory < MAXMUSICCATEGORY; iMusicCategory++)
	{
		std::string musicPath = musicdirectory + getDirectorySeperator() + std::string(g_szMusicCategoryNames[iMusicCategory]);
		if(File_Exists(musicPath))
		{
			SimpleFileList musiclist;
			musiclist.init(musicPath + getDirectorySeperator(), ".ogg");

			short iCount = musiclist.GetCount();

			//printf("Found %d files in %s\n", iCount, musicPath.c_str());

			for(short iFile = 0; iFile < iCount; iFile++)
			{
				if(numsongsforcategory[iMusicCategory] < MAXCATEGORYTRACKS)
				{
					songFileNames.push_back(musiclist.current_name());
					songsforcategory[iMusicCategory][numsongsforcategory[iMusicCategory]] = iNumFile;
					numsongsforcategory[iMusicCategory]++;
					iNumFile++;
				}

				musiclist.next();
			}
		}
	}

	if(iNumFile == 0)
	{
		printf("Error: No songs found in: %s\n", musicdirectory.c_str());
		fError = true;
		return;
	}

	//Verify we have at least one track for each category
	for(i = 0; i < MAXMUSICCATEGORY; i++)
	{
		if(numsongsforcategory[i] == 0)
		{
			if(i < 4)
			{
				printf("Error: Missing track definition for music category: %s\n", g_szMusicCategoryNames[i]);
				fError = true;
				return;
			}
			else  //Use default category
			{
				numsongsforcategory[i] = numsongsforcategory[g_iDefaultMusicCategory[i]];
				for(k = 0; k < numsongsforcategory[i]; k++)
				{
					songsforcategory[i][k] = songsforcategory[g_iDefaultMusicCategory[i]][k];
				}
			}
		}
	}

	/*
	std::vector<std::string>::iterator itr = songFileNames.begin();

	printf("------ Songlist ------\n");
	short iCounter = 0;
	while(itr != songFileNames.end())
	{
		printf("%d: %s\n", iCounter++, itr->c_str());
		itr++;
	}

	printf("\n\n");

	for(i = 0; i < MAXMUSICCATEGORY; i++)
	{
		printf("---- %d ----\n", i);
		printf("Count: %d\n", numsongsforcategory[i]);
		for(k = 0; k < numsongsforcategory[i]; k++)
			printf("%d\n", songsforcategory[i][k]);
	}
	*/
}



string MusicEntry::GetMusic(unsigned int musicID)
{
    if (musicID >= songFileNames.size())
        return songFileNames[songFileNames.size()-1];

    return songFileNames[musicID];
}

string MusicEntry::GetRandomMusic(int iMusicCategory, const char * szMapName, const char * szBackground)
{
	//First check if there is specific map music
	if(fUsesMapOverrides && mapoverride.find(szMapName) != mapoverride.end())
	{
		if(mapoverride[szMapName]->songs.size() > 0)
		{
			iCurrentMusic = rand() % mapoverride[szMapName]->songs.size();
			return songFileNames[mapoverride[szMapName]->songs[iCurrentMusic]];
		}
	}
	//Then check if there is specific background music
	if(fUsesBackgroundOverrides && backgroundoverride.find(szBackground) != backgroundoverride.end())
	{
		if(backgroundoverride[szBackground]->songs.size() > 0)
		{
			iCurrentMusic = rand() % backgroundoverride[szBackground]->songs.size();
			return songFileNames[backgroundoverride[szBackground]->songs[iCurrentMusic]];
		}
	}

	//Then default to the music category
	if(iMusicCategory >= 0 && iMusicCategory < MAXMUSICCATEGORY && numsongsforcategory[iMusicCategory] > 0)
	{
		iCurrentMusic = rand() % numsongsforcategory[iMusicCategory];
		return songFileNames[songsforcategory[iMusicCategory][iCurrentMusic]];
	}

	return songFileNames[4];
}

string MusicEntry::GetNextMusic(int iMusicCategory, const char * szMapName, const char * szBackground)
{
	//First check if there is specific map music
	if(mapoverride.find(szMapName) != mapoverride.end())
	{
		if(mapoverride[szMapName]->songs.size() > 0)
		{
			if(++iCurrentMusic >= mapoverride[szMapName]->songs.size())
				iCurrentMusic = 0;

			return songFileNames[mapoverride[szMapName]->songs[iCurrentMusic]];
		}
	}
	//Then check if there is specific background music
	if(backgroundoverride.find(szBackground) != backgroundoverride.end())
	{
		if(backgroundoverride[szBackground]->songs.size() > 0)
		{
			if(++iCurrentMusic >= backgroundoverride[szBackground]->songs.size())
				iCurrentMusic = 0;

			return songFileNames[backgroundoverride[szBackground]->songs[iCurrentMusic]];
		}
	}

	//Then default to the music category
	if(iMusicCategory >= 0 && iMusicCategory < MAXMUSICCATEGORY && numsongsforcategory[iMusicCategory] > 0)
	{
		if(++iCurrentMusic >= numsongsforcategory[iMusicCategory])
			iCurrentMusic = 0;

		return songFileNames[songsforcategory[iMusicCategory][iCurrentMusic]];
	}

	return songFileNames[4];
}

void MusicEntry::UpdateWithOverrides()
{
	short iNumFile = songFileNames.size();

	if(mapmusicoverrides.size() > 0)
		fUsesMapOverrides = true;

	for(unsigned short i = 0; i < mapmusicoverrides.size(); i++)
	{
		MapMusicOverride * override = mapmusicoverrides[i];

		if(mapoverride.find(override->mapname) == mapoverride.end())
			mapoverride[override->mapname] = new MusicOverride();	

		for(unsigned short j = 0; j < override->songs.size(); j++)
		{
			songFileNames.push_back(override->songs[j]);
			mapoverride[override->mapname]->songs.push_back(iNumFile);
			iNumFile++;
		}
	}
}

///////////// MusicList ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
WorldMusicList::WorldMusicList()
{
	
}

WorldMusicList::~WorldMusicList()
{
	for(unsigned int i = 0; i < entries.size(); i++)
	{
		delete entries[i];
	}

	entries.clear();
}

void WorldMusicList::init()
{
	DirectoryListing d;
	d.init(convertPath("music/world/"));
	std::string currentdir;
	while(d.NextDirectory(currentdir))
	{
		WorldMusicEntry *m = new WorldMusicEntry(d.fullName(currentdir));
		if (!m->fError)
			entries.push_back(m);
		else
			delete m;
	}
	
	if(entries.empty())
	{
		//printf("ERROR: Empty Music directory!\n");
		//exit(0);
	}

	currentIndex = 0;
}

string WorldMusicList::GetMusic(int musicID, const char * szWorldName)
{
	return entries[currentIndex]->GetMusic(musicID, szWorldName);
}

string WorldMusicList::GetCurrentMusic()
{
	return CurrentMusic;
}


void WorldMusicList::next()
{
	if(currentIndex+1 == int(entries.size()))
		currentIndex = 0;
	else
		currentIndex++;
}

void WorldMusicList::prev()
{
	if(currentIndex == 0)
		currentIndex = entries.size()-1;
	else
		currentIndex--;
}

void WorldMusicList::UpdateEntriesWithOverrides()
{
	for(unsigned short i = 0; i < entries.size(); i++)
	{
		entries[i]->UpdateWithOverrides();
	}
}
		

///////////// WorldMusicEntry ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
WorldMusicEntry::WorldMusicEntry(const std::string & musicdirectory)
{
	fError = false;
	fUsesWorldOverrides = false;

	int i, k;

	char * szDir = (char*)(musicdirectory.c_str());
	
	char musiclistname[FILEBUFSIZE];

	char cDirSeperator = getDirectorySeperator().c_str()[0];
	char * p = strrchr(szDir, cDirSeperator);
    if (!p) p=szDir; else p++;
    strcpy(musiclistname, p);

	for(i = (int)strlen(musiclistname); i >= 0; i--)
	{
		if(musiclistname[i] == '.')
		{
			musiclistname[i] = '\0';
			break;
		}
	}

	name = musiclistname;

	std::string musicfile = musicdirectory + getDirectorySeperator() + std::string("Music.txt");

	FILE * in = fopen(musicfile.c_str(), "r");

	if(!in)
	{
		printf("Error: Could not open: %s\n", musicfile.c_str());
		fError = true;
		return;
	}

	int iAddToCategory = -1;
	char szBuffer[256];
	while(fgets(szBuffer, 256, in))
	{
		//Ignore comment lines
		if(szBuffer[0] == '#' || szBuffer[0] == ' ' || szBuffer[0] == '\t' || szBuffer[0] == '\n' || szBuffer[0] == '\r')
			continue;

		//Chop off line ending
		int stringLength = strlen(szBuffer);
		for(k = 0; k < stringLength; k++)
		{
			if(szBuffer[k] == '\r' || szBuffer[k] == '\n')
			{
				szBuffer[k] = '\0';
				break;
			}
		}

		//If we found a category header
		if(szBuffer[0] == '[')
		{
			if(!_stricmp(szBuffer, "[grass]"))
				iAddToCategory = 0;
			else if(!_stricmp(szBuffer, "[desert]"))
				iAddToCategory = 1;
			else if(!_stricmp(szBuffer, "[water]"))
				iAddToCategory = 2;
			else if(!_stricmp(szBuffer, "[giant]"))
				iAddToCategory = 3;
			else if(!_stricmp(szBuffer, "[sky]"))
				iAddToCategory = 4;
			else if(!_stricmp(szBuffer, "[ice]"))
				iAddToCategory = 5;
			else if(!_stricmp(szBuffer, "[pipe]"))
				iAddToCategory = 6;
			else if(!_stricmp(szBuffer, "[dark]"))
				iAddToCategory = 7;
			else if(!_stricmp(szBuffer, "[space]"))
				iAddToCategory = 8;
			else if(!_stricmp(szBuffer, "[bonus]"))
				iAddToCategory = WORLDMUSICBONUS;
			else if(!_stricmp(szBuffer, "[sleep]"))
				iAddToCategory = WORLDMUSICSLEEP;
			else if(!_stricmp(szBuffer, "[worlds]"))
				iAddToCategory = WORLDMUSICWORLDS;

			continue;
		}

		if(iAddToCategory > -1 && iAddToCategory <= WORLDMUSICSLEEP)
		{
			std::string sPath = musicdirectory + getDirectorySeperator() + convertPartialPath(std::string(szBuffer));

			if(File_Exists(sPath.c_str()))
				songFileNames[iAddToCategory] = sPath;
		}
		else if(iAddToCategory == WORLDMUSICWORLDS)
		{
			char * pszName = strtok(szBuffer, ",\n");
			
			if(!pszName)
				continue;

			char * pszMusic = strtok(NULL, ",\n");
			
			if(!pszMusic)
				continue;

			std::string sPath = musicdirectory + getDirectorySeperator() + convertPartialPath(std::string(pszMusic));

			if(!File_Exists(sPath.c_str()))
				continue;

			fUsesWorldOverrides = true;
			worldoverride[pszName] = sPath;
				
		}
	}

	fclose(in);
}



string WorldMusicEntry::GetMusic(unsigned int musicID, const char * szWorldName)
{
	//First check if there is specific map music
	if(fUsesWorldOverrides && worldoverride.find(szWorldName) != worldoverride.end())
	{
		return worldoverride[szWorldName];
	}

    if (musicID < 0 || musicID > WORLDMUSICSLEEP)
        return songFileNames[0];

    return songFileNames[musicID];
}

void WorldMusicEntry::UpdateWithOverrides()
{
	if(worldmusicoverrides.size() > 0)
		fUsesWorldOverrides = true;

	for(unsigned short i = 0; i < worldmusicoverrides.size(); i++)
	{
		WorldMusicOverride * override = worldmusicoverrides[i];
		worldoverride[override->worldname] = override->song;	
	}
}


