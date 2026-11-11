# QuizNoSQL

## C++ NoSQL: Sistema de Carrito de Compras con Redis

Este proyecto presenta una implementación de un sistema básico de gestión de carritos de compra utilizando **C++** y **Redis** como sistema de almacenamiento NoSQL.

El objetivo principal es demostrar el uso de una base de datos **Key-Value** para administrar información temporal y de acceso frecuente, utilizando estructuras nativas de Redis como `Hash` y mecanismos de expiración mediante `TTL`.

El proyecto simula el flujo básico de un carrito de compras:

1. Conexión con el servidor Redis.
2. Creación y actualización del carrito de un usuario.
3. Almacenamiento de productos y cantidades.
4. Consulta del contenido del carrito.
5. Configuración de expiración automática.
6. Finalización de la compra.
7. Eliminación del carrito después del proceso de compra.

---

## 1. Contexto del problema

En una plataforma de comercio electrónico, los usuarios interactúan constantemente con sus carritos de compra. Entre las operaciones más frecuentes se encuentran:

- Agregar productos.
- Modificar cantidades.
- Consultar productos.
- Eliminar productos.
- Finalizar una compra.

Una parte importante de estos carritos puede ser abandonada sin completar una compra. Por esta razón, almacenar indefinidamente esta información puede generar datos temporales que ya no son necesarios.

En un sistema de alta concurrencia, utilizar exclusivamente una base de datos relacional para manejar todas las operaciones temporales del carrito puede incrementar la carga sobre el sistema principal.

Redis permite solucionar este problema mediante almacenamiento en memoria, estructuras de datos especializadas y mecanismos automáticos de expiración.

---

## 2. Justificación del uso de Redis

Redis es una base de datos NoSQL que utiliza principalmente un modelo de estructuras de datos en memoria. Para este proyecto resulta apropiado debido a las siguientes características.

### 2.1 Alto rendimiento

Redis mantiene los datos principalmente en memoria RAM, lo que permite realizar operaciones de lectura y escritura con una latencia muy baja.

Esto resulta conveniente para información que debe consultarse y modificarse frecuentemente, como los carritos de compra.

### 2.2 Estructura Hash

Redis proporciona estructuras `Hash` que permiten almacenar múltiples pares campo-valor asociados a una misma llave.

En este proyecto se utiliza la siguiente estructura:

```text
cart:user_88392
    |
    +-- prod_A_macbook          -> 1
    |
    +-- prod_B_mouse_logitech   -> 2
```

Donde:

- `cart:user_88392` representa el carrito del usuario.
- `prod_A_macbook` representa un producto.
- `prod_B_mouse_logitech` representa otro producto.
- Los valores `1` y `2` representan las cantidades.

### 2.3 Expiración automática

Redis permite establecer un tiempo de vida para una llave mediante `EXPIRE`.

En este proyecto se establece un tiempo de:

```text
86400 segundos = 24 horas
```

Por lo tanto, si el carrito permanece sin ser actualizado durante ese período, Redis puede eliminarlo automáticamente.

Esto evita tener que implementar un proceso externo exclusivamente para limpiar carritos abandonados.

---

## 3. Arquitectura del proyecto

La aplicación está compuesta principalmente por una clase denominada:

```cpp
ShoppingCartService
```

Esta clase encapsula la comunicación con Redis y proporciona las operaciones necesarias para administrar el carrito.

La arquitectura básica puede representarse de la siguiente manera:

```text
+-------------------------+
|       Aplicación C++    |
+------------+------------+
             |
             | Redis++
             v
+-------------------------+
|          Redis          |
|                         |
|  Hash: cart:userId      |
|                         |
|  productId -> quantity  |
+-------------------------+
```

La aplicación C++ utiliza la biblioteca **Redis++** para comunicarse con el servidor Redis.

---

## 4. Operaciones principales

### 4.1 Agregar un producto

La función:

```cpp
addItem()
```

permite agregar o actualizar un producto dentro del carrito.

Internamente utiliza:

```text
HSET
```

Ejemplo conceptual:

```text
HSET cart:user_88392 prod_A_macbook 1
```

Si el producto ya existe, su cantidad se actualiza.

Después de realizar la operación, se vuelve a establecer el tiempo de expiración del carrito:

```text
EXPIRE cart:user_88392 86400
```

---

### 4.2 Consultar el carrito

La función:

```cpp
viewCart()
```

recupera todos los elementos almacenados dentro del Hash.

Para ello utiliza:

```text
HGETALL
```

Ejemplo:

```text
HGETALL cart:user_88392
```

Los datos obtenidos desde Redis se almacenan temporalmente en un:

```cpp
std::unordered_map
```

para posteriormente mostrarlos en la consola.

---

### 4.3 Finalizar la compra

La función:

```cpp
checkout()
```

simula la finalización de una compra.

Una vez completada la operación, el carrito temporal deja de ser necesario y se elimina mediante:

```text
DEL
```

Ejemplo:

```text
DEL cart:user_88392
```

De esta manera, los datos temporales del carrito son eliminados de Redis.

---

## 5. Flujo de ejecución

El programa sigue el siguiente flujo:

```text
Inicio
  |
  v
Conectar con Redis
  |
  v
Crear ShoppingCartService
  |
  v
Agregar producto A
  |
  v
Agregar producto B
  |
  v
Consultar carrito
  |
  v
Realizar checkout
  |
  v
Eliminar carrito
  |
  v
Consultar nuevamente
  |
  v
Carrito vacío
  |
  v
Fin
```

---

## 6. Tecnologías utilizadas

| Tecnología | Uso |
|------------|-----|
| C++ | Lenguaje principal del proyecto |
| Redis | Base de datos NoSQL |
| Redis++ | Biblioteca para utilizar Redis desde C++ |
| Docker | Ejecución del servidor Redis |
| CMake | Sistema de construcción del proyecto |
| Conan | Administración de dependencias |

---

## 7. Requisitos

Para ejecutar el proyecto se requiere:

- Docker.
- Un compilador compatible con C++.
- CMake 3.15 o superior.
- Conan.
- Redis++.
- Una versión de C++ compatible con las características utilizadas en el código.

---

## 8. Instalación y ejecución

### 8.1 Clonar el repositorio

```bash
git clone <URL_DEL_REPOSITORIO>
```

Ingresar posteriormente al directorio:

```bash
cd QuizNoSQL
```

---

### 8.2 Ejecutar Redis mediante Docker

Para iniciar una instancia local de Redis:

```bash
docker run --name redis-nosql-demo -p 6379:6379 -d redis:alpine
```

Este comando realiza las siguientes acciones:

- Descarga la imagen de Redis si no se encuentra instalada.
- Crea un contenedor llamado `redis-nosql-demo`.
- Expone el puerto `6379`.
- Ejecuta Redis en segundo plano.

Para comprobar que el contenedor está ejecutándose:

```bash
docker ps
```

---

### 8.3 Compilar el proyecto

Si el proyecto utiliza CMake, se puede realizar una compilación mediante:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

El proceso puede variar dependiendo del compilador y del entorno de desarrollo utilizado.

---

### 8.4 Ejecutar el programa

Una vez compilado, se ejecuta el archivo generado por CMake.

Por ejemplo:

```bash
./QuizNoSQL
```

En Windows, el ejecutable puede encontrarse dentro de una carpeta como:

```text
build/Debug/
```

o:

```text
build/Release/
```

dependiendo de la configuración utilizada.

---

## 9. Ejemplo de ejecución

Al ejecutar el programa se espera una salida similar a:

```text
=== Iniciando Servicio de Carrito de Compras (NoSQL) ===

[INFO] Producto 'prod_A_macbook' (Cant: 1) guardado en el carrito de user_88392.

[INFO] Producto 'prod_B_mouse_logitech' (Cant: 2) guardado en el carrito de user_88392.


--- Carrito de user_88392 ---
  - Producto: prod_A_macbook | Cantidad: 1
  - Producto: prod_B_mouse_logitech | Cantidad: 2
----------------------------


[SUCCESS] Checkout completado para user_88392.
El carrito efímero
