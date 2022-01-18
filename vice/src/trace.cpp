
#include "trace.h"

#include "lib/bap-frames/libtrace/src/trace.container.hpp"

#define LOG_PREFIX "\033[36m[TRACE]\033[0m "

#define eprintf(...) fprintf(stderr, LOG_PREFIX __VA_ARGS__)

static void close_writer(SerializedTrace::TraceContainerWriter *w) {
	eprintf("closing.\n");
    w->finish();
	delete w;
}

std::unique_ptr<SerializedTrace::TraceContainerWriter, decltype(&close_writer)> writer(nullptr, close_writer);

extern "C" void trace_open(void) {
	eprintf("opening...\n");
	if (writer) {
		eprintf("already open.\n");
	}

	tracer *trac = new tracer();
	trac->set_name("x64sc");
	trac->set_version("");

	meta_frame meta;
	meta.set_allocated_tracer(trac);

	try {
		writer.reset(new SerializedTrace::TraceContainerWriter(
			"trace.frames", // TODO: filename
			meta
			// TODO: 6502 + machine
			));
	} catch (std::exception e) {
		eprintf("open failed: %s\n", e.what());
	}
	eprintf("opened\n");
}

extern "C" void trace_close(void) {
	writer.reset(nullptr);
}
