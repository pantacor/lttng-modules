/* SPDX-License-Identifier: (GPL-2.0-only or LGPL-2.1-only)
 *
 * lttng-context-vgid.c
 *
 * LTTng namespaced real group ID context.
 *
 * Copyright (C) 2009-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *               2019 Michael Jeanson <mjeanson@efficios.com>
 *
 */

#include <linux/module.h>
#include <linux/sched.h>
#include <lttng-events.h>
#include <lttng-tracer.h>
#include <wrapper/ringbuffer/frontend_types.h>
#include <wrapper/vmalloc.h>
#include <wrapper/user_namespace.h>

static
size_t vgid_get_size(size_t offset)
{
	size_t size = 0;

	size += lib_ring_buffer_align(offset, lttng_alignof(gid_t));
	size += sizeof(gid_t);
	return size;
}

static
void vgid_record(struct lttng_ctx_field *field,
		 struct lib_ring_buffer_ctx *ctx,
		 struct lttng_channel *chan)
{
	gid_t vgid;

	vgid = lttng_current_vgid();
	lib_ring_buffer_align_ctx(ctx, lttng_alignof(vgid));
	chan->ops->event_write(ctx, &vgid, sizeof(vgid));
}

static
void vgid_get_value(struct lttng_ctx_field *field,
		struct lttng_probe_ctx *lttng_probe_ctx,
		union lttng_ctx_value *value)
{
	value->s64 = lttng_current_vgid();
}

int lttng_add_vgid_to_ctx(struct lttng_ctx **ctx)
{
	struct lttng_ctx_field *field;

	field = lttng_append_context(ctx);
	if (!field)
		return -ENOMEM;
	if (lttng_find_context(*ctx, "vgid")) {
		lttng_remove_context_field(ctx, field);
		return -EEXIST;
	}
	field->event_field.name = "vgid";
	field->event_field.type.atype = atype_integer;
	field->event_field.type.u.integer.size = sizeof(gid_t) * CHAR_BIT;
	field->event_field.type.u.integer.alignment = lttng_alignof(gid_t) * CHAR_BIT;
	field->event_field.type.u.integer.signedness = lttng_is_signed_type(gid_t);
	field->event_field.type.u.integer.reverse_byte_order = 0;
	field->event_field.type.u.integer.base = 10;
	field->event_field.type.u.integer.encoding = lttng_encode_none;
	field->get_size = vgid_get_size;
	field->record = vgid_record;
	field->get_value = vgid_get_value;
	lttng_context_update(*ctx);
	wrapper_vmalloc_sync_mappings();
	return 0;
}
EXPORT_SYMBOL_GPL(lttng_add_vgid_to_ctx);
