/* Copyright (c) 2017 - 2018 LiteSpeed Technologies Inc.  See LICENSE. */
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

#include "lsquic.h"

#include "lsquic_int_types.h"
#include "lsquic_packet_common.h"
#include "lsquic_packet_out.h"
#include "lsquic_parse.h"
#include "lsquic_conn_flow.h"
#include "lsquic_sfcw.h"
#include "lsquic_stream.h"
#include "lsquic_types.h"
#include "lsquic_malo.h"
#include "lsquic_mm.h"
#include "lsquic_engine_public.h"
#include "lsquic_logger.h"


static const struct parse_funcs *const pf = select_pf_by_ver(LSQVER_035);

static struct {
    char        buf[0x100];
    size_t      bufsz;
    uint64_t    off;
} stream_contents;


void
setup_stream_contents (uint64_t off, const char *str)
{
    stream_contents.bufsz = strlen(str);
    stream_contents.off   = off;
    memcpy(stream_contents.buf, str, stream_contents.bufsz);
}


int
lsquic_stream_tosend_fin (const lsquic_stream_t *stream)
{
    return 0;
}


uint64_t
lsquic_stream_tosend_offset (const lsquic_stream_t *stream)
{
    return stream_contents.off;
}


size_t
lsquic_stream_tosend_read (lsquic_stream_t *stream, void *buf, size_t len,
                           int *reached_fin)
{
    if (stream_contents.bufsz < len)
        len = stream_contents.bufsz;
    memcpy(buf, stream_contents.buf, len);
    *reached_fin = lsquic_stream_tosend_fin(stream);
    return len;
}


size_t
lsquic_stream_tosend_sz (const lsquic_stream_t *stream)
{
    return stream_contents.bufsz;
}


void
lsquic_stream_acked (lsquic_stream_t *stream)
{
    --stream->n_unacked;
}


static void
elide_single_stream_frame (void)
{
    struct packet_out_srec_iter posi;
    struct lsquic_engine_public enpub;
    lsquic_stream_t streams[1];
    lsquic_packet_out_t *packet_out;
    int len, off = 0;

    memset(streams, 0, sizeof(streams));
    memset(&enpub, 0, sizeof(enpub));
    lsquic_mm_init(&enpub.enp_mm);
    packet_out = lsquic_mm_get_packet_out(&enpub.enp_mm, NULL, QUIC_MAX_PAYLOAD_SZ);

    setup_stream_contents(123, "Dude, where is my car?");
    len = pf->pf_gen_stream_frame(packet_out->po_data + packet_out->po_data_sz,
            lsquic_packet_out_avail(packet_out),
            streams[0].id, lsquic_stream_tosend_offset(&streams[0]),
            lsquic_stream_tosend_fin(&streams[0]),
            lsquic_stream_tosend_sz(&streams[0]),
            (gsf_read_f) lsquic_stream_tosend_read,
            &streams[0]);
    packet_out->po_data_sz += len;
    packet_out->po_frame_types |= (1 << QUIC_FRAME_STREAM);
    lsquic_packet_out_add_stream(packet_out, &enpub.enp_mm, &streams[0],
                                                QUIC_FRAME_STREAM, off, len);
    assert(1 == streams[0].n_unacked);
    assert(posi_first(&posi, packet_out));

    streams[0].stream_flags |= STREAM_RST_SENT;

    lsquic_packet_out_elide_reset_stream_frames(packet_out, 0);
    assert(0 == streams[0].n_unacked);
    assert(0 == packet_out->po_frame_types);
    assert(!posi_first(&posi, packet_out));

    lsquic_packet_out_destroy(packet_out, &enpub);
    lsquic_mm_cleanup(&enpub.enp_mm);
}


/* This test is more involved.  We will construct the following packet:
 *
 *      | ACK | STREAM A | STREAM B | STREAM C | RST A | STREAM D | STREAM E
 *
 * and elide STREAM A, STREAM C, and STREAM E to get
 *
 *      | ACK | STREAM B | RST A | STREAM D |
 *
 * If `chop_regen' is set, ACK is dropped (this tests what happens when
 * packet is resent).
 *
 * This should test most of the corner cases.
 */
static void
elide_three_stream_frames (int chop_regen)
{
    struct packet_out_srec_iter posi;
    struct lsquic_engine_public enpub;
    lsquic_stream_t streams[5];
    lsquic_packet_out_t *packet_out, *ref_out;
    struct stream_rec *srec;
    unsigned short b_off, d_off;
    int len;

    memset(streams, 0, sizeof(streams));
    memset(&enpub, 0, sizeof(enpub));
    lsquic_mm_init(&enpub.enp_mm);

    /* First, we construct the reference packet.  We will only use it to
     * compare payload and sizes:
     */
    {
        ref_out = lsquic_mm_get_packet_out(&enpub.enp_mm, NULL, QUIC_MAX_PAYLOAD_SZ);
        /* This is fake data for regeneration */
        strcpy((char *) ref_out->po_data, "REGEN");
        ref_out->po_data_sz = ref_out->po_regen_sz = 5;
        /* STREAM B */
        setup_stream_contents(123, "BBBBBBBBBB");
        streams[0].id = 'B';
        len = pf->pf_gen_stream_frame(ref_out->po_data + ref_out->po_data_sz,
                lsquic_packet_out_avail(ref_out),
                streams[0].id, lsquic_stream_tosend_offset(&streams[0]),
                lsquic_stream_tosend_fin(&streams[0]),
                lsquic_stream_tosend_sz(&streams[0]),
                (gsf_read_f) lsquic_stream_tosend_read,
                &streams[0]);
        b_off = ref_out->po_data_sz;
        ref_out->po_data_sz += len;
        len = pf->pf_gen_rst_frame(ref_out->po_data + ref_out->po_data_sz,
                lsquic_packet_out_avail(ref_out), 'A', 133, 0);
        ref_out->po_data_sz += len;
        /* STREAM D */
        setup_stream_contents(123, "DDDDDDDDDD");
        streams[0].id = 'D';
        len = pf->pf_gen_stream_frame(ref_out->po_data + ref_out->po_data_sz,
                lsquic_packet_out_avail(ref_out),
                streams[0].id, lsquic_stream_tosend_offset(&streams[0]),
                lsquic_stream_tosend_fin(&streams[0]),
                lsquic_stream_tosend_sz(&streams[0]),
                (gsf_read_f) lsquic_stream_tosend_read,
                &streams[0]);
        d_off = ref_out->po_data_sz;
        ref_out->po_data_sz += len;
    }

    /* Construct packet from which we will elide streams.  Here, we attach
     * stream objects to the packet.
     */
    {
        packet_out = lsquic_mm_get_packet_out(&enpub.enp_mm, NULL, QUIC_MAX_PAYLOAD_SZ);
        /* This is fake data for regeneration */
        strcpy((char *) packet_out->po_data, "REGEN");
        packet_out->po_data_sz = packet_out->po_regen_sz = 5;
        /* STREAM A */
        setup_stream_contents(123, "AAAAAAAAAA");
        streams[0].id = 'A';
        len = pf->pf_gen_stream_frame(packet_out->po_data + packet_out->po_data_sz,
                lsquic_packet_out_avail(packet_out),
                streams[0].id, lsquic_stream_tosend_offset(&streams[0]),
                lsquic_stream_tosend_fin(&streams[0]),
                lsquic_stream_tosend_sz(&streams[0]),
                (gsf_read_f) lsquic_stream_tosend_read,
                &streams[0]);
        lsquic_packet_out_add_stream(packet_out, &enpub.enp_mm, &streams[0],
                                    QUIC_FRAME_STREAM, packet_out->po_data_sz, len);
        packet_out->po_data_sz += len;
        /* STREAM B */
        setup_stream_contents(123, "BBBBBBBBBB");
        streams[1].id = 'B';
        len = pf->pf_gen_stream_frame(packet_out->po_data + packet_out->po_data_sz,
                lsquic_packet_out_avail(packet_out),
                streams[1].id, lsquic_stream_tosend_offset(&streams[1]),
                lsquic_stream_tosend_fin(&streams[1]),
                lsquic_stream_tosend_sz(&streams[1]),
                (gsf_read_f) lsquic_stream_tosend_read,
                &streams[1]);
        lsquic_packet_out_add_stream(packet_out, &enpub.enp_mm, &streams[1],
                                    QUIC_FRAME_STREAM, packet_out->po_data_sz, len);
        packet_out->po_data_sz += len;
        /* STREAM C */
        setup_stream_contents(123, "CCCCCCCCCC");
        streams[2].id = 'C';
        len = pf->pf_gen_stream_frame(packet_out->po_data + packet_out->po_data_sz,
                lsquic_packet_out_avail(packet_out),
                streams[2].id, lsquic_stream_tosend_offset(&streams[2]),
                lsquic_stream_tosend_fin(&streams[2]),
                lsquic_stream_tosend_sz(&streams[2]),
                (gsf_read_f) lsquic_stream_tosend_read,
                &streams[2]);
        lsquic_packet_out_add_stream(packet_out, &enpub.enp_mm, &streams[2],
                                    QUIC_FRAME_STREAM, packet_out->po_data_sz, len);
        packet_out->po_data_sz += len;
        /* Reset A */
        len = pf->pf_gen_rst_frame(packet_out->po_data + packet_out->po_data_sz,
                lsquic_packet_out_avail(packet_out), 'A', 133, 0);
        lsquic_packet_out_add_stream(packet_out, &enpub.enp_mm, &streams[0],
                                     QUIC_FRAME_RST_STREAM, 0, 0);
        packet_out->po_data_sz += len;
        /* STREAM D */
        setup_stream_contents(123, "DDDDDDDDDD");
        streams[3].id = 'D';
        len = pf->pf_gen_stream_frame(packet_out->po_data + packet_out->po_data_sz,
                lsquic_packet_out_avail(packet_out),
                streams[3].id, lsquic_stream_tosend_offset(&streams[3]),
                lsquic_stream_tosend_fin(&streams[3]),
                lsquic_stream_tosend_sz(&streams[3]),
                (gsf_read_f) lsquic_stream_tosend_read,
                &streams[3]);
        lsquic_packet_out_add_stream(packet_out, &enpub.enp_mm, &streams[3],
                                QUIC_FRAME_STREAM, packet_out->po_data_sz, len);
        packet_out->po_data_sz += len;
        /* STREAM E */
        setup_stream_contents(123, "EEEEEEEEEE");
        streams[4].id = 'E';
        len = pf->pf_gen_stream_frame(packet_out->po_data + packet_out->po_data_sz,
                lsquic_packet_out_avail(packet_out),
                streams[4].id, lsquic_stream_tosend_offset(&streams[4]),
                lsquic_stream_tosend_fin(&streams[4]),
                lsquic_stream_tosend_sz(&streams[4]),
                (gsf_read_f) lsquic_stream_tosend_read,
                &streams[4]);
        lsquic_packet_out_add_stream(packet_out, &enpub.enp_mm, &streams[4],
                                QUIC_FRAME_STREAM, packet_out->po_data_sz, len);
        packet_out->po_data_sz += len;
        packet_out->po_frame_types = (1 << QUIC_FRAME_STREAM) | (1 << QUIC_FRAME_RST_STREAM);
    }

    /* Reset streams A, C, and E: */
    streams[0].stream_flags |= STREAM_RST_SENT;
    streams[2].stream_flags |= STREAM_RST_SENT;
    streams[4].stream_flags |= STREAM_RST_SENT;

    if (chop_regen)
        lsquic_packet_out_chop_regen(packet_out);
    lsquic_packet_out_elide_reset_stream_frames(packet_out, 0);

    assert(ref_out->po_data_sz == packet_out->po_data_sz + (chop_regen ? 5 : 0));
    assert(ref_out->po_regen_sz == packet_out->po_regen_sz + (chop_regen ? 5 : 0));
    if (chop_regen)
        assert(0 == memcmp(ref_out->po_data + 5, packet_out->po_data, packet_out->po_data_sz));
    else
        assert(0 == memcmp(ref_out->po_data, packet_out->po_data, packet_out->po_data_sz));

    assert(1 == streams[0].n_unacked);  /* Still has RST outstanding */
    assert(1 == streams[1].n_unacked);
    assert(0 == streams[2].n_unacked);
    assert(1 == streams[3].n_unacked);
    assert(0 == streams[4].n_unacked);

    assert(packet_out->po_frame_types == ((1 << QUIC_FRAME_STREAM) | (1 << QUIC_FRAME_RST_STREAM)));

    srec = posi_first(&posi, packet_out);
    assert(srec->sr_stream == &streams[0]);
    assert(srec->sr_frame_types == (1 << QUIC_FRAME_RST_STREAM));

    srec = posi_next(&posi);
    assert(srec->sr_stream == &streams[1]);
    assert(srec->sr_frame_types == (1 << QUIC_FRAME_STREAM));
    assert(srec->sr_off == b_off - (chop_regen ? 5 : 0));

    srec = posi_next(&posi);
    assert(srec->sr_stream == &streams[3]);
    assert(srec->sr_frame_types == (1 << QUIC_FRAME_STREAM));
    assert(srec->sr_off == d_off - (chop_regen ? 5 : 0));

    srec = posi_next(&posi);
    assert(!srec);

    lsquic_packet_out_destroy(packet_out, &enpub);
    lsquic_packet_out_destroy(ref_out, &enpub);
    lsquic_mm_cleanup(&enpub.enp_mm);
}


int
main (void)
{
    /* TODO-ENDIAN: test with every PF */
    elide_single_stream_frame();
    elide_three_stream_frames(0);
    elide_three_stream_frames(1);

    return 0;
}
