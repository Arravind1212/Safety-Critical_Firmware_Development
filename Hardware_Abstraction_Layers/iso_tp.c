// ISO_tp.c
#include "isotp.h"
#include "can_driver.h"
#include <string.h>
#include <stdlib.h>

static uint8_t tx_buffer[ISOTP_MAX_MESSAGE_SIZE];
static uint16_t tx_len = 0;
static uint16_t tx_index = 0;
static uint8_t tx_seq_num = 1;
static bool tx_pending = false;
static uint16_t tx_can_id = 0;

void IsoTp_Init(IsoTpContext* ctx) {
    memset(ctx, 0, sizeof(IsoTpContext));
}

bool IsoTp_ReceiveFrame(IsoTpContext* ctx, const uint8_t* data, uint8_t len) {
    if (len < 1) return false;

    IsoTpFrameType type = (data[0] >> 4) & 0xF;
    switch (type) {
        case ISOTP_SINGLE_FRAME: {
            uint8_t msg_len = data[0] & 0x0F;
            memcpy(ctx->data, &data[1], msg_len);
            ctx->length = msg_len;
            return true;
        }
        case ISOTP_FIRST_FRAME: {
            ctx->length = ((data[0] & 0x0F) << 8) | data[1];
            ctx->received = len - 2;
            memcpy(ctx->data, &data[2], ctx->received);
            ctx->sequence_number = 1;
            ctx->receiving = true;

            // Send Flow Control Frame
            CAN_Frame fc;
            fc.id = tx_can_id;
            fc.dlc = 8;
            fc.data[0] = (ISOTP_FLOW_CONTROL << 4) | ISOTP_FC_CTS;
            fc.data[1] = ISOTP_DEFAULT_BLOCK_SIZE;
            fc.data[2] = ISOTP_DEFAULT_STMIN;
            memset(&fc.data[3], 0, 5);
            CAN_Send(&fc);
            return false;
        }
        case ISOTP_CONSEC_FRAME: {
            if (!ctx->receiving) return false;
            uint8_t seq = data[0] & 0x0F;
            if (seq != ctx->sequence_number) return false;

            memcpy(&ctx->data[ctx->received], &data[1], len - 1);
            ctx->received += len - 1;
            ctx->sequence_number++;

            if (ctx->received >= ctx->length) {
                ctx->receiving = false;
                return true;
            }
            return false;
        }
        default:
            return false;
    }
}

bool IsoTp_SendMessage(uint16_t can_id, const uint8_t* data, uint16_t len) {
    if (len <= 7) {
        CAN_Frame frame;
        frame.id = can_id;
        frame.dlc = len + 1;
        frame.data[0] = (ISOTP_SINGLE_FRAME << 4) | (len & 0x0F);
        memcpy(&frame.data[1], data, len);
        return CAN_Send(&frame);
    }

    memcpy(tx_buffer, data, len);
    tx_len = len;
    tx_index = 6;
    tx_seq_num = 1;
    tx_can_id = can_id;
    tx_pending = true;

    CAN_Frame frame;
    frame.id = can_id;
    frame.dlc = 8;
    frame.data[0] = (ISOTP_FIRST_FRAME << 4) | ((len >> 8) & 0x0F);
    frame.data[1] = len & 0xFF;
    memcpy(&frame.data[2], data, 6);

    return CAN_Send(&frame);
}

bool IsoTp_ProcessTxQueue(void) {
    if (!tx_pending) return false;
    if (tx_index >= tx_len) {
        tx_pending = false;
        return true;
    }

    CAN_Frame frame;
    frame.id = tx_can_id;
    frame.dlc = 8;
    frame.data[0] = (ISOTP_CONSEC_FRAME << 4) | (tx_seq_num & 0x0F);
    uint8_t bytes_left = tx_len - tx_index;
    uint8_t segment_size = (bytes_left > 7) ? 7 : bytes_left;
    memcpy(&frame.data[1], &tx_buffer[tx_index], segment_size);
    memset(&frame.data[1 + segment_size], 0xAA, 7 - segment_size); // padding

    tx_index += segment_size;
    tx_seq_num++;

    return CAN_Send(&frame);
}
