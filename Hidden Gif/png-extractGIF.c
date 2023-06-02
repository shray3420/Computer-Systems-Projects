#include "png-extractGIF.h"

int png_extractGIF(const char *png_filename, const char *gif_filename)
{
  PNG *png = PNG_open(png_filename, "r");
  FILE *gif = fopen(gif_filename, "w");
  if (!png || !gif)
  {
    return ERROR_INVALID_FILE;
  }

  PNG_Chunk chunk;
  // Read chunks until reaching "IEND" or an invalid chunk:
  while (1)
  {
    // Read chunk and ensure we get a valid result (exit on error):

    if (PNG_read(png, &chunk) == 0)
    {
      fclose(gif);
      PNG_close(png);
      return ERROR_INVALID_CHUNK_DATA;
    }
    if (strcmp(chunk.type, "uiuc") == 0)
    {
      break;
    }

    // Check for the "IEND" chunk to exit:
    if (strcmp(chunk.type, "IEND") == 0)
    {
      fclose(gif);
      PNG_close(png);
      PNG_free_chunk(&chunk);
      return ERROR_NO_UIUC_CHUNK;
    }

    // Free the memory associated with the chunk we just read:
    PNG_free_chunk(&chunk);
  }
  fwrite(chunk.data, chunk.len, sizeof(char), gif);
  fclose(gif);
  PNG_close(png);
  PNG_free_chunk(&chunk);
  return 0;
}
