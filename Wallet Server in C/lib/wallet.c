#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wallet.h"

/**
 * Initializes an empty wallet.
 */
void wallet_init(wallet_t *wallet)
{
  // Implement `wallet_init`
  wallet->head = NULL;
  pthread_mutex_init(&wallet->state, NULL);
}

/**
 * Returns the amount of a given `resource` in the given `wallet`.
 */
int wallet_get(wallet_t *wallet, const char *resource)
{
  // Implement `wallet_get`
  // basic linked list traversal
  // if the node's resource and the resource passed in are equal
  // then return acc balance
  resource_node *curr = wallet->head;
  // int total_balance = 0;
  while (curr)
  {
    if (strcmp(curr->resource_, resource) == 0)
      return curr->balance;
    curr = curr->next;
  }
  return 0;
}

/**
 * Modifies the amount of a given `resource` in a given `wallet by `delta`.
 * - If `delta` is negative, this function MUST NOT RETURN until the resource can be satisfied.
 *   (Ths function MUST BLOCK until the wallet has enough resources to satisfy the request.)
 * - Returns the amount of resources in the wallet AFTER the change has been applied.
 */
int wallet_change_resource(wallet_t *wallet, const char *resource, const int delta)
{
  // Implement `wallet_change_resource`
  pthread_mutex_lock(&wallet->state);

  resource_node *curr = wallet->head;

  // find the resource
  while (curr)
  {
    if (strcmp(curr->resource_, resource) == 0)
      break;
    curr = curr->next;
  }

  // if resource wasn't found or wallet is empty then add resource to the start of wallet

  if (!curr)
  {
    resource_node *node = malloc(sizeof(resource_node));
    node->resource_ = malloc(strlen(resource) + 1);
    strcpy(node->resource_, resource);
    // node->resource_ = resource;
    node->balance = 0;
    node->next = wallet->head;
    wallet->head = node;
    curr = node;
    pthread_cond_init(&curr->cond, NULL);
  }

  // if adding delta causes it to go negative then it should
  // wait for diff thread to update the wallet so that adding delta makes
  // this threads balance positive
  while (curr->balance + delta < 0)
    pthread_cond_wait(&curr->cond, &wallet->state);

  curr->balance += delta;

  pthread_cond_broadcast(&curr->cond);
  pthread_mutex_unlock(&wallet->state);

  return curr->balance; // returns updated balance
}

/**
 * Destroys a wallet, freeing all associated memory.
 */
void wallet_destroy(wallet_t *wallet)
{
  // Implement `wallet_destroy`

  resource_node *curr = wallet->head;

  // basic linked list destructor
  while (curr)
  {
    resource_node *temp = curr->next;
    free(curr->resource_);
    free(curr);
    curr = temp;
  }
}