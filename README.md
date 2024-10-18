# SwiftC - Guía de Usuario

SwiftC es un lenguaje de programación simple inspirado en C, diseñado con fines educativos. Esta guía te enseñará todo lo que necesitas saber para comenzar a programar en SwiftC.

## Índice

1. [Instalación](#instalación)
2. [Conceptos Básicos](#conceptos-básicos)
3. [Variables](#variables)
4. [Operaciones Aritméticas](#operaciones-aritméticas)
5. [Estructuras de Control](#estructuras-de-control)
6. [Impresión](#impresión)
7. [Ejemplos Completos](#ejemplos-completos)
8. [Consejos y Mejores Prácticas](#consejos-y-mejores-prácticas)
9. [Solución de Problemas](#solución-de-problemas)

## Instalación

1. Clona este repositorio:
   ```
   git clone https://github.com/tu-usuario/swiftc.git
   ```
2. Compila el compilador de SwiftC:
   ```
   gcc -o swiftc swiftc.c
   ```
3. Asegúrate de que el compilador esté en tu PATH o usa la ruta completa al ejecutarlo.

## Conceptos Básicos

- Los programas SwiftC se escriben en archivos con extensión `.swc`
- Cada instrucción debe terminar con un punto y coma (`;`)
- Los bloques de código se encierran entre llaves (`{}`)
- SwiftC es sensible a mayúsculas y minúsculas

## Variables

En SwiftC, todas las variables son enteros. No es necesario declararlas antes de usarlas.

```
x = 5;
contador = 0;
resultado = x + contador;
```

## Operaciones Aritméticas

SwiftC soporta las operaciones aritméticas básicas:

- Suma: `+`
- Resta: `-`
- Multiplicación: `*`
- División: `/`

Ejemplo:
```
a = 10;
b = 3;
suma = a + b;
diferencia = a - b;
producto = a * b;
cociente = a / b;
```

## Estructuras de Control

### If-Else

```
if condición {
    // código si la condición es verdadera
} else {
    // código si la condición es falsa
}
```

Operadores de comparación:
- Igual que: `==`
- Diferente que: `!=`
- Mayor que: `>`
- Menor que: `<`
- Mayor o igual que: `>=`
- Menor o igual que: `<=`

### While

```
while condición {
    // código que se repite mientras la condición sea verdadera
}
```

## Impresión

Para imprimir valores, usa la función `print`:

```
print x;
print x + y;
```

## Ejemplos Completos

### Ejemplo 1: Contador

```
contador = 0;
while contador < 5 {
    print contador;
    contador = contador + 1;
}
```

### Ejemplo 2: Par o Impar

```
numero = 7;
if numero / 2 * 2 == numero {
    print 0;  // 0 significa par
} else {
    print 1;  // 1 significa impar
}
```

### Ejemplo 3: Suma de Números

```
suma = 0;
i = 1;
while i <= 10 {
    suma = suma + i;
    i = i + 1;
}

print suma;
```

## Consejos y Mejores Prácticas

1. Usa nombres descriptivos para tus variables.
2. Comenta tu código para explicar partes complejas.
3. Divide problemas grandes en partes más pequeñas y manejables.
4. Prueba tu código con diferentes entradas para asegurarte de que funciona correctamente.

## Solución de Problemas

- Si obtienes un error de "variable no definida", asegúrate de haber asignado un valor a la variable antes de usarla.
- Si tu programa se queda en un bucle infinito, verifica la condición de tu bucle `while`.
- Si obtienes resultados inesperados en operaciones aritméticas, recuerda que SwiftC solo maneja enteros, por lo que la división redondeará hacia abajo.

---

¡Feliz codificación con SwiftC! Si tienes más preguntas o necesitas ayuda, no dudes en abrir un issue en este repositorio.
