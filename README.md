# QuizNoSQL
# NoSQL C++: E-Commerce Shopping Cart Cache 🛒⚡

Este repositorio contiene una demostración práctica de arquitectura de software para resolver un problema común en sistemas de alta concurrencia: **la gestión de carritos de compras efímeros**. 

El proyecto está escrito en **C++** y utiliza **Redis** (un motor de base de datos NoSQL de estilo Llave-Valor) como capa de persistencia en memoria.

## 📖 Contexto del Problema

En las plataformas de e-commerce, los usuarios interactúan con sus carritos de compra constantemente (agregando, quitando o modificando cantidades). La enorme mayoría de estos carritos **son abandonados**. 

Si utilizáramos una base de datos relacional tradicional (como PostgreSQL o MySQL) para registrar cada clic en el carrito, generaríamos un cuello de botella debido a la carga en disco (I/O) y el bloqueo de tablas, desgastando la base de datos con datos que probablemente no terminen en una venta.

## 🧠 ¿Por qué elegimos un modelo NoSQL Llave-Valor (Redis)?

Para este caso de uso específico, una base de datos Key-Value es la solución arquitectónica óptima por tres ventajas estructurales:

1. **Rendimiento O(1) en Memoria:** Las bases Key-Value acceden a los datos de manera directa a través de su llave. Al operar 100% en memoria RAM, las latencias de lectura/escritura son de microsegundos.
2. **Estructuras Nativas Ideales:** Redis soporta `Hashes`, lo que nos permite modelar la relación perfectamente: `Llave Principal (cart:userId) -> Campos (productId) -> Valores (quantity)`.
3. **Manejo de Ciclo de Vida (TTL):** El modelo Key-Value en Redis permite asignar un "Time To Live" a nivel de llave. En este código, si el usuario no interactúa en 24 horas, la base de datos elimina el registro automáticamente, ahorrándonos el desarrollo de "cronjobs" de limpieza.

## 🚀 Instrucciones de Ejecución

### Prerrequisitos
- [Docker](https://www.docker.com/) (Para levantar la base de datos localmente)
- Compilador C++ (GCC/Clang/MSVC) compatible con C++
- CMake (>= 3.15)
- [Conan C++ Package Manager](https://conan.io/)

### Paso 1: Levantar la Base de Datos NoSQL
Levantaremos una instancia de Redis usando Docker en el puerto por defecto (6379).
```bash
docker run --name redis-nosql-demo -p 6379:6379 -d redis:alpine
