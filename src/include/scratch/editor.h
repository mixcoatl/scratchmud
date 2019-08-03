/*!
 * \file editor.h
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup editor
 */
#ifndef _SCRATCH_EDITOR_H_
#define _SCRATCH_EDITOR_H_

#include <scratch/scratch.h>

/* Forward type declarations */
typedef struct _Client Client;
typedef struct _Editor Editor;

/*!
 * Declares an editor callback.
 * \addtogroup editor
 * \param name the name of the function
 */
#define EDITOR(name) \
  void (name)(Client *client, const char *text, void *userData)

/*!
 * Declares an editor command.
 * \addtogroup editor
 * \param name the name of the function
 */
#define EDITORCOMMAND(name) \
  void (name)(Client *client, const char *input)

/*!
 * The type of an editor command function.
 * \addtogroup editor
 */
typedef EDITORCOMMAND(*EditorCommandProc);

/*!
 * The type of an editor callback.
 * \addtogroup editor
 */
typedef EDITOR(*EditorProc);

/*!
 * The editor structure.
 * \addtogroup editor
 * \{
 */
struct _Editor {
  EditorProc    abortProc;      /*!< Called when editor is aborted */
  EditorProc    finishProc;     /*!< Called when editor is finished */
  size_t        maximum;        /*!< Maximum length of string */
  char         *text;           /*!< The editor content */
  size_t        textLength;     /*!< The length of the editor content */
  void         *userData;       /*!< The user-specified data */
};
/*! \} */

/*!
 * Aborts an editor session.
 * \addtogroup editor
 * \param client the client
 */
void EditorAbort(Client *client);

/*!
 * Adds editor content.
 * \addtogroup editor
 * \param client the client
 * \param text the editor content to add
 */
void EditorAdd(
	Client *client,
	const char *text);

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
	const char *input);

/*!
 * Finishes an editor session.
 * \addtogroup editor
 * \param client the client
 */
void EditorFinish(Client *client);

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
	void *userData);

#endif /* _SCRATCH_EDITOR_H_ */
