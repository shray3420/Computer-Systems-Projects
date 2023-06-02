#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "emoji.h"
#include "emoji-translate.h"

void emoji_init(emoji_t *emoji)
{
  emoji->head = NULL;
}

void emoji_add_translation(emoji_t *emoji, const unsigned char *source, const unsigned char *translation)
{

  // initalize a new node to add
  emoji_node *node = (emoji_node *)malloc(sizeof(emoji_node));

  // add source to node and initialize source length
  node->src = (unsigned char *)malloc(strlen(source) + 1);
  strcpy(node->src, source);
  // int source_len = strlen(source);
  //  add translation to node
  node->trans = (unsigned char *)malloc(strlen(translation) + 1);
  strcpy(node->trans, translation);

  // add lengths of the emoji to node
  node->src_len = strlen(source);
  node->trans_len = strlen(translation);

  // point node's next to null
  node->next = NULL;

  // add node to linked list in descending order
  // guarantees that the longest emoji is at the start
  // first part of if statement checks if list is empty or if
  // the new emoji is longer then the head
  // in both instances the node would be added to start of linked list
  if (emoji->head == NULL || strlen(source) > strlen(emoji->head->src))
  {
    node->next = emoji->head;
    emoji->head = node;
  }
  // middle case
  else
  {
    // regular linked list insert
    emoji_node *curr = emoji->head;
    while (curr->next != NULL && curr->src_len <= node->src_len)
      curr = curr->next;

    // properly set next's
    node->next = curr->next;
    curr->next = node;
  }
}

const unsigned char *emoji_translate_file_alloc(emoji_t *emoji, const char *fileName)
{
  // open and read file in a way that concatenates the entire file into one string
  FILE *f;
  f = fopen(fileName, "r");
  if (f == NULL)
    return NULL;
  long bytes;
  char *line;
  fseek(f, 0L, SEEK_END);
  bytes = ftell(f);
  fseek(f, 0L, SEEK_SET);

  line = (char *)calloc(bytes, sizeof(char) + 1);
  if (line == NULL)
  {
    fclose(f);
    return NULL;
  }
  fread(line, sizeof(char), bytes, f);
  fclose(f);

  // set len equal to 0, will be updated later once it is calculated
  int translated_len = 0;

  emoji_node *curr = emoji->head;

  while (curr != NULL)
  {
    // only want to iterate to next emoji if it gets found
    // strstr points to the point in memory that the emoji was found
    unsigned char *found = strstr(line, curr->src);
    if (found == NULL)
    {
      curr = curr->next;
      continue;
    }

    // calcultes how many bytes are needed and allocates that much as well as storing the length into a new variable
    unsigned char *copy = (unsigned char *)malloc(strlen(line) - curr->src_len + curr->trans_len + 1);
    translated_len = strlen(line) - curr->src_len + curr->trans_len;
    int idx = 0;

    // loop that checks all the way up to when an emoji is found
    for (size_t old_i = 0; old_i < strlen(line); old_i++)
    {
      if (found != (unsigned char *)&line[old_i])
      {
        copy[old_i] = line[old_i];
      }
      else
      {
        idx = old_i;
        break;
      }
    }
    // sets the proper chars in the copy and copies in the translation
    for (int j = idx; j < idx + curr->trans_len; j++)
      copy[j] = curr->trans[j - idx];

    // stopping index will be 1 before the end so that room is left for terminating character
    int stop_idx = idx + curr->trans_len;

    // loop through everything after emoji
    for (size_t k = idx + curr->src_len; k < strlen(line); k++)
    {
      copy[stop_idx] = line[k];
      stop_idx++;
    }
    // using the stored variable from before we set the last character to be the terminating character
    copy[translated_len] = '\0';

    // delete what is in line and replace it with the translated copy
    free(line);
    line = copy;
  }
  return line;
}

void emoji_destroy(emoji_t *emoji)
{
  // create a node and point it to head
  // loop through linked list and free all of the data fields inside and then the node itself
  // point head to null
  emoji_node *curr = emoji->head;
  while (curr != NULL)
  {
    emoji_node *temp = curr->next;
    free(curr->src);
    free(curr->trans);
    free(curr);
    // free(curr->src_len);
    // free(curr->trans_len);
    curr = temp;
  }
  emoji->head = NULL;
}
