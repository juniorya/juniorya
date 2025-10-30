/**
 * @file st_compiler.c
 * @brief Structured Text to C translation for the desktop IDE.
 */

#include "st_compiler.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *keywords[] = {"PROGRAM",   "END_PROGRAM", "VAR",     "END_VAR",  "IF",
                                 "THEN",      "ELSE",        "ELSIF",   "FOR",      "TO",
                                 "BY",        "DO",          "END_IF",  "END_FOR",  "WHILE",
                                 "END_WHILE", "RETURN"};

static bool is_keyword(const char *token)
{
    for (size_t i = 0U; i < (sizeof(keywords) / sizeof(keywords[0])); ++i)
    {
        if (strcmp(token, keywords[i]) == 0)
        {
            return true;
        }
    }
    return false;
}

static void add_token(stc_compilation *out, size_t start, size_t length, const char *category)
{
    if ((out == NULL) || (out->token_count >= STC_MAX_TOKENS))
    {
        return;
    }
    out->tokens[out->token_count].start = start;
    out->tokens[out->token_count].length = length;
    strncpy(out->tokens[out->token_count].category, category, sizeof(out->tokens[out->token_count].category) - 1U);
    out->tokens[out->token_count].category[sizeof(out->tokens[out->token_count].category) - 1U] = '\0';
    out->token_count++;
}

static void emit_line(stc_compilation *out, const char *line)
{
    if ((out == NULL) || (line == NULL))
    {
        return;
    }
    size_t len = strlen(line);
    if ((out->c_length + len + 2U) >= STC_MAX_OUTPUT)
    {
        return;
    }
    memcpy(&out->c_source[out->c_length], line, len);
    out->c_length += len;
    out->c_source[out->c_length++] = '\n';
    out->c_source[out->c_length] = '\0';
}

static void translate_assignment(char *buffer, size_t capacity)
{
    char *assign = strstr(buffer, ":=");
    if (assign != NULL)
    {
        assign[0] = '=';
        memmove(assign + 1, assign + 2, strlen(assign + 2) + 1U);
    }
    size_t len = strlen(buffer);
    if ((len == 0U) || (buffer[len - 1U] != ';'))
    {
        if ((len + 2U) < capacity)
        {
            buffer[len] = ';';
            buffer[len + 1U] = '\0';
        }
    }
}

static void process_line(const char *line, size_t offset, stc_compilation *out)
{
    char token[64];
    size_t idx = 0U;
    size_t token_start = 0U;
    while (line[idx] != '\0')
    {
        if (isspace((unsigned char)line[idx]))
        {
            idx++;
            continue;
        }
        token_start = idx;
        size_t len = 0U;
        while ((line[idx] != '\0') && !isspace((unsigned char)line[idx]))
        {
            if (len + 1U < sizeof(token))
            {
                token[len++] = (char)toupper((unsigned char)line[idx]);
            }
            idx++;
        }
        token[len] = '\0';
        if (len > 0U)
        {
            if (is_keyword(token))
            {
                add_token(out, offset + token_start, len, "keyword");
            }
            else if (isalpha((unsigned char)token[0]))
            {
                add_token(out, offset + token_start, len, "identifier");
            }
            else if (isdigit((unsigned char)token[0]))
            {
                add_token(out, offset + token_start, len, "number");
            }
        }
    }
}

static void translate_source(const char *source, stc_compilation *out)
{
    const char *cursor = source;
    size_t offset = 0U;
    emit_line(out, "#include \"ide/st_runtime.h\"");
    emit_line(out, "void st_program_entry(st_runtime_context *ctx)");
    emit_line(out, "{");
    while (*cursor != '\0')
    {
        char line[256];
        size_t len = 0U;
        while ((*cursor != '\0') && (*cursor != '\n') && (len + 1U < sizeof(line)))
        {
            line[len++] = *cursor;
            cursor++;
        }
        line[len] = '\0';
        if (*cursor == '\n')
        {
            cursor++;
            offset++;
        }
        offset += len;
        if (len == 0U)
        {
            continue;
        }
        process_line(line, offset - len, out);
        translate_assignment(line, sizeof(line));
        char c_line[320];
        snprintf(c_line, sizeof(c_line), "    %s", line);
        emit_line(out, c_line);
    }
    emit_line(out, "}");
}

bool stc_compile_string(const char *source, stc_compilation *out)
{
    if ((source == NULL) || (out == NULL))
    {
        return false;
    }
    memset(out, 0, sizeof(*out));
    translate_source(source, out);
    return out->c_length > 0U;
}

bool stc_compile_file(const char *path, stc_compilation *out)
{
    if ((path == NULL) || (out == NULL))
    {
        return false;
    }
    FILE *fp = fopen(path, "r");
    if (fp == NULL)
    {
        return false;
    }
    char buffer[2048];
    size_t len = fread(buffer, 1U, sizeof(buffer) - 1U, fp);
    fclose(fp);
    buffer[len] = '\0';
    return stc_compile_string(buffer, out);
}
