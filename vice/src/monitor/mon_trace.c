
//#include "vice.h"

//#include <ctype.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

//#ifdef HAVE_STRINGS_H
//#include <strings.h>
//#endif

//#include "lib.h"
//#include "machine.h"
//#include "mon_disassemble.h"
#include "mon_trace.h"
//#include "monitor.h"
//#include "montypes.h"
//#include "screenshot.h"
//#include "types.h"

#ifdef FEATURE_BAP_FRAMES
#include "trace.h"

void mon_bap_frames_open(const char *filename) {
    if (!trace_open(filename)) {
		mon_out("Failed to open. See stderr.\n");
	} else {
		mon_out("opened.\n");
	}
}

void mon_bap_frames_close(void) {
    trace_close();
	mon_out("closed.\n");
}

#else

static void mon_memmap_stub(void) {
    mon_out("Disabled. configure with --enable-bap-frames and recompile.\n");
}

void mon_bap_frames_open(const char *filename) {
	mon_bap_frames_stub();
}

void mon_bap_frames_close(void) {
	mon_bap_frames_stub();
}

#endif

