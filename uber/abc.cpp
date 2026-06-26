#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <limits>
using namespace std;

// ============================================================================
// SECTION 1: ENUMERATIONS (Domain Layer)
// ============================================================================

// VehicleType represents the types of vehicles available for rides.
enum class VehicleType
{
    BIKE,
    AUTO,
    CAB
};

// RideStatus represents the lifecycle of a ride request/ride.
enum class RideStatus
{
    REQUESTED,
    ACCEPTED,
    STARTED,
    COMPLETED,
    CANCELLED
};

// PaymentStatus represents the state of a payment.
enum class PaymentStatus
{
    PENDING,
    SUCCESS,
    FAILED
};

// Helper function to convert VehicleType to string for display.
inline std::string vehicleTypeToString(VehicleType type)
{
    switch (type)
    {
    case VehicleType::BIKE:
        return "BIKE";
    case VehicleType::AUTO:
        return "AUTO";
    case VehicleType::CAB:
        return "CAB";
    }
    return "UNKNOWN";
}

// Helper function to convert RideStatus to string for display.
inline std::string rideStatusToString(RideStatus status)
{
    switch (status)
    {
    case RideStatus::REQUESTED:
        return "REQUESTED";
    case RideStatus::ACCEPTED:
        return "ACCEPTED";
    case RideStatus::STARTED:
        return "STARTED";
    case RideStatus::COMPLETED:
        return "COMPLETED";
    case RideStatus::CANCELLED:
        return "CANCELLED";
    }
    return "UNKNOWN";
}

// ============================================================================
// SECTION 2: DOMAIN ENTITIES & VALUE OBJECTS (Domain Layer)
// ============================================================================

// ----------------------------------------------------------------------------
// Location (Value Object)
// Represents a geographical coordinate point.
// ----------------------------------------------------------------------------
class Location
{
private:
    double latitude;
    double longitude;

public:
    Location(double lat, double lon) : latitude(lat), longitude(lon) {}

    double getLatitude() const { return latitude; }
    double getLongitude() const { return longitude; }
};

// ----------------------------------------------------------------------------
// Vehicle (Value Object / Entity)
// Represents the driver's vehicle.
// ----------------------------------------------------------------------------
class Vehicle
{
private:
    std::string registrationNo;
    VehicleType type;

public:
    Vehicle(const std::string &regNo, VehicleType type)
        : registrationNo(regNo), type(type) {}

    std::string getRegistrationNo() const { return registrationNo; }
    VehicleType getType() const { return type; }
};

// ----------------------------------------------------------------------------
// User (Abstract Base Class)
// Represents a generic system user.
// NOTE: "i dont want location for rider". Thus, Location is NOT part of User
// to ensure Rider does not inherit Location. Only Driver will possess a Location.
// ----------------------------------------------------------------------------
class User
{
protected:
    int id;
    std::string name;
    std::string email;
    std::string phoneNo;

public:
    User(int id, const std::string &name, const std::string &email, const std::string &phone)
        : id(id), name(name), email(email), phoneNo(phone) {}

    virtual ~User() = default;

    int getId() const { return id; }
    std::string getName() const { return name; }
    std::string getEmail() const { return email; }
    std::string getPhoneNo() const { return phoneNo; }
};

// ----------------------------------------------------------------------------
// Rider (Domain Entity)
// Represents the passenger. Initiates/Requests rides.
// No direct creation of Ride/RideRequest happens here; it is delegated to Managers.
// ----------------------------------------------------------------------------
class Rider : public User
{
public:
    Rider(int id, const std::string &name, const std::string &email, const std::string &phone)
        : User(id, name, email, phone) {}

    void requestRide(const Location &source, const Location &destination) const
    {
        std::cout << "[Rider: " << name << "] Triggered requestRide from ("
                  << source.getLatitude() << ", " << source.getLongitude() << ") to ("
                  << destination.getLatitude() << ", " << destination.getLongitude() << ").\n";
    }

    VehicleType selectVehicleType(VehicleType type) const
    {
        std::cout << "[Rider: " << name << "] Selected vehicle type: " << vehicleTypeToString(type) << ".\n";
        return type;
    }

    void payRide(double amount) const
    {
        std::cout << "[Rider: " << name << "] Processing payment of $" << amount << ".\n";
    }

    void rateRide(int stars, const std::string &review) const
    {
        std::cout << "[Rider: " << name << "] Rated ride: " << stars << " stars. Review: \"" << review << "\"\n";
    }
};

// ----------------------------------------------------------------------------
// Driver (Domain Entity)
// Represents the driver offering rides. Possesses a current location and vehicle.
// ----------------------------------------------------------------------------
class Driver : public User
{
private:
    std::string licenseNo;
    Vehicle vehicle;
    bool available;
    Location location;
    double rating; // Stores driver's average rating

public:
    Driver(int id, const std::string &name, const std::string &email, const std::string &phone,
           const std::string &license, const Vehicle &vehicle, const Location &initialLocation, double rating = 5.0)
        : User(id, name, email, phone),
          licenseNo(license),
          vehicle(vehicle),
          available(true),
          location(initialLocation),
          rating(rating) {}

    std::string getLicenseNo() const { return licenseNo; }
    Vehicle getVehicle() const { return vehicle; }
    double getRating() const { return rating; }
    void setRating(double r) { rating = r; }

    bool isAvailable() const { return available; }
    void setAvailability(bool status)
    {
        available = status;
    }

    Location getLocation() const { return location; }
    void setLocation(const Location &newLocation)
    {
        location = newLocation;
    }

    bool acceptRide()
    {
        std::cout << "[Driver: " << name << "] Accepted the ride.\n";
        return true;
    }

    void rejectRide()
    {
        std::cout << "[Driver: " << name << "] Rejected the ride.\n";
    }

    void startRide()
    {
        std::cout << "[Driver: " << name << "] Starting the journey.\n";
    }

    void completeRide()
    {
        std::cout << "[Driver: " << name << "] Completed the journey.\n";
    }
};

// ----------------------------------------------------------------------------
// Fare (Value Object)
// Holds information about the fare amount.
// ----------------------------------------------------------------------------
class Fare
{
private:
    double amount;

public:
    Fare() : amount(0.0) {}
    explicit Fare(double amount) : amount(amount) {}

    double getAmount() const { return amount; }
};

// ----------------------------------------------------------------------------
// Payment (Domain Entity)
// Tracks the status and details of a ride's payment.
// ----------------------------------------------------------------------------
class Payment
{
private:
    double amount;
    PaymentStatus status;

public:
    Payment(double amount) : amount(amount), status(PaymentStatus::PENDING) {}

    double getAmount() const { return amount; }
    PaymentStatus getStatus() const { return status; }

    void processPayment()
    {
        status = PaymentStatus::SUCCESS;
        std::cout << "[Payment] Successfully charged $" << amount << ". Status: SUCCESS.\n";
    }
};

// ----------------------------------------------------------------------------
// Rating (Value Object)
// Represents a feedback score and review for a completed ride.
// ----------------------------------------------------------------------------
class Rating
{
private:
    int rating;
    std::string review;

public:
    Rating(int ratingVal, const std::string &reviewText = "")
        : rating(ratingVal), review(reviewText) {}

    int getRating() const { return rating; }
    std::string getReview() const { return review; }
};

// ----------------------------------------------------------------------------
// RideRequest (Domain Entity)
// Holds parameters of a ride request initiated by a Rider.
// ----------------------------------------------------------------------------
class RideRequest
{
private:
    std::shared_ptr<Rider> rider;
    Location source;
    Location destination;
    VehicleType vehicleType;
    Fare estimatedFare;
    RideStatus status;

public:
    RideRequest(const std::shared_ptr<Rider> &rider, const Location &source, const Location &destination,
                VehicleType vehicleType, const Fare &estimatedFare)
        : rider(rider),
          source(source),
          destination(destination),
          vehicleType(vehicleType),
          estimatedFare(estimatedFare),
          status(RideStatus::REQUESTED) {}

    std::shared_ptr<Rider> getRider() const { return rider; }
    Location getSource() const { return source; }
    Location getDestination() const { return destination; }
    VehicleType getVehicleType() const { return vehicleType; }
    Fare getFare() const { return estimatedFare; }
    RideStatus getStatus() const { return status; }
    void setStatus(RideStatus newStatus) { status = newStatus; }
};

// ----------------------------------------------------------------------------
// Ride (Domain Entity)
// Represents an active or completed trip details.
// ----------------------------------------------------------------------------
class Ride
{
private:
    std::shared_ptr<Rider> rider;
    std::shared_ptr<Driver> driver;
    Vehicle vehicle;
    Location source;
    Location destination;
    Fare finalFare;
    std::shared_ptr<Payment> payment;
    std::shared_ptr<Rating> rating;
    RideStatus status;
    std::time_t startTime;
    std::time_t endTime;
    double distance;

public:
    Ride(const std::shared_ptr<Rider> &rider, const std::shared_ptr<Driver> &driver, const Vehicle &vehicle,
         const Location &source, const Location &destination, const Fare &finalFare, double distance)
        : rider(rider),
          driver(driver),
          vehicle(vehicle),
          source(source),
          destination(destination),
          finalFare(finalFare),
          payment(nullptr),
          rating(nullptr),
          status(RideStatus::ACCEPTED),
          startTime(0),
          endTime(0),
          distance(distance) {}

    std::shared_ptr<Rider> getRider() const { return rider; }
    std::shared_ptr<Driver> getDriver() const { return driver; }
    Vehicle getVehicle() const { return vehicle; }
    Location getSource() const { return source; }
    Location getDestination() const { return destination; }
    Fare getFinalFare() const { return finalFare; }
    std::shared_ptr<Payment> getPayment() const { return payment; }
    std::shared_ptr<Rating> getRating() const { return rating; }
    RideStatus getStatus() const { return status; }
    double getDistance() const { return distance; }

    void startRide()
    {
        status = RideStatus::STARTED;
        startTime = std::time(nullptr);
        std::cout << "[Ride] Ride status: STARTED at " << startTime << ".\n";
    }

    void completeRide()
    {
        status = RideStatus::COMPLETED;
        endTime = std::time(nullptr);
        std::cout << "[Ride] Ride status: COMPLETED at " << endTime << ".\n";
    }

    void cancelRide()
    {
        status = RideStatus::CANCELLED;
        std::cout << "[Ride] Ride status: CANCELLED.\n";
    }

    void setPayment(const std::shared_ptr<Payment> &pay) { payment = pay; }
    void setRating(const std::shared_ptr<Rating> &rat) { rating = rat; }
};

// ============================================================================
// SECTION 3: DESIGN PATTERNS - STRATEGIES (Domain Layer)
// ============================================================================

// ----------------------------------------------------------------------------
// Strategy Pattern: Fare Strategy
// Open-Closed Principle: Easily extendable with new pricing models.
// ----------------------------------------------------------------------------
class FareStrategy
{
public:
    virtual ~FareStrategy() = default;
    virtual double calculateFare(double distance) const = 0;
};

class BikeFareStrategy : public FareStrategy
{
public:
    double calculateFare(double distance) const override
    {
        return distance * 8.0; // $8 per unit distance
    }
};

class AutoFareStrategy : public FareStrategy
{
public:
    double calculateFare(double distance) const override
    {
        return distance * 12.0; // $12 per unit distance
    }
};

class CabFareStrategy : public FareStrategy
{
public:
    double calculateFare(double distance) const override
    {
        return distance * 18.0; // $18 per unit distance
    }
};

// ----------------------------------------------------------------------------
// Strategy Pattern: Driver Matching Strategy
// Decouples matching logic from managers.
// ----------------------------------------------------------------------------
class DriverMatchingStrategy
{
public:
    virtual ~DriverMatchingStrategy() = default;
    virtual std::shared_ptr<Driver> findDriver(
        const std::shared_ptr<RideRequest> &request,
        const std::vector<std::shared_ptr<Driver>> &drivers) const = 0;
};

// Matches the nearest driver using Euclidean distance
class NearestDriverStrategy : public DriverMatchingStrategy
{
public:
    std::shared_ptr<Driver> findDriver(
        const std::shared_ptr<RideRequest> &request,
        const std::vector<std::shared_ptr<Driver>> &drivers) const override
    {

        std::shared_ptr<Driver> bestDriver = nullptr;
        double minDistance = std::numeric_limits<double>::max();
        Location source = request->getSource();

        for (const auto &driver : drivers)
        {
            if (!driver->isAvailable())
                continue;
            if (driver->getVehicle().getType() != request->getVehicleType())
                continue;

            Location driverLoc = driver->getLocation();
            double dist = std::sqrt(std::pow(driverLoc.getLatitude() - source.getLatitude(), 2) +
                                    std::pow(driverLoc.getLongitude() - source.getLongitude(), 2));

            if (dist < minDistance)
            {
                minDistance = dist;
                bestDriver = driver;
            }
        }
        return bestDriver;
    }
};

// Matches the highest-rated driver available
class HighestRatedDriverStrategy : public DriverMatchingStrategy
{
public:
    std::shared_ptr<Driver> findDriver(
        const std::shared_ptr<RideRequest> &request,
        const std::vector<std::shared_ptr<Driver>> &drivers) const override
    {

        std::shared_ptr<Driver> bestDriver = nullptr;
        double maxRating = -1.0;

        for (const auto &driver : drivers)
        {
            if (!driver->isAvailable())
                continue;
            if (driver->getVehicle().getType() != request->getVehicleType())
                continue;

            if (driver->getRating() > maxRating)
            {
                maxRating = driver->getRating();
                bestDriver = driver;
            }
        }
        return bestDriver;
    }
};

// ============================================================================
// SECTION 4: APPLICATION MANAGERS (Application Layer)
// All business logic is encapsulated in these managers.
// ============================================================================

// ----------------------------------------------------------------------------
// DriverManager
// Responsible for registering drivers, updating location, and getting available drivers.
// ----------------------------------------------------------------------------
class DriverManager
{
private:
    std::vector<std::shared_ptr<Driver>> drivers;

public:
    void registerDriver(const std::shared_ptr<Driver> &driver)
    {
        drivers.push_back(driver);
        std::cout << "[DriverManager] Registered Driver: " << driver->getName()
                  << " | Vehicle Type: " << vehicleTypeToString(driver->getVehicle().getType())
                  << " | Initial Location: (" << driver->getLocation().getLatitude() << ", "
                  << driver->getLocation().getLongitude() << ") | Rating: " << driver->getRating() << "\n";
    }

    std::vector<std::shared_ptr<Driver>> getAvailableDrivers() const
    {
        std::vector<std::shared_ptr<Driver>> available;
        for (const auto &driver : drivers)
        {
            if (driver->isAvailable())
            {
                available.push_back(driver);
            }
        }
        return available;
    }

    void updateDriverLocation(const std::shared_ptr<Driver> &driver, const Location &newLocation)
    {
        driver->setLocation(newLocation);
        std::cout << "[DriverManager] Updated " << driver->getName() << "'s location to ("
                  << newLocation.getLatitude() << ", " << newLocation.getLongitude() << ").\n";
    }
};

// ----------------------------------------------------------------------------
// FareManager
// Calculates & estimates fares using various FareStrategies.
// ----------------------------------------------------------------------------
class FareManager
{
private:
    std::map<VehicleType, std::unique_ptr<FareStrategy>> strategies;

public:
    FareManager()
    {
        strategies[VehicleType::BIKE] = std::make_unique<BikeFareStrategy>();
        strategies[VehicleType::AUTO] = std::make_unique<AutoFareStrategy>();
        strategies[VehicleType::CAB] = std::make_unique<CabFareStrategy>();
    }

    std::map<VehicleType, double> estimateFare(double distance) const
    {
        std::map<VehicleType, double> estimates;
        for (const auto &[type, strategy] : strategies)
        {
            estimates[type] = strategy->calculateFare(distance);
        }
        return estimates;
    }

    double calculateFareForType(VehicleType type, double distance) const
    {
        auto it = strategies.find(type);
        if (it != strategies.end())
        {
            return it->second->calculateFare(distance);
        }
        return 0.0;
    }
};

// ----------------------------------------------------------------------------
// MatchingManager
// Uses a matching strategy to assign a driver.
// ----------------------------------------------------------------------------
class MatchingManager
{
private:
    std::unique_ptr<DriverMatchingStrategy> matchingStrategy;

public:
    explicit MatchingManager(std::unique_ptr<DriverMatchingStrategy> strategy)
        : matchingStrategy(std::move(strategy)) {}

    void setMatchingStrategy(std::unique_ptr<DriverMatchingStrategy> strategy)
    {
        matchingStrategy = std::move(strategy);
    }

    std::shared_ptr<Driver> assignDriver(const std::shared_ptr<RideRequest> &request,
                                         const std::vector<std::shared_ptr<Driver>> &availableDrivers)
    {
        return matchingStrategy->findDriver(request, availableDrivers);
    }
};

// ----------------------------------------------------------------------------
// PaymentManager
// Handles payment processing and status updates.
// ----------------------------------------------------------------------------
class PaymentManager
{
public:
    void processPayment(const std::shared_ptr<Ride> &ride)
    {
        std::cout << "[PaymentManager] Starting payment processing for Ride...\n";
        double amount = ride->getFinalFare().getAmount();
        auto payment = std::make_shared<Payment>(amount);
        payment->processPayment();
        ride->setPayment(payment);
    }
};

// ----------------------------------------------------------------------------
// RatingManager
// Manages and stores ride ratings.
// ----------------------------------------------------------------------------
class RatingManager
{
public:
    void submitRating(const std::shared_ptr<Ride> &ride, int ratingVal, const std::string &review = "")
    {
        std::cout << "[RatingManager] Submitting Ride Rating: " << ratingVal << " stars. Review: \"" << review << "\"\n";
        auto ratingObj = std::make_shared<Rating>(ratingVal, review);
        ride->setRating(ratingObj);

        // Update driver's rating (simple simulation update)
        auto driver = ride->getDriver();
        double oldRating = driver->getRating();
        double newRating = (oldRating + ratingVal) / 2.0; // Simplified moving average
        driver->setRating(newRating);
        std::cout << "[RatingManager] Updated Driver " << driver->getName() << "'s rating to " << std::fixed << std::setprecision(2) << newRating << ".\n";
    }
};

// ----------------------------------------------------------------------------
// RideManager (Orchestrator)
// Facade & Coordinator that guides the lifecycle of a ride request and trip.
// ----------------------------------------------------------------------------
class RideManager
{
private:
    std::shared_ptr<DriverManager> driverManager;
    std::shared_ptr<FareManager> fareManager;
    std::shared_ptr<MatchingManager> matchingManager;
    std::shared_ptr<PaymentManager> paymentManager;
    std::shared_ptr<RatingManager> ratingManager;

public:
    RideManager(const std::shared_ptr<DriverManager> &dm,
                const std::shared_ptr<FareManager> &fm,
                const std::shared_ptr<MatchingManager> &mm,
                const std::shared_ptr<PaymentManager> &pm,
                const std::shared_ptr<RatingManager> &rm)
        : driverManager(dm),
          fareManager(fm),
          matchingManager(mm),
          paymentManager(pm),
          ratingManager(rm) {}

    std::shared_ptr<RideRequest> createRideRequest(const std::shared_ptr<Rider> &rider,
                                                   const Location &source,
                                                   const Location &destination,
                                                   VehicleType vehicleType)
    {

        // Calculate Euclidean distance between source and destination
        double distance = std::sqrt(std::pow(destination.getLatitude() - source.getLatitude(), 2) +
                                    std::pow(destination.getLongitude() - source.getLongitude(), 2));

        double estimate = fareManager->calculateFareForType(vehicleType, distance);
        Fare estFare(estimate);

        auto request = std::make_shared<RideRequest>(rider, source, destination, vehicleType, estFare);
        std::cout << "[RideManager] RideRequest successfully created for " << rider->getName()
                  << " | Distance: " << std::fixed << std::setprecision(2) << distance << " units"
                  << " | Estimated Fare: $" << estimate << "\n";
        return request;
    }

    std::shared_ptr<Ride> createRide(const std::shared_ptr<RideRequest> &request, const std::shared_ptr<Driver> &driver)
    {
        if (!driver)
        {
            std::cout << "[RideManager] Matching failed. No available driver matched the criteria.\n";
            return nullptr;
        }

        double distance = std::sqrt(std::pow(request->getDestination().getLatitude() - request->getSource().getLatitude(), 2) +
                                    std::pow(request->getDestination().getLongitude() - request->getSource().getLongitude(), 2));

        auto ride = std::make_shared<Ride>(
            request->getRider(),
            driver,
            driver->getVehicle(),
            request->getSource(),
            request->getDestination(),
            request->getFare(),
            distance);

        request->setStatus(RideStatus::ACCEPTED);
        driver->setAvailability(false); // Make driver unavailable for other rides
        std::cout << "[RideManager] Ride successfully matched & created with Driver: " << driver->getName() << "\n";
        return ride;
    }

    void startRide(const std::shared_ptr<Ride> &ride)
    {
        if (!ride)
            return;
        ride->startRide();
    }

    void completeRide(const std::shared_ptr<Ride> &ride)
    {
        if (!ride)
            return;
        ride->completeRide();
        paymentManager->processPayment(ride);
        ride->getDriver()->setAvailability(true); // Driver is available again
    }

    void cancelRide(const std::shared_ptr<Ride> &ride)
    {
        if (!ride)
            return;
        ride->cancelRide();
        ride->getDriver()->setAvailability(true);
    }
};

// ============================================================================
// SECTION 5: SIMULATION (UI/Presentation Layer)
// Demonstrates the full ride-booking lifecycle.
// ============================================================================
int main()
{
    cout << "============================================================\n";
    cout << "         UBER-LIKE RIDE BOOKING SYSTEM SIMULATION\n";
    cout << "============================================================\n\n";

    auto driverManager = make_shared<DriverManager>();
    auto fareManager = make_shared<FareManager>();

    auto matchingManager =
        make_shared<MatchingManager>(
            make_unique<NearestDriverStrategy>());

    auto paymentManager = make_shared<PaymentManager>();
    auto ratingManager = make_shared<RatingManager>();

    auto rideManager = make_shared<RideManager>(
        driverManager,
        fareManager,
        matchingManager,
        paymentManager,
        ratingManager);

    // ====================================================
    // Rider Input
    // ====================================================

    string riderName;
    string email;
    string phone;

    cout << "Enter Rider Name : ";
    getline(cin, riderName);

    cout << "Enter Email : ";
    getline(cin, email);

    cout << "Enter Phone : ";
    getline(cin, phone);

    auto rider =
        make_shared<Rider>(
            1,
            riderName,
            email,
            phone);

    // ====================================================
    // Register Drivers
    // ====================================================

    Vehicle v1("CAB-1234", VehicleType::CAB);
    Vehicle v2("AUTO-5678", VehicleType::AUTO);
    Vehicle v3("CAB-9999", VehicleType::CAB);

    auto driverBob =
        make_shared<Driver>(
            101,
            "Bob",
            "bob@driver.com",
            "+1-555-0201",
            "LIC-BOB123",
            v1,
            Location(10.0, 10.0),
            4.8);

    auto driverCharlie =
        make_shared<Driver>(
            102,
            "Charlie",
            "charlie@driver.com",
            "+1-555-0202",
            "LIC-CHAS56",
            v2,
            Location(5.0, 5.0),
            4.5);

    auto driverDavid =
        make_shared<Driver>(
            103,
            "David",
            "david@driver.com",
            "+1-555-0203",
            "LIC-DAV999",
            v3,
            Location(2.0, 2.0),
            4.9);

    driverManager->registerDriver(driverBob);
    driverManager->registerDriver(driverCharlie);
    driverManager->registerDriver(driverDavid);

    // ====================================================
    // Source / Destination Input
    // ====================================================

    double srcLat, srcLon;
    double destLat, destLon;

    cout << "\nEnter Source Latitude : ";
    cin >> srcLat;

    cout << "Enter Source Longitude : ";
    cin >> srcLon;

    cout << "Enter Destination Latitude : ";
    cin >> destLat;

    cout << "Enter Destination Longitude : ";
    cin >> destLon;

    Location pickup(srcLat, srcLon);
    Location dropoff(destLat, destLon);

    rider->requestRide(pickup, dropoff);

    // ====================================================
    // Distance Calculation
    // ====================================================

    double distance =
        sqrt(
            pow(destLat - srcLat, 2) +
            pow(destLon - srcLon, 2));

    cout << "\nDistance : "
         << fixed
         << setprecision(2)
         << distance
         << "\n";

    // ====================================================
    // Fare Estimation
    // ====================================================

    auto estimatedFares =
        fareManager->estimateFare(distance);

    cout << "\nAvailable Ride Types\n";

    cout << "1. BIKE : "
         << estimatedFares[VehicleType::BIKE]
         << "\n";

    cout << "2. AUTO : "
         << estimatedFares[VehicleType::AUTO]
         << "\n";

    cout << "3. CAB  : "
         << estimatedFares[VehicleType::CAB]
         << "\n";

    // ====================================================
    // Vehicle Selection
    // ====================================================

    int choice;

    cout << "\nSelect Vehicle Type : ";
    cin >> choice;

    VehicleType selectedType;

    switch (choice)
    {
    case 1:
        selectedType = VehicleType::BIKE;
        break;

    case 2:
        selectedType = VehicleType::AUTO;
        break;

    default:
        selectedType = VehicleType::CAB;
    }

    auto request =
        rideManager->createRideRequest(
            rider,
            pickup,
            dropoff,
            selectedType);

    // ====================================================
    // Driver Matching
    // ====================================================

    auto availableDrivers =
        driverManager->getAvailableDrivers();

    auto matchedDriver =
        matchingManager->assignDriver(
            request,
            availableDrivers);

    if (!matchedDriver)
    {
        cout << "\nNo Driver Available\n";
        return 0;
    }

    cout << "\nDriver Assigned : "
         << matchedDriver->getName()
         << "\n";

    // ====================================================
    // Driver Acceptance
    // ====================================================
cin.ignore(numeric_limits<streamsize>::max(), '\n');
    char accept;

    cout << "\nShould Driver Accept? (y/n) : ";
    cin >> accept;

    if (accept != 'y' && accept != 'Y')
    {
        cout << "\nRide Rejected\n";
        return 0;
    }

    auto ride =
        rideManager->createRide(
            request,
            matchedDriver);

    // ====================================================
    // Start Ride
    // ====================================================

    rideManager->startRide(ride);

    cout << "\nPress Enter To Complete Ride...";
    cin.ignore();
    cin.get();

    // ====================================================
    // Complete Ride
    // ====================================================

    rideManager->completeRide(ride);

    // ====================================================
    // Rating Input
    // ====================================================

    int rating;

    cout << "\nRate Ride (1-5) : ";
    cin >> rating;

    cin.ignore();

    string review;

    cout << "Review : ";
    getline(cin, review);

    ratingManager->submitRating(
        ride,
        rating,
        review);

    // ====================================================
    // Summary
    // ====================================================

    cout << "\n================ RIDE SUMMARY ================\n";

    cout << "Rider : "
         << ride->getRider()->getName()
         << "\n";

    cout << "Driver : "
         << ride->getDriver()->getName()
         << "\n";

    cout << "Fare : "
         << ride->getFinalFare().getAmount()
         << "\n";

    cout << "Distance : "
         << ride->getDistance()
         << "\n";

    cout << "Rating : "
         << rating
         << "\n";

    cout << "Review : "
         << review
         << "\n";

    cout << "==============================================\n";

    return 0;
}

/*
int main() {
    std::cout << "============================================================\n";
    std::cout << "         UBER-LIKE RIDE BOOKING SYSTEM SIMULATION\n";
    std::cout << "============================================================\n\n";

    // 1. Initialize Managers (Application layer services)
    auto driverManager = std::make_shared<DriverManager>();
    auto fareManager = std::make_shared<FareManager>();
    
    // Start with Nearest Driver Strategy
    auto matchingManager = std::make_shared<MatchingManager>(std::make_unique<NearestDriverStrategy>());
    auto paymentManager = std::make_shared<PaymentManager>();
    auto ratingManager = std::make_shared<RatingManager>();

    auto rideManager = std::make_shared<RideManager>(
        driverManager,
        fareManager,
        matchingManager,
        paymentManager,
        ratingManager
    );

    // 2. Create Rider (Rider has NO location component, as requested)
    std::cout << "--- Step 1: Create Rider ---\n";
    auto rider = std::make_shared<Rider>(1, "Alice", "alice@example.com", "+1-555-0199");
    std::cout << "Rider Registered: " << rider->getName() << " (" << rider->getEmail() << ")\n\n";

    // 3. Create Vehicles & Register 3 Drivers
    std::cout << "--- Step 2: Register Drivers & Vehicles ---\n";
    Vehicle v1("CAB-1234", VehicleType::CAB);
    Vehicle v2("AUTO-5678", VehicleType::AUTO);
    Vehicle v3("CAB-9999", VehicleType::CAB);

    // Bob is a CAB driver, located at (10.0, 10.0), rating 4.8
    auto driverBob = std::make_shared<Driver>(101, "Bob", "bob@driver.com", "+1-555-0201", "LIC-BOB123", v1, Location(10.0, 10.0), 4.8);
    // Charlie is an AUTO driver, located at (5.0, 5.0), rating 4.5
    auto driverCharlie = std::make_shared<Driver>(102, "Charlie", "charlie@driver.com", "+1-555-0202", "LIC-CHAS56", v2, Location(5.0, 5.0), 4.5);
    // David is a CAB driver, located at (2.0, 2.0) (closest to rider's pickup), rating 4.9
    auto driverDavid = std::make_shared<Driver>(103, "David", "david@driver.com", "+1-555-0203", "LIC-DAV999", v3, Location(2.0, 2.0), 4.9);

    driverManager->registerDriver(driverBob);
    driverManager->registerDriver(driverCharlie);
    driverManager->registerDriver(driverDavid);
    std::cout << "\n";

    // 4. Booking Flow begins: Rider enters Source and Destination coordinates
    std::cout << "--- Step 3: Enter Ride Details ---\n";
    Location pickup(0.0, 0.0);
    Location dropoff(3.0, 4.0);
    rider->requestRide(pickup, dropoff);

    // 5. Calculate distance between pickup and dropoff
    double dist = std::sqrt(std::pow(dropoff.getLatitude() - pickup.getLatitude(), 2) +
                            std::pow(dropoff.getLongitude() - pickup.getLongitude(), 2));
    std::cout << "Calculated Distance: " << dist << " units.\n\n";

    // 6. Display Estimated Fares for all available vehicle options
    std::cout << "--- Step 4: Estimate Fares ---\n";
    auto estimatedFares = fareManager->estimateFare(dist);
    for (const auto& [type, amount] : estimatedFares) {
        std::cout << "  - " << vehicleTypeToString(type) << " Estimated Fare: $" << std::fixed << std::setprecision(2) << amount << "\n";
    }
    std::cout << "\n";

    // 7. Rider selects vehicle type: CAB
    std::cout << "--- Step 5: Select Vehicle & Create RideRequest ---\n";
    VehicleType selectedType = rider->selectVehicleType(VehicleType::CAB);
    auto request = rideManager->createRideRequest(rider, pickup, dropoff, selectedType);
    std::cout << "\n";

    // 8. Find matching driver
    std::cout << "--- Step 6: Match Available Driver ---\n";
    std::cout << "[MatchingManager] Matching using NearestDriverStrategy...\n";
    auto availableDrivers = driverManager->getAvailableDrivers();
    auto matchedDriver = matchingManager->assignDriver(request, availableDrivers);

    if (matchedDriver) {
        std::cout << "Matched Driver: " << matchedDriver->getName() 
                  << " at Location (" << matchedDriver->getLocation().getLatitude() << ", " << matchedDriver->getLocation().getLongitude() << ")"
                  << " with Rating " << matchedDriver->getRating() << "\n";
    }
    std::cout << "\n";

    // 9. Driver Accepts Ride
    std::cout << "--- Step 7: Driver Accepts Ride ---\n";
    bool driverAccepted = matchedDriver->acceptRide();
    std::shared_ptr<Ride> ride = nullptr;
    if (driverAccepted) {
        ride = rideManager->createRide(request, matchedDriver);
    }
    std::cout << "\n";

    if (ride) {
        // 10. Start Ride
        std::cout << "--- Step 8: Start Ride ---\n";
        rideManager->startRide(ride);
        std::cout << "\n";

        // 11. Complete Ride
        std::cout << "--- Step 9: Complete Ride ---\n";
        rideManager->completeRide(ride);
        std::cout << "\n";

        // 12. Submit Rating
        std::cout << "--- Step 10: Submit Rating ---\n";
        rider->rateRide(5, "Excellent service, very fast and clean cab!");
        ratingManager->submitRating(ride, 5, "Excellent service, very fast and clean cab!");
        std::cout << "\n";

        // 13. Print Summary
        std::cout << "============================================================\n";
        std::cout << "                       RIDE SUMMARY                         \n";
        std::cout << "============================================================\n";
        std::cout << "Rider Name        : " << ride->getRider()->getName() << "\n";
        std::cout << "Driver Name       : " << ride->getDriver()->getName() << "\n";
        std::cout << "Vehicle Reg No    : " << ride->getVehicle().getRegistrationNo() << "\n";
        std::cout << "Vehicle Type      : " << vehicleTypeToString(ride->getVehicle().getType()) << "\n";
        std::cout << "Pickup Point      : (" << ride->getSource().getLatitude() << ", " << ride->getSource().getLongitude() << ")\n";
        std::cout << "Dropoff Point     : (" << ride->getDestination().getLatitude() << ", " << ride->getDestination().getLongitude() << ")\n";
        std::cout << "Distance Travelled: " << std::fixed << std::setprecision(2) << ride->getDistance() << " units\n";
        std::cout << "Total Paid        : $" << ride->getFinalFare().getAmount() << "\n";
        if (ride->getPayment()) {
            std::string payStatusStr = (ride->getPayment()->getStatus() == PaymentStatus::SUCCESS) ? "SUCCESS" : "FAILED";
            std::cout << "Payment Status    : " << payStatusStr << "\n";
        }
        if (ride->getRating()) {
            std::cout << "Rating Score      : " << ride->getRating()->getRating() << " / 5 stars\n";
            std::cout << "Review Feedback   : \"" << ride->getRating()->getReview() << "\"\n";
        }
        std::cout << "============================================================\n";
    }

    return 0;
}

i want it to accept input from user rather then hard coded and provide me updated main function
*/
