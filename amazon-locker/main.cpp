#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <ctime>

using namespace std;


enum class SlotSize {
    SMALL,
    MEDIUM,
    LARGE
};

enum class PackageStatus {
    CREATED,
    ASSIGNED_TO_AGENT,
    OUT_FOR_DELIVERY,
    DELIVERED_TO_LOCKER,
    PICKED_UP,
    EXPIRED,
    RETURNED
};


string generateRandomOTP() {
    return "123456"; 
}



class User {
protected:
    string id;
    string name;
    string email;
    string phone;
    string address;

public:
    User(string id,
         string name,
         string email,
         string phone,
         string address)
        : id(id),
          name(name),
          email(email),
          phone(phone),
          address(address) {}

    virtual ~User() = default;

    string getId() const {
        return id;
    }

    string getName() const {
        return name;
    }
};



class Customer : public User {
public:
    Customer(string id,
             string name,
             string email,
             string phone,
             string address)
        : User(id, name, email, phone, address) {}
};



class DeliveryAgent : public User {
public:
    DeliveryAgent(string id,
                  string name,
                  string email,
                  string phone,
                  string address)
        : User(id, name, email, phone, address) {}
};



class LockerSlot;

class Package {
private:
    string packageId;
    SlotSize size;
    PackageStatus status;

    shared_ptr<Customer> customer;
    shared_ptr<DeliveryAgent> agent;

    LockerSlot* assignedSlot;

public:
    Package(string packageId,
            SlotSize size,
            shared_ptr<Customer> customer,
            shared_ptr<DeliveryAgent> agent)
        : packageId(packageId),
          size(size),
          status(PackageStatus::CREATED),
          customer(customer),
          agent(agent),
          assignedSlot(nullptr) {}

    string getPackageId() const {
        return packageId;
    }

    SlotSize getSize() const {
        return size;
    }

    PackageStatus getStatus() const {
        return status;
    }

    void setStatus(PackageStatus status) {
        this->status = status;
    }

    void assignSlot(LockerSlot* slot) {
        assignedSlot = slot;
    }
};



class PickupCode {
private:
    string code;
    time_t expiryTime;
    bool used;

public:
    PickupCode(int validSeconds = 300) {
        code = generateRandomOTP();
        expiryTime = time(nullptr) + validSeconds;
        used = false;
    }

    string getCode() const {
        return code;
    }

    bool validate(string enteredCode) {
        if (used) return false;

        if (time(nullptr) > expiryTime)
            return false;

        if (enteredCode != code)
            return false;

        used = true;
        return true;
    }
};



class LockerSlot {
private:
    string slotId;
    SlotSize size;

    shared_ptr<Package> assignedPackage;

public:
    LockerSlot(string slotId, SlotSize size)
        : slotId(slotId),
          size(size),
          assignedPackage(nullptr) {}

    string getSlotId() const {
        return slotId;
    }

    SlotSize getSize() const {
        return size;
    }

    bool isAvailable() const {
        return assignedPackage == nullptr;
    }

    bool canFit(SlotSize packageSize) {
        return static_cast<int>(size) >= static_cast<int>(packageSize);
    }

    bool assignPackage(shared_ptr<Package> package) {
        if (!isAvailable())
            return false;

        if (!canFit(package->getSize()))
            return false;

        assignedPackage = package;

        package->assignSlot(this);
        package->setStatus(PackageStatus::DELIVERED_TO_LOCKER);

        return true;
    }

    void removePackage() {
        if (assignedPackage != nullptr) {
            assignedPackage->setStatus(PackageStatus::PICKED_UP);
        }

        assignedPackage = nullptr;
    }

    shared_ptr<Package> getAssignedPackage() {
        return assignedPackage;
    }
};



class Locker {
private:
    string lockerId;
    string zipCode;

    vector<shared_ptr<LockerSlot>> slots;

public:
    Locker(string lockerId, string zipCode)
        : lockerId(lockerId),
          zipCode(zipCode) {}

    void addSlot(shared_ptr<LockerSlot> slot) {
        slots.push_back(slot);
    }

    vector<shared_ptr<LockerSlot>>& getSlots() {
        return slots;
    }

    string getLockerId() const {
        return lockerId;
    }
};



class LockerAllocationStrategy {
public:
    virtual shared_ptr<LockerSlot>
    findSlot(shared_ptr<Package> package,
             vector<shared_ptr<Locker>>& lockers) = 0;

    virtual ~LockerAllocationStrategy() = default;
};



class BestFitAllocationStrategy : public LockerAllocationStrategy {
public:
    shared_ptr<LockerSlot>
    findSlot(shared_ptr<Package> package,
             vector<shared_ptr<Locker>>& lockers) override {

        shared_ptr<LockerSlot> bestSlot = nullptr;

        for (auto& locker : lockers) {
            for (auto& slot : locker->getSlots()) {

                if (slot->isAvailable() &&
                    slot->canFit(package->getSize())) {

                    if (bestSlot == nullptr ||
                        static_cast<int>(slot->getSize()) <
                        static_cast<int>(bestSlot->getSize())) {

                        bestSlot = slot;
                    }
                }
            }
        }

        return bestSlot;
    }
};



class OTPService {
public:
    PickupCode createOTP() {
        return PickupCode();
    }

    bool validateOTP(PickupCode& pickupCode,
                     const string& enteredOtp) {

        return pickupCode.validate(enteredOtp);
    }
};



class LockerManager {
private:
    vector<shared_ptr<Locker>> lockers;

    unique_ptr<LockerAllocationStrategy> strategy;

public:
    LockerManager(unique_ptr<LockerAllocationStrategy> strategy)
        : strategy(move(strategy)) {}

    void addLocker(shared_ptr<Locker> locker) {
        lockers.push_back(locker);
    }

    bool assignLocker(shared_ptr<Package> package) {

        auto slot = strategy->findSlot(package, lockers);

        if (slot == nullptr) {
            cout << "No suitable slot found\n";
            return false;
        }

        bool assigned = slot->assignPackage(package);

        if (assigned) {
            cout << "Package assigned to Slot: "
                 << slot->getSlotId() << endl;
        }

        return assigned;
    }

    void releaseLocker(shared_ptr<LockerSlot> slot) {
        slot->removePackage();
    }
};

/*
===========================================================
MAIN
===========================================================
*/

int main() {

    /*
    ==========================
    CREATE USERS
    ==========================
    */

    auto customer = make_shared<Customer>(
        "C1",
        "Rahul",
        "rahul@gmail.com",
        "9999999999",
        "Mumbai");

    auto agent = make_shared<DeliveryAgent>(
        "A1",
        "DeliveryBoy",
        "agent@gmail.com",
        "8888888888",
        "Mumbai");

    /*
    ==========================
    CREATE PACKAGE
    ==========================
    */

    auto package = make_shared<Package>(
        "P1",
        SlotSize::SMALL,
        customer,
        agent);

    /*
    ==========================
    CREATE LOCKER + SLOTS
    ==========================
    */

    auto locker = make_shared<Locker>(
        "L1",
        "400001");

    locker->addSlot(
        make_shared<LockerSlot>(
            "S1",
            SlotSize::SMALL));

    locker->addSlot(
        make_shared<LockerSlot>(
            "S2",
            SlotSize::MEDIUM));

    locker->addSlot(
        make_shared<LockerSlot>(
            "S3",
            SlotSize::LARGE));

    /*
    ==========================
    CREATE MANAGER
    ==========================
    */

    LockerManager manager(
        make_unique<BestFitAllocationStrategy>());

    manager.addLocker(locker);

    /*
    ==========================
    ASSIGN LOCKER
    ==========================
    */

    manager.assignLocker(package);

    /*
    ==========================
    OTP FLOW
    ==========================
    */

    OTPService otpService;

    PickupCode pickupCode = otpService.createOTP();

    cout << "Generated OTP: "
         << pickupCode.getCode() << endl;

    bool valid = otpService.validateOTP(
        pickupCode,
        "123456");

    if (valid) {
        cout << "OTP validated successfully\n";
    }
    else {
        cout << "Invalid OTP\n";
    }

    return 0;
}