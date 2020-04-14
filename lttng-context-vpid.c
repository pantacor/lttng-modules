/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng-context-vpid.c
 *
 * LTTng vPID context.
 *
 * Copyright (C) 2009-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <lttng/lttng-events.h>
#include <ringbuffer/frontend_types.h>
#include <lttng/lttng-tracer.h>

static
size_t vpid_get_size(size_t offset)
{
	size_t size = 0;

	size += lib_ring_buffer_align(offset, lttng_alignof(pid_t));
	size += sizeof(pid_t);
	return size;
}

static
void vpid_record(struct lttng_ctx_field *field,
		 struct lib_ring_buffer_ctx *ctx,
		 struct lttng_channel *chan)
{
	pid_t vpid;

	/*
	 * nsproxy can be NULL when scheduled out of exit.
	 */
	if (!current->nsproxy)
		vpid = 0;
	else
		vpid = task_tgid_vnr(current);
	lib_ring_buffer_align_ctx(ctx, lttng_alignof(vpid));
	chan->ops->event_write(ctx, &vpid, sizeof(vpid));
}

static
void vpid_get_value(struct lttng_ctx_field *field,
		struct lttng_probe_ctx *lttng_probe_ctx,
		union lttng_ctx_value *value)
{
	pid_t vpid;

	/*
	 * nsproxy can be NULL when scheduled out of exit.
	 */
	if (!current->nsproxy)
		vpid = 0;
	else
		vpid = task_tgid_vnr(current);
	value->s64 = vpid;
}

int lttng_add_vpid_to_ctx(struct lttng_ctx **ctx)
{
	struct lttng_ctx_field *field;

	field = lttng_append_context(ctx);
	if (!field)
		return -ENOMEM;
	if (lttng_find_context(*ctx, "vpid")) {
		lttng_remove_context_field(ctx, field);
		return -EEXIST;
	}
	field->event_field.name = "vpid";
	field->event_field.type.atype = atype_integer;
	field->event_field.type.u.integer.size = sizeof(pid_t) * CHAR_BIT;
	field->event_field.type.u.integer.alignment = lttng_alignof(pid_t) * CHAR_BIT;
	field->event_field.type.u.integer.signedness = lttng_is_signed_type(pid_t);
	field->event_field.type.u.integer.reverse_byte_order = 0;
	field->event_field.type.u.integer.base = 10;
	field->event_field.type.u.integer.encoding = lttng_encode_none;
	field->get_size = vpid_get_size;
	field->record = vpid_record;
	field->get_value = vpid_get_value;
	lttng_context_update(*ctx);
	return 0;
}
EXPORT_SYMBOL_GPL(lttng_add_vpid_to_ctx);
