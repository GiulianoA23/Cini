#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

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
    if (!lexer) {
        printf("Error: No hay suficiente memoria para el lexer\n");
        exit(1);
    }
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
    
    while (isdigit(peek(lexer)) && i < 255) {
        buffer[i++] = peek(lexer);
        advance(lexer);
    }
    buffer[i] = '\0';
    
    Token* token = (Token*)malloc(sizeof(Token));
    if (!token) {
        printf("Error: No hay suficiente memoria para el token\n");
        exit(1);
    }
    token->type = TOKEN_NUMBER;
    token->value = strdup(buffer);
    return token;
}

// Leer identificador o palabra clave
Token* readIdentifier(Lexer* lexer) {
    char buffer[256];
    int i = 0;
    
    while ((isalnum(peek(lexer)) || peek(lexer) == '_') && i < 255) {
        buffer[i++] = peek(lexer);
        advance(lexer);
    }
    buffer[i] = '\0';
    
    Token* token = (Token*)malloc(sizeof(Token));
    if (!token) {
        printf("Error: No hay suficiente memoria para el token\n");
        exit(1);
    }
    
    // Palabras clave
    if (strcmp(buffer, "if") == 0) {
        token->type = TOKEN_IF;
    } else if (strcmp(buffer, "else") == 0) {
        token->type = TOKEN_ELSE;
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

// Verificar operador de dos caracteres
int isDoubleOperator(Lexer* lexer, char first, char second) {
    if (peek(lexer) == second) {
        advance(lexer);
        return 1;
    }
    return 0;
}

// Obtener siguiente token
Token* getNextToken(Lexer* lexer) {
    skipWhitespace(lexer);
    
    if (lexer->position >= lexer->length) {
        Token* token = (Token*)malloc(sizeof(Token));
        if (!token) {
            printf("Error: No hay suficiente memoria para el token\n");
            exit(1);
        }
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
    if (!token) {
        printf("Error: No hay suficiente memoria para el token\n");
        exit(1);
    }
    token->value = (char*)malloc(2);
    if (!token->value) {
        free(token);
        printf("Error: No hay suficiente memoria para el valor del token\n");
        exit(1);
    }
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
            advance(lexer);
            if (peek(lexer) == '=') {
                token->type = TOKEN_EQUALS;
                advance(lexer);
            } else {
                token->type = TOKEN_ASSIGN;
            }
            break;
        case '!':
            advance(lexer);
            if (peek(lexer) == '=') {
                token->type = TOKEN_NOT_EQUALS;
                advance(lexer);
            } else {
                printf("Error: Carácter inesperado después de '!'\n");
                exit(1);
            }
            break;
        case '>':
            advance(lexer);
            if (peek(lexer) == '=') {
                token->type = TOKEN_GREATER_EQUALS;
                advance(lexer);
            } else {
                token->type = TOKEN_GREATER;
            }
            break;
        case '<':
            advance(lexer);
            if (peek(lexer) == '=') {
                token->type = TOKEN_LESS_EQUALS;
                advance(lexer);
            } else {
                token->type = TOKEN_LESS;
            }
            break;
        case '{':
            token->type = TOKEN_LBRACE;
            break;
        case '}':
            token->type = TOKEN_RBRACE;
            break;
        case '(':
            token->type = TOKEN_LPAREN;
            break;
        case ')':
            token->type = TOKEN_RPAREN;
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
    
    // Verificar si el símbolo ya existe
    for (int i = 0; i < symbolCount; i++) {
        if (strcmp(symbolTable[i].name, name) == 0) {
            symbolTable[i].value = value;
            return;
        }
    }
    
    symbolTable[symbolCount].name = strdup(name);
    if (!symbolTable[symbolCount].name) {
        printf("Error: No hay suficiente memoria para el símbolo\n");
        exit(1);
    }
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

// Evaluador de expresiones
int evaluateExpression(Lexer* lexer) {
    Token* token = getNextToken(lexer);
    int result = evaluate(token, lexer);
    
    Token* nextToken = getNextToken(lexer);
    while (nextToken->type == TOKEN_PLUS || nextToken->type == TOKEN_MINUS || 
           nextToken->type == TOKEN_MULTIPLY || nextToken->type == TOKEN_DIVIDE) {
        Token* nextValue = getNextToken(lexer);
        int value = evaluate(nextValue, lexer);
        
        switch (nextToken->type) {
            case TOKEN_PLUS:
                result += value;
                break;
            case TOKEN_MINUS:
                result -= value;
                break;
            case TOKEN_MULTIPLY:
                result *= value;
                break;
            case TOKEN_DIVIDE:
                if (value == 0) {
                    printf("Error: División por cero\n");
                    exit(1);
                }
                result /= value;
                break;
            default:
                break;
        }
        free(nextValue->value);
        free(nextValue);
        nextToken = getNextToken(lexer);
    }
    
    return result;
}

// Compilador principal
void compile(char* source) {
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
                    
                    // Consumir punto y coma
                    Token* semicolon = getNextToken(lexer);
                    if (semicolon->type != TOKEN_SEMICOLON) {
                        printf("Error: Se esperaba ';'\n");
                        exit(1);
                    }
                }
                free(name);
                break;
            }
            case TOKEN_PRINT: {
                Token* value = getNextToken(lexer);
                int result = evaluate(value, lexer);
                printf("%d\n", result);
                
                // Consumir punto y coma
                Token* semicolon = getNextToken(lexer);
                if (semicolon->type != TOKEN_SEMICOLON) {
                    printf("Error: Se esperaba ';'\n");
                    exit(1);
                }
                break;
            }
            case TOKEN_IF: {
                Token* condition = getNextToken(lexer);
                int result = evaluate(condition, lexer);
                
                Token* lbrace = getNextToken(lexer);
                if (lbrace->type != TOKEN_LBRACE) {
                    printf("Error: Se esperaba '{'\n");
                    exit(1);
                }
                
                if (result) {
                    // Ejecutar bloque if
                    while ((token = getNextToken(lexer))->type != TOKEN_RBRACE) {
                        free(token->value);
                        free(token);
                    }
                } else {
                    // Saltar bloque if
                    int braceCount = 1;
                    while (braceCount > 0) {
                        token = getNextToken(lexer);
                        if (token->type == TOKEN_LBRACE) braceCount++;
                        if (token->type == TOKEN_RBRACE) braceCount--;
                    }
                }
                break;
            }
            case TOKEN_WHILE: {
                int startPos = lexer->position;
                Token* condition = getNextToken(lexer);
                int result = evaluate(condition, lexer);
                
                Token* lbrace = getNextToken(lexer);
                if (lbrace->type != TOKEN_LBRACE) {
                    printf("Error: Se esperaba '{'\n");
                    exit(1);
                }
                
                while (result) {
                    // Ejecutar bloque while
                    while ((token = getNextToken(lexer))->type != TOKEN_RBRACE) {
                        // Procesar tokens dentro del bloque
                    }
                    
                    // Volver al inicio del while
                    lexer->position = startPos;
                    condition = getNextToken(lexer);
                    result = evaluate(condition, lexer);
                }
                
                // Saltar bloque while cuando la condición es falsa
                int braceCount = 1;
                while (braceCount > 0) {
                    token = getNextToken(lexer);
                    if (token->type == TOKEN_LBRACE) braceCount++;
                    if (token->type == TOKEN_RBRACE) braceCount--;
                }
                break;
            }
        }
    }
    
    free(lexer);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Uso: cinic <archivo_fuente>\n");
        printf("Ejemplo: cinic programa.cini\n");
        return 1;
    }
    
    // Leer archivo fuente con mejor manejo de errores
    FILE* file = fopen(argv[1], "r");
    if (!file) {
        printf("Error al abrir el archivo '%s': %s\n", argv[1], strerror(errno));
        return 1;
    }
    
    // Verificar extensión del archivo
    char* extension = strrchr(argv[1], '.');
    if (!extension || strcmp(extension, ".cini") != 0) {
        printf("Error: El archivo debe tener extensión .cini\n");
        fclose(file);
        return 1;
    }

    
    // Obtener tamaño del archivo
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
	if (size < 0) {
    	printf("Error al determinar el tamaño del archivo: %s\n", strerror(errno));
    	fclose(file);
	    return 1;
	}

    fseek(file, 0, SEEK_SET);
    
    // Leer contenido con verificación
    char* source = (char*)malloc(size + 1);
    if (!source) {
        printf("Error: No hay suficiente memoria\n");
        fclose(file);
        return 1;
    }
    
    size_t read = fread(source, 1, size, file);
    if (read != size) {
        printf("Error al leer el archivo: %s\n", strerror(errno));
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
