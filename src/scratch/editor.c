/*!
 * \file editor.h
 *
 * \par Copyright
 * Copyright (C) 1999-2023 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup editor
 */
#define _SCRATCH_EDITOR_C_

#include <scratch/color.h>
#include <scratch/descriptor.h>
#include <scratch/editor.h>
#include <scratch/log.h>
#include <scratch/memory.h>
#include <scratch/scratch.h>
#include <scratch/state.h>
#include <scratch/string.h>

/*!
 * Aborts a string editor.
 * \addtogroup editor
 * \param d the descriptor instance
 * \sa EditorFinish(Descriptor*)
 */
void EditorAbort(Descriptor *d) {
  if (!d) {
    Log(L_ASSERT, "Invalid `d` Descriptor.");
  } else if (!d->editor) {
    Log(L_ASSERT, "Descriptor %s has no string editor.", d->name);
  } else {
    /* Call aborted function */
    if (d->editor->aborted)
      d->editor->aborted(d, d->game, d->editor->string, d->editor->userData);

    /* Editor cleanup */
    EditorFree(d->editor);
    d->editor = NULL;

    /* Show prompt again */
    if (d->state && d->state->bits.prompt)
      d->bits.prompt = true;
  }
}

/*!
 * Adds string editor content.
 * \addtogroup editor
 * \param d the descriptor instance
 * \param str the string editor content to add
 */
void EditorAdd(
	Descriptor *d,
	const char *str) {
  if (!d) {
    Log(L_ASSERT, "Invalid `d` Descriptor.");
  } else if (!d->editor) {
    Log(L_ASSERT, "Descriptor %s has no string editor.", d->name);
  } else if (!EditorAddInterpret(d, str)) {
    /* Need length to test failure */
    const size_t stringN = d->editor->stringN;

    /* Add new string to string editor buffer */
    BPrintf(d->editor->string, d->editor->maximum, d->editor->stringN, "%s\r\n", str);

    /* Failed to add to string editor buffer */
    if (d->editor->stringN == stringN)
      DescriptorPrint(d, "%sString too long.  Last line skipped.%s\r\n", QX_FAILED, Q_NORMAL);
  }
}

/*! The /ABORT editor command. */
static EDITORCOMMAND(EditorCommandAbort) {
  EditorAbort(d);
}

/*! The /CLEAR editor command. */
static EDITORCOMMAND(EditorCommandClear) {
  /* Clear string editor buffer */
  MemoryZero(d->editor->string, char, d->editor->maximum + 1);
  d->editor->stringN = 0;

  /* Tell player */
  DescriptorPrint(d, "%sString editor buffer cleared.%s\r\n", QX_OKAY, Q_NORMAL);
}

/*! The /HELP editor command. */
static EDITORCOMMAND(EditorCommandHelp) {
  DescriptorPrint(d,
	"%sString editor commands:%s\r\n"
	" %s* %s/Abort %s- %sAborts string editor.%s\r\n"
	" %s* %s/Clear %s- %sClears string editor buffer.%s\r\n"
	" %s* %s/Help  %s- %sPrints string editor commands.%s\r\n"
	" %s* %s/List  %s- %sPrints string editor buffer.%s\r\n"
	" %s* %s/Save  %s- %sSaves text and exits string editor.%s\r\n",
	QX_PROMPT, Q_NORMAL,
	QX_PUNCTUATION, QX_EMPHASIS, QX_PUNCTUATION, QX_PROMPT, Q_NORMAL,
	QX_PUNCTUATION, QX_EMPHASIS, QX_PUNCTUATION, QX_PROMPT, Q_NORMAL,
	QX_PUNCTUATION, QX_EMPHASIS, QX_PUNCTUATION, QX_PROMPT, Q_NORMAL,
	QX_PUNCTUATION, QX_EMPHASIS, QX_PUNCTUATION, QX_PROMPT, Q_NORMAL,
	QX_PUNCTUATION, QX_EMPHASIS, QX_PUNCTUATION, QX_PROMPT, Q_NORMAL);
}

/*! The /LIST editor command. */
static EDITORCOMMAND(EditorCommandList) {
  if (!d->editor->stringN) {
    DescriptorPrint(d, "%sString editor buffer is empty.%s\r\n", QX_FAILED, Q_NORMAL);
  } else {
    DescriptorPrint(d,
	"%sString editor buffer:%s\r\n"
	"%s%s%s",
	QX_PROMPT, Q_NORMAL,
	QX_TEXT, d->editor->string, Q_NORMAL);
  }
}

/*! The /SAVE editor command. */
static EDITORCOMMAND(EditorCommandSave) {
  EditorFinish(d);
}

/*!
 * Interprets a string editor command.
 * \addtogroup editor
 * \param d the descriptor instance
 * \param input the string editor input to interpret
 * \return true if the specified input was successfully interpreted
 *     as a string editor command
 */
bool EditorAddInterpret(
	Descriptor *d,
	const char *input) {
  register bool result = false;
  if (!d) {
    Log(L_ASSERT, "Invalid `d` Descriptor.");
  } else if (!d->editor) {
    Log(L_ASSERT, "Descriptor %s has no string editor", d->name);
  } else if (input && *input != '\0') {
    /* Read command name */
    char name[MAXLEN_INPUT] = {'\0'};
    input = StringOneWord(name, sizeof(name), input);

    /* Editor commands */
    static struct {
      const char             *name;
      const EditorCommandFunc function;
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
      if (!StringCaseCompare(commands[commandN].name, name))
	break;
    }

    if (*commands[commandN].name != '\n' && commands[commandN].function) {
      commands[commandN].function(d, d->game, input);
      result = true;
    } else if (*name == '/') {
      DescriptorPrint(d, "%sUnknown %s string editor command.%s\r\n", QX_FAILED, name, Q_NORMAL);
      result = true;
    }
  }
  return (result);
}

/*!
 * Constructs a new string editor.
 * \addtogroup editor
 * \param maximum the maximum string length
 * \param aborted the function to call when the string editor aborts
 * \param finished the function to call when the string editor finishes
 * \param userData the user-specified data
 * \return the new string editor
 * \sa EditorFree(Editor*)
 * \sa EditorFreeV(void*)
 */
Editor *EditorAlloc(
	const size_t maximum,
	const EditorFunc aborted,
	const EditorFunc finished,
	void *userData) {
  Editor *editor;
  MemoryCreate(editor, Editor, 1);
  MemoryCreate(editor->string, char, maximum);
  editor->aborted = aborted;
  editor->finished = finished;
  editor->maximum = maximum;
  editor->stringN = 0;
  editor->userData = userData;
  return (editor);
}

/*!
 * Finishes a string editor.
 * \addtogroup editor
 * \param d the descriptor instance
 * \sa EditorAbort(Descriptor*)
 */
void EditorFinish(Descriptor *d) {
  if (!d) {
    Log(L_ASSERT, "Invalid `d` Descriptor.");
  } else if (!d->editor) {
    Log(L_ASSERT, "Descriptor %s has no string editor.", d->name);
  } else {
    /* Call aborted function */
    if (d->editor->finished)
      d->editor->finished(d, d->game, d->editor->string, d->editor->userData);

    /* Editor cleanup */
    EditorFree(d->editor);
    d->editor = NULL;

    /* Show prompt again */
    if (d->state && d->state->bits.prompt)
      d->bits.prompt = true;
  }
}

/*!
 * Frees a string editor.
 * \addtogroup editor
 * \param editor the string editor to free
 * \sa EditorAlloc()
 * \sa EditorFree(Editor*)
 */
void EditorFree(Editor *editor) {
  if (editor) {
    StringFree(editor->string);
    MemoryFree(editor);
  }
}

/*!
 * Frees a string editor.
 * \addtogroup editor
 * \param editor the string editor to free
 * \sa EditorAlloc()
 * \sa EditorFreeV(void*)
 */
void EditorFreeV(void *editor) {
  EditorFree(editor);
}

/*!
 * Starts a string editor.
 * \addtogroup editor
 * \param d the descriptor instance
 * \param str the string editor contents
 * \param maximum the maximum string length
 * \param aborted the function to call when the string editor aborts
 * \param finished the function to call when the string editor finishes
 * \param userData the user-specified data
 */
void EditorStart(
	Descriptor *d,
	const char *str,
	const size_t maximum,
	const EditorFunc aborted,
	const EditorFunc finished,
	void *userData) {
  if (!d) {
    Log(L_ASSERT, "Invalid `d` Descriptor.");
  } else if (d->editor) {
    Log(L_ASSERT, "Descriptor %s already has string editor.", d->name);
    DescriptorPrint(d, "%sYou're already editing something!%s\r\n", QX_FAILED, Q_NORMAL);
  } else {
    /* Create string editor */
    d->editor = EditorAlloc(maximum, aborted, finished, userData);

    /* Some basic instructions */
    DescriptorPrint(d,
	"%sType %s/Save %sto save, %s/Abort %sto abort, or %s/Help %sfor more commands.%s\r\n",
	QX_PROMPT, QX_EMPHASIS,
	QX_PROMPT, QX_EMPHASIS,
	QX_PROMPT, QX_EMPHASIS,
	QX_PROMPT, Q_NORMAL);

    /* The initial editor content */
    if (str && *str != '\0') {
      d->editor->stringN = strlen(str);
      if (d->editor->stringN > maximum - 1)
	d->editor->stringN = maximum - 1;

      strlcpy(d->editor->string, str, d->editor->maximum);
      DescriptorPrint(d, "%s%s%s", QX_TEXT, d->editor->string, Q_NORMAL);
    }
  }
}
