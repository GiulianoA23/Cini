#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

// ===== Definiciones de tipos y estructuras =====

typedef enum {
    TOKEN_EOF = 0,
    TOKEN_NUMBER,
    TOKEN_IDENTIFIER,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE,
    TOKEN_ASSIGN,
    TOKEN_SEMICOLON,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_WHILE,
    TOKEN_PRINT,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_EQUALS,
    TOKEN_NOT_EQUALS,
    TOKEN_GREATER,
    TOKEN_LESS,
    TOKEN_GREATER_EQUALS,
    TOKEN_LESS_EQUALS
} TokenType;

typedef struct {
    TokenType type;
    char* value;
} Token;

typedef struct {
    char* source;
    size_t position;
    size_t length;
} Lexer;

typedef struct {
    char* name;
    int value;
} Symbol;

// ===== Constantes y variables globales =====

#define MAX_SYMBOLS 100
#define MAX_TOKEN_VALUE 256

Symbol symbolTable[MAX_SYMBOLS];
size_t symbolCount = 0;

// ===== Prototipos de funciones =====

Lexer* initLexer(const char* source);
void freeLexer(Lexer* lexer);
char peek(const Lexer* lexer);
void advance(Lexer* lexer);
void skipWhitespace(Lexer* lexer);
Token* createToken(TokenType type, const char* value);
void freeToken(Token* token);
Token* readNumber(Lexer* lexer);
Token* readIdentifier(Lexer* lexer);
Token* getNextToken(Lexer* lexer);
void addSymbol(const char* name, int value);
int findSymbol(const char* name);
int evaluate(const Token* token, Lexer* lexer);
void compile(const char* source);
void error(const char* message);

// ===== Implementaciones de funciones =====

Lexer* initLexer(const char* source) {
    Lexer* lexer = (Lexer*)malloc(sizeof(Lexer));
    if (!lexer) error("No hay suficiente memoria para el lexer");
    
    lexer->source = strdup(source);
    if (!lexer->source) {
        free(lexer);
        error("No hay suficiente memoria para el código fuente");
    }
    
    lexer->position = 0;
    lexer->length = strlen(source);
    return lexer;
}

void freeLexer(Lexer* lexer) {
    if (lexer) {
        free(lexer->source);
        free(lexer);
    }
}

char peek(const Lexer* lexer) {
    return (lexer->position >= lexer->length) ? '\0' : lexer->source[lexer->position];
}

void advance(Lexer* lexer) {
    if (lexer->position < lexer->length) lexer->position++;
}

void skipWhitespace(Lexer* lexer) {
    while (isspace(peek(lexer))) advance(lexer);
}

Token* createToken(TokenType type, const char* value) {
    Token* token = (Token*)malloc(sizeof(Token));
    if (!token) error("No hay suficiente memoria para el token");
    
    token->type = type;
    token->value = value ? strdup(value) : NULL;
    if (value && !token->value) {
        free(token);
        error("No hay suficiente memoria para el valor del token");
    }
    
    return token;
}

void freeToken(Token* token) {
    if (token) {
        free(token->value);
        free(token);
    }
}

Token* readNumber(Lexer* lexer) {
    char buffer[MAX_TOKEN_VALUE];
    size_t i = 0;
    
    while (isdigit(peek(lexer)) && i < MAX_TOKEN_VALUE - 1) {
        buffer[i++] = peek(lexer);
        advance(lexer);
    }
    buffer[i] = '\0';
    
    return createToken(TOKEN_NUMBER, buffer);
}

Token* readIdentifier(Lexer* lexer) {
    char buffer[MAX_TOKEN_VALUE];
    size_t i = 0;
    
    while ((isalnum(peek(lexer)) || peek(lexer) == '_') && i < MAX_TOKEN_VALUE - 1) {
        buffer[i++] = peek(lexer);
        advance(lexer);
    }
    buffer[i] = '\0';
    
    if (strcmp(buffer, "if") == 0) return createToken(TOKEN_IF, buffer);
    if (strcmp(buffer, "else") == 0) return createToken(TOKEN_ELSE, buffer);
    if (strcmp(buffer, "while") == 0) return createToken(TOKEN_WHILE, buffer);
    if (strcmp(buffer, "print") == 0) return createToken(TOKEN_PRINT, buffer);
    
    return createToken(TOKEN_IDENTIFIER, buffer);
}

Token* getNextToken(Lexer* lexer) {
    skipWhitespace(lexer);
    
    if (lexer->position >= lexer->length) {
        return createToken(TOKEN_EOF, NULL);
    }
    
    char current = peek(lexer);
    
    if (isdigit(current)) return readNumber(lexer);
    if (isalpha(current) || current == '_') return readIdentifier(lexer);
    
    Token* token;
    advance(lexer);
    
    switch (current) {
        case '+': token = createToken(TOKEN_PLUS, "+"); break;
        case '-': token = createToken(TOKEN_MINUS, "-"); break;
        case '*': token = createToken(TOKEN_MULTIPLY, "*"); break;
        case '/': token = createToken(TOKEN_DIVIDE, "/"); break;
        case '=':
            if (peek(lexer) == '=') {
                advance(lexer);
                token = createToken(TOKEN_EQUALS, "==");
            } else {
                token = createToken(TOKEN_ASSIGN, "=");
            }
            break;
        case '!':
            if (peek(lexer) == '=') {
                advance(lexer);
                token = createToken(TOKEN_NOT_EQUALS, "!=");
            } else {
                error("Carácter inesperado después de '!'");
            }
            break;
        case '>':
            if (peek(lexer) == '=') {
                advance(lexer);
                token = createToken(TOKEN_GREATER_EQUALS, ">=");
            } else {
                token = createToken(TOKEN_GREATER, ">");
            }
            break;
        case '<':
            if (peek(lexer) == '=') {
                advance(lexer);
                token = createToken(TOKEN_LESS_EQUALS, "<=");
            } else {
                token = createToken(TOKEN_LESS, "<");
            }
            break;
        case '{': token = createToken(TOKEN_LBRACE, "{"); break;
        case '}': token = createToken(TOKEN_RBRACE, "}"); break;
        case '(': token = createToken(TOKEN_LPAREN, "("); break;
        case ')': token = createToken(TOKEN_RPAREN, ")"); break;
        case ';': token = createToken(TOKEN_SEMICOLON, ";"); break;
        default:
            error("Carácter no reconocido");
    }
    
    return token;
}

void addSymbol(const char* name, int value) {
    if (symbolCount >= MAX_SYMBOLS) {
        error("Tabla de símbolos llena");
    }
    
    for (size_t i = 0; i < symbolCount; i++) {
        if (strcmp(symbolTable[i].name, name) == 0) {
            symbolTable[i].value = value;
            return;
        }
    }
    
    symbolTable[symbolCount].name = strdup(name);
    if (!symbolTable[symbolCount].name) {
        error("No hay suficiente memoria para el símbolo");
    }
    symbolTable[symbolCount].value = value;
    symbolCount++;
}

int findSymbol(const char* name) {
    for (size_t i = 0; i < symbolCount; i++) {
        if (strcmp(symbolTable[i].name, name) == 0) {
            return symbolTable[i].value;
        }
    }
    char errorMsg[100];
    snprintf(errorMsg, sizeof(errorMsg), "Variable '%s' no definida", name);
    error(errorMsg);
    return 0; // Nunca se alcanza, pero evita advertencias del compilador
}

int evaluate(const Token* token, Lexer* lexer) {
    if (token->type == TOKEN_NUMBER) {
        return atoi(token->value);
    }
    
    if (token->type == TOKEN_IDENTIFIER) {
        return findSymbol(token->value);
    }
    
    error("Token inesperado en evaluación");
    return 0; // Nunca se alcanza, pero evita advertencias del compilador
}

void compile(const char* source) {
    Lexer* lexer = initLexer(source);
    Token* token;
    
    while ((token = getNextToken(lexer))->type != TOKEN_EOF) {
        switch (token->type) {
            case TOKEN_IDENTIFIER: {
                char* name = strdup(token->value);
                Token* next = getNextToken(lexer);
                
                if (next->type == TOKEN_ASSIGN) {
                    Token* value = getNextToken(lexer);
                    int result = evaluate(value, lexer);
                    addSymbol(name, result);
                    
                    Token* semicolon = getNextToken(lexer);
                    if (semicolon->type != TOKEN_SEMICOLON) {
                        error("Se esperaba ';'");
                    }
                    freeToken(semicolon);
                }
                free(name);
                freeToken(next);
                freeToken(value);
                break;
            }
            case TOKEN_PRINT: {
                Token* value = getNextToken(lexer);
                int result = evaluate(value, lexer);
                printf("%d\n", result);
                
                Token* semicolon = getNextToken(lexer);
                if (semicolon->type != TOKEN_SEMICOLON) {
                    error("Se esperaba ';'");
                }
                freeToken(semicolon);
                freeToken(value);
                break;
            }
            case TOKEN_IF: {
                Token* condition = getNextToken(lexer);
                int result = evaluate(condition, lexer);
                
                Token* lbrace = getNextToken(lexer);
                if (lbrace->type != TOKEN_LBRACE) {
                    error("Se esperaba '{'");
                }
                
                if (result) {
                    while ((token = getNextToken(lexer))->type != TOKEN_RBRACE) {
                        // Procesar tokens dentro del bloque
                        freeToken(token);
                    }
                } else {
                    int braceCount = 1;
                    while (braceCount > 0) {
                        token = getNextToken(lexer);
                        if (token->type == TOKEN_LBRACE) braceCount++;
                        if (token->type == TOKEN_RBRACE) braceCount--;
                        freeToken(token);
                    }
                }
                freeToken(condition);
                freeToken(lbrace);
                break;
            }
            case TOKEN_WHILE: {
                size_t startPos = lexer->position;
                Token* condition = getNextToken(lexer);
                int result = evaluate(condition, lexer);
                
                Token* lbrace = getNextToken(lexer);
                if (lbrace->type != TOKEN_LBRACE) {
                    error("Se esperaba '{'");
                }
                
                while (result) {
                    while ((token = getNextToken(lexer))->type != TOKEN_RBRACE) {
                        // Procesar tokens dentro del bloque
                        freeToken(token);
                    }
                    
                    lexer->position = startPos;
                    freeToken(condition);
                    condition = getNextToken(lexer);
                    result = evaluate(condition, lexer);
                }
                
                int braceCount = 1;
                while (braceCount > 0) {
                    token = getNextToken(lexer);
                    if (token->type == TOKEN_LBRACE) braceCount++;
                    if (token->type == TOKEN_RBRACE) braceCount--;
                    freeToken(token);
                }
                freeToken(condition);
                freeToken(lbrace);
                break;
            }
            default:
                freeToken(token);
        }
    }
    
    freeToken(token);
    freeLexer(lexer);
}

void error(const char* message) {
    fprintf(stderr, "Error: %s\n", message);
    exit(1);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Uso: cini <archivo_fuente>\n");
        printf("Ejemplo: cini programa.cini\n");
        return 1;
    }
    
    FILE* file = fopen(argv[1], "r");
    if (!file) {
        fprintf(stderr, "Error al abrir el archivo '%s': %s\n", argv[1], strerror(errno));
        return 1;
    }
    
    char* extension = strrchr(argv[1], '.');
    if (!extension || strcmp(extension, ".cini") != 0) {
        fprintf(stderr, "Error: El archivo debe tener extensión .cini\n");
        fclose(file);
        return 1;
    }
    
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    if (size == 0) {
        fprintf(stderr, "Error: El archivo está vacío\n");
        fclose(file);
        return 1;
    }
    fseek(file, 0, SEEK_SET);
    
    char* source = (char*)malloc(size + 1);
    if (!source) {
        fprintf(stderr, "Error: No hay suficiente memoria\n");
        fclose(file);
        return 1;
    }
    
    size_t read = fread(source, 1, size, file);
    if (read != size) {
        fprintf(stderr, "Error al leer el archivo: %s\n", strerror(errno));
        free(source);
        fclose(file);
        return 1;
    }
    source[size] = '\0';
    fclose(file);
    
    printf("Compilando %s...\n", argv[1]);
    compile(source);
    printf("Compilación exitosa.\n");
    
    free(source);
    return 0;
}
