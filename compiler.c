// SwiftC - Un lenguaje de programación simple y optimizado
// Compilador implementado en C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Definición de tipos de tokens
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
    TOKEN_WHILE,
    TOKEN_PRINT
} TokenType;

// Estructura para tokens
typedef struct {
    TokenType type;
    char* value;
} Token;

// Estructura para el lexer
typedef struct {
    char* source;
    int position;
    int length;
} Lexer;

// Estructura para símbolos
typedef struct {
    char* name;
    int value;
} Symbol;

// Tabla de símbolos
#define MAX_SYMBOLS 100
Symbol symbolTable[MAX_SYMBOLS];
int symbolCount = 0;

// Inicializar lexer
Lexer* initLexer(char* source) {
    Lexer* lexer = (Lexer*)malloc(sizeof(Lexer));
    lexer->source = source;
    lexer->position = 0;
    lexer->length = strlen(source);
    return lexer;
}

// Obtener siguiente carácter
char peek(Lexer* lexer) {
    if (lexer->position >= lexer->length) {
        return '\0';
    }
    return lexer->source[lexer->position];
}

// Avanzar al siguiente carácter
void advance(Lexer* lexer) {
    lexer->position++;
}

// Saltar espacios en blanco
void skipWhitespace(Lexer* lexer) {
    while (isspace(peek(lexer))) {
        advance(lexer);
    }
}

// Leer número
Token* readNumber(Lexer* lexer) {
    char buffer[256];
    int i = 0;
    
    while (isdigit(peek(lexer))) {
        buffer[i++] = peek(lexer);
        advance(lexer);
    }
    buffer[i] = '\0';
    
    Token* token = (Token*)malloc(sizeof(Token));
    token->type = TOKEN_NUMBER;
    token->value = strdup(buffer);
    return token;
}

// Leer identificador
Token* readIdentifier(Lexer* lexer) {
    char buffer[256];
    int i = 0;
    
    while (isalnum(peek(lexer)) || peek(lexer) == '_') {
        buffer[i++] = peek(lexer);
        advance(lexer);
    }
    buffer[i] = '\0';
    
    Token* token = (Token*)malloc(sizeof(Token));
    
    // Palabras clave
    if (strcmp(buffer, "if") == 0) {
        token->type = TOKEN_IF;
    } else if (strcmp(buffer, "while") == 0) {
        token->type = TOKEN_WHILE;
    } else if (strcmp(buffer, "print") == 0) {
        token->type = TOKEN_PRINT;
    } else {
        token->type = TOKEN_IDENTIFIER;
    }
    
    token->value = strdup(buffer);
    return token;
}

// Obtener siguiente token
Token* getNextToken(Lexer* lexer) {
    skipWhitespace(lexer);
    
    if (lexer->position >= lexer->length) {
        Token* token = (Token*)malloc(sizeof(Token));
        token->type = TOKEN_EOF;
        token->value = NULL;
        return token;
    }
    
    char current = peek(lexer);
    
    if (isdigit(current)) {
        return readNumber(lexer);
    }
    
    if (isalpha(current) || current == '_') {
        return readIdentifier(lexer);
    }
    
    Token* token = (Token*)malloc(sizeof(Token));
    token->value = (char*)malloc(2);
    token->value[0] = current;
    token->value[1] = '\0';
    
    switch (current) {
        case '+':
            token->type = TOKEN_PLUS;
            break;
        case '-':
            token->type = TOKEN_MINUS;
            break;
        case '*':
            token->type = TOKEN_MULTIPLY;
            break;
        case '/':
            token->type = TOKEN_DIVIDE;
            break;
        case '=':
            token->type = TOKEN_ASSIGN;
            break;
        case ';':
            token->type = TOKEN_SEMICOLON;
            break;
        default:
            printf("Error: Carácter no reconocido '%c'\n", current);
            exit(1);
    }
    
    advance(lexer);
    return token;
}

// Agregar símbolo a la tabla
void addSymbol(char* name, int value) {
    if (symbolCount >= MAX_SYMBOLS) {
        printf("Error: Tabla de símbolos llena\n");
        exit(1);
    }
    
    symbolTable[symbolCount].name = strdup(name);
    symbolTable[symbolCount].value = value;
    symbolCount++;
}

// Buscar símbolo en la tabla
int findSymbol(char* name) {
    for (int i = 0; i < symbolCount; i++) {
        if (strcmp(symbolTable[i].name, name) == 0) {
            return symbolTable[i].value;
        }
    }
    printf("Error: Variable '%s' no definida\n", name);
    exit(1);
}

// Compilador principal
void compile(char* source) {
    Lexer* lexer = initLexer(source);
    Token* token;
    
    while ((token = getNextToken(lexer))->type != TOKEN_EOF) {
        switch (token->type) {
            case TOKEN_IDENTIFIER:
                // Manejo de identificadores
                printf("Identificador: %s\n", token->value);
                break;
            case TOKEN_NUMBER:
                // Manejo de números
                printf("Número: %s\n", token->value);
                break;
            case TOKEN_PRINT:
                // Manejo de la instrucción print
                printf("Instrucción print\n");
                break;
            // Agregar más casos según sea necesario
            default:
                printf("Token tipo: %d, valor: %s\n", token->type, token->value);
        }
        
        free(token->value);
        free(token);
    }
    
    free(token);
    free(lexer);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Uso: %s <archivo_fuente>\n", argv[0]);
        return 1;
    }
    
    // Leer archivo fuente
    FILE* file = fopen(argv[1], "r");
    if (!file) {
        printf("Error: No se pudo abrir el archivo\n");
        return 1;
    }
    
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* source = (char*)malloc(size + 1);
    fread(source, 1, size, file);
    source[size] = '\0';
    
    fclose(file);
    
    // Compilar el código
    compile(source);
    
    free(source);
    return 0;
}
