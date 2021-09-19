#include "save_functions.h"

#include "satiator_functions.h"

SAVES g_Saves[MAX_SAVES];

// save name and savefilename are not the same thing
char saveFilename[MAX_FILENAME]; // file name
char saveName[MAX_SAVE_FILENAME]; // selected save file name
char saveComment[MAX_SAVE_COMMENT]; // selected save comment
unsigned char saveLanguage; // selected save language
unsigned int saveDate; // selected save date;
unsigned int saveFileSize; // selected save file size
PBUP_HEADER saveBupHeader; // the bup header. Immediately following is the saveFileData
unsigned char* saveFileData; // the raw save data


void initSaves()  // initialization routine for Save subsystem currently does not handle errors but if any errors occur the per-game save option should be disabled.
{
    enum SAVE_ERROR_CODE ret;
    // initialize globals
    jo_memset(g_Saves, 0, sizeof(g_Saves));
    // theres no point in allocing more than the 32KB limit of the sram. 
    saveBupHeader = jo_malloc(sizeof(BUP_HEADER) + 32 * 1024);
    if(saveBupHeader == NULL)
    {
        return;
    }
    saveFileData = (unsigned char*)(saveBupHeader + 1);
    unsigned int saveDataSlot = 0;
    ret = saveReadGameIdInInternalMemory(gameId, &saveDataSlot);
    switch (ret)
    {
        case SAVE_ERROR:
            return;
        case SAVE_NO_GAMEID_FOUND:
            // No game id found in the SRAM so these saves are not tied to a game run by Rings

            break;
        case SAVE_SUCCESS:
            // a game was running on previous boot and we need to save its SRAM state to its folder.

            // create a savedir in case this is a different card than the one used to boot the game previously
            saveCreateSaveDirectory(gameId, saveDataSlot);
            
            saveClearSaveDirectory(gameId, saveDataSlot);

            ret = saveCopyInternalMemoryToSaveDirectory(gameId, saveDataSlot);
            if ((int)ret == SATIATOR_WRITE_ERR) 
            {
                return;
            }
            // now that we've copied over the per-game SRAM contents to its respective dir
            // clear the SRAM and place back the original values of the SRAM before we ran the game
            ret = saveClearInternalMemory();
            if (ret != SAVE_SUCCESS) 
            {
                return;
            }
            ret = saveCreateSaveDirectory("_BACKUP", 0);
            if ((int)ret == SATIATOR_WRITE_ERR)
            {
                return;
            }
            ret = saveCopySaveDirectoryToInternalMemory("_BACKUP", 0);
            if (ret != SAVE_SUCCESS)
            {
                return;
            }
            break;
        default: //should not be possible
            break;
    }

    return;
}

// convert the game id and save slot number into a path
char * getPathForGameId(int slot,char * gameid)
{
    char * savepath = jo_malloc(SAVE_MAXDIRPATHSIZE);
    strcpy(savepath,SAVE_FOLDERNAME);
    strcat(savepath,"/");
    if(slot > 0)
    {
        strcat(savepath,"[SLOT");
        char slotNum[4];
        sprintf(slotNum, "%02d", slot);
        strcat(savepath, slotNum);
        strcat(savepath,"]");
        strcat(savepath,"/");
    }
    strcat(savepath,gameid);
    return savepath;
}

// Create save subdirectory for the provided gameid in the save directory
// returns success if the gameid provided is empty, otherwise returns satiatorCreateDirectory's error code
enum SATIATOR_ERROR_CODE saveCreateSaveDirectory(char* gameid, int slot) 
{
   
    enum SATIATOR_ERROR_CODE ret = 0;
    
    if (gameid[0] != '\0') 
    {
        char * savepath = jo_malloc(SAVE_MAXDIRPATHSIZE);
        strcpy(savepath,SAVE_FOLDERNAME);
        strcat(savepath,"/");
        if(slot > 0)
        {
            strcat(savepath,"[SLOT");
            char slotNum[4];
            sprintf(slotNum, "%02d", slot);
            strcat(savepath, slotNum);
            strcat(savepath,"]");
            ret = satiatorCreateDirectory(savepath);
            if(ret < 0)
            {
                jo_free(savepath);
                return ret;
            }
            strcat(savepath,"/");
        }
        strcat(savepath,gameid);  
        ret = satiatorCreateDirectory(savepath);
        jo_free(savepath);
    }
    return ret;
}

// Generic function to copy all saves from one save medium to another. hardcoded to search through a subdirectory based on gameid
// in the DEFINE'd save directory location. 
enum SAVE_ERROR_CODE saveBulkCopyBetweenDevices(int sourceDevice, int destinationDevice, char * gameid, int slot) 
{
    enum SAVE_ERROR_CODE result = SAVE_SUCCESS;
    int ret;
    int bytecount = 0; // count of the total transfered bytes in copy operation
    
    char * savepath = getPathForGameId(slot, gameid);
    
    // create a savedir in case this is a different card than the one used to boot the game previously
    saveCreateSaveDirectory(gameId, slot);   
    s_chdir(savepath);

    int count = 0;
    count = listSaveFiles(sourceDevice, g_Saves, COUNTOF(g_Saves));
    if (count < 0)
    {
        result = SAVE_ERROR;
    }
    
    for (int i = 0; i < count; i++)
    {

        if (!strcmp(g_Saves[i].name,SAVE_MAGICFILENAME))
        {
            continue; // we skip the save indicator file itself during copy      
        }
        // UGLY CODE WARNING: these explicit checks were done as the SGC code requires a filename
        //                    instead of save name for files stored on satiator
        if (sourceDevice == SatiatorBackup) 
        {
            ret = readSaveFile(sourceDevice, g_Saves[i].filename, (unsigned char*)saveBupHeader, g_Saves[i].datasize + sizeof(BUP_HEADER));
        }
        else
        {
            ret = readSaveFile(sourceDevice, g_Saves[i].name, (unsigned char*)saveBupHeader, g_Saves[i].datasize + sizeof(BUP_HEADER));
        }
        if (ret != 0) 
        {
            continue; 
        }
        bytecount += g_Saves[i].datasize;
        if (bytecount >= 32 * 1024)
        {
            result = SAVE_OUT_OF_SPACE;
            break;
        }
        if (destinationDevice == SatiatorBackup) 
        {
            ret = writeSaveFile(destinationDevice, g_Saves[i].filename, (unsigned char*)saveBupHeader, g_Saves[i].datasize + sizeof(BUP_HEADER));
        }
        else 
        {
            ret = writeSaveFile(destinationDevice, g_Saves[i].name, (unsigned char*)saveBupHeader, g_Saves[i].datasize + sizeof(BUP_HEADER));
        }
       
        if (ret != 0) 
        {
            result = SAVE_ERROR;
            break;
        }
    }
    jo_free(savepath);
    
    s_chdir(currentDirectory);
    return result;
}

// Deletes all files in a folder
// This could be done faster by interleaving the list operations loop and the delete call, doing this manually by using satiator calls or 
enum SAVE_ERROR_CODE saveBulkDeleteDevice(int targetDevice,char * gameid, int slot) 
{
    enum SAVE_ERROR_CODE result = SAVE_SUCCESS;
    int ret;

    if (targetDevice == SatiatorBackup)
    {
        if (gameid == NULL)
        {
            result = SAVE_ERROR; 
            return result;
        }
        char * savepath = getPathForGameId(slot, gameid);   
        s_chdir(savepath);
        jo_free(savepath);
    }

    int count = 0;
    count = listSaveFiles(targetDevice, g_Saves, COUNTOF(g_Saves));
    if (count < 0)
    {
        result = SAVE_ERROR;
    }
    
    for (int i = 0; i < count; i++)
    {
        
        
        if (targetDevice == SatiatorBackup) 
        {
            ret = deleteSaveFile(targetDevice,  g_Saves[i].filename);
        }
        else
        {
            ret = deleteSaveFile(targetDevice, g_Saves[i].name);
        }
        if (ret != 0) 
        {
            result = SAVE_ERROR;
            continue; //if we couldnt delete a specific entry report a failure but attempt to 
        }

    }

    if (targetDevice == SatiatorBackup)
    {
        s_chdir(currentDirectory);
    }
    return result;
}

enum SAVE_ERROR_CODE saveClearInternalMemory()
{
    return saveBulkDeleteDevice(JoInternalMemoryBackup,NULL,0);
}

enum SAVE_ERROR_CODE saveClearSaveDirectory(char * gameid, int slot)
{
    return saveBulkDeleteDevice(SatiatorBackup,gameid, slot);
}

enum SAVE_ERROR_CODE saveCopyInternalMemoryToSaveDirectory(char * gameid, int slot)
{
    return saveBulkCopyBetweenDevices(JoInternalMemoryBackup,SatiatorBackup,gameid, slot);
}

enum SAVE_ERROR_CODE saveCopySaveDirectoryToInternalMemory(char * gameid, int slot)
{
    return saveBulkCopyBetweenDevices(SatiatorBackup,JoInternalMemoryBackup,gameid, slot);
}

enum SAVE_ERROR_CODE saveReadGameIdInInternalMemory(char * gameid, unsigned int * slot)
{
    int count = 0;
    strcpy(gameid,"");
    count = listSaveFiles(JoInternalMemoryBackup, g_Saves, COUNTOF(g_Saves));
    if (count < 0)
    {
        return SAVE_ERROR;
    }
    //Search for a savefile that has the magic filename
    for (int i = 0; i < count; i++)
    {
        //found the save entry
        if (!strcmp(g_Saves[i].name,SAVE_MAGICFILENAME))
        {     
            //extract gameid from the save entry comment metadata
            strncpy(gameid,g_Saves[i].comment,MAX_SAVE_COMMENT);
            unsigned int dt = g_Saves[i].date;
            *slot = 0;
            if((dt != 0x014BDE33) && (dt < MAX_SAVE_SLOTS)) // 0x014BDE33 = old random date before slot usage
                *slot = dt;
            return SAVE_SUCCESS;
        }
    }
    // did not find it
    return SAVE_NO_GAMEID_FOUND;
}

enum SAVE_ERROR_CODE saveSaveGameIdToInternalMemory(char * gameid, int slot)
{
    int ret;
    // initialize saveBupHeader and the 1 ""data"" byte
    jo_memset(saveBupHeader, 0, sizeof(BUP_HEADER)+SAVE_MAGICFILESIZE);

    memcpy(saveBupHeader->magic, VMEM_MAGIC_STRING, VMEM_MAGIC_STRING_LEN);
    strncpy((char*)saveBupHeader->dir.filename, SAVE_MAGICFILENAME, MAX_SAVE_FILENAME);
    saveBupHeader->dir.filename[MAX_SAVE_FILENAME - 1] = '\0'; // null termination for the homies <is this needed?>

    strncpy((char*)saveBupHeader->dir.comment,gameid,MAX_SAVE_COMMENT);
    saveBupHeader->dir.comment[MAX_SAVE_COMMENT - 1] = '\0';
    saveBupHeader->dir.datasize = SAVE_MAGICFILESIZE; //0x33
    
    saveBupHeader->dir.blocksize = SAVE_MAGICBLOCKSIZE; // this was the value used by Ultraman in its 0x33 datasize save so i copied that
    saveBupHeader->dir.language = backup_english;
    saveBupHeader->dir.date = (unsigned int)slot; // Currently set to a random date. Should use this field for future multi save slot designation
    saveBupHeader->date = saveBupHeader->dir.date;
    //strncpy((char*)saveFileData,"TESTTESTTESTTEST",SAVE_MAGICFILESIZE); // additional metadata about the game and save folder can be stored here
    ret = writeSaveFile(JoInternalMemoryBackup, SAVE_MAGICFILENAME, (unsigned char*) saveBupHeader, sizeof(BUP_HEADER) + SAVE_MAGICFILESIZE);
    if (ret != 0)
    {
        return SAVE_ERROR;
    }
    return SAVE_SUCCESS;
}
