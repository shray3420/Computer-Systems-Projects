#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "emoji.h"
#include "emoji-translate.h"

int main()
{
  // Feel free to delete all of this and use this function in any way you want:
  // printf("Currently, `main.c` provides a basic call to your functions.\n");
  // printf("- Edit `main.c` to test your functions within the context of `main`.\n");
  // printf("- A test suite is provided in `tests` folder.\n");

  // printf("Your favorite emoji: %s\n", emoji_favorite());

  emoji_t emoji;
  emoji_init(&emoji);

  emoji_add_translation(&emoji, (const unsigned char *)"😊", (const unsigned char *)":happy:");

  unsigned char *translation = (unsigned char *)emoji_translate_file_alloc(&emoji, "tests/txt/text-inbetween.txt");
  // REQUIRE(translation != NULL);

  // INFO("translation := " << translation);
  // REQUIRE(strcmp((char *)translation, ":happy: CS340 :happy:") == 0);
  free(translation);

  emoji_destroy(&emoji);
  return 0;
}