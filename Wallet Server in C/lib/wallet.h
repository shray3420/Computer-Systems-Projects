#pragma once
#include <pthread.h>

#ifdef __cplusplus
extern "C"
{
#endif

  typedef struct resource_node
  {
    int balance;
    char *resource_;
    struct resource_node *next;
    pthread_cond_t cond;
  } resource_node;

  typedef struct wallet_t_
  {
    // Add anything here! :)
    resource_node *head;
    pthread_mutex_t state;
  } wallet_t;

  void wallet_init(wallet_t *wallet);
  int wallet_get(wallet_t *wallet, const char *resource);
  int wallet_change_resource(wallet_t *wallet, const char *resource, const int delta);
  void wallet_destroy(wallet_t *wallet);

#ifdef __cplusplus
}
#endif