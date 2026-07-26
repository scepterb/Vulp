/**
 * @file io.cpp
 * @author Ben Beredo
 * @date 2026
 */

#ifndef __IO__
#define __IO__

#define MSG_MAGIC '%'
#define MSG_STOP '!'

void setupSerial();
int read_byte_blocking();
void read_header();

/** @brief Reads bytes from serial, blocking, until the stop byte is reached.
 *
 *  @param buf A pointer to a buffer to store the bytes.
 *  @param len The length of the buffer (to avoid overflow)
 */
void read_bytes(void* buf, uint16_t len);

/** @brief Reads a message from serial.
 *
 *  @param buf A pointer to a buffer to store the incoming message.
 *  @param len The length of the buffer (to avoid overflow)
 *
 *  @return 0 on success. -1 if overflow detected.
 */
void read_message(void* buf, uint16_t len);

#endif