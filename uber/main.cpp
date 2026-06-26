#include <string>
#include <map>
#include <vector>
using namespace std;

enum class VehicleType
{
    BIKE,
    AUTO,
    CAB
};

enum class RideStatus
{
    REQUESTED,
    ACCEPTED,
    STARTED,
    COMPLETED,
    CANCELLED
};

enum class PaymentStatus
{
    PENDING,
    SUCCESS,
    FAILED
};

class Location
{
private:
    double latitude;
    double longitude;

public:
    Location(double lat, double lon)
        : latitude(lat), longitude(lon) {}

    double getLatitude() const
    {
        return latitude;
    }

    double getLongitude() const
    {
        return longitude;
    }
};

class Vehicle
{
private:
    string registrationNo;
    VehicleType type;

public:
    Vehicle(string regNo, VehicleType type)
        : registrationNo(regNo), type(type) {}

    VehicleType getType() const
    {
        return type;
    }

    string getRegistrationNo() const
    {
        return registrationNo;
    }
};

class User
{
protected:
    int id;
    string name;
    string email;
    string phoneNo;
    Location location;

public:
    User(
        int id,
        string name,
        string email,
        string phone,
        Location location)
        : id(id),
          name(name),
          email(email),
          phoneNo(phone),
          location(location) {}

    virtual ~User() {}

    Location getLocation() const
    {
        return location;
    }

    string getName() const
    {
        return name;
    }
};

class Rider : public User
{

public:
    Rider(
        int id,
        string name,
        string email,
        string phone)
        : User(id, name, email, phone) {}

    void startRide()
    {
        // start ride code must be here
    }

    void selectRideType()
    {
        // ride selevtion code must be here
    }
};

class Driver : public User
{

private:
    string licenseNo;
    Vehicle vehicle;

public:
    Driver(
        int id,
        string name,
        string email,
        string phone,
        Location location,
        string license,
        Vehicle vehicle)
        : User(id, name, email, phone, location),
          licenseNo(license),
          vehicle(vehicle) {}

    Vehicle getVehicle() const
    {
        return vehicle;
    }

    void actionOnRide(bool accept)
    {

        if (accept)
            //   accept code must be here
            else
        // reject code must be here
    }
};

class FareStrategy
{
public:
    virtual double calculateFare(double distance) = 0;

    virtual ~FareStrategy() {}
};

class BikeFareStrategy : public FareStrategy
{

public:
    double calculateFare(double distance) override
    {

        return distance * 8;
    }
};

class CabFareStrategy : public FareStrategy
{

public:
    double calculateFare(double distance) override
    {

        return distance * 18;
    }
};

class AutoFareStrategy : public FareStrategy
{

public:
    double calculateFare(double distance) override
    {

        return distance * 12;
    }
};

class Fare
{

private:
    double amount;

public:
    Fare(double amount)
        : amount(amount) {}

    double getAmount()
    {

        return amount;
    }
};

class FareManager
{

public:
    map<VehicleType, double> estimateFare(double distance)
    {

        map<VehicleType, double> fares;

        BikeFareStrategy bike;
        AutoFareStrategy autoFare;
        CabFareStrategy cab;

        fares[VehicleType::BIKE] = bike.calculateFare(distance);

        fares[VehicleType::AUTO] = autoFare.calculateFare(distance);

        fares[VehicleType::CAB] = cab.calculateFare(distance);

        return fares;
    }
};

class RideRequest
{

private:
    Rider *rider;

    Location source;

    Location destination;

    VehicleType vehicleType;

    Fare fare;

    RideStatus status;

public:
    RideRequest(
        Rider *rider,
        Location src,
        Location dest,
        VehicleType type,
        Fare fare)
        : rider(rider),
          source(src),
          destination(dest),
          vehicleType(type),
          fare(fare),
          status(RideStatus::REQUESTED)
    {
    }

    VehicleType getVehicleType()
    {

        return vehicleType;
    }

    Rider *getRider()
    {

        return rider;
    }
};

class DriverMatchingStrategy {

public:

    virtual Driver* findDriver(
        RideRequest* request,
        vector<Driver*>& drivers) = 0;

    virtual ~DriverMatchingStrategy() {}
};

class NearestDriverStrategy : public DriverMatchingStrategy {

public:

    Driver* findDriver(
        RideRequest* request,
        vector<Driver*>& drivers) override {

        if(drivers.empty())
            return nullptr;

        return drivers[0];
    }
};

class DriverManager {

private:

    vector<Driver*> drivers;

public:

    void addDriver(Driver* driver) {

        drivers.push_back(driver);
    }

    vector<Driver*>& getAvailableDrivers() {

        return drivers;
    }
};

class MatchingManager {

private:

    DriverMatchingStrategy* strategy;

public:

    MatchingManager(
        DriverMatchingStrategy* strategy)
        : strategy(strategy) {}

    Driver* assignDriver(
        RideRequest* request,
        vector<Driver*>& drivers) {

        return strategy->findDriver(
            request,
            drivers);
    }
};

class Ride {

private:

    Rider* rider;
    Driver* driver;

    Location source;
    Location destination;

    Vehicle vehicle;

    Fare fare;

    RideStatus status;

public:

    Ride(
        Rider* rider,
        Driver* driver,
        Location src,
        Location dest,
        Vehicle vehicle,
        Fare fare)
        :
        rider(rider),
        driver(driver),
        source(src),
        destination(dest),
        vehicle(vehicle),
        fare(fare),
        status(RideStatus::ACCEPTED)
    {
    }

    void startRide() {

        status = RideStatus::STARTED;
    }

    void completeRide() {

        status = RideStatus::COMPLETED;
    }
};


class RideManager {

public:

    Ride* createRide(
            RideRequest* request,
            Driver* driver) {

        Ride* ride = new Ride(
                request->getRider(),
                driver,
                request->getSource(),
                request->getDestination(),
                driver->getVehicle(),
                request->getFare());

        return ride;
    }
};