/*!
 * \file data.h
 *
 * \par Copyright
 * Copyright (C) 1999-2023 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup data
 */
#ifndef _SCRATCH_DATA_H_
#define _SCRATCH_DATA_H_

#include <scratch/scratch.h>

/* Forward type declarations */
typedef struct Data Data;
typedef struct DataEntry DataEntry;

/*!
 * The data structure.
 * \addtogroup data
 * \{
 */
struct Data {
  DataEntry            *entries;        /*!< The entry list */
  size_t                entriesN;       /*!< The length of the entry list */
  char                 *value;          /*!< The scalar value */
};
/*! \} */

/*!
 * One data entry.
 * \addtogroup data
 * \{
 */
struct DataEntry {
  char                 *key;            /*!< The entry's key */
  Data                 *value;          /*!< The entry's value */
};
/*! \} */

/*!
 * Constructs a new data element.
 * \addtogroup data
 * \return the new data element
 * \sa DataFree(Data*)
 * \sa DataFreeV(void*)
 */
Data *DataAlloc(void);

/*!
 * Clears a data element.
 * \addtogroup data
 * \param d the data element to clear
 */
void DataClear(Data *d);

/*!
 * Opens a cursor over a data element.
 * \addtogroup data
 * \param d the data element to iterate
 * \param cursor the name of the cursor variable
 */
#define DataForEach(d, cursor) \
  for (DataEntry *cursor = (d) && (d)->entries ? (d)->entries : 0; \
		  cursor && cursor < (d)->entries + (d)->entriesN; ++cursor)

/*!
 * Frees a data element.
 * \addtogroup data
 * \param d the data element to free
 * \sa DataAlloc()
 * \sa DataFreeV(void*)
 */
void DataFree(Data *d);

/*!
 * Frees a data element.
 * \addtogroup data
 * \param d the data element to free
 * \sa DataAlloc()
 * \sa DataFree(Data*)
 */
void DataFreeV(void *d);

/*!
 * Searches for an entry and returns its value.
 * \addtogroup data
 * \param d the data element to search
 * \param key the key that identifies the entry whose value to return
 * \return the value of the entry indicated by the specified key or NULL
 */
Data *DataGet(
	Data *d,
	const char *key);

/*!
 * Searches for an entry and returns its value.
 * \addtogroup data
 * \param d the data element to search
 * \param key the key that identifies the entry whose value to return
 * \param format the sscanf-style format specifier
 * \return the number of values successfully scanned
 */
size_t DataGetFormatted(
	Data *d,
	const char *key,
	const char *format, ...)
	__attribute__ ((format (scanf, 3, 4)));

/*!
 * Searches for an entry and returns its value.
 * \addtogroup data
 * \param d the data element to search
 * \param key the key that identifies the entry whose value to return
 * \param defaultValue the value to return if the specified key
 *     cannot be resolved, or the value of the entry indicated by
 *     the specified key cannot be converted to the desired type
 * \return the value of the entry indicated by the specified key
 *     or the specified default value
 */
double DataGetNumber(
	Data *d,
	const char *key,
	const double defaultValue);

/*!
 * Searches for an entry and returns its value.
 * \addtogroup data
 * \param d the data element to search
 * \param key the key that identifies the entry whose value to return
 * \param defaultValue the value to return if the specified key
 *     cannot be resolved, or the value of the entry indicated by
 *     the specified key cannot be converted to the desired type
 * \return the value of the entry indicated by the specified key
 *     or the specified default value
 */
const char *DataGetString(
	Data *d,
	const char *key,
	const char *defaultValue);

/*!
 * Searches for an entry and returns its value.
 * \addtogroup data
 * \param d the data element to search
 * \param key the key that identifies the entry whose value to return
 * \param defaultValue the value to return if the specified key
 *     cannot be resolved, or the value of the entry indicated by
 *     the specified key cannot be converted to the desired type
 * \return the value of the entry indicated by the specified key
 *     or the specified default value
 */
char *DataGetStringCopy(
	Data *d,
	const char *key,
	const char *defaultValue);

/*!
 * Searches for an entry and returns its value.
 * \addtogroup data
 * \param d the data element to search
 * \param key the key that identifies the entry whose value to return
 * \param defaultValue the value to return if the specified key
 *     cannot be resolved, or the value of the entry indicated by
 *     the specified key cannot be converted to the desired type
 * \return the value of the entry indicated by the specified key
 *     or the specified default value
 */
time_t DataGetTime(
	Data *d,
	const char *key,
	const time_t defaultValue);

/*!
 * Searches for an entry and returns its value.
 * \addtogroup data
 * \param d the data element to search
 * \param key the key that identifies the entry whose value to return
 * \param names the newline-terminated list of names
 * \param defaultValue the value to return if the specified key
 *     cannot be resolved, or the value of the entry indicated by
 *     the specified key cannot be converted to the desired type
 * \return the value of the entry indicated by the specified key
 *     or the specified default value
 */
int DataGetType(
	Data *d,
	const char *key,
	const char *names[],
	const int defaultValue);

/*!
 * Searches for an entry and returns its value.
 * \addtogroup data
 * \param d the data element to search
 * \param key the key that identifies the entry whose value to return
 * \param defaultValue the value to return if the specified key
 *     cannot be resolved, or the value of the entry indicated by
 *     the specified key cannot be converted to the desired type
 * \return the value of the entry indicated by the specified key
 *     or the specified default value
 */
bool DataGetYesNo(
	Data *d,
	const char *key,
	const bool defaultValue);

/*!
 * Loads a data element.
 * \addtogroup data
 * \param fname the filename of the file to read
 * \return a data element representing the contents of
 *     the file indicated by the specified filename, or NULL
 * \sa DataLoadStream(FILE*)
 */
Data *DataLoadFile(const char *fname);

/*!
 * Loads a data element.
 * \addtogroup data
 * \param stream the stream to read
 * \return a data element representing the contents of
 *     the specified stream, or NULL
 * \sa DataLoadFile(const char*)
 */
Data *DataLoadStream(FILE *stream);

/*!
 * Inserts or updates an entry into a data element.
 * \addtogroup data
 * \param d the data element into which to insert an entry
 * \param key the key that identifies the entry to insert or update
 * \param value the value of the entry identified by the specified key
 * \return the new of updated data element or NULL
 */
Data *DataPut(
	Data *d,
	const char *key,
	Data *value);

/*!
 * Inserts or updates an entry into a data element.
 * \addtogroup data
 * \param d the data element into which to insert an entry
 * \param key the key that identifies the entry to insert or update
 * \param format the printf-style format specifier
 * \return the new of updated data element or NULL
 */
Data *DataPutFormatted(
	Data *d,
	const char *key,
	const char *format, ...)
	__attribute__ ((format (printf, 3, 4)));;


/*!
 * Inserts or updates an entry into a data element.
 * \addtogroup data
 * \param d the data element into which to insert an entry
 * \param key the key that identifies the entry to insert or update
 * \param value the value of the entry identified by the specified key
 * \return the new of updated data element or NULL
 */
Data *DataPutNumber(
	Data *d,
	const char *key,
	const double value);

/*!
 * Inserts or updates an entry into a data element.
 * \addtogroup data
 * \param d the data element into which to insert an entry
 * \param key the key that identifies the entry to insert or update
 * \param value the value of the entry identified by the specified key
 * \return the new of updated data element or NULL
 */
Data *DataPutString(
	Data *d,
	const char *key,
	const char *value);

/*!
 * Inserts or updates an entry into a data element.
 * \addtogroup data
 * \param d the data element into which to insert an entry
 * \param key the key that identifies the entry to insert or update
 * \param value the value of the entry identified by the specified key
 * \return the new of updated data element or NULL
 */
Data *DataPutTime(
	Data *d,
	const char *key,
	const time_t value);

/*!
 * Inserts or updates an entry into a data element.
 * \addtogroup data
 * \param d the data element into which to insert an entry
 * \param key the key that identifies the entry to insert or update
 * \param names the newline-terminated list of names
 * \param value the value of the entry identified by the specified key
 * \return the new of updated data element or NULL
 */
Data *DataPutType(
	Data *d,
	const char *key,
	const char *names[],
	const int value);

/*!
 * Inserts or updates an entry into a data element.
 * \addtogroup data
 * \param d the data element into which to insert an entry
 * \param key the key that identifies the entry to insert or update
 * \param value the value of the entry identified by the specified key
 * \return the new of updated data element or NULL
 */
Data *DataPutYesNo(
	Data *d,
	const char *key,
	const bool value);

/*!
 * Saves a data element.
 * \addtogroup data
 * \param d the data element to save
 * \param fname the filename of the file to write
 * \return true if the file indicated by the specified filename
 *     was successfully written
 * \sa DataSaveStream(const Data*, FILE*)
 */
bool DataSaveFile(
	Data *d,
	const char *fname);

/*!
 * Saves a data element.
 * \addtogroup data
 * \param d the data element to save
 * \param stream the stream to which to write
 * \return true if the specified stream was successfully written
 * \sa DataSaveFile(const Data*, const char*)
 */
bool DataSaveStream(
	Data *d,
	FILE *stream);

/*!
 * Gets the length of a data element.
 * \addtogroup data
 * \param d the data element whose length to return
 * \return the length of the specified data element or zero
 */
size_t DataSize(Data *d);

/*!
 * Sorts a data element.
 * \addtogroup data
 * \param d the data element whose keys to sort
 */
void DataSort(Data *d);

#endif /* _SCRATCH_DATA_H_ */
