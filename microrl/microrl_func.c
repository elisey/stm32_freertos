#include <string.h>
#include "microrl_func.h"
#include "cmsis/core_cm3.h"

exFunc_t exFuncs[microrlNUM_OF_FUNC];
int exFuncsIdx = 0;

microrl_t rl;
microrl_t * prl = &rl;

static void prv_registerBaseFuncs();
static int prv_execute(int argc, const char * const * argv);
#ifdef _USE_COMPLETE
static char ** prv_complet (int argc, const char * const * argv);
#endif
#ifdef _USE_CTLR_C
static void prv_sigint (void);
#endif
static int prv_about(int argc, const char * const * argv);
static int prv_help(int argc, const char * const * argv);
static int prv_clear(int argc, const char * const * argv);

void microrl_run(void *pvParameters)
{
	prv_registerBaseFuncs();

	microrl_init(prl, microrl_sendString);
	microrl_set_execute_callback (prl, prv_execute);
#ifdef _USE_COMPLETE
	microrl_set_complete_callback (prl, prv_complet);
#endif
#ifdef _USE_CTLR_C
	microrl_set_sigint_callback (prl, prv_sigint);
#endif

	while (1) {
		microrl_insert_char (prl, microrl_getChar());
	}
}

void microrl_registerExecuteFunc(int (*func)(int, const char* const*), const char* name, const char* help)
{
	assert_param(exFuncsIdx < microrlNUM_OF_FUNC);

	exFuncs[exFuncsIdx].func = func;
	exFuncs[exFuncsIdx].name = name;
	exFuncs[exFuncsIdx].help = help;

	exFuncsIdx++;
}

static void prv_registerBaseFuncs()
{
	microrl_registerExecuteFunc(prv_about, "about", NULL);
	microrl_registerExecuteFunc(prv_help, "help", "This function allows to view the available options and their descriptions.");
	microrl_registerExecuteFunc(prv_clear, "clear", "This function clears the screen.");
}

static int prv_execute(int argc, const char * const * argv)
{
	if (argc > 0)	{
		int i;
		for (i = 0; i < exFuncsIdx; ++i) {
			if (strcmp(argv[0], exFuncs[i].name) == 0)	{
				return exFuncs[i].func(--argc, ++argv);
			}
		}
	}
	microrl_sendString("Unknown command. Type \"help\" to see available commands");
	microrl_sendString(ENDL);
	return 0;
}

#ifdef _USE_COMPLETE
static char ** prv_complet (int argc, const char * const * argv)
{
	static char * compl_world [microrlNUM_OF_FUNC + 1];
	int j = 0, i;
	compl_world[0] = NULL;
	if (argc == 1)	{
		char * bit = (char*)argv [argc-1];
		for (i = 0; i < exFuncsIdx; i++) {
			if (strstr(exFuncs[i].name, bit) == exFuncs[i].name) {
				compl_world [j++] = (char*)(exFuncs[i].name);
			}
		}
	}
	compl_world [j] = NULL;
	return compl_world;
}
#endif

#ifdef _USE_CTLR_C
static void prv_sigint (void)
{
	NVIC_SystemReset();
}
#endif
static int prv_about(int argc, const char * const * argv)
{
	microrl_sendString("Microrl based terminal. Ver. ");
	microrl_sendString(MICRORL_LIB_VER);
	microrl_sendString(ENDL);
	return 0;
}

static int prv_help(int argc, const char * const * argv)
{
	if (argc == 0)	{
		int i;
		microrl_sendString("type \"help\" <command> for more details.");
		microrl_sendString(ENDL);
		microrl_sendString(ENDL);
		microrl_sendString("Available commands:");
		microrl_sendString(ENDL);

		for (i = 0; i < exFuncsIdx; ++i) {
			microrl_sendString(exFuncs[i].name);
			microrl_sendString(" ");
		}
		microrl_sendString(ENDL);
		return 0;
	}

	if(argc == 1)	{
		int i;
		for (i = 0; i < exFuncsIdx; ++i) {
			if (strcmp(argv[0], exFuncs[i].name) == 0)	{
				if (exFuncs[i].help != NULL)	{
					microrl_sendString(exFuncs[i].help);
					microrl_sendString(ENDL);
					return 0;
				}
				else	{
					microrl_sendString("Help is not available for this command.");
					microrl_sendString(ENDL);
					return (-1);
				}
			}
		}
		microrl_sendString("Command not found.");
		microrl_sendString(ENDL);
		return (-1);
	}
	else {
		microrl_sendString("Help is available only for high-level commands. It isn't available for subcommands.");
		microrl_sendString(ENDL);
		return (-1);
	}

}

static int prv_clear(int argc, const char * const * argv)
{
	microrl_sendString ("\033[2J");    // ESC seq for clear entire screen
	microrl_sendString ("\033[H");     // ESC seq for move cursor at left-top corner
	return 0;
}
