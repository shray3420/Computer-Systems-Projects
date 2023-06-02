#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Return your favorite emoji.  Do not allocate new memory.
// (This should **really** be your favorite emoji, we plan to use this later in the semester. :))
char *emoji_favorite()
{
  return "\xF0\x9F\x98\xB9";
}

// Count the number of emoji in the UTF-8 string `utf8str`, returning the count.  You should
// consider everything in the ranges starting from (and including) U+1F000 up to (and including) U+1FAFF.
int emoji_count(const unsigned char *utf8str)
{
  int count = 0;
  int len = strlen(utf8str);
  for (int i = 0; i < len - 3; i++)
  {
    if (utf8str[i] == 0xF0 && utf8str[i + 1] == 0x9F && utf8str[i + 2] <= 0xAB && utf8str[i + 2] >= 0x80 && utf8str[i + 3] <= 0xBF && utf8str[i + 3] >= 0x80)
    {
      count++;
    }
  }
  return count;
}

// Return a random emoji stored in new heap memory you have allocated.  Make sure what
// you return is a valid C-string that contains only one random emoji.
char *emoji_random_alloc()
{
  int two = rand() % 0x2B + 0x80;
  int three = rand() % 0x3F + 0x80;

  char *s = (char *)malloc(100);
  strcpy(s, "\xF0\x9F\x98\x8A");

  s[2] = two;
  s[3] = three;

  return s;
}

// Modify the UTF-8 string `utf8str` to invert the FIRST character (which may be up to 4 bytes)
// in the string if it the first character is an emoji.  At a minimum:
// - Invert "😊" U+1F60A ("\xF0\x9F\x98\x8A") into ANY non-smiling face.
// - Choose at least five more emoji to invert.
//
void emoji_invertChar(unsigned char *utf8str)
{
  if (utf8str[0] == 0xF0 && utf8str[1] == 0x9F && utf8str[2] == 0x98 && utf8str[3] == 0x8A)
  {
    // changes from smiling (\xF0\x9F\x98\x8A) to frown
    utf8str[3] = 0x94;
  }
  if (utf8str[0] == 0xF0 && utf8str[1] == 0x9F && utf8str[2] == 0xA4 && utf8str[3] == 0x96)
  {
    // changes from robot(\xF0\x9F\xA4\x96) to human
    utf8str[2] = 0x99;
    utf8str[3] = 0x8D;
  }

  if (utf8str[0] == 0xF0 && utf8str[1] == 0x9F && utf8str[2] == 0x99 && utf8str[3] == 0x82)
  {
    // changes from  right side up face(\xF0\x9F\x99\x82) to upside down face
    utf8str[3] = 0x83;
  }
  if (utf8str[0] == 0xF0 && utf8str[1] == 0x9F && utf8str[2] == 0xA5 && utf8str[3] == 0xB5)
  {
    // changes from hot(\xF0\x9F\xA5\B5) to cold
    utf8str[3] = 0xB6;
  }
  if (utf8str[0] == 0xF0 && utf8str[1] == 0x9F && utf8str[2] == 0x98 && utf8str[3] == 0x90)
  {
    // changes from neutral face(\xF0\x9F\x98\x90) for wavy face
    utf8str[2] = 0xA5;
    utf8str[3] = 0xB4;
  }
  if (utf8str[0] == 0xF0 && utf8str[1] == 0x9F && utf8str[2] == 0x91 && utf8str[3] == 0x88)
  {
    // changes from left(\xF0\x9F\x91\x88) to right
    utf8str[3] = 0x89;
  }
}

// Modify the UTF-8 string `utf8str` to invert ALL of the character by calling your
// `emoji_invertChar` function on each character.
void emoji_invertAll(unsigned char *utf8str)
{
  int len = strlen(utf8str);
  for (int i = 0; i < len; i++)
    emoji_invertChar(&utf8str[i]);
}

// Reads the full contents of the file `fileName, inverts all emojis, and
// returns a newly allocated string with the inverted file's content.
unsigned char *emoji_invertFile_alloc(const char *fileName)
{
  FILE *f;
  f = fopen(fileName, "r");
  if (f == NULL)
    return NULL;
  char *ans = malloc(100);
  ans = fgets(ans, 100, f);
  emoji_invertAll(ans);
  fclose(f);
  return ans;
}
