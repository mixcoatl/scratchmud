/*!
 * \file data.c
 *
 * \par Copyright
 * Copyright (C) 1999-2023 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup data
 */
#define _SCRATCH_DATA_C_

#include <scratch/data.h>
#include <scratch/log.h>
#include <scratch/memory.h>
#include <scratch/scratch.h>
#include <scratch/string.h>

/*!
 * Constructs a new data element.
 * \addtogroup data
 * \return the new data element
 * \sa DataFree(Data*)
 * \sa DataFreeV(void*)
 */
Data *DataAlloc(void) {
  Data *d;
  MemoryCreate(d, Data, 1);
  d->entries  = NULL;
  d->entriesN = 0;
  d->value    = NULL;
  return (d);
}

/*!
 * Clears a data element.
 * \addtogroup data
 * \param d the data element to clear
 */
void DataClear(Data *d) {
  if (!d) {
    Log(L_ASSERT, "Invalid `d` Data.");
  } else {
    for (; d->entriesN; --d->entriesN) {
      StringFree(d->entries[d->entriesN - 1].key);
      DataFree(d->entries[d->entriesN - 1].value);
    }
    MemoryFree(d->entries);
    StringFree(d->value);
  }
}

/*!
 * Frees a data element.
 * \addtogroup data
 * \param d the data element to free
 * \sa DataAlloc()
 * \sa DataFreeV(void*)
 */
void DataFree(Data *d) {
  if (d) {
    DataClear(d);
    MemoryFree(d);
  }
}

/*!
 * Frees a data element.
 * \addtogroup data
 * \param d the data element to free
 * \sa DataAlloc()
 * \sa DataFree(Data*)
 */
void DataFreeV(void *d) {
  DataFree(d);
}

/*!
 * Searches for an entry and returns its value.
 * \addtogroup data
 * \param d the data element to search
 * \param key the key that identifies the entry whose value to return
t * \return the value of the entry indicated by the specified key or NULL
 */
Data *DataGet(
	Data *d,
	const char *key) {
  if (d && key && *key != '\0') {
    DataForEach(d, tEntry) {
      if (!StringCaseCompare(tEntry->key, key))
	return (tEntry->value);
    }
  }
  return (key && *key != '\0' ? NULL : d);
}

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
	const char *format, ...) {
  register int howMany = -1;
  register Data *found = DataGet(d, key);
  if (found && found->value && *found->value != '\0') {
    va_list args;
    va_start(args, format);
    howMany = vsscanf(found->value, format, args);
    va_end(args);
  }
  return (size_t) (howMany > 0 ? howMany : 0);
}

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
	const double defaultValue) {
  double value;
  return !DataGetFormatted(d, key, " %lg ", &value) ? defaultValue : value;
}

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
	const char *defaultValue) {
  register const char *result = defaultValue;
  register Data *found = DataGet(d, key);
  if (found && found->value && *found->value != '\0') {
    result = found->value;
  }
  return (result);
}

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
	const char *defaultValue) {
  const char *value = DataGetString(d, key, defaultValue);
  return value ? strdup(value) : NULL;
}

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
	const time_t defaultValue) {
  struct tm time;
  memset(&time, '\0', sizeof(time));

  /* Scan time fields from value */
  const size_t howMany = DataGetFormatted(d, key,
	" %d/%d/%d %d:%d:%d %d ",
	&time.tm_year, &time.tm_mon, &time.tm_mday,
	&time.tm_hour, &time.tm_min, &time.tm_sec,
	&time.tm_isdst);

  /* Valid when 3, 6, or 7 values provided */
  if (howMany != 3 && howMany != 6 && howMany != 7)
    return (defaultValue);

  time.tm_mon  -= 1;    /* Month must be 0-11 */
  time.tm_year -= 1900; /* Year must be 114 for 2014 */
  return mktime(&time);
}

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
	const int defaultValue) {
  if (!names) {
    Log(L_ASSERT, "Invalid `names` string array.");
  } else {
    const char *found = DataGetString(d, key, NULL);
    if (found && *found != '\0') {
      register size_t nameN = 0;
      for (; *names[nameN] != '\n'; ++nameN) {
	if (StringCaseCompare(names[nameN], found) == 0)
	  return (nameN);
      }
      Log(L_DATA, "Key `%s` has unknown type `%s`.", key, found);
    }
  }
  return (defaultValue);
}

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
	const bool defaultValue) {
  register bool result = defaultValue;
  register const char *found = DataGetString(d, key, NULL);
  if (found && *found != '\0') {
    double value = 0.0;
    if (sscanf(found, " %lg ", &value) == 1) {
      result = /* NaN */ value == value && value != 0.0;
    } else {
      /* Check for `Yes` */
      if (StringCaseCompare(found, "Y") == 0 ||
	  StringCaseCompare(found, "Yes") == 0)
	result = true;

      /* Check for `No` */
      if (StringCaseCompare(found, "N") == 0 ||
	  StringCaseCompare(found, "No") == 0)
	result = false;
    }
  }
  return (result);
}

/*!
 * Loads a data element.
 * \addtogroup data
 * \param fname the filename of the file to read
 * \return a data element representing the contents of
 *     the file indicated by the specified filename, or NULL
 * \sa DataLoadStream(FILE*)
 */
Data *DataLoadFile(const char *fname) {
  register Data *loaded = NULL;
  if (!fname || *fname == '\0') {
    Log(L_ASSERT, "Invalid `fname` string.");
  } else {
    FILE *stream = fopen(fname, "rt");
    if (!stream) {
      Log(L_DATA, "Couldn't open file %s for reading.", fname);
    } else {
      loaded = DataLoadStream(stream);
      fclose(stream);
    }
  }
  return (loaded);
}

/*! Data helper function. */
static Data *DataReadString(FILE *stream) {
  register Data *d = NULL;
  if (!stream) {
    Log(L_ASSERT, "Invalid `stream` FILE.");
  } else {
    char messg[MAXLEN_STRING] = {'\0'};
    register size_t messgpos =  0;

    while (!d) {
      int ch = fgetc(stream);
      if (ch == EOF)
	break;

      if (ch == '~') {
	ch = fgetc(stream);
	if (ch == '~') {
	  BPrintf(messg, sizeof(messg), messgpos, "%c", ch);
	} else {
	  while (ch != EOF && ch != '\n' && isspace(ch))
	    ch = fgetc(stream);

	  if (ch == EOF || ch == '\n') {
	    d = DataAlloc();
	    d->value = strdup(messg);
	  }
	}
      } else
	BPrintf(messg, sizeof(messg), messgpos, "%c", ch);
    }
    if (!d)
      Log(L_DATA, "Unexpected EOF while reading string: %s.", messg);
  }
  return (d);
}

/*! Data helper function. */
static Data *DataReadStringBlock(FILE *stream) {
  register Data *d = NULL;
  if (!stream) {
    Log(L_ASSERT, "Invalid 'stream' FILE.");
  } else {
    char messg[MAXLEN_STRING] = {'\0'};
    register size_t messgpos = 0;

    while (!d) {
      register int ch = fgetc(stream);
      if (ch == EOF)
	break;

      if (ch == '~') {
	ch = fgetc(stream);
	if (ch == '~') {
	  BPrintf(messg, sizeof(messg), messgpos, "%c", ch);
	} else {
	  while (ch != EOF && ch != '\n' && isspace(ch))
	    ch = fgetc(stream);

	  if (ch != EOF && ch != '\n') {
	    Log(L_DATA, "Missing EOF or EOL while reading string block.");
	    break;
	  }
	  /* Queue of string block lines */
	  char **lines = NULL;
	  register size_t linesN = 0;
	  MemoryRecreate(lines, char*, linesN + 1);
	  lines[linesN++] = messg;

	  /* Enqueue string block lines */
	  register const char *ptr = NULL;
	  for (ptr = messg; *ptr != '\0'; ++ptr) {
	    if (*ptr == '\n') {
	      MemoryRecreate(lines, char*, linesN + 1);
	      lines[linesN++] = (char*) ptr + 1;
	    }
	  }

	  /* Count least number of leading spaces */
	  register size_t fewestSpaces = (size_t) -1;
	  for (register size_t lineN = 0; lineN < linesN; ++lineN) {
	    register size_t currentSpaces = 0;
	    for (ptr = lines[lineN]; *ptr != '\0' && *ptr != '\n'; ++ptr) {
	      if (isspace((int) *ptr) && *ptr != '\r') {
		++currentSpaces;
	      } else if (*ptr != '\r')
		break;
	    }
	    if (fewestSpaces > currentSpaces || fewestSpaces == (size_t) -1)
	      fewestSpaces = currentSpaces;
	  }

	  /* Reconstruct the string block */
	  if (fewestSpaces && fewestSpaces != (size_t) -1) {
	    register size_t wmessgpos = 0;
	    for (register size_t lineN = 0; lineN < linesN; ++lineN) {
	      for (ptr = lines[lineN] + fewestSpaces; *ptr != '\0'; ++ptr) {
		if (*ptr == '\n')
		  BPrintf(messg, sizeof(messg), wmessgpos, "\r");
		if (*ptr != '\r')
		  BPrintf(messg, sizeof(messg), wmessgpos, "%c", *ptr);
		if (*ptr == '\n')
		  break;
	      }
	    }
	  }

	  /* Cleanup line queue */
	  MemoryFree(lines);

	  /* Create the data element */
	  d = DataAlloc();
	  d->value = strdup(messg);
	}
      } else if (ch != '\r') {
	if (ch == '\n')
	  BPrintf(messg, sizeof(messg), messgpos, "\r");
	BPrintf(messg, sizeof(messg), messgpos, "%c", ch);
      }
    }
  }
  return (d);
}

/*! Data helper function. */
static bool DataReadStructKey(
	char *key, const size_t keylen,
	FILE *stream) {
  register bool result = false;
  if (!key && keylen) {
    Log(L_ASSERT, "Invalid `key` buffer.");
  } else if (!stream) {
    Log(L_ASSERT, "Invalid `stream` FILE.");
  } else {
    register size_t keypos = 0;
    for (result = true; result; ) {
      const int ch = fgetc(stream);
      if (ch == EOF) {
	Log(L_DATA, "Unexpected EOF while reading structure key: %s.", key);
	result = false;
      } else if (ch == ':') {
	if (keypos == 0) {
	  Log(L_DATA, "Unexpected colon while reading structure key.");
	  result = false;
	} else
	  break;
      } else if (isalnum(ch) || ch == '_' || ch == '$') {
	BPrintf(key, keylen, keypos, "%c", ch);
      } else {
	Log(L_DATA, "Invalid '%c' while reading structure key: %s.", ch, key);
	result = false;
      }
    }
  }
  return (result);
}

/* Function prototype */
static Data *DataReadStructValue(FILE *stream);

/*! Data helper function. */
static Data *DataReadStruct(FILE *stream) {
  register Data *d = NULL;
  if (!stream) {
    Log(L_ASSERT, "Invalid 'stream' FILE.");
  } else {
    while (true) {
      int ch = fgetc(stream);
      if (ch == EOF)
	break;
      if (ch == '~')
	break;
      if (isalnum(ch) || ch == '_' || ch == '$') {
	if (ungetc(ch, stream) != ch) {
	  Log(L_SYSTEM, "ungetc() failed: errno=%d.", errno);
	  DataFree(d), d = NULL;
	  break;
	}
	char key[PATH_MAX] = {'\0'};
	if (!DataReadStructKey(key, sizeof(key), stream)) {
	  Log(L_DATA, "Couldn't read structure key.");
	  DataFree(d), d = NULL;
	  break;
	}
	Data *value = DataReadStructValue(stream);
	if (!value) {
	  Log(L_DATA, "Couldn't read structure value.");
	  DataFree(d), d = NULL;
	  break;
	}
	if (!d)
	  d = DataAlloc();

	if (DataPut(d, key, value) != value) {
	  Log(L_DATA, "Couldn't add structure value: %s.", key);
	  DataFree(d), d = NULL;
	  break;
	}
      } else if (!isspace(ch))
	break;
    }
  }
  return (d);
}

/*! Data helper function. */
static Data *DataReadStructValue(FILE *stream) {
  register Data *d = NULL;
  if (!stream) {
    Log(L_ASSERT, "Invalid `stream` FILE.");
  } else {
    register int ch = fgetc(stream);
    if (ch == EOF) {
      Log(L_DATA, "Unexpected EOF while reading structure value.");
    } else if (ch == '-') {
      ch = fgetc(stream);
      while (ch != EOF && ch != '\n' && isspace(ch))
	ch = fgetc(stream);

      if (ch == EOF) {
	Log(L_DATA, "Unexpected EOF while reading structure value.");
      } else if (ch != '\n') {
	Log(L_DATA, "Missing EOL while reading structure value.");
      } else {
	d = DataReadStringBlock(stream);
      }
    } else {
      while (ch != EOF && ch != '\n' && isspace(ch))
	ch = fgetc(stream);

      if (ch == EOF) {
	Log(L_DATA, "Unexpected EOF while reading structure value.");
      } else if (ch == '\n') {
	const int chSaved = ch;
	const long position = ftell(stream);
	while (isspace(ch) && ch != EOF)
	  ch = fgetc(stream);
	fseek(stream, position, SEEK_SET);
	ch = chSaved;
	d = DataReadStruct(stream);
      } else {
	if (ungetc(ch, stream) != ch) {
	  Log(L_SYSTEM, "ungetc() failed: errno=%d.", errno);
	} else
	  d = DataReadString(stream);
      }
    }
    if (!d)
      Log(L_DATA, "Error while reading structure value.");
  }
  return (d);
}

/*!
 * Loads a data element.
 * \addtogroup data
 * \param stream the stream to read
 * \return a data element representing the contents of
 *     the specified stream, or NULL
 * \sa DataLoadFile(const char*)
 */
Data *DataLoadStream(FILE *stream) {
  register Data *loaded = NULL;
  if (!stream) {
    Log(L_ASSERT, "Invalid `stream` FILE.");
  } else {
    loaded = DataReadStruct(stream);
  }
  return (loaded);
}

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
	Data *value) {
  if (!d) {
    Log(L_ASSERT, "Invalid `d` Data.");
    value = NULL;
  } else if (!key || *key == '\0') {
    Log(L_ASSERT, "Invalid `key` string.");
    value = NULL;
  } else if (!value) {
    Log(L_ASSERT, "Invalid `value` Data.");
  } else {
    /* Make copy of key */
    char realKey[PATH_MAX] = {'\0'};
    strlcpy(realKey, key, sizeof(realKey));

    /* Find next highest index */
    if (strcmp(realKey, "%") == 0) {
      register size_t highest = 0;
      DataForEach(d, tEntry) {
	size_t index = 0;
	if (sscanf(tEntry->key, " %zu ", &index) == 1)
	  highest = index > highest ? index : highest;
      }
      snprintf(realKey, sizeof(realKey), "%zu", highest + 1);
    }

    /* Search for entry */
    register size_t entryN = 0;
    for (; entryN < d->entriesN; ++entryN) {
      if (StringCaseCompare(d->entries[entryN].key, realKey) == 0)
	break;
    }

    /* Create new entry if the entry not found */
    if (entryN == d->entriesN) {
      MemoryRecreate(d->entries, DataEntry, d->entriesN + 1);
      d->entries[d->entriesN].key   = NULL;
      d->entries[d->entriesN].value = NULL;
      d->entriesN++;
    }

    /* Entry key */
    StringFree(d->entries[entryN].key);
    d->entries[entryN].key = strdup(realKey);

    /* Entry value */
    DataFree(d->entries[entryN].value);
    d->entries[entryN].value = value;
  }
  return (value);
}

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
	const char *format, ...) {
  register Data *result = NULL;
  if (!d) {
    Log(L_ASSERT, "Invalid `d` Data.");
  } else {
    va_list args;
    va_start(args, format);
    char messg[MAXLEN_STRING] = {'\0'};
    const int length = vsnprintf(messg, sizeof(messg), format, args);
    if (length >= 0 && (size_t) length < sizeof(messg) - 1) {
      result = DataPutString(d, key, messg);
    }
    va_end(args);
  }
  return (result);
}

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
	const double value) {
  register Data *result = NULL;
  if (!d) {
    Log(L_ASSERT, "Invalid `d` Data.");
  } else {
    result = DataPutFormatted(d, key, "%lg", value);
  }
  return (result);
}

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
	const char *value) {
  register Data *result = NULL;
  if (!d) {
    Log(L_ASSERT, "Invalid `d` Data.");
  } else {
    if (!key || *key == '\0') {
      result = d;
    } else {
      result = DataAlloc();
      if (DataPut(d, key, result) != result) {
	DataFree(result);
	result = NULL;
      }
    }
    if (result) {
      /* Copy value so DataClear is safe */
      char *valueCopy = strdup(value ? value : "");
      DataClear(result);

      /* Assign copied value */
      result->value = valueCopy;
    }
  }
  return (result);
}

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
	const time_t value) {
  register Data *result = NULL;
  if (!d) {
    Log(L_ASSERT, "Invalid `d` Data.");
  } else {
    struct tm time;
    if (localtime_r(&value, &time) != &time) {
      Log(L_SYSTEM, "localtime_r() failed: errno=%d.", errno);
    } else {
      result = DataPutFormatted(d, key,
	"%-4.4d/%-2.2d/%-2.2d %-2.2d:%-2.2d:%-2.2d %d",
	time.tm_year + 1900, time.tm_mon + 1, time.tm_mday,
	time.tm_hour, time.tm_min, time.tm_sec,
	time.tm_isdst);
    }
  }
  return (result);
}

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
	const int value) {
  register Data *result = NULL;
  if (!d) {
    Log(L_ASSERT, "Invalid `d` Data.");
  } else if (!names) {
    Log(L_ASSERT, "Invalid `names` string array.");
  } else {
    register size_t nameN = 0;
    for (; !result && *names[nameN] != '\n'; ++nameN) {
      if (value >= 0 && nameN == (size_t) value)
	result = DataPutString(d, key, names[nameN]);
    }
    if (!result && *names[nameN] == '\n')
      result = DataPutNumber(d, key, nameN);
  }
  return (result);
}

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
	const bool value) {
  register Data *result = NULL;
  if (!d) {
    Log(L_ASSERT, "Invalid `d` Data.");
  } else {
    result = DataPutString(d, key, value ? "Yes" : "No");
  }
  return (result);
}

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
	const char *fname) {
  register bool result = false;
  if (!d) {
    Log(L_ASSERT, "Invalid `d` Data.");
  } else if (!fname || *fname == '\0') {
    Log(L_ASSERT, "Invalid `fname` string.");
  } else {
    char tempfname[PATH_MAX] = {'\0'};
    if (snprintf(tempfname, sizeof(tempfname), "%s.tmp", fname) > 0) {
      FILE *stream = fopen(tempfname, "wt");
      if (!stream) {
	Log(L_DATA, "Couldn't open file `%s` for writing.", tempfname);
      } else {
	result = DataSaveStream(d, stream);
	fclose(stream);
	if (result && rename(tempfname, fname) != 0)
	  Log(L_SYSTEM, "rename() failed: errno=%d.", errno);
	if (unlink(tempfname) != 0 && errno != ENOENT)
	  Log(L_SYSTEM, "unlink() failed: errno=%d.", errno);
      }
    }
  }
  return (result);
}

/*! Data helper function. */
static bool DataWriteIndent(
	FILE *stream,
	const size_t indent) {
  bool result = false;
  if (!stream) {
    Log(L_ASSERT, "Invalid `stream` FILE.");
  } else {
    const int spaces = (int) indent * 2;
    if (fprintf(stream, "%*s", spaces, "") == spaces)
      result = true;
  }
  return (result);
}

/*! Data helper function. */
static bool DataWriteStructKey(
	FILE *stream,
	const char *key) {
  bool result = false;
  if (!stream) {
    Log(L_ASSERT, "Invalid `stream` FILE.");
  } else if (!key || *key == '\0') {
    Log(L_ASSERT, "Invalid `key` string.");
  } else {
    register const char *ptr = key;
    for (result = true; result && *ptr != '\0'; ++ptr) {
      if (result && fputc(*ptr, stream) == EOF)
	result = false;
    }
    if (result && fputc(':', stream) == EOF)
      result = false;
  }
  return (result);
}

/*! Data helper function. */
static bool DataWriteString(
	FILE *stream,
	const size_t indent,
	Data *d) {
  register bool result = false;
  if (!stream) {
    Log(L_ASSERT, "Invalid `stream` FILE.");
  } else if (!d) {
    Log(L_ASSERT, "Invalid `d` Data.");
  } else {
    register const char *ptr = d->value ? d->value : "";
    if (strchr(ptr, '\n') != NULL) {
      result = DataWriteIndent(stream, indent);
    } else {
      result = true;
      while (*ptr != '\0' && isspace((int) *ptr))
	++ptr;
    }
    for (; result && *ptr != '\0'; ptr++) {
      if (result && *ptr != '\r' && fputc(*ptr, stream) == EOF)
	result = false;
      if (result && *ptr == '~' && fputc(*ptr, stream) == EOF)
	result = false;
      if (result && *ptr == '\n')
	result = DataWriteIndent(stream, indent);
    }
    if (result && fprintf(stream, "~\n") != 2)
      result = false;
  }
  return (result);
}

/* Function prototype */
static bool DataWriteStructValue(FILE *stream, const size_t indent, Data *d);

/*! Data helper function. */
static bool DataWriteStruct(
	FILE *stream,
	const size_t indent,
	Data *d) {
  register bool result = false;
  if (!stream) {
    Log(L_ASSERT, "Invalid `stream` FILE.");
  } else if (!d) {
    Log(L_ASSERT, "Invalid `d` Data.");
  } else {
    result = true;
    DataForEach(d, tEntry) {
      result = result &&
	DataWriteIndent(stream, indent) &&
	DataWriteStructKey(stream, tEntry->key) &&
	DataWriteStructValue(stream, indent + 1, tEntry->value);
    }
    result = result && DataWriteIndent(stream, indent);
    result = result && fprintf(stream, "~\n") == 2;
  }
  return (result);
}

/*! Data helper function. */
static bool DataWriteStructValue(
	FILE *stream,
	const size_t indent,
	Data *d) {
  register bool result = false;
  if (!stream) {
    Log(L_ASSERT, "Invalid `stream` FILE.");
  } else if (!d) {
    Log(L_ASSERT, "Invalid `d` Data.");
  } else {
    if (d->entriesN) {
      if (fputc('\n', stream) != EOF)
	result = true;
      if (result)
	result = DataWriteStruct(stream, indent, d);
    } else {
      if (strchr(d->value ? d->value : "", '\n') != NULL) {
	if (fputc('-', stream) != EOF)
	  result = true;
	if (result && fputc('\n', stream) == EOF)
	  result = true;
      } else {
	if (fputc(' ', stream) != EOF)
	  result = true;
      }
      if (result)
	result = DataWriteString(stream, indent, d);
    }
  }
  return (result);
}

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
	FILE *stream) {
  register bool result = false;
  if (!stream) {
    Log(L_ASSERT, "Invalid `stream` FILE.");
  } else {
    result = DataWriteStruct(stream, 0, d);
  }
  return (result);
}

/*!
 * Gets the length of a data element.
 * \addtogroup data
 * \param d the data element whose length to return
 * \return the length of the specified data element or zero
 */
size_t DataSize(Data *d) {
  return (d ? d->entriesN : 0);
}

/*! Data helper function. */
static int DataSortFunc(const void *left, const void *right) {
  const DataEntry *leftEntry  = (const DataEntry*) left;
  const DataEntry *rightEntry = (const DataEntry*) right;
  return StringCaseCompare(leftEntry->key, rightEntry->key);
}

/*!
 * Sorts a data element.
 * \addtogroup data
 * \param d the data element whose keys to sort
 */
void DataSort(Data *d) {
  if (!d) {
    Log(L_ASSERT, "Invalid `d` Data.");
  } else {
    qsort(d->entries, d->entriesN, sizeof(DataEntry), DataSortFunc);
  }
}
