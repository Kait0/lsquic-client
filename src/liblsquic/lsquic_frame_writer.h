/* Copyright (c) 2017 - 2018 LiteSpeed Technologies Inc.  See LICENSE. */
/*
 * lsquic_frame_writer.h -- write frames to HEADERS stream.
 */

#ifndef LSQUIC_FRAME_WRITER_H
#define LSQUIC_FRAME_WRITER_H 1

#include <stddef.h>
#include <stdint.h>

struct iovec;
struct lshpack_enc;
struct lsquic_mm;
struct lsquic_frame_writer;
struct lsquic_stream;
struct lsquic_http_headers;
struct lsquic_http2_setting;

typedef ssize_t (*fw_write_f)(struct lsquic_stream *, const void *, size_t);

struct lsquic_frame_writer *
lsquic_frame_writer_new (struct lsquic_mm *, struct lsquic_stream *,
                         unsigned max_frame_sz, struct lshpack_enc *,
                         fw_write_f, int is_server);

void
lsquic_frame_writer_destroy (struct lsquic_frame_writer *);

int
lsquic_frame_writer_have_leftovers (const struct lsquic_frame_writer *);

int
lsquic_frame_writer_flush (struct lsquic_frame_writer *);

int
lsquic_frame_writer_write_headers (struct lsquic_frame_writer *,
                                   uint32_t stream_id,
                                   const struct lsquic_http_headers *,
                                   int eos, unsigned weight);

int
lsquic_frame_writer_write_settings (struct lsquic_frame_writer *,
    const struct lsquic_http2_setting *, unsigned n_settings);

int
lsquic_frame_writer_write_priority (struct lsquic_frame_writer *,
            uint32_t stream_id, int exclusive, uint32_t stream_dep_id,
            unsigned priority);

int
lsquic_frame_writer_write_promise (struct lsquic_frame_writer *,
                       uint32_t stream_id, uint32_t promised_stream_id,
                       const struct iovec *path, const struct iovec *host,
                       const struct lsquic_http_headers *headers);

void
lsquic_frame_writer_max_header_list_size (struct lsquic_frame_writer *,
                                          uint32_t max_size);

size_t
lsquic_frame_writer_mem_used (const struct lsquic_frame_writer *);

#endif
