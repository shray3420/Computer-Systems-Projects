#include "png-hideGIF.h"

int png_hideGIF(const char *png_filename_source, const char *gif_filename, const char *png_filename_out)
{

  PNG *png = PNG_open(png_filename_source, "r");
  FILE *gif = fopen(gif_filename, "r");
  if (!png || !gif)
  {
    return ERROR_INVALID_FILE;
  }

  PNG *out = PNG_open(png_filename_out, "w");
  printf("PNG Header written.\n");

  size_t bytesWritten;
  size_t written_already = 0;

  //  Read chunks until reaching "IEND" or in invalid chunk:
  while (1)
  {
    PNG_Chunk chunk;
    if (PNG_read(png, &chunk) == 0)
    {
      fclose(gif);
      PNG_close(png);
      PNG_close(out);
      return ERROR_INVALID_CHUNK_DATA;
    }
    // checks if the gif is already written (0 being false)
    // and checks that it is not the first chunk
    // creates a chunk for the gif and sets all of the chunk data properly
    // updates written already to 1 so that it doesn't get written in the future
    if (written_already == 0 && strcmp(chunk.type, "IHDR") != 0)
    {
      PNG_Chunk g;
      // moves pointer to the end
      // sets the legnth of the chunk to be legnth of file
      // moves pointer back to start
      fseek(gif, 0, SEEK_END);
      g.len = ftell(gif);
      fseek(gif, 0, SEEK_SET);

      char uiuc[5] = {'u', 'i', 'u', 'c', '\x0'};
      strcpy(g.type, uiuc);

      // if length of gif file is >0 then read in the file into chunk.data
      if (g.len > 0)
      {
        g.data = malloc(g.len);
        fread(g.data, sizeof(char), g.len, gif);
      }
      else
      {
        g.data = 0;
      }

      PNG_write(out, &g);
      free(g.data);
      // updated written_already so this block of code doesn't run again
      written_already = 1;
    }

    // Report data about the chunk to the command line:
    bytesWritten = PNG_write(out, &chunk);
    printf("PNG chunk %s written (%lu bytes)\n", chunk.type, bytesWritten);

    // Check for the "IEND" chunk to exit:
    if (strcmp(chunk.type, "IEND") == 0)
    {
      PNG_free_chunk(&chunk);
      break;
    }

    // Free the memory associated with the chunk we just read:
    PNG_free_chunk(&chunk);
  }
  fclose(gif);
  PNG_close(out);
  PNG_close(png);
  return 0;
}
