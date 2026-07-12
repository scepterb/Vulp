/**
 * @file io.cpp
 * @author Ben Beredo
 * @date 2026
 */

#include "inc/io.h"

int read_byte_blocking() {
  while (!Serial.available()) {}
  int incomingByte = Serial.read();  // will not be -1
  return incomingByte;
}

/** @brief Reads bytes from serial, blocking, until the stop byte is reached.
 * 
 *  @param buf A pointer to a buffer to store the bytes.
 *  @param len The length of the buffer (to avoid overflow)
 */
void read_bytes(void *buf, uint16_t len) {
  int incomingByte = 0;

  for (int i = 0; i < len; i++) {
    incomingByte = read_byte_blocking();

    if (incomingByte == MSG_STOP) {
      break;
    }
    ((uint8_t *)buf)[i] = incomingByte;
  }
}

void read_header() {
  int magic = read_byte_blocking();
  while (magic != MSG_MAGIC) {
    magic = read_byte_blocking();
  }
}

/** @brief Reads a message from serial.
 * 
 *  @param buf A pointer to a buffer to store the incoming message.
 *  @param len The length of the buffer (to avoid overflow)
 */
void read_message(void *buf, uint16_t len)
{
  read_header();

  if (buf != NULL) {
    read_bytes(buf, len);
  }
}

void setupSerial() {
  Serial.begin(115200);
  while (!Serial) delay(10);
}