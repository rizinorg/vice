
#include "trace.h"

#include "lib/bap-frames/libtrace/src/trace.container.hpp"

#define LOG_PREFIX "\033[36m[TRACE]\033[0m "

#define eprintf(...) fprintf(stderr, LOG_PREFIX __VA_ARGS__)

TraceFrame build_frame;

static void close_writer(SerializedTrace::TraceContainerWriter *w) {
	eprintf("closing...\n");
    w->finish();
	delete w;
	eprintf("closed.\n");
}

std::unique_ptr<SerializedTrace::TraceContainerWriter, decltype(&close_writer)> writer(nullptr, close_writer);

extern "C" void trace_open(void) {
	eprintf("opening...\n");
	if (writer) {
		eprintf("already open.\n");
	}

	std::string filename = "trace.frames"; // TODO

	tracer *trac = new tracer();
	trac->set_name("x64sc");
	trac->set_version("");

	target *tgt = new target();
	tgt->set_path("");
	tgt->set_md5sum("");

	fstats *fst = new fstats();
	fst->set_size(0);
	fst->set_atime(0.0);
	fst->set_mtime(0.0);
	fst->set_ctime(0.0);

	meta_frame meta;
	meta.set_allocated_tracer(trac);
	meta.set_allocated_target(tgt);
	meta.set_allocated_fstats(fst);
	meta.set_user("");
	meta.set_host("");
	meta.set_time(0.0);

	try {
		writer.reset(new SerializedTrace::TraceContainerWriter(filename, meta, frame_arch_6502, 0));
	} catch (std::exception e) {
		eprintf("open failed: %s\n", e.what());
	}
	eprintf("opened\n");
}

extern "C" void trace_close(void) {
	writer.reset(nullptr);
}

static void push_reg(operand_value_list *out, const char *name, uint16_t v, size_t bits, bool r, bool w) {
	reg_operand *ro = new reg_operand();
	ro->set_name(name);
	operand_info_specific *s = new operand_info_specific();
	s->set_allocated_reg_operand(ro);

	operand_usage *u = new operand_usage();
	u->set_read(r);
	u->set_written(w);
	u->set_index(false);
	u->set_base(false);

	taint_info *ti = new taint_info();

	operand_info *i = out->add_elem();
	i->set_allocated_operand_info_specific(s);
	i->set_bit_length((int32_t)bits);
	i->set_allocated_operand_usage(u);
	i->set_allocated_taint_info(ti);
	uint8_t va[2] = { (uint8_t)v, (uint8_t)(v >> 8) };
	i->set_value(std::string((const char *)va, bits / 8));
}

static void push_regs(operand_value_list *out, TraceRegs *in, bool r, bool w, TraceRegs *diff) {
	// can't observe which registers were actually used, so we always dump all of them (except unchanged)
	if (!diff || in->a != diff->a) {
		push_reg(out, "a", in->a, 8, r, w);
	}
	if (!diff || in->x != diff->x) {
		push_reg(out, "x", in->x, 8, r, w);
	}
	if (!diff || in->y != diff->y) {
		push_reg(out, "y", in->y, 8, r, w);
	}
	if (!diff || in->sp != diff->sp) {
		push_reg(out, "sp", in->sp, 8, r, w);
	}
	if (!diff || in->pc != diff->pc) {
		push_reg(out, "pc", in->pc, 16, r, w);
	}
	if (!diff || in->sr != diff->sr) {
		push_reg(out, "sr", in->pc, 8, r, w);
	}
}

static void push_mems(operand_value_list *out, TraceMem *mems, size_t count, bool r, bool w) {
	for (size_t idx = 0; idx < count; idx++) {
		TraceMem *m = &mems[idx];

		mem_operand *mo = new mem_operand();
		mo->set_address(m->addr);
		operand_info_specific *s = new operand_info_specific();
		s->set_allocated_mem_operand(mo);

		operand_usage *u = new operand_usage();
		u->set_read(r);
		u->set_written(w);
		u->set_index(false);
		u->set_base(false);

		taint_info *ti = new taint_info();

		operand_info *i = out->add_elem();
		i->set_allocated_operand_info_specific(s);
		i->set_bit_length(8);
		i->set_allocated_operand_usage(u);
		i->set_allocated_taint_info(ti);
		i->set_value(std::string((const char *)&m->val, 1));
	}
}

extern "C" void trace_push(TraceFrame *tf) {
	if (!writer) {
		eprintf("tried to push but not opened.\n");
	}

	operand_value_list *pre = new operand_value_list();
	push_regs(pre, &tf->pre.regs, true, false, nullptr);
	push_mems(pre, tf->pre.mems, tf->pre.mems_count, true, false);

	operand_value_list *post = new operand_value_list();
	push_regs(post, &tf->post.regs, false, true, &tf->pre.regs);
	push_mems(post, tf->post.mems, tf->post.mems_count, false, true);

	std_frame *sf = new std_frame();
	sf->set_address(tf->pre.regs.pc);
	sf->set_thread_id(0);
	sf->set_rawbytes(std::string((const char *)tf->op, tf->op_size));
	sf->set_allocated_operand_pre_list(pre);
	sf->set_allocated_operand_post_list(post);

	frame f;
	f.set_allocated_std_frame(sf);

	writer->add(f);
}
