// Starts the plugin registration

#include "general.h"
#include <string.h>

// forward declarations
Bool Register_AMa_1dSnap(void);

C4D_CrashHandler old_handler;

void SDKCrashHandler(CHAR* crashinfo)
{
	//printf("SDK CrashInfo:\n");
	//printf(crashinfo);
	
	// don't forget to call the original handler!!!
	if (old_handler) (*old_handler)(crashinfo);
}

Bool PluginStart(void)
{
	// example of installing a crashhandler
	old_handler = C4DOS.CrashHandler; // backup the original handler (must be called!)
	C4DOS.CrashHandler = SDKCrashHandler; // insert the own handler

	C4DPL_CommandLineArgs args;
	if (GetCommandLineArgs(args))
	{
		GeConsoleOut("COMMANDLINE number of arguments: "+LongToString(args.argc)); 
	}

	return Register_AMa_1dSnap();
}

void PluginEnd(void)
{
}

Bool PluginMessage(LONG id, void *data)
{
	//use the following lines to set a plugin priority
	//
	switch (id)
	{
		case C4DPL_INIT_SYS:
			if (!resource.Init()) return FALSE; // don't start plugin without resource
			//if (!RegisterExampleDataType()) return FALSE;
			return TRUE;

		case C4DMSG_PRIORITY: 
			return TRUE;

		case C4DPL_COMMANDLINEARGS:
			{
				C4DPL_CommandLineArgs* args = (C4DPL_CommandLineArgs*)data;

				for (LONG i=0; i<args->argc; i++)
				{
					if (!args->argv[i]) continue;
					
					if (!strcmp(args->argv[i],"--help") || !strcmp(args->argv[i],"-help"))
					{
						// do not clear the entry so that other plugins can make their output!!!
						GePrint("\x01-SDK is here :-)");
					}
					else if (!strcmp(args->argv[i],"-SDK"))
					{
						args->argv[i] = NULL;
						GePrint("\x01-SDK executed:-)");
					}
					else if (!strcmp(args->argv[i],"-plugincrash"))
					{
						args->argv[i] = NULL;
						*((LONG*)0) = 1234;
					}
				}
			}
			break;

		case C4DPL_EDITIMAGE:
			{
				C4DPL_EditImage* editimage = (C4DPL_EditImage*)data;
				if (!data) break;
				if (editimage->return_processed) break;
				GePrint("C4DSDK - Edit Image Hook: "+editimage->imagefn->GetString());
				// editimage->return_processed = TRUE; if image was processed
			}
			return FALSE;
	}

	return FALSE;
}
