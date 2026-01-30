
#include <iostream>
#include <string>
#include <stdexcept>
#include <memory>

class PassengerTransport {
protected:
    std::string routeNumber; // Номер маршрута
    unsigned passengerCapacity; // Вместимость пассажиров
    unsigned currentPassengers; // Текущее количество пассажиров
    double ticketPrice; // Цена билета

    static unsigned totalVehicles; // Общее количество транспортных средств

public:
    // Конструктор по умолчанию
    PassengerTransport(const std::string& route = "Неизвестно", unsigned capacity = 50, double price = 20.0) :
        routeNumber(route), passengerCapacity(capacity), currentPassengers(0), ticketPrice(price) {
        totalVehicles++;
        std::cout << "Создано транспортное средство. Всего: " << totalVehicles << std::endl;
    }

    // Конструктор копирования
    PassengerTransport(const PassengerTransport& other) :
        routeNumber(other.routeNumber), passengerCapacity(other.passengerCapacity),
        currentPassengers(other.currentPassengers), ticketPrice(other.ticketPrice) {
        totalVehicles++;
        std::cout << "Создана копия транспортного средства. Всего: " << totalVehicles << std::endl;
    }

    // Оператор присваивания
    PassengerTransport& operator=(const PassengerTransport& other) {
        if (this != &other) {
            routeNumber = other.routeNumber;
            passengerCapacity = other.passengerCapacity;
            currentPassengers = other.currentPassengers;
            ticketPrice = other.ticketPrice;
        }
        std::cout << "Транспортному средству присвоены новые значения." << std::endl;
        return *this;
    }

    // Виртуальный деструктор
    virtual ~PassengerTransport() {
        totalVehicles--;
        std::cout << "Транспортное средство уничтожено. Всего: " << totalVehicles << std::endl;
    }

    // Константные методы
    std::string getRouteNumber() const { return routeNumber; }
    unsigned getPassengerCapacity() const { return passengerCapacity; }
    unsigned getCurrentPassengers() const { return currentPassengers; }
    double getTicketPrice() const { return ticketPrice; }
    static unsigned getTotalVehicles() { return totalVehicles; }
    virtual std::string getType() const { return "Пассажирский транспорт"; }

    // Другие методы
    virtual bool embarkPassengers(unsigned numPassengers) {
        if (currentPassengers + numPassengers <= passengerCapacity) {
            currentPassengers += numPassengers;
            std::cout << "Посажено " << numPassengers << " пассажиров." << std::endl;
            return true;
        }
        std::cout << "Недостаточно места для " << numPassengers << " пассажиров." << std::endl;
        return false;
    }

    virtual void disembarkPassengers(unsigned numPassengers) {
        if (numPassengers > currentPassengers) {
            std::cout << "Высажено " << currentPassengers << " пассажиров (все)." << std::endl;
            currentPassengers = 0;
        } else {
            currentPassengers -= numPassengers;
            std::cout << "Высажено " << numPassengers << " пассажиров." << std::endl;
        }
    }

    // Дружественная функция для вывода в поток
    friend std::ostream& operator<<(std::ostream& os, const PassengerTransport& transport) {
        os << "Тип: " << transport.getType() << ", Маршрут: " << transport.routeNumber << ", Вместимость: " << transport.passengerCapacity
           << ", Пассажиров: " << transport.currentPassengers << ", Цена: " << transport.ticketPrice
           << ", Всего ТС: " << PassengerTransport::getTotalVehicles();
        return os;
    }

    // Оператор [] для расчета цены билета по возрасту (тупо, чтоб было по заданию, никакой практической ценности не несёт)
    double operator[](unsigned age) const {
        double discount = 0.0;
        if (age <= 7) {
            discount = 0.5; // 50% скидка для детей до 7 лет
            std::cout << "Применена детская скидка (50%)." << std::endl;
        } else if (age >= 65) {
            discount = 0.3; // 30% скидка для пенсионеров
            std::cout << "Применена пенсионная скидка (30%)." << std::endl;
        } else {
            std::cout << "Скидка не применена." << std::endl;
        }
        return ticketPrice * (1 - discount); // Цена со скидкой
    }

    
    virtual double calculateRevenue() const = 0;
};

unsigned PassengerTransport::totalVehicles = 0;

class Bus : public PassengerTransport {
private:
    bool hasWifi; // Есть ли Wi-Fi

public:
    Bus(const std::string& route = "Городской", unsigned capacity = 40, double price = 25.0, bool wifi = false) :
        PassengerTransport(route, capacity, price), hasWifi(wifi) {}

    Bus(const Bus& other) : PassengerTransport(other), hasWifi(other.hasWifi) {}

    Bus& operator=(const Bus& other) {
        if (this != &other) {
            PassengerTransport::operator=(other);
            hasWifi = other.hasWifi;
        }
        return *this;
    }

    ~Bus() override = default;

    friend std::ostream& operator<<(std::ostream& os, const Bus& bus) {
        os << static_cast<const PassengerTransport&>(bus) << ", Wi-Fi: " << (bus.hasWifi ? "Да" : "Нет");
        return os;
    }

    bool getHasWifi() const { return hasWifi; } // Added getter

    std::string getType() const override { return "Автобус"; }

    double calculateRevenue() const override {
        return currentPassengers * ticketPrice;
    }
};

class Tram : public PassengerTransport {
private:
    unsigned numberOfCars; // Количество вагонов

public:
    Tram(const std::string& route = "Трамвайная линия", unsigned capacity = 100, double price = 18.0, unsigned cars = 3) :
        PassengerTransport(route, capacity, price), numberOfCars(cars) {}

    Tram(const Tram& other) : PassengerTransport(other), numberOfCars(other.numberOfCars) {}

    Tram& operator=(const Tram& other) {
        if (this != &other) {
            PassengerTransport::operator=(other);
            numberOfCars = other.numberOfCars;
        }
        return *this;
    }

    ~Tram() override = default;

    friend std::ostream& operator<<(std::ostream& os, const Tram& tram) {
        os << static_cast<const PassengerTransport&>(tram) << ", Вагонов: " << tram.numberOfCars;
        return os;
    }

    std::string getType() const override { return "Трамвай"; }

    double calculateRevenue() const override {
        return currentPassengers * ticketPrice;
    }
};

void testTransport(PassengerTransport* transport) {
    int choice;
    do {
        std::cout << "\n--- Меню тестирования ---\n";
        std::cout << "1. Получить номер маршрута\n";
        std::cout << "2. Получить вместимость\n";
        std::cout << "3. Получить текущее количество пассажиров\n";
        std::cout << "4. Получить цену билета\n";
        std::cout << "5. Посадить пассажиров\n";
        std::cout << "6. Высадить пассажиров\n";
        std::cout << "7. Рассчитать выручку\n";
        std::cout << "8. Использовать Operator[] (Рассчитать цену билета по возрасту)\n";
        std::cout << "9. Вывести информацию\n";
        std::cout << "10. Проверить наличие Wi-Fi (только для автобусов)\n"; // Added option
        std::cout << "0. Выход\n";
        std::cout << "Выберите действие: ";
        std::cin >> choice;

        try {
            switch (choice) {
                case 1:
                    std::cout << "Номер маршрута: " << transport->getRouteNumber() << std::endl;
                    break;
                case 2:
                    std::cout << "Вместимость: " << transport->getPassengerCapacity() << std::endl;
                    break;
                case 3:
                    std::cout << "Текущее количество пассажиров: " << transport->getCurrentPassengers() << std::endl;
                    break;
                case 4:
                    std::cout << "Цена билета: " << transport->getTicketPrice() << std::endl;
                    break;
                case 5: {
                    unsigned numPassengers;
                    std::cout << "Введите количество пассажиров для посадки: ";
                    std::cin >> numPassengers;
                    transport->embarkPassengers(numPassengers);
                    break;
                }
                case 6: {
                    unsigned numPassengers;
                    std::cout << "Введите количество пассажиров для высадки: ";
                    std::cin >> numPassengers;
                    transport->disembarkPassengers(numPassengers);
                    break;
                }
                case 7:
                    std::cout << "Рассчитанная выручка: " << transport->calculateRevenue() << std::endl;
                    break;
                case 8: {
                    unsigned age;
                    std::cout << "Введите возраст пассажира: ";
                    std::cin >> age;
                    std::cout << "Цена билета (с учетом возраста): " << (*transport)[age] << std::endl;
                    break;
                }
                case 9:
                    std::cout << *transport << std::endl;
                    break;

                case 10: {  // Added case for Wi-Fi check
                    Bus* bus = dynamic_cast<Bus*>(transport); // Check if it's a Bus
                    if (bus) {
                        std::cout << "Наличие Wi-Fi: " << (bus->getHasWifi() ? "Да" : "Нет") << std::endl;
                    } else {
                        std::cout << "Это не автобус, проверка Wi-Fi невозможна.\n";
                    }
                    break;
                }

                case 0:
                    std::cout << "Выход...\n";
                    break;
                default:
                    std::cout << "Неверный выбор. Пожалуйста, попробуйте снова.\n";
            }
        } catch (const std::out_of_range& e) {
            std::cerr << "Ошибка: " << e.what() << std::endl;
        }
    } while (choice != 0);
}

int main() {
    PassengerTransport** vehicles = nullptr; // Указатель на массив указателей на PassengerTransport
    size_t vehiclesCount = 0;             // Количество транспортных средств в массиве
    size_t vehiclesCapacity = 0;          // Вместимость массива (сколько можно добавить без перевыделения)

    // Функция для добавления транспортного средства в массив
    auto addVehicle = [&](std::unique_ptr<PassengerTransport> vehicle) {
        if (vehiclesCount == vehiclesCapacity) {
            // Массив заполнен, нужно перевыделить память
            size_t newCapacity = (vehiclesCapacity == 0) ? 1 : vehiclesCapacity * 2; // Удваиваем размер
            PassengerTransport** newVehicles = new PassengerTransport*[newCapacity]; // Создаем новый массив

            
            for (size_t i = 0; i < vehiclesCount; ++i) {
                newVehicles[i] = vehicles[i];
            }

            
            delete[] vehicles;

            
            vehicles = newVehicles;
            vehiclesCapacity = newCapacity;
        }

       
        vehicles[vehiclesCount] = vehicle.release(); // передаем владение сырым указателем
        vehiclesCount++;
    };


    auto cleanupVehicles = [&]() {
        for (size_t i = 0; i < vehiclesCount; ++i) {
            delete vehicles[i];
        }
        delete[] vehicles;
        vehicles = nullptr;
        vehiclesCount = 0;
        vehiclesCapacity = 0;
    };

    int choice;
    do {
        std::cout << "\n--- Главное меню ---\n";
        std::cout << "1. Добавить автобус\n";
        std::cout << "2. Добавить трамвай\n";
        std::cout << "3. Тестировать транспортное средство (по индексу)\n";
        std::cout << "4. Вывести информацию о всех транспортных средствах\n";
        std::cout << "0. Выход\n";
        std::cout << "Выберите действие: ";
        std::cin >> choice;

        switch (choice) {
            case 1: {
                std::string route;
                unsigned capacity;
                double price;
                bool wifi;

                std::cout << "Введите номер маршрута: ";
                std::cin >> route;
                std::cout << "Введите вместимость: ";
                std::cin >> capacity;
                std::cout << "Введите цену билета: ";
                std::cin >> price;
                std::cout << "Есть ли Wi-Fi (1 - да, 0 - нет): ";
                std::cin >> wifi;

                try {
                    addVehicle(std::make_unique<Bus>(route, capacity, price, wifi));
                } catch (const std::bad_alloc& e) {
                    std::cerr << "Ошибка выделения памяти: " << e.what() << std::endl;
                }
                break;
            }
            case 2: {
                std::string route;
                unsigned capacity;
                double price;
                unsigned cars;

                std::cout << "Введите номер маршрута: ";
                std::cin >> route;
                std::cout << "Введите вместимость: ";
                std::cin >> capacity;
                std::cout << "Введите цену билета: ";
                std::cin >> price;
                std::cout << "Введите количество вагонов: ";
                std::cin >> cars;

                try {
                   addVehicle(std::make_unique<Tram>(route, capacity, price, cars));
                } catch (const std::bad_alloc& e) {
                    std::cerr << "Ошибка выделения памяти: " << e.what() << std::endl;
                }
                break;
            }
            case 3: {
                if (vehiclesCount == 0) {
                    std::cout << "Нет доступных транспортных средств для тестирования.\n";
                } else {
                    unsigned index;
                    std::cout << "Введите индекс транспортного средства для тестирования (0 - " << vehiclesCount - 1 << "): ";
                    std::cin >> index;

                    if (index < vehiclesCount) {
                        testTransport(vehicles[index]);
                    } else {
                        std::cout << "Неверный индекс транспортного средства.\n";
                    }
                }
                break;
            }
            case 4: {
                if (vehiclesCount == 0) {
                    std::cout << "Нет транспортных средств для вывода.\n";
                } else {
                    std::cout << "\n--- Информация о транспортных средствах ---\n";
                    for (size_t i = 0; i < vehiclesCount; ++i) {
                        std::cout << "Транспортное средство [" << i << "]: " << *vehicles[i] << std::endl;
                    }
                }
                break;
            }
            case 0:
                std::cout << "Выход из программы...\n";
                std::cout << "Общее количество транспортных средств: " << PassengerTransport::getTotalVehicles() << std::endl;
                break;
            default:
                std::cout << "Неверный выбор. Пожалуйста, попробуйте снова.\n";
        }
    } while (choice != 0);

    
    cleanupVehicles();
    return 0;
}
