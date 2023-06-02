#pragma once

#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct emoji_node
    {
        unsigned char *src;
        unsigned char *trans;
        int src_len;
        int trans_len;
        struct emoji_node *next;
    } emoji_node;

    typedef struct _emoji_t
    {
        emoji_node *head;
    } emoji_t;

    void emoji_init(emoji_t *emoji);
    void emoji_add_translation(emoji_t *emoji, const unsigned char *source, const unsigned char *translation);
    const unsigned char *emoji_translate_file_alloc(emoji_t *emoji, const char *fileName);
    void emoji_destroy(emoji_t *emoji);

#ifdef __cplusplus
}
#endif