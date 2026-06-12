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
    MOTORCYCLE
};

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

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
        : spotId(spotId), type(type), vehicle(nullptr)
    {
    }

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
    MotorcycleSpot(std::string spotId) : ParkingSpot(spotId, SpotType::MOTORCYCLE) {}

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
};