#include <string.h>
#include "microrl_func.h"
#include "cmsis/core_cm3.h"

terminalFunc_t terminalFuncArray[microrlNUM_OF_TERMINAL_FUNC];
int terminalFuncArrayIdx = 0;

microrl_t rl;
microrl_t * prl = &rl;

static void prv_registerBasicTerminalFuncs();
static void prv_sendStringNewLine(const char *str);
static void prv_sendNewLine();
static int prv_getFuncIndex(const char * name);
static void prv_printMainHelp();
static void prv_printTerminalFuncHelp(const char *name);

static int prv_execute(int argc, const char * const * argv);
#ifdef _USE_COMPLETE
static char ** prv_complet (int argc, const char * const * argv);
#endif
#ifdef _USE_CTLR_C
static void prv_sigint (void);
#endif

//Базовые терминальные функции
static int prv_TerminalFunc_about(int argc, const char * const * argv);
static int prv_TerminalFunc_help(int argc, const char * const * argv);
static int prv_TerminalFunc_clear(int argc, const char * const * argv);



void microrl_run(void *pvParameters)
{
	prv_registerBasicTerminalFuncs();

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
	assert_param(terminalFuncArrayIdx < microrlNUM_OF_TERMINAL_FUNC);

	terminalFuncArray[terminalFuncArrayIdx].func = func;
	terminalFuncArray[terminalFuncArrayIdx].name = name;
	terminalFuncArray[terminalFuncArrayIdx].help = help;

	terminalFuncArrayIdx++;
}

static void prv_registerBasicTerminalFuncs()
{
	microrl_registerExecuteFunc(prv_TerminalFunc_about, "about", NULL);
	microrl_registerExecuteFunc(prv_TerminalFunc_help, "help", "This function allows to view the available options and their descriptions.");
	microrl_registerExecuteFunc(prv_TerminalFunc_clear, "clear", "This function clears the screen.");
}

static void prv_sendStringNewLine(const char *str)
{
	microrl_sendString(str);
	microrl_sendString(ENDL);
}

static void prv_sendNewLine()
{
	microrl_sendString(ENDL);
}

static int prv_getFuncIndex(const char * name)
{
	int i;
	for (i = 0; i < terminalFuncArrayIdx; ++i) {
		if (strcmp(name, terminalFuncArray[i].name) == 0)	{
			return i;
		}
	}
	return (-1);
}

static int prv_execute(int argc, const char * const * argv)
{
	int funcIndex;

	funcIndex = prv_getFuncIndex(argv[0]);
	if (funcIndex == (-1))	{
		prv_sendStringNewLine("Unknown command. Type \"help\" to see available commands");
		return (-1);
	}
	return terminalFuncArray[funcIndex].func(--argc, ++argv);
}

#ifdef _USE_COMPLETE
static char ** prv_complet (int argc, const char * const * argv)
{
	static char * compl_world [microrlNUM_OF_TERMINAL_FUNC + 1];
	int j = 0, i;
	compl_world[0] = NULL;
	if (argc == 1)	{
		char * bit = (char*)argv [argc-1];
		for (i = 0; i < terminalFuncArrayIdx; i++) {
			if (strstr(terminalFuncArray[i].name, bit) == terminalFuncArray[i].name) {	//FIXME попробовать использовать prv_getFuncIndex
				compl_world [j++] = (char*)(terminalFuncArray[i].name);
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

static int prv_TerminalFunc_about(int argc, const char * const * argv)
{
	microrl_sendString("Microrl based terminal. Ver. ");
	prv_sendStringNewLine(MICRORL_LIB_VER);
	return 0;
}

static int prv_TerminalFunc_help(int argc, const char * const * argv)
{
	switch (argc)
	{
	case 0:
		prv_printMainHelp();
		break;
	case 1:
		prv_printTerminalFuncHelp(argv[0]);
		break;
	default:
		prv_sendStringNewLine("Help is available only for high-level commands. It isn't available for subcommands.");
		break;
	}
	return 0;
}

static void prv_printMainHelp()
{
	prv_sendStringNewLine("type \"help\" <command> for more details.");
	prv_sendNewLine();
	prv_sendStringNewLine("Available commands:");
	int i;
	for (i = 0; i < terminalFuncArrayIdx; ++i) {
		microrl_sendString(terminalFuncArray[i].name);
		microrl_sendString(" ");
	}
	prv_sendNewLine();
}

static void prv_printTerminalFuncHelp(const char *name)
{
	int funcIndex = prv_getFuncIndex(name);
	if (funcIndex == (-1))	{
		prv_sendStringNewLine("Command not found.");
		return;
	}
	if (terminalFuncArray[funcIndex].help != NULL)	{
		prv_sendStringNewLine(terminalFuncArray[funcIndex].help);
	}
	else	{
		prv_sendStringNewLine("Help is not available for this command.");
	}
}

static int prv_TerminalFunc_clear(int argc, const char * const * argv)
{
	microrl_sendString ("\033[2J");    // ESC seq for clear entire screen
	microrl_sendString ("\033[H");     // ESC seq for move cursor at left-top corner
	return 0;
}
