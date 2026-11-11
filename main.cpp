#include <sw/redis++/redis++.h>

using namespace sw::redis;

/**
 * @class ShoppingCartService
 */
class ShoppingCartService {
private:
    Redis redis;

public:

    /**
     */
        std::string cartKey = "cart:" + userId;


        redis.expire(cartKey, 86400);

    }

    /**
     */
        std::string cartKey = "cart:" + userId;
        std::unordered_map<std::string, std::string> cart;


        }
            }
        }

    /**
         */
        std::string cartKey = "cart:" + userId;
        redis.del(cartKey);
            << ". El carrito efímero ha sido destruido.\n";
    }
};

        std::string userId = "user_88392";



        cartService.viewCart(userId);

        cartService.checkout(userId);

        cartService.viewCart(userId);

        return 1;
    }

    return 0;
}