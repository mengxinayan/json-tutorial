#include "leptjson.h"
#include <assert.h>  /* assert() */
#include <stdlib.h>  /* NULL, strtod() */
#include <string.h>  /* string, strlen() */
#include <math.h>    /* HUGE_VAL */


#define EXPECT(c, ch)           do { assert(*c->json == (ch)); c->json++; } while(0)
#define IS_DIGIT(ch)            ((ch) >= '0' && (ch) <= '9')
#define IS_DIGIT_1_TO_9(ch)     ((ch) >= '1' && (ch) <= '9')

typedef struct {
    const char* json;
}lept_context;

static void lept_parse_whitespace(lept_context* c) {
    const char *p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    c->json = p;
}

/**
 * 可以传入要比较的字符串与其类型，这样就方便进行赋值
 * Exist Warning: ISO C90 forbids mixed declarations and code [-Wdeclaration-after-statement]
 * 把 int word_length; int i; 放到函数最前面就好了
 */
static int lept_parse_literal(lept_context* c, lept_value* v, char* word, lept_type type) {
    int word_length;
    int i;
    EXPECT(c, word[0]);
    word_length = strlen(word);
    for (i = 1; i != word_length; i++) {
        if (c->json[0] != word[i])
            return LEPT_PARSE_INVALID_VALUE;
        c->json++;
    }
    v->type = type;
    return LEPT_PARSE_OK;
}

#if 0
static int lept_parse_true(lept_context* c, lept_value* v) {
    EXPECT(c, 't');
    if (c->json[0] != 'r' || c->json[1] != 'u' || c->json[2] != 'e')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 3;
    v->type = LEPT_TRUE;
    return LEPT_PARSE_OK;
}

static int lept_parse_false(lept_context* c, lept_value* v) {
    EXPECT(c, 'f');
    if (c->json[0] != 'a' || c->json[1] != 'l' || c->json[2] != 's' || c->json[3] != 'e')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 4;
    v->type = LEPT_FALSE;
    return LEPT_PARSE_OK;
}

static int lept_parse_null(lept_context* c, lept_value* v) {
    EXPECT(c, 'n');
    if (c->json[0] != 'u' || c->json[1] != 'l' || c->json[2] != 'l')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 3;
    v->type = LEPT_NULL;
    return LEPT_PARSE_OK;
}
#endif

static int lept_parse_number(lept_context* c, lept_value* v) {
    /* char* end; */
    /* \TODO validate number */
    const char* tmp;
    tmp = c->json;
    if (*tmp == '-')
        tmp++;
    if (IS_DIGIT_1_TO_9(*tmp)) {
        tmp++;
        /** 
         * It will wrong when add this code
         * if (!IS_DIGIT(*tmp))
            return LEPT_PARSE_INVALID_VALUE;
         * Ihe number having one number, such as 1, 2
         */
        while (IS_DIGIT(*tmp))
            tmp++;
    } else if (*tmp == '0') {
        tmp++;
    } else {
        return LEPT_PARSE_INVALID_VALUE;
    }
    if (*tmp == '.') {
        tmp++;
        if (!IS_DIGIT(*tmp))
            return LEPT_PARSE_INVALID_VALUE;
        while (IS_DIGIT(*tmp))
            tmp++;
    }
    if (*tmp == 'e' || *tmp == 'E') {
        tmp++;
        if (*tmp == '+' || *tmp == '-')
            tmp++;
        if (!IS_DIGIT(*tmp))
            return LEPT_PARSE_INVALID_VALUE;
        while (IS_DIGIT(*tmp))
            tmp++;
    }
    v->n = strtod(c->json, NULL);
    if ((v->n == HUGE_VAL || v->n == -HUGE_VAL))
        return LEPT_PARSE_NUMBER_TOO_BIG;
    c->json = tmp;
    v->type = LEPT_NUMBER;
    return LEPT_PARSE_OK;

#if 0
    v->n = strtod(c->json, &end);
    if (c->json == end)
        return LEPT_PARSE_INVALID_VALUE;
    c->json = end;
    v->type = LEPT_NUMBER;
    return LEPT_PARSE_OK;
#endif
}

static int lept_parse_value(lept_context* c, lept_value* v) {
    switch (*c->json) {
        #if 0
        case 't':  return lept_parse_true(c, v);
        case 'f':  return lept_parse_false(c, v);
        case 'n':  return lept_parse_null(c, v);
        #endif
        case 't':  return lept_parse_literal(c, v, "true", LEPT_TRUE);
        case 'f':  return lept_parse_literal(c, v, "false", LEPT_FALSE);
        case 'n':  return lept_parse_literal(c, v, "null", LEPT_NULL);
        default:   return lept_parse_number(c, v);
        case '\0': return LEPT_PARSE_EXPECT_VALUE;
    }
}

int lept_parse(lept_value* v, const char* json) {
    lept_context c;
    int ret;
    assert(v != NULL);
    c.json = json;
    v->type = LEPT_NULL;
    lept_parse_whitespace(&c);
    if ((ret = lept_parse_value(&c, v)) == LEPT_PARSE_OK) {
        lept_parse_whitespace(&c);
        if (*c.json != '\0') {
            v->type = LEPT_NULL;
            ret = LEPT_PARSE_ROOT_NOT_SINGULAR;
        }
    }
    return ret;
}

lept_type lept_get_type(const lept_value* v) {
    assert(v != NULL);
    return v->type;
}

double lept_get_number(const lept_value* v) {
    #if 0
    assert(v != NULL && v->type == LEPT_NUMBER);
    #endif
    assert(v != NULL);
    assert(v->type == LEPT_NUMBER);
    
    return v->n;
}
