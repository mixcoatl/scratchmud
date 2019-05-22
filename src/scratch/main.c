/*!
 * \file main.c
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup main
 */
#include <main.h>
#include <scratch/log.h>
#include <scratch/scratch.h>

/*!
 * Program entry point.
 * \addtogroup main
 * \param argc the number of command line arguments
 * \param argv an array containing the command line arguments
 * \return zero for normal program termination, non-zero otherwise
 */
int main(int argc, const char *argv[]) {
  Log("Package " PACKAGE_STRING "");
  return (EXIT_SUCCESS);
}
