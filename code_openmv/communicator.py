#
# communicator library
# this module must be manually copied into the root folder of the OpenMV Cam
#

import pyb, math, time
import ustruct
import commands      # this is a "local library" and must be manually stored on OpenMV cam root folder


# constants for incoming serial communications from main
FSM_SEEKING_START = 0
FSM_COLLECTING_CHARS = 1
FSM_EXPECTING_STOP = 2

MSG_SET_MODE = 0
MSG_SEND_PIC = 1

ECOMM_START_CHAR = 0xAA
ECOMM_END_CHAR = 0xA8
ECOMM_MESSAGE_LENGTH_ENHANCED = 8

ecomm_param_int_1 = None
ecomm_param_int_2 = None
ecomm_param_float = None

uart = pyb.UART(3, 230400, timeout_char=1000)
ecomm_buffer = bytearray(ECOMM_MESSAGE_LENGTH_ENHANCED)

def init_communicator():
    global ecomm_next_buffer_index, ecomm_fsm_state, ecomm_msg_type_expected, ecomm_buffer
    global ecomm_num_good_messages, ecomm_num_error_framing, ecomm_num_error_checksum
    global checksum_calculated
    global FSM_SEEKING_START, FSM_COLLECTING_CHARS, FSM_EXPECTING_STOP
    global MSG_SET_MODE, MSG_SEND_PIC
    global ECOMM_START_CHAR, ECOMM_END_CHAR, ECOMM_MESSAGE_LENGTH

    ecomm_next_buffer_index = 0
    ecomm_fsm_state = FSM_SEEKING_START
    ecomm_msg_type_expected = MSG_SET_MODE
    ecomm_param_int_1 = 0
    ecomm_param_int_2 = 0
    ecomm_param_float = 0
    ecomm_num_good_messages = 0
    ecomm_num_error_framing = 0
    ecomm_num_error_checksum = 0
    checksum_calculated = 0

def write_buf(out_buf):
    uart.write(out_buf)

def check_for_commands(sensor, img):
    global ecomm_next_buffer_index, ecomm_fsm_state, ecomm_msg_type_expected, ecomm_buffer
    global ecomm_num_good_messages, ecomm_num_error_framing, ecomm_num_error_checksum
    global checksum_calculated
    global FSM_SEEKING_START, FSM_COLLECTING_CHARS, FSM_EXPECTING_STOP
    global MSG_SET_MODE, MSG_SEND_PIC
    global ECOMM_START_CHAR, ECOMM_END_CHAR, ECOMM_MESSAGE_LENGTH
    global ecomm_param_int_1, ecomm_param_int_2, ecomm_param_float

    while uart.any() > 0:
        rcvd_char = uart.readchar()
        if (ecomm_fsm_state == FSM_SEEKING_START):
            if (rcvd_char == ECOMM_START_CHAR):
                ecomm_fsm_state = FSM_COLLECTING_CHARS
                ecomm_buffer[0] = rcvd_char
                ecomm_next_buffer_index = 1
                checksum_calculated = 0

        elif (ecomm_fsm_state == FSM_COLLECTING_CHARS):
            # note START or STOP character is ok in middle of message
            # when we reach the LAST expected char, it must be a STOP char
            # if not, then we discard whole message and return to FSM_SEEKING_START
            # if it IS a STOP character, then we check the message for valid checksum
            # and parse it of checksum ok, else discard it
            # there is no ACK or NACK on messages or discarded messages, they just
            # disappear.
            ecomm_buffer[ecomm_next_buffer_index] = rcvd_char
            if ((ecomm_next_buffer_index < ECOMM_MESSAGE_LENGTH_ENHANCED - 2)):
                checksum_calculated += rcvd_char
            ecomm_next_buffer_index += 1
            if (ecomm_next_buffer_index >= (ECOMM_MESSAGE_LENGTH_ENHANCED -1)):
                # waiting for end character
                ecomm_fsm_state = FSM_EXPECTING_STOP
                ecomm_next_buffer_index = ECOMM_MESSAGE_LENGTH_ENHANCED - 1

        elif (ecomm_fsm_state == FSM_EXPECTING_STOP):
            if (rcvd_char == ECOMM_END_CHAR):
                # successfully received END_CHAR as expected, so buffer is complete
                # now verify checksum before accepting message
                checksum_calculated = checksum_calculated & 0xFF
                if (checksum_calculated != ecomm_buffer[ECOMM_MESSAGE_LENGTH_ENHANCED - 2]):
                    ecomm_next_buffer_index = 0
                    ecomm_fsm_state = FSM_SEEKING_START
                    ecomm_num_error_checksum += 1
                    print(" CHECKSUM FAILED")
                else:
                    # here we capture parameters from message
                    cmd = ecomm_buffer[1]
                    ecomm_param_int_1 = ecomm_buffer[2] | (ecomm_buffer[3] << 8)
                    ecomm_param_int_2 = ecomm_buffer[4] | (ecomm_buffer[5] << 8)
                    aa = bytearray([ ecomm_buffer[2], ecomm_buffer[3], ecomm_buffer[4], ecomm_buffer[5] ])
                    # note the ustruct.unpack returns a tuple with the floating point value in the first element
                    ecomm_param_float = ustruct.unpack('<f', aa)[0]
                    # and call a function to actually DO the action requested
                    commands.process_cmd(cmd, sensor, img)

                    # then close out the (good) message
                    ecomm_next_buffer_index = 0
                    ecomm_fsm_state = FSM_SEEKING_START
                    ecomm_num_good_messages += 1
            else:
                # we wanted ECOMM_END_CHAR and its not -- this is an error
                # so throw away this buffer and wait for a new one to start
                    ecomm_next_buffer_index = 0
                    ecomm_fsm_state = FSM_SEEKING_START
                    ecomm_num_error_framing += 1

def get_int_param1():
    global ecomm_param_int_1
    return ecomm_param_int_1

def get_int_param2():
    global ecomm_param_int_2
    return ecomm_param_int_2

def get_float_param():
    global ecomm_param_float
    return ecomm_param_float
