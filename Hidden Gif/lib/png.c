#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "crc32.h"
#include "png.h"

const int ERROR_INVALID_PARAMS = 1;
const int ERROR_INVALID_FILE = 2;
const int ERROR_INVALID_CHUNK_DATA = 3;
const int ERROR_NO_UIUC_CHUNK = 4;

/**
 * Opens a PNG file for reading (mode == "r" or mode == "r+") or writing (mode == "w").
 *
 * (Note: The function follows the same function prototype as `fopen`.)
 *
 * When the file is opened for reading this function must verify the PNG signature.  When opened for
 * writing, the file should write the PNG signature.
 *
 * This function must return NULL on any errors; otherwise, return a new PNG struct for use
 * with further fuctions in this library.
 */
PNG *PNG_open(const char *filename, const char *mode)
{
  PNG *png = malloc(sizeof(PNG));
  png->f = fopen(filename, mode);

  if (png->f == NULL)
  {
    free(png);
    return NULL;
  }

  // fseek(png->f, 8, SEEK_SET);

  // check in PNG is actually a PNG by checking first 8 bytes for PNG signature
  if (*mode == 'r' || strcmp(mode, "r+") == 0)
  {
    char cmp[8];
    if (fread(cmp, sizeof(char), 8, png->f) != 8 || memcmp(cmp, "\x89PNG\r\n\x1a\n", 8) != 0)
    {
      fclose(png->f);
      free(png);
      return NULL;
    }
  }
  else if (*mode == 'w')
  {
    fwrite("\x89PNG\r\n\x1a\n", sizeof(char), 8, png->f);
  }

  return png;
}

/**
 * Reads the next PNG chunk from `png`.
 *
 * If a chunk exists, a the data in the chunk is populated in `chunk` and the
 * number of bytes read (the length of the chunk in the file) is returned.
 * Otherwise, a zero value is returned.
 *
 * Any memory allocated within `chunk` must be freed in `PNG_free_chunk`.
 * Users of the library must call `PNG_free_chunk` on all returned chunks.
 */
size_t PNG_read(PNG *png, PNG_Chunk *chunk)
{
  if (png == NULL || chunk == NULL)
    return 0;

  // total number of bytes written
  size_t byte_total = 0;

  // read 4 byte length of chunk
  // if fread does not return 4 then an error happened and 0 should be returned
  // must convert chunk len from network byte order into host byte order
  // add byte len to the total length
  size_t byte_len = fread(&chunk->len, sizeof(char), sizeof(uint32_t), png->f);
  if (byte_len != 4)
    return 0;
  byte_total += byte_len;
  chunk->len = ntohl(chunk->len);

  // read 4 byte type of chunk
  // if fread doesn't return 4 then an error happened and 0 should be returned
  // set the chunk's last element to 0 to serve as terminating character
  // add byte type to total length
  size_t byte_type = fread(chunk->type, sizeof(char), sizeof(uint32_t), png->f);
  if (byte_type != sizeof(uint32_t))
    return 0;
  chunk->type[4] = 0;
  byte_total += byte_type;

  // if chunk->len is 0 then set data to be NULL

  if (chunk->len == 0)
  {
    chunk->data = NULL;
    // return 0;
  }
  // if it is > 0 then read in chunk->len's byte length of data into chunk->data
  // check to make sure fread happened properly, return 0 if error
  // add byte chunk len to the total
  else if (chunk->len > 0)
  {
    chunk->data = malloc(chunk->len);
    size_t byte_chunk_len = fread(chunk->data, sizeof(char), chunk->len, png->f);
    if (byte_chunk_len != chunk->len)
      return 0;
    byte_total += byte_chunk_len;
  }
  // read 4 byte length of crc
  // if fread does not return 4 then an error happened and 0 should be returned
  // must convert chunk crc from network byte order into host byte order
  // add crc len to the total length
  size_t byte_crc = fread(&chunk->crc, sizeof(char), sizeof(uint32_t), png->f);
  if (byte_crc != sizeof(uint32_t))
    return 0;
  byte_total += byte_crc;
  chunk->crc = ntohl(chunk->crc);

  return byte_total;
}

/**
 * Writes a PNG chunk to `png`.
 *
 * Returns the number of bytes written.
 */
size_t PNG_write(PNG *png, PNG_Chunk *chunk)
{
  if (png == NULL || chunk == NULL)
    return 0;

  // counter to track the total number of bytes written
  // update total
  size_t byte_total = 0;

  // convert chunk->len into network byte order
  uint32_t byte_network = htonl(chunk->len);

  // writes the converted net length to the png file
  // update total
  size_t byte_network_len = fwrite(&byte_network, sizeof(uint32_t), sizeof(char), png->f) * sizeof(uint32_t);
  byte_total += byte_network_len;

  // writes the length of chunk type into the png file
  // update total
  size_t byte_type = fwrite(chunk->type, sizeof(char), sizeof(uint32_t), png->f);
  byte_total += byte_type;

  // writes the length of chunk data into png file
  // update total
  size_t byte_data = fwrite(chunk->data, sizeof(char), chunk->len, png->f);
  byte_total += byte_data;

  unsigned char buffer[sizeof(uint32_t) + chunk->len];

  // copy in the chunk type into buffer
  memcpy(buffer, chunk->type, sizeof(uint32_t));
  // copy in chunk data into buffer "byte_type" bytes after the start
  memcpy(buffer + byte_type, chunk->data, chunk->len);

  // calculate crc using the function given
  uint32_t crc = 0;
  crc32(buffer, sizeof(uint32_t) + chunk->len, &crc);

  // write the crc to png fine in network byte order
  uint32_t crc_net = htonl(crc);
  size_t byte_crc = fwrite(&crc_net, sizeof(uint32_t), sizeof(char), png->f) * sizeof(uint32_t);
  byte_total += byte_crc;

  return byte_total;
}

/**
 * Frees all memory allocated by this library related to `chunk`.
 */
void PNG_free_chunk(PNG_Chunk *chunk)
{
  if (chunk != NULL)
  {
    free(chunk->data);
    chunk->data = NULL;
  }
}

/**
 * Closes the PNG file and frees all memory related to `png`.
 */
void PNG_close(PNG *png)
{
  if (png == NULL)
    return;
  fclose(png->f);
  png->f = NULL;
  free(png);
}