#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "http.h"

/**
 * httprequest_parse_headers
 *
 * Populate a `req` with the contents of `buffer`, returning the number of bytes used from `buf`.
 */

ssize_t httprequest_parse_headers(HTTPRequest *req, char *buffer, ssize_t buffer_len)
{
  // size_t action_len = strcspn(buffer, " ");
  // char *action = malloc(action_len + 1);
  // memcpy(action, buffer, action_len);
  // action[action_len] = '\0';
  // req->action = action;
  // bytes_used += action_len;
  // buffer += action_len + 1; //points to /
  // char *leftover;
  char *buff = buffer;
  int bytes_used = 0;
  req->head = NULL;

  char *token = strtok_r(buff, " ", &buff);
  req->action = strdup(token);
  // bytes_used += strlen(token);

  token = strtok_r(NULL, " ", &buff);
  req->path = strdup(token);
  bytes_used += strlen(token);

  token = strtok_r(NULL, "\r\n", &buff);
  req->version = strdup(token);
  bytes_used += strlen(token);

  req->payload = NULL;

  while (token != NULL)
  {
    token = strtok_r(NULL, "\r\n", &buff);

    if (token == NULL)
      break;
    // printf("%s\n", token);
    if (strstr(token, ":"))
    {
      char *key = strtok_r(token, ": ", &token);
      char *value = strtok_r(NULL, ": ", &token);
      if (key != NULL && value != NULL)
      {
        Node *temp = malloc(sizeof(Node));
        temp->key = strdup(key);
        temp->value = strdup(value);

        if (req->head == NULL)
        {
          req->head = temp;
          req->head->next = NULL;
        }
        else
        {
          temp->next = req->head;
          req->head = temp;
        }
        if (strcmp(key, "Content-Length") == 0)
        {
          ssize_t content_len = atoi(value);
          if (content_len > 0)
          {

            req->payload = (char *)malloc(content_len);
            memcpy((void *)req->payload, buffer + buffer_len - content_len, content_len);
            // char *payload = strtok_r(NULL, "\r\n\r\n", &buffer);
            // req->payload = strdup(payload);
          }
        }
      }
    }
  }

  // free(version);
  // free(path);
  // free(action);
  // free(buff);
  if (req == NULL)
    return -1;
  return bytes_used;
}

/**
 * httprequest_read
 *
 * Populate a `req` from the socket `sockfd`, returning the number of bytes read to populate `req`.
 */
//
ssize_t httprequest_read(HTTPRequest *req, int sockfd)
{
  // ssize_t bytes_read = 0;
  char buffer[5 * 1024 * 1024 + 72];

  ssize_t bytes_read = read(sockfd, buffer, sizeof(buffer) - 1);
  ssize_t sockfd_len = 0;
  while (bytes_read > 0)
  {
    sockfd_len += bytes_read;
    bytes_read = read(sockfd, buffer + sockfd_len, sizeof(buffer) - sockfd_len - 1);
  }
  buffer[sockfd_len] = '\0';
  return httprequest_parse_headers(req, buffer, sockfd_len);
}

/**
 * httprequest_get_action
 *
 * Returns the HTTP action verb for a given `req`.
 */
const char *httprequest_get_action(HTTPRequest *req)
{
  return req->action;
}

/**
 * httprequest_get_header
 *
 * Returns the value of the HTTP header `key` for a given `req`.
 */
const char *httprequest_get_header(HTTPRequest *req, const char *key)
{
  Node *curr = req->head;
  while (curr != NULL)
  {
    if (strcmp(key, curr->key) == 0)
      return curr->value;
    curr = curr->next;
  }
  return NULL;
}

/**
 * httprequest_get_path
 *
 * Returns the requested path for a given `req`.
 */
const char *httprequest_get_path(HTTPRequest *req)
{
  return req->path;
}

/**
 * httprequest_destroy
 *
 * Destroys a `req`, freeing all associated memory.
 */
void httprequest_destroy(HTTPRequest *req)
{

  free((void *)req->action);
  free((void *)req->path);
  free((void *)req->version);
  free((void *)req->payload);

  Node *curr = req->head;
  while (curr != NULL)
  {
    Node *temp = curr->next;
    free(curr->key);
    free(curr->value);
    free(curr);
    curr = temp;
  }
  // free(req);
}