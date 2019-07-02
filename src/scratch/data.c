/*!
 * \file data.c
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
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
 */
Data *DataAlloc(void) {
  Data *d = NULL;
  MemoryCreate(d, Data, 1);
  return (d);
}

/*!
 * Clears a data element.
 * \addtogroup data
 * \param d the data element to clear
 */
static void DataClear(Data *d) {
  if (!d) {
    Log("invalid `d` Data");
  } else {
    register size_t j = 0;
    for (j = 0; j < d->entryListN; ++j) {
      MemoryFree(d->entryList[j].key);
      DataFree(d->entryList[j].value);
    }
    MemoryFree(d->value);
    MemoryFree(d->entryList);
    memset(d, '\0', sizeof(Data));
  }
}

/*!
 * Frees a data element.
 * \addtogroup data
 * \param d the data element to free
 * \sa DataAlloc()
 */
void DataFree(Data *d) {
  if (d) {
    DataClear(d);
    MemoryFree(d);
  }
}

/*!
 * Searches for an entry and returns its value.
 * \addtogroup data
 * \param d the data element to search
 * \param key the key identifying the entry whose value to return
 * \return the value of the entry indicated by the specified key or NULL
 */
Data *DataGet(
	Data *d,
	const char *key) {
  register Data *result = NULL;
  if (d && *StringBlank(key) != '\0') {
    register size_t j = 0;
    for (j = 0; !result && d && j < d->entryListN; ++j) {
      if (StringCmpCI(d->entryList[j].key, key) == 0)
	result = d->entryList[j].value;
    }
  } else
    result = d;
  return (result);
}

/*!
 * Searches for an entry and returns its value.
 * \addtogroup data
 * \param d the data element to search
 * \param key the key identifying the entry whose value to return
 * \param nameList the newline-terminated list of names
 * \param defaultValue the value to return if the specified key
 *     cannot be resolved, or the value of the entry indicated by
 *     the specified key cannot be converted to the desired type
 * \return the value of the entry indicated by the specified key
 *     or the specified default value
 */
Bitvector DataGetBits(
	Data *d,
	const char *key,
	const char *nameList[],
	const Bitvector defaultValue) {
  register Bitvector result = defaultValue;
  if (!nameList) {
    Log("invalid `nameList` string array");
  } else {
    register Data *found = DataGet(d, key);
    if (found) {
      register size_t j = 0;
      for (j = 0, result = 0; *nameList[j] !=  '\n'; ++j) {
	BitRemoveN(result, j);
	if (DataGetYesNo(found, nameList[j], false))
	  BitSetN(result, j);
      }
    }
  }
  return (result);
}

/*!
 * Searches for an entry and returns its value.
 * \addtogroup data
 * \param d the data element to search
 * \param key the key identifying the entry whose value to return
 * \param format the sscanf-style format specifier
 * \return the number of values successfully scanned or -1
 */
ssize_t DataGetFormatted(
	Data *d,
	const char *key,
	const char *format, ...) {
  register ssize_t result = -1;
  register Data *found = DataGet(d, key);
  if (found && *StringBlank(found->value) != '\0') {
    va_list args;
    va_start(args, format);
    result = vsscanf(found->value, format, args);
    va_end(args);
  }
  return (result);
}

/*!
 * Searches for an entry and returns its value.
 * \addtogroup data
 * \param d the data element to search
 * \param key the key identifying the entry whose value to return
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
  double result = defaultValue;
  if (DataGetFormatted(d, key, " %lg ", &result) != 1) {
    result = defaultValue;
  }
  return (result);
}

/*!
 * Searches for an entry and returns its value.
 * \addtogroup data
 * \param d the data element to search
 * \param key the key identifying the entry whose value to return
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
  if (found && *StringBlank(found->value) != '\0') {
    result = StringBlank(found->value);
  }
  return (result);
}

/*!
 * Searches for an entry and returns its value.
 * \addtogroup data
 * \param d the data element to search
 * \param key the key identifying the entry whose value to return
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
  register char *result = (char*) defaultValue;
  register Data *found = DataGet(d, key);
  if (found && *StringBlank(found->value) != '\0') {
    result = StringBlank(found->value);
  }
  if (result)
    result = strdup(result);

  return (result);
}

/*!
 * Searches for an entry and returns its value.
 * \addtogroup data
 * \param d the data element to search
 * \param key the key identifying the entry whose value to return
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

  /* Scan time fields from the value */
  const ssize_t N = DataGetFormatted(d, key,
	" %d-%d-%d %d:%d:%d %d ",
	&time.tm_year, &time.tm_mon, &time.tm_mday,
	&time.tm_hour, &time.tm_min, &time.tm_sec,
	&time.tm_isdst);

  /* Valid when 3, 6, or 7 values provided */
  register time_t result = defaultValue;
  if (N == 3 || N == 6 || N == 7) {
    time.tm_mon  -= 1;    /* Month must be 0-11 */
    time.tm_year -= 1900; /* Year must be 114 for 2014 */
    result = mktime(&time);
  }
  return (defaultValue);
}

/*!
 * Searches for an entry and returns its value.
 * \addtogroup data
 * \param d the data element to search
 * \param key the key identifying the entry whose value to return
 * \param nameList the newline-terminated list of names
 * \param defaultValue the value to return if the specified key
 *     cannot be resolved, or the value of the entry indicated by
 *     the specified key cannot be converted to the desired type
 * \return the value of the entry indicated by the specified key
 *     or the specified default value
 */
ssize_t DataGetType(
	Data *d,
	const char *key,
	const char *nameList[],
	const ssize_t defaultValue) {
  if (!nameList) {
    Log("invalid `nameList` string array");
  } else {
    const char *found = DataGetString(d, key, NULL);
    if (found && *found != '\0') {
      register size_t j = 0;
      for (j = 0; *nameList[j] != '\n'; ++j) {
	if (StringCmpCI(nameList[j], found) == 0)
	  return (j);
      }
      Log("Key `%s` has unknown type `%s`", key, found);
    }
  }
  return (defaultValue);
}

/*!
 * Searches for an entry and returns its value.
 * \addtogroup data
 * \param d the data element to search
 * \param key the key identifying the entry whose value to return
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
  if (*StringBlank(found) != '\0') {
    double value = 0.0;
    if (sscanf(found, " %lg ", &value) == 1) {
      result = /* NaN */ value == value && value != 0.0;
    } else {
      /* Check for `Yes` */
      if (StringCmpCI(found, "Y") == 0 ||
	  StringCmpCI(found, "Yes") == 0)
	result = true;

      /* Check for `No` */
      if (StringCmpCI(found, "N") == 0 ||
	  StringCmpCI(found, "No") == 0)
	result = false;
    }
  }
  return (result);
}

/*!
 * Returns the key of the entry at the specified index.
 * \addtogroup data
 * \param d the data element
 * \param index the zero-based index of the entry whose key to return
 * \return the key of the entry at the specified index or NULL
 * \sa DataValueAt(const Data*, const size_t)
 */
const char *DataKeyAt(
	Data *d,
	const size_t index) {
  register const char *keyAt = NULL;
  if (d && index < d->entryListN) {
    keyAt = StringBlank(d->entryList[index].key);
  }
  return (keyAt);
}

/* Some local prototypes */
static Data *DataReadString(FILE *stream);
static Data *DataReadStringBlock(FILE *stream);
static Data *DataReadStruct(FILE *stream);
static bool DataReadStructKey(
	char *key, const size_t keylen,
	FILE *stream);
static Data *DataReadStructValue(FILE *stream);

/*!
 * Reads a string from a stream.
 * \addtogroup value
 * \param stream the stream from which to read a string
 * \return a new string, or NULL
 */
Data *DataReadString(FILE *stream) {
  register Data *d = NULL;
  if (!stream) {
    Log("invalid `stream` FILE");
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
      Log("/DATA/ Unexpected EOF while reading string: %s", messg);
  }
  return (d);
}

/*!
 * Reads a string from a stream.
 * \addtogroup data
 * \param stream the stream from which to read
 * \return a new string, or NULL
 */
Data *DataReadStringBlock(FILE *stream) {
  register Data *d = NULL;
  if (!stream) {
    Log("invalid 'stream' FILE");
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
	    Log("/DATA/ Missing EOF or EOL while parsing string block");
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
	  register size_t fewestSpaces = -1;
	  for (register size_t lineN = 0; lineN < linesN; ++lineN) {
	    register size_t currentSpaces = 0;
	    for (ptr = lines[lineN]; *ptr != '\0' && *ptr != '\n'; ++ptr) {
	      if (isspace(*ptr) && *ptr != '\r') {
		++currentSpaces;
	      } else if (*ptr != '\r')
		break;
	    }
	    if (fewestSpaces > currentSpaces || fewestSpaces == -1)
	      fewestSpaces = currentSpaces;
	  }

	  /* Reconstruct the string block */
	  if (fewestSpaces && fewestSpaces != -1) {
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

/*!
 * Reads a struct key from a stream.
 * \addtogroup data
 * \param stream the stream from which to read
 * \return a new struct key or NULL
 */
bool DataReadStructKey(
	char *key, const size_t keylen,
	FILE *stream) {
  register bool result = false;
  if (!key && keylen) {
    Log("invalid `key` buffer");
  } else if (!stream) {
    Log("invalid `stream` FILE");
  } else {
    register size_t keypos = 0;
    for (result = true; result; ) {
      const int ch = fgetc(stream);
      if (ch == EOF) {
	Log("/DATA/ Unexpected EOF while parsing structure key: %s", key);
	result = false;
      } else if (ch == ':') {
	if (keypos == 0) {
	  Log("/DATA/ Unexpected colon while parsing structure key");
	  result = false;
	} else
	  break;
      } else if (isalnum(ch) || ch == '_' || ch == '$') {
	BPrintf(key, keylen, keypos, "%c", ch);
      } else {
	Log("/DATA/ Invalid '%c' while parsing structure key: %s", ch, key);
	result = false;
      }
    }
  }
  return (result);
}

/*!
 * Reads a struct from a stream.
 * \addtogroup data
 * \param stream the stream from which to read
 * \return a new struct, or NULL
 */
Data *DataReadStruct(FILE *stream) {
  register Data *d = NULL;
  if (!stream) {
    Log("invalid 'stream' FILE");
  } else {
    while (true) {
      int ch = fgetc(stream);
      if (ch == EOF)
	break;
      if (ch == '~')
	break;
      if (isalnum(ch) || ch == '_' || ch == '$') {
	if (ungetc(ch, stream) != ch) {
	  Log("/DATA/ ungetc() failed: errno=%d", errno);
	  DataFree(d), d = NULL;
	  break;
	}
	char key[MAXLEN_INPUT] = {'\0'};
	if (!DataReadStructKey(key, sizeof(key), stream)) {
	  Log("/DATA/ Couldn't parse structure key");
	  DataFree(d), d = NULL;
	  break;
	}
	Data *value = DataReadStructValue(stream);
	if (!value) {
	  Log("/DATA/ Couldn't parse structure value");
	  DataFree(d), d = NULL;
	  break;
	}
	if (!d)
	  d = DataAlloc();

	if (DataPut(d, key, value) != value) {
	  Log("/DATA/ Couldn't add structure value: %s", key);
	  DataFree(d), d = NULL;
	  break;
	}
      } else if (!isspace(ch))
	break;
    }
  }
  return (d);
}

/*!
 * Reads a data element from a stream.
 * \addtogroup data
 * \param stream the stream from which to read
 * \return a new data element, or NULL
 */
Data *DataReadStructValue(FILE *stream) {
  register Data *d = NULL;
  if (!stream) {
    Log("invalid `stream` FILE");
  } else {
    register int ch = fgetc(stream);
    if (ch == EOF) {
      Log("/DATA/ Unexpected EOF while reading structure value");
    } else if (ch == '-') {
      ch = fgetc(stream);
      while (ch != EOF && ch != '\n' && isspace(ch))
	ch = fgetc(stream);

      if (ch == EOF) {
	Log("/DATA/ Unexpected EOF while reading structure value");
      } else if (ch != '\n') {
	Log("/DATA/ Missing EOL while reading structure value");
      } else {
	d = DataReadStringBlock(stream);
      }
    } else {
      while (ch != EOF && ch != '\n' && isspace(ch))
	ch = fgetc(stream);

      if (ch == EOF) {
	Log("/DATA/ Unexpected EOF while reading structure value");
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
	  Log("/DATA/ ungetc() failed: errno=%d", errno);
	} else
	  d = DataReadString(stream);
      }
    }
    if (!d)
      Log("/DATA/ Error while reading structure value");
  }
  return (d);
}

/*!
 * Loads a data element.
 * \addtogroup data
 * \param filename the filename of the file to read
 * \return a data element representing the contents of
 *     the file indicated by the specified filename, or NULL
 * \sa DataLoadStream(FILE*)
 */
Data *DataLoadFile(const char *filename) {
  register Data *loaded = NULL;
  if (*StringBlank(filename) == '\0') {
    Log("invalid `filename` string");
  } else {
    FILE *stream = fopen(filename, "rt");
    if (!stream) {
      Log("Could not open file %s for reading", filename);
    } else {
      loaded = DataLoadStream(stream);
      fclose(stream);
    }
  }
  return (loaded);
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
    Log("invalid `stream` FILE");
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
    Log("invalid `d` Data");
    value = NULL;
  } else if (*StringBlank(key) == '\0') {
    Log("invalid `key` string");
    value = NULL;
  } else if (!value) {
    Log("invalid `value` Data");
  } else {
    /* Make a copy of the key */
    char realKey[MAXLEN_INPUT] = {'\0'};
    strlcpy(realKey, key, sizeof(realKey));

    /* Find the next highest index */
    if (strcmp(realKey, "%") == 0) {
      register size_t highest = 0;
      if (d->entryListN) {
	register size_t j = 0;
	for (j = 0; j < d->entryListN; ++j) {
	  size_t N = 0;
	  if (sscanf(DataKeyAt(d, j), " %zu ", &N) == 1)
	    highest = highest > N ? highest : N;
	}
      }
      snprintf(realKey, sizeof(realKey), "%zu", highest + 1);
    }

    /* Search for the entry */
    register size_t j = 0;
    for (j = 0; j < d->entryListN; ++j) {
      if (StringCmpCI(d->entryList[j].key, realKey) == 0)
	break;
    }

    /* Create a new entry if the entry is not found */
    if (j == d->entryListN) {
      MemoryRecreate(d->entryList, DataEntry, d->entryListN + 1);
      d->entryList[d->entryListN].key   = NULL;
      d->entryList[d->entryListN].value = NULL;
      d->entryListN++;
    }
    MemoryFree(d->entryList[j].key);
    d->entryList[j].key = strdup(realKey);
    DataFree(d->entryList[j].value);
    d->entryList[j].value = value;
  }
  return (value);
}

/*!
 * Inserts or updates an entry into a data element.
 * \addtogroup data
 * \param d the data element into which to insert an entry
 * \param key the key that identifies the entry to insert or update
 * \param nameList the newline-terminated list of names
 * \param value the value of the entry identified by the specified key
 * \return the new of updated data element or NULL
 */
Data *DataPutBits(
	Data *d,
	const char *key,
	const char *nameList[],
	const Bitvector value) {
  register Data *result = NULL;
  if (!d) {
    Log("invalid `d` Data");
  } else if (!nameList) {
    Log("invalid `nameList` string array");
  } else {
    if (*StringBlank(key) == '\0') {
      result = d;
    } else {
      result = DataAlloc();
      if (DataPut(d, key, result) != result) {
	DataFree(result);
	result = NULL;
      }
    }
    if (result) {
      register size_t j = 0;
      for (j = 0; *nameList[j] != '\n'; ++j) {
	if (BitCheckN(value, j) != 0)
	  DataPutYesNo(result, nameList[j], true);
      }
      DataSort(result);
    }
  }
  return (result);
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
    Log("invalid `d` Data");
  } else {
    va_list args;
    va_start(args, format);
    char messg[MAXLEN_STRING] = {'\0'};
    const ssize_t N = vsnprintf(messg, sizeof(messg), format, args);
    if (N >= 0 && N < sizeof(messg) - 1) {
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
    Log("invalid `d` Data");
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
    Log("invalid `d` Data");
  } else {
    if (*StringBlank(key) == '\0') {
      result = d;
    } else {
      result = DataAlloc();
      if (DataPut(d, key, result) != result) {
	DataFree(result);
	result = NULL;
      }
    }
    if (result) {
      DataClear(result);
      MemoryFree(result->value);
      result->value = strdup(StringBlank(value));
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
    Log("invalid `d` Data");
  } else {
    struct tm time;
    if (localtime_r(&value, &time) != &time) {
      Log("localtime_r() failed: errno=%d", errno);
    } else {
      result = DataPutFormatted(d, key,
	"%-4.4d-%-2.2d-%-2.2d %-2.2d:%-2.2d:%-2.2d %d",
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
 * \param nameList the newline-terminated list of names
 * \param value the value of the entry identified by the specified key
 * \return the new of updated data element or NULL
 */
Data *DataPutType(
	Data *d,
	const char *key,
	const char *nameList[],
	const ssize_t value) {
  register Data *result = NULL;
  if (!d) {
    Log("invalid `d` Data");
  } else if (!nameList) {
    Log("invalid `nameList` string array");
  } else {
    register size_t j = 0;
    for (j = 0; !result && *nameList[j] != '\n'; j++) {
      if (j == value)
	result = DataPutString(d, key, nameList[j]);
    }
    if (!result && *nameList[j] == '\n') {
      result = DataPutNumber(d, key, j);
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
Data *DataPutYesNo(
	Data *d,
	const char *key,
	const bool value) {
  register Data *result = NULL;
  if (!d) {
    Log("invalid `d` Data");
  } else {
    result = DataPutString(d, key, value ? "Yes" : "No");
  }
  return (result);
}

/* Some local prototypes */
static bool DataWriteIndent(FILE *stream, const size_t indent);
static bool DataWriteString(FILE *stream, const size_t indent, Data *d);
static bool DataWriteStruct(FILE *stream, const size_t indent, Data *d);
static bool DataWriteStructKey(FILE *stream, const char *key);
static bool DataWriteStructValue(FILE *stream, const size_t indent, Data *d);

/*!
 * Writes indentation to a stream.
 * \addtogroup data
 * \param stream the stream to which to write
 * \param indent the indentation level
 * \return true if the number of spaces indicated by the
 *     specified indentation level were successfully written
 *     to the specified stream
 */
static bool DataWriteIndent(
	FILE *stream,
	const size_t indent) {
  bool result = false;
  if (!stream) {
    Log("invalid `stream` FILE");
  } else {
    const int spaces = (int) indent * 2;
    if (fprintf(stream, "%*s", spaces, "") == spaces)
      result = true;
  }
  return (result);
}

/*!
 * Writes a structure key to a stream.
 * \addtogroup data
 * \param stream the stream to which to write
 * \param key the key to write to the specified stream
 * \return true if the specified key was successfully written
 */
static bool DataWriteStructKey(
	FILE *stream,
	const char *key) {
  bool result = false;
  if (!stream) {
    Log("invalid `stream` FILE");
  } else if (*StringBlank(key) == '\0') {
    Log("invalid `key` string");
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

/*!
 * Writes a scalar to a stream.
 * \addtogroup data
 * \param stream the stream to which to write
 * \param indent the indentation level
 * \param d the scalar to write to the stream
 * \return true if the scalar was successfully written
 */
static bool DataWriteString(
	FILE *stream,
	const size_t indent,
	Data *d) {
  register bool result = false;
  if (!stream) {
    Log("invalid `stream` FILE");
  } else if (!d) {
    Log("invalid `d` Data");
  } else {
    register const char *ptr = StringBlank(d->value);
    if (strchr(ptr, '\n') != NULL) {
      result = DataWriteIndent(stream, indent);
    } else {
      result = true;
      while (*ptr != '\0' && isspace(*ptr))
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

/*!
 * Writes a struct to a stream.
 * \addtogroup data
 * \param stream the stream to which to write
 * \param indent the indentation level
 * \param d the struct to write to the stream
 * \return true if the struct was successfully written
 */
static bool DataWriteStruct(
	FILE *stream,
	const size_t indent,
	Data *d) {
  register bool result = false;
  if (!stream) {
    Log("invalid `stream` FILE");
  } else if (!d) {
    Log("invalid `d` Data");
  } else {
    result = true;
    if (d->entryListN) {
      register size_t j = 0;
      for (j = 0; result && j < d->entryListN; ++j) {
	if (result)
	  result = DataWriteIndent(stream, indent);
	if (result) {
	  const char *keyAt = DataKeyAt(d, j);
	  result = DataWriteStructKey(stream, keyAt);
	}
	if (result) {
	  Data *valueAt = DataValueAt(d, j);
	  result = DataWriteStructValue(stream, indent + 1, valueAt);
	}
      }
    }
    if (result)
      result = DataWriteIndent(stream, indent);
    if (result && fprintf(stream, "~\n") != 2)
      result = false;
  }
  return (result);
}

/*!
 * Writes a struct value to a stream.
 * \addtogroup data
 * \param stream the stream to which to write
 * \param indent the indentation level
 * \param value the value to write to the stream
 * \return true if the specified value was successfully written
 */
static bool DataWriteStructValue(
	FILE *stream,
	const size_t indent,
	Data *d) {
  register bool result = false;
  if (!stream) {
    Log("invalid `stream` FILE");
  } else if (!d) {
    Log("invalid `d` Data");
  } else {
    if (d->entryListN) {
      if (fputc('\n', stream) != EOF)
	result = true;
      if (result)
	result = DataWriteStruct(stream, indent, d);
    } else {
      if (strchr(StringBlank(d->value), '\n') != NULL) {
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
    Log("invalid `d` Data");
  } else if (*StringBlank(fname) == '\0') {
    Log("invalid `fname` string");
  } else {
    char tempfname[PATH_MAX] = {'\0'};
    if (snprintf(tempfname, sizeof(tempfname), "%s.tmp", fname) > 0) {
      FILE *stream = fopen(tempfname, "wt");
      if (!stream) {
	Log("Couldn't open file `%s` for writing", tempfname);
      } else {
	result = DataSaveStream(d, stream);
	fclose(stream);
	if (result && rename(tempfname, fname) != 0)
	  Log("rename() failed: errno=%d", errno);
	if (unlink(tempfname) != 0 && errno != ENOENT)
	  Log("unlink() failed: errno=%d", errno);
      }
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
    Log("invalid `stream` FILE");
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
  register size_t result = 0;
  if (d) {
    result = d->entryListN;
  }
  return (result);
}

/*!
 * Compares two data element entries.
 * \addtogroup data
 * \param x the first data element entry to compare
 * \param y the second data element entry to compare
 * \return < 0 if the first entry precedes the second entry;
 *         > 0 if the first entry follows the second entry;
 *           0 if the two data element entries are equal
 */
static int DataSortProc(const void *x, const void *y) {
  const DataEntry *xEntry = (const DataEntry*) x;
  const DataEntry *yEntry = (const DataEntry*) y;
  return StringCmpCI(xEntry->key, yEntry->key);
}

/*!
 * Sorts a data element.
 * \addtogroup data
 * \param d the data element whose keys to sort
 */
void DataSort(Data *d) {
  if (!d) {
    Log("invalid `d` Data");
  } else {
    qsort(d->entryList,
	  d->entryListN,
	  sizeof(DataEntry),
	  DataSortProc);
  }
}

/*!
 * Returns the value of the entry value at the specified index.
 * \addtogroup data
 * \param d the data element
 * \param index the zero-based index of the entry whose value to return
 * \return the value of the entry at the specified index or NULL
 * \sa DataKeyAt(const Data*, const size_t)
 */
Data *DataValueAt(
	Data *d,
	const size_t index) {
  register Data *valueAt = NULL;
  if (d && index < d->entryListN) {
      valueAt = d->entryList[index].value;
  }
  return (valueAt);
}
