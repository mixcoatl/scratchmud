/*!
 * \file editor.c
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup editor
 */
#define _SCRATCH_EDITOR_C_

#include <scratch/bitvector.h>
#include <scratch/client.h>
#include <scratch/color.h>
#include <scratch/editor.h>
#include <scratch/log.h>
#include <scratch/memory.h>
#include <scratch/scratch.h>
#include <scratch/state.h>
#include <scratch/string.h>

/* Local functions */
EDITORCOMMAND(EditorCommandAbort);
EDITORCOMMAND(EditorCommandClear);
EDITORCOMMAND(EditorCommandHelp);
EDITORCOMMAND(EditorCommandList);
EDITORCOMMAND(EditorCommandSave);

/*!
 * Aborts an editor session.
 * \addtogroup editor
 * \param client the client
 */
void EditorAbort(Client *client) {
  if (!client) {
    Log("invalid `client` Client");
  } else if (!client->editor) {
    Log("client %s has no editor", client->name);
  } else {
    /* Abort the edit session */
    if (client->editor->abortProc) {
      client->editor->abortProc(client, client->editor->text,
					client->editor->userData);
    }

    /* Editor cleanup */
    StringFree(client->editor->text);
    MemoryFree(client->editor);

    /* Show the prompt again */
    if (StateNeedsPrompt(client->state))
      BitSetN(client->flags, CLIENT_PROMPT);
  }
}

/*!
 * Adds editor content.
 * \addtogroup editor
 * \param client the client
 * \param text the editor content to add
 */
void EditorAdd(
	Client *client,
	const char *text) {
  if (!client) {
    Log("invalid `client` Client");
  } else if (!client->editor) {
    Log("client %s has no editor", client->name);
  } else if (!EditorAddInterpret(client, text)) {
    const size_t textlen = strlen(text);
    if (client->editor->textLength + textlen + 2 > client->editor->maximum - 1) {
      ClientPrint(client, "%sString too long.  Last line skipped.%s\r\n", Q_RED, Q_NORMAL);
      EditorFinish(client);
    } else {
      /* Copy new text into the editor */
      const ssize_t result = snprintf(
	client->editor->text + client->editor->textLength,
	client->editor->maximum - client->editor->textLength,
	"%s\r\n", text);

      /* Fix the length */
      if (result == textlen + 2)
	client->editor->textLength += result;

      /* Make sure it's correctly terminated */
      client->editor->text[client->editor->textLength] = '\0';

      /* Do we need to bail out now? */
      if (result != textlen + 2) {
	ClientPrint(client, "%sString too long.  Last line skipped.%s\r\n", Q_RED, Q_NORMAL);
	EditorFinish(client);
      }
    }
  }
}

/*!
 * Interprets an editor command.
 * \addtogroup editor
 * \param client the client
 * \param input the editor input to interpret
 * \return true if the specified text was successfully interpreted
 *     as an editor command
 */
bool EditorAddInterpret(
	Client *client,
	const char *input) {
  register bool result = false;
  if (!client) {
    Log("invalid `client` Client");
  } else if (!client->editor) {
    Log("client %s has no editor", client->name);
  } else if (*StringBlank(input) != '\0') {
    /* Read one argument */
    char arg[MAXLEN_INPUT] = {'\0'};
    input = StringOneWord(arg, sizeof(arg), input);

    /* Editor commands */
    static struct {
      const char             *name;
      const EditorCommandProc function;
    } commands[] = {
      /* 00 */ { "/Abort", EditorCommandAbort },
      /* 01 */ { "/Clear", EditorCommandClear },
      /* 02 */ { "/Help",  EditorCommandHelp },
      /* 03 */ { "/List",  EditorCommandList },
      /* 04 */ { "/Save",  EditorCommandSave },
	       { "\n",	   NULL }
    };

    /* Search for the editor command by name */
    register size_t commandN = 0;
    for (; *commands[commandN].name != '\n'; ++commandN) {
      if (StringPrefixCI(commands[commandN].name, arg))
	break;
    }

    if (*commands[commandN].name != '\n' && commands[commandN].function) { 
      commands[commandN].function(client, input);
      result = true;
    } else if (StringPrefixCI(arg, "/")) {
      ClientPrint(client, "%sUnknown editor command %s.%s\r\n", Q_RED, arg, Q_NORMAL);
      result = true;
    }
  }
  return (result);
}

/*!
 * The /ABORT editor command.
 * \addtogroup editor
 * \param client the client
 * \param input the command line input to the editor command
 */
EDITORCOMMAND(EditorCommandAbort) {
  EditorAbort(client);
}

/*!
 * The /CLEAR editor command.
 * \addtogroup editor
 * \param client the client
 * \param input the command line input to the editor command
 */
EDITORCOMMAND(EditorCommandClear) {
  /* Tell the player */
  ClientPrint(client, "%sString editor buffer cleared.%s\r\n", Q_CYAN, Q_NORMAL);

  /* Clear string editor buffer */
  MemoryZero(client->editor->text, char, client->editor->maximum + 1);
  client->editor->textLength = 0;
}

/*!
 * The /HELP editor command.
 * \addtogroup editor
 * \param client the client
 * \param input the command line input to the editor command
 */
EDITORCOMMAND(EditorCommandHelp) {
  ClientPrint(client,
	"%sString editor commands%s:%s\r\n"
	"%s/Abort  %sAborts string editor.%s\r\n"
	"%s/Clear  %sClears string editor buffer.%s\r\n"
	"%s/Help   %sPrints string editor commands.%s\r\n"
	"%s/List   %sPrints string editor buffer.%s\r\n"
	"%s/Save   %sSaves text and exits string editor.%s\r\n",
	Q_GREEN, Q_WHITE, Q_NORMAL,
	Q_RED, Q_GREEN, Q_NORMAL,
	Q_RED, Q_GREEN, Q_NORMAL,
	Q_RED, Q_GREEN, Q_NORMAL,
	Q_RED, Q_GREEN, Q_NORMAL,
	Q_RED, Q_GREEN, Q_NORMAL);
}

/*!
 * The /LIST editor command.
 * \addtogroup editor
 * \param client the client
 * \param input the command line input to the editor command
 */
EDITORCOMMAND(EditorCommandList) {
  if (*StringBlank(client->editor->text) == '\0') {
    ClientPrint(client, "%sString editor buffer is empty.%s\r\n", Q_CYAN, Q_NORMAL);
  } else {
    ClientPrint(client,
	"%sString editor buffer%s:%s\r\n"
	"%s%s%s",
	Q_CYAN, Q_WHITE, Q_NORMAL,
	Q_WHITE, StringBlank(client->editor->text), Q_NORMAL);
  }
}

/*!
 * The /SAVE editor command.
 * \addtogroup editor
 * \param client the client
 * \param input the command line input to the editor command
 */
EDITORCOMMAND(EditorCommandSave) {
  EditorFinish(client);
}

/*!
 * Finishes an editor session.
 * \addtogroup editor
 * \param client the client
 */
void EditorFinish(Client *client) {
  if (!client) {
    Log("invalid `client` Client");
  } else if (!client->editor) {
    Log("client %s has no editor", client->name);
  } else {
    /* Finish the edit session */
    if (client->editor->finishProc) {
      client->editor->finishProc(client, client->editor->text,
					 client->editor->userData);
    }

    /* Editor cleanup */
    StringFree(client->editor->text);
    MemoryFree(client->editor);

    /* Show the prompt again */
    if (StateNeedsPrompt(client->state))
      BitSetN(client->flags, CLIENT_PROMPT);
  }
}

/*!
 * Starts an editor session.
 * \addtogroup editor
 * \param client the client
 * \param text the initial editor content
 * \param maximum the maximum length of the editor content
 * \param abortProc the function to call when the editor is aborted
 * \param finishProc the function to call when the editor is finished
 * \param userData the user-specified data
 */
void EditorStart(
	Client *client,
	const char *text,
	const size_t maximum,
	const EditorProc abortProc,
	const EditorProc finishProc,
	void *userData) {
  if (!client) {
    Log("invalid `client` client");
  } else if (client->editor) {
    Log("client %s already has an editor", client->name);
    ClientPrint(client, "You're already editing something!\r\n");
  } else {
    /* Create and initialize editor */
    MemoryCreate(client->editor, Editor, 1);
    MemoryCreate(client->editor->text, char, maximum);
    client->editor->abortProc  = abortProc;
    client->editor->finishProc = finishProc;
    client->editor->textLength = 0;
    client->editor->maximum    = maximum;
    client->editor->userData   = userData;

    /* Some basic instructions */
    ClientPrint(client,
	"%sType %s/Save %sto save, %s/Abort %sto abort, or %s/Help %sfor additional commands.%s\r\n",
	Q_GREEN, Q_WHITE, Q_GREEN, Q_WHITE, Q_GREEN, Q_WHITE, Q_GREEN,
	Q_NORMAL);

    /* The initial editor content */
    if (*StringBlank(text) != '\0') {
      client->editor->textLength = strlen(text);
      if (client->editor->textLength > maximum - 1)
	client->editor->textLength = maximum - 1;

      strlcpy(client->editor->text, text, client->editor->maximum);
      ClientPrint(client, "%s", client->editor->text);
    }
  }
}
