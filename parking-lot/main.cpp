#include <thread>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <chrono>

enum VehicleType
{
    CAR,
    MOTORCYCLE,
    TRUCK
};

enum SpotType
{
    COMPACT,
    LARGE,
    SMALL
};

// VEHICLE PART
class Vehicle
{

protected:
    std::string licensePlate;
    VehicleType type;

public:
    Vehicle(std::string licensePlate, VehicleType type) : licensePlate(licensePlate),
                                                          type(type) {}
    std::string getLicensePlate() const { return licensePlate; }
    VehicleType getType() const { return type; }
    virtual ~Vehicle() = default;
};

class Car : public Vehicle
{
public:
    Car(std::string licensePlate) : Vehicle(licensePlate, VehicleType::CAR) {}
};

class Motorcycle : public Vehicle
{
public:
    Motorcycle(std::string licensePlate) : Vehicle(licensePlate, VehicleType::MOTORCYCLE) {}
};

class Truck : public Vehicle
{
public:
    Truck(std::string licensePlate) : Vehicle(licensePlate, VehicleType::TRUCK) {}
};

// PARKING PART

class ParkingSpot
{
protected:
    std::string spotId;
    SpotType type;
    Vehicle *vehicle;

public:
    ParkingSpot(const std::string &spotId, SpotType type)
        : spotId(spotId), type(type), vehicle(nullptr) {}

    virtual ~ParkingSpot() = default;

    std::string getSpotId() const
    {
        return spotId;
    }

    SpotType getType() const
    {
        return type;
    }

    Vehicle *getVehicle() const
    {
        return vehicle;
    }

    bool isOccupied() const
    {
        return vehicle != nullptr;
    }

    void assignVehicle(Vehicle *v)
    {
        vehicle = v;
    }

    void removeVehicle()
    {
        vehicle = nullptr;
    }

    virtual bool canFitVehicle(const Vehicle &vehicle) const = 0;
};

class CarSpot : public ParkingSpot
{
public:
    CarSpot(const std::string &spotId) : ParkingSpot(spotId, SpotType::COMPACT) {}

    bool canFitVehicle(const Vehicle &vehicle) const override
    {
        return !isOccupied() &&
               vehicle.getType() == VehicleType::CAR;
    }
};

class MotorcycleSpot : public ParkingSpot
{
public:
    MotorcycleSpot(std::string spotId) : ParkingSpot(spotId, SpotType::SMALL) {}

    bool canFitVehicle(const Vehicle &vehicle) const override
    {
        return !isOccupied() &&
               vehicle.getType() == VehicleType::MOTORCYCLE;
    }
};

class TruckSpot : public ParkingSpot
{
public:
    TruckSpot(std::string spotId) : ParkingSpot(spotId, SpotType::LARGE) {}

    bool canFitVehicle(const Vehicle &vehicle) const override
    {
        return !isOccupied() &&
               vehicle.getType() == VehicleType::TRUCK;
    }
};

class ParkingFloor
{
private:
    std::string floorId;
    std::vector<std::unique_ptr<ParkingSpot>> spots;

public:
    ParkingFloor(const std::string &floorId) : floorId(floorId) {}

    void addSpot(std::unique_ptr<ParkingSpot> spot)
    {
        spots.push_back(std::move(spot));
    }

    ParkingSpot *getAvailableSpot(const Vehicle &vehicle)
    {
        for (auto &spot : spots)
        {
            if (spot->canFitVehicle(vehicle))
                return spot.get();
        }

        return nullptr;
    }

    std::string getFloorId() const
    {
        return floorId;
    }
};

class ParkingLot
{
private:
    std::unordered_map<std::string, std::unique_ptr<ParkingFloor>> floors;

public:
    ParkingSpot *findAvailableSpot(const Vehicle &vehicle)
    {
        for (auto &floor : floors)
        {
            ParkingSpot *spot =
                floor.second->getAvailableSpot(vehicle);

            if (spot)
                return spot;
        }

        return nullptr;
    }

    void addFloor(std::unique_ptr<ParkingFloor> floor)
    {
        floors[floor->getFloorId()] = std::move(floor);
    }
};

class Ticket
{
private:
    std::string ticketId;

    Vehicle *vehicle;

    ParkingSpot *spot;

    time_t entryTime;

public:
    Ticket(std::string ticketId, Vehicle *vehicle, ParkingSpot *spot)
        : ticketId(ticketId), vehicle(vehicle), spot(spot), entryTime(time(nullptr)) {}

    std::string getTicketId() const { return ticketId; }
    Vehicle *getVehicle() const
    {
        return vehicle;
    }

    ParkingSpot *getSpot() const
    {
        return spot;
    }
    time_t getEntryTime() const
    {
        return entryTime;
    }
};

class FeeStrategy
{
public:
    virtual double calculateFee(const Ticket &ticket) const = 0;

    virtual ~FeeStrategy() = default;
};
class ParkingManager
{
private:
    ParkingLot &parkingLot;

    FeeStrategy &feeStrategy;

    std::unordered_map<
        std::string,
        std::unique_ptr<Ticket>>
        tickets;

    int ticketCounter = 0;

    std::string generateTicketId()
    {
        return "TICKET-" +
               std::to_string(++ticketCounter);
    }

public:
    ParkingManager(
        ParkingLot &lot,
        FeeStrategy &strategy)
        : parkingLot(lot),
          feeStrategy(strategy)
    {
    }

    Ticket *parkVehicle(
        Vehicle &vehicle)
    {
        ParkingSpot *spot =
            parkingLot.findAvailableSpot(vehicle);

        if (!spot)
            return nullptr;

        spot->assignVehicle(&vehicle);

        std::string id =
            generateTicketId();

        auto ticket =
            std::make_unique<Ticket>(
                id,
                &vehicle,
                spot);

        Ticket *ptr =
            ticket.get();

        tickets[id] =
            std::move(ticket);

        return ptr;
    }

    double exitVehicle(
        const std::string &ticketId)
    {
        auto it =
            tickets.find(ticketId);

        if (it == tickets.end())
            return 0.0;

        Ticket *ticket =
            it->second.get();

        double fee =
            feeStrategy.calculateFee(*ticket);

        ticket->getSpot()->removeVehicle();

        tickets.erase(it);

        return fee;
    }
};

class HourlyFeeStrategy : public FeeStrategy
{
public:
    double calculateFee(const Ticket &ticket) const override
    {
        time_t currentTime = time(nullptr);

        double hours =
            difftime(currentTime,
                     ticket.getEntryTime()) /
            3600.0;

        return hours * 2.0;
    }
};

int main()
{
    // Create Parking Lot
    ParkingLot parkingLot;

    // Floor 1
    auto floor1 = std::make_unique<ParkingFloor>("F1");

    floor1->addSpot(std::make_unique<CarSpot>("C1"));
    floor1->addSpot(std::make_unique<CarSpot>("C2"));
    floor1->addSpot(std::make_unique<MotorcycleSpot>("M1"));
    floor1->addSpot(std::make_unique<TruckSpot>("T1"));

    parkingLot.addFloor(std::move(floor1));

    // Pricing Strategy
    HourlyFeeStrategy feeStrategy;

    // Parking Manager
    ParkingManager manager(parkingLot, feeStrategy);

    // Vehicles
    Car car1("PB10AA1111");
    Car car2("PB10AA2222");
    Motorcycle bike1("PB10BB1111");
    Truck truck1("PB10CC1111");

    // -------------------------
    // Park Vehicles
    // -------------------------

    Ticket *ticket1 = manager.parkVehicle(car1);
    Ticket *ticket2 = manager.parkVehicle(car2);
    Ticket *ticket3 = manager.parkVehicle(bike1);
    Ticket *ticket4 = manager.parkVehicle(truck1);

    if (ticket1)
    {
        std::cout << "Car1 parked.\n";
        std::cout << "Ticket : "
                  << ticket1->getTicketId()
                  << std::endl;
        std::cout << "Spot : "
                  << ticket1->getSpot()->getSpotId()
                  << std::endl;
    }

    if (ticket2)
    {
        std::cout << "\nCar2 parked.\n";
        std::cout << "Ticket : "
                  << ticket2->getTicketId()
                  << std::endl;
        std::cout << "Spot : "
                  << ticket2->getSpot()->getSpotId()
                  << std::endl;
    }

    if (ticket3)
    {
        std::cout << "\nBike parked.\n";
        std::cout << "Ticket : "
                  << ticket3->getTicketId()
                  << std::endl;
        std::cout << "Spot : "
                  << ticket3->getSpot()->getSpotId()
                  << std::endl;
    }

    if (ticket4)
    {
        std::cout << "\nTruck parked.\n";
        std::cout << "Ticket : "
                  << ticket4->getTicketId()
                  << std::endl;
        std::cout << "Spot : "
                  << ticket4->getSpot()->getSpotId()
                  << std::endl;
    }

    // -------------------------
    // Wait for 5 seconds
    // -------------------------

    std::this_thread::sleep_for(std::chrono::seconds(5));

    // -------------------------
    // Exit Car1
    // -------------------------

    double fee =
        manager.exitVehicle(ticket1->getTicketId());

    std::cout << "\nCar1 exited.\n";
    std::cout << "Fee = "
              << fee
              << std::endl;

    return 0;
}

                                