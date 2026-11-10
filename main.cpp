#include <iostream>
#include <string>
#include <unordered_map>
#include <sw/redis++/redis++.h>

using namespace sw::redis;

/**
 * @class ShoppingCartService
 * @brief Maneja la lógica de negocio de los carritos de compra usando Redis (Key-Value).
 */
class ShoppingCartService {
private:
    Redis redis;

public:
    // Constructor: Inicializa la conexión a la base de datos NoSQL
    explicit ShoppingCartService(const std::string& connection_uri) : redis(connection_uri) {}

    /**
     * @brief Añade o actualiza un producto en el carrito de un usuario.
     * Utiliza la estructura Hash (HSET) de Redis para agrupar los ítems del usuario.
     */
    void addItem(const std::string& userId, const std::string& productId, int quantity) {
        std::string cartKey = "cart:" + userId;
        
        // HSET guarda el par [productId -> quantity] dentro de la llave principal [cart:userId]
        redis.hset(cartKey, productId, std::to_string(quantity));
        
        // Refresca el TTL (Time-To-Live). El carrito expira en 24 horas de inactividad.
        redis.expire(cartKey, 86400); 
        
        std::cout << "[INFO] Producto '" << productId << "' (Cant: " << quantity 
                  << ") guardado en el carrito de " << userId << ".\n";
    }

    /**
     * @brief Muestra todos los ítems actuales del carrito (HGETALL).
     */
    void viewCart(const std::string& userId) {
        std::string cartKey = "cart:" + userId;
        std::unordered_map<std::string, std::string> cart;
        
        // Recupera todo el Hash y lo inserta en un mapa de C++
        redis.hgetall(cartKey, std::inserter(cart, cart.begin()));

        std::cout << "\n--- Carrito de " << userId << " ---\n";
        if (cart.empty()) {
            std::cout << "  (El carrito está vacío o ha expirado)\n";
        } else {
            for (const auto& [product, quantity] : cart) {
                std::cout << "  - Producto: " << product << " | Cantidad: " << quantity << "\n";
            }
        }
        std::cout << "----------------------------\n\n";
    }

    /**
     * @brief Finaliza la compra y elimina el estado efímero de Redis (DEL).
     */
    void checkout(const std::string& userId) {
        std::string cartKey = "cart:" + userId;
        redis.del(cartKey);
        std::cout << "[SUCCESS] Checkout completado para " << userId 
                  << ". El carrito efímero ha sido destruido.\n";
    }
};

int main() {
    try {
        // Conexión al servidor local de Redis
        ShoppingCartService cartService("tcp://127.0.0.1:6379");
        std::string userId = "user_88392";

        std::cout << "=== Iniciando Servicio de Carrito de Compras (NoSQL) ===\n\n";

        // 1. El usuario añade productos (Simulación de concurrencia rápida)
        cartService.addItem(userId, "prod_A_macbook", 1);
        cartService.addItem(userId, "prod_B_mouse_logitech", 2);

        // 2. El usuario revisa su carrito
        cartService.viewCart(userId);

        // 3. El usuario completa su compra
        cartService.checkout(userId);

        // 4. Verificación de que los datos efímeros ya no existen
        cartService.viewCart(userId);

    } catch (const Error &e) {
        std::cerr << "[ERROR DE CONEXIÓN] No se pudo conectar a Redis: " << e.what() << "\n";
        std::cerr << "Asegúrate de que el contenedor de Docker esté corriendo.\n";
        return 1;
    }

    return 0;
}