#include "base.h"

NBASE_Parser *NBASE_Parser_new() {
    NBASE_Parser *p;
    NEW(p);
    p->numPendingTokens = 0;
    p->location = 0;
    p->col = 0;
    p->line = 0;
    p->subParser = 0;
    
    p->source = 0;
    p->restoreCol = 0;
    p->restoreLine = 0;
    p->keepWhitespace = 0;
    for (S32 i = 0; i < 100; i++) {
        p->pendingTokens[i].type = 0;
        p->pendingTokens[i].col = 0;
        p->pendingTokens[i].length = 0;
        p->pendingTokens[i].matchNumber = -1;
        p->pendingTokens[i].line = 0;
        p->pendingTokens[i].string = 0;
    }
    return p;
    
}

void NBASE_Parser_pushSource(NBASE_Parser *p, char *source) {
    if (!p->source) {
        p->source = source;
        p->location = source;
        p->col = 0;
        p->line = 0;
    } else {
        NBASE_Parser *pp = p;
        while (p->subParser) {
            pp = p->subParser;
        }
        pp->subParser = NBASE_Parser_new();
        p->subParser->source = source;
        p->subParser->location = source;
        p->subParser->col = 0;
        p->subParser->line = 0;
    }
}

S32 NBASE_Parser_isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

S32 NBASE_Parser_isNumeric(char c) {
    return c >= '0' && c <= '9';
}

S32 NBASE_Parser_isAlphaNumeric(char c) {
    return NBASE_Parser_isAlpha(c) || NBASE_Parser_isNumeric(c);
}

S32 NBASE_Parser_isWhiteSpace(char c) {
    return c == ' ' || c == '\n' || c == '\t' || c == '\r';
}

NBASE_Parser_token NBASE_Parser_getToken(NBASE_Parser *p) {
    if (p->subParser && p->subParser->source) {
        NBASE_Parser_token token = NBASE_Parser_getToken(p->subParser);
        if (token.type != TOKEN_EOF) {
            return token;
        }
        p->keepWhitespace = p->subParser->keepWhitespace;
    }
    
    NBASE_Parser_token token;
    if (p->numPendingTokens > 0) {
        token = p->pendingTokens[0];
        for (S32 i = 0; i < p->numPendingTokens-1; i++) {
            p->pendingTokens[i] = p->pendingTokens[i+1];
        }
        p->numPendingTokens--;
        return token;
    }
    token.type = TOKEN_UNKNOWN;
    token.string = p->location;
    token.col = p->col;
    token.line = p->line;
    token.matchNumber = 0;
    token.matchStringNumber = -1;
    token.length = 0;
    if (NBASE_Parser_isWhiteSpace(*p->location)) {
        while (NBASE_Parser_isWhiteSpace(*p->location)) {
            if (p->keepWhitespace) {
                token.length++;
            }
            p->location++;
            p->col++;
            if (*p->location == '\n') {
                p->line++;
                p->col = 0;
            }
        }
        if (p->keepWhitespace) {
            token.type = TOKEN_WHITESPACE;
            return token;
        }
    }

    if (p->source == 0 || *p->location == 0) {
        token.type = TOKEN_EOF;
        p->source = 0;
        return token;
    }
    
    token.string = p->location;
    token.col = p->col;
    token.line = p->line;
    token.length = 0;

    if (p->location[0] == '/' && p->location[1] == '/') {
        token.type = TOKEN_LINE_COMMENT;
        while (*p->location != '\n' && *p->location != 0) {
            p->location++;
            p->col++;
            token.length++;
        }
        if (*p->location == '\n') {
            p->line++;
            p->col = 0;
            token.length++;
        }
        return token;
    }

    if (p->location[0] == '/' && p->location[1] == '*') {
        token.type = TOKEN_BLOCK_COMMENT;
        while (p->location[0] && p->location[1] && p->location[0] != '*' && p->location[1] != '/') {
            p->location++;
            p->col++;
            if (*p->location == '\n') {
                p->line++;
                p->col = 0;
            }
            token.length++;
        }
        p->location += 2;
        p->col += 2;
        token.length += 2;
        return token;
    }

    char *doubles[] = {"<<", ">>", "||", "&&", 0};
    NBASE_Parser_tokenType doublesTokens[] = {TOKEN_DOUBLE_LARROW, TOKEN_DOUBLE_RARROW, TOKEN_OR, TOKEN_AND};
    for (S32 i = 0; doubles[i] != 0; i++) {
        if (p->location[0] == doubles[i][0] && p->location[1] == doubles[i][1]) {
            token.type = doublesTokens[i];
            p->location += 2;
            p->col += 2;
            token.length = 2;
            return token;
        }
    }

    char singles[] = {'!', '(', ')', '[', ']', '<', '>', '-', 0};
    NBASE_Parser_tokenType singlesTokens[] = {TOKEN_BANG, TOKEN_LPAREN, TOKEN_RPAREN, TOKEN_LBRACKET, TOKEN_RBRACKET, TOKEN_LARROW, TOKEN_RARROW, TOKEN_HYPHEN};
    for (S32 i = 0; singles[i] != 0; i++) {
        if (*p->location == singles[i]) {
            token.type = singlesTokens[i];
            p->location++;
            p->col++;
            token.length = 1;
            return token;
        }
    }

    if (NBASE_Parser_isAlpha(*p->location) || *p->location == '_') {
        token.type = TOKEN_IDENTIFIER;
        while (NBASE_Parser_isAlphaNumeric(*p->location) || *p->location == '_' || *p->location == '-') {
            p->location++;
            p->col++;
            token.length++;
        }
        return token;
    }
    if (NBASE_Parser_isNumeric(*p->location)) {
        token.type = TOKEN_NUMBER;
        while (NBASE_Parser_isNumeric(*p->location)) {
            p->location++;
            p->col++;
            token.length++;
        }
        return token;
    }
    // printf("Unknown character: %c\n", *p->location);
    token.length = 1;
    p->location++;
    return token;
}

void NBASE_ParserToken_extractString(NBASE_Parser_token *token, char *dest) {
    for (S32 i = 0; i < token->length; i++) {
        dest[i] = token->string[i];
    }
    dest[token->length] = 0;
}

char *NBASE_ParserToken_copyString(NBASE_Parser_token *token) {
    char *out;
    NEWN(out, token->length+1);
    for (S32 i = 0; i < token->length; i++) {
        out[i] = token->string[i];
    }
    out[token->length] = 0;
    return out;
}

S32 NBASE_ParserToken_matchesString(NBASE_Parser_token *token, const char *testMatch) {
    for (S32 i = 0; i < token->length; i++) {
        if (testMatch[i] != token->string[i]) {
            return 0;
        }
    }
    return testMatch[token->length] == 0;
}

S32 NBASE_Parser_expectToken(NBASE_Parser_token expected, NBASE_Parser_token got) {
    if (expected.type == got.type) {
        if (expected.length == 0) return 1;
        return NBASE_ParserToken_matchesString(&got, expected.string);
    }
    return 0;
}

void NBASE_Parser_skipWhitespace(NBASE_Parser *p) {
    if (p->subParser && p->subParser->source) {
        return NBASE_Parser_skipWhitespace(p->subParser);
    }
    p->keepWhitespace = 0;
}

void NBASE_Parser_keepWhitespace(NBASE_Parser *p) {
    if (p->subParser && p->subParser->source) {
        return NBASE_Parser_keepWhitespace(p->subParser);
    }
    p->keepWhitespace = 1;
}


S32 NBASE_Parser_EOF(NBASE_Parser *p) {
    if (p->subParser && p->subParser->source && p->subParser->location && *p->subParser->location) {
        return 0;
    }
    if (p->numPendingTokens > 0) {
        return p->pendingTokens[0].type == TOKEN_EOF;
    }
    return (!p->source || *p->location == 0);
}

S32 NBASE_Parser_matchSeries(NBASE_Parser *p, S32 seriesLength, NBASE_Parser_match *series, S32 *matchLength, NBASE_Parser_token *out) {
    if (p->subParser && p->subParser->source) {
        return NBASE_Parser_matchSeries(p->subParser, seriesLength, series, matchLength, out);
    }
    *matchLength = 0;
    char *locationBeforeSeries = p->location;
    S32 colBeforeSeries = p->col;
    S32 lineBeforeSeries = p->line;

    NBASE_Parser_token pulledTokens[100];
    S32 numPulledTokens = 0;

    NBASE_Parser_token token;
    S32 skipPull = 0;
    
    for (S32 i = 0; i < seriesLength; i++) {
        if (!skipPull) {
            token = NBASE_Parser_getToken(p);
            pulledTokens[numPulledTokens++] = token;
        }
        skipPull = 0;
        // No matches array provided, which means we just want to check the token type
        if (series[i].matches == 0 && (token.type == series[i].type || series[i].type == TOKEN_ANY)) {
            out[*matchLength] = token;
            token.matchNumber = i;
            (*matchLength)++;
            continue;
        // Matches array provided, which means we try each one until we get to the null token
        } else if (series[i].matches != 0) {
            S32 found = 0;
            S32 idx = 0;
            while (series[i].matches[idx]) {
                char *match = series[i].matches[idx];
                S32 matchLen = strlen(match);
                if (NBASE_Parser_expectToken((NBASE_Parser_token){.type = token.type, .length = matchLen, .string = match}, token)) {
                    token.matchNumber = i;
                    token.matchStringNumber = idx;
                    found = 1;
                    out[*matchLength] = token;
                    (*matchLength)++;
                    break;
                }
                idx++;
            }
            if (found) {
                continue;
            }
        }
        if (series[i].optional) {
            skipPull = 1;
            // If the last token was optional and did not match, we need to give the token back
            if (i == seriesLength-1) {
                for (S32 j = p->numPendingTokens; j > 0; j--) {
                    p->pendingTokens[j] = p->pendingTokens[j-1];
                }
                p->pendingTokens[0] = token;
                p->numPendingTokens++;
                return 1;
            }
            continue;
        } else {
            for (S32 j = 0; j < numPulledTokens; j++) {
                if (pulledTokens[numPulledTokens].type != TOKEN_EOF) {
                    for (S32 k = p->numPendingTokens; k > 0; k--) {
                        p->pendingTokens[k] = p->pendingTokens[k-1];
                    }
                    p->pendingTokens[0] = pulledTokens[j];
                    p->numPendingTokens++;
                }
            }
            return 0;
        }
    }
    return 1;
}