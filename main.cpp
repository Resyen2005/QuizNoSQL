#include <iostream>              // Permite utilizar cout y cerr para mostrar mensajes
#include <string>                // Permite trabajar con variables de tipo string
#include <unordered_map>         // Permite utilizar mapas clave-valor en C++
#include <iterator>              // Permite utilizar std::inserter

// Librería de Redis para C++
// Redis++ permite conectarnos y trabajar con un servidor Redis
#include <sw/redis++/redis++.h>

// Importamos el espacio de nombres de Redis++
// Esto permite escribir Redis, Error, etc., sin colocar sw::redis::
using namespace sw::redis;


/**
 * @class ShoppingCartService
 *
 * @brief
 * Clase encargada de administrar los carritos de compra
 * utilizando Redis como base de datos NoSQL.
 *
 * En Redis utilizaremos una estructura HASH.
 *
 * Ejemplo:
 *
 * cart:user_88392
 *      |
 *      |---- prod_A_macbook          -> 1
 *      |
 *      |---- prod_B_mouse_logitech   -> 2
 *
 * La llave principal representa el carrito del usuario.
 * Cada producto dentro del Hash almacena su respectiva cantidad.
 */
class ShoppingCartService {

private:

    /*
     * Objeto que representa nuestra conexión con Redis.
     *
     * A través de este objeto podemos ejecutar operaciones
     * como:
     *
     * - HSET
     * - HGETALL
     * - EXPIRE
     * - DEL
     *
     * sobre el servidor Redis.
     */
    Redis redis;


public:

    /**
     * @brief Constructor de la clase ShoppingCartService.
     *
     * Recibe la dirección donde se encuentra ejecutándose
     * el servidor Redis.
     *
     * Ejemplo:
     *
     * tcp://127.0.0.1:6379
     *
     * 127.0.0.1 -> nuestra propia computadora
     * 6379      -> puerto utilizado normalmente por Redis
     *
     * @param connection_uri Dirección de conexión al servidor Redis.
     */
    explicit ShoppingCartService(const std::string& connection_uri)
        : redis(connection_uri)
    {
        // La conexión con Redis se inicializa automáticamente
        // mediante el constructor del objeto Redis.
    }


    /**
     * @brief Añade o actualiza un producto dentro del carrito.
     *
     * Esta función utiliza HSET de Redis.
     *
     * La información se almacena de la siguiente manera:
     *
     * cart:user_88392
     *
     * producto                  cantidad
     * ----------------------------------
     * prod_A_macbook            1
     * prod_B_mouse_logitech     2
     *
     * Si el producto ya existe, HSET actualiza su cantidad.
     *
     * @param userId Identificador del usuario.
     * @param productId Identificador del producto.
     * @param quantity Cantidad del producto.
     */
    void addItem(
        const std::string& userId,
        const std::string& productId,
        int quantity
    )
    {

        /*
         * Construimos la llave que identificará
         * el carrito del usuario.
         *
         * Por ejemplo:
         *
         * userId = "user_88392"
         *
         * cartKey será:
         *
         * "cart:user_88392"
         */
        std::string cartKey = "cart:" + userId;


        /*
         * HSET permite guardar información dentro de un HASH.
         *
         * En este caso:
         *
         * llave principal:
         * cart:user_88392
         *
         * campo:
         * prod_A_macbook
         *
         * valor:
         * 1
         *
         * Conceptualmente sería:
         *
         * HSET cart:user_88392 prod_A_macbook 1
         *
         * Redis se encarga de almacenar esta información.
         */
        redis.hset(
            cartKey,
            productId,
            std::to_string(quantity)
        );


        /*
         * Configuramos un tiempo de expiración para el carrito.
         *
         * 86400 segundos = 24 horas.
         *
         * Esto significa que si el carrito permanece
         * sin ser actualizado durante 24 horas,
         * Redis eliminará automáticamente la llave.
         *
         * Esto es útil porque los carritos son información
         * temporal y no necesitamos conservarlos para siempre.
         */
        redis.expire(cartKey, 86400);


        /*
         * Mostramos un mensaje indicando que el producto
         * fue almacenado correctamente.
         */
        std::cout
            << "[INFO] Producto '" << productId
            << "' (Cant: " << quantity
            << ") guardado en el carrito de "
            << userId
            << ".\n";
    }


    /**
     * @brief Muestra todos los productos que existen
     *        actualmente en el carrito.
     *
     * Utiliza HGETALL de Redis para recuperar
     * todos los campos y valores del Hash.
     *
     * @param userId Identificador del usuario.
     */
    void viewCart(const std::string& userId)
    {

        /*
         * Construimos nuevamente la llave
         * correspondiente al carrito.
         *
         * Ejemplo:
         *
         * cart:user_88392
         */
        std::string cartKey = "cart:" + userId;


        /*
         * Creamos un unordered_map de C++ para almacenar
         * temporalmente los productos recuperados de Redis.
         *
         * La estructura será:
         *
         * producto -> cantidad
         *
         * Por ejemplo:
         *
         * prod_A_macbook -> 1
         * prod_B_mouse_logitech -> 2
         */
        std::unordered_map<std::string, std::string> cart;


        /*
         * HGETALL recupera todos los campos y valores
         * almacenados dentro del Hash.
         *
         * Conceptualmente estamos ejecutando:
         *
         * HGETALL cart:user_88392
         *
         * Los datos obtenidos desde Redis se insertan
         * directamente dentro del unordered_map "cart".
         *
         * std::inserter permite insertar automáticamente
         * los elementos recuperados dentro del mapa.
         */
        redis.hgetall(
            cartKey,
            std::inserter(cart, cart.begin())
        );


        /*
         * Mostramos el encabezado del carrito.
         */
        std::cout
            << "\n--- Carrito de "
            << userId
            << " ---\n";


        /*
         * Verificamos si el mapa está vacío.
         *
         * Esto puede ocurrir por dos razones:
         *
         * 1. El usuario nunca agregó productos.
         *
         * 2. El carrito expiró después de 24 horas.
         *
         * 3. El usuario realizó un checkout y el carrito
         *    fue eliminado.
         */
        if (cart.empty())
        {

            std::cout
                << "  (El carrito está vacío o ha expirado)\n";
        }
        else
        {

            /*
             * Recorremos todos los productos encontrados
             * dentro del carrito.
             *
             * "product" representa el identificador
             * del producto.
             *
             * "quantity" representa la cantidad.
             */
            for (const auto& [product, quantity] : cart)
            {

                /*
                 * Mostramos cada producto y su cantidad.
                 */
                std::cout
                    << "  - Producto: "
                    << product
                    << " | Cantidad: "
                    << quantity
                    << "\n";
            }
        }


        /*
         * Línea final para hacer más clara
         * la visualización del carrito.
         */
        std::cout
            << "----------------------------\n\n";
    }


    /**
     * @brief Finaliza la compra del usuario.
     *
     * Cuando el usuario realiza el checkout,
     * el carrito deja de ser necesario.
     *
     * Por esta razón eliminamos completamente
     * la llave del carrito utilizando DEL.
     *
     * @param userId Identificador del usuario.
     */
    void checkout(const std::string& userId)
    {

        /*
         * Construimos la llave del carrito.
         *
         * Ejemplo:
         *
         * cart:user_88392
         */
        std::string cartKey = "cart:" + userId;


        /*
         * DEL elimina completamente la llave
         * y todos los datos almacenados dentro de ella.
         *
         * Conceptualmente:
         *
         * DEL cart:user_88392
         *
         * Después de ejecutar esta instrucción,
         * el carrito ya no existe en Redis.
         */
        redis.del(cartKey);


        /*
         * Mostramos un mensaje indicando que
         * el checkout fue completado.
         */
        std::cout
            << "[SUCCESS] Checkout completado para "
            << userId
            << ". El carrito efímero ha sido destruido.\n";
    }
};


/**
 * @brief Función principal del programa.
 *
 * Aquí se prueba el funcionamiento completo
 * del servicio de carrito:
 *
 * 1. Conectarse a Redis.
 * 2. Crear un carrito.
 * 3. Agregar productos.
 * 4. Consultar el carrito.
 * 5. Realizar checkout.
 * 6. Comprobar que el carrito fue eliminado.
 */
int main()
{

    /*
     * try permite ejecutar el código que puede producir
     * una excepción.
     *
     * En este caso, las operaciones de Redis pueden
     * generar errores si el servidor no está disponible.
     */
    try
    {

        /*
         * Creamos un objeto ShoppingCartService.
         *
         * Le indicamos que Redis está ejecutándose
         * en nuestra propia computadora.
         *
         * 127.0.0.1 = localhost
         * 6379      = puerto predeterminado de Redis
         */
        ShoppingCartService cartService(
            "tcp://127.0.0.1:6379"
        );


        /*
         * Identificador del usuario que vamos a utilizar
         * para realizar la simulación.
         */
        std::string userId = "user_88392";


        /*
         * Mostramos un título en la consola.
         */
        std::cout
            << "=== Iniciando Servicio de Carrito de Compras (NoSQL) ===\n\n";


        // ============================================================
        // 1. AGREGAR PRODUCTOS AL CARRITO
        // ============================================================

        /*
         * Agregamos una MacBook al carrito.
         *
         * Producto:
         * prod_A_macbook
         *
         * Cantidad:
         * 1
         */
        cartService.addItem(
            userId,
            "prod_A_macbook",
            1
        );


        /*
         * Agregamos dos mouse Logitech.
         *
         * Producto:
         * prod_B_mouse_logitech
         *
         * Cantidad:
         * 2
         */
        cartService.addItem(
            userId,
            "prod_B_mouse_logitech",
            2
        );


        // ============================================================
        // 2. CONSULTAR EL CARRITO
        // ============================================================

        /*
         * Recuperamos y mostramos todos los productos
         * almacenados actualmente en Redis.
         */
        cartService.viewCart(userId);


        // ============================================================
        // 3. REALIZAR CHECKOUT
        // ============================================================

        /*
         * El usuario confirma la compra.
         *
         * Esto provoca que Redis elimine completamente
         * el carrito temporal.
         */
        cartService.checkout(userId);


        // ============================================================
        // 4. COMPROBAR QUE EL CARRITO FUE ELIMINADO
        // ============================================================

        /*
         * Intentamos consultar nuevamente el carrito.
         *
         * Como checkout() eliminó la llave,
         * el carrito debería aparecer vacío.
         */
        cartService.viewCart(userId);

    }


    /*
     * Capturamos cualquier error producido por Redis.
     *
     * Error es una excepción proporcionada por Redis++.
     */
    catch (const Error& e)
    {

        /*
         * Mostramos el mensaje de error proporcionado
         * por Redis.
         */
        std::cerr
            << "[ERROR DE CONEXIÓN] No se pudo conectar a Redis: "
            << e.what()
            << "\n";


        /*
         * Mostramos una recomendación para el usuario.
         */
        std::cerr
            << "Asegúrate de que el contenedor de Docker esté corriendo.\n";


        /*
         * Retornamos 1 para indicar que el programa
         * terminó debido a un error.
         */
        return 1;
    }


    /*
     * Retornamos 0 para indicar que el programa
     * terminó correctamente.
     */
    return 0;
}