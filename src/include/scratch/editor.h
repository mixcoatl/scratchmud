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
#ifndef _SCRATCH_EDITOR_H_
#define _SCRATCH_EDITOR_H_

#include <scratch/scratch.h>

/* Forward type declarations */
typedef struct Descriptor Descriptor;
typedef struct Editor Editor;
typedef struct Game Game;

/*!
 * Declares an editor function.
 * \addtogroup editor
 */
#define EDITOR(name) \
  void (name)(Descriptor *d, Game *game, const char *text, void *userData)

/*!
 * Declares an editor command function.
 * \addtogroup editor
 */
#define EDITORCOMMAND(name) \
  void (name)(Descriptor *d, Game *game, const char *input)

/*!
 * The type of an editor command function.
 * \addtogroup editor
 */
typedef EDITORCOMMAND(*EditorCommandFunc);

/*!
 * The type of an editor function.
 * \addtogroup editor
 */
typedef EDITOR(*EditorFunc);

/*!
 * The editor structure.
 * \addtogroup editor
 * \{
 */
struct Editor {
  EditorFunc            aborted;        /*!< Called when editor is aborted */
  EditorFunc            finished;       /*!< Called when editor is finished */
  size_t                maximum;        /*!< The maximum string length */
  char                 *string;         /*!< The string edit buffer */
  size_t                stringN;        /*!< The string edit buffer used */
  void                 *userData;       /*!< The user-specified data */
};
/*! \} */

/*!
 * Aborts a string editor.
 * \addtogroup editor
 * \param d the descriptor instance
 * \sa EditorFinish(Descriptor*)
 */
void EditorAbort(Descriptor *d);

/*!
 * Adds string editor content.
 * \addtogroup editor
 * \param d the descriptor instance
 * \param str the string editor content to add
 */
void EditorAdd(
	Descriptor *d,
	const char *str);

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
	const char *input);

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
	void *userData);

/*!
 * Finishes a string editor.
 * \addtogroup editor
 * \param d the descriptor instance
 * \sa EditorAbort(Descriptor*)
 */
void EditorFinish(Descriptor *d);

/*!
 * Frees a string editor.
 * \addtogroup editor
 * \param editor the string editor to free
 * \sa EditorAlloc()
 * \sa EditorFree(Editor*)
 */
void EditorFree(Editor *editor);

/*!
 * Frees a string editor.
 * \addtogroup editor
 * \param editor the string editor to free
 * \sa EditorAlloc()
 * \sa EditorFreeV(void*)
 */
void EditorFreeV(void *editor);

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
	void *userData);

#endif /* _SCRATCH_EDITOR_H_ */
