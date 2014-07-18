//====== Copyright (c) 2012, Valve Corporation, All rights reserved. ========//
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
//
// Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// Redistributions in binary form must reproduce the above copyright notice, 
// this list of conditions and the following disclaimer in the documentation 
// and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
// THE POSSIBILITY OF SUCH DAMAGE.
//===========================================================================//

#include <stdio.h>
#include "demofiledump.h"

bool eventsOnly = false;

void display_usage(const char *programName)
{
	printf("Usage: %s [options] filename.dem\n", programName);
	printf("Options:\n");
	printf("\t-e, --events-only\t\tOnly output game event info\n");
	printf("\n");
}

int main( int argc, char *argv[] )
{
	CDemoFileDump DemoFileDump;

	if( argc <= 1 )
	{
		display_usage(argv[0]);
		exit(1);
	}

	for (int i = 0; i < argc; i++) {
		char *arg = argv[i];
		if ((strcmp(arg, "-e") == 0) || (strcmp(arg, "--events-only") == 0)) {
			if (i + 1 < argc) {
				eventsOnly = true;
			} else {
				display_usage(argv[0]);
				exit(1);
			}
		}
	}

	if( DemoFileDump.Open( argv[ argc-1 ] ) )
	{
		DemoFileDump.DoDump();
	}

	return 1;
}

